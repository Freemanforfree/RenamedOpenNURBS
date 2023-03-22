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

int MYON_ParseLengthExpression( 
  const wchar_t* str,
  int str_count,
  MYON_ParseSettings parse_settings,
  MYON::LengthUnitSystem length_value_unit_system,
  double* length_value,
  MYON_ParseSettings* parse_results,
  MYON::LengthUnitSystem* str_length_unit_system
  )
{
  MYON::LengthUnitSystem us = MYON::LengthUnitSystem::None;

  if ( MYON::LengthUnitSystem::CustomUnits == length_value_unit_system )
  {
    // must use the MYON_UnitSystem override for custom units.
    if ( 0 != str_length_unit_system )
      *str_length_unit_system = us;
    if ( 0 != length_value )
      *length_value = MYON_UNSET_VALUE;
    if ( 0 != parse_results )
      *parse_results = MYON_ParseSettings::FalseSettings;
    return 0; 
  }
  
  double x = MYON_UNSET_VALUE;
  int parsed_element_count = MYON_ParseLengthExpression(
    str,
    str_count,
    parse_settings,
    &x,
    parse_results,
    &us
    );

  if ( parsed_element_count > 0 )
  {
    if (    us != length_value_unit_system 
         && MYON::LengthUnitSystem::None != length_value_unit_system
         && MYON::LengthUnitSystem::None != us
         )
    {
      double s = MYON::UnitScale(us,length_value_unit_system);
      x *= s;
    }
  }

  if ( 0 != str_length_unit_system )
    *str_length_unit_system = us;
  if ( 0 != length_value )
    *length_value = x;

  return parsed_element_count;
}


int MYON_ParseLengthExpression( 
  const wchar_t* str,
  int str_count,
  MYON_ParseSettings parse_settings,
  const MYON_UnitSystem& length_value_unit_system,
  double* length_value,
  MYON_ParseSettings* parse_results,
  MYON::LengthUnitSystem* str_length_unit_system
  )
{
  MYON::LengthUnitSystem parse_unit_system = ((MYON::LengthUnitSystem::CustomUnits != length_value_unit_system.UnitSystem() ) ? length_value_unit_system.UnitSystem() : MYON::LengthUnitSystem::None);
  MYON::LengthUnitSystem str_us = MYON::LengthUnitSystem::None;
  double x = MYON_UNSET_VALUE;
  int parsed_element_count = MYON_ParseLengthExpression(
    str,
    str_count,
    parse_settings,
    parse_unit_system,
    &x,
    parse_results,
    &str_us
    );

  if ( parsed_element_count > 0 
       && MYON::LengthUnitSystem::CustomUnits == length_value_unit_system.UnitSystem()
     )
  {
    if ( MYON::LengthUnitSystem::None == str_us )
    {
      const MYON_wString custom_unit_name(length_value_unit_system.UnitSystemName());
      if ( custom_unit_name.IsNotEmpty() )
      {
        const wchar_t* sCustomUnitName = static_cast<const wchar_t*>(custom_unit_name);
        const int imax = ( -1 == str_count ? 2147483647 : str_count) - parsed_element_count;
        for ( int i = 0; i < imax; i++ )
        {
          if ( 0 == sCustomUnitName[i] )
          {
            str_us = MYON::LengthUnitSystem::CustomUnits;
            parsed_element_count += i;
            break;
          }
          if ( sCustomUnitName[i] != str[parsed_element_count + i] )
            break;
        }
      }
    }
    double s = MYON::UnitScale(str_us,length_value_unit_system);
    x *= s;
  }

  if ( 0 != str_length_unit_system )
    *str_length_unit_system = str_us;
  if ( 0 != length_value )
    *length_value = x;

  return parsed_element_count;
}

int MYON_ParseLengthExpression( 
  const wchar_t* str,
  int str_count,
  MYON::AngleUnitSystem default_str_angle_unit_system,
  MYON::LengthUnitSystem length_value_unit_system,
  double* length_value
  )
{
  MYON_ParseSettings ps(MYON_ParseSettings::DefaultSettings);
  ps.SetDefaultAngleUnitSystem(default_str_angle_unit_system);

  int parsed_element_count = MYON_ParseLengthExpression(
    str,
    str_count,
    ps,
    length_value_unit_system,
    length_value,
    nullptr,
    nullptr
    );

  return parsed_element_count;
}

int MYON_ParseLengthExpression( 
  const wchar_t* str,
  int str_count,
  MYON::AngleUnitSystem default_str_angle_unit_system,
  const MYON_UnitSystem& length_value_unit_system,
  double* length_value
  )
{
  MYON_ParseSettings ps(MYON_ParseSettings::DefaultSettings);
  ps.SetDefaultAngleUnitSystem(default_str_angle_unit_system);

  int parsed_element_count = MYON_ParseLengthExpression(
    str,
    str_count,
    ps,
    length_value_unit_system,
    length_value,
    nullptr,
    nullptr
    );

  return parsed_element_count;
}


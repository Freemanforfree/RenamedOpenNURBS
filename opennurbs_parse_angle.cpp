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

int MYON_ParseAngleExpression( 
  const wchar_t* str,
  int str_count,
  MYON_ParseSettings parse_settings,
  const MYON::AngleUnitSystem angle_value_unit_system,
  double* angle_value,
  MYON_ParseSettings* parse_results,
  MYON::AngleUnitSystem* str_angle_unit_system
  )
{
  MYON::AngleUnitSystem parsed_angle_us = MYON::AngleUnitSystem::None;

  double x = MYON_UNSET_VALUE;
  int parsed_element_count = MYON_ParseAngleExpression(
    str,
    str_count,
    parse_settings,
    &x,
    parse_results,
    &parsed_angle_us
    );

  if ( parsed_element_count > 0 )
  {
    if (    parsed_angle_us != angle_value_unit_system 
         && MYON::AngleUnitSystem::None != angle_value_unit_system
         && MYON::AngleUnitSystem::None != parsed_angle_us
         )
    {
      double s = MYON::AngleUnitScale(parsed_angle_us,angle_value_unit_system);
      x *= s;
    }
  }

  if ( 0 != str_angle_unit_system )
    *str_angle_unit_system = parsed_angle_us;
  if ( 0 != angle_value )
    *angle_value = x;

  return parsed_element_count;
}

int MYON_ParseAngleRadiansExpression( 
  const wchar_t* str,
  int str_count,
  MYON::AngleUnitSystem default_str_angle_unit_system,
  double* angle_value_in_radians
  )
{
  MYON_ParseSettings ps(MYON_ParseSettings::DefaultSettings);
  ps.SetDefaultAngleUnitSystem(default_str_angle_unit_system);

  int parsed_element_count = MYON_ParseAngleExpression(
    str,
    str_count,
    ps,
    MYON::AngleUnitSystem::Radians, // specify radians as the unit system for the returned value
    angle_value_in_radians,
    nullptr,
    nullptr
    );

  return parsed_element_count;
}

int MYON_ParseAngleArcDegreesExpression( 
  const wchar_t* str,
  int str_count,
  MYON::AngleUnitSystem default_str_angle_unit_system,
  double* angle_value_in_arc_degrees
  )
{
  MYON_ParseSettings ps(MYON_ParseSettings::DefaultSettings);
  ps.SetDefaultAngleUnitSystem(default_str_angle_unit_system);

  int parsed_element_count = MYON_ParseAngleExpression(
    str,
    str_count,
    ps,
    MYON::AngleUnitSystem::Degrees, // specify arc_degrees as the unit system for the returned value
    angle_value_in_arc_degrees,
    nullptr,
    nullptr
    );

  return parsed_element_count;
}

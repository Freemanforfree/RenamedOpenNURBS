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

// MYON_Table class
#ifndef OPENNURBS_NUMBERFORMAT_H_INCLUDED
#define OPENNURBS_NUMBERFORMAT_H_INCLUDED

class MYON_NumberFormatter
{
  MYON_NumberFormatter();
public:
  static bool bFormatIsAccurate;
  
  static void Fraction(
    double value,
    int& wholenumber,
    int& numerator,
    int& denominator,
    int precision);

  static double RoundOff(
    double number,
    double round_off);

  static void SuppressZeros(
    MYON_wString& dist,
    MYON_DimStyle::suppress_zero sz);

  // When FormatNumber() or FormatLength() is called with 
  // output_lengthformat == MYON_DimStyle::OBSOLETE_length_format::FeetInches
  // distance must be in decimal feet units to get the right answer.
  static bool FormatNumber(
    double distance,
    MYON_DimStyle::OBSOLETE_length_format output_lengthformat, // dec, frac, ft-in
    double round_off,
    int resolution,
    MYON_DimStyle::suppress_zero zero_suppress,
    bool bracket_fractions,
    MYON_wString& output);

  // When FormatNumber() or FormatLength() is called with 
  // output_lengthformat == MYON_DimStyle::LengthDisplay::FeetAndInches
  // distance must be in decimal feet units to get the right answer.
  static bool FormatLength(
    double distance,
    MYON_DimStyle::LengthDisplay output_lengthdisplay,
    double round_off,
    int resolution,
    MYON_DimStyle::suppress_zero zero_suppress,
    bool bracket_fractions,
    MYON_wString& output);

  static bool FormatAngleStringDMS(
    double angle_radians,
    int resolution,
    MYON_wString& formatted_string);

  static bool FormatAngleStringDMS(
    double angle_degrees,
    MYON_wString& formatted_string);

  static bool FormatAngleStringDecimal(
    double angle_radians,
    int resolution,
    double roundoff,
    MYON_DimStyle::suppress_zero zero_suppression,
    MYON_wString& formatted_string);


};

#endif


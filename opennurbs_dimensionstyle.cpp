//
// Copyright (c) 1993-2022 Robert McNeel & Associates. All rights reved.
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

// obsolete V5 dimension style
#include "opennurbs_internal_V5_dimstyle.h"

#pragma region DimStyleContext

MYON_DimStyleContext::~MYON_DimStyleContext()
{};

const MYON_DimStyle& MYON_DimStyleContext::CurrentDimStyle() const
{
  const MYON::LengthUnitSystem model_unit_system = ModelUnitSystem();

  if (MYON::IsUnitedStatesCustomaryLengthUnit(model_unit_system))
  {
    if ( MYON::LengthUnitSystem::Feet == model_unit_system )
      return MYON_DimStyle::DefaultFootInchArchitecture;
    return MYON_DimStyle::DefaultInchDecimal;
  }

  if (MYON::UnitScale(model_unit_system, MYON::LengthUnitSystem::Meters) >= 1.0)
    return MYON_DimStyle::DefaultMillimeterLarge;

  return MYON_DimStyle::DefaultMillimeterSmall;
}

const MYON_DimStyle* MYON_DimStyleContext::DimStyleFromId(
  MYON_UUID id,
  const MYON_DimStyle* not_found_result
) const
{
  const MYON_DimStyle& system_dim_style = MYON_DimStyle::SystemDimstyleFromId(id);
  if (&MYON_DimStyle::Unset != &system_dim_style)
    return &system_dim_style;
  return not_found_result;
}

const MYON_DimStyle* MYON_DimStyleContext::DimStyleFromName(
  const MYON_NameHash& name_hash,
  const MYON_DimStyle* not_found_result
) const
{
  const MYON_DimStyle& system_dim_style = MYON_DimStyle::SystemDimstyleFromName(name_hash);
  if (&MYON_DimStyle::Unset != &system_dim_style)
    return &system_dim_style;
  return not_found_result;
}

const MYON_DimStyle* MYON_DimStyleContext::DimStyleFromContentHash(
  const MYON_SHA1_Hash& content_hash,
  const MYON_DimStyle* not_found_result
) const
{
  const MYON_DimStyle& system_dim_style = MYON_DimStyle::SystemDimstyleFromContentHash(content_hash);
  if (&MYON_DimStyle::Unset != &system_dim_style)
    return &system_dim_style;
  return not_found_result;
}

const MYON_DimStyle* MYON_DimStyleContext::DimStyleFromFont(
  const MYON_Font& font,
  double model_space_text_scale,
  double text_height,
  MYON::LengthUnitSystem text_height_unit_system,
  bool bReturnClosestMatch,
  const MYON_DimStyle* not_found_result
) const
{
  const MYON_DimStyle& current_dimstyle = CurrentDimStyle();
  if (
    &MYON_DimStyle::Unset != &current_dimstyle
    && current_dimstyle.Font().ManagedFontSerialNumber() == font.ManagedFontSerialNumber()
    )
  {
    return &current_dimstyle;
  }
  return not_found_result;
}

MYON::LengthUnitSystem MYON_DimStyleContext::ModelUnitSystem() const
{
  return m_unit_system;
}

MYON__UINT64 MYON_DimStyleContext::ModelSerialNumber() const
{
  return m_model_serial_number;
}

bool MYON_DimStyleContext::AddDimStyle(
  const MYON_DimStyle& dim_style,
  bool bResolveNameAndIdConflicts
)
{
  return false;
}

bool MYON_DimStyleContext::ModifyDimStyle(
  MYON_UUID model_dim_style_id,
  const MYON_DimStyle& dim_style
)
{
  return false;
}

const MYON_DimStyle* MYON_DimStyleContext::FirstDimStyle(
  bool bIncludeSystemDimStyles,
  bool bIncludeDeletedDimStyles
) const
{
  return nullptr;
}


const MYON_DimStyle* MYON_DimStyleContext::NextDimStyle(
  MYON_UUID id,
  bool bIncludeSystemDimStyles,
  bool bIncludeDeletedDimStyles
) const
{
  return nullptr;
}

const MYON_DimStyle* MYON_DimStyleContext::PrevDimStyle(
  MYON_UUID id,
  bool bIncludeSystemDimStyles,
  bool bIncludeDeletedDimStyles
) const
{
  return nullptr;
}


#pragma endregion DimStyleContext


#pragma region Arrowheads

bool MYON_Arrowhead::operator!=(const MYON_Arrowhead& other) const
{
  return !operator==(other);
}

bool MYON_Arrowhead::operator==(const MYON_Arrowhead& other) const
{
  if (m_arrowhead_type == other.m_arrowhead_type)
  {
    if (m_arrowhead_type == arrow_type::UserBlock)
      return (m_arrow_block_id == other.m_arrow_block_id);
    else
      return true;
  }
  else
    return false;
}

MYON_Arrowhead::arrow_type MYON_Arrowhead::ArrowTypeFromUnsigned(
  unsigned int type_as_unsigned
  )
{
  switch (type_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Arrowhead::arrow_type::None);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Arrowhead::arrow_type::UserBlock);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Arrowhead::arrow_type::SolidTriangle);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Arrowhead::arrow_type::Dot);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Arrowhead::arrow_type::Tick);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Arrowhead::arrow_type::ShortTriangle);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Arrowhead::arrow_type::OpenArrow);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Arrowhead::arrow_type::Rectangle);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Arrowhead::arrow_type::LongTriangle);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Arrowhead::arrow_type::LongerTriangle);
  }
  MYON_ERROR("Invalid type_as_unsigned parameter.");
  return (MYON_Arrowhead::arrow_type::None);
}

MYON_DimStyle::LengthDisplay MYON_DimStyle::LengthDisplayFromUnsigned(
  unsigned int length_display_as_unsigned
)
{
  switch (length_display_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::LengthDisplay::ModelUnits);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::LengthDisplay::Millmeters);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::LengthDisplay::Centimeters);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::LengthDisplay::Meters);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::LengthDisplay::Kilometers);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::LengthDisplay::InchesDecimal);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::LengthDisplay::InchesFractional);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::LengthDisplay::FeetDecimal);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::LengthDisplay::FeetAndInches);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::LengthDisplay::Miles);
  }
  MYON_ERROR("Invalid length_display_as_unsigned parameter.");
  return (MYON_DimStyle::LengthDisplay::ModelUnits);
}


bool MYON_DimStyle::LengthDisplayIsDecimal(
  MYON_DimStyle::LengthDisplay dimension_length_display
)
{
  return
    MYON_DimStyle::LengthDisplay::InchesFractional != dimension_length_display
    && MYON_DimStyle::LengthDisplay::FeetAndInches != dimension_length_display;
}

MYON::LengthUnitSystem MYON_DimStyle::LengthUnitSystemFromLengthDisplay(
  MYON_DimStyle::LengthDisplay dimension_length_display
)
{
  switch (dimension_length_display)
  {
  case LengthDisplay::ModelUnits:
    return MYON::LengthUnitSystem::None;
  case LengthDisplay::Millmeters:
    return MYON::LengthUnitSystem::Millimeters;
  case LengthDisplay::Centimeters:
    return MYON::LengthUnitSystem::Centimeters;
  case LengthDisplay::Meters:
    return MYON::LengthUnitSystem::Meters;
  case LengthDisplay::Kilometers:
    return MYON::LengthUnitSystem::Kilometers;
  case LengthDisplay::InchesDecimal:
    return MYON::LengthUnitSystem::Inches;
  case LengthDisplay::InchesFractional:
    return MYON::LengthUnitSystem::Inches;
  case LengthDisplay::FeetDecimal:
    return MYON::LengthUnitSystem::Feet;
  case LengthDisplay::FeetAndInches:
    return MYON::LengthUnitSystem::Feet;
  case LengthDisplay::Miles:
    return MYON::LengthUnitSystem::Miles;
  }
  return MYON::LengthUnitSystem::None;
}

MYON_DimStyle::tolerance_format MYON_DimStyle::ToleranceFormatFromUnsigned(
  unsigned int format_as_unsigned
  )
{
  switch (format_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::tolerance_format::None);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::tolerance_format::Symmetrical);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::tolerance_format::Deviation);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::tolerance_format::Limits);
  }
  MYON_ERROR("invalid format_as_unsigned parameter.");
  return (MYON_DimStyle::tolerance_format::None);
}

MYON_DimStyle::ContentAngleStyle MYON_DimStyle::ContentAngleStyleFromUnsigned(
  unsigned int alignment_as_unsigned
  )
{
  switch (alignment_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::ContentAngleStyle::Horizontal);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::ContentAngleStyle::Aligned);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::ContentAngleStyle::Rotated);
  }
  MYON_ERROR("invalid alignment_as_unsigned parameter.");
  return (MYON_DimStyle::ContentAngleStyle::Horizontal);
}

MYON_DimStyle::leader_curve_type MYON_DimStyle::LeaderCurveTypeFromUnsigned(
  unsigned int type_as_unsigned
  )
{
  switch (type_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::leader_curve_type::None);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::leader_curve_type::Polyline);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::leader_curve_type::Spline);
  }
  MYON_ERROR("invalid type_as_unsigned parameter.");
  return (MYON_DimStyle::leader_curve_type::Polyline);
}

MYON_DimStyle::field MYON_DimStyle::FieldFromUnsigned(
  unsigned int field_as_unsigned
  )
{
  switch (field_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::Unset);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::Name);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::Index);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::ExtensionLineExtension);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::ExtensionLineOffset);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::Arrowsize);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::LeaderArrowsize);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::Centermark);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::TextGap);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::TextHeight);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::DimTextLocation);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::MaskFrameType);
    // OBSOLETE // //MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::OBSOLETE_LengthFormat_);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::LengthResolution);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::AngleFormat);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::AngleResolution);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::Font);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::LengthFactor);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::Alternate);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::AlternateLengthFactor);
    // OBSOLETE // //MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::OBSOLETE_AlternateLengthFormat_);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::AlternateLengthResolution);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::Prefix);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::Suffix);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::AlternatePrefix);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::AlternateSuffix);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::DimensionLineExtension);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::SuppressExtension1);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::SuppressExtension2);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::ExtLineColorSource);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::DimLineColorSource);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::ArrowColorSource);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::TextColorSource);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::ExtLineColor);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::DimLineColor);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::ArrowColor);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::TextColor);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::ExtLinePlotColorSource);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::DimLinePlotColorSource);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::ArrowPlotColorSource);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::TextPlotColorSource);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::ExtLinePlotColor);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::DimLinePlotColor);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::ArrowPlotColor);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::TextPlotColor);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::ExtLinePlotWeightSource);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::DimLinePlotWeightSource);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::ExtLinePlotWeight_mm);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::DimLinePlotWeight_mm);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::ToleranceFormat);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::ToleranceResolution);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::ToleranceUpperValue);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::ToleranceLowerValue);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::AltToleranceResolution);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::ToleranceHeightScale);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::BaselineSpacing);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::DrawMask);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::MaskColorSource);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::MaskColor);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::MaskBorder);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::DimensionScale);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::DimscaleSource);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::FixedExtensionLength);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::FixedExtensionOn);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::TextRotation);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::SuppressArrow1);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::SuppressArrow2);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::TextmoveLeader);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::ArclengthSymbol);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::StackTextheightScale);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::StackFormat);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::AltRound);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::Round);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::AngularRound);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::AltZeroSuppress);
    // OBSOLETE // MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::ToleranceZeroSuppress);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::AngleZeroSuppress);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::ZeroSuppress);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::AltBelow);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::ArrowType1);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::ArrowType2);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::LeaderArrowType);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::ArrowBlockId1);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::ArrowBlockId2);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::LeaderArrowBlock);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::DimRadialTextLocation);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::TextVerticalAlignment);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::LeaderTextVerticalAlignment);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::LeaderContentAngleStyle);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::LeaderCurveType);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::LeaderContentAngle);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::LeaderHasLanding);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::LeaderLandingLength);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::MaskFlags);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::CentermarkStyle);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::TextHorizontalAlignment);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::LeaderTextHorizontalAlignment);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::DrawForward);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::SignedOrdinate);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::UnitSystem);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::TextMask);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::TextOrientation);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::LeaderTextOrientation);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::DimTextOrientation);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::DimRadialTextOrientation);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::DimTextAngleStyle);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::DimRadialTextAngleStyle);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::TextUnderlined);
    // OBSOLETE // //MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::OBSOLETE_DimensionUnitSystem_);
    // OBSOLETE // //MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::OBSOLETE_AlternateDimensionUnitSystem_);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::DimensionLengthDisplay);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::AlternateDimensionLengthDisplay);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::ForceDimLine);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::TextFit);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::ArrowFit);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::field::DecimalSeparator);
  }
  if (field_as_unsigned > static_cast<unsigned int>(MYON_DimStyle::field::AlternateDimensionLengthDisplay))
  {
    MYON_ERROR("invalid field_as_unsigned parameter.");
  }
  return (MYON_DimStyle::field::Unset);
}

MYON_DimStyle::angle_format MYON_DimStyle::AngleFormatFromUnsigned(
  unsigned int format_as_unsigned
  )
{
  switch (format_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::angle_format::DecimalDegrees);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::angle_format::DegMinSec);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::angle_format::Radians);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::angle_format::Grads);
  }
  MYON_ERROR("invalid format_as_unsigned parameter.");
  return (MYON_DimStyle::angle_format::DecimalDegrees);
}

MYON_DimStyle::stack_format MYON_DimStyle::StackFormatFromUnsigned(
  unsigned int format_as_unsigned
  )
{
  switch (format_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::stack_format::None);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::stack_format::StackHorizontal);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::stack_format::StackDiagonal);
  }
  MYON_ERROR("invalid format_as_unsigned parameter.");
  return (MYON_DimStyle::stack_format::None);
}


MYON_DimStyle::centermark_style MYON_DimStyle::CentermarkStyleFromUnsigned(
  unsigned int centermark_as_unsigned
)
{
  switch (centermark_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::centermark_style::None);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::centermark_style::Mark);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::centermark_style::MarkAndLines);
  }
  MYON_ERROR("invalid centermark_as_unsigned parameter.");
  return (MYON_DimStyle::centermark_style::None);
}

MYON_DimStyle::OBSOLETE_length_format MYON_DimStyle::OBSOLETE_LengthFormatFromUnsigned(
  unsigned int format_as_unsigned
  )
{
  switch (format_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::OBSOLETE_length_format::Decimal);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::OBSOLETE_length_format::Fractional);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::OBSOLETE_length_format::FeetInches);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::OBSOLETE_length_format::FeetDecimalInches);
  }
  MYON_ERROR("invalid format_as_unsigned parameter.");
  return (MYON_DimStyle::OBSOLETE_length_format::Decimal);
}

MYON_DimStyle::arrow_fit MYON_DimStyle::ArrowFitFromUnsigned(
  unsigned int arrow_fit_as_unsigned
)
{
  switch (arrow_fit_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::arrow_fit::Auto);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::arrow_fit::ArrowsInside);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::arrow_fit::ArrowsOutside);
  }
  MYON_ERROR("invalid arrow_fit_as_unsigned parameter.");
  return (MYON_DimStyle::arrow_fit::Auto);
}

MYON_DimStyle::text_fit MYON_DimStyle::TextFitFromUnsigned(
  unsigned int text_fit_as_unsigned
)
{
  switch (text_fit_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::text_fit::Auto);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::text_fit::TextInside);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::text_fit::TextRight);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::text_fit::TextLeft);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::text_fit::TextHintRight);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::text_fit::TextHintLeft);
  }
  MYON_ERROR("invalid text_fit_as_unsigned parameter.");
  return (MYON_DimStyle::text_fit::Auto);
}


MYON_DimStyle::OBSOLETE_length_format MYON_DimStyle::OBSOLETE_LengthFormatFromLengthDisplay(
  MYON_DimStyle::LengthDisplay dimension_length_display,
  MYON::LengthUnitSystem model_unit_system
)
{
  if (MYON_DimStyle::LengthDisplay::FeetAndInches == dimension_length_display)
    return MYON_DimStyle::OBSOLETE_length_format::FeetInches;

  if (MYON_DimStyle::LengthDisplay::InchesFractional == dimension_length_display
    && MYON::LengthUnitSystem::Inches == model_unit_system
    )
    return MYON_DimStyle::OBSOLETE_length_format::Fractional;

  return MYON_DimStyle::OBSOLETE_length_format::Decimal;
}

MYON_DimStyle::OBSOLETE_length_format MYON_DimStyle::OBSOLETE_LengthFormatFromLengthDisplay(
  MYON_DimStyle::LengthDisplay dimension_length_display,
  unsigned int model_serial_number
)
{
  return MYON_DimStyle::OBSOLETE_LengthFormatFromLengthDisplay(
    dimension_length_display,
    MYON::ModelLengthUnitSystem(model_serial_number)
  );
}


MYON_DimStyle::TextLocation MYON_DimStyle::TextLocationFromUnsigned(
  unsigned int dim_text_location_as_unsigned
  )
{
  switch (dim_text_location_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::TextLocation::AboveDimLine);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::TextLocation::InDimLine);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::TextLocation::BelowDimLine);
  }
  MYON_ERROR("invalid dim_text_location_as_unsigned parameter.");
  return (MYON_DimStyle::TextLocation::AboveDimLine);
}

MYON_DimStyle::suppress_zero MYON_DimStyle::ZeroSuppressFromUnsigned(
  unsigned int suppress_ero_as_unsigned
)
{
  switch (suppress_ero_as_unsigned)
  {
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::suppress_zero::None);
    break;
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::suppress_zero::SuppressLeading);
    break;
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::suppress_zero::SuppressTrailing);
    break;
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::suppress_zero::SuppressLeadingAndTrailing);
    break;
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::suppress_zero::SuppressZeroFeet);
    break;
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::suppress_zero::SuppressZeroInches);
    break;
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_DimStyle::suppress_zero::SuppressZeroFeetAndZeroInches);
    break;
  }
  MYON_ERROR("invalid suppress_zero_as_unsigned parameter.");
  return MYON_DimStyle::suppress_zero::None;
}

MYON_DimStyle::LengthDisplay MYON_DimStyle::LengthDisplayFromUnitsAndFormat(
  MYON::LengthUnitSystem dimunits, 
  MYON_DimStyle::OBSOLETE_length_format lengthformat)
{
  MYON_DimStyle::LengthDisplay display = MYON_DimStyle::LengthDisplay::ModelUnits;

  if (
    MYON_DimStyle::OBSOLETE_length_format::FeetInches == lengthformat
    || MYON_DimStyle::OBSOLETE_length_format::FeetDecimalInches == lengthformat
    )
  {
    display = MYON_DimStyle::LengthDisplay::FeetAndInches;
  }
  else
  {
    switch (dimunits)
    {
    case MYON::LengthUnitSystem::Millimeters:
      display = MYON_DimStyle::LengthDisplay::Millmeters;
      break;
    case MYON::LengthUnitSystem::Centimeters:
      display = MYON_DimStyle::LengthDisplay::Centimeters;
      break;
    case MYON::LengthUnitSystem::Meters:
      display = MYON_DimStyle::LengthDisplay::Meters;
      break;
    case MYON::LengthUnitSystem::Kilometers:
      display = MYON_DimStyle::LengthDisplay::Kilometers;
      break;
    case MYON::LengthUnitSystem::Miles:
      display = MYON_DimStyle::LengthDisplay::Miles;
      break;
    case MYON::LengthUnitSystem::Inches:
      if (MYON_DimStyle::OBSOLETE_length_format::Fractional == lengthformat)
        display = MYON_DimStyle::LengthDisplay::InchesFractional;
      else
        display = MYON_DimStyle::LengthDisplay::InchesDecimal;
      break;
    case MYON::LengthUnitSystem::Feet:
      display = MYON_DimStyle::LengthDisplay::FeetDecimal;
      break;
    default:
      break;
    }
  }
  return display;
}

MYON_DimStyle::MYON_DimStyle::LengthDisplay MYON_DimStyle::DimensionLengthDisplay() const
{
  return m_dimension_length_display;
}

MYON_DimStyle::MYON_DimStyle::LengthDisplay MYON_DimStyle::AlternateDimensionLengthDisplay() const
{
  return m_alternate_dimension_length_display;
}

void MYON_DimStyle::SetDimensionLengthDisplay(MYON_DimStyle::MYON_DimStyle::LengthDisplay length_display)
{
  if (m_dimension_length_display != length_display)
  {
    m_dimension_length_display = length_display;
    Internal_ContentChange();
    if (!MYON_DimStyle::ZeroSuppressMatchesLengthDisplay(ZeroSuppress(), length_display))
      SetZeroSuppress(suppress_zero::None);
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::DimensionLengthDisplay);
}

void MYON_DimStyle::SetAlternateDimensionLengthDisplay(MYON_DimStyle::LengthDisplay length_display)
{
  if (m_alternate_dimension_length_display != length_display)
  {
    m_alternate_dimension_length_display = length_display;
    Internal_ContentChange();
    if (!MYON_DimStyle::ZeroSuppressMatchesLengthDisplay(AlternateZeroSuppress(), length_display))
      SetAlternateZeroSuppress(suppress_zero::None);
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::AlternateDimensionLengthDisplay);
}

static MYON::LengthUnitSystem Internal_DimensionLengthDisplay(
  const MYON_ModelComponent& model_component,
  MYON_DimStyle::LengthDisplay dim_length_display,
  unsigned int model_sn
)
{
  MYON::LengthUnitSystem us = MYON_DimStyle::LengthUnitSystemFromLengthDisplay(dim_length_display);
  if (MYON::LengthUnitSystem::None == us)
  {
    if (MYON_UNSET_UINT_INDEX == model_sn)
      model_sn = model_component.ModelSerialNumber();
    if ( model_sn > 0)
      us = MYON::ModelLengthUnitSystem(model_sn);
  }
  return us;
}

MYON::LengthUnitSystem MYON_DimStyle::DimensionLengthDisplayUnit(
  unsigned int model_sn
) const
{
  return Internal_DimensionLengthDisplay(*this, DimensionLengthDisplay(), model_sn);
}

MYON::LengthUnitSystem MYON_DimStyle::AlternateDimensionLengthDisplayUnit(
  unsigned int model_sn
) const
{
  return Internal_DimensionLengthDisplay(*this, AlternateDimensionLengthDisplay(), model_sn);
}

MYON_Arrowhead::arrow_type MYON_Arrowhead::ArrowheadType() const
{
  return m_arrowhead_type;
}

void MYON_Arrowhead::SetArrowheadType(MYON_Arrowhead::arrow_type type)
{
  m_arrowhead_type = type;
}

MYON_UUID MYON_Arrowhead::ArrowBlockId() const
{
  return m_arrow_block_id;
}

void MYON_Arrowhead::SetArrowBlockId(MYON_UUID id)
{
  m_arrow_block_id = id;
}

MYON_Arrowhead::arrow_type MYON_Arrowhead::DefaultArrowType()
{
  return MYON_Arrowhead::arrow_type::SolidTriangle;
}

MYON__UINT32 MYON_Arrowhead::GetPoints(arrow_type type, const double*& points)
{
  // Polygon points for built-in arrowheads
  // SolidTriangle
  static double a2[] = { 0.0, 0.0, -1.0, 0.25, -1.0, -0.25 };
  // Dot
  static double a3[] = { 0.5, 0.0, 0.483, 0.129, 0.433, 0.25, 0.353, 0.353, 0.25, 0.433, 0.129, 0.483,
    0.0, 0.5, -0.129, 0.483, -0.25, 0.433, -0.353, 0.353, -0.433, 0.25, -0.483, 0.129,
    -0.5, 0.0, -0.483, -0.129, -0.433, -0.25, -0.353, -0.353, -0.25, -0.433, -0.129, -0.483,
    0.0, -0.5, 0.129, -0.483, 0.25, -0.433, 0.353, -0.353, 0.433, -0.25, 0.483, -0.129 };
  // Tick
  static double a4[] = { -0.46, -0.54, 0.54, 0.46, 0.46, 0.54, -0.54, -0.46 };
  // ShortTriangle
  static double a5[] = { 0.0, 0.0, -0.5, 0.5, -0.5, -0.5 };
  // Arrow
  static double a6[] = { 0.0, 0.0, -0.707, 0.707, -0.777, 0.636, -0.141, 0.0, -0.777, -0.636, -0.707, -0.707 };
  // Rectangle
  static double a7[] = { 0.0, 0.0, -1.0, 0.0, -1.0, 0.2, 0.0, 0.2 };
  // LongTriangle
  static double a8[] = { 0.0, 0.0, -1.0, 0.125, -1.0, -0.125 };
  // LongerTriangle
  static double a9[] = { 0.0, 0.0, -1.0, 0.0833, -1.0, -0.0833 };

  static double* arrow_points[] = { 0, 0, a2, a3, a4, a5, a6, a7, a8, a9 };
  static unsigned int c[] = {
    0,  // NoArrow
    0,  // User
    sizeof(a2) / sizeof(a2[0]) / 2,
    sizeof(a3) / sizeof(a3[0]) / 2,
    sizeof(a4) / sizeof(a4[0]) / 2,
    sizeof(a5) / sizeof(a5[0]) / 2,
    sizeof(a6) / sizeof(a6[0]) / 2,
    sizeof(a7) / sizeof(a7[0]) / 2,
    sizeof(a8) / sizeof(a8[0]) / 2,
    sizeof(a9) / sizeof(a9[0]) / 2
  };
  MYON__UINT32 pointcount = 0;
  if (arrow_type::UserBlock < type && arrow_type::LongerTriangle >= type)
  {
    pointcount = c[(int)type];
    points = arrow_points[(int)type];
  }
  return pointcount;
}

MYON__UINT32 MYON_Arrowhead::GetPoints(arrow_type type, MYON_2dPointArray& points)
{
  const double* pts = nullptr;
  unsigned int pcount = MYON_Arrowhead::GetPoints(type, pts);
  if (0 < pcount)
  {
    points.Empty();
    points.Reserve(pcount);
    points.Append(pcount, (MYON_2dPoint*)pts);
  }
  return pcount;
}

bool MYON_Arrowhead::GetArrowheadBoundingBox(
  MYON_Arrowhead::arrow_type arrowtype,
  MYON_UUID arrow_block_id,
  MYON_Xform xform,
  MYON_BoundingBox& bbox,
  bool grow)
{
  bool rc = false;

  if (!grow)
    bbox.Destroy();
  if (MYON_Arrowhead::arrow_type::UserBlock != arrowtype && MYON_Arrowhead::arrow_type::None != arrowtype)
  {
    // Draw polygon from built-in shapes
    MYON_2dPointArray points2d;
    int count = MYON_Arrowhead::GetPoints(arrowtype, points2d);
    if (2 < count)
    {
      MYON_3dPointArray points3d(count);
      for (int i = 0; i < count; i++)
      {
        MYON_3dPoint& p = points3d.AppendNew();
        p = points2d[i];
        p.Transform(xform);
        bbox.Set(p, grow);
        grow = true;
      }
      rc = true;
    }
  }
  else if (MYON_Arrowhead::arrow_type::UserBlock == arrowtype)
  {
    MYON_3dPoint points[4] = {
      MYON_3dPoint( 0.0, -0.5, 0.0 ),
      MYON_3dPoint( 1.0, -0.5, 0.0 ),
      MYON_3dPoint( 1.0, 0.5, 0.0 ),
      MYON_3dPoint( 0.0, 0.5, 0.0 ) };

    for (int i = 0; i < 4; i++)
    {
      points[i].Transform(xform);
      bbox.Set(points[i], grow);
      grow = true;
    }
    rc = true;
  }
  return rc;
}


#pragma endregion Arrowheads

#pragma region TextMask

const MYON_SHA1_Hash& MYON_TextMask::ContentHash() const
{
  if (m_content_hash.IsZeroDigest())
  {
    MYON_SHA1 sha1;
    unsigned int u[3] = {
      (m_bDrawMask ? 1U : 0U),
      (unsigned int)(static_cast<unsigned char>(m_mask_type)),
      (unsigned int)(static_cast<unsigned char>(m_mask_frame))
    };
    sha1.AccumulateUnsigned32(u[0]);
    sha1.AccumulateUnsigned32(u[1]);
    sha1.AccumulateUnsigned32(u[2]);
    sha1.AccumulateUnsigned32(m_mask_color);
    sha1.AccumulateDouble(m_mask_border);
    m_content_hash = sha1.Hash();
  }
  return m_content_hash;
}

int MYON_TextMask::Compare(
  const MYON_TextMask& lhs,
  const MYON_TextMask& rhs
  )
{
  return MYON_SHA1_Hash::Compare(lhs.ContentHash(), rhs.ContentHash());
}

bool operator==(
  const MYON_TextMask& lhs,
  const MYON_TextMask& rhs
  )
{
  return 0 == MYON_TextMask::Compare(lhs, rhs);
}

bool operator!=(
  const MYON_TextMask& lhs,
  const MYON_TextMask& rhs
  )
{
  return 0 != MYON_TextMask::Compare(lhs, rhs);
}

bool MYON_TextMask::DrawTextMask() const
{
  return m_bDrawMask;
}

void MYON_TextMask::SetDrawTextMask(bool bDraw)
{
  bool b = bDraw ? true : false;
  if (m_bDrawMask != b)
  {
    m_bDrawMask = b;
    m_content_hash = MYON_SHA1_Hash::ZeroDigest;
  }
}

MYON_TextMask::MaskType  MYON_TextMask::MaskFillType() const
{
  return m_mask_type;
}

void MYON_TextMask::SetMaskFillType(MYON_TextMask::MaskType  type)
{
  if (m_mask_type != type)
  {
    m_mask_type = type;
    m_content_hash = MYON_SHA1_Hash::ZeroDigest;
  }
}

MYON_TextMask::MaskFrame MYON_TextMask::MaskFrameType() const
{
  return m_mask_frame;
}

void MYON_TextMask::SetMaskFrameType(MYON_TextMask::MaskFrame frame) 
{
  if (m_mask_frame != frame)
  {
    m_mask_frame = frame;
    m_content_hash = MYON_SHA1_Hash::ZeroDigest;
  }
}

MYON_Color MYON_TextMask::MaskColor() const
{
  return m_mask_color;
}

void MYON_TextMask::SetMaskColor(MYON_Color color)
{
  if (color != MYON_Color::UnsetColor && m_mask_color != color)
  {
    m_mask_color = color;
    m_content_hash = MYON_SHA1_Hash::ZeroDigest;
  }
}

double MYON_TextMask::MaskBorder() const
{
  return m_mask_border;
}

void MYON_TextMask::SetMaskBorder(double border)
{
  if (border >= 0.0 && m_mask_border != border)
  {
    m_mask_border = border;
    m_content_hash = MYON_SHA1_Hash::ZeroDigest;
  }
}

MYON_TextMask::MaskType MYON_TextMask::MaskTypeFromUnsigned(
  unsigned int mask_type_as_unsigned
)
{
  switch (mask_type_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_TextMask::MaskType::BackgroundColor);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_TextMask::MaskType::MaskColor);
  }
  MYON_ERROR("mask_type_as_unsigned parameter is not valid");
  return MYON_TextMask::MaskType::BackgroundColor;
}

MYON_TextMask::MaskFrame MYON_TextMask::MaskFrameFromUnsigned(
  unsigned int mask_frame_as_unsigned
)
{
  switch (mask_frame_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_TextMask::MaskFrame::NoFrame);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_TextMask::MaskFrame::RectFrame);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_TextMask::MaskFrame::CapsuleFrame);
  }
  MYON_ERROR("mask_type_as_unsigned parameter is not valid");
  return MYON_TextMask::MaskFrame::NoFrame;
}


bool MYON_TextMask::Write(
  MYON_BinaryArchive& archive
) const
{
  const int chunk_version = 1;  // Oct. 9, 2019 - mask_frame
  if (!archive.BeginWrite3dmAnonymousChunk(chunk_version))
    return false;

  bool rc = false;

  for (;;)
  {
    if (!archive.WriteBool(m_bDrawMask))
      break;
    const unsigned int mask_type_as_unsigned = (unsigned int)(static_cast<unsigned char>(m_mask_type));
    if (!archive.WriteInt(mask_type_as_unsigned))
      break;
    if (!archive.WriteColor(m_mask_color))
      break;
    if (!archive.WriteDouble(m_mask_border))
      break;
    // DO NOT write m_content_hash
    // END of chunk_version = 0 information

    const unsigned int mask_frame_as_unsigned = (unsigned int)(static_cast<unsigned char>(m_mask_frame));
    if (!archive.WriteInt(mask_frame_as_unsigned))
      break;
    // END of chunk_version = 1 information

    rc = true;
    break;
  }

  if (!archive.EndWrite3dmChunk())
    rc = false;
  return rc;
}


bool MYON_TextMask::Read(
  MYON_BinaryArchive& archive
) 
{
  *this = MYON_TextMask::None;
  int chunk_version = 0;
  if (!archive.BeginRead3dmAnonymousChunk(&chunk_version))
    return false;

  bool rc = false;

  for (;;)
  {
    if (!archive.ReadBool(&m_bDrawMask))
      break;
    unsigned int mask_type_as_unsigned = (unsigned int)(static_cast<unsigned char>(m_mask_type));
    if (!archive.ReadInt(&mask_type_as_unsigned))
      break;
    m_mask_type = MYON_TextMask::MaskTypeFromUnsigned(mask_type_as_unsigned);
    if (!archive.ReadColor(m_mask_color))
      break;
    if (!archive.ReadDouble(&m_mask_border))
      break;
    // END of chunk_version = 0 information

    if (chunk_version > 0)
    {
      unsigned int mask_frame_as_unsigned = (unsigned int)(static_cast<unsigned char>(m_mask_frame));
      if (!archive.ReadInt(&mask_frame_as_unsigned))
        break;
      m_mask_frame = MYON_TextMask::MaskFrameFromUnsigned(mask_frame_as_unsigned);
    }

    rc = true;
    break;
  }

  if (!archive.EndRead3dmChunk())
    rc = false;
  return rc;
}

#pragma endregion TextMask



MYON_OBJECT_IMPLEMENT(MYON_DimStyle, MYON_ModelComponent, "67AA51A5-791D-4BEC-8AED-D23B462B6F87");

const MYON_DimStyle* MYON_DimStyle::FromModelComponentRef(
  const class MYON_ModelComponentReference& model_component_reference,
  const MYON_DimStyle* none_return_value
  )
{
  const MYON_DimStyle* p = MYON_DimStyle::Cast(model_component_reference.ModelComponent());
  return (nullptr != p) ? p : none_return_value;
}

#pragma region Legacy dimstyle conversions
// convert MYON_DimStyle::OBSOLETE_length_format enum to MYON::OBSOLETE_DistanceDisplayMode enum
MYON::OBSOLETE_DistanceDisplayMode MYON_DimStyle::DistanceDisplayModeFromLengthFormat(
  MYON_DimStyle::OBSOLETE_length_format lf
  )
{
  MYON::OBSOLETE_DistanceDisplayMode ddm;
  switch (lf)
  {
  case MYON_DimStyle::OBSOLETE_length_format::Decimal:
    ddm = MYON::OBSOLETE_DistanceDisplayMode::Decimal;
    break;
  case MYON_DimStyle::OBSOLETE_length_format::Fractional:
    ddm = MYON::OBSOLETE_DistanceDisplayMode::Fractional;
    break;
  case MYON_DimStyle::OBSOLETE_length_format::FeetInches:
    ddm = MYON::OBSOLETE_DistanceDisplayMode::FeetInches;
    break;
  default:
    ddm = MYON::OBSOLETE_DistanceDisplayMode::Decimal;
    break;
  }
  return ddm;
}

MYON_DimStyle::OBSOLETE_length_format MYON_DimStyle::LengthFormatFromDistanceDisplayMode(
  MYON::OBSOLETE_DistanceDisplayMode ddm
  )
{
  MYON_DimStyle::OBSOLETE_length_format lf;
  switch (ddm)
  {
  case MYON::OBSOLETE_DistanceDisplayMode::Decimal:
    lf = MYON_DimStyle::OBSOLETE_length_format::Decimal;
    break;
  case MYON::OBSOLETE_DistanceDisplayMode::Fractional:
    lf = MYON_DimStyle::OBSOLETE_length_format::Fractional;
    break;
  case MYON::OBSOLETE_DistanceDisplayMode::FeetInches:
    lf = MYON_DimStyle::OBSOLETE_length_format::FeetInches;
    break;
  default:
    lf = MYON_DimStyle::OBSOLETE_length_format::Decimal;
    break;
  }
  return lf;
}


int MYON_DimStyle::V5ArrowType(MYON_Arrowhead::arrow_type v6type)
{
  int v5type;
  switch (v6type)
  {
  default:
  case MYON_Arrowhead::arrow_type::SolidTriangle:
  case MYON_Arrowhead::arrow_type::UserBlock:
    v5type = MYON_V5x_DimStyle::eArrowType::solidtriangle;
    break;
  case MYON_Arrowhead::arrow_type::Dot:
    v5type = MYON_V5x_DimStyle::eArrowType::dot;
    break;
  case MYON_Arrowhead::arrow_type::Tick:
    v5type = MYON_V5x_DimStyle::eArrowType::tick;
    break;
  case MYON_Arrowhead::arrow_type::ShortTriangle:
    v5type = MYON_V5x_DimStyle::eArrowType::shorttriangle;
    break;
  case MYON_Arrowhead::arrow_type::OpenArrow:
    v5type = MYON_V5x_DimStyle::eArrowType::arrow;
    break;
  case MYON_Arrowhead::arrow_type::Rectangle:
    v5type = MYON_V5x_DimStyle::eArrowType::rectangle;
    break;
  case MYON_Arrowhead::arrow_type::LongTriangle:
    v5type = MYON_V5x_DimStyle::eArrowType::longtriangle;
    break;
  case MYON_Arrowhead::arrow_type::LongerTriangle:
    v5type = MYON_V5x_DimStyle::eArrowType::longertriangle;
    break;
  }

  return v5type;
}

int MYON_DimStyle::V5LengthFormat(MYON_DimStyle::OBSOLETE_length_format v6format)
{
  int v5format = 0;
  switch (v6format)
  {
  default:
  case MYON_DimStyle::OBSOLETE_length_format::Decimal:
    v5format = 0;
    break;
  case MYON_DimStyle::OBSOLETE_length_format::Fractional:
    v5format = 1;
    break;
  case MYON_DimStyle::OBSOLETE_length_format::FeetInches:
    v5format = 2;
    break;
  }
  return v5format;
}

int MYON_DimStyle::V5AngleFormat(MYON_DimStyle::angle_format v6format)
{
  int v5format = 0;
  return v5format;
}

int MYON_DimStyle::V5ToleranceFormat(MYON_DimStyle::tolerance_format v6format)
{
  int v5style = 0;
  switch (v6format)
  {
  default:
  case MYON_DimStyle::tolerance_format::None:
    v5style = 0;
    break;
  case MYON_DimStyle::tolerance_format::Symmetrical:
    v5style = 1;
    break;
  case MYON_DimStyle::tolerance_format::Deviation:
    v5style = 2;
    break;
  case MYON_DimStyle::tolerance_format::Limits:
    v5style = 3;
    break;
  }
  return v5style;
}

int MYON_DimStyle::V5MaskColorSourceFromV6MaskType(MYON_TextMask::MaskType mask_type)
{
  return
    (mask_type == MYON_TextMask::MaskType::MaskColor)
    ? 1
    : 0;
}

MYON_Arrowhead::arrow_type MYON_DimStyle::V6ArrowType(int v5type)
{
  MYON_Arrowhead::arrow_type v6type;
  switch (v5type)
  {
  case MYON_V5x_DimStyle::eArrowType::solidtriangle:
    v6type = MYON_Arrowhead::arrow_type::SolidTriangle;
    break;
  case MYON_V5x_DimStyle::eArrowType::dot:
    v6type = MYON_Arrowhead::arrow_type::Dot;
    break;
  case MYON_V5x_DimStyle::eArrowType::tick:
    v6type = MYON_Arrowhead::arrow_type::Tick;
    break;
  case MYON_V5x_DimStyle::eArrowType::shorttriangle:
    v6type = MYON_Arrowhead::arrow_type::ShortTriangle;
    break;
  case MYON_V5x_DimStyle::eArrowType::arrow:
    v6type = MYON_Arrowhead::arrow_type::OpenArrow;
    break;
  case MYON_V5x_DimStyle::eArrowType::rectangle:
    v6type = MYON_Arrowhead::arrow_type::Rectangle;
    break;
  case MYON_V5x_DimStyle::eArrowType::longtriangle:
    v6type = MYON_Arrowhead::arrow_type::LongTriangle;
    break;
  case MYON_V5x_DimStyle::eArrowType::longertriangle:
    v6type = MYON_Arrowhead::arrow_type::LongerTriangle;
    break;
  default:
    v6type = MYON_DimStyle::Default.ArrowType1();
    break;
  }
  return v6type;
}

MYON_DimStyle::OBSOLETE_length_format MYON_DimStyle::V6LengthFormat(int v5format)
{
  MYON_DimStyle::OBSOLETE_length_format v6format = MYON_DimStyle::OBSOLETE_length_format::Decimal;
  switch (v5format)
  {
  default:
  case 0:
    v6format = MYON_DimStyle::OBSOLETE_length_format::Decimal;
    break;
  case 1:
    v6format = MYON_DimStyle::OBSOLETE_length_format::Fractional;
    break;
  case 2:
    v6format = MYON_DimStyle::OBSOLETE_length_format::FeetInches;
    break;
  }
  return v6format;
}

MYON_DimStyle::angle_format MYON_DimStyle::V6AngleFormat(int v5format)
{
  MYON_DimStyle::angle_format v6format = MYON_DimStyle::angle_format::DecimalDegrees;
  return v6format;
}

MYON_DimStyle::tolerance_format MYON_DimStyle::V6ToleranceFormat(int v5style)
{
  MYON_DimStyle::tolerance_format  v6style = MYON_DimStyle::tolerance_format::None;
  switch (v5style)
  {
  default:
  case MYON_V5x_DimStyle::eToleranceStyle::tsNone:
    v6style = MYON_DimStyle::tolerance_format::None;
    break;
  case MYON_V5x_DimStyle::eToleranceStyle::tsSymmetrical:
    v6style = MYON_DimStyle::tolerance_format::Symmetrical;
    break;
  case MYON_V5x_DimStyle::eToleranceStyle::tsDeviation:
    v6style = MYON_DimStyle::tolerance_format::Deviation;
    break;
  case MYON_V5x_DimStyle::eToleranceStyle::tsLimits:
    v6style = MYON_DimStyle::tolerance_format::Limits;
    break;
  }
  return v6style;
}

MYON_TextMask::MaskType MYON_DimStyle::V6MaskTypeFromV5MaskColorSource(int v5_mask_source)
{
  return
    (1 == v5_mask_source)
    ? MYON_TextMask::MaskType::MaskColor
    : MYON_TextMask::None.MaskFillType();
}

void MYON_DimStyle::Internal_ContentChange() const
{
  IncrementContentVersionNumber();
  m_content_hash = MYON_SHA1_Hash::EmptyContentHash;
}


MYON_DimStyle::MYON_DimStyle()
  : MYON_ModelComponent(MYON_ModelComponent::Type::DimStyle)
{
}

struct V5_to_V6_field_id_map 
{
  MYON_V5x_DimStyle::Field m_v5_field_id;
  MYON_DimStyle::field m_v6_field_id;
};

static const struct V5_to_V6_field_id_map* GetDimStyleFieldIdMap(
  size_t* field_id_map_count
)
{
  static V5_to_V6_field_id_map field_id_map[] =
  {
    {MYON_V5x_DimStyle::Field::fn_name,MYON_DimStyle::field::Name},
    {MYON_V5x_DimStyle::Field::fn_index,MYON_DimStyle::field::Index},
    {MYON_V5x_DimStyle::Field::fn_extextension,MYON_DimStyle::field::ExtensionLineExtension},
    {MYON_V5x_DimStyle::Field::fn_extoffset,MYON_DimStyle::field::ExtensionLineOffset},
    {MYON_V5x_DimStyle::Field::fn_arrowsize,MYON_DimStyle::field::Arrowsize},
    {MYON_V5x_DimStyle::Field::fn_centermark,MYON_DimStyle::field::Centermark},
    {MYON_V5x_DimStyle::Field::fn_textgap,MYON_DimStyle::field::TextGap},
    {MYON_V5x_DimStyle::Field::fn_textheight,MYON_DimStyle::field::TextHeight},
    {MYON_V5x_DimStyle::Field::fn_textalign,MYON_DimStyle::field::DimTextLocation},
    {MYON_V5x_DimStyle::Field::fn_arrowtype,MYON_DimStyle::field::ArrowType1},
    {MYON_V5x_DimStyle::Field::fn_arrowtype,MYON_DimStyle::field::ArrowType2},
    {MYON_V5x_DimStyle::Field::fn_angularunits,MYON_DimStyle::field::Unset},
    // OBSOLETE // //{MYON_V5x_DimStyle::Field::fn_lengthformat,MYON_DimStyle::field::OBSOLETE_LengthFormat_},
    {MYON_V5x_DimStyle::Field::fn_angleformat,MYON_DimStyle::field::AngleFormat},
    {MYON_V5x_DimStyle::Field::fn_angleresolution,MYON_DimStyle::field::AngleResolution},
    {MYON_V5x_DimStyle::Field::fn_lengthresolution,MYON_DimStyle::field::LengthResolution},
    {MYON_V5x_DimStyle::Field::fn_fontindex,MYON_DimStyle::field::Font},
    {MYON_V5x_DimStyle::Field::fn_lengthfactor,MYON_DimStyle::field::LengthFactor},
    {MYON_V5x_DimStyle::Field::fn_bAlternate,MYON_DimStyle::field::Alternate},
    {MYON_V5x_DimStyle::Field::fn_alternate_lengthfactor,MYON_DimStyle::field::AlternateLengthFactor},
    // OBSOLETE // //{MYON_V5x_DimStyle::Field::fn_alternate_lengthformat,MYON_DimStyle::field::OBSOLETE_LengthFormat_}, 
    {MYON_V5x_DimStyle::Field::fn_alternate_lengthresolution,MYON_DimStyle::field::AlternateLengthResolution},
    {MYON_V5x_DimStyle::Field::fn_alternate_angleformat,MYON_DimStyle::field::AngleFormat}, 
    {MYON_V5x_DimStyle::Field::fn_alternate_angleresolution,MYON_DimStyle::field::AngleResolution},
    {MYON_V5x_DimStyle::Field::fn_prefix,MYON_DimStyle::field::Prefix},
    {MYON_V5x_DimStyle::Field::fn_suffix,MYON_DimStyle::field::Suffix},
    {MYON_V5x_DimStyle::Field::fn_alternate_prefix,MYON_DimStyle::field::AlternatePrefix},
    {MYON_V5x_DimStyle::Field::fn_alternate_suffix,MYON_DimStyle::field::AlternateSuffix},
    {MYON_V5x_DimStyle::Field::fn_dimextension,MYON_DimStyle::field::DimensionLineExtension},
    {MYON_V5x_DimStyle::Field::fn_leaderarrowsize,MYON_DimStyle::field::LeaderArrowsize},
    {MYON_V5x_DimStyle::Field::fn_leaderarrowtype,MYON_DimStyle::field::LeaderArrowType},
    {MYON_V5x_DimStyle::Field::fn_suppressextension1,MYON_DimStyle::field::SuppressExtension1},
    {MYON_V5x_DimStyle::Field::fn_suppressextension2,MYON_DimStyle::field::SuppressExtension2},
    {MYON_V5x_DimStyle::Field::fn_overall_scale,MYON_DimStyle::field::Unset},
    {MYON_V5x_DimStyle::Field::fn_ext_line_color_source,MYON_DimStyle::field::ExtLineColorSource},
    {MYON_V5x_DimStyle::Field::fn_dim_line_color_source,MYON_DimStyle::field::DimLineColorSource},
    {MYON_V5x_DimStyle::Field::fn_arrow_color_source,MYON_DimStyle::field::ArrowColorSource},
    {MYON_V5x_DimStyle::Field::fn_text_color_source,MYON_DimStyle::field::TextColorSource},
    {MYON_V5x_DimStyle::Field::fn_ext_line_color,MYON_DimStyle::field::ExtLineColor},
    {MYON_V5x_DimStyle::Field::fn_dim_line_color,MYON_DimStyle::field::DimLineColor},
    {MYON_V5x_DimStyle::Field::fn_arrow_color,MYON_DimStyle::field::ArrowColor},
    {MYON_V5x_DimStyle::Field::fn_text_color,MYON_DimStyle::field::TextColor},
    {MYON_V5x_DimStyle::Field::fn_ext_line_plot_color_source,MYON_DimStyle::field::ExtLinePlotColorSource},
    {MYON_V5x_DimStyle::Field::fn_dim_line_plot_color_source,MYON_DimStyle::field::DimLineColorSource},
    {MYON_V5x_DimStyle::Field::fn_arrow_plot_color_source,MYON_DimStyle::field::ArrowPlotColorSource},
    {MYON_V5x_DimStyle::Field::fn_text_plot_color_source,MYON_DimStyle::field::TextPlotColorSource},
    {MYON_V5x_DimStyle::Field::fn_ext_line_plot_color ,MYON_DimStyle::field::ExtLinePlotColor},
    {MYON_V5x_DimStyle::Field::fn_dim_line_plot_color,MYON_DimStyle::field::DimLinePlotColor},
    {MYON_V5x_DimStyle::Field::fn_arrow_plot_color,MYON_DimStyle::field::ArrowPlotColor},
    {MYON_V5x_DimStyle::Field::fn_text_plot_color,MYON_DimStyle::field::TextPlotColor},
    {MYON_V5x_DimStyle::Field::fn_ext_line_plot_weight_source,MYON_DimStyle::field::ExtLinePlotWeightSource},
    {MYON_V5x_DimStyle::Field::fn_dim_line_plot_weight_source,MYON_DimStyle::field::DimLinePlotWeightSource},
    {MYON_V5x_DimStyle::Field::fn_ext_line_plot_weight_mm,MYON_DimStyle::field::ExtLinePlotWeight_mm},
    {MYON_V5x_DimStyle::Field::fn_dim_line_plot_weight_mm,MYON_DimStyle::field::DimLinePlotWeight_mm},
    {MYON_V5x_DimStyle::Field::fn_tolerance_style,MYON_DimStyle::field::ToleranceFormat},
    {MYON_V5x_DimStyle::Field::fn_tolerance_resolution,MYON_DimStyle::field::ToleranceResolution},
    {MYON_V5x_DimStyle::Field::fn_tolerance_upper_value,MYON_DimStyle::field::ToleranceUpperValue},
    {MYON_V5x_DimStyle::Field::fn_tolerance_lower_value,MYON_DimStyle::field::ToleranceLowerValue},
    {MYON_V5x_DimStyle::Field::fn_tolerance_height_scale,MYON_DimStyle::field::ToleranceHeightScale},
    {MYON_V5x_DimStyle::Field::fn_baseline_spacing,MYON_DimStyle::field::BaselineSpacing},
    {MYON_V5x_DimStyle::Field::fn_draw_mask,MYON_DimStyle::field::DrawMask},
    {MYON_V5x_DimStyle::Field::fn_mask_color_source,MYON_DimStyle::field::MaskColorSource},
    {MYON_V5x_DimStyle::Field::fn_mask_color,MYON_DimStyle::field::MaskColor},
    {MYON_V5x_DimStyle::Field::fn_mask_border,MYON_DimStyle::field::MaskBorder},
    {MYON_V5x_DimStyle::Field::fn_dimscale,MYON_DimStyle::field::DimensionScale},
    {MYON_V5x_DimStyle::Field::fn_dimscale_source,MYON_DimStyle::field::DimscaleSource},
    {MYON_V5x_DimStyle::Field::fn_fixed_extension_len,MYON_DimStyle::field::FixedExtensionLength},
    {MYON_V5x_DimStyle::Field::fn_fixed_extension_on,MYON_DimStyle::field::FixedExtensionOn},
    {MYON_V5x_DimStyle::Field::fn_text_rotation,MYON_DimStyle::field::TextRotation},
    {MYON_V5x_DimStyle::Field::fn_tolerance_alt_resolution,MYON_DimStyle::field::AltToleranceResolution},
    {MYON_V5x_DimStyle::Field::fn_tolerance_textheight_fraction,MYON_DimStyle::field::Unset},
    {MYON_V5x_DimStyle::Field::fn_suppress_arrow1,MYON_DimStyle::field::SuppressArrow1},
    {MYON_V5x_DimStyle::Field::fn_suppress_arrow2,MYON_DimStyle::field::SuppressArrow2},
    {MYON_V5x_DimStyle::Field::fn_textmove_leader,MYON_DimStyle::field::TextmoveLeader},
    {MYON_V5x_DimStyle::Field::fn_arclength_sym,MYON_DimStyle::field::ArclengthSymbol},
    {MYON_V5x_DimStyle::Field::fn_stack_textheight_fraction,MYON_DimStyle::field::StackTextheightScale},
    {MYON_V5x_DimStyle::Field::fn_stack_format,MYON_DimStyle::field::StackFormat},
    {MYON_V5x_DimStyle::Field::fn_alt_round,MYON_DimStyle::field::AltRound},
    {MYON_V5x_DimStyle::Field::fn_round,MYON_DimStyle::field::Round},
    {MYON_V5x_DimStyle::Field::fn_alt_zero_suppress,MYON_DimStyle::field::AltZeroSuppress},
    // OBSOLETE // {MYON_V5x_DimStyle::Field::fn_tol_zero_suppress,MYON_DimStyle::field::ToleranceZeroSuppress},
    {MYON_V5x_DimStyle::Field::fn_ang_zero_suppress,MYON_DimStyle::field::AngleZeroSuppress},
    {MYON_V5x_DimStyle::Field::fn_zero_suppress,MYON_DimStyle::field::ZeroSuppress},
    {MYON_V5x_DimStyle::Field::fn_alt_below,MYON_DimStyle::field::AltBelow},
    {MYON_V5x_DimStyle::Field::fn_dim_arrow_type1,MYON_DimStyle::field::ArrowType1},
    {MYON_V5x_DimStyle::Field::fn_dim_arrow_type2,MYON_DimStyle::field::ArrowType2},
    {MYON_V5x_DimStyle::Field::fn_dim_arrow_blockname1,MYON_DimStyle::field::ArrowBlockId1},
    {MYON_V5x_DimStyle::Field::fn_dim_arrow_blockname2,MYON_DimStyle::field::ArrowBlockId2}
  };

  *field_id_map_count = sizeof(field_id_map) / sizeof(field_id_map[0]);
  return field_id_map;
}

static void Internal_V6LengthDisplayFromV5LengthFormatAndFactor(
  MYON::LengthUnitSystem model_unit_system,
  int v5_length_format, // 0 = decimal, 1 = fractional, 2 = feet and inches
  double v5_length_factor,
  MYON_DimStyle::LengthDisplay& v6_dimension_length_display,
  double& v6_length_factor
)
{
  if ( 2 == v5_length_format)
  {
    v6_dimension_length_display = MYON_DimStyle::LengthDisplay::FeetAndInches;
    v6_length_factor = 1.0;
    return;
  }

  if (!(v5_length_factor > 0.0 && v5_length_factor < MYON_UNSET_POSITIVE_VALUE))
    v5_length_factor = 1.0;

  v6_dimension_length_display = MYON_DimStyle::LengthDisplay::ModelUnits;
  v6_length_factor = v5_length_factor;

  if (false == MYON::IsTerrestrialLengthUnit(model_unit_system))
  {
    return;
  }

  if (fabs(1.0 - v5_length_factor) <= 0.001)
  {
    v6_length_factor = 1.0;
    if (1 == v5_length_format && MYON::LengthUnitSystem::Inches == model_unit_system)
      v6_dimension_length_display = MYON_DimStyle::LengthDisplay::InchesFractional;
    return;
  }

  // See if v5_length_factor is a V5 length unit scale factor
  const MYON::LengthUnitSystem dim_style_unit_systems[]
    = {
    MYON::LengthUnitSystem::Millimeters,
    MYON::LengthUnitSystem::Centimeters,
    MYON::LengthUnitSystem::Meters,
    MYON::LengthUnitSystem::Kilometers,
    MYON::LengthUnitSystem::Inches,
    MYON::LengthUnitSystem::Feet,
    MYON::LengthUnitSystem::Miles
  };

  const int dim_style_unit_systems_count = (int)(sizeof(dim_style_unit_systems) / sizeof(dim_style_unit_systems[0]));
  MYON::LengthUnitSystem dim_style_unit_system = MYON::LengthUnitSystem::None;
  double dim_style_unit_system_delta = 1e300;
  for (int i = 0; i < dim_style_unit_systems_count; i++)
  {
    const double lf = MYON::UnitScale(model_unit_system, dim_style_unit_systems[i]);
    if (!(lf > 0.0))
      continue;
    const double delta = fabs(lf - v5_length_factor);
    if (!(delta >= 0.0))
      continue;
    if (delta > 0.001*lf)
      continue;

    if (delta < dim_style_unit_system_delta)
    {
      dim_style_unit_system_delta = delta;
      dim_style_unit_system = dim_style_unit_systems[i];
    }
  }

  switch (dim_style_unit_system)
  {
    case MYON::LengthUnitSystem::Millimeters: 
      v6_length_factor = 1.0;
      v6_dimension_length_display = MYON_DimStyle::LengthDisplay::Millmeters;
      break;

    case MYON::LengthUnitSystem::Centimeters:
      v6_length_factor = 1.0;
      v6_dimension_length_display = MYON_DimStyle::LengthDisplay::Centimeters;
      break;

    case MYON::LengthUnitSystem::Meters:
      v6_length_factor = 1.0;
      v6_dimension_length_display = MYON_DimStyle::LengthDisplay::Meters;
      break;

    case MYON::LengthUnitSystem::Kilometers:
      v6_length_factor = 1.0;
      v6_dimension_length_display = MYON_DimStyle::LengthDisplay::Kilometers;
      break;

    case MYON::LengthUnitSystem::Inches:
      v6_length_factor = 1.0;
      if (1 == v5_length_format)
        v6_dimension_length_display = MYON_DimStyle::LengthDisplay::InchesFractional;
      else
        v6_dimension_length_display = MYON_DimStyle::LengthDisplay::InchesDecimal;
      break;

    case MYON::LengthUnitSystem::Feet:
      v6_length_factor = 1.0;
      v6_dimension_length_display = MYON_DimStyle::LengthDisplay::FeetDecimal;
      break;

    case MYON::LengthUnitSystem::Miles:
      v6_length_factor = 1.0;
      v6_dimension_length_display = MYON_DimStyle::LengthDisplay::Miles;
      break;
    default:
      break;
  }

  return;
}

// Convert from V5 MYON_dDimStyle to V6 MYON_Dimstyle
MYON_DimStyle::MYON_DimStyle(
  MYON::LengthUnitSystem model_length_unit_system,
  const MYON_V5x_DimStyle& src
)
  : MYON_ModelComponent(src)
{
  // MYON_ModelComponent(src) copies the parent id

  bool bV5LengthFactorOverride = false;
  bool bV5AltLengthFactorOverride = false;
  bool bV5LengthFormatOverride = false;
  bool bV5AltLengthFormatOverride = false;

  if ( src.ParentIdIsNotNil() && src.m_field_override_count > 0)
  {
    // src.m_field_override_count > 0) indicates some of the settings on the V5 dimstyle
    // are independent of the parent dimstyle. When we find those, 
    // we will set them on this as well.
    size_t field_id_map_count = 0;
    const struct V5_to_V6_field_id_map* field_id_map = GetDimStyleFieldIdMap(&field_id_map_count);
    for (size_t i = 0; i < field_id_map_count; i++)
    {
      if (MYON_V5x_DimStyle::Field::fn_unset == field_id_map[i].m_v5_field_id)
        continue; // no mapping from V6 setting to V6 MYON_V5x_DimStyle
      if (MYON_DimStyle::field::Unset == field_id_map[i].m_v6_field_id)
        continue; // no mapping from V5 setting to V6 MYON_DimStyle
      if (MYON_DimStyle::field::Name == field_id_map[i].m_v6_field_id || MYON_DimStyle::field::Index == field_id_map[i].m_v6_field_id)
        continue; // name and index are never from parent
      if (src.IsFieldOverride(field_id_map[i].m_v5_field_id))
        this->SetFieldOverride(field_id_map[i].m_v6_field_id, true);
    }

    // Now handle obsolete overrides that convert to dimension length display override
    bV5LengthFormatOverride = src.IsFieldOverride(MYON_V5x_DimStyle::Field::fn_lengthformat);
    bV5AltLengthFormatOverride = src.IsFieldOverride(MYON_V5x_DimStyle::Field::fn_alternate_lengthformat);
    bV5LengthFactorOverride = src.IsFieldOverride(MYON_V5x_DimStyle::Field::fn_lengthfactor);
    bV5AltLengthFactorOverride = src.IsFieldOverride(MYON_V5x_DimStyle::Field::fn_alternate_lengthfactor);
  }

  m_extextension = src.m_extextension;
  m_extoffset = src.m_extoffset;
  m_arrowsize = src.m_arrowsize;
  m_centermark = src.m_centermark;
  m_textgap = src.m_textgap;
  m_textheight = src.m_textheight;


  // V5 had a single MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode enum
  // V6 has 8 separate settings and 2 enums
  switch (src.m_dimstyle_textalign)
  {
  case MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kNormal:
    m_dim_text_location = MYON_DimStyle::TextLocation::AboveDimLine;
    m_dimradial_text_location = MYON_DimStyle::TextLocation::InDimLine;
    m_dimradial_text_angle_style = MYON_DimStyle::ContentAngleStyle::Horizontal;
    break;

  case MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kHorizontalToScreen:
    m_dim_text_location = MYON_DimStyle::TextLocation::InDimLine;
    m_dimradial_text_location = MYON_DimStyle::TextLocation::InDimLine;
    m_leader_text_orientation     = MYON::TextOrientation::InView;
    m_dim_text_orientation        = MYON::TextOrientation::InView;
    m_dimradial_text_orientation  = MYON::TextOrientation::InView;
    m_dimradial_text_angle_style = MYON_DimStyle::ContentAngleStyle::Horizontal;
    break;

  case MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kAboveLine:
    m_dim_text_location = MYON_DimStyle::TextLocation::AboveDimLine;
    m_dimradial_text_location = MYON_DimStyle::TextLocation::InDimLine;
    m_dimradial_text_angle_style = MYON_DimStyle::ContentAngleStyle::Horizontal;
    break;

  case MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kInLine:
    m_dim_text_location = MYON_DimStyle::TextLocation::InDimLine;
    m_dimradial_text_location = MYON_DimStyle::TextLocation::InDimLine;
    m_dimradial_text_angle_style = MYON_DimStyle::ContentAngleStyle::Horizontal;
    break;

  case MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kHorizontalInCplane:
    m_dim_text_location = MYON_DimStyle::TextLocation::InDimLine;
    m_dimradial_text_location = MYON_DimStyle::TextLocation::InDimLine;
    m_dim_text_angle_style = MYON_DimStyle::ContentAngleStyle::Horizontal;
    m_dimradial_text_angle_style = MYON_DimStyle::ContentAngleStyle::Horizontal;
    break;
  }

  m_arrow_type_1 = V6ArrowType(src.m_arrowtype);
  m_arrow_type_2 = V6ArrowType(src.m_arrowtype);
  m_angleformat = MYON_DimStyle::angle_format::DecimalDegrees;

  // Set m_dimension_length_display and m_lengthfactor
  const MYON_DimStyle::LengthDisplay dimension_length_display0 = m_dimension_length_display;
  Internal_V6LengthDisplayFromV5LengthFormatAndFactor(
    model_length_unit_system,
    src.m_lengthformat, // 0 = decimal, 1 = fractional, 2 = feet and inches
    src.m_lengthfactor,
    m_dimension_length_display,
    m_lengthfactor
  );
  if (bV5LengthFormatOverride || bV5LengthFactorOverride)
  {
    if (dimension_length_display0 != m_dimension_length_display)
    {
      this->SetFieldOverride(MYON_DimStyle::field::DimensionLengthDisplay, true);
      if ( 1.0 == m_lengthfactor )
        this->SetFieldOverride(MYON_DimStyle::field::LengthFactor, false);
    }
    if( m_lengthfactor != src.m_lengthfactor && bV5LengthFactorOverride)
      this->SetFieldOverride(MYON_DimStyle::field::LengthFactor, false);
  }

  // Set m_alternate_dimension_length_display and m_alternate_lengthfactor
  const MYON_DimStyle::LengthDisplay alternate_dimension_length_display0 = m_alternate_dimension_length_display;
  Internal_V6LengthDisplayFromV5LengthFormatAndFactor(
    model_length_unit_system,
    src.m_alternate_lengthformat, // 0 = decimal, 1 = fractional, 2 = feet and inches
    src.m_alternate_lengthfactor,
    m_alternate_dimension_length_display,
    m_alternate_lengthfactor
  );
  if (bV5AltLengthFormatOverride || bV5AltLengthFactorOverride)
  {
    if (alternate_dimension_length_display0 != m_alternate_dimension_length_display)
    {
      this->SetFieldOverride(MYON_DimStyle::field::AlternateDimensionLengthDisplay, true);
      if ( 1.0 == m_alternate_lengthfactor )
        this->SetFieldOverride(MYON_DimStyle::field::AlternateLengthFactor, false);
    }
    if( m_alternate_lengthfactor != src.m_alternate_lengthfactor && bV5AltLengthFactorOverride)
      this->SetFieldOverride(MYON_DimStyle::field::AlternateLengthFactor, false);
  }

  m_lengthresolution = src.m_lengthresolution;
  m_angleresolution = src.m_angleresolution;
  SetFont( src.V5TextStyle().Font() );
  m_bAlternate = src.m_bAlternate;

  m_alternate_lengthresolution = src.m_alternate_lengthresolution;
  m_prefix = src.m_prefix;
  m_suffix = src.m_suffix;
  m_alternate_prefix = src.m_alternate_prefix;
  m_alternate_suffix = src.m_alternate_suffix;
  m_dimextension = src.m_dimextension;
  m_leaderarrowsize = src.m_leaderarrowsize;
  m_leader_arrow_type = V6ArrowType(src.m_leaderarrowtype);
  m_bSuppressExtension1 = src.m_bSuppressExtension1;
  m_bSuppressExtension2 = src.m_bSuppressExtension2;
  m_source_dimstyle = src.m_source_dimstyle;
  m_tolerance_format = V6ToleranceFormat(src.m_tolerance_style);
  m_tolerance_resolution = src.m_tolerance_resolution;
  m_tolerance_upper_value = src.m_tolerance_upper_value;
  m_tolerance_lower_value = src.m_tolerance_lower_value;
  m_tolerance_height_scale = src.m_tolerance_height_scale;
  m_baseline_spacing = src.m_baseline_spacing;
  SetDrawTextMask(src.m_bDrawMask);
  SetMaskFillType(V6MaskTypeFromV5MaskColorSource(src.m_mask_color_source));
  SetMaskColor(src.m_mask_color);
  SetMaskBorder(src.m_textgap);
  //m_dimscale = src.m_dimscale;
  SetDimScale( src.m_dimscale); // sets m_scale_value and m_dimscale fields.
  m_dimscale_source = src.m_dimscale_source;
  m_leader_landing_length = m_textheight;

  switch (src.m_dimstyle_textalign)
  {

  case MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kHorizontalToScreen:
  case MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kHorizontalInCplane:
    m_leader_content_angle_style = MYON_DimStyle::ContentAngleStyle::Horizontal;
    break;

  case MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kNormal:
  case MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kAboveLine:
  case MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kInLine:
  default:
    m_leader_content_angle_style = MYON_DimStyle::ContentAngleStyle::Aligned;
    break;
  }

  m_leader_has_landing = false; // V5 had no landing concept.  V5 Leaders have an extra horizontal segment

  if (MYON_DimStyle::LengthDisplay::FeetAndInches == m_dimension_length_display)
  {
    // this matches V5 default behavior.
    m_zero_suppress = MYON_DimStyle::suppress_zero::SuppressZeroFeet;
  }


  // Make sure zero suppression settings are valid
  if (false == MYON_DimStyle::ZeroSuppressMatchesLengthDisplay(m_zero_suppress, m_dimension_length_display))
    m_zero_suppress = MYON_DimStyle::suppress_zero::None;

  if (false == MYON_DimStyle::ZeroSuppressMatchesLengthDisplay(m_alt_zero_suppress, m_alternate_dimension_length_display))
    m_alt_zero_suppress = MYON_DimStyle::suppress_zero::None;

  // any decimal format will do to test m_ang_zero_suppress
  if (false == MYON_DimStyle::ZeroSuppressMatchesLengthDisplay(m_ang_zero_suppress, MYON_DimStyle::LengthDisplay::ModelUnits))
    m_ang_zero_suppress = MYON_DimStyle::suppress_zero::None;
}

MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode MYON_INTERNAL_OBSOLETE::V5TextDisplayModeFromV6DimStyle(
  const MYON_DimStyle& V6_dim_style
)
{
  if (V6_dim_style.DimTextOrientation() == MYON::TextOrientation::InView)
  {
    return MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kHorizontalToScreen;
  }

  switch (V6_dim_style.DimTextLocation())
  {
  case MYON_DimStyle::TextLocation::AboveDimLine:
    return MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kAboveLine;
    break;

  case MYON_DimStyle::TextLocation::InDimLine:
    return (MYON_DimStyle::ContentAngleStyle::Horizontal == V6_dim_style.DimTextAngleStyle() )
      ? MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kHorizontalInCplane
      : MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kInLine;
    break;

  case MYON_DimStyle::TextLocation::BelowDimLine:
    return MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kAboveLine;
    break;

  default:
    break;
  }

  return MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kNormal;
}

static void Internal_SetV5LengthFormatAndFactorFromV6LengthDisplay(
  MYON::LengthUnitSystem model_length_units,
  MYON_DimStyle::LengthDisplay V6length_display,
  double V6_length_factor,
  int& V5_lengthformat,
  double& V5_lengthfactor
)
{
  if (!(V6_length_factor > 0.0 && V6_length_factor < MYON_UNSET_POSITIVE_VALUE))
    V6_length_factor = 1.0;

  V5_lengthformat = 0;
  V5_lengthfactor = V6_length_factor;

  if (MYON_DimStyle::LengthDisplay::ModelUnits == V6length_display)
  {
    return;
  }

  if (MYON_DimStyle::LengthDisplay::FeetAndInches == V6length_display)
  {
    V5_lengthformat = 2;
    return;
  }

  if (false == MYON::IsTerrestrialLengthUnit(model_length_units))
  {
    return;
  }

  const MYON::LengthUnitSystem V6length_display_unit_system = MYON_DimStyle::LengthUnitSystemFromLengthDisplay(V6length_display);
  if ( MYON::LengthUnitSystem::None == V6length_display_unit_system || false == MYON::IsTerrestrialLengthUnit(V6length_display_unit_system))
  {
    return;
  }

  V5_lengthfactor = V6_length_factor*MYON::UnitScale(model_length_units, V6length_display_unit_system);

  if (MYON_DimStyle::LengthDisplay::InchesFractional == V6length_display)
  {
    V5_lengthformat = 1;
    return;
  }

  return; 
}

// Convert from V6 MYON_Dimstyle to v5 MYON_Dimstyle
MYON_V5x_DimStyle::MYON_V5x_DimStyle(
  MYON::LengthUnitSystem model_length_unit_system,
  const MYON_DimStyle& src
)
  : MYON_ModelComponent(src)
{
  ClearAllFieldOverrides();
  if ( src.ParentIdIsNotNil() )
  {
    ///ClearId(); // why?
    ///ClearIndex(); // why?
    ClearModelSerialNumber(); // why?

    SetParentId(src.ParentId());
    if ( src.HasOverrides() )
    {
      size_t field_id_map_count = 0;
      const struct V5_to_V6_field_id_map* field_id_map = GetDimStyleFieldIdMap(&field_id_map_count);
      for (size_t i = 0; i < field_id_map_count; i++)
      {        
        if ( MYON_V5x_DimStyle::Field::fn_unset == field_id_map[i].m_v5_field_id )
          continue; // no mapping from V6 setting to V6 MYON_V5x_DimStyle
        if ( MYON_DimStyle::field::Unset == field_id_map[i].m_v6_field_id)
          continue; // no mapping from V5 setting to V6 MYON_DimStyle
        if ( MYON_DimStyle::field::Name == field_id_map[i].m_v6_field_id)
          continue; // name is never from parent
        if ( MYON_DimStyle::field::Index == field_id_map[i].m_v6_field_id)
          continue; // index is never from parent
        if ( src.IsFieldOverride(field_id_map[i].m_v6_field_id) )
          SetFieldOverride(field_id_map[i].m_v5_field_id, true);
      }
    }
  }

  m_extextension = src.m_extextension;
  m_extoffset = src.m_extoffset;
  m_arrowsize = src.m_arrowsize;
  m_centermark = src.m_centermark;
  m_textgap = src.m_textgap;
  m_textheight = src.m_textheight;
  m_dimstyle_textalign = MYON_INTERNAL_OBSOLETE::V5TextDisplayModeFromV6DimStyle(src);
  m_arrowtype = MYON_DimStyle::V5ArrowType(src.m_arrow_type_1);
  m_angularunits = 0;

  m_lengthformat = 0;
  m_lengthfactor = 1.0;
  Internal_SetV5LengthFormatAndFactorFromV6LengthDisplay(
    model_length_unit_system,
    src.m_dimension_length_display,
    src.m_lengthfactor,
    m_lengthformat,
    m_lengthfactor
  );

  m_alternate_lengthformat = 0;
  m_alternate_lengthfactor = 1.0;
  Internal_SetV5LengthFormatAndFactorFromV6LengthDisplay(
    model_length_unit_system,
    src.m_alternate_dimension_length_display,
    src.m_alternate_lengthfactor,
    m_alternate_lengthformat,
    m_alternate_lengthfactor
  );

  m_angleformat = MYON_DimStyle::V5AngleFormat(src.m_angleformat);

  //const MYON_DimStyle::OBSOLETE_length_format obsolete_v6_length_format = MYON_DimStyle::OBSOLETE_LengthFormatFromLengthDisplay(
  //  src.DimensionLengthDisplay(),
  //  src.ModelSerialNumber()
  //);
  
  m_lengthresolution = src.m_lengthresolution;
  m_angleresolution = src.m_angleresolution;
  m_v5_text_style.SetFont(src.Font());
  m_bAlternate = src.m_bAlternate;

  //const MYON_DimStyle::OBSOLETE_length_format obsolete_v6_alt_length_format = MYON_DimStyle::OBSOLETE_LengthFormatFromLengthDisplay(
  //  src.AlternateDimensionLengthDisplay(),
  //  src.ModelSerialNumber()
  //);

  m_alternate_lengthresolution = src.m_alternate_lengthresolution;
  m_prefix = src.m_prefix;
  m_suffix = src.m_suffix;
  m_alternate_prefix = src.m_alternate_prefix;
  m_alternate_suffix = src.m_alternate_suffix;
  m_dimextension = src.m_dimextension;
  m_leaderarrowsize = src.m_leaderarrowsize;
  m_leaderarrowtype = MYON_DimStyle::V5ArrowType(src.m_leader_arrow_type);
  m_bSuppressExtension1 = src.m_bSuppressExtension1;
  m_bSuppressExtension2 = src.m_bSuppressExtension2;
  m_source_dimstyle = src.m_source_dimstyle;
  m_tolerance_style = MYON_DimStyle::V5ToleranceFormat(src.m_tolerance_format);
  m_tolerance_resolution = src.m_tolerance_resolution;
  m_tolerance_upper_value = src.m_tolerance_upper_value;
  m_tolerance_lower_value = src.m_tolerance_lower_value;
  m_tolerance_height_scale = src.m_tolerance_height_scale;
  m_baseline_spacing = src.m_baseline_spacing;
  m_bDrawMask = src.DrawTextMask();
  m_mask_color_source = MYON_DimStyle::V5MaskColorSourceFromV6MaskType(src.MaskFillType());
  m_mask_color = src.MaskColor();
  m_dimscale = src.DimScale();
  m_dimscale_source = src.m_dimscale_source;
}


#pragma endregion Legacy dimstyle conversions

unsigned int MYON_DimStyle::Internal_GetSystemDimstyleList(
  MYON_SimpleArray<const MYON_DimStyle*>& system_dimstyle_list
)
{
  system_dimstyle_list.SetCount(0);
  system_dimstyle_list.Reserve(8);
  system_dimstyle_list.Append(&MYON_DimStyle::Default);
  system_dimstyle_list.Append(&MYON_DimStyle::DefaultInchDecimal);
  system_dimstyle_list.Append(&MYON_DimStyle::DefaultInchFractional);
  system_dimstyle_list.Append(&MYON_DimStyle::DefaultFootInchArchitecture);
  system_dimstyle_list.Append(&MYON_DimStyle::DefaultMillimeterSmall);
  system_dimstyle_list.Append(&MYON_DimStyle::DefaultMillimeterLarge);
  system_dimstyle_list.Append(&MYON_DimStyle::DefaultMillimeterArchitecture);
  return system_dimstyle_list.UnsignedCount();
}

const MYON_DimStyle& MYON_DimStyle::DimStyleOrDefault(
  const class MYON_DimStyle* dimstyle
)
{
  if (nullptr == dimstyle)
    dimstyle = &MYON_DimStyle::Default;
  return *dimstyle;
}


const MYON_DimStyle& MYON_DimStyle::SystemDimstyleFromId(
  MYON_UUID id
)
{
  if (id != MYON_nil_uuid)
  {
    MYON_SimpleArray<const MYON_DimStyle*> system_dimstyle_list;
    const unsigned int count = MYON_DimStyle::Internal_GetSystemDimstyleList(system_dimstyle_list);
    for (unsigned int i = 0; i < count; i++)
    {
      if (id == system_dimstyle_list[i]->Id())
        return *system_dimstyle_list[i];
    }
  }
  return MYON_DimStyle::Unset;
}

const MYON_DimStyle& MYON_DimStyle::SystemDimstyleFromIndex(
  int index
)
{
  if (index < 0 && index > MYON_UNSET_INT_INDEX)
  {
    MYON_SimpleArray<const MYON_DimStyle*> system_dimstyle_list;
    const unsigned int count = MYON_DimStyle::Internal_GetSystemDimstyleList(system_dimstyle_list);
    for (unsigned int i = 0; i < count; i++)
    {
      if (index == system_dimstyle_list[i]->Index())
        return *system_dimstyle_list[i];
    }
  }
  return MYON_DimStyle::Unset;
}

const MYON_DimStyle& MYON_DimStyle::SystemDimstyleFromName(
  const MYON_NameHash& name_hash
)
{
  if (name_hash.IsValidAndNotEmpty())
  {
    MYON_SimpleArray<const MYON_DimStyle*> system_dimstyle_list;
    const unsigned int count = MYON_DimStyle::Internal_GetSystemDimstyleList(system_dimstyle_list);
    for (unsigned int i = 0; i < count; i++)
    {
      if ( name_hash == system_dimstyle_list[i]->NameHash() )
        return *system_dimstyle_list[i];
    }
  }
  return MYON_DimStyle::Unset;
}


const MYON_DimStyle& MYON_DimStyle::SystemDimstyleFromContentHash(
  const MYON_SHA1_Hash& content_hash
)
{
  if (false == content_hash.IsZeroDigestOrEmptyContentHash())
  {
    MYON_SimpleArray<const MYON_DimStyle*> system_dimstyle_list;
    const unsigned int count = MYON_DimStyle::Internal_GetSystemDimstyleList(system_dimstyle_list);
    for (unsigned int i = 0; i < count; i++)
    {
      if ( content_hash == system_dimstyle_list[i]->ContentHash() )
        return *system_dimstyle_list[i];
    }
  }
  return MYON_DimStyle::Unset;
}

// construct from V4 and earier MYON_3dmAnnotationSettings and add a couple of fields
MYON_DimStyle::MYON_DimStyle(const MYON_3dmAnnotationSettings& src)
  : MYON_ModelComponent(MYON_ModelComponent::Type::DimStyle)
{
  m_extextension = src.m_dimexe;
  m_extoffset = src.m_dimexo;
  m_arrowsize = src.m_arrowlength;
  m_centermark = src.m_centermark;
  m_textgap = src.m_dimexo / 2.0;
  m_textheight = src.m_textheight;
  m_dimension_length_display
    = (2 == src.m_lengthformat)
    ? MYON_DimStyle::LengthDisplay::FeetAndInches
    : MYON_DimStyle::LengthDisplay::ModelUnits;
  m_angleformat = (MYON_DimStyle::angle_format) src.m_angleformat;
  m_lengthresolution = src.m_resolution;
  m_angleresolution = src.m_resolution;
  m_dimension_length_display
    = (2 == src.m_lengthformat)
    ? MYON_DimStyle::LengthDisplay::FeetAndInches
    : MYON_DimStyle::LengthDisplay::ModelUnits;
}

bool MYON_DimStyle::Internal_EqualOverrideParentFields(
    const MYON_DimStyle& a,
    const MYON_DimStyle& b
  )
{
  return (
    a.m_field_override_parent_count == b.m_field_override_parent_count
    && a.m_field_override_parent_bits0 == b.m_field_override_parent_bits0
    && a.m_field_override_parent_bits1 == b.m_field_override_parent_bits1
    && a.m_field_override_parent_bits2 == b.m_field_override_parent_bits2
    && a.m_field_override_parent_bits3 == b.m_field_override_parent_bits3
    );
}

bool MYON_DimStyle::CompareDimstyle(const MYON_DimStyle& style) const
{
  return (
    0 == MYON_ModelComponent::CompareNameExact(*this, style)
    && CompareFields(style)
    && MYON_DimStyle::Internal_EqualOverrideParentFields(*this,style)
    );
}

bool MYON_DimStyle::CompareFields(const MYON_DimStyle& style) const
{
  if (
    Font().ManagedFontSerialNumber() == style.Font().ManagedFontSerialNumber() && // text rendered the same way
    0 == MYON_Font::CompareFontCharacteristics(m_font_characteristics,style.m_font_characteristics) && // same values passed to set font
    m_extextension == style.m_extextension &&
    m_extoffset == style.m_extoffset &&
    m_arrowsize == style.m_arrowsize &&
    m_centermark == style.m_centermark &&
    m_centermark_style == style.m_centermark_style &&
    m_textgap == style.m_textgap &&
    m_textheight == style.m_textheight &&
    m_dim_text_location == style.m_dim_text_location &&
    m_dimradial_text_location == style.m_dimradial_text_location &&

    m_dimension_length_display == style.m_dimension_length_display &&
    m_alternate_dimension_length_display == style.m_alternate_dimension_length_display &&

    m_lengthresolution == style.m_lengthresolution &&
    m_angleformat == style.m_angleformat &&
    m_angleresolution == style.m_angleresolution &&
    m_lengthfactor == style.m_lengthfactor &&

    m_bAlternate == style.m_bAlternate &&
    m_alternate_lengthfactor == style.m_alternate_lengthfactor &&
    m_alternate_lengthresolution == style.m_alternate_lengthresolution &&

    m_prefix == style.m_prefix &&
    m_suffix == style.m_suffix &&
    m_alternate_prefix == style.m_alternate_prefix &&
    m_alternate_suffix == style.m_alternate_suffix &&

    m_dimextension == style.m_dimextension &&
    m_bSuppressExtension1 == style.m_bSuppressExtension1 &&
    m_bSuppressExtension2 == style.m_bSuppressExtension2 &&

    ParentId() == style.ParentId() &&
    m_source_dimstyle == style.m_source_dimstyle &&

    m_tolerance_format == style.m_tolerance_format &&
    m_tolerance_resolution == style.m_tolerance_resolution &&
    m_alternate_tolerance_resolution == style.m_alternate_tolerance_resolution &&
    m_tolerance_upper_value == style.m_tolerance_upper_value &&
    m_tolerance_lower_value == style.m_tolerance_lower_value &&
    m_tolerance_height_scale == style.m_tolerance_height_scale &&
    m_baseline_spacing == style.m_baseline_spacing &&
    m_text_mask == style.m_text_mask &&
    DimScale() == style.DimScale() &&
    m_dimscale_source == style.m_dimscale_source &&

    m_ext_line_color_source == style.m_ext_line_color_source &&
    m_dim_line_color_source == style.m_dim_line_color_source &&
    m_arrow_color_source == style.m_arrow_color_source &&
    m_text_color_source == style.m_text_color_source &&
    m_ext_line_color == style.m_ext_line_color &&
    m_dim_line_color == style.m_dim_line_color &&
    m_arrow_color == style.m_arrow_color &&
    m_text_color == style.m_text_color &&
    m_ext_line_plot_color_source == style.m_ext_line_plot_color_source &&
    m_dim_line_plot_color_source == style.m_dim_line_plot_color_source &&
    m_arrow_plot_color_source == style.m_arrow_plot_color_source &&
    m_text_plot_color_source == style.m_text_plot_color_source &&
    m_ext_line_plot_color == style.m_ext_line_plot_color &&
    m_dim_line_plot_color == style.m_dim_line_plot_color &&
    m_arrow_plot_color == style.m_arrow_plot_color &&
    m_text_plot_color == style.m_text_plot_color &&
    m_ext_line_plot_weight_source == style.m_ext_line_plot_weight_source &&
    m_dim_line_plot_weight_source == style.m_dim_line_plot_weight_source &&
    m_ext_line_plot_weight_mm == style.m_ext_line_plot_weight_mm &&
    m_dim_line_plot_weight_mm == style.m_dim_line_plot_weight_mm &&

    m_fixed_extension_len == style.m_fixed_extension_len &&
    m_fixed_extension_len_on == style.m_fixed_extension_len_on &&
    m_text_rotation == style.m_text_rotation &&
    m_alternate_tolerance_resolution == style.m_alternate_tolerance_resolution &&
    m_suppress_arrow1 == style.m_suppress_arrow1 &&
    m_suppress_arrow2 == style.m_suppress_arrow2 &&
    m_textmove_leader == style.m_textmove_leader &&
    m_arclength_sym == style.m_arclength_sym &&
    m_stack_textheight_fraction == style.m_stack_textheight_fraction &&
    m_stack_format == style.m_stack_format &&
    m_alt_round == style.m_alt_round &&
    m_round == style.m_round &&
    m_angular_round == style.m_angular_round &&
    m_alt_zero_suppress == style.m_alt_zero_suppress &&
    m_zero_suppress == style.m_zero_suppress &&
    m_ang_zero_suppress == style.m_ang_zero_suppress &&

    m_arrow_type_1 == style.m_arrow_type_1 &&
    m_arrow_type_2 == style.m_arrow_type_2 &&
    m_leader_arrow_type == style.m_leader_arrow_type &&
    m_arrow_block_id_1 == style.m_arrow_block_id_1 &&
    m_arrow_block_id_2 == style.m_arrow_block_id_2 &&
    m_leader_arrow_block_id == style.m_leader_arrow_block_id &&

    m_text_vertical_alignment == style.m_text_vertical_alignment &&
    m_leader_text_vertical_alignment == style.m_leader_text_vertical_alignment &&
    m_leader_text_horizontal_alignment == style.m_leader_text_horizontal_alignment &&
    m_leader_content_angle_style == style.m_leader_content_angle_style &&
    m_leader_curve_type == style.m_leader_curve_type &&
    m_leader_content_angle == style.m_leader_content_angle &&
    m_leader_has_landing == style.m_leader_has_landing &&
    m_leader_landing_length == style.m_leader_landing_length &&
    m_draw_forward == style.m_draw_forward &&
    m_signed_ordinate == style.m_signed_ordinate &&

    m_scale_value.LeftToRightScale() == style.m_scale_value.LeftToRightScale() &&
    m_dimstyle_unitsystem == style.m_dimstyle_unitsystem &&
    m_text_orientation == style.m_text_orientation &&
    m_leader_text_orientation == style.m_leader_text_orientation &&
    m_dim_text_orientation == style.m_dim_text_orientation &&
    m_dimradial_text_orientation == style.m_dimradial_text_orientation &&
    m_dim_text_angle_style == style.m_dim_text_angle_style &&
    m_dimradial_text_angle_style == style.m_dimradial_text_angle_style &&
    m_text_underlined == style.m_text_underlined &&
    
    m_bForceDimLine == style.m_bForceDimLine &&
    m_ArrowFit == style.m_ArrowFit &&
    m_TextFit == style.m_TextFit &&
    m_decimal_separator == style.m_decimal_separator
    )
    return true;
  else
    return false;
}

//////////////////////////////////////////////////////////////////////
//
// MYON_Object overrides

bool MYON_DimStyle::IsValid(MYON_TextLog* text_log) const
{
  return (NameIsNotEmpty() && Index() >= 0);
}

void MYON_DimStyle::Dump(MYON_TextLog& dump) const
{
  MYON_ModelComponent::Dump(dump);
  dump.Print("Font\n");
  dump.PushIndent();
  m_font_characteristics.Dump(dump);
  dump.PopIndent();
}


/////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////





bool MYON_DimStyle::Write(
  MYON_BinaryArchive& file // serialize definition to binary archive
  ) const
{
  if (!file.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK, 1, 9))
    return false;

  bool rc = false;
  for (;;)
  {
    if (!file.WriteModelComponentAttributes(*this, MYON_ModelComponent::Attributes::BinaryArchiveAttributes))
      break;

    unsigned int u;

    if (!file.WriteDouble(m_extextension)) break;
    if (!file.WriteDouble(m_extoffset)) break;
    if (!file.WriteDouble(m_arrowsize)) break;
    if (!file.WriteDouble(m_leaderarrowsize)) break;
    if (!file.WriteDouble(m_centermark)) break;
    if (!file.WriteDouble(m_textgap)) break;
    if (!file.WriteDouble(m_textheight)) break;


    MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode obsolete_dimstyle_textalign = MYON_INTERNAL_OBSOLETE::V5TextDisplayModeFromV6DimStyle(*this);
    u = static_cast<unsigned int>(obsolete_dimstyle_textalign);
    if (!file.WriteInt(u)) break;

    u = static_cast<unsigned char>(m_angleformat);
    if (!file.WriteInt(u)) break;

    // do not pass in model_sn - we want MYON::LengthUnitSystem::None from MYON_DimStyle::LengthDisplay::ModelUnits.
    const MYON::LengthUnitSystem dim_length_display_unit = DimensionLengthDisplayUnit(0);
    const MYON::LengthUnitSystem alt_dim_length_display_unit = AlternateDimensionLengthDisplayUnit(0);

    // write obsolete MYON_DimStyle::OBSOLETE_LengthForma enum value
    u = static_cast<unsigned char>(
      MYON_DimStyle::OBSOLETE_LengthFormatFromLengthDisplay(
        DimensionLengthDisplay(),
        dim_length_display_unit
      )
      );
    if (!file.WriteInt(u)) break;

    if (!file.WriteInt(m_angleresolution)) break;
    if (!file.WriteInt(m_lengthresolution)) break;

    if (file.Archive3dmVersion() >= 60)
    {
      int ignored_text_style_index = MYON_UNSET_INT_INDEX;
      if (!file.WriteInt(ignored_text_style_index))
        break;
    }
    else
    {
      // MYON_ModelComponent::Type::TextStyle, Index() are not  mistakes.
      // The code in Write3dmReferencedComponentIndex() will convert the dim style index to an approprate
      // value tht depends on the version ofr 3dm archive (<= V5 or >= V6) being saved.
      if (!file.Write3dmReferencedComponentIndex(MYON_ModelComponent::Type::TextStyle, Index())) break;
    }

    if (!file.WriteDouble(m_lengthfactor)) break;
    if (!file.WriteBool(m_bAlternate)) break;
    if (!file.WriteDouble(m_alternate_lengthfactor)) break;

    // write alternate obsolete MYON_DimStyle::OBSOLETE_LengthForma enum value
    u = static_cast<unsigned char>(
      MYON_DimStyle::OBSOLETE_LengthFormatFromLengthDisplay(
        AlternateDimensionLengthDisplay(),
        alt_dim_length_display_unit
      )
      );
    if (!file.WriteInt(u)) break;

    if (!file.WriteInt(m_alternate_lengthresolution)) break;

    // The MYON_wString::IsValid calls prevent corrupt strings from breaking file IO
    m_prefix.IsValid(true);
    if (!file.WriteString(m_prefix)) break;

    m_suffix.IsValid(true);
    if (!file.WriteString(m_suffix)) break;

    m_alternate_prefix.IsValid(true);
    if (!file.WriteString(m_alternate_prefix)) break;

    m_alternate_suffix.IsValid(true);
    if (!file.WriteString(m_alternate_suffix)) break;

    if (!file.WriteDouble(m_dimextension)) break;
    if (!file.WriteBool(m_bSuppressExtension1)) break;
    if (!file.WriteBool(m_bSuppressExtension2)) break;

    if (!file.WriteUuid(ParentId())) break;

    unsigned int ignored_legacy_value = m_field_override_parent_count;
    if (!file.WriteInt(ignored_legacy_value)) break;

    bool bOverrides = (m_field_override_parent_count > 0);
    if (!file.WriteBool(bOverrides)) break;
    if (bOverrides)
    {
      const unsigned int count = static_cast<unsigned int>(MYON_DimStyle::field::Count);
      MYON_SimpleArray<bool> overrides(count);
      for (unsigned int i = 0; i < count; i++)
      {
        overrides.Append(this->IsFieldOverride(MYON_DimStyle::FieldFromUnsigned(i)));
      }
      if (!file.WriteArray(overrides)) break;
    }

    u = static_cast<unsigned char>(m_tolerance_format);
    if (!file.WriteInt(u)) break;

    if (!file.WriteInt(m_tolerance_resolution)) break;
    if (!file.WriteDouble(m_tolerance_upper_value)) break;   // or both upper and lower in symmetrical style
    if (!file.WriteDouble(m_tolerance_lower_value)) break;
    if (!file.WriteDouble(m_tolerance_height_scale)) break;  // relative to the main dimension text

    if (!file.WriteDouble(m_baseline_spacing)) break;

    // The text mask settings used to be a collection of several member variables
    // on MYON_DimStyle and are now in an MYON_TextMask class and a m_text_mask on MYON_DimStyle.
    // These values are written here so the file format is not broken and 
    // older versions of Rhino can read newer file.
    if (!file.WriteBool(DrawTextMask())) break;
    u = (unsigned int)(static_cast<unsigned char>(MaskFillType()));
    if (!file.WriteInt(u)) break;
    if (!file.WriteColor(MaskColor())) break;

    if (!file.WriteDouble( DimScale() )) break;
    if (!file.WriteInt(m_dimscale_source)) break;

    if (!file.WriteUuid(m_source_dimstyle)) break;
    if (!file.WriteChar(m_ext_line_color_source)) break;
    if (!file.WriteChar(m_dim_line_color_source)) break;
    if (!file.WriteChar(m_arrow_color_source)) break;
    if (!file.WriteChar(m_text_color_source)) break;
    if (!file.WriteColor(m_ext_line_color)) break;
    if (!file.WriteColor(m_dim_line_color)) break;
    if (!file.WriteColor(m_arrow_color)) break;
    if (!file.WriteColor(m_text_color)) break;
    if (!file.WriteChar(m_ext_line_plot_color_source)) break;
    if (!file.WriteChar(m_dim_line_plot_color_source)) break;
    if (!file.WriteChar(m_arrow_plot_color_source)) break;
    if (!file.WriteChar(m_text_plot_color_source)) break;
    if (!file.WriteColor(m_ext_line_plot_color)) break;
    if (!file.WriteColor(m_dim_line_plot_color)) break;
    if (!file.WriteColor(m_arrow_plot_color)) break;
    if (!file.WriteColor(m_text_plot_color)) break;
    if (!file.WriteChar(m_ext_line_plot_weight_source)) break;
    if (!file.WriteChar(m_dim_line_plot_weight_source)) break;
    if (!file.WriteDouble(m_ext_line_plot_weight_mm)) break;
    if (!file.WriteDouble(m_dim_line_plot_weight_mm)) break;

    if (!file.WriteDouble(m_fixed_extension_len)) break;
    if (!file.WriteBool(m_fixed_extension_len_on)) break;

    if (!file.WriteDouble(m_text_rotation)) break;
    if (!file.WriteInt(m_alternate_tolerance_resolution)) break;
    if (!file.WriteDouble(m_tol_textheight_fraction)) break;

    if (!file.WriteBool(m_suppress_arrow1)) break;
    if (!file.WriteBool(m_suppress_arrow2)) break;
    if (!file.WriteInt(m_textmove_leader)) break;
    if (!file.WriteInt(m_arclength_sym)) break;
    if (!file.WriteDouble(m_stack_textheight_fraction)) break;

    u = static_cast<unsigned char>(m_stack_format);
    if (!file.WriteInt(u)) break;
    if (!file.WriteDouble(m_alt_round)) break;
    if (!file.WriteDouble(m_round)) break;
    if (!file.WriteDouble(m_angular_round)) break;

    u = static_cast<unsigned char>(m_alt_zero_suppress);
    if (!file.WriteInt(u)) break;

    u = 0;// OBSOLETE; static_cast<unsigned char>(m_tol_zero_suppress);
    if (!file.WriteInt(u)) break;

    u = static_cast<unsigned char>(m_zero_suppress);
    if (!file.WriteInt(u)) break;
    u = static_cast<unsigned char>(m_ang_zero_suppress);
    if (!file.WriteInt(u)) break;


    if (!file.WriteBool(m_alt_below)) break;

    // true: display alternate text after main text
    u = static_cast<unsigned int>(m_arrow_type_1);
    if (!file.WriteInt(u)) break;
    u = static_cast<unsigned int>(m_arrow_type_2);
    if (!file.WriteInt(u)) break;
    u = static_cast<unsigned int>(m_leader_arrow_type);
    if (!file.WriteInt(u)) break;

    // Dale Lear April 8, 2016 
    //  working on http://mcneel.myjetbrains.com/youtrack/issue/RH-31796
    //
    //   It appears that when this code got changed from something that
    //   wrote 16 bytes to something that wrote 3 uuids, the chunk's
    //   minor version number was not increased and no provision was
    //   made to read files that had the the 16 bytes.
    //
    //   The file "layer_test.3dm" written by Windows Rhino WIP built on October 21, 2015
    //   is an example. See the comment in MYON_DimStyle::Read() for more details
    //   and a fix that allows us to read the earlier files.
    //
    if (!file.WriteUuid(m_arrow_block_id_1)) break;
    if (!file.WriteUuid(m_arrow_block_id_2)) break;
    if (!file.WriteUuid(m_leader_arrow_block_id)) break;

    // End of version chunk 1.0 fields (in most cases - see comment above)

    // June 10, 2016
    // http://mcneel.myjetbrains.com/youtrack/issue/RH-33795
    // chunk version 1.1 information added

    // BEGIN chunk version 1.1 information

    u = 1; // OBSOLETE MYON_DimStyle::leader_content_type::Text
    if (!file.WriteInt(u)) break;

    MYON_INTERNAL_OBSOLETE::V5_vertical_alignment obsolete_text_valign = MYON_INTERNAL_OBSOLETE::V5VerticalAlignmentFromV6VerticalAlignment(m_text_vertical_alignment);
    u = static_cast<unsigned int>(obsolete_text_valign);
    if (!file.WriteInt(u)) break;

    MYON_INTERNAL_OBSOLETE::V5_vertical_alignment obsolete_leader_valign = MYON_INTERNAL_OBSOLETE::V5VerticalAlignmentFromV6VerticalAlignment(m_leader_text_vertical_alignment);
    u = static_cast<unsigned int>(obsolete_leader_valign);
    if (!file.WriteInt(u)) break;


    u = static_cast<unsigned int>(m_leader_content_angle_style);
    if (!file.WriteInt(u)) break;

    u = static_cast<unsigned int>(m_leader_curve_type);
    if (!file.WriteInt(u)) break;

    if (!file.WriteDouble(m_leader_content_angle)) break;
    if (!file.WriteBool(m_leader_has_landing)) break;
    if (!file.WriteDouble(m_leader_landing_length)) break;

    MYON_INTERNAL_OBSOLETE::V5_horizontal_alignment obsolete_text_halign = MYON_INTERNAL_OBSOLETE::V5HorizontalAlignmentFromV6HorizontalAlignment(m_text_horizontal_alignment);
    u = static_cast<unsigned int>(obsolete_text_halign);
    if (!file.WriteInt(u)) break;

    MYON_INTERNAL_OBSOLETE::V5_horizontal_alignment obsolete_leader_halign = MYON_INTERNAL_OBSOLETE::V5HorizontalAlignmentFromV6HorizontalAlignment(m_leader_text_horizontal_alignment);
    u = static_cast<unsigned int>(obsolete_leader_halign);
    if (!file.WriteInt(u)) break;

    if (!file.WriteBool(m_draw_forward)) break;
    if (!file.WriteBool(m_signed_ordinate)) break;

    if (!m_scale_value.Write(file)) break;

    u = static_cast<unsigned int>(this->UnitSystem());
    if (!file.WriteInt(u)) break;

    // END chunk version 1.1 information

    // August 2016 added m_font_characteristics to archive.
    if (!m_font_characteristics.Write(file)) break;

    // END chunk version 1.2 information

    // October 2016 added m_text_mask
    if (!m_text_mask.Write(file)) break;

    // END chunk version 1.3 information

    // Feb 2017 added enum values
    u = static_cast<unsigned int>(m_dim_text_location);
    if (!file.WriteInt(u)) break;

    u = static_cast<unsigned int>(m_dimradial_text_location);
    if (!file.WriteInt(u)) break;

    u = static_cast<unsigned int>(m_text_vertical_alignment);
    if (!file.WriteInt(u)) break;

    u = static_cast<unsigned int>(m_text_horizontal_alignment);
    if (!file.WriteInt(u)) break;

    u = static_cast<unsigned int>(m_leader_text_vertical_alignment);
    if (!file.WriteInt(u)) break;

    u = static_cast<unsigned int>(m_leader_text_horizontal_alignment);
    if (!file.WriteInt(u)) break;

    u = static_cast<unsigned int>(m_text_orientation);
    if (!file.WriteInt(u)) break;

    u = static_cast<unsigned int>(m_leader_text_orientation);
    if (!file.WriteInt(u)) break;

    u = static_cast<unsigned int>(m_dim_text_orientation);
    if (!file.WriteInt(u)) break;

    u = static_cast<unsigned int>(m_dimradial_text_orientation);
    if (!file.WriteInt(u)) break;

    u = static_cast<unsigned int>(m_dim_text_angle_style);
    if (!file.WriteInt(u)) break;

    u = static_cast<unsigned int>(m_dimradial_text_angle_style);
    if (!file.WriteInt(u)) break;

    if (!file.WriteBool(m_text_underlined)) break;

    // END chunk version 1.4 information

    // June 16, 2017 chunk 1.5 information
    // added m_dimension_unitsystem and m_alt_dimension_unitsystem 
    u = static_cast<unsigned int>(dim_length_display_unit);
    if (!file.WriteInt(u)) break;

    u = static_cast<unsigned int>(alt_dim_length_display_unit);
    if (!file.WriteInt(u)) break;

    // END chunk version 1.5 information

    // June 29, 2017 chunk 1.6 added 
    // m_dimension_length_display and m_alternate_dimension_length_display
    u = static_cast<unsigned int>(m_dimension_length_display);
    if (!file.WriteInt(u)) break;
    u = static_cast<unsigned int>(m_alternate_dimension_length_display);
    if (!file.WriteInt(u)) break;
    // END chunk version 1.6 information

    u = static_cast<unsigned int>(m_centermark_style);
    if (!file.WriteInt(u)) break;
    // END chunk version 1.7 information

    if (!file.WriteBool(m_bForceDimLine)) break;

    u = static_cast<unsigned int>(m_TextFit);
    if (!file.WriteInt(u)) break;

    u = static_cast<unsigned int>(m_ArrowFit);
    if (!file.WriteInt(u)) break;
    // END chunk version 1.8 information

    u = static_cast<unsigned int>(m_decimal_separator);
    if (!file.WriteInt(u)) break;
    // END chunk version 1.9 information

    rc = true;
    break;
  }
  if (!file.EndWrite3dmChunk())
    rc = false;

  return rc;
}

bool MYON_DimStyle::Read(
  MYON_BinaryArchive& file // restore definition from binary archive
  )
{
  *this = MYON_DimStyle::Unset;

  int major_version = 0;
  int minor_version = 0;
  if (!file.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK, &major_version, &minor_version))
    return false;

  MYON__UINT32 archive_field_override_parent_count = 0;
  MYON__UINT32 archive_field_override_parent_bits0 = 0;
  MYON__UINT32 archive_field_override_parent_bits1 = 0;
  MYON__UINT32 archive_field_override_parent_bits2 = 0;
  MYON__UINT32 archive_field_override_parent_bits3 = 0;

  bool rc = false;
  for (;;)
  {
    if (1 != major_version)
      break;

    if (!file.ReadModelComponentAttributes(*this, nullptr))
      break;

    unsigned int u;

    if (!file.ReadDouble(&m_extextension)) break;
    if (!file.ReadDouble(&m_extoffset)) break;
    if (!file.ReadDouble(&m_arrowsize)) break;
    if (!file.ReadDouble(&m_leaderarrowsize)) break;
    if (!file.ReadDouble(&m_centermark)) break;
    if (!file.ReadDouble(&m_textgap)) break;
    if (!file.ReadDouble(&m_textheight)) break;

    u = 0;
    if (!file.ReadInt(&u)) break;
    MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode obsolete_dimstyle_textalign = MYON_INTERNAL_OBSOLETE::V5TextDisplayModeFromUnsigned(u);
    switch (obsolete_dimstyle_textalign)
    {
    case MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kHorizontalToScreen:
      m_dim_text_location = MYON_DimStyle::TextLocation::InDimLine;
      m_dimradial_text_location = MYON_DimStyle::TextLocation::InDimLine;
      m_text_orientation = MYON::TextOrientation::InPlane;
      m_leader_text_orientation = MYON::TextOrientation::InView;
      m_dim_text_orientation = MYON::TextOrientation::InView;
      m_dimradial_text_orientation = MYON::TextOrientation::InView;
      m_dim_text_angle_style = MYON_DimStyle::ContentAngleStyle::Horizontal;
      m_dimradial_text_angle_style = MYON_DimStyle::ContentAngleStyle::Horizontal;
      break;
    case MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kHorizontalInCplane:
      m_dim_text_location = MYON_DimStyle::TextLocation::InDimLine;
      m_dimradial_text_location = MYON_DimStyle::TextLocation::InDimLine;
      m_text_orientation = MYON::TextOrientation::InPlane;
      m_leader_text_orientation = MYON::TextOrientation::InPlane;
      m_dim_text_orientation = MYON::TextOrientation::InPlane;
      m_dimradial_text_orientation = MYON::TextOrientation::InPlane;
      m_dim_text_angle_style = MYON_DimStyle::ContentAngleStyle::Horizontal;
      m_dimradial_text_angle_style = MYON_DimStyle::ContentAngleStyle::Horizontal;
      break;
    case MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kNormal:
      m_dim_text_location = MYON_DimStyle::TextLocation::AboveDimLine;
      m_dimradial_text_location = MYON_DimStyle::TextLocation::InDimLine;
      break;
    case MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kAboveLine:
      m_dim_text_location = MYON_DimStyle::TextLocation::AboveDimLine;
      m_dimradial_text_location = MYON_DimStyle::TextLocation::AboveDimLine;
      break;
    case MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kInLine:
      m_dim_text_location = MYON_DimStyle::TextLocation::InDimLine;
      m_dimradial_text_location = MYON_DimStyle::TextLocation::InDimLine;
      break;
    default:
      // DO NO HARM ... use MYON_DimStyle constructor defaults.
      break;
    }


    u = static_cast<unsigned char>(m_angleformat);
    if (!file.ReadInt(&u)) break;
    m_angleformat = MYON_DimStyle::AngleFormatFromUnsigned(u);

    // obsolete dim length format setting
    int obsolete_length_format = 0;
    // (MYON_DimStyle::OBSOLETE_length_format) 0 = decimal, 1 = fractional, 2,3 = fett and inches
    if (!file.ReadInt(&obsolete_length_format)) break;
    if (3 == obsolete_length_format)
      obsolete_length_format = 2;

    if (!file.ReadInt(&m_angleresolution)) break;
    if (!file.ReadInt(&m_lengthresolution)) break;
    int text_style_index = MYON_UNSET_INT_INDEX;
    if (!file.ReadInt(&text_style_index)) break;
    const MYON_TextStyle* text_style = file.ArchiveTextStyleFromArchiveTextStyleIndex(text_style_index);
    if (nullptr != text_style)
    {
      SetFont(text_style->Font());
    }
    
    if (!file.ReadDouble(&m_lengthfactor)) break;

    // Now we have both obsolete_length_format and m_lengthfactor.
    // Try to use this information to m_dimension_length_display.
    if (minor_version <= 5)
    {
      // obsolete_length_format and current m_lengthfactor are V5-ish
      Internal_V6LengthDisplayFromV5LengthFormatAndFactor(
        file.Archive3dmSettings().m_ModelUnitsAndTolerances.m_unit_system.UnitSystem(),
        obsolete_length_format, 
        m_lengthfactor,
        m_dimension_length_display,
        m_lengthfactor
      );
      m_lengthfactor = 1.0;
    }
    else
    {
      MYON_Internal_FixBogusDimStyleLengthFactor(file, m_lengthfactor);
    }

    if (!file.ReadBool(&m_bAlternate)) break;
    if (!file.ReadDouble(&m_alternate_lengthfactor)) break;

    // obsolete alternate dim length format setting
    int obsolete_alternate_length_format = 0;
    // (MYON_DimStyle::OBSOLETE_length_format) 0 = decimal, 1 = fractional, 2,3 = fett and inches
    if (!file.ReadInt(&obsolete_alternate_length_format)) break;
    if (3 == obsolete_alternate_length_format)
      obsolete_alternate_length_format = 2;

    // Now we have both obsolete_alternate_length_format and m_alternate_lengthfactor
    // Try to use this information to m_alternate_dimension_length_display.
    if (minor_version <= 5)
    {
      // obsolete_alternate_length_format and current m_alternate_lengthfactor are V5-ish
      Internal_V6LengthDisplayFromV5LengthFormatAndFactor(
        file.Archive3dmSettings().m_ModelUnitsAndTolerances.m_unit_system.UnitSystem(),
        obsolete_alternate_length_format, 
        m_alternate_lengthfactor,
        m_alternate_dimension_length_display,
        m_alternate_lengthfactor
      );
      m_alternate_lengthfactor = 1.0;
    }
    else
    {
      MYON_Internal_FixBogusDimStyleLengthFactor(file, m_alternate_lengthfactor);
    }

    if (!file.ReadInt(&m_alternate_lengthresolution)) break;
    if (!file.ReadString(m_prefix)) break;
    if (!file.ReadString(m_suffix)) break;
    if (!file.ReadString(m_alternate_prefix)) break;
    if (!file.ReadString(m_alternate_suffix)) break;
    if (!file.ReadDouble(&m_dimextension)) break;
    if (!file.ReadBool(&m_bSuppressExtension1)) break;
    if (!file.ReadBool(&m_bSuppressExtension2)) break;

    MYON_UUID parent_dimstyle_id = ParentId();
    if (!file.ReadUuid(parent_dimstyle_id)) break;
    SetParentId(parent_dimstyle_id);

    unsigned int unused_legacy_value = 0;
    if (!file.ReadInt(&unused_legacy_value)) break;

    bool bHasPropertyOverrideBools = false;
    if (!file.ReadBool(&bHasPropertyOverrideBools)) break;
    if (bHasPropertyOverrideBools)
    {
      MYON_SimpleArray<bool> property_override_bool;
      if (!file.ReadArray(property_override_bool)) break;
      unsigned int overridescount = property_override_bool.UnsignedCount();
      if (overridescount > static_cast<unsigned int>(MYON_DimStyle::field::Count))
      {
        overridescount = static_cast<unsigned int>(MYON_DimStyle::field::Count);
        property_override_bool.SetCount(overridescount);
      }
      for (unsigned int i = static_cast<unsigned int>(MYON_DimStyle::field::Index)+1; i < overridescount; i++)
      {
        SetFieldOverride(MYON_DimStyle::FieldFromUnsigned(i), property_override_bool[i]);
      }
    }

    // The reading below calls some Set functions which may change the m_field_override_parent_* values.
    // Save what came from the archive so we can restore it.
    archive_field_override_parent_count = m_field_override_parent_count;
    archive_field_override_parent_bits0 = m_field_override_parent_bits0;
    archive_field_override_parent_bits1 = m_field_override_parent_bits1;
    archive_field_override_parent_bits2 = m_field_override_parent_bits2;
    archive_field_override_parent_bits3 = m_field_override_parent_bits3;
    
    u = static_cast<unsigned char>(m_tolerance_format);
    if (!file.ReadInt(&u)) break;
    m_tolerance_format = MYON_DimStyle::ToleranceFormatFromUnsigned(u);

    if (!file.ReadInt(&m_tolerance_resolution)) break;
    if (!file.ReadDouble(&m_tolerance_upper_value)) break;   // or both upper and lower in symmetrical style
    if (!file.ReadDouble(&m_tolerance_lower_value)) break;
    if (!file.ReadDouble(&m_tolerance_height_scale)) break;  // relative to the main dimension text

    if (!file.ReadDouble(&m_baseline_spacing)) break;

    // The text mask settings used to be a collection of several member variables
    // on MYON_DimStyle and are now in an MYON_TextMask class and a m_text_mask on MYON_DimStyle.
    // These values are written here so the file format is not broken and 
    // older versions of Rhino can read newer file.
    bool b = DrawTextMask();
    if (!file.ReadBool(&b)) break;
    SetDrawTextMask(b);
    u = (unsigned int)(static_cast<unsigned char>(MaskFillType()));
    if (!file.ReadInt(&u)) break;
    SetMaskFillType(MYON_TextMask::MaskTypeFromUnsigned(u));
    MYON_Color c = MaskColor();
    if (!file.ReadColor(c)) break;
    SetMaskColor(c);

    double dimscale = DimScale();
    if (!file.ReadDouble(&dimscale)) break;
    if ( dimscale > 0.0 && dimscale != DimScale() )
      SetDimScale(dimscale);
    if (!file.ReadInt(&m_dimscale_source)) break;

    if (!file.ReadUuid(m_source_dimstyle)) break;
    if (!file.ReadChar(&m_ext_line_color_source)) break;
    if (!file.ReadChar(&m_dim_line_color_source)) break;
    if (!file.ReadChar(&m_arrow_color_source)) break;
    if (!file.ReadChar(&m_text_color_source)) break;
    if (!file.ReadColor(m_ext_line_color)) break;
    if (!file.ReadColor(m_dim_line_color)) break;
    if (!file.ReadColor(m_arrow_color)) break;
    if (!file.ReadColor(m_text_color)) break;
    if (!file.ReadChar(&m_ext_line_plot_color_source)) break;
    if (!file.ReadChar(&m_dim_line_plot_color_source)) break;
    if (!file.ReadChar(&m_arrow_plot_color_source)) break;
    if (!file.ReadChar(&m_text_plot_color_source)) break;
    if (!file.ReadColor(m_ext_line_plot_color)) break;
    if (!file.ReadColor(m_dim_line_plot_color)) break;
    if (!file.ReadColor(m_arrow_plot_color)) break;
    if (!file.ReadColor(m_text_plot_color)) break;
    if (!file.ReadChar(&m_ext_line_plot_weight_source)) break;
    if (!file.ReadChar(&m_dim_line_plot_weight_source)) break;
    if (!file.ReadDouble(&m_ext_line_plot_weight_mm)) break;
    if (!file.ReadDouble(&m_dim_line_plot_weight_mm)) break;

    if (!file.ReadDouble(&m_fixed_extension_len)) break;
    if (!file.ReadBool(&m_fixed_extension_len_on)) break;

    if (!file.ReadDouble(&m_text_rotation)) break;
    if (!file.ReadInt(&m_alternate_tolerance_resolution)) break;
    if (!file.ReadDouble(&m_tol_textheight_fraction)) break;

    if (!file.ReadBool(&m_suppress_arrow1)) break;
    if (!file.ReadBool(&m_suppress_arrow2)) break;
    if (!file.ReadInt(&m_textmove_leader)) break;
    if (!file.ReadInt(&m_arclength_sym)) break;
    if (!file.ReadDouble(&m_stack_textheight_fraction)) break;

    u = static_cast<unsigned char>(m_stack_format);
    if (!file.ReadInt(&u)) break;
    m_stack_format = MYON_DimStyle::StackFormatFromUnsigned(u);

    if (!file.ReadDouble(&m_alt_round)) break;
    if (!file.ReadDouble(&m_round)) break;
    if (!file.ReadDouble(&m_angular_round)) break;

    u = static_cast<unsigned char>(m_alt_zero_suppress);
    if (!file.ReadInt(&u)) break;
    m_alt_zero_suppress = MYON_DimStyle::ZeroSuppressFromUnsigned(u);

    u = 0;
    if (!file.ReadInt(&u)) break; // read obsolete value and throw it away
    // OBSOLETE // m_tol_zero_suppress = MYON_DimStyle::ZeroSuppressFromUnsigned(u);

    u = static_cast<unsigned char>(m_zero_suppress);
    if (!file.ReadInt(&u)) break;
    m_zero_suppress = MYON_DimStyle::ZeroSuppressFromUnsigned(u);

    u = static_cast<unsigned char>(m_ang_zero_suppress);
    if (!file.ReadInt(&u)) break;
    m_ang_zero_suppress = MYON_DimStyle::ZeroSuppressFromUnsigned(u);

    if (!file.ReadBool(&m_alt_below)) break;

    // true: display alternate text after main text
    u = static_cast<unsigned int>(m_arrow_type_1);
    if (!file.ReadInt(&u)) break;
    m_arrow_type_1 = MYON_Arrowhead::ArrowTypeFromUnsigned(u);

    u = static_cast<unsigned int>(m_arrow_type_2);
    if (!file.ReadInt(&u)) break;
    m_arrow_type_2 = MYON_Arrowhead::ArrowTypeFromUnsigned(u);

    u = static_cast<unsigned int>(m_leader_arrow_type);
    if (!file.ReadInt(&u)) break;
    m_leader_arrow_type = MYON_Arrowhead::ArrowTypeFromUnsigned(u);

    if (minor_version <= 0)
    {
      // old files do not contain unit system information
      if (false == UnitSystemIsSet())
        SetUnitSystemFromContext(true,file.Archive3dmSettings().m_ModelUnitsAndTolerances.m_unit_system.UnitSystem(),MYON::LengthUnitSystem::Unset);

      if (60 == file.Archive3dmVersion() && file.ArchiveOpenNURBSVersion() <= 2348833437)
      {
        // Dale Lear April 8, 2016 
        //  working on http://mcneel.myjetbrains.com/youtrack/issue/RH-31796
        //
        //   It appears that when this code got changed from something that
        //   read 16 bytes to something that read 3 uuids, the chunk's
        //   minor version number was not increased and no provision was
        //   made to read files that had the the 16 bytes.
        //
        //   Unfortunately, there are files that do not contain 3 uuids and do
        //   contain 16 bytes and these files need to be read.
        //
        //   The file "layer_test.3dm" written by a Windows Rhino WIP built on October 21, 2015
        //   is an example. This file contains everything up to the m_leader_arrow_type unsigned int
        //   but does not contain these uuids. 
        //
        //   The version of opennurbs that wrote that file was 2348833437. So,
        //   I have to add this check and stop reading here to permit files 
        //   written before the the date the m_arrow_block_id_1 IO code was shipped (date unknown)
        //   to read. We know this date is sometime after October 21, 2015 and some version
        //   of opennurbs after version 2348833437.
        rc = true;
        break;
      }
    }

    if (!file.ReadUuid(m_arrow_block_id_1)) break;
    if (!file.ReadUuid(m_arrow_block_id_2)) break;
    if (!file.ReadUuid(m_leader_arrow_block_id)) break;
    // End of version chunk 1.0 fields (in most cases)

    if (minor_version <= 0)
    {
      rc = true;
      break;
    }

    // June 10, 2016
    // http://mcneel.myjetbrains.com/youtrack/issue/RH-33795
    // chunk version 1.1 information added

    // BEGIN chunk version 1.1 information

    // OBSOLETE leader content type
    u = 0;
    if (!file.ReadInt(&u)) break;

    u = 0;
    if (!file.ReadInt(&u)) break;
    MYON_INTERNAL_OBSOLETE::V5_vertical_alignment obsolete_text_valign = MYON_INTERNAL_OBSOLETE::V5VerticalAlignmentFromUnsigned(u);
    m_text_vertical_alignment = MYON_INTERNAL_OBSOLETE::V6VerticalAlignmentFromV5VerticalAlignment(obsolete_text_valign);

    u = 0;
    if (!file.ReadInt(&u)) break;
    MYON_INTERNAL_OBSOLETE::V5_vertical_alignment obsolete_leader_valign = MYON_INTERNAL_OBSOLETE::V5VerticalAlignmentFromUnsigned(u);
    m_leader_text_vertical_alignment = MYON_INTERNAL_OBSOLETE::V6VerticalAlignmentFromV5VerticalAlignment(obsolete_leader_valign);

    u = static_cast<unsigned int>(m_leader_content_angle_style);
    if (!file.ReadInt(&u)) break;
    m_leader_content_angle_style = ContentAngleStyleFromUnsigned(u);

    u = static_cast<unsigned int>(m_leader_curve_type);
    if (!file.ReadInt(&u)) break;
    m_leader_curve_type = LeaderCurveTypeFromUnsigned(u);

    if (!file.ReadDouble(&m_leader_content_angle)) break;
    if (!file.ReadBool(&m_leader_has_landing)) break;
    if (!file.ReadDouble(&m_leader_landing_length)) break;
    
    u = 0;
    if (!file.ReadInt(&u)) break;
    MYON_INTERNAL_OBSOLETE::V5_horizontal_alignment obsolete_text_halign = MYON_INTERNAL_OBSOLETE::V5HorizontalAlignmentFromUnsigned(u);
    m_text_horizontal_alignment = MYON_INTERNAL_OBSOLETE::V6HorizontalAlignmentFromV5HorizontalAlignment(obsolete_text_halign);

    u = 0;
    if (!file.ReadInt(&u)) break;
    MYON_INTERNAL_OBSOLETE::V5_horizontal_alignment obsolete_leader_halign = MYON_INTERNAL_OBSOLETE::V5HorizontalAlignmentFromUnsigned(u);
    m_leader_text_horizontal_alignment = MYON_INTERNAL_OBSOLETE::V6HorizontalAlignmentFromV5HorizontalAlignment(obsolete_leader_halign);

    if (!file.ReadBool(&m_draw_forward)) break;
    if (!file.ReadBool(&m_signed_ordinate)) break;

    if (!m_scale_value.Read(file)) break;

    u = static_cast<unsigned int>(this->UnitSystem());
    if (!file.ReadInt(&u)) break;
    m_dimstyle_unitsystem = MYON::LengthUnitSystemFromUnsigned(u);
    if (MYON::LengthUnitSystem::Unset == m_dimstyle_unitsystem || MYON::LengthUnitSystem::CustomUnits == m_dimstyle_unitsystem)
      m_dimstyle_unitsystem = MYON::LengthUnitSystem::None;
    
    // END chunk version 1.1 information
    if (minor_version <= 1)
    {
      rc = true;
      break;
    }

    // August 2016 - added m_font_characteristics
    if (!m_font_characteristics.Read(file)) break;
    m_managed_font = m_font_characteristics.ManagedFont();

    // END chunk version 1.2 information
    if (minor_version <= 2)
    {
      rc = true;
      break;
    }

    if (!m_text_mask.Read(file)) break;

    // END chunk version 1.3 information
    if (minor_version <= 3)
    {
      rc = true;
      break;
    }

    // Feb 2017 added enum values
    u = static_cast<unsigned int>(m_dim_text_location);
    if (!file.ReadInt(&u)) break;
    m_dim_text_location = MYON_DimStyle::TextLocationFromUnsigned(u);

    u = static_cast<unsigned int>(m_dimradial_text_location);
    if (!file.ReadInt(&u)) break;
    m_dimradial_text_location = MYON_DimStyle::TextLocationFromUnsigned(u);


    u = static_cast<unsigned int>(m_text_vertical_alignment);
    if (!file.ReadInt(&u)) break;
    m_text_vertical_alignment = MYON::TextVerticalAlignmentFromUnsigned(u);

    u = static_cast<unsigned int>(m_text_horizontal_alignment);
    if (!file.ReadInt(&u)) break;
    m_text_horizontal_alignment = MYON::TextHorizontalAlignmentFromUnsigned(u);

    u = static_cast<unsigned int>(m_leader_text_vertical_alignment);
    if (!file.ReadInt(&u)) break;
    m_leader_text_vertical_alignment = MYON::TextVerticalAlignmentFromUnsigned(u);

    u = static_cast<unsigned int>(m_leader_text_horizontal_alignment);
    if (!file.ReadInt(&u)) break;
    m_leader_text_horizontal_alignment = MYON::TextHorizontalAlignmentFromUnsigned(u);

    u = static_cast<unsigned int>(m_text_orientation);
    if (!file.ReadInt(&u)) break;
    m_text_orientation = MYON::TextOrientationFromUnsigned(u);

    u = static_cast<unsigned int>(m_leader_text_orientation);
    if (!file.ReadInt(&u)) break;
    m_leader_text_orientation = MYON::TextOrientationFromUnsigned(u);

    u = static_cast<unsigned int>(m_dim_text_orientation);
    if (!file.ReadInt(&u)) break;
    m_dim_text_orientation = MYON::TextOrientationFromUnsigned(u);

    u = static_cast<unsigned int>(m_dimradial_text_orientation);
    if (!file.ReadInt(&u)) break;
    m_dimradial_text_orientation = MYON::TextOrientationFromUnsigned(u);

    u = static_cast<unsigned int>(m_dim_text_angle_style);
    if (!file.ReadInt(&u)) break;
    m_dim_text_angle_style = MYON_DimStyle::ContentAngleStyleFromUnsigned(u);

    u = static_cast<unsigned int>(m_dimradial_text_angle_style);
    if (!file.ReadInt(&u)) break;
    m_dimradial_text_angle_style = MYON_DimStyle::ContentAngleStyleFromUnsigned(u);

    if (!file.ReadBool(&m_text_underlined)) break;

    // END chunk version 1.4 information
    if (minor_version <= 4)
    {
      rc = true;
      break;
    }
    // June 16, 2017 chunk 1.5 information
    // added m_dimension_unitsystem and m_alt_dimension_unitsystem 

    //MYON::LengthUnitSystem obsolete_dim_length_us = MYON::LengthUnitSystem::None;
    //u = static_cast<unsigned int>(obsolete_dim_length_us);
    u = 0;
    if (!file.ReadInt(&u)) break;
    //obsolete_dim_length_us = MYON::LengthUnitSystemFromUnsigned(u);


    //MYON::LengthUnitSystem obsolete_alt_dim_length_us = MYON::LengthUnitSystem::None;
    //u = static_cast<unsigned int>(obsolete_alt_dim_length_us);
    u = 0;
    if (!file.ReadInt(&u)) break;
    //obsolete_alt_dim_length_us = MYON::LengthUnitSystemFromUnsigned(u);

    // END chunk version 1.5 information
    if (minor_version <= 5)
    {
      rc = true;
      break;
    }
    u = static_cast<unsigned int>(m_dimension_length_display);
    if (!file.ReadInt(&u)) break;
    m_dimension_length_display = MYON_DimStyle::LengthDisplayFromUnsigned(u);
    
    u = static_cast<unsigned int>(m_dimension_length_display);
    if (!file.ReadInt(&u)) break;
    m_alternate_dimension_length_display = MYON_DimStyle::LengthDisplayFromUnsigned(u);
    // END chunk version 1.6 information
    if (minor_version <= 6)
    {
      rc = true;
      break;
    }

    u = static_cast<unsigned int>(m_centermark_style);
    if (!file.ReadInt(&u)) break;
    m_centermark_style = MYON_DimStyle::CentermarkStyleFromUnsigned(u);
    // END chunk version 1.7 information

    if (minor_version <= 7)
    {
      rc = true;
      break;
    }

    if (!file.ReadBool(&m_bForceDimLine)) break;

    u = static_cast<unsigned int>(m_TextFit);
    if (!file.ReadInt(&u)) break;
    m_TextFit = MYON_DimStyle::TextFitFromUnsigned(u);

    u = static_cast<unsigned int>(m_ArrowFit);
    if (!file.ReadInt(&u)) break;
    m_ArrowFit = MYON_DimStyle::ArrowFitFromUnsigned(u);
    // END chunk version 1.8 information

    if (minor_version <= 8)
    {
      rc = true;
      break;
    }
    u = static_cast<unsigned int>(m_decimal_separator);
    if (!file.ReadInt(&u)) break;
    m_decimal_separator = (wchar_t)u;
    // END chunk version 1.9 information

    rc = true;
    break;
  }
  // Dale Lear April 8, 2016 
  //  working on http://mcneel.myjetbrains.com/youtrack/issue/RH-31796
  //  bSupressPartiallyReadChunkWarning suppresses a partially read chunk warning
  //  of skipping 16 bytes.

  const bool bSupressPartiallyReadChunkWarning
    = 60 == file.Archive3dmVersion()
    && file.ArchiveOpenNURBSVersion() <= 2348833437
    && 1 == major_version
    && 0 == minor_version
    ;
  if (!file.EndRead3dmChunk(bSupressPartiallyReadChunkWarning))
    rc = false;

  if (nullptr == m_managed_font)
  {
    m_font_characteristics = MYON_Font::Default;
    m_managed_font = &MYON_Font::Default;
  }

  // Make sure zero suppression settings are valid
  if (false == MYON_DimStyle::ZeroSuppressMatchesLengthDisplay(m_zero_suppress, m_dimension_length_display ))
    m_zero_suppress = MYON_DimStyle::suppress_zero::None;

  if (false == MYON_DimStyle::ZeroSuppressMatchesLengthDisplay(m_alt_zero_suppress, m_alternate_dimension_length_display))
    m_alt_zero_suppress = MYON_DimStyle::suppress_zero::None;

  // Any decimal format will work to test m_ang_zero_suppress.
  if (false == MYON_DimStyle::ZeroSuppressMatchesLengthDisplay(m_ang_zero_suppress, MYON_DimStyle::LengthDisplay::ModelUnits))
    m_ang_zero_suppress = MYON_DimStyle::suppress_zero::None;

  m_field_override_parent_count = archive_field_override_parent_count;
  m_field_override_parent_bits0 = archive_field_override_parent_bits0;
  m_field_override_parent_bits1 = archive_field_override_parent_bits1;
  m_field_override_parent_bits2 = archive_field_override_parent_bits2;
  m_field_override_parent_bits3 = archive_field_override_parent_bits3;
  
  return rc;
}

void MYON_DimStyle::EmergencyDestroy()
{
  m_prefix.EmergencyDestroy();
  m_suffix.EmergencyDestroy();
  m_alternate_prefix.EmergencyDestroy();
  m_alternate_suffix.EmergencyDestroy();
}


//////////////////////////////////////////////////////////////////////
//
// Interface

void MYON_DimStyle::Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field field_id)
{
  // When a dimstyle is an override candidate, any call to a Set...() function sets that field as overridden.
  if (this->IsOverrideDimStyleCandidate(ParentId(), false))
    this->SetFieldOverride(field_id, true);
}

bool MYON_DimStyle::Internal_SetBoolMember(
  MYON_DimStyle::field field_id,
  bool value,
  bool& class_member
)
{
  bool bValueChanged = false;
  const bool b = value ? true : false;
  if (b != class_member)
  {
    class_member = b;
    Internal_ContentChange();
    bValueChanged = true;
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(field_id);
  return bValueChanged;
}

bool MYON_DimStyle::Internal_SetUnsignedCharMember(
  MYON_DimStyle::field field_id,
  unsigned char value,
  unsigned char& class_member
)
{
  bool bValueChanged = false;
  if (value != class_member)
  {
    class_member = value;
    Internal_ContentChange();
    bValueChanged = true;
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(field_id);
  return bValueChanged;
}

bool MYON_DimStyle::Internal_SetIntMember(
  MYON_DimStyle::field field_id,
  int value,
  int& class_member
)
{
  bool bValueChanged = false;
  if (value != class_member)
  {
    class_member = value;
    Internal_ContentChange();
    bValueChanged = true;
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(field_id);
  return bValueChanged;
}


bool MYON_DimStyle::Internal_SetColorMember(
  MYON_DimStyle::field field_id,
  MYON_Color value,
  MYON_Color& class_member
)
{
  bool bValueChanged = false;
  if (value != class_member)
  {
    class_member = value;
    Internal_ContentChange();
    bValueChanged = true;
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(field_id);
  return bValueChanged;
}

bool MYON_DimStyle::Internal_SetDoubleMember(
  MYON_DimStyle::field field_id,
  double value,
  double& class_member
)
{
  bool bValueChanged = false;
  if (MYON_IsValid(value))
  {
    // !(value == class_member) is used instead of (value != class_member)
    // so that when class_member = NaN, its value can be changed.
    if ( !(value == class_member) )
    {
      class_member = value;
      Internal_ContentChange();
      bValueChanged = true;
    }
    Internal_SetOverrideDimStyleCandidateFieldOverride(field_id);
  }
  return bValueChanged;
}

bool MYON_DimStyle::Internal_SetIdMember(
  MYON_DimStyle::field field_id,
  MYON_UUID value,
  MYON_UUID& class_member
)
{
  bool bValueChanged = false;
  if (value != class_member)
  {
    class_member = value;
    Internal_ContentChange();
    bValueChanged = true;
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(field_id);
  return bValueChanged;
}

bool MYON_DimStyle::Internal_SetStringMember(
  MYON_DimStyle::field field_id,
  const wchar_t* value,
  MYON_wString& class_member
)
{
  if (false == class_member.IsValid(true))
  {
    // just in case value pointed to the bogus string array.
    value = L"";
  }
  bool bValueChanged = false;
  if (false == class_member.EqualOrdinal(value,false) )
  {
    class_member = value;
    Internal_ContentChange();
    bValueChanged = true;
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(field_id);
  return bValueChanged;
}


double MYON_DimStyle::ExtExtension() const
{
  return m_extextension;
}

void MYON_DimStyle::SetExtExtension(const double e)
{
  double x = fabs(e);
  Internal_SetDoubleMember(MYON_DimStyle::field::ExtensionLineExtension, x, m_extextension);
}

double MYON_DimStyle::ExtOffset() const
{
  return m_extoffset;
}

void MYON_DimStyle::SetExtOffset(const double e)
{
  Internal_SetDoubleMember(MYON_DimStyle::field::ExtensionLineOffset, e, m_extoffset);
}

double MYON_DimStyle::ArrowSize() const
{
  return m_arrowsize;
}

void MYON_DimStyle::SetArrowSize(const double e)
{
  Internal_SetDoubleMember(MYON_DimStyle::field::Arrowsize, e, m_arrowsize);
}

double MYON_DimStyle::LeaderArrowSize() const
{
  return m_leaderarrowsize;
}

void MYON_DimStyle::SetLeaderArrowSize(const double e)
{
  Internal_SetDoubleMember( MYON_DimStyle::field::LeaderArrowsize, e, m_leaderarrowsize);
}

double MYON_DimStyle::CenterMark() const
{
  return m_centermark;
}

void MYON_DimStyle::SetCenterMark(const double e)
{
  Internal_SetDoubleMember(MYON_DimStyle::field::Centermark, e, m_centermark);
}

MYON_DimStyle::centermark_style MYON_DimStyle::CenterMarkStyle() const
{
  return m_centermark_style;
}

void MYON_DimStyle::SetCenterMarkStyle(MYON_DimStyle::centermark_style style)
{
  if (m_centermark_style != style)
  {
    m_centermark_style = style;
    Internal_ContentChange();
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::CentermarkStyle);
}

MYON_DimStyle::TextLocation MYON_DimStyle::DimTextLocation() const
{
  return m_dim_text_location;
}

void MYON_DimStyle::SetDimTextLocation(MYON_DimStyle::TextLocation a)
{
  if (m_dim_text_location != a)
  {
    m_dim_text_location = a;
    Internal_TextPositionPropertiesChange();
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::DimTextLocation);
}


MYON_DimStyle::TextLocation MYON_DimStyle::DimRadialTextLocation() const
{
  return m_dimradial_text_location;
}

void MYON_DimStyle::SetDimRadialTextLocation(MYON_DimStyle::TextLocation a)
{
  if (m_dimradial_text_location != a)
  {
    m_dimradial_text_location = a;
    Internal_TextPositionPropertiesChange();
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::DimRadialTextLocation);
}

MYON_DimStyle::angle_format MYON_DimStyle::AngleFormat() const
{
  return m_angleformat;
}

void MYON_DimStyle::SetAngleFormat(MYON_DimStyle::angle_format f)
{
  if (m_angleformat != f)
  {
    m_angleformat = f;
    Internal_ContentChange();
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::AngleFormat);
}

int MYON_DimStyle::LengthResolution() const
{
  return m_lengthresolution;
}

void MYON_DimStyle::SetLengthResolution(int r)
{
  if (r >= 0 && r < 16)
    Internal_SetIntMember(MYON_DimStyle::field::LengthResolution, r, m_lengthresolution);
}

int MYON_DimStyle::AngleResolution() const
{
  return m_angleresolution;
}

void MYON_DimStyle::SetAngleResolution(int r)
{
  if (r >= 0 && r < 16)
    Internal_SetIntMember(MYON_DimStyle::field::AngleResolution, r, m_angleresolution);
}

const MYON_Font& MYON_DimStyle::Font() const
{
  return (nullptr != m_managed_font) ? *m_managed_font : MYON_Font::Default;
}

const MYON_Font& MYON_DimStyle::ParentDimStyleFont() const
{
  // If this dimstyle has a parent dimstyle and this dimstyle's font overrides the parent dimstyle's font,
  // then the parent dimstyle's font is returned. Otherwise this dimstyle's font is returned.
  return
    (nullptr != m_parent_dimstyle_managed_font && false == (MYON_nil_uuid == ParentId()) && IsFieldOverride(MYON_DimStyle::field::Font) ) 
    ? *m_parent_dimstyle_managed_font 
    : Font();
}

const bool MYON_DimStyle::FontSubstituted() const
{
  return
    (nullptr == m_managed_font)
    || (0 != MYON_Font::CompareFontCharacteristics(*m_managed_font, m_font_characteristics))
    || (m_managed_font->Description() != m_font_characteristics.Description())
    ;
}

const MYON_Font& MYON_DimStyle::FontCharacteristics() const
{
  return m_font_characteristics;
}

void MYON_DimStyle::SetFont(
  const MYON_Font& font_characteristics
)
{
  const MYON_Font* managed_font = font_characteristics.ManagedFont();
  if (nullptr == managed_font)
  {
    MYON_ERROR("MYON_Font::GetManagedFont(font_characteristics) failed.");
    return;
  }

  const bool bManagedFontChanged = (m_managed_font != managed_font);

  const bool bFontChanged
    = bManagedFontChanged
    || (0 != MYON_Font::CompareFontCharacteristics(font_characteristics, m_font_characteristics))
    || m_font_characteristics.Description() != font_characteristics.Description()
    ;

  // copy font_characteristics unconditionally in case compare missed some detail.
  m_font_characteristics = font_characteristics;

  if (bFontChanged)
  {
    m_managed_font = managed_font;
    Internal_TextPositionPropertiesChange();
  }

  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::Font);
}

MYON_DimStyle* MYON_DimStyle::CreateFromFont(
  const MYON_Font* font_characteristics,
  double model_view_text_scale,
  const MYON_DimStyle* dimstyle_settings,
  const class MYON_ComponentManifest* manifest,
  MYON_DimStyle* destination
)
{
  if (nullptr == font_characteristics)
    font_characteristics = &MYON_Font::Default;

  if (nullptr == dimstyle_settings)
  {
    dimstyle_settings = &MYON_DimStyle::Default;
  }
  if (nullptr == destination)
    destination = new MYON_DimStyle(*dimstyle_settings);
  else 
  {
    if (destination != dimstyle_settings)
      *destination = *dimstyle_settings;
  }

  destination->ClearModelComponentAttributes(MYON_ModelComponent::Attributes::AllAttributes);

  destination->SetFont(*font_characteristics);

  if ( model_view_text_scale > 0.0 && MYON_IsValid(model_view_text_scale))
    destination->SetDimScale(model_view_text_scale);

  // Dale Lear RH-63824 May 3, 2021
  // It is critical that bIncludeNotOnDevice be set to false.
  // Otherwise missing fonts will have a description beginning with "[Not on device]"
  // and square brackets are not permitted in names.
  // This code is inventing a Rhino 6/7 dimstyle name from a V4 text style.
  // The text style names were unreliable in V4 and we've used the font
  // description as a proxy for years now.
  const bool bIncludeNotOnDevice = false;

  const MYON_wString font_description = font_characteristics->Description(MYON_Font::NameLocale::LocalizedFirst, MYON_wString::HyphenMinus, MYON_wString::Space, true, bIncludeNotOnDevice);
  if (font_description.IsNotEmpty())
  {
    const MYON_wString name
      = (nullptr != manifest)
      ? manifest->UnusedName(MYON_ModelComponent::Type::DimStyle, MYON_nil_uuid, font_description, nullptr, nullptr, 0, nullptr)
      : font_description;
    destination->SetName(name);
  }

  // The calls to destination->SetFont() and destination->SetDimScale()
  // set the overrides.
  destination->ClearAllFieldOverrides();

  return destination;
}

const class MYON_SHA1_Hash MYON_DimStyle::TextPositionPropertiesHash() const
{
  // sha1 hash of properties that have any possible effect on annotation text appearance, size, shape
  if (m_text_position_properties_hash.IsEmptyContentHash())
  {
    MYON_SHA1 sha1;
    
    const MYON_SHA1_Hash font_hash
      = nullptr != m_managed_font
      ? m_managed_font->FontCharacteristicsHash()
      : m_font_characteristics.FontCharacteristicsHash();
    sha1.AccumulateSubHash(font_hash);
    sha1.AccumulateDouble(DimScale());
    sha1.AccumulateDouble(m_extextension);
    sha1.AccumulateDouble(m_extoffset);
    sha1.AccumulateDouble(m_textheight);
    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_dim_text_location));
    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_dimradial_text_location));
    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_angleformat));

    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_dimension_length_display));
    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_alternate_dimension_length_display));

    sha1.AccumulateInteger32(m_angleresolution);
    sha1.AccumulateInteger32(m_lengthresolution);
    sha1.AccumulateDouble(m_lengthfactor);
    sha1.AccumulateBool(m_bAlternate);
    sha1.AccumulateDouble(m_alternate_lengthfactor);
    sha1.AccumulateInteger32(m_alternate_lengthresolution);

    m_prefix.IsValid(true);
    sha1.AccumulateString(m_prefix);

    m_suffix.IsValid(true);
    sha1.AccumulateString(m_suffix);

    m_alternate_prefix.IsValid(true);
    sha1.AccumulateString(m_alternate_prefix);

    m_alternate_suffix.IsValid(true);
    sha1.AccumulateString(m_alternate_suffix);

    sha1.AccumulateDouble(m_dimextension);
    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_tolerance_format));
    sha1.AccumulateInteger32(m_tolerance_resolution);
    sha1.AccumulateDouble(m_tolerance_upper_value);
    sha1.AccumulateDouble(m_tolerance_lower_value);
    sha1.AccumulateDouble(m_tolerance_height_scale);
    sha1.AccumulateDouble(m_text_rotation);
    sha1.AccumulateInteger32(m_alternate_tolerance_resolution);
    sha1.AccumulateDouble(m_tol_textheight_fraction);
    sha1.AccumulateInteger32(m_textmove_leader);
    sha1.AccumulateInteger32(m_arclength_sym);
    sha1.AccumulateDouble(m_stack_textheight_fraction);
    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_stack_format));
    sha1.AccumulateDouble(m_alt_round);
    sha1.AccumulateDouble(m_round);
    sha1.AccumulateDouble(m_angular_round);
    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_alt_zero_suppress));
    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_zero_suppress));
    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_ang_zero_suppress));
    sha1.AccumulateBool(m_alt_below);
    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_text_vertical_alignment));
    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_text_horizontal_alignment));
    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_leader_text_vertical_alignment));
    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_leader_text_horizontal_alignment));
    sha1.AccumulateDouble(m_scale_value.LeftToRightScale());

    sha1.AccumulateUnsigned32(static_cast<unsigned int>(this->UnitSystem()));

    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_text_orientation));
    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_leader_text_orientation));
    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_dim_text_orientation));
    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_dimradial_text_orientation));
    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_dim_text_angle_style));
    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_dimradial_text_angle_style));
    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_text_underlined));
    
    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_ArrowFit));
    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_TextFit));

    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_decimal_separator));

    // Save hash in mutable m_text_position_properties_hash
    m_text_position_properties_hash = sha1.Hash();
  }
  return m_text_position_properties_hash;
}

const class MYON_SHA1_Hash& MYON_DimStyle::ContentHash() const
{
  if (m_content_hash.IsEmptyContentHash())
  {
    MYON_SHA1 sha1;

    MYON_SHA1_Hash text_position_hash = TextPositionPropertiesHash();
    sha1.AccumulateSubHash(text_position_hash);

    sha1.AccumulateDouble(m_arrowsize);
    sha1.AccumulateDouble(m_leaderarrowsize);
    sha1.AccumulateDouble(m_centermark);
    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_centermark_style));
    sha1.AccumulateDouble(m_textgap);

    sha1.AccumulateBool(m_bSuppressExtension1);
    sha1.AccumulateBool(m_bSuppressExtension2);
    sha1.AccumulateUnsigned32(m_field_override_parent_count);
    sha1.AccumulateUnsigned32(m_field_override_parent_bits0);
    sha1.AccumulateUnsigned32(m_field_override_parent_bits1);
    sha1.AccumulateUnsigned32(m_field_override_parent_bits2);
    sha1.AccumulateUnsigned32(m_field_override_parent_bits3);

    sha1.AccumulateDouble(m_baseline_spacing);
    sha1.AccumulateSubHash(m_text_mask.ContentHash());
    sha1.AccumulateSubHash(m_scale_value.ContentHash());
    sha1.AccumulateInteger32(m_dimscale_source);
    sha1.AccumulateId(m_source_dimstyle);
    sha1.AccumulateUnsigned8(m_ext_line_color_source);
    sha1.AccumulateUnsigned8(m_dim_line_color_source);
    sha1.AccumulateUnsigned8(m_arrow_color_source);
    sha1.AccumulateUnsigned8(m_text_color_source);
    sha1.AccumulateUnsigned32(m_ext_line_color);
    sha1.AccumulateUnsigned32(m_dim_line_color);
    sha1.AccumulateUnsigned32(m_arrow_color);
    sha1.AccumulateUnsigned32(m_text_color);
    sha1.AccumulateUnsigned8(m_ext_line_plot_color_source);
    sha1.AccumulateUnsigned8(m_dim_line_plot_color_source);
    sha1.AccumulateUnsigned8(m_arrow_plot_color_source);
    sha1.AccumulateUnsigned8(m_text_plot_color_source);
    sha1.AccumulateUnsigned32(m_ext_line_plot_color);
    sha1.AccumulateUnsigned32(m_dim_line_plot_color);
    sha1.AccumulateUnsigned32(m_arrow_plot_color);
    sha1.AccumulateUnsigned32(m_text_plot_color);
    sha1.AccumulateUnsigned8(m_ext_line_plot_weight_source);
    sha1.AccumulateUnsigned8(m_dim_line_plot_weight_source);
    sha1.AccumulateDouble(m_ext_line_plot_weight_mm);
    sha1.AccumulateDouble(m_dim_line_plot_weight_mm);
    sha1.AccumulateDouble(m_fixed_extension_len);
    sha1.AccumulateBool(m_fixed_extension_len_on);

    sha1.AccumulateBool(m_suppress_arrow1);
    sha1.AccumulateBool(m_suppress_arrow2);

    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_arrow_type_1));
    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_arrow_type_2));
    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_leader_arrow_type));
    sha1.AccumulateId(m_arrow_block_id_1);
    sha1.AccumulateId(m_arrow_block_id_2);
    sha1.AccumulateId(m_leader_arrow_block_id);

    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_leader_content_angle_style));
    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_leader_curve_type));
    sha1.AccumulateDouble(m_leader_content_angle);
    sha1.AccumulateBool(m_leader_has_landing);

    sha1.AccumulateBool(m_draw_forward);
    sha1.AccumulateBool(m_signed_ordinate);

    sha1.AccumulateBool(static_cast<unsigned int>(m_bForceDimLine));
    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_ArrowFit));
    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_TextFit));

    sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_decimal_separator));

    // Save hash in mutable m_content_hash
    m_content_hash = sha1.Hash();
  }

  return m_content_hash;
}

void MYON_DimStyle::Internal_TextPositionPropertiesChange()
{
  m_text_position_properties_hash = MYON_SHA1_Hash::EmptyContentHash;
  Internal_ContentChange();
}

double MYON_DimStyle::TextGap() const
{
  return m_textgap;
}

void MYON_DimStyle::SetTextGap(double gap)
{
  if (gap >= 0.0)
    Internal_SetDoubleMember(MYON_DimStyle::field::TextGap, gap, m_textgap);
}

double MYON_DimStyle::TextHeight() const
{
  return m_textheight;
}

double MYON_DimStyle::TextAdvanceOfCodePoint(unsigned unicode_code_point) const
{
  for (;;)
  {
    const double text_height = TextHeight();
    if (false == text_height > 0.0 && text_height < MYON_UNSET_POSITIVE_VALUE)
      break;

    const MYON_Font& font = Font();
    const MYON_FontGlyph* glyph = font.CodePointGlyph(unicode_code_point);
    if (nullptr == glyph)
      break;

    const MYON_TextBox font_unit_bbox = glyph->FontUnitGlyphBox();
    const MYON_TextBox normalized_bbox = glyph->GlyphBox();

    const MYON_FontMetrics& normalized_fm = font.FontMetrics();
    const MYON_FontMetrics& font_unit_fm = font.FontUnitFontMetrics();

    const int normalized_cap_height = normalized_fm.AscentOfCapital();
    const int normalized_glyph_advance = normalized_bbox.m_advance.i; // positive even for fonts designed for R to L locales
    const int font_unit_cap_height = font_unit_fm.AscentOfCapital();
    const int font_unit_glyph_advance = font_unit_bbox.m_advance.i; // positive even for fonts designed for R to L locales
    
    const double normalized_scale = normalized_cap_height > 0 ? (((double)normalized_glyph_advance) / ((double)normalized_cap_height)) : 0.0;    
    const double font_unit_scale = font_unit_cap_height > 0 ? (((double)font_unit_glyph_advance) / ((double)font_unit_cap_height)) : 0.0;

    const double s = (font_unit_scale >= normalized_scale) ? font_unit_scale : normalized_scale;

    const double width_of_space = s * text_height;
    if (width_of_space > 0.0 && width_of_space < MYON_UNSET_POSITIVE_VALUE)
      return width_of_space;

    break; // damaged or really odd font
  }

  return 0.0;
}

double MYON_DimStyle::TextWidthOfEmSpace() const
{
  // This is the fundemental WidthOfXSpace() function. 
  // Other WidthOfXSpace() functions call TextWidthOfEmSpace() when TextAdvanceOfCodePoint(obvious code point) fails.
  // This function may only call TextAdvanceOfCodePoint() and TextHeight().
  double w = TextAdvanceOfCodePoint(MYON_UnicodeCodePoint::MYON_EmSpace);
  if (false == w > 0.0)
  {
    w = TextAdvanceOfCodePoint('M');
    if (false == w > 0.0)
    {
      w = 2.0 * TextAdvanceOfCodePoint(MYON_UnicodeCodePoint::MYON_EnSpace);
      if (false == w > 0.0)
      {
        w = 2.0 * TextAdvanceOfCodePoint('N');
        if (false == w > 0.0)
        {
          w = 4.0 * TextAdvanceOfCodePoint(MYON_UnicodeCodePoint::MYON_Space);
          if (false == w > 0.0)
            w = 1.5 * TextHeight();
        }
      }
    }
  }
  return w;
}

double MYON_DimStyle::TextWidthOfEnSpace() const
{
  double w = TextAdvanceOfCodePoint(MYON_UnicodeCodePoint::MYON_EnSpace);
  // Don't test 'N' in this function. 
  // It is critical that 2*TextWidthOfEnSpace() = TextWidthOfEmSpace()
  // unless the font designer explicitly deviated from this standard.
  return w > 0.0 ? w : 0.5 * TextWidthOfEmSpace();
}

double MYON_DimStyle::TextWidthOfSpace() const
{
  double w = TextAdvanceOfCodePoint(MYON_UnicodeCodePoint::MYON_Space);
  return w > 0.0 ? w : 0.25 * TextWidthOfEmSpace();
}

double MYON_DimStyle::TextWidthOfFigureSpace() const
{
  double w = TextAdvanceOfCodePoint(MYON_UnicodeCodePoint::MYON_FigureSpace);
  if (false == w > 0.0)
  {
    w = TextAdvanceOfCodePoint('0');
    if (false == w > 0.0)
      w = 0.55 * TextWidthOfEmSpace();
  }
  return w;
}

double MYON_DimStyle::TextWidthOfIdeographicSpace() const
{
  double w = TextAdvanceOfCodePoint(MYON_UnicodeCodePoint::MYON_IdeographicSpace);
  return w > 0.0 ? w : TextWidthOfEmSpace();
}

double MYON_DimStyle::TextWidthOfMediumMathematicalSpace() const
{
  double w = TextAdvanceOfCodePoint(MYON_UnicodeCodePoint::MYON_MediumMathematicalSpace);
  return w > 0.0 ? w : (2.0/9.0)*TextWidthOfEmSpace();
}

void MYON_DimStyle::SetTextHeight(double height)
{
  if (MYON_IsValid(height) && height > MYON_SQRT_EPSILON)
  {
    if (Internal_SetDoubleMember(MYON_DimStyle::field::TextHeight, height, m_textheight))
      Internal_TextPositionPropertiesChange();
  }
}

double MYON_DimStyle::LengthFactor() const
{
  return m_lengthfactor;
}

void MYON_DimStyle::SetLengthFactor(double factor)
{
  Internal_SetDoubleMember(MYON_DimStyle::field::LengthFactor, factor, m_lengthfactor);
}

bool MYON_DimStyle::Alternate() const
{
  return m_bAlternate;
}
void MYON_DimStyle::SetAlternate(bool bAlternate)
{
  if ( Internal_SetBoolMember(MYON_DimStyle::field::Alternate, bAlternate, m_bAlternate) )
    Internal_TextPositionPropertiesChange();
}

double MYON_DimStyle::AlternateLengthFactor() const
{
  return m_alternate_lengthfactor;
}

void MYON_DimStyle::SetAlternateLengthFactor(double factor)
{
  Internal_SetDoubleMember(MYON_DimStyle::field::AlternateLengthFactor, factor, m_alternate_lengthfactor);
}

int MYON_DimStyle::AlternateLengthResolution() const
{
  return m_alternate_lengthresolution;
}

void MYON_DimStyle::SetAlternateLengthResolution(int resolution)
{
  Internal_SetIntMember(MYON_DimStyle::field::AlternateLengthResolution, resolution, m_alternate_lengthresolution);
}

const MYON_wString& MYON_DimStyle::Prefix() const
{
  m_prefix.IsValid(true);
  return m_prefix;
}

void MYON_DimStyle::SetPrefix(const wchar_t* prefix)
{
  Internal_SetStringMember(MYON_DimStyle::field::Prefix, prefix, m_prefix);
}

const MYON_wString& MYON_DimStyle::Suffix() const
{
  m_suffix.IsValid(true);
  return m_suffix;
}

void MYON_DimStyle::SetSuffix(const wchar_t* suffix)
{
  Internal_SetStringMember(MYON_DimStyle::field::Suffix, suffix, m_suffix);
}

const MYON_wString& MYON_DimStyle::AlternatePrefix() const
{
  m_alternate_prefix.IsValid(true);
  return m_alternate_prefix;
}

void MYON_DimStyle::SetAlternatePrefix(const wchar_t* prefix)
{
  Internal_SetStringMember(MYON_DimStyle::field::AlternatePrefix, prefix, m_alternate_prefix);
}

const MYON_wString& MYON_DimStyle::AlternateSuffix() const
{
  m_alternate_suffix.IsValid(true);
  return m_alternate_suffix;
}

void MYON_DimStyle::SetAlternateSuffix(const wchar_t* suffix)
{
  Internal_SetStringMember(MYON_DimStyle::field::AlternateSuffix, suffix, m_alternate_suffix);
}

bool MYON_DimStyle::SuppressExtension1() const
{
  return m_bSuppressExtension1;
}

void MYON_DimStyle::SetSuppressExtension1(bool suppress)
{
  Internal_SetBoolMember(MYON_DimStyle::field::SuppressExtension1, suppress, m_bSuppressExtension1);
}

bool MYON_DimStyle::SuppressExtension2() const
{
  return m_bSuppressExtension2;
}

void MYON_DimStyle::SetSuppressExtension2(bool suppress)
{
  Internal_SetBoolMember(MYON_DimStyle::field::SuppressExtension2, suppress, m_bSuppressExtension2);
}

double MYON_DimStyle::DimExtension() const
{
  return m_dimextension;
}

void MYON_DimStyle::SetDimExtension(const double e)
{
  // Allow negative for hollow user arrows
  Internal_SetDoubleMember(MYON_DimStyle::field::DimensionLineExtension, e, m_dimextension);
}

# pragma region SubObjectDisplay
void MYON_DimStyle::SetExtensionLineColorSource(const MYON::object_color_source src)
{
  Internal_SetUnsignedCharMember(MYON_DimStyle::field::ExtLineColorSource, (unsigned char)src, m_ext_line_color_source);
}

MYON::object_color_source MYON_DimStyle::ExtensionLineColorSource() const
{
  return (MYON::object_color_source)m_ext_line_color_source;
}
void MYON_DimStyle::SetDimensionLineColorSource(const MYON::object_color_source src)
{
  Internal_SetUnsignedCharMember(MYON_DimStyle::field::DimLineColorSource, (unsigned char)src, m_dim_line_color_source);
}

MYON::object_color_source MYON_DimStyle::DimensionLineColorSource() const
{
  return (MYON::object_color_source)m_dim_line_color_source;
}

void MYON_DimStyle::SetArrowColorSource(const MYON::object_color_source src)
{
  Internal_SetUnsignedCharMember(MYON_DimStyle::field::ArrowColorSource, (unsigned char)src, m_arrow_color_source);
}

MYON::object_color_source MYON_DimStyle::ArrowColorSource() const
{
  return (MYON::object_color_source)m_arrow_color_source;
}

void MYON_DimStyle::SetTextColorSource(const MYON::object_color_source src)
{
  Internal_SetUnsignedCharMember(MYON_DimStyle::field::TextColorSource, (unsigned char)src, m_text_color_source);
}

MYON::object_color_source MYON_DimStyle::TextColorSource() const
{
  return (MYON::object_color_source)m_text_color_source;
}

void MYON_DimStyle::SetExtensionLineColor(MYON_Color c)
{
  Internal_SetColorMember(MYON_DimStyle::field::ExtLineColor, c, m_ext_line_color);
}

MYON_Color MYON_DimStyle::ExtensionLineColor() const
{
  return m_ext_line_color;
}

void MYON_DimStyle::SetDimensionLineColor(MYON_Color c)
{
  Internal_SetColorMember(MYON_DimStyle::field::DimLineColor, c, m_dim_line_color);
}

MYON_Color MYON_DimStyle::DimensionLineColor() const
{
  return m_dim_line_color;
}

void MYON_DimStyle::SetArrowColor(MYON_Color c)
{
  Internal_SetColorMember(MYON_DimStyle::field::ArrowColor, c, m_arrow_color);
}

MYON_Color MYON_DimStyle::ArrowColor() const
{
  return m_arrow_color;
}

void MYON_DimStyle::SetTextColor(MYON_Color c)
{
  Internal_SetColorMember(MYON_DimStyle::field::TextColor, c, m_text_color);
}

MYON_Color MYON_DimStyle::TextColor() const
{
  return m_text_color;
}

void MYON_DimStyle::SetExtensionLinePlotColorSource(const MYON::plot_color_source src)
{
  Internal_SetUnsignedCharMember(MYON_DimStyle::field::ExtLinePlotColorSource, (unsigned char)src, m_ext_line_plot_color_source);
}

MYON::plot_color_source MYON_DimStyle::ExtensionLinePlotColorSource() const
{
  return (MYON::plot_color_source)m_ext_line_plot_color_source;
}

void MYON_DimStyle::SetDimensionLinePlotColorSource(const MYON::plot_color_source src)
{
  Internal_SetUnsignedCharMember(MYON_DimStyle::field::DimLinePlotColorSource, (unsigned char)src,m_dim_line_plot_color_source);
}

MYON::plot_color_source MYON_DimStyle::DimensionLinePlotColorSource() const
{
  return (MYON::plot_color_source)m_dim_line_plot_color_source;
}

void MYON_DimStyle::SetArrowPlotColorSource(const MYON::plot_color_source src)
{
  Internal_SetUnsignedCharMember(MYON_DimStyle::field::ArrowPlotColorSource, (unsigned char)src,m_arrow_plot_color_source);
}

MYON::plot_color_source MYON_DimStyle::ArrowPlotColorSource() const
{
  return (MYON::plot_color_source)m_arrow_plot_color_source;
}

void MYON_DimStyle::SetTextPlotColorSource(const MYON::object_color_source src)
{
  Internal_SetUnsignedCharMember(MYON_DimStyle::field::TextPlotColorSource, (unsigned char)src,m_text_plot_color_source);
}

MYON::object_color_source MYON_DimStyle::TextPlotColorSource() const
{
  return (MYON::object_color_source)m_text_plot_color_source;
}

void MYON_DimStyle::SetExtensionLinePlotColor(MYON_Color c)
{
  Internal_SetColorMember(MYON_DimStyle::field::ExtLinePlotColor, c, m_ext_line_plot_color);
}

MYON_Color MYON_DimStyle::ExtensionLinePlotColor() const
{
  return m_ext_line_plot_color;
}

void MYON_DimStyle::SetDimensionLinePlotColor(MYON_Color c)
{
  Internal_SetColorMember(MYON_DimStyle::field::DimLinePlotColor, c, m_dim_line_plot_color);
}

MYON_Color MYON_DimStyle::DimensionLinePlotColor() const
{
  return m_dim_line_plot_color;
}

void MYON_DimStyle::SetArrowPlotColor(MYON_Color c)
{
  Internal_SetColorMember(MYON_DimStyle::field::ArrowPlotColor, c, m_arrow_plot_color);
}

MYON_Color MYON_DimStyle::ArrowPlotColor() const
{
  return m_arrow_plot_color;
}

void MYON_DimStyle::SetTextPlotColor(MYON_Color c)
{
  Internal_SetColorMember(MYON_DimStyle::field::TextPlotColor, c, m_text_plot_color);
}

MYON_Color MYON_DimStyle::TextPlotColor() const
{
  return m_text_plot_color;
}

void MYON_DimStyle::SetExtensionLinePlotWeightSource(const MYON::plot_weight_source src)
{
  Internal_SetUnsignedCharMember(MYON_DimStyle::field::ExtLinePlotWeightSource, (unsigned char)src, m_ext_line_plot_weight_source);
}

MYON::plot_weight_source MYON_DimStyle::ExtensionLinePlotWeightSource() const
{
  return (MYON::plot_weight_source)m_ext_line_plot_weight_source;
}

void MYON_DimStyle::SetDimensionLinePlotWeightSource(const MYON::plot_weight_source src)
{
  Internal_SetUnsignedCharMember(MYON_DimStyle::field::DimLinePlotWeightSource, (unsigned char)src, m_dim_line_plot_weight_source);
}

MYON::plot_weight_source MYON_DimStyle::DimensionLinePlotWeightSource() const
{
  return (MYON::plot_weight_source)m_dim_line_plot_weight_source;
}

void MYON_DimStyle::SetExtensionLinePlotWeight(double w)
{
  if (w >= 0.0)
    Internal_SetDoubleMember(MYON_DimStyle::field::ExtLinePlotWeight_mm, w, m_ext_line_plot_weight_mm);
}

double MYON_DimStyle::ExtensionLinePlotWeight() const
{
  return m_ext_line_plot_weight_mm;
}

void MYON_DimStyle::SetDimensionLinePlotWeight(double w)
{
  if (w >= 0.0)
    Internal_SetDoubleMember(MYON_DimStyle::field::DimLinePlotWeight_mm, w, m_dim_line_plot_weight_mm);
}

double MYON_DimStyle::DimensionLinePlotWeight() const
{
  return m_dim_line_plot_weight_mm;
}
# pragma endregion SubObjectDisplay

void MYON_DimStyle::SetFixedExtensionLen(double l)
{
  if (l >= 0.0)
    Internal_SetDoubleMember(MYON_DimStyle::field::FixedExtensionLength, l, m_fixed_extension_len);
}

double MYON_DimStyle::FixedExtensionLen() const
{
  return m_fixed_extension_len;
}

void MYON_DimStyle::SetFixedExtensionLenOn(bool on)
{
  Internal_SetBoolMember(MYON_DimStyle::field::FixedExtensionOn, on, m_fixed_extension_len_on);
}

bool MYON_DimStyle::FixedExtensionLenOn() const
{
  return m_fixed_extension_len_on;
}

void MYON_DimStyle::SetTextRotation(double r)
{
  Internal_SetDoubleMember(MYON_DimStyle::field::TextRotation, r, m_text_rotation);
}

double MYON_DimStyle::TextRotation() const
{
  return m_text_rotation;
}

void MYON_DimStyle::SetAlternateToleranceResolution(int r)
{
  if (r >= 0)
    Internal_SetIntMember(MYON_DimStyle::field::AltToleranceResolution, r, m_alternate_tolerance_resolution);
}

int MYON_DimStyle::AlternateToleranceResolution() const
{
  return m_alternate_tolerance_resolution;
}

void MYON_DimStyle::SetSuppressArrow1(bool s)
{
  Internal_SetBoolMember(MYON_DimStyle::field::SuppressArrow1, s, m_suppress_arrow1);
}

bool MYON_DimStyle::SuppressArrow1() const
{
  return m_suppress_arrow1;
}

void MYON_DimStyle::SetSuppressArrow2(bool s)
{
  Internal_SetBoolMember(MYON_DimStyle::field::SuppressArrow2, s, m_suppress_arrow2);
}

bool MYON_DimStyle::SuppressArrow2() const
{
  return m_suppress_arrow2;
}

void MYON_DimStyle::SetTextMoveLeader(int m)
{
  Internal_SetIntMember(MYON_DimStyle::field::TextmoveLeader, m, m_textmove_leader);
}

int MYON_DimStyle::TextMoveLeader() const
{
  return m_textmove_leader;
}

void MYON_DimStyle::SetArcLengthSymbol(int m)
{
  Internal_SetIntMember(MYON_DimStyle::field::ArclengthSymbol, m, m_arclength_sym);
}

int MYON_DimStyle::ArcLengthSymbol() const
{
  return m_arclength_sym;
}

void MYON_DimStyle::SetStackFractionFormat(MYON_DimStyle::stack_format f)
{
  if (m_stack_format != f)
  {
    m_stack_format = f;
    Internal_ContentChange();
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::StackFormat);
}

MYON_DimStyle::stack_format MYON_DimStyle::StackFractionFormat() const
{
  return m_stack_format;
}

void MYON_DimStyle::SetStackHeightScale(double f)
{
  if (f > MYON_SQRT_EPSILON)
    Internal_SetDoubleMember(MYON_DimStyle::field::StackTextheightScale, f, m_stack_textheight_fraction);
}

double MYON_DimStyle::StackHeightScale() const
{
  return m_stack_textheight_fraction;
}

void MYON_DimStyle::SetRoundOff(double r)
{
  Internal_SetDoubleMember(MYON_DimStyle::field::Round, r, m_round);
}

double MYON_DimStyle::RoundOff() const
{
  return m_round;
}

void MYON_DimStyle::SetAlternateRoundOff(double r)
{
  Internal_SetDoubleMember(MYON_DimStyle::field::AltRound, r, m_alt_round);
}

double MYON_DimStyle::AlternateRoundOff() const
{
  return m_alt_round;
}

void MYON_DimStyle::SetAngleRoundOff(double r)
{
  Internal_SetDoubleMember(MYON_DimStyle::field::AngularRound, r, m_angular_round);
}

double MYON_DimStyle::AngleRoundOff() const
{
  return m_angular_round;
}

void MYON_DimStyle::SetZeroSuppress(MYON_DimStyle::suppress_zero s)
{
  if (m_zero_suppress != s)
  {
    if (MYON_DimStyle::ZeroSuppressMatchesLengthDisplay(s, DimensionLengthDisplay()))
    {
      m_zero_suppress = s;
      Internal_ContentChange();
    }
    //else
    //{
    //  MYON_ERROR("Attempting to set zero suppression to a value that doesn't match length display.");
    //}
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::ZeroSuppress);
}

MYON_DimStyle::suppress_zero MYON_DimStyle::ZeroSuppress() const
{
  return m_zero_suppress;
}

void MYON_DimStyle::SetAlternateZeroSuppress(MYON_DimStyle::suppress_zero s)
{
  if (m_alt_zero_suppress != s)
  {
    m_alt_zero_suppress = s;
    Internal_ContentChange();
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::AltZeroSuppress);
}

MYON_DimStyle::suppress_zero MYON_DimStyle::AlternateZeroSuppress() const
{
  return m_alt_zero_suppress;
}

void MYON_DimStyle::SetToleranceZeroSuppress(MYON_DimStyle::suppress_zero s)
{
  // OBSOLETE PROPERTY
  // The ZeroSuppress() or  AlternateZeroSuppress() property
  // is used to format tolerance display.  ToleranceZeroSuppress() is ignored.
}

MYON_DimStyle::suppress_zero MYON_DimStyle::ToleranceZeroSuppress() const
{
  // OBSOLETE PROPERTY
  // The ZeroSuppress() or  AlternateZeroSuppress() property
  // is used to format tolerance display.  ToleranceZeroSuppress() is ignored.
  return MYON_DimStyle::suppress_zero::None;
}

void MYON_DimStyle::SetAngleZeroSuppress(MYON_DimStyle::suppress_zero s)
{
  if (m_ang_zero_suppress != s)
  {
    m_ang_zero_suppress = s;
    Internal_ContentChange();
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::AngleZeroSuppress);
}

MYON_DimStyle::suppress_zero MYON_DimStyle::AngleZeroSuppress() const
{
  return m_ang_zero_suppress;
}

void MYON_DimStyle::SetAlternateBelow(bool below)
{
  Internal_SetBoolMember(MYON_DimStyle::field::AltBelow, below, m_alt_below);
}

bool MYON_DimStyle::AlternateBelow() const
{
  return m_alt_below;
}

void MYON_DimStyle::SetArrowType1(MYON_Arrowhead::arrow_type type)
{
  if (m_arrow_type_1 != type)
  {
    m_arrow_type_1 = type;
    Internal_ContentChange();
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::ArrowType1);
}

MYON_Arrowhead::arrow_type MYON_DimStyle::ArrowType1() const
{
  return m_arrow_type_1;
}

void  MYON_DimStyle::SetArrowType1And2(MYON_Arrowhead::arrow_type type)
{
  SetArrowType1(type);
  SetArrowType2(type);
}


void MYON_DimStyle::SetArrowType2(MYON_Arrowhead::arrow_type type)
{
  if (m_arrow_type_2 != type)
  {
    m_arrow_type_2 = type;
    Internal_ContentChange();
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::ArrowType2);
}

MYON_Arrowhead::arrow_type MYON_DimStyle::ArrowType2() const
{
  return m_arrow_type_2;
}

void MYON_DimStyle::SetLeaderArrowType(MYON_Arrowhead::arrow_type type)
{
  if (m_leader_arrow_type != type)
  {
    m_leader_arrow_type = type;
    Internal_ContentChange();
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::LeaderArrowType);
}

MYON_Arrowhead::arrow_type MYON_DimStyle::LeaderArrowType() const
{
  return m_leader_arrow_type;
}

void MYON_DimStyle::SetArrowBlockId1(MYON_UUID id)
{
  Internal_SetIdMember(MYON_DimStyle::field::ArrowBlockId1, id, m_arrow_block_id_1);
}

MYON_UUID MYON_DimStyle::ArrowBlockId1() const
{
  return m_arrow_block_id_1;
}

void MYON_DimStyle::SetArrowBlockId2(MYON_UUID id)
{
  Internal_SetIdMember(MYON_DimStyle::field::ArrowBlockId2, id, m_arrow_block_id_2);
}

MYON_UUID MYON_DimStyle::ArrowBlockId2() const
{
  return m_arrow_block_id_2;
}

void MYON_DimStyle::SetLeaderArrowBlockId(MYON_UUID id)
{
  Internal_SetIdMember(MYON_DimStyle::field::LeaderArrowBlock, id, m_leader_arrow_block_id);
}

MYON_UUID MYON_DimStyle::LeaderArrowBlockId() const
{
  return m_leader_arrow_block_id;
}

MYON::TextVerticalAlignment MYON_DimStyle::TextVerticalAlignment() const
{
  return m_text_vertical_alignment;
}

void MYON_DimStyle::SetTextVerticalAlignment(MYON::TextVerticalAlignment style)
{
  if (m_text_vertical_alignment != style)
  {
    m_text_vertical_alignment = style;
    Internal_TextPositionPropertiesChange();
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::TextVerticalAlignment);
}

MYON::TextVerticalAlignment MYON_DimStyle::LeaderTextVerticalAlignment() const
{
  return m_leader_text_vertical_alignment;
}

void MYON_DimStyle::SetLeaderTextVerticalAlignment(MYON::TextVerticalAlignment style)
{
  if (m_leader_text_vertical_alignment != style)
  {
    m_leader_text_vertical_alignment = style;
    Internal_TextPositionPropertiesChange();
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::LeaderTextVerticalAlignment);
}

MYON_DimStyle::ContentAngleStyle MYON_DimStyle::LeaderContentAngleStyle() const
{
  return m_leader_content_angle_style;
}

void MYON_DimStyle::SetLeaderContentAngleStyle(MYON_DimStyle::ContentAngleStyle alignment)
{
  if (m_leader_content_angle_style != alignment)
  {
    m_leader_content_angle_style = alignment;
    Internal_ContentChange();
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::LeaderContentAngleStyle);
}

double MYON_DimStyle::LeaderContentAngleRadians() const
{
  return m_leader_content_angle;
}

void MYON_DimStyle::SetLeaderContentAngleRadians(double angle_radians)
{
  if (!(angle_radians >= -2.1*MYON_PI && angle_radians <= 2.1*MYON_PI))
  {
    MYON_ERROR("Invalid angle_radians parameter in MYON_DimStyle::SetContentAngle().");
    return;
  }
  
  // positive value so commpare function will work as expected.
  while (angle_radians < 0.0)
    angle_radians += 2.0*MYON_PI;
  while (angle_radians >= 2.0*MYON_PI)
    angle_radians -= 2.0*MYON_PI;

  Internal_SetDoubleMember(MYON_DimStyle::field::LeaderContentAngle, angle_radians, m_leader_content_angle);
}

double MYON_DimStyle::LeaderContentAngleDegrees() const
{
  return LeaderContentAngleRadians() * MYON_RADIANS_TO_DEGREES;
}

void MYON_DimStyle::SetLeaderContentAngleDegrees(double angle_degrees)
{
  SetLeaderContentAngleRadians(angle_degrees * MYON_DEGREES_TO_RADIANS);
}

MYON_DimStyle::leader_curve_type MYON_DimStyle::LeaderCurveType() const
{
  return m_leader_curve_type;
}

void MYON_DimStyle::SetLeaderCurveType(MYON_DimStyle::leader_curve_type type)
{
  if (m_leader_curve_type != type)
  {
    m_leader_curve_type = type;
    Internal_ContentChange();
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::LeaderCurveType);
}

MYON::TextHorizontalAlignment MYON_DimStyle::LeaderTextHorizontalAlignment() const
{
  return m_leader_text_horizontal_alignment;
}

void MYON_DimStyle::SetLeaderTextHorizontalAlignment(MYON::TextHorizontalAlignment halign)
{
  if (m_leader_text_horizontal_alignment != halign)
  {
    m_leader_text_horizontal_alignment = halign;
    Internal_TextPositionPropertiesChange();
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::LeaderTextHorizontalAlignment);
}

MYON::TextHorizontalAlignment MYON_DimStyle::TextHorizontalAlignment() const
{
  return m_text_horizontal_alignment;
}

void MYON_DimStyle::SetTextHorizontalAlignment(MYON::TextHorizontalAlignment halign)
{
  if ( m_text_horizontal_alignment != halign )
  {
    m_text_horizontal_alignment = halign;
    Internal_TextPositionPropertiesChange();
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::TextHorizontalAlignment);
}

bool MYON_DimStyle::DrawForward() const
{
  return m_draw_forward;
}

void MYON_DimStyle::SetDrawForward(bool drawforward)
{
  if (m_draw_forward != drawforward)
  {
    m_draw_forward = drawforward;
    Internal_ContentChange();
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::DrawForward);
}

bool MYON_DimStyle::LeaderHasLanding() const
{
  return m_leader_has_landing;
}

void MYON_DimStyle::SetLeaderHasLanding(bool landing)
{
  if (m_leader_has_landing != landing)
  {
    m_leader_has_landing = landing;
    Internal_ContentChange();
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::LeaderHasLanding);
}

double MYON_DimStyle::LeaderLandingLength() const
{
  return m_leader_landing_length;
}

void MYON_DimStyle::SetLeaderLandingLength(double length)
{
  if (!(length > MYON_UNSET_VALUE && length < MYON_UNSET_POSITIVE_VALUE))
  {
    MYON_ERROR("Invalid length parameter in MYON_DimStyle::SetLandingLength().");
    return;
  }
  Internal_SetDoubleMember(MYON_DimStyle::field::LeaderLandingLength, length, m_leader_landing_length);
}

bool MYON_DimStyle::SignedOrdinate() const
{
  return m_signed_ordinate;
}

void MYON_DimStyle::SetSignedOrdinate(bool allowsigned)
{
  Internal_SetBoolMember(MYON_DimStyle::field::SignedOrdinate, allowsigned, m_signed_ordinate);
}

MYON::LengthUnitSystem MYON_DimStyle::UnitSystem() const
{
  /// NOTE WELL: A dimstyle unit system was added in V6, but has never been fully used.
  /// The idea was this would make it easier to figure out what text height/ arrow size, 
  /// ... actually meant. Especially in situations where model space and page space have
  /// different unit systems, and in more complex cases like text in instance definitions
  /// and inserting annotation from models with mismatched unit systems.
  /// It is used internally to get some scales properly set and use in limited
  /// merging contexts.
  ///   
  /// From a user's perspective, in Rhino 6 and Rhino 7 MYON_DimStyle lengths like TextHeight(), ArrowSize(), ... 
  /// are with respect to the context the annotation resides in. For example, if TextHeight() = 3.5,
  /// model units = meters, page units = millimters, and DimScale() = 1, then 
  /// text created in model space will be 3.5 meters high and
  /// text created in page space will be 3.5 millimeters high.
  /// 
  /// Ideally, MYON_DimStyle::UnitSystem() would specify the text height units 
  /// and MYON_DimStyle::DimScale() cound be adjusted as model space extents require.
  /// Text in instance definitions would have a well defined height and references
  /// to those instance defintions would display predictably in both model space and page space.

  // It is critical that this function never return Unset or CustomUnits.
  // returning None insures unknown scal values are 1 instead of MYON_DBL_QNAN
  if (MYON::LengthUnitSystem::Unset == m_dimstyle_unitsystem || MYON::LengthUnitSystem::CustomUnits == m_dimstyle_unitsystem)
    return MYON::LengthUnitSystem::None;

  return m_dimstyle_unitsystem;
}

void MYON_DimStyle::SetUnitSystem(MYON::LengthUnitSystem us)
{
  /// NOTE WELL: A dimstyle unit system was added in V6, but has never been fully used.
  /// The idea was this would make it easier to figure out what text height/ arrow size, 
  /// ... actually meant. Especially in situations where model space and page space have
  /// different unit systems, and in more complex cases like text in instance definitions
  /// and inserting annotation from models with mismatched unit systems.
  /// It is used internally to get some scales properly set and use in limited
  /// merging contexts.
  ///   
  /// From a user's perspective, in Rhino 6 and Rhino 7 MYON_DimStyle lengths like TextHeight(), ArrowSize(), ... 
  /// are with respect to the context the annotation resides in. For example, if TextHeight() = 3.5,
  /// model units = meters, page units = millimters, and DimScale() = 1, then 
  /// text created in model space will be 3.5 meters high and
  /// text created in page space will be 3.5 millimeters high.
  /// 
  /// Ideally, MYON_DimStyle::UnitSystem() would specify the text height units 
  /// and MYON_DimStyle::DimScale() cound be adjusted as model space extents require.
  /// Text in instance definitions would have a well defined height and references
  /// to those instance defintions would display predictably in both model space and page space.
  if (MYON::LengthUnitSystem::CustomUnits == us || MYON::LengthUnitSystem::Unset == us)
  {
    MYON_ERROR("Annotation styles cannot have unset or custom length units.");
    us = MYON::LengthUnitSystem::Millimeters; // Using None insures scale values are 1 instead of MYON_DBL_QNAN
  }

  if (m_dimstyle_unitsystem != us)
  {
    m_dimstyle_unitsystem = us;
    Internal_ContentChange();
  }

  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::UnitSystem);
}

void MYON_DimStyle::SetUnitSystemFromContext(
  bool bUseName,
  MYON::LengthUnitSystem source_unit_system,
  MYON::LengthUnitSystem destination_unit_system
)
{
  /// NOTE WELL: A dimstyle unit system was added in V6, but has never been fully used.
  /// The idea was this would make it easier to figure out what text height/ arrow size, 
  /// ... actually meant. Especially in situations where model space and page space have
  /// different unit systems, and in more complex cases like text in instance definitions
  /// and inserting annotation from models with mismatched unit systems.
  /// It is used internally to get some scales properly set and use in limited
  /// merging contexts.
  ///   
  /// From a user's perspective, in Rhino 6 and Rhino 7 MYON_DimStyle lengths like TextHeight(), ArrowSize(), ... 
  /// are with respect to the context the annotation resides in. For example, if TextHeight() = 3.5,
  /// model units = meters, page units = millimters, and DimScale() = 1, then 
  /// text created in model space will be 3.5 meters high and
  /// text created in page space will be 3.5 millimeters high.
  /// 
  /// Ideally, MYON_DimStyle::UnitSystem() would specify the text height units 
  /// and MYON_DimStyle::DimScale() cound be adjusted as model space extents require.
  /// Text in instance definitions would have a well defined height and references
  /// to those instance defintions would display predictably in both model space and page space.


  MYON::LengthUnitSystem dim_style_units = MYON::LengthUnitSystemFromUnsigned(static_cast<unsigned int>(UnitSystem()));

  switch (dim_style_units)
  {
    case MYON::LengthUnitSystem::None:
    case MYON::LengthUnitSystem::CustomUnits:
    case MYON::LengthUnitSystem::Unset:
      dim_style_units = MYON::LengthUnitSystem::None;
      break;
    default:
      break;
  }
  
  for (int pass = 0; pass < 3; pass++)
  {
    if (MYON::LengthUnitSystem::None != dim_style_units)
      break;
    if (0 == pass && false == bUseName)
      continue;
    switch (pass)
    {
    case 0: // search dimstyle name for metric or United States customary unit system names
      {
        const MYON_NameHash name_hash = NameHash();
        if (name_hash.IsValidAndNotEmpty())
        {
          const MYON_DimStyle& from_name = MYON_DimStyle::SystemDimstyleFromName(name_hash);
          if (name_hash == from_name.NameHash() && name_hash != MYON_DimStyle::Default.NameHash())
          {
            dim_style_units = from_name.UnitSystem();
            continue;
          }
        }
    
        const MYON_UUID id = Id();
        if (MYON_nil_uuid != id )
        {
          const MYON_DimStyle& from_id = MYON_DimStyle::SystemDimstyleFromId(id);
          if (id == from_id.Id() && id != MYON_DimStyle::Default.Id() )
          {
            dim_style_units = from_id.UnitSystem();
            continue;
          }
        }

        MYON_wString dim_style_name = Name();
        dim_style_name.TrimLeftAndRight();
        dim_style_name.MakeLowerOrdinal();
        if (dim_style_name.IsEmpty())
          break;
        const int dim_style_name_length = dim_style_name.Length();
        if (dim_style_name_length <= 0)
          break;

        const size_t name_list_capacity = MYON_LengthUnitName::GetLengthUnitNameList(0, nullptr);
        if (name_list_capacity <= 0)
          break;

        MYON_SimpleArray<MYON_LengthUnitName> name_list(name_list_capacity);
        name_list.SetCount((int)name_list_capacity);
        MYON_LengthUnitName::GetLengthUnitNameList(name_list.UnsignedCount(), name_list.Array());
        
        for (unsigned int i = 0; i < name_list.UnsignedCount(); i++)
        {
          MYON::LengthUnitSystem name_us = name_list[i].LengthUnit();
          if (false == MYON::IsTerrestrialLengthUnit(name_us))
            continue;
          if (
            false == MYON::IsMetricLengthUnit(name_us)
            && false == MYON::IsUnitedStatesCustomaryLengthUnit(name_us) 
            )
            continue;
          MYON_wString unit_name = name_list[i].LengthUnitName();
          unit_name.TrimLeftAndRight();
          unit_name.MakeLowerOrdinal();
          if (unit_name.IsEmpty())
            continue;
          const int name_length = unit_name.Length();
          if (name_length <= 0)
            continue;
          if (name_length > dim_style_name_length )
            continue;

          const wchar_t* s = static_cast<const wchar_t*>(dim_style_name);
          const wchar_t* s1 = s + dim_style_name_length - name_length;
          for (/*empty init*/; s <= s1; s++)
          {
            if (MYON_wString::EqualOrdinal(static_cast<const wchar_t*>(unit_name), name_length, s, name_length, false))
            {
              if (MYON::IsUnitedStatesCustomaryLengthUnit(name_us))
                dim_style_units = MYON::LengthUnitSystem::Inches;
              else
                dim_style_units = MYON::LengthUnitSystem::Millimeters;
              break;
            }
          }
          if (MYON::LengthUnitSystem::None != dim_style_units)
            break;
        }
      }
      break;

    case 1:
    case 2:
      {
        const MYON::LengthUnitSystem context_us
          = (1 == pass)
          ? source_unit_system
          : destination_unit_system;
        if (MYON::IsUnitedStatesPrinterLengthUnit(context_us))
          dim_style_units = context_us;
        else if (MYON::IsTerrestrialLengthUnit(context_us))
        {
          if (MYON::IsUnitedStatesCustomaryLengthUnit(context_us))
            dim_style_units = MYON::LengthUnitSystem::Inches;
          else if (MYON::IsMetricLengthUnit(context_us))
            dim_style_units = MYON::LengthUnitSystem::Millimeters;
        }
      }
      break;
    }
  }

  if (MYON::LengthUnitSystem::None == dim_style_units)
    dim_style_units = MYON::LengthUnitSystem::Millimeters;

  if( dim_style_units != UnitSystem() )
    SetUnitSystem(dim_style_units);

  return;
}

static bool Internal_IsUnsetDimstyleUnitSystem(
  MYON::LengthUnitSystem us
)
{
  return (
    MYON::LengthUnitSystem::Unset == us
    || MYON::LengthUnitSystem::None == us
    || MYON::LengthUnitSystem::CustomUnits == us
    );
}

bool MYON_DimStyle::UnitSystemIsSet() const
{
  return false == Internal_IsUnsetDimstyleUnitSystem(this->UnitSystem());
}

void MYON_DimStyle::SetDimScale(
  double left_val,
  MYON::LengthUnitSystem left_us,
  double right_val,
  MYON::LengthUnitSystem right_us)
{
  const unsigned int locale_id = 0; // Will result in current locale id being used
  //const double clean_format_tol = 0.0;


  const MYON_LengthValue::StringFormat left_string_format
    = (left_us == MYON::LengthUnitSystem::Inches)
    ? MYON_LengthValue::StringFormat::CleanProperFraction
    : MYON_LengthValue::StringFormat::CleanDecimal;
  MYON_LengthValue lvl = MYON_LengthValue::Create(left_val, left_us, locale_id, left_string_format);

  const MYON_LengthValue::StringFormat right_string_format
    = (right_us == MYON::LengthUnitSystem::Inches)
    ? MYON_LengthValue::StringFormat::CleanProperFraction
    : MYON_LengthValue::StringFormat::CleanDecimal;
  MYON_LengthValue lvr = MYON_LengthValue::Create(right_val, right_us, locale_id, right_string_format);

  const MYON_ScaleValue::ScaleStringFormat fmt = MYON_ScaleValue::ScaleStringFormat::None;
  MYON_ScaleValue sv = MYON_ScaleValue::Create(lvl, lvr, fmt);
  if (sv.IsSet())
  {
    MYON_DimStyle::SetDimScale(sv);
  }
}

void MYON_DimStyle::SetDimScale(double scale)
{
  if (MYON_IsValid(scale) && 0.0 < scale)
  {
    MYON_ScaleValue sv;
    const MYON_ScaleValue::ScaleStringFormat fmt = MYON_ScaleValue::ScaleStringFormat::None;
    const unsigned int locale_id = 0; // will result in using the current locale id
    const MYON_LengthValue::StringFormat lv_fmt = MYON_LengthValue::StringFormat::CleanDecimal;
    //const double lv_clean_tol = 0.0;

    MYON_LengthValue left_lv; // unset
    MYON_LengthValue right_lv;
    MYON::LengthUnitSystem left_us = UnitSystem();
    MYON::LengthUnitSystem right_us = UnitSystem();
    double left_val = 1.0;
    double right_val = 1.0;

    if (m_scale_value.IsSet())
    {
      left_lv = m_scale_value.LeftLengthValue();
      right_lv = m_scale_value.RightLengthValue();
    }
    if (left_lv.IsSet())
    {
      left_us = left_lv.LengthStringParseSettings().ContextLengthUnitSystem();
      left_val = left_lv.Length(left_us);
    }

    if (right_lv.IsSet())
    {
      right_us = right_lv.LengthStringParseSettings().ContextLengthUnitSystem();
      right_val = right_lv.Length(right_us);
    }

    double unit_scale = MYON::UnitScale(right_us, left_us);
    right_val = scale * left_val / unit_scale;

    const MYON_LengthValue scale_left_lv
      = MYON_LengthValue::Create(left_val, left_us, locale_id, lv_fmt);

    const MYON_LengthValue scale_right_lv
      = MYON_LengthValue::Create(right_val, right_us, locale_id, lv_fmt);

    sv = MYON_ScaleValue::Create(scale_left_lv, scale_right_lv, fmt);

    if (sv.IsSet())
      SetDimScale(sv);
  }
}


double MYON_DimStyle::DimScale() const
{
  return m_scale_value.RightToLeftScale();
}

void MYON_DimStyle::SetDimScale(MYON_ScaleValue sv)
{
  if (0 != MYON_ScaleValue::Compare(m_scale_value,sv))
  {
    Internal_ContentChange();
    Internal_TextPositionPropertiesChange();
  }
  m_scale_value = sv;
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::DimensionScale);
}

const MYON_ScaleValue& MYON_DimStyle::ScaleValue() const
{
  return m_scale_value;
}

double MYON_DimStyle::ScaleLeftLength_mm() const
{
  double d = 1.0;
  const MYON_LengthValue& lv = m_scale_value.LeftLengthValue();
  d = lv.Length(MYON::LengthUnitSystem::Millimeters);
  return d;
}

double MYON_DimStyle::ScaleRightLength_mm() const
{
  double d = 1.0;
  const MYON_LengthValue& lv = m_scale_value.RightLengthValue();
  d = lv.Length(MYON::LengthUnitSystem::Millimeters);
  return d;
}

void MYON_DimStyle::SetTextOrientation(MYON::TextOrientation orientation)
{
  if (m_text_orientation != orientation)
  {
    m_text_orientation = orientation;
    Internal_TextPositionPropertiesChange();
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::TextOrientation);
}

MYON::TextOrientation MYON_DimStyle::TextOrientation() const
{
  return m_text_orientation;
}

void MYON_DimStyle::SetLeaderTextOrientation(MYON::TextOrientation orientation)
{ 
  if (m_leader_text_orientation != orientation)
  {
    m_leader_text_orientation = orientation;
    Internal_TextPositionPropertiesChange();
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::LeaderTextOrientation);
}

MYON::TextOrientation MYON_DimStyle::LeaderTextOrientation() const
{
  return m_leader_text_orientation;
}

void MYON_DimStyle::SetDimTextOrientation(MYON::TextOrientation orientation)
{
  if (m_dim_text_orientation != orientation)
  {
    m_dim_text_orientation = orientation;
    Internal_TextPositionPropertiesChange();
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::DimTextOrientation);
}

MYON::TextOrientation MYON_DimStyle::DimTextOrientation() const
{
  return m_dim_text_orientation;
}

void MYON_DimStyle::SetDimRadialTextOrientation(MYON::TextOrientation orientation)
{
  if (m_dimradial_text_orientation != orientation)
  {
    m_dimradial_text_orientation = orientation;
    Internal_TextPositionPropertiesChange();
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::DimRadialTextOrientation);
}

MYON::TextOrientation MYON_DimStyle::DimRadialTextOrientation() const
{
  return m_dimradial_text_orientation;
}

void MYON_DimStyle::SetDimTextAngleStyle(MYON_DimStyle::ContentAngleStyle style)
{
  if (m_dim_text_angle_style != style)
  {
    m_dim_text_angle_style = style;
    Internal_TextPositionPropertiesChange();
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::DimTextAngleStyle);
}

MYON_DimStyle::ContentAngleStyle MYON_DimStyle::DimTextAngleStyle() const
{
  return m_dim_text_angle_style;
}

void MYON_DimStyle::SetDimRadialTextAngleStyle(MYON_DimStyle::ContentAngleStyle style)
{
  if (m_dimradial_text_angle_style != style)
  {
    m_dimradial_text_angle_style = style;
    Internal_TextPositionPropertiesChange();
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::DimRadialTextAngleStyle);
}

MYON_DimStyle::ContentAngleStyle MYON_DimStyle::DimRadialTextAngleStyle() const
{
  return m_dimradial_text_angle_style;
}

bool MYON_DimStyle::TextUnderlined() const
{
  return m_text_underlined;
}

void MYON_DimStyle::SetTextUnderlined(bool underlined)
{
  if (m_text_underlined != underlined)
  {
    m_text_underlined = underlined;
    Internal_TextPositionPropertiesChange();
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::TextUnderlined);
}

bool MYON_DimStyle::ForceDimLine() const
{
  return m_bForceDimLine;
}
void MYON_DimStyle::SetForceDimLine(bool forcedimline)
{
  if (m_bForceDimLine != forcedimline)
  {
    m_bForceDimLine = forcedimline;
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::ForceDimLine);
}

void MYON_DimStyle::SetArrowFit(MYON_DimStyle::arrow_fit arrowfit)
{
  if (m_ArrowFit != arrowfit)
  {
    m_ArrowFit = arrowfit;
    Internal_TextPositionPropertiesChange();
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::ArrowFit);
}

MYON_DimStyle::arrow_fit MYON_DimStyle::ArrowFit() const
{
  return m_ArrowFit;
}

void MYON_DimStyle::SetTextFit(MYON_DimStyle::text_fit textfit)
{
  if (m_TextFit != textfit)
  {
    m_TextFit = textfit;
    Internal_TextPositionPropertiesChange();
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::TextFit);
}

MYON_DimStyle::text_fit MYON_DimStyle::TextFit() const
{
  return m_TextFit;
}

void MYON_DimStyle::SetDecimalSeparator(wchar_t separator)
{
  if(separator == MYON_wString::DecimalAsComma)
    m_decimal_separator = MYON_wString::DecimalAsComma;
  else
    m_decimal_separator = MYON_wString::DecimalAsPeriod;
}

wchar_t MYON_DimStyle::DecimalSeparator() const 
{
  return m_decimal_separator;
}

MYON__UINT32* MYON_DimStyle::Internal_GetOverrideParentBit(MYON_DimStyle::field field_id, MYON__UINT32* mask) const
{
  unsigned int bitdex = 0;
  unsigned int i = static_cast<unsigned int>(field_id);
  if (i >= static_cast<unsigned int>(MYON_DimStyle::field::Count))
  {
    MYON_ERROR("Invalid field_id value.");
    return nullptr;
  }

  if (i <= static_cast<unsigned int>(MYON_DimStyle::field::Index))
  {
    // query makes no sense for Unset, Name and Index
    // because these cannot be inherited from parent.
    // false is returned instead of the more accurate true
    // because that's the way this code has worked for years
    // and being accuruate would cause bugs at this point.
    return nullptr; 
  }

  while (i >= 32 && bitdex < 4)
  {
    bitdex++;
    i -= 32;
  }
  *mask = (1 << i);

  const MYON__UINT32* bits;
  if (0 == bitdex)
    bits = &m_field_override_parent_bits0;
  else if (1 == bitdex)
    bits = &m_field_override_parent_bits1;
  else if (2 == bitdex)
    bits = &m_field_override_parent_bits2;
  else if (3 == bitdex)
    bits = &m_field_override_parent_bits3;
  else
  {
    MYON_ERROR("field_id too big - add another m_independent_of_parent_bitsN = 0 member.");
    bits = nullptr;
    *mask = 0;
  }

  return const_cast<MYON__UINT32*>(bits);
}


bool MYON_DimStyle::IsFieldOverride(MYON_DimStyle::field field_id) const
{
  MYON__UINT32 mask = 0;
  const MYON__UINT32* bits
    = (m_field_override_parent_count > 0)
    ? Internal_GetOverrideParentBit(field_id, &mask)
    : nullptr;
  return (nullptr == bits) ? false : (0 != (*bits & mask));
}

void MYON_DimStyle::SetFieldOverrideAll(bool bOverrideParent)
{
  if (false == bOverrideParent)
  {
    const bool bContentChange = (0 != m_field_override_parent_count);
    m_field_override_parent_count = 0;
    m_field_override_parent_bits0 = 0;
    m_field_override_parent_bits1 = 0;
    m_field_override_parent_bits2 = 0;
    m_field_override_parent_bits3 = 0;
    if (bContentChange)
      Internal_ContentChange();
  }
  else
  {
    const unsigned int field_id_count = static_cast<unsigned int>(MYON_DimStyle::field::Count);
    for (
      unsigned int field_id = static_cast<unsigned int>(MYON_DimStyle::field::Index) + 1;
      field_id < field_id_count;
      field_id++)
    {
      SetFieldOverride(MYON_DimStyle::FieldFromUnsigned(field_id), true);
    }
  }
}

void MYON_DimStyle::OverrideFieldsWithDifferentValues(
  const MYON_DimStyle& source,
  const MYON_DimStyle& parent
)
{
  if (source.ContentHash() == parent.ContentHash())
  {
    SetFieldOverrideAll(false);
  }
  else
  {
    SetFieldOverrideAll(true);
    // OverrideFields() will set to false any fields that have equal property values.
    OverrideFields(source, parent);
  }
}

void MYON_DimStyle::SetFieldOverride(MYON_DimStyle::field field_id, bool bOverrideParent)
{
  // Note well:
  //   The Name and Index properties cannot be inherited from a parent dimension style.
  if (
    static_cast<unsigned int>(field_id) > static_cast<unsigned int>(MYON_DimStyle::field::Index)
    && static_cast<unsigned int>(field_id) < static_cast<unsigned int>(MYON_DimStyle::field::Count)
    && (m_field_override_parent_count > 0 || bOverrideParent)
    )
  {
    MYON__UINT32 mask = 0;
    MYON__UINT32* bits = Internal_GetOverrideParentBit(field_id, &mask);
    if (nullptr != bits)
    {
      const MYON__UINT32 bits0 = *bits;
      if (bOverrideParent)
      {
        // field_id bit = 1 indicates the field value is independent of the parent.
        *bits |= mask;
        if (*bits != bits0)
        {
          m_field_override_parent_count++;
          Internal_ContentChange();
        }
      }
      else
      {
        // field_id bit = 0 indicates the field value is inherited from the parent.
        *bits &= ~mask;
        if (*bits != bits0)
        {
          m_field_override_parent_count--;
          Internal_ContentChange();
        }
      }
    }
  }
}

void MYON_DimStyle::ClearAllFieldOverrides()
{
  SetFieldOverrideAll(false);
}

const MYON_DimStyle MYON_DimStyle::CreateOverrideCandidate() const
{
  MYON_DimStyle override_candidate(*this);
  
  // NOTE:
  // parent_id can be nil. This allows a default constructed dimstyle to be
  // used as an override candidate.
  if ( this->IsOverrideDimStyleCandidate(ParentId(), false))
    return override_candidate;

  MYON_UUID parent_id
    = override_candidate.ParentIdIsNotNil()
    ? override_candidate.ParentId()
    : override_candidate.Id();

  override_candidate.ClearId();
  override_candidate.ClearName();
  override_candidate.ClearIndex();

  if (MYON_nil_uuid == parent_id)
  {
    parent_id = MYON_DimStyle::Default.Id();
  }
  
  override_candidate.SetParentId(parent_id);

  if (override_candidate.HasOverrides())
  {
    const MYON_DimStyle& system_parent = MYON_DimStyle::SystemDimstyleFromId(parent_id);
    if (system_parent.Id() == parent_id)
      override_candidate.OverrideFields(override_candidate, system_parent);
  }

  if (false == override_candidate.IsOverrideDimStyleCandidate(parent_id, false))
  {
    MYON_ERROR("Failed to create valid override candidate.");
  }

  return override_candidate;  
}

const MYON_SHA1_Hash& MYON_DimStyle::ParentContentHash() const
{
  // The value of m_parent_dim_style_content_hash cannot be reliably cleared 
  // because ParentId() is maintained on a base class. So a "lazy" check on
  // a mutable m_parent_dim_style_content_hash is used.
  if (ParentIdIsNil() && m_field_override_parent_count <= 0)
  {
    // no parent at this point.
    m_parent_dim_style_content_hash = MYON_SHA1_Hash::EmptyContentHash;
  }
  return m_parent_dim_style_content_hash;
}

bool MYON_DimStyle::HasOverrides() const
{
  return (m_field_override_parent_count > 0);
}

MYON__UINT32 MYON_DimStyle::OverrideCount() const
{
  return m_field_override_parent_count;
}

void MYON_DimStyle::OverrideFields(const MYON_DimStyle& source, const MYON_DimStyle& parent)
{
  if (ParentId() != parent.Id())
  {
    SetParentId(parent.Id());
  }
  m_parent_dimstyle_managed_font = parent.m_managed_font;

  // leave the Unset, Name, Index fields as is. They cannot be overridden
  for (unsigned int i = static_cast<unsigned int>(MYON_DimStyle::field::Index)+1; i < static_cast<unsigned int>(MYON_DimStyle::field::Count); i++)
  {
    // NOTE WELL:
    //  "this" could be source or parent, so do not modify member values until they are no longer needed.

    const MYON_DimStyle::field field_id = MYON_DimStyle::FieldFromUnsigned(i);
    if (MYON_DimStyle::field::Unset == field_id)
      continue;

    bool bClearOverrideTest = false;

    const MYON_DimStyle* copyfrom;
    if (source.IsFieldOverride(field_id))
    {
      // override the parent setting.
      SetFieldOverride(field_id, true);
      copyfrom = &source; // copy the independent value from source
      bClearOverrideTest = (*this != &parent);
    }
    else
    {
      // inherit the value from the parent
      SetFieldOverride(field_id, false);
      copyfrom = &parent;
    }

    if (this == copyfrom && false == bClearOverrideTest)
      continue; // nothing to do.


    // NOTE: If you change MYON_INTERNAL_UPDATE_PROPERTY, then also update MYON_DimStyle::field::Font: case below
#define MYON_INTERNAL_UPDATE_PROPERTY(PROP) if (this != copyfrom) this->Set ## PROP(copyfrom->PROP()); if (false == bClearOverrideTest || this->PROP() == parent.PROP()) this->SetFieldOverride(field_id, false)

    switch (field_id)
    {
    case MYON_DimStyle::field::ExtensionLineExtension:
      MYON_INTERNAL_UPDATE_PROPERTY(ExtExtension);
      break;
    case MYON_DimStyle::field::ExtensionLineOffset:
      MYON_INTERNAL_UPDATE_PROPERTY(ExtOffset);
      break;
    case MYON_DimStyle::field::Arrowsize:
      MYON_INTERNAL_UPDATE_PROPERTY(ArrowSize);
      break;
    case MYON_DimStyle::field::LeaderArrowsize:
      MYON_INTERNAL_UPDATE_PROPERTY(LeaderArrowSize);
      break;
    case MYON_DimStyle::field::Centermark:
      MYON_INTERNAL_UPDATE_PROPERTY(CenterMark);
      break;
    case MYON_DimStyle::field::TextGap:
      MYON_INTERNAL_UPDATE_PROPERTY(TextGap);
      break;
    case MYON_DimStyle::field::TextHeight:
      MYON_INTERNAL_UPDATE_PROPERTY(TextHeight);
      break;
    case MYON_DimStyle::field::DimTextLocation:
      MYON_INTERNAL_UPDATE_PROPERTY(DimTextLocation);
      break;
    case MYON_DimStyle::field::MaskFrameType:
      MYON_INTERNAL_UPDATE_PROPERTY(MaskFrameType);
      break;
    case MYON_DimStyle::field::LengthResolution:
      MYON_INTERNAL_UPDATE_PROPERTY(LengthResolution);
      break;
    case MYON_DimStyle::field::AngleFormat:
      MYON_INTERNAL_UPDATE_PROPERTY(AngleFormat);
      break;
    case MYON_DimStyle::field::AngleResolution:
      MYON_INTERNAL_UPDATE_PROPERTY(AngleResolution);
      break;
    case MYON_DimStyle::field::Font:
      // SPECIAL CASE
      // The MYON_Font operator == is not the correct choice for a compare
      if (this != copyfrom) 
        SetFont(copyfrom->m_font_characteristics);
      if (false == bClearOverrideTest || m_font_characteristics.FontCharacteristicsHash() == parent.m_font_characteristics.FontCharacteristicsHash())
        SetFieldOverride(field_id, false);
      break;
    case MYON_DimStyle::field::LengthFactor:
      MYON_INTERNAL_UPDATE_PROPERTY(LengthFactor);
      break;
    case MYON_DimStyle::field::Alternate:
      MYON_INTERNAL_UPDATE_PROPERTY(Alternate);
      break;
    case MYON_DimStyle::field::AlternateLengthFactor:
      MYON_INTERNAL_UPDATE_PROPERTY(AlternateLengthFactor);
      break;
    case MYON_DimStyle::field::AlternateLengthResolution:
      MYON_INTERNAL_UPDATE_PROPERTY(AlternateLengthResolution);
      break;
    case MYON_DimStyle::field::Prefix:
      MYON_INTERNAL_UPDATE_PROPERTY(Prefix);
      break;
    case MYON_DimStyle::field::Suffix:
      MYON_INTERNAL_UPDATE_PROPERTY(Suffix);
      break;
    case MYON_DimStyle::field::AlternatePrefix:
      MYON_INTERNAL_UPDATE_PROPERTY(AlternatePrefix);
      break;
    case MYON_DimStyle::field::AlternateSuffix:
      MYON_INTERNAL_UPDATE_PROPERTY(AlternateSuffix);
      break;
    case MYON_DimStyle::field::DimensionLineExtension:
      MYON_INTERNAL_UPDATE_PROPERTY(DimExtension);
      break;
    case MYON_DimStyle::field::SuppressExtension1:
      MYON_INTERNAL_UPDATE_PROPERTY(SuppressExtension1);
      break;
    case MYON_DimStyle::field::SuppressExtension2:
      MYON_INTERNAL_UPDATE_PROPERTY(SuppressExtension2);
      break;
    case MYON_DimStyle::field::ExtLineColorSource:
      MYON_INTERNAL_UPDATE_PROPERTY(ExtensionLineColorSource);
      break;
    case MYON_DimStyle::field::DimLineColorSource:
      MYON_INTERNAL_UPDATE_PROPERTY(DimensionLineColorSource);
      break;
    case MYON_DimStyle::field::ArrowColorSource:
      MYON_INTERNAL_UPDATE_PROPERTY(ArrowColorSource);
      break;
    case MYON_DimStyle::field::TextColorSource:
      MYON_INTERNAL_UPDATE_PROPERTY(TextColorSource);
      break;
    case MYON_DimStyle::field::ExtLineColor:
      MYON_INTERNAL_UPDATE_PROPERTY(ExtensionLineColor);
      break;
    case MYON_DimStyle::field::DimLineColor:
      MYON_INTERNAL_UPDATE_PROPERTY(DimensionLineColor);
      break;
    case MYON_DimStyle::field::ArrowColor:
      MYON_INTERNAL_UPDATE_PROPERTY(ArrowColor);
      break;
    case MYON_DimStyle::field::TextColor:
      MYON_INTERNAL_UPDATE_PROPERTY(TextColor);
      break;
    case MYON_DimStyle::field::ExtLinePlotColorSource:
      MYON_INTERNAL_UPDATE_PROPERTY(ExtensionLinePlotColorSource);
      break;
    case MYON_DimStyle::field::DimLinePlotColorSource:
      MYON_INTERNAL_UPDATE_PROPERTY(DimensionLinePlotColorSource);
      break;
    case MYON_DimStyle::field::ArrowPlotColorSource:
      MYON_INTERNAL_UPDATE_PROPERTY(ArrowPlotColorSource);
      break;
    case MYON_DimStyle::field::TextPlotColorSource:
      MYON_INTERNAL_UPDATE_PROPERTY(TextPlotColorSource);
      break;
    case MYON_DimStyle::field::ExtLinePlotColor:
      MYON_INTERNAL_UPDATE_PROPERTY(ExtensionLinePlotColor);
      break;
    case MYON_DimStyle::field::DimLinePlotColor:
      MYON_INTERNAL_UPDATE_PROPERTY(DimensionLinePlotColor);
      break;
    case MYON_DimStyle::field::ArrowPlotColor:
      MYON_INTERNAL_UPDATE_PROPERTY(ArrowPlotColor);
      break;
    case MYON_DimStyle::field::TextPlotColor:
      MYON_INTERNAL_UPDATE_PROPERTY(TextPlotColor);
      break;
    case MYON_DimStyle::field::ExtLinePlotWeightSource:
      MYON_INTERNAL_UPDATE_PROPERTY(ExtensionLinePlotWeightSource);
      break;
    case MYON_DimStyle::field::DimLinePlotWeightSource:
      MYON_INTERNAL_UPDATE_PROPERTY(DimensionLinePlotWeightSource);
      break;
    case MYON_DimStyle::field::ExtLinePlotWeight_mm:
      MYON_INTERNAL_UPDATE_PROPERTY(ExtensionLinePlotWeight);
      break;
    case MYON_DimStyle::field::DimLinePlotWeight_mm:
      MYON_INTERNAL_UPDATE_PROPERTY(DimensionLinePlotWeight);
      break;
    case MYON_DimStyle::field::ToleranceFormat:
      MYON_INTERNAL_UPDATE_PROPERTY(ToleranceFormat);
      break;
    case MYON_DimStyle::field::ToleranceResolution:
      MYON_INTERNAL_UPDATE_PROPERTY(ToleranceResolution);
      break;
    case MYON_DimStyle::field::ToleranceUpperValue:
      MYON_INTERNAL_UPDATE_PROPERTY(ToleranceUpperValue);
      break;
    case MYON_DimStyle::field::ToleranceLowerValue:
      MYON_INTERNAL_UPDATE_PROPERTY(ToleranceLowerValue);
      break;
    case MYON_DimStyle::field::AltToleranceResolution:
      MYON_INTERNAL_UPDATE_PROPERTY(AlternateToleranceResolution);
      break;
    case MYON_DimStyle::field::ToleranceHeightScale:
      MYON_INTERNAL_UPDATE_PROPERTY(ToleranceHeightScale);
      break;
    case MYON_DimStyle::field::BaselineSpacing:
      MYON_INTERNAL_UPDATE_PROPERTY(BaselineSpacing);
      break;
    case MYON_DimStyle::field::DrawMask:
      MYON_INTERNAL_UPDATE_PROPERTY(DrawTextMask);
      break;
    case MYON_DimStyle::field::MaskColorSource:
      MYON_INTERNAL_UPDATE_PROPERTY(MaskFillType);
      break;
    case MYON_DimStyle::field::MaskColor:
      MYON_INTERNAL_UPDATE_PROPERTY(MaskColor);
      break;
    case MYON_DimStyle::field::MaskBorder:
      MYON_INTERNAL_UPDATE_PROPERTY(MaskBorder);
      break;
    case MYON_DimStyle::field::DimensionScale:
      MYON_INTERNAL_UPDATE_PROPERTY(DimScale);
      break;
    case MYON_DimStyle::field::DimscaleSource:
      MYON_INTERNAL_UPDATE_PROPERTY(DimScaleSource);
      break;
    case MYON_DimStyle::field::FixedExtensionLength:
      MYON_INTERNAL_UPDATE_PROPERTY(FixedExtensionLen);
      break;
    case MYON_DimStyle::field::FixedExtensionOn:
      MYON_INTERNAL_UPDATE_PROPERTY(FixedExtensionLenOn);
      break;
    case MYON_DimStyle::field::TextRotation:
      MYON_INTERNAL_UPDATE_PROPERTY(TextRotation);
      break;
    case MYON_DimStyle::field::SuppressArrow1:
      MYON_INTERNAL_UPDATE_PROPERTY(SuppressArrow1);
      break;
    case MYON_DimStyle::field::SuppressArrow2:
      MYON_INTERNAL_UPDATE_PROPERTY(SuppressArrow2);
      break;
    case MYON_DimStyle::field::TextmoveLeader:
      MYON_INTERNAL_UPDATE_PROPERTY(TextMoveLeader);
      break;
    case MYON_DimStyle::field::ArclengthSymbol:
      MYON_INTERNAL_UPDATE_PROPERTY(ArcLengthSymbol);
      break;
    case MYON_DimStyle::field::StackTextheightScale:
      MYON_INTERNAL_UPDATE_PROPERTY(StackHeightScale);
      break;
    case MYON_DimStyle::field::StackFormat:
      MYON_INTERNAL_UPDATE_PROPERTY(StackFractionFormat);
      break;
    case MYON_DimStyle::field::AltRound:
      MYON_INTERNAL_UPDATE_PROPERTY(AlternateRoundOff);
      break;
    case MYON_DimStyle::field::Round:
      MYON_INTERNAL_UPDATE_PROPERTY(RoundOff);
      break;
    case MYON_DimStyle::field::AngularRound:
      MYON_INTERNAL_UPDATE_PROPERTY(AngleRoundOff);
      break;
    case MYON_DimStyle::field::AltZeroSuppress:
      MYON_INTERNAL_UPDATE_PROPERTY(AlternateZeroSuppress);
      break;
    //case MYON_DimStyle::field::ToleranceZeroSuppress:
    //  MYON_INTERNAL_UPDATE_PROPERTY(ToleranceZeroSuppress);
    //  break;
    case MYON_DimStyle::field::AngleZeroSuppress:
      MYON_INTERNAL_UPDATE_PROPERTY(AngleZeroSuppress);
      break;
    case MYON_DimStyle::field::ZeroSuppress:
      MYON_INTERNAL_UPDATE_PROPERTY(ZeroSuppress);
      break;
    case MYON_DimStyle::field::AltBelow:
      MYON_INTERNAL_UPDATE_PROPERTY(AlternateBelow);
      break;
    case MYON_DimStyle::field::ArrowType1:
      MYON_INTERNAL_UPDATE_PROPERTY(ArrowType1);
      break;
    case MYON_DimStyle::field::ArrowType2:
      MYON_INTERNAL_UPDATE_PROPERTY(ArrowType2);
      break;
    case MYON_DimStyle::field::LeaderArrowType:
      MYON_INTERNAL_UPDATE_PROPERTY(LeaderArrowType);
      break;
    case MYON_DimStyle::field::ArrowBlockId1:
      MYON_INTERNAL_UPDATE_PROPERTY(ArrowBlockId1);
      break;
    case MYON_DimStyle::field::ArrowBlockId2:
      MYON_INTERNAL_UPDATE_PROPERTY(ArrowBlockId2);
      break;
    case MYON_DimStyle::field::LeaderArrowBlock:
      MYON_INTERNAL_UPDATE_PROPERTY(LeaderArrowBlockId);
      break;
    case MYON_DimStyle::field::DimRadialTextLocation:
      MYON_INTERNAL_UPDATE_PROPERTY(DimRadialTextLocation);
      break;
    case MYON_DimStyle::field::TextVerticalAlignment:
      MYON_INTERNAL_UPDATE_PROPERTY(TextVerticalAlignment);
      break;      
    case MYON_DimStyle::field::LeaderTextVerticalAlignment:
      MYON_INTERNAL_UPDATE_PROPERTY(LeaderTextVerticalAlignment);
      break;
    case MYON_DimStyle::field::LeaderContentAngleStyle:
      MYON_INTERNAL_UPDATE_PROPERTY(LeaderContentAngleStyle);
      break;
    case MYON_DimStyle::field::LeaderCurveType:
      MYON_INTERNAL_UPDATE_PROPERTY(LeaderCurveType);
      break;
    case MYON_DimStyle::field::LeaderContentAngle:
      MYON_INTERNAL_UPDATE_PROPERTY(LeaderContentAngleRadians);
      break;
    case MYON_DimStyle::field::LeaderHasLanding:
      MYON_INTERNAL_UPDATE_PROPERTY(LeaderHasLanding);
      break;
    case MYON_DimStyle::field::LeaderLandingLength:
      MYON_INTERNAL_UPDATE_PROPERTY(LeaderLandingLength);
      break;
    case MYON_DimStyle::field::MaskFlags: 
      // SPECIAL CASE
      // field not used   
      SetFieldOverride(MYON_DimStyle::field::MaskFlags, false);
      break;
    case MYON_DimStyle::field::CentermarkStyle:
      MYON_INTERNAL_UPDATE_PROPERTY(CenterMarkStyle);
      break;
    case MYON_DimStyle::field::TextHorizontalAlignment:
      MYON_INTERNAL_UPDATE_PROPERTY(TextHorizontalAlignment);
      break;
    case MYON_DimStyle::field::LeaderTextHorizontalAlignment:
      MYON_INTERNAL_UPDATE_PROPERTY(LeaderTextHorizontalAlignment);
      break;
    case MYON_DimStyle::field::DrawForward:
      MYON_INTERNAL_UPDATE_PROPERTY(DrawForward);
      break;
    case MYON_DimStyle::field::SignedOrdinate:
      MYON_INTERNAL_UPDATE_PROPERTY(SignedOrdinate);
      break;
    case MYON_DimStyle::field::UnitSystem:
      MYON_INTERNAL_UPDATE_PROPERTY(UnitSystem);
      break;
    case MYON_DimStyle::field::TextMask:
      // SPECIAL CASE
      // The TextMask values are all modifed individually by the cases for
      // MYON_DimStyle::field::DrawMask:
      // MYON_DimStyle::field::MaskColorSource:
      // MYON_DimStyle::field::MaskColor:
      // MYON_DimStyle::field::MaskBorder:
      //
      ////HIDEME_SetTextMask(copyfrom->TextMask());
      SetFieldOverride(field_id, false);
      break;
    case MYON_DimStyle::field::TextOrientation:
      MYON_INTERNAL_UPDATE_PROPERTY(TextOrientation);
      break;
    case MYON_DimStyle::field::LeaderTextOrientation:
      MYON_INTERNAL_UPDATE_PROPERTY(LeaderTextOrientation);
      break;
    case MYON_DimStyle::field::DimTextOrientation:
      MYON_INTERNAL_UPDATE_PROPERTY(DimTextOrientation);
      break;
    case MYON_DimStyle::field::DimRadialTextOrientation:
      MYON_INTERNAL_UPDATE_PROPERTY(DimRadialTextOrientation);
      break;
    case MYON_DimStyle::field::DimTextAngleStyle:
      MYON_INTERNAL_UPDATE_PROPERTY(DimTextAngleStyle);
      break;
    case MYON_DimStyle::field::DimRadialTextAngleStyle:
      MYON_INTERNAL_UPDATE_PROPERTY(DimRadialTextAngleStyle);
      break;
    case MYON_DimStyle::field::TextUnderlined:
      MYON_INTERNAL_UPDATE_PROPERTY(TextUnderlined);
      break;
    case MYON_DimStyle::field::DimensionLengthDisplay:
      MYON_INTERNAL_UPDATE_PROPERTY(DimensionLengthDisplay);
      break;
    case MYON_DimStyle::field::AlternateDimensionLengthDisplay:
      MYON_INTERNAL_UPDATE_PROPERTY(AlternateDimensionLengthDisplay);
      break;
    case MYON_DimStyle::field::ForceDimLine:
      MYON_INTERNAL_UPDATE_PROPERTY(ForceDimLine);
      break;
    case MYON_DimStyle::field::TextFit:
      MYON_INTERNAL_UPDATE_PROPERTY(TextFit);
      break;
    case MYON_DimStyle::field::ArrowFit:
      MYON_INTERNAL_UPDATE_PROPERTY(ArrowFit);
      break;
    case MYON_DimStyle::field::DecimalSeparator:
      MYON_INTERNAL_UPDATE_PROPERTY(DecimalSeparator);
      break;
    default:
      MYON_ERROR("The switch statement in this function has gaps!");
      SetFieldOverride(field_id, false);
      break;
    }

#undef MYON_INTERNAL_UPDATE_PROPERTY

    //// DEBUGGING CODE TO BREAK MYON VALUES THAT ARE ACTUALLY OVERRIDDEN
    //if ( bClearOverrideTest && this->IsFieldOverride(field_id) )
    //{
    //  int i = 99;
    //}

  }

  m_parent_dim_style_content_hash = parent.ContentHash();


  return;
}

void MYON_DimStyle::InheritFields(const MYON_DimStyle& parent)
{
  this->OverrideFields(*this, parent);
}

bool MYON_DimStyle::IsChildDimstyle() const
{
  return ParentIdIsNotNil();
}

bool MYON_DimStyle::IsChildOf(const MYON_UUID& parent_id) const
{
  return (parent_id != MYON_nil_uuid && ParentId() == parent_id);
}

MYON_DimStyle::tolerance_format MYON_DimStyle::ToleranceFormat() const
{
  return m_tolerance_format;
}

int MYON_DimStyle::ToleranceResolution() const
{
  return m_tolerance_resolution;
}

double MYON_DimStyle::ToleranceUpperValue() const
{
  return m_tolerance_upper_value;
}

double MYON_DimStyle::ToleranceLowerValue() const
{
  return m_tolerance_lower_value;
}

double MYON_DimStyle::ToleranceHeightScale() const
{
  // Intentionally using StackHeightScale for both fractions and tolerances
  // Tolerances aren't allowed with fractional formats
  return StackHeightScale();
//  return m_tolerance_height_scale;
}

double MYON_DimStyle::BaselineSpacing() const
{
  return m_baseline_spacing;
}

//-------------------

static bool Internal_IsValidDimStyleScale(double scale)
{
  return MYON_IsValidPositiveNumber(scale)  && fabs(scale - 1.0) > MYON_ZERO_TOLERANCE;
}

void MYON_DimStyle::Scale(double scale)
{
  // If you modify this code, be sure to update MYON_Annotation::ScaleOverrideDimstyle().

  if (Internal_IsValidDimStyleScale(scale))
  {
    m_extextension *= scale;
    m_extoffset *= scale;
    m_arrowsize *= scale;
    m_centermark *= scale;
    m_textgap *= scale;
    m_textheight *= scale;
    m_dimextension *= scale;
    m_baseline_spacing *= scale;
    m_fixed_extension_len *= scale;
    m_leaderarrowsize *= scale;
    m_leader_landing_length *= scale;
    SetMaskBorder(MaskBorder() * scale);
    Internal_ContentChange();
  }
}



double Internal_ScaleOverrideLength(
  double parent_dimstyle_length,
  double current_dimstyle_length,
  double scale
)
{
  // returns scaled dimstyle length
  const double x = current_dimstyle_length * scale;
  const double rel_tol = 1.0e-6 * fabs(parent_dimstyle_length);
  const double abs_tol = MYON_ZERO_TOLERANCE;
  const double delta = fabs(x - parent_dimstyle_length);
  return (delta <= rel_tol || delta <= abs_tol) ? parent_dimstyle_length : x;
}

void MYON_Annotation::ScaleOverrideDimstyle(
  const MYON_DimStyle* parent_dimstyle,
  double scale
)
{
  // Dale Lear Setp 2020 https://mcneel.myjetbrains.com/youtrack/issue/RH-60536
  // I added this function to scale the appearance of annotation
  // in layout/page views. Using DimScale() does not work in layout/page
  // views because it only applies to annotation in model space views.

  if (Internal_IsValidDimStyleScale(scale))
  {
    // The list of properties here must exactly match what is in void MYON_DimStyle::Scale(double scale).
    const MYON_DimStyle d0 = MYON_DimStyle::DimStyleOrDefault(parent_dimstyle);

    // x0 = current value (may already be an override and different from the d0 value)
    // x1 = scaled value (may end up being the d0 value)
    double x0, x1;

    // m_extextension *= scale;
    x0 = this->ExtensionLineExtension(parent_dimstyle);
    x1 = Internal_ScaleOverrideLength(d0.ExtExtension(), x0, scale);
    this->SetExtensionLineExtension(parent_dimstyle, x1);

    // m_extoffset *= scale
    x0 = this->ExtensionLineOffset(parent_dimstyle);
    x1 = Internal_ScaleOverrideLength(d0.ExtOffset(), x0, scale);
    this->SetExtensionLineOffset(parent_dimstyle, x1);

    // m_arrowsize *= scale;
    x0 = this->ArrowSize(parent_dimstyle);
    x1 = Internal_ScaleOverrideLength(d0.ArrowSize(), x0, scale);
    this->SetArrowSize(parent_dimstyle, x1);

    // m_centermark *= scale;
    x0 = this->CenterMarkSize(parent_dimstyle);
    x1 = Internal_ScaleOverrideLength(d0.CenterMark(), x0, scale);
    this->SetCenterMarkSize(parent_dimstyle, x1);

    // m_textgap *= scale;
    x0 = this->TextGap(parent_dimstyle);
    x1 = Internal_ScaleOverrideLength(d0.TextGap(), x0, scale);
    this->SetTextGap(parent_dimstyle, x1);
    
    // m_textheight *= scale
    x0 = this->TextHeight(parent_dimstyle);
    x1 = Internal_ScaleOverrideLength(d0.TextHeight(), x0, scale);
    this->SetTextHeight(parent_dimstyle, x1);
    
    // m_dimextension *= scale;
    x0 = this->DimExtension(parent_dimstyle);
    x1 = Internal_ScaleOverrideLength(d0.DimExtension(), x0, scale);
    this->SetDimExtension(parent_dimstyle, x1);
    
    // m_baseline_spacing *= scale;
    x0 = this->BaselineSpacing(parent_dimstyle);
    x1 = Internal_ScaleOverrideLength(d0.BaselineSpacing(), x0, scale);
    this->SetBaselineSpacing(parent_dimstyle, x1);
    
    // m_fixed_extension_len *= scale;
    x0 = this->FixedExtensionLength(parent_dimstyle);
    x1 = Internal_ScaleOverrideLength(d0.FixedExtensionLen(), x0, scale);
    this->SetFixedExtensionLength(parent_dimstyle, x1);
    
    // m_leaderarrowsize *= scale;
    x0 = this->LeaderArrowSize(parent_dimstyle);
    x1 = Internal_ScaleOverrideLength(d0.LeaderArrowSize(), x0, scale);
    this->SetLeaderArrowSize(parent_dimstyle, x1);
    
    // m_leader_landing_length *= scale;
    x0 = this->LeaderLandingLength(parent_dimstyle);
    x1 = Internal_ScaleOverrideLength(d0.LeaderLandingLength(), x0, scale);
    this->SetLeaderLandingLength(parent_dimstyle, x1);
  }

}

void MYON_DimStyle::SetToleranceFormat(MYON_DimStyle::tolerance_format format)
{
  if (m_tolerance_format != format)
  {
    m_tolerance_format = format;
    Internal_ContentChange();
    Internal_TextPositionPropertiesChange();
  }
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::ToleranceFormat);
}

void MYON_DimStyle::SetToleranceResolution(int resolution)
{
  if (resolution >= 0 && resolution < 16)
    Internal_SetIntMember(MYON_DimStyle::field::ToleranceResolution, resolution, m_tolerance_resolution);
}

void MYON_DimStyle::SetToleranceUpperValue(double upper_value)
{
  if (MYON_IsValid(upper_value))
    Internal_SetDoubleMember(MYON_DimStyle::field::ToleranceUpperValue, upper_value, m_tolerance_upper_value);
}

void MYON_DimStyle::SetToleranceLowerValue(double lower_value)
{
  if (MYON_IsValid(lower_value))
    Internal_SetDoubleMember(MYON_DimStyle::field::ToleranceLowerValue, lower_value, m_tolerance_lower_value);
}

void MYON_DimStyle::SetToleranceHeightScale(double scale)
{
  // Intentionally using StackHeightScale for both fractions and tolerances
  // Tolerances aren't allowed with fractional formats
  SetStackHeightScale(scale);
  //if (MYON_IsValid(scale) && scale > MYON_SQRT_EPSILON)
  //  Internal_SetDoubleMember(MYON_DimStyle::field::ToleranceHeightScale, scale, m_tolerance_height_scale);
}

void MYON_DimStyle::SetBaselineSpacing(double spacing)
{
  if (MYON_IsValid(spacing) && spacing > MYON_SQRT_EPSILON)
    Internal_SetDoubleMember(MYON_DimStyle::field::BaselineSpacing, spacing, m_baseline_spacing);
}

const MYON_TextMask& MYON_DimStyle::TextMask() const
{
  return m_text_mask;
}

void MYON_DimStyle::SetTextMask(const MYON_TextMask& mask)
{
  // In order for overrides to work correctly, each text mask property must be
  // set individually.
  const MYON_TextMask local_mask(mask);
  SetDrawTextMask(local_mask.DrawTextMask());
  SetMaskColor(local_mask.MaskColor());
  SetMaskFillType(local_mask.MaskFillType());
  SetMaskBorder(local_mask.MaskBorder());
  SetMaskFrameType(local_mask.MaskFrameType());
}

void MYON_DimStyle::Internal_SetTextMask(
  const MYON_TextMask& text_mask
)
{
  if (m_text_mask != text_mask)
  {
    m_text_mask = text_mask;
    Internal_ContentChange();
  }
  SetFieldOverride(MYON_DimStyle::field::MaskFlags, false); // currently never used.
}

bool MYON_DimStyle::DrawTextMask() const
{
  // This function is for legacy compatibility.
  // In October 2016, text mask information was moved from
  // a collection of individual values on MYON_DimStyle to
  // an MYON_TextMask class and a single MYON_TextMask m_text_mask member
  // on MYON_DimStyle.
  return TextMask().DrawTextMask();
}

void MYON_DimStyle::SetDrawTextMask(bool bDraw)
{
  // This function is for legacy compatibility.
  // In October 2016, text mask information was moved from
  // a collection of individual values on MYON_DimStyle to
  // an MYON_TextMask class and a single MYON_TextMask m_text_mask member
  // on MYON_DimStyle.
  MYON_TextMask text_mask = TextMask();
  text_mask.SetDrawTextMask(bDraw);
  Internal_SetTextMask(text_mask);
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::DrawMask);
}

MYON_TextMask::MaskType  MYON_DimStyle::MaskFillType() const
{
  // This function is for legacy compatibility.
  // In October 2016, text mask information was moved from
  // a collection of individual values on MYON_DimStyle to
  // an MYON_TextMask class and a single MYON_TextMask m_text_mask member
  // on MYON_DimStyle.
  return TextMask().MaskFillType();
}

void MYON_DimStyle::SetMaskFillType(MYON_TextMask::MaskType source)
{
  // This function is for legacy compatibility.
  // In October 2016, text mask information was moved from
  // a collection of individual values on MYON_DimStyle to
  // an MYON_TextMask class and a single MYON_TextMask m_text_mask member
  // on MYON_DimStyle.
  MYON_TextMask text_mask = TextMask();
  text_mask.SetMaskFillType(source);
  Internal_SetTextMask(text_mask);
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::MaskColorSource);
}

MYON_TextMask::MaskFrame  MYON_DimStyle::MaskFrameType() const
{
  // This function is for legacy compatibility.
  // In October 2016, text mask information was moved from
  // a collection of individual values on MYON_DimStyle to
  // an MYON_TextMask class and a single MYON_TextMask m_text_mask member
  // on MYON_DimStyle.
  return TextMask().MaskFrameType();
}

void MYON_DimStyle::SetMaskFrameType(MYON_TextMask::MaskFrame source)
{
  // This function is for legacy compatibility.
  // In October 2016, text mask information was moved from
  // a collection of individual values on MYON_DimStyle to
  // an MYON_TextMask class and a single MYON_TextMask m_text_mask member
  // on MYON_DimStyle.
  MYON_TextMask text_mask = TextMask();
  text_mask.SetMaskFrameType(source);
  Internal_SetTextMask(text_mask);
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::MaskFrameType);
}

MYON_Color MYON_DimStyle::MaskColor() const
{
  // This function is for legacy compatibility.
  // In October 2016, text mask information was moved from
  // a collection of individual values on MYON_DimStyle to
  // an MYON_TextMask class and a single MYON_TextMask m_text_mask member
  // on MYON_DimStyle.
  return TextMask().MaskColor();
}

void MYON_DimStyle::SetMaskColor(MYON_Color color)
{
  // This function is for legacy compatibility.
  // In October 2016, text mask information was moved from
  // a collection of individual values on MYON_DimStyle to
  // an MYON_TextMask class and a single MYON_TextMask m_text_mask member
  // on MYON_DimStyle.
  MYON_TextMask text_mask = TextMask();
  text_mask.SetMaskColor(color);
  Internal_SetTextMask(text_mask);
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::MaskColor);
}

double MYON_DimStyle::MaskBorder() const
{
  // This function is for legacy compatibility.
  // In October 2016, text mask information was moved from
  // a collection of individual values on MYON_DimStyle to
  // an MYON_TextMask class and a single MYON_TextMask m_text_mask member
  // on MYON_DimStyle.
  return TextMask().MaskBorder();
}

void MYON_DimStyle::SetMaskBorder(double border)
{
  // This function is for legacy compatibility.
  // In October 2016, text mask information was moved from
  // a collection of individual values on MYON_DimStyle to
  // an MYON_TextMask class and a single MYON_TextMask m_text_mask member
  // on MYON_DimStyle.
  MYON_TextMask text_mask = TextMask();
  text_mask.SetMaskBorder(border);
  Internal_SetTextMask(text_mask);
  Internal_SetOverrideDimStyleCandidateFieldOverride(MYON_DimStyle::field::MaskBorder);
}

void MYON_DimStyle::SetDimScaleSource(int source)
{
  Internal_SetIntMember(MYON_DimStyle::field::DimscaleSource, source, m_dimscale_source);
}

int MYON_DimStyle::DimScaleSource() const
{
  return m_dimscale_source;
}

void MYON_DimStyle::SetSourceDimstyle(MYON_UUID source_uuid)
{
  // Can be MYON_nil_uuid
  if (m_source_dimstyle != source_uuid)
  {
    m_source_dimstyle = source_uuid;
    Internal_ContentChange();
  }
}

MYON_UUID MYON_DimStyle::SourceDimstyle() const
{
  return m_source_dimstyle;
}

const MYON_DimStyle MYON_DimStyle::CreateFromProperties(
  const MYON_DimStyle& parent_dim_style,
  MYON::AnnotationType annotation_type,
  const MYON_Font* font,
  double model_space_text_scale,
  double text_height,
  MYON::LengthUnitSystem text_height_unit_system
)
{
  MYON_DimStyle dim_style(parent_dim_style.CreateOverrideCandidate());

  Internal_CreateFromProperties(
    parent_dim_style,
    annotation_type,
    font,
    model_space_text_scale,
    text_height,
    text_height_unit_system,
    false,
    MYON::TextVerticalAlignment::Middle,       // value ignored
    MYON::TextHorizontalAlignment::Center,     // value ignored
    false,
    MYON::TextOrientation::InPlane,            // value ignored
    false,
    MYON_DimStyle::TextLocation::AboveDimLine, // value ignored
    dim_style
  );

  return dim_style;
}

const MYON_DimStyle MYON_DimStyle::CreateFromProperties(
  const MYON_DimStyle& parent_dim_style,
  MYON::AnnotationType annotation_type,
  const MYON_Font* font,
  double model_space_text_scale,
  double text_height,
  MYON::LengthUnitSystem text_height_unit_system,
  MYON::TextVerticalAlignment valign,
  MYON::TextHorizontalAlignment halign
)
{
  MYON_DimStyle dim_style(parent_dim_style.CreateOverrideCandidate());

  Internal_CreateFromProperties(
    parent_dim_style,
    annotation_type,
    font,
    model_space_text_scale,
    text_height,
    text_height_unit_system,
    true,
    valign,
    halign,
    false,
    MYON::TextOrientation::InPlane,            // value ignored
    false,
    MYON_DimStyle::TextLocation::AboveDimLine, // value ignored
    dim_style
  );

  return dim_style;
}

const MYON_DimStyle MYON_DimStyle::CreateFromProperties(
  const MYON_DimStyle& parent_dim_style,
  MYON::AnnotationType annotation_type,
  const MYON_Font* font,
  double model_space_text_scale,
  double text_height,
  MYON::LengthUnitSystem text_height_unit_system,
  MYON::TextVerticalAlignment valign,
  MYON::TextHorizontalAlignment halign,
  MYON::TextOrientation orientation,
  MYON_DimStyle::TextLocation dim_text_location
  )
{
  MYON_DimStyle dim_style(parent_dim_style.CreateOverrideCandidate());

  Internal_CreateFromProperties(
    parent_dim_style,
    annotation_type,
    font,
    model_space_text_scale,
    text_height,
    text_height_unit_system,
    true,
    valign,
    halign,
    true,
    orientation,
    true,
    dim_text_location,
    dim_style
  );

  return dim_style;
}

void MYON_DimStyle::Internal_CreateFromProperties(
  const MYON_DimStyle& parent_dim_style,
  MYON::AnnotationType annotation_type,
  const MYON_Font* font,
  double model_space_text_scale,
  double text_height,
  MYON::LengthUnitSystem text_height_unit_system,
  bool bSetAlignment,
  MYON::TextVerticalAlignment valign,
  MYON::TextHorizontalAlignment halign,
  bool bSetOrientation,
  MYON::TextOrientation orientation,
  bool bSetLocation,
  MYON_DimStyle::TextLocation location,
  MYON_DimStyle& destination
)
{
  if (nullptr != font)
  {
    if (0 != MYON_Font::CompareFontCharacteristics(*font, parent_dim_style.Font()))
      destination.SetFont(*font);
  }

  if (model_space_text_scale > 0.0)
  {
    if (fabs(model_space_text_scale - parent_dim_style.DimScale()) > 0.001*model_space_text_scale )
      destination.SetDimScale(model_space_text_scale);
  }

  if (text_height > 0.0)
  {
    double x = MYON::UnitScale(text_height_unit_system, parent_dim_style.UnitSystem())*text_height;
    if (x > 0.0 && fabs(x - parent_dim_style.TextHeight()) > 0.001*x )
      destination.SetTextHeight(x);
  }

  /////////////////////////////////////////////////
  //
  // Use annotation_type parameter to determine what annotation types are effected by the settings
  //
  const bool bAllAnnotationTypes = (MYON::AnnotationType::Unset == annotation_type);
  const bool bDimLinearType 
    = MYON::AnnotationType::Aligned == annotation_type 
    || MYON::AnnotationType::Rotated == annotation_type
    ;
  const bool bDimAngularType 
    = MYON::AnnotationType::Angular == annotation_type
    || MYON::AnnotationType::Angular3pt == annotation_type
    ;
  const bool bDimRadialType 
    = MYON::AnnotationType::Radius == annotation_type
    || MYON::AnnotationType::Diameter == annotation_type
    ;

  const bool bSetTextProps 
    = (bAllAnnotationTypes || MYON::AnnotationType::Text == annotation_type);

  const bool bSetLeaderProps
    = (bAllAnnotationTypes || MYON::AnnotationType::Leader == annotation_type);

  const bool bSetDimProps
    = (bAllAnnotationTypes || bDimLinearType || bDimAngularType || MYON::AnnotationType::Ordinate == annotation_type);

  const bool bSetDimRadialProps
    = (bAllAnnotationTypes || bDimRadialType);

  /////////////////////////////////////////////////
  //
  // Override type specific properties as needed
  //
  if (bSetAlignment)
  {
    const bool bOverrideTextHAlign = bSetTextProps && halign != parent_dim_style.TextHorizontalAlignment();
    const bool bOverrideLeaderHAlign = bSetLeaderProps && halign != parent_dim_style.LeaderTextHorizontalAlignment();
    if (bOverrideTextHAlign)
      destination.SetTextHorizontalAlignment(halign);
    if (bOverrideLeaderHAlign)
      destination.SetLeaderTextHorizontalAlignment(halign);

    const bool bOverrideTextVAlign = bSetTextProps && valign != parent_dim_style.TextVerticalAlignment();
    const bool bOverrideLeaderVAlign = bSetLeaderProps && valign != parent_dim_style.LeaderTextVerticalAlignment();
    if (bOverrideTextVAlign)
      destination.SetTextVerticalAlignment(valign);
    if (bOverrideLeaderVAlign)
      destination.SetLeaderTextVerticalAlignment(valign);
  }
  
  if (bSetOrientation)
  {
    const bool bOverrideTextOrientation = bSetTextProps && orientation != parent_dim_style.TextOrientation();
    const bool bOverrideLeaderOrientation = bSetLeaderProps && orientation != parent_dim_style.LeaderTextOrientation();
    const bool bOverrideDimOrientation = bSetDimProps && orientation != parent_dim_style.DimTextOrientation();
    const bool bOverrideDimRadialOrientation = bSetDimRadialProps && orientation != parent_dim_style.DimRadialTextOrientation();
    if (bOverrideTextOrientation)
      destination.SetTextOrientation(orientation);
    if (bOverrideLeaderOrientation)
      destination.SetLeaderTextOrientation(orientation);
    if (bOverrideDimOrientation)
      destination.SetDimTextOrientation(orientation);
    if (bOverrideDimRadialOrientation)
      destination.SetDimRadialTextOrientation(orientation);
  }

  if (bSetLocation)
  {
    const bool bOverrideDimTextLocation = bSetDimProps && location != parent_dim_style.DimTextLocation();
    const bool bOverrideDimRadialTextLocation = bSetDimRadialProps && location != parent_dim_style.DimRadialTextLocation();
    if (bOverrideDimTextLocation)
      destination.SetDimTextLocation(location);
    if (bOverrideDimRadialTextLocation)
      destination.SetDimRadialTextLocation(location);
  }
}

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

#if !defined(OPENNURBS_APPLE_NSFONT_INC_)
#define OPENNURBS_APPLE_NSFONT_INC_

#if defined(MYON_RUNTIME_APPLE_CORE_TEXT_AVAILABLE)

MYON_DECL
unsigned int MYON_AppleFontGlyphIndex(
  CTFontRef appleFont,
  unsigned int unicode_code_point
);

MYON_DECL
bool MYON_AppleFontGetGlyphMetrics(
  CTFontRef appleFont,
  unsigned int font_design_units_per_M,
  unsigned int glyphIndex,
  class MYON_TextBox& glyph_metrics
);

MYON_DECL
bool MYON_AppleFontGetGlyphOutline(
  CTFontRef appleFont,
  unsigned int font_design_units_per_M,
  unsigned int glyphIndex,
  MYON_OutlineFigure::Type figure_type,
  class MYON_Outline& outline
);
#endif

#endif

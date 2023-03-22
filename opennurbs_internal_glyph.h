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

#if !defined(OPENNURBS_INTERNAL_GLYPH_INC_)
#define OPENNURBS_INTERNAL_GLYPH_INC_

class MYON_Internal_FontGlyphPool : private MYON_FixedSizePool
{
private:
  friend class MYON_FontGlyph;
  friend class MYON_GlyphMap;
  MYON_Internal_FontGlyphPool();
  ~MYON_Internal_FontGlyphPool() = default;
  MYON_Internal_FontGlyphPool(const MYON_Internal_FontGlyphPool&) = delete;
  MYON_Internal_FontGlyphPool operator=(const MYON_Internal_FontGlyphPool&) = delete;
  static MYON_Internal_FontGlyphPool theGlyphItemPool;
};

class MYON_ManagedFonts
{
public:
  // List is the only instance of this class.
  static MYON_ManagedFonts List;

  static const MYON_FontList& InstalledFonts();

  static const MYON_FontList& ManagedFonts()
  {
    return List.m_managed_fonts;
  }

  const MYON_Font* GetFromFontCharacteristics(
    const MYON_Font& font_characteristics,
    bool bCreateIfNotFound
    );

  const MYON_Font* GetFromSerialNumber(
    unsigned int managed_font_runtime_serial_number
    );

#if defined(MYON_OS_WINDOWS_GDI)
  static void Internal_GetWindowsInstalledFonts(MYON_SimpleArray<const MYON_Font*>&);
#endif

#if defined (MYON_RUNTIME_APPLE_CORE_TEXT_AVAILABLE)
  static void Internal_GetAppleInstalledCTFonts(MYON_SimpleArray<const MYON_Font*>& platform_font_list);
#endif

private:
  static void Internal_SetFakeWindowsLogfontNames(
    MYON_SimpleArray<const MYON_Font*>& device_list
  );
  static void Internal_SetFakeWindowsLogfontName(
    const MYON_Font* font,
    const MYON_wString fake_loc_logfont_name,
    const MYON_wString fake_en_logfont_name
  );
public:

  // sorts nulls to end of lists
  static int CompareFontPointer(MYON_Font const* const* lhs, MYON_Font const* const* rhs);

  /*
  Returns:
    0: failure
    >0: success font glyph index
  */
  static unsigned int GetGlyphMetricsInFontDesignUnits(
    const class MYON_Font* font, 
    MYON__UINT32 unicode_code_point,
    class MYON_TextBox& glyph_metrics_in_font_design_units
    );

  /*
  Parameters:
    font - [in]
    font_metrics_in_font_design_units - [out]
  Returns:
    True: 
      font_metrics_in_font_design_units set from a font installed on the
      current device.
    False:
      MYON_FontMetrics::LastResortMetrics used or other corrections applied.
  */

  static bool GetFontMetricsInFontDesignUnits(
    const MYON_Font* font,
    MYON_FontMetrics& font_metrics_in_font_design_units
    );

private:
  // The purpose of this nondefault constructor is to create MYON_ManagedFonts::List 
  // in opennurbs_statics.cpp in a way that Apple's CLang will actually compile.
  // The only instance of MYON_ManagedFonts is MYON_ManagedFonts::List.
  MYON_ManagedFonts(MYON__UINT_PTR zero);

  ~MYON_ManagedFonts();

private:
  MYON_ManagedFonts() = delete;
  MYON_ManagedFonts(const MYON_ManagedFonts&) = delete;
  MYON_ManagedFonts& operator=(const MYON_ManagedFonts&) = delete;

private:
  /*
  Parameters:
    managed_font_metrics_in_font_design_units - [in]
      Pass nullptr if not available.
      If not nullptr, then the values are assumed to be accurate
      and the units are the font design units (not normalized).
  */
  const MYON_Font* Internal_AddManagedFont(
    const MYON_Font* managed_font,
    const MYON_FontMetrics* managed_font_metrics_in_font_design_units // can be nullptr
    );

private:
  MYON__UINT_PTR m_default_font_ptr = 0;

private:
  // Managed fonts used in annotation, etc.
  // They may or may not be installed on this device
  MYON_FontList m_managed_fonts;


private:
  // Fonts installed on this device
  MYON_FontList m_installed_fonts;
};

class MYON_CLASS MYON_GlyphMap
{
public:
  MYON_GlyphMap();
  ~MYON_GlyphMap() = default;

public:
  const class MYON_FontGlyph* FindGlyph(
    const MYON__UINT32 unicode_code_point
  ) const;

  // returns pointer to the persistent glyph item
  const MYON_FontGlyph* InsertGlyph(
    const MYON_FontGlyph& glyph
  );

  unsigned int GlyphCount() const;

private:
  friend class MYON_Font;
  friend class MYON_FontGlyph;
  unsigned int m_glyph_count = 0;
  mutable MYON_SleepLock m_sleep_lock;
  MYON_SimpleArray< const class MYON_FontGlyph* > m_glyphs;
};

#if defined(MYON_OS_WINDOWS_GDI)
/*
Parameters:
  glyph - [in]
  font_metrics - [out]
    font metrics in font design units
Returns:
  >0: glyph index
  0: failed
*/
MYON_DECL
void MYON_WindowsDWriteGetFontMetrics(
  const MYON_Font* font,
  MYON_FontMetrics& font_metrics
);

/*
Parameters:
  glyph - [in]
  glyph_metrics - [out]
    Returns glyph metrics in font design units
Returns:
  >0: glyph index
  0: failed
*/
MYON_DECL
unsigned int MYON_WindowsDWriteGetGlyphMetrics(
  const MYON_FontGlyph* glyph,
  MYON_TextBox& glyph_metrics
);

/*
Parameters:
  glyph - [in]
  bSingleStrokeFont - [in]
  outline - [out]
    outline and metrics in font design units
*/
MYON_DECL
bool MYON_WindowsDWriteGetGlyphOutline(
  const MYON_FontGlyph* glyph,
  MYON_OutlineFigure::Type figure_type,
  class MYON_Outline& outline
);
#endif

#if defined(MYON_RUNTIME_APPLE_CORE_TEXT_AVAILABLE)
/*
Parameters:
  glyph - [in]
  font_metrics - [out]
    font metrics in font design units
Returns:
  >0: glyph index
  0: failed
*/
MYON_DECL
void MYON_AppleFontGetFontMetrics(
  const MYON_Font* font,
  MYON_FontMetrics& font_metrics
);

/*
Parameters:
  glyph - [in]
  glyph_metrics - [out]
    Returns glyph metrics in font design units
Returns:
  >0: glyph index
  0: failed
*/
MYON_DECL
unsigned int MYON_AppleFontGetGlyphMetrics(
  const MYON_FontGlyph* glyph,
  MYON_TextBox& glyph_metrics
);

/*
Parameters:
  glyph - [in]
  figure_type - [in]
    Pass MYON_OutlineFigure::Type::Unset if not known.
  outline - [out]
    outline and metrics in font design units
*/
MYON_DECL
bool MYON_AppleFontGetGlyphOutline(
  const MYON_FontGlyph* glyph,
  MYON_OutlineFigure::Type figure_type,
  class MYON_Outline& outline
);
#endif

#endif

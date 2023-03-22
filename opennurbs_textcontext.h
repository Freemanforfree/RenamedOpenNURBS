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

#ifndef OPENNURBS_TEXTCONTEXT_H_INCLUDED
#define OPENNURBS_TEXTCONTEXT_H_INCLUDED


//----------------------------------------------------------
class MYON_CLASS MYON_TextContext
{
  MYON_TextContext();
public:
  // Use MYON_FontGlyph::GetGlyphList to measure strings.
  static int ConvertCodepointsToString(int cplen, const MYON__UINT32* cp, MYON_wString& str);
  static int ConvertStringToCodepoints(const wchar_t* str, MYON__UINT32*& cp);


  static const MYON_wString FormatRtfString(const wchar_t* rtfstr,
    const MYON_DimStyle* dimstyle,
    bool clear_bold, bool set_bold,
    bool clear_italic, bool set_italic,
    bool clear_underline, bool set_underline,
    bool clear_facename, bool set_facename, const wchar_t* override_facename);

  static bool RtfFirstCharProperties(const wchar_t* rtfstr,
    bool& bold, bool& italic, bool& underline, MYON_wString& facename);
};




#endif


                                                                            

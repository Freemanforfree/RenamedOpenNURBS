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
////////////////////////////////////////////////////////////////

#include "opennurbs.h"
//#include "opennurbs_text.h"
//
//#ifdef RHRICHTEXT
//
//const MYON_GlyphItem* MYON_GlyphMap::FindGlyph(MYON__UINT32 glyph_codepoint)
//{
//  for(int i = 0; i < m_glyphs.Count(); i++)
//  {
//    if(0 == m_glyphs[i])
//      continue;
//    if(m_glyphs[i]->m_codepoint == glyph_codepoint)
//      return m_glyphs[i];
//    if(m_glyphs[i]->m_codepoint > glyph_codepoint)
//      break;
//  }
//  return 0;
//}
//
//void MYON_GlyphMap::InsertGlyph(MYON_GlyphItem item)
//{
//  int i = 0;
//  for(i = 0; i < m_glyphs.Count(); i++)
//  {
//    if(m_glyphs[i]->m_codepoint == item.m_codepoint)
//      return;
//    if(m_glyphs[i]->m_codepoint > item.m_codepoint)
//      break;
//  }
//  MYON_GlyphItem* newitem = MYON_GlyphMap::GlyphPool.AllocGlyph();
//  if(0 != newitem)
//  {
//    *newitem = item;
//    if(i < m_glyphs.Count())
//      m_glyphs.Insert(i, newitem);
//    else
//      m_glyphs.Append(newitem);
//  }
//}
//
//void MYON_GlyphMap::ReplaceGlyph(MYON__UINT32 codepoint, MYON_GlyphItem new_item)
//{
//  MYON_GlyphItem* gi = const_cast< MYON_GlyphItem* >(FindGlyph(codepoint));
//  if(0 != gi)
//    *gi = new_item;
//  else
//    InsertGlyph(new_item);
//}
//
//
//#endif // RHRICHTEXT

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

#define MYON_RDK_RENDER_CHANNELS        L"render-channels"
  #define MYON_RDK_RCH_MODE               L"mode"
  #define   MYON_RDK_RCH_MODE_AUTOMATIC     L"automatic"
  #define   MYON_RDK_RCH_MODE_CUSTOM        L"custom"
  #define MYON_RDK_RCH_LIST               L"list"

class MYON_RenderChannels::CImpl : public MYON_InternalXMLImpl
{
public:
  CImpl() { }
  CImpl(MYON_XMLNode& n) : MYON_InternalXMLImpl(&n) { }
};

static const wchar_t* XMLPath(void)
{
  return MYON_RDK_DOCUMENT   MYON_RDK_SLASH  MYON_RDK_SETTINGS  MYON_RDK_SLASH
         MYON_RDK_RENDERING  MYON_RDK_SLASH  MYON_RDK_RENDER_CHANNELS;
}

MYON_RenderChannels::MYON_RenderChannels()
{
  m_impl = new CImpl;
}

MYON_RenderChannels::MYON_RenderChannels(MYON_XMLNode& model_node)
{
  m_impl = new CImpl(model_node);
}

MYON_RenderChannels::MYON_RenderChannels(const MYON_RenderChannels& rch)
{
  m_impl = new CImpl;
  operator = (rch);
}

MYON_RenderChannels::~MYON_RenderChannels()
{
  delete m_impl;
  m_impl = nullptr;
}

const MYON_RenderChannels& MYON_RenderChannels::operator = (const MYON_RenderChannels& rch)
{
  if (this != &rch)
  {
    SetMode(rch.Mode());

    MYON_SimpleArray<MYON_UUID> chan;
    rch.GetCustomList(chan);
    SetCustomList(chan);
  }

  return *this;
}

static MYON_wString GetSortedSemicolonDelimitedString(const MYON_SimpleArray<MYON_UUID>& chan)
{
  MYON_wString s;

  MYON_ClassArray<MYON_wString> a;

  for (int i = 0; i < chan.Count(); i++)
  {
    MYON_UuidToString(chan[i], s);
    a.Append(s);
  }

  a.QuickSort(MYON_CompareIncreasing);

  s = L"";
  for (int i = 0; i < a.Count(); i++)
  {
    s += a[i];
    if (i < (a.Count() - 1))
      s += L";";
  }

  return s;
}

static MYON_wString GetSortedCustomListAsString(const MYON_RenderChannels& rch)
{
  MYON_SimpleArray<MYON_UUID> chan;
  rch.GetCustomList(chan);

  return GetSortedSemicolonDelimitedString(chan);
}

bool MYON_RenderChannels::operator == (const MYON_RenderChannels& rch)
{
  if (Mode() != rch.Mode())
    return false;

  MYON_ClassArray<MYON_wString> a, b;
  const MYON_wString s1 = GetSortedCustomListAsString(*this);
  const MYON_wString s2 = GetSortedCustomListAsString(rch);
  if (s1 != s2)
    return false;

  return true;
}

bool MYON_RenderChannels::operator != (const MYON_RenderChannels& rch)
{
  return !(operator == (rch));
}

MYON_RenderChannels::Modes MYON_RenderChannels::Mode(void) const
{
  auto mode = Modes::Automatic;

  const MYON_wString s = m_impl->GetParameter(XMLPath(), MYON_RDK_RCH_MODE, MYON_RDK_RCH_MODE_AUTOMATIC).AsString();
  if (MYON_RDK_RCH_MODE_CUSTOM == s)
    mode = Modes::Custom;

  return mode;
}

void MYON_RenderChannels::SetMode(Modes m)
{
  MYON_wString s = MYON_RDK_RCH_MODE_AUTOMATIC;
  if (Modes::Custom == m)
    s = MYON_RDK_RCH_MODE_CUSTOM;

  m_impl->SetParameter(XMLPath(), MYON_RDK_RCH_MODE, s);
}

void MYON_RenderChannels::GetCustomList(MYON_SimpleArray<MYON_UUID>& chan) const
{
  MYON_wString s = m_impl->GetParameter(XMLPath(), MYON_RDK_RCH_LIST, false).AsString();
  const int len = s.Length();
  if (len == 0)
    return;

  if (s[len-1] != L';')
    s += L';';

  int pos = -1;
  while ((pos = s.Find(L";")) >= 0)
  {
    const MYON_UUID uuid = MYON_UuidFromString(s.Left(pos));
    chan.Append(uuid);
    s = s.Mid(pos+1);
  }
}

void MYON_RenderChannels::SetCustomList(const MYON_SimpleArray<MYON_UUID>& chan)
{
  const MYON_wString s = GetSortedSemicolonDelimitedString(chan);
  m_impl->SetParameter(XMLPath(), MYON_RDK_RCH_LIST, s);
}

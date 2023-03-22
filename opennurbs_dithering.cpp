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

// These settings are inside the MYON_RDK_RENDERING section.

#define MYON_USE_DITHERING                L"use-dithering"
#define MYON_DITHERING_METHOD             L"dithering"
#define    MYON_DITHERING_FLOYD_STEINBERG   L"floyd-steinberg"
#define    MYON_DITHERING_SIMPLE_NOISE      L"simple-noise"

class MYON_Dithering::CImpl : public MYON_InternalXMLImpl
{
public:
  CImpl() { }
  CImpl(MYON_XMLNode& n) : MYON_InternalXMLImpl(&n) { }
};

static const wchar_t* XMLPathDit(void)
{
  return MYON_RDK_DOCUMENT  MYON_RDK_SLASH  MYON_RDK_SETTINGS  MYON_RDK_SLASH  MYON_RDK_RENDERING;
}

MYON_Dithering::MYON_Dithering()
{
  m_impl = new CImpl;
}

MYON_Dithering::MYON_Dithering(MYON_XMLNode& model_node)
{
  m_impl = new CImpl(model_node);
}

MYON_Dithering::MYON_Dithering(const MYON_Dithering& dit)
{
  m_impl = new CImpl;
  operator = (dit);
}

MYON_Dithering::~MYON_Dithering()
{
  delete m_impl;
  m_impl = nullptr;
}

const MYON_Dithering& MYON_Dithering::operator = (const MYON_Dithering& dit)
{
  if (this != &dit)
  {
    SetOn    (dit.On());
    SetMethod(dit.Method());
  }

  return *this;
}

bool MYON_Dithering::operator == (const MYON_Dithering& dit)
{
  if (On()     != dit.On())     return false;
  if (Method() != dit.Method()) return false;

  return true;
}

bool MYON_Dithering::operator != (const MYON_Dithering& dit)
{
  return !(operator == (dit));
}

bool MYON_Dithering::On(void) const
{
  return m_impl->GetParameter(XMLPathDit(), MYON_USE_DITHERING, false);
}

void MYON_Dithering::SetOn(bool b)
{
  m_impl->SetParameter(XMLPathDit(), MYON_USE_DITHERING, b);
}

MYON_Dithering::Methods MYON_Dithering::Method(void) const
{
  const MYON_wString s = m_impl->GetParameter(XMLPathDit(), MYON_DITHERING_METHOD, L"").AsString();
  if (MYON_DITHERING_FLOYD_STEINBERG == s)
    return Methods::FloydSteinberg;

  return Methods::SimpleNoise;
}

void MYON_Dithering::SetMethod(Methods m)
{
  const wchar_t* wsz = MYON_DITHERING_SIMPLE_NOISE;
  if (Methods::FloydSteinberg == m)
    wsz = MYON_DITHERING_FLOYD_STEINBERG;

  m_impl->SetParameter(XMLPathDit(), MYON_DITHERING_METHOD, wsz);
}

MYON__UINT32 MYON_Dithering::DataCRC(MYON__UINT32 crc) const
{
  const bool b = On();
  crc = MYON_CRC32(crc, sizeof(b), &b);

  const auto m = Method();
  crc = MYON_CRC32(crc, sizeof(m), &m);

  return crc;
}

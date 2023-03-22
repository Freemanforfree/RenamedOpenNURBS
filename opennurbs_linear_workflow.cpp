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

class MYON_LinearWorkflow::CImpl : public MYON_InternalXMLImpl
{
public:
  CImpl() { }
  CImpl(MYON_XMLNode& n) : MYON_InternalXMLImpl(&n) { }
};

static const wchar_t* XMLPath(void)
{
  return MYON_RDK_DOCUMENT  MYON_RDK_SLASH  MYON_RDK_SETTINGS  MYON_RDK_SLASH  MYON_RDK_RENDERING;
}

MYON_LinearWorkflow::MYON_LinearWorkflow()
{
  m_impl = new CImpl;
}

MYON_LinearWorkflow::MYON_LinearWorkflow(MYON_XMLNode& model_node)
{
  m_impl = new CImpl(model_node);
}

MYON_LinearWorkflow::MYON_LinearWorkflow(const MYON_LinearWorkflow& lw)
{
  m_impl = new CImpl;
  operator = (lw);
}

MYON_LinearWorkflow::~MYON_LinearWorkflow()
{
  delete m_impl;
  m_impl = nullptr;
}

const MYON_LinearWorkflow& MYON_LinearWorkflow::operator = (const MYON_LinearWorkflow& lw)
{
  if (this != &lw)
  {
    SetPreProcessTextures(lw.PreProcessTextures());
    SetPreProcessColors  (lw.PreProcessColors());
    SetPreProcessGamma   (lw.PreProcessGamma());
    SetPostProcessGamma  (lw.PostProcessGamma());
    SetPostProcessGammaOn(lw.PostProcessGammaOn());
  }

  return *this;
}

bool MYON_LinearWorkflow::operator == (const MYON_LinearWorkflow& lw)
{
  if (PreProcessTextures()     != lw.PreProcessTextures())      return false;
  if (PreProcessColors()       != lw.PreProcessColors())        return false;
  if (PostProcessGammaOn()     != lw.PostProcessGammaOn())      return false;
  if (!IsFloatEqual(PreProcessGamma(),  lw.PreProcessGamma()))  return false;
  if (!IsFloatEqual(PostProcessGamma(), lw.PostProcessGamma())) return false;

  return true;
}

bool MYON_LinearWorkflow::operator != (const MYON_LinearWorkflow& sf)
{
  return !(operator == (sf));
}

bool MYON_LinearWorkflow::PreProcessTextures(void) const
{
	return m_impl->GetParameter(XMLPath(), MYON_RDK_USE_LINEAR_WORKFLOW, false);
}

void MYON_LinearWorkflow::SetPreProcessTextures(bool b)
{
		m_impl->SetParameter(XMLPath(), MYON_RDK_USE_LINEAR_WORKFLOW, b);
}

bool MYON_LinearWorkflow::PreProcessColors(void) const
{
	return PreProcessTextures();
}

void MYON_LinearWorkflow::SetPreProcessColors(bool b)
{
  return SetPreProcessTextures(b);
}

float MYON_LinearWorkflow::PreProcessGamma(void) const
{
  const float f = m_impl->GetParameter(XMLPath(), MYON_RDK_GAMMA, 2.2f).AsFloat();
  return std::min(5.0f, std::max(0.2f, f));
}

void MYON_LinearWorkflow::SetPreProcessGamma(float gamma)
{
  const float f = std::min(5.0f, std::max(0.2f, gamma));
  m_impl->SetParameter(XMLPath(), MYON_RDK_GAMMA, f);
}

bool MYON_LinearWorkflow::PostProcessGammaOn(void) const
{
	return m_impl->GetParameter(XMLPath(), MYON_RDK_USE_POST_PROCESS_GAMMA, true);
}

void MYON_LinearWorkflow::SetPostProcessGammaOn(bool on)
{
	m_impl->SetParameter(XMLPath(), MYON_RDK_USE_POST_PROCESS_GAMMA, on);
}

bool MYON_LinearWorkflow::PostProcessFrameBuffer(void) const
{
  return true; // Always on. For possible future use.
}

void MYON_LinearWorkflow::SetPostProcessFrameBuffer(bool)
{
	// Always on. Ignore the call.
}

float MYON_LinearWorkflow::PostProcessGamma(void) const
{
  return PreProcessGamma();
}

void MYON_LinearWorkflow::SetPostProcessGamma(float gamma)
{
  SetPreProcessGamma(gamma);
}

float MYON_LinearWorkflow::PostProcessGammaReciprocal(void) const
{
  return 1.0f / PostProcessGamma();
}

void MYON_LinearWorkflow::ApplyPreProcessGamma(MYON_4fColor& col, bool for_texture) const
{
  const bool check = for_texture ? PreProcessTextures() : PreProcessColors();
  if (!check)
    return;

   const float gamma = PreProcessGamma();
   if (!IsFloatEqual(gamma, 1.0f))
   {
     float* f = col.FloatArray();

     MYON_ASSERT((f[0] >= 0.0) && (f[1] >= 0.0) && (f[2] >= 0.0));

     if (f[0] > 0.0) f[0] = powf(f[0], gamma);
     if (f[1] > 0.0) f[1] = powf(f[1], gamma);
     if (f[2] > 0.0) f[2] = powf(f[2], gamma);
   }
}

MYON__UINT32 MYON_LinearWorkflow::DataCRC(MYON__UINT32 crc) const
{
  bool b[] = { PreProcessTextures(), PostProcessGammaOn(), PreProcessColors() };
  crc = MYON_CRC32(crc, sizeof(b), b);

  MYON__INT64 f[] = { Integerize(PreProcessGamma()), Integerize(PostProcessGamma()) };
  crc = MYON_CRC32(crc, sizeof(f), f);

  return crc;
}

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

#define MYON_RDK_SKYLIGHT_ON                      L"skylight-on"
#define MYON_RDK_SKYLIGHT_SHADOW_INTENSITY        L"skylight-shadow-intensity"
#define MYON_RDK_SKYLIGHT_CUSTOM_ENVIRONMENT_ON   L"skylight-custom-environment-on"
#define MYON_RDK_SKYLIGHT_CUSTOM_ENVIRONMENT      L"skylight-custom-environment"

class MYON_Skylight::CImpl : public MYON_InternalXMLImpl
{
public:
  CImpl() { }
  CImpl(MYON_XMLNode& n) : MYON_InternalXMLImpl(&n) { }
};

static const wchar_t* XMLPath(void)
{
  return MYON_RDK_DOCUMENT  MYON_RDK_SLASH  MYON_RDK_SETTINGS  MYON_RDK_SLASH  MYON_RDK_SUN;
}

MYON_Skylight::MYON_Skylight()
{
  m_impl = new CImpl;
}

MYON_Skylight::MYON_Skylight(MYON_XMLNode& model_node)
{
  m_impl = new CImpl(model_node);
}

MYON_Skylight::MYON_Skylight(const MYON_Skylight& sl)
{
  m_impl = new CImpl;
  operator = (sl);
}

MYON_Skylight::~MYON_Skylight()
{
  delete m_impl;
  m_impl = nullptr;
}

const MYON_Skylight& MYON_Skylight::operator = (const MYON_Skylight& sl)
{
  if (this != &sl)
  {
    SetOn                 (sl.On());
    SetCustomEnvironmentOn(sl.CustomEnvironmentOn());
    SetCustomEnvironment  (sl.CustomEnvironment());
    SetShadowIntensity    (sl.ShadowIntensity());
  }

  return *this;
}

bool MYON_Skylight::operator == (const MYON_Skylight& sl)
{
  if (On()                  != sl.On()                 ) return false;
  if (CustomEnvironmentOn() != sl.CustomEnvironmentOn()) return false;
  if (CustomEnvironment()   != sl.CustomEnvironment()  ) return false;
  if (ShadowIntensity()     != sl.ShadowIntensity()    ) return false;

  return true;
}

bool MYON_Skylight::operator != (const MYON_Skylight& gp)
{
  return !(operator == (gp));
}

bool MYON_Skylight::On(void) const
{
  return m_impl->GetParameter(XMLPath(), MYON_RDK_SKYLIGHT_ON, false).AsBool();
}

void MYON_Skylight::SetOn(bool b)
{
  m_impl->SetParameter(XMLPath(), MYON_RDK_SKYLIGHT_ON, b);
}

bool MYON_Skylight::CustomEnvironmentOn(void) const
{
  return m_impl->GetParameter(XMLPath(), MYON_RDK_SKYLIGHT_CUSTOM_ENVIRONMENT_ON, false).AsBool();
}

void MYON_Skylight::SetCustomEnvironmentOn(bool on)
{
  m_impl->SetParameter(XMLPath(), MYON_RDK_SKYLIGHT_CUSTOM_ENVIRONMENT_ON, on);
}

MYON_UUID MYON_Skylight::CustomEnvironment(void) const
{
  return m_impl->GetParameter(XMLPath(), MYON_RDK_SKYLIGHT_CUSTOM_ENVIRONMENT, MYON_nil_uuid).AsUuid();
}

void MYON_Skylight::SetCustomEnvironment(const MYON_UUID& uuid)
{
  m_impl->SetParameter(XMLPath(), MYON_RDK_SKYLIGHT_CUSTOM_ENVIRONMENT, uuid);
}

double MYON_Skylight::ShadowIntensity(void) const
{
  return m_impl->GetParameter(XMLPath(), MYON_RDK_SKYLIGHT_SHADOW_INTENSITY, 1.0).AsDouble();
}

void MYON_Skylight::SetShadowIntensity(double d)
{
  m_impl->SetParameter(XMLPath(), MYON_RDK_SKYLIGHT_SHADOW_INTENSITY, d);
}

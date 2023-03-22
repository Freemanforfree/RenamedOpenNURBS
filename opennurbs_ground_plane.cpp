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

#define MYON_RDK_GROUND_PLANE           L"ground-plane"
#define    MYON_RDK_GP_ON                 L"on"
#define    MYON_RDK_GP_ALTITUDE           L"altitude"
#define    MYON_RDK_GP_MATERIAL           L"material"
#define    MYON_RDK_GP_TEXTURE_OFFSET     L"texture-offset"
#define    MYON_RDK_GP_TEXTURE_SIZE       L"texture-size"
#define    MYON_RDK_GP_TEXTURE_ROTATION   L"texture-rotation"
#define    MYON_RDK_GP_OFFSET_LOCK        L"offset-lock"
#define    MYON_RDK_GP_REPEAT_LOCK        L"repeat-lock"
#define    MYON_RDK_GP_SHOW_UNDERSIDE     L"show-underside"
#define    MYON_RDK_GP_AUTO_ALTITUDE      L"auto-altitude"
#define    MYON_RDK_GP_SHADOW_ONLY        L"shadow-only"

class MYON_GroundPlane::CImpl : public MYON_InternalXMLImpl
{
public:
  CImpl() { }
  CImpl(MYON_XMLNode& n) : MYON_InternalXMLImpl(&n) { }
};

static const wchar_t* XMLPath(void)
{
  return MYON_RDK_DOCUMENT  MYON_RDK_SLASH  MYON_RDK_SETTINGS  MYON_RDK_SLASH  MYON_RDK_GROUND_PLANE;
}

MYON_GroundPlane::MYON_GroundPlane()
{
  m_impl = new CImpl;
}

MYON_GroundPlane::MYON_GroundPlane(MYON_XMLNode& model_node)
{
  m_impl = new CImpl(model_node);
}

MYON_GroundPlane::MYON_GroundPlane(const MYON_GroundPlane& gp)
{
  m_impl = new CImpl;
  operator = (gp);
}

MYON_GroundPlane::~MYON_GroundPlane()
{
  delete m_impl;
  m_impl = nullptr;
}

const MYON_GroundPlane& MYON_GroundPlane::operator = (const MYON_GroundPlane& gp)
{
  if (this != &gp)
  {
    SetOn                 (gp.On());
    SetShowUnderside      (gp.ShowUnderside());
    SetAutoAltitude       (gp.AutoAltitude());
    SetShadowOnly         (gp.ShadowOnly());
    SetMaterialInstanceId (gp.MaterialInstanceId());
    SetTextureOffset      (gp.TextureOffset());
    SetTextureOffsetLocked(gp.TextureOffsetLocked());
    SetTextureRepeatLocked(gp.TextureRepeatLocked());
    SetTextureSize        (gp.TextureSize());
    SetTextureRotation    (gp.TextureRotation());
    SetAltitude           (gp.Altitude());
  }

  return *this;
}

bool MYON_GroundPlane::operator == (const MYON_GroundPlane& gp)
{
  if (On()                  != gp.On()                 ) return false;
  if (ShowUnderside()       != gp.ShowUnderside()      ) return false;
  if (AutoAltitude()        != gp.AutoAltitude()       ) return false;
  if (ShadowOnly()          != gp.ShadowOnly()         ) return false;
  if (MaterialInstanceId()  != gp.MaterialInstanceId() ) return false;
  if (TextureOffset()       != gp.TextureOffset()      ) return false;
  if (TextureOffsetLocked() != gp.TextureOffsetLocked()) return false;
  if (TextureRepeatLocked() != gp.TextureRepeatLocked()) return false;
  if (TextureSize()         != gp.TextureSize()        ) return false;

  if (!IsDoubleEqual(Altitude(),        gp.Altitude()))        return false;
  if (!IsDoubleEqual(TextureRotation(), gp.TextureRotation())) return false;

  return true;
}

bool MYON_GroundPlane::operator != (const MYON_GroundPlane& gp)
{
  return !(operator == (gp));
}

bool MYON_GroundPlane::On(void) const
{
  return m_impl->GetParameter(XMLPath(), MYON_RDK_GP_ON, false).AsBool();
}

bool MYON_GroundPlane::ShowUnderside(void) const
{
  return m_impl->GetParameter(XMLPath(), MYON_RDK_GP_SHOW_UNDERSIDE, false).AsBool();
}

double MYON_GroundPlane::Altitude(void) const
{
  return m_impl->GetParameter(XMLPath(), MYON_RDK_GP_ALTITUDE, 0.0).AsDouble();
}

bool MYON_GroundPlane::AutoAltitude(void) const
{
  return m_impl->GetParameter(XMLPath(), MYON_RDK_GP_AUTO_ALTITUDE, false).AsBool();
}

bool MYON_GroundPlane::ShadowOnly(void) const
{
  return m_impl->GetParameter(XMLPath(), MYON_RDK_GP_SHADOW_ONLY, true).AsBool();
}

MYON_UUID MYON_GroundPlane::MaterialInstanceId(void) const
{
  return m_impl->GetParameter(XMLPath(), MYON_RDK_GP_MATERIAL, MYON_nil_uuid).AsUuid();
}

MYON_2dVector MYON_GroundPlane::TextureOffset(void) const
{
  return MYON_2dVector(m_impl->GetParameter(XMLPath(), MYON_RDK_GP_TEXTURE_OFFSET, MYON_nil_uuid).As2dPoint());
}

bool MYON_GroundPlane::TextureOffsetLocked(void) const
{
  return m_impl->GetParameter(XMLPath(), MYON_RDK_GP_OFFSET_LOCK, false).AsBool();
}

bool MYON_GroundPlane::TextureRepeatLocked(void) const
{
  return m_impl->GetParameter(XMLPath(), MYON_RDK_GP_REPEAT_LOCK, false).AsBool();
}

MYON_2dVector MYON_GroundPlane::TextureSize(void) const
{
  return MYON_2dVector(m_impl->GetParameter(XMLPath(), MYON_RDK_GP_TEXTURE_SIZE, MYON_nil_uuid).As2dPoint());
}

double MYON_GroundPlane::TextureRotation(void) const
{
  return m_impl->GetParameter(XMLPath(), MYON_RDK_GP_TEXTURE_ROTATION, false).AsDouble();
}

void MYON_GroundPlane::SetOn(bool b)
{
  m_impl->SetParameter(XMLPath(), MYON_RDK_GP_ON, b);
}

void MYON_GroundPlane::SetShowUnderside(bool b)
{
  m_impl->SetParameter(XMLPath(), MYON_RDK_GP_SHOW_UNDERSIDE, b);
}

void MYON_GroundPlane::SetAltitude(double d)
{
  m_impl->SetParameter(XMLPath(), MYON_RDK_GP_ALTITUDE, d);
}

void MYON_GroundPlane::SetAutoAltitude(bool b)
{
  m_impl->SetParameter(XMLPath(), MYON_RDK_GP_AUTO_ALTITUDE, b);
}

void MYON_GroundPlane::SetShadowOnly(bool b)
{
  m_impl->SetParameter(XMLPath(), MYON_RDK_GP_SHADOW_ONLY, b);
}

void MYON_GroundPlane::SetMaterialInstanceId(const MYON_UUID& u)
{
  m_impl->SetParameter(XMLPath(), MYON_RDK_GP_MATERIAL, u);
}

void MYON_GroundPlane::SetTextureOffset(const MYON_2dVector& v)
{
  const MYON_2dPoint p = v;
  m_impl->SetParameter(XMLPath(), MYON_RDK_GP_TEXTURE_OFFSET, p);
}

void MYON_GroundPlane::SetTextureOffsetLocked(bool b)
{
  m_impl->SetParameter(XMLPath(), MYON_RDK_GP_OFFSET_LOCK, b);
}

void MYON_GroundPlane::SetTextureRepeatLocked(bool b)
{
  m_impl->SetParameter(XMLPath(), MYON_RDK_GP_REPEAT_LOCK, b);
}

void MYON_GroundPlane::SetTextureSize(const MYON_2dVector& v)
{
  const MYON_2dPoint p = v;
  m_impl->SetParameter(XMLPath(), MYON_RDK_GP_TEXTURE_SIZE, p);
}

void MYON_GroundPlane::SetTextureRotation(double d)
{
  m_impl->SetParameter(XMLPath(), MYON_RDK_GP_TEXTURE_ROTATION, d);
}

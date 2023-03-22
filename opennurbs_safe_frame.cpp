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

#define MYON_RDK_SAFE_FRAME              L"safe-frame"
#define    MYON_RDK_SF_ON                  L"on"
#define    MYON_RDK_SF_PERSPECTIVE_ONLY    L"perspective-only"
#define    MYON_RDK_SF_FIELD_DISPLAY_ON    L"field-display-on"
#define    MYON_RDK_SF_LIVE_FRAME          L"live-frame"
#define    MYON_RDK_SF_ACTION_FRAME        L"action-frame"
#define    MYON_RDK_SF_TITLE_FRAME         L"title-frame"
#define       MYON_RDK_SFF_ON                L"on"
#define       MYON_RDK_SFF_XSCALE            L"x-scale"
#define       MYON_RDK_SFF_YSCALE            L"y-scale"
#define       MYON_RDK_SFF_LINK              L"link"

class MYON_SafeFrame::CImpl : public MYON_InternalXMLImpl
{
public:
  CImpl() { }
  CImpl(MYON_XMLNode& n) : MYON_InternalXMLImpl(&n) { }

  MYON_XMLVariant GetBaseParameter(const wchar_t* param_name, const MYON_XMLVariant& def) const;
  bool SetBaseParameter(const wchar_t* param_name, const MYON_XMLVariant& value);

  MYON_XMLVariant GetFrameParameter(const wchar_t* frame_name, const wchar_t* param_name, const MYON_XMLVariant& def) const;
  bool SetFrameParameter(const wchar_t* frame_name, const wchar_t* param_name, const MYON_XMLVariant& value);
};

static const wchar_t* XMLPathBase(void)
{
  return MYON_RDK_DOCUMENT  MYON_RDK_SLASH  MYON_RDK_SETTINGS  MYON_RDK_SLASH  MYON_RDK_SAFE_FRAME;
}

static MYON_wString XMLPath(const MYON_wString& section_name)
{
  MYON_wString s = XMLPathBase();
  MYON_ASSERT(section_name.IsNotEmpty());
  s += MYON_RDK_SLASH;
  s += section_name;

  return s;
}

MYON_XMLVariant MYON_SafeFrame::CImpl::GetBaseParameter(const wchar_t* param_name, const MYON_XMLVariant& def) const
{
  return GetParameter(XMLPathBase(), param_name, def);
}

bool MYON_SafeFrame::CImpl::SetBaseParameter(const wchar_t* param_name, const MYON_XMLVariant& value)
{
  return SetParameter(XMLPathBase(), param_name, value);
}

MYON_XMLVariant MYON_SafeFrame::CImpl::GetFrameParameter(const wchar_t* frame_name, const wchar_t* param_name, const MYON_XMLVariant& def) const
{
  return GetParameter(XMLPath(frame_name), param_name, def);
}

bool MYON_SafeFrame::CImpl::SetFrameParameter(const wchar_t* frame_name, const wchar_t* param_name, const MYON_XMLVariant& value)
{
  return SetParameter(XMLPath(frame_name), param_name, value);
}

MYON_SafeFrame::MYON_SafeFrame()
{
  m_impl = new CImpl;
}

MYON_SafeFrame::MYON_SafeFrame(MYON_XMLNode& model_node)
{
  m_impl = new CImpl(model_node);
}

MYON_SafeFrame::MYON_SafeFrame(const MYON_SafeFrame& sf)
{
  m_impl = new CImpl;
  operator = (sf);
}

MYON_SafeFrame::~MYON_SafeFrame()
{
  delete m_impl;
  m_impl = nullptr;
}

const MYON_SafeFrame& MYON_SafeFrame::operator = (const MYON_SafeFrame& sf)
{
  if (this != &sf)
  {
    SetOn               (sf.On());
    SetPerspectiveOnly  (sf.PerspectiveOnly());
    SetFieldGridOn      (sf.FieldGridOn());
    SetLiveFrameOn      (sf.LiveFrameOn());
    SetActionFrameOn    (sf.ActionFrameOn());
    SetActionFrameLinked(sf.ActionFrameLinked());
    SetTitleFrameOn     (sf.TitleFrameOn());
    SetTitleFrameLinked (sf.TitleFrameLinked());
    SetActionFrameXScale(sf.ActionFrameXScale());
    SetActionFrameYScale(sf.ActionFrameYScale());
    SetTitleFrameXScale (sf.TitleFrameXScale());
    SetTitleFrameYScale (sf.TitleFrameYScale());
  }

  return *this;
}

bool MYON_SafeFrame::operator == (const MYON_SafeFrame& sf)
{
  if (On()                != sf.On())                return false;
  if (PerspectiveOnly()   != sf.PerspectiveOnly())   return false;
  if (FieldGridOn()       != sf.FieldGridOn())       return false;
  if (LiveFrameOn()       != sf.LiveFrameOn())       return false;
  if (ActionFrameOn()     != sf.ActionFrameOn())     return false;
  if (ActionFrameLinked() != sf.ActionFrameLinked()) return false;
  if (TitleFrameOn()      != sf.TitleFrameOn())      return false;
  if (TitleFrameLinked()  != sf.TitleFrameLinked())  return false;

  if (!IsDoubleEqual(ActionFrameXScale(), sf.ActionFrameXScale())) return false;
  if (!IsDoubleEqual(ActionFrameYScale(), sf.ActionFrameYScale())) return false;
  if (!IsDoubleEqual(TitleFrameXScale() , sf.TitleFrameXScale())) return false;
  if (!IsDoubleEqual(TitleFrameYScale() , sf.TitleFrameYScale())) return false;

  return true;
}

bool MYON_SafeFrame::operator != (const MYON_SafeFrame& sf)
{
  return !(operator == (sf));
}

bool MYON_SafeFrame::On(void) const
{
  return m_impl->GetBaseParameter(MYON_RDK_SF_ON, false).AsBool();
}

void MYON_SafeFrame::SetOn(bool b)
{
  m_impl->SetBaseParameter(MYON_RDK_SF_ON, b);
}

bool MYON_SafeFrame::PerspectiveOnly(void) const
{
  return m_impl->GetBaseParameter(MYON_RDK_SF_PERSPECTIVE_ONLY, false).AsBool();
}

void MYON_SafeFrame::SetPerspectiveOnly(bool b)
{
  m_impl->SetBaseParameter(MYON_RDK_SF_PERSPECTIVE_ONLY, b);
}

bool MYON_SafeFrame::FieldGridOn(void) const
{
  return m_impl->GetBaseParameter(MYON_RDK_SF_FIELD_DISPLAY_ON, false).AsBool();
}

void MYON_SafeFrame::SetFieldGridOn(bool b)
{
  m_impl->SetBaseParameter(MYON_RDK_SF_FIELD_DISPLAY_ON, b);
}

bool MYON_SafeFrame::LiveFrameOn(void) const
{
  return m_impl->GetFrameParameter(MYON_RDK_SF_LIVE_FRAME, MYON_RDK_SFF_ON, false).AsBool();
}

void MYON_SafeFrame::SetLiveFrameOn(bool b)
{
  m_impl->SetFrameParameter(MYON_RDK_SF_LIVE_FRAME, MYON_RDK_SFF_ON, b);
}

bool MYON_SafeFrame::ActionFrameOn(void) const
{
  return m_impl->GetFrameParameter(MYON_RDK_SF_ACTION_FRAME, MYON_RDK_SFF_ON, false).AsBool();
}

void MYON_SafeFrame::SetActionFrameOn(bool b)
{
  m_impl->SetFrameParameter(MYON_RDK_SF_ACTION_FRAME, MYON_RDK_SFF_ON, b);
}

bool MYON_SafeFrame::ActionFrameLinked(void) const
{
  return m_impl->GetFrameParameter(MYON_RDK_SF_ACTION_FRAME, MYON_RDK_SFF_LINK, false).AsBool();
}

void MYON_SafeFrame::SetActionFrameLinked(bool b)
{
  m_impl->SetFrameParameter(MYON_RDK_SF_ACTION_FRAME, MYON_RDK_SFF_LINK, b);
}

double MYON_SafeFrame::ActionFrameXScale(void) const
{
  return m_impl->GetFrameParameter(MYON_RDK_SF_ACTION_FRAME, MYON_RDK_SFF_XSCALE, 0.9).AsDouble();
}

void MYON_SafeFrame::SetActionFrameXScale(double d)
{
  m_impl->SetFrameParameter(MYON_RDK_SF_ACTION_FRAME, MYON_RDK_SFF_XSCALE, d);
}

double MYON_SafeFrame::ActionFrameYScale(void) const
{
  return m_impl->GetFrameParameter(MYON_RDK_SF_ACTION_FRAME, MYON_RDK_SFF_YSCALE, 0.9).AsDouble();
}

void MYON_SafeFrame::SetActionFrameYScale(double d)
{
  m_impl->SetFrameParameter(MYON_RDK_SF_ACTION_FRAME, MYON_RDK_SFF_YSCALE, d);
}

bool MYON_SafeFrame::TitleFrameOn(void) const
{
  return m_impl->GetFrameParameter(MYON_RDK_SF_TITLE_FRAME, MYON_RDK_SFF_ON, false).AsBool();
}

void MYON_SafeFrame::SetTitleFrameOn(bool b)
{
  m_impl->SetFrameParameter(MYON_RDK_SF_TITLE_FRAME, MYON_RDK_SFF_ON, b);
}

bool MYON_SafeFrame::TitleFrameLinked(void) const
{
  return m_impl->GetFrameParameter(MYON_RDK_SF_TITLE_FRAME, MYON_RDK_SFF_LINK, false).AsBool();
}

void MYON_SafeFrame::SetTitleFrameLinked(bool b)
{
  m_impl->SetFrameParameter(MYON_RDK_SF_TITLE_FRAME, MYON_RDK_SFF_LINK, b);
}

double MYON_SafeFrame::TitleFrameXScale(void) const
{
  return m_impl->GetFrameParameter(MYON_RDK_SF_TITLE_FRAME, MYON_RDK_SFF_XSCALE, 0.8).AsDouble();
}

void MYON_SafeFrame::SetTitleFrameXScale(double d)
{
  m_impl->SetFrameParameter(MYON_RDK_SF_TITLE_FRAME, MYON_RDK_SFF_XSCALE, d);
}

double MYON_SafeFrame::TitleFrameYScale(void) const
{
  return m_impl->GetFrameParameter(MYON_RDK_SF_TITLE_FRAME, MYON_RDK_SFF_YSCALE, 0.8).AsDouble();
}

void MYON_SafeFrame::SetTitleFrameYScale(double d)
{
  m_impl->SetFrameParameter(MYON_RDK_SF_TITLE_FRAME, MYON_RDK_SFF_YSCALE, d);
}

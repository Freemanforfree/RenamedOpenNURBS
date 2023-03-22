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

#define MYON_RDK_PEP_PLUG_IN  L"plug-in"
#define MYON_RDK_PEP_NAME     L"name"
#define MYON_RDK_PEP_ID       L"id"
#define MYON_RDK_PEP_SHOWN    L"shown"
#define MYON_RDK_PEP_ON       L"on"
#define MYON_RDK_PEP_STATE    L"state"

// MYON_PostEffect
//
// TODO:
// NOTE TO JOHN: On writing this I see I have made a terrible assumption that the model will only
// be loaded and saved once! It will need to be changed so that the nodes are not stolen out of the
// document, but they are copied.

MYON_XMLNode* FindPostEffectNodeForId(const MYON_XMLNode& pep_section_node, const MYON_UUID& id)
{
  auto it = pep_section_node.GetChildIterator();

  auto* pep_node = it.GetNextChild();
  while (nullptr != pep_node)
  {
    const auto* prop = pep_node->GetNamedProperty(MYON_RDK_PEP_ID);
    if (nullptr != prop)
    {
      if (prop->GetValue().AsUuid() == id)
        return pep_node;
    }

    pep_node = it.GetNextChild();
  }

  return nullptr;
}

class MYON_PostEffect::CImpl
{
public:
  CImpl(const MYON_XMLNode& n, Types t) : m_node(n), m_type(t) { }

  MYON_XMLVariant GetPropertyValue(const wchar_t* name) const;

public:
  MYONX_Model* m_model = nullptr;
  MYON_XMLNode m_node;
  MYON_PostEffect::Types m_type = MYON_PostEffect::Types::Early;
};

MYON_XMLVariant MYON_PostEffect::CImpl::GetPropertyValue(const wchar_t* name) const
{
  MYON_XMLVariant v;

  const auto* pProp = m_node.GetNamedProperty(name);
  if (nullptr != pProp)
  {
    v = pProp->GetValue();
  }

  return v;
}

void SetModel(const MYON_PostEffect& pep, MYONX_Model& m)
{
  pep.m_impl->m_model = &m;
}

MYON_OBJECT_IMPLEMENT(MYON_PostEffect, MYON_ModelComponent, "A7755211-7C60-4C46-8705-1C91151C9CD9");

MYON_PostEffect::MYON_PostEffect()
  :
  MYON_ModelComponent(MYON_ModelComponent::Type::PostEffect)
{
  m_impl = new CImpl(nullptr, Types::Unset);
}

MYON_PostEffect::MYON_PostEffect(const MYON_XMLNode& node, Types type)
  :
  MYON_ModelComponent(MYON_ModelComponent::Type::PostEffect)
{
  m_impl = new CImpl(node, type);

  // Copy the name to the component name.
  SetName(m_impl->GetPropertyValue(MYON_RDK_PEP_NAME).AsString());

  // Copy the XML instance id to the component id. This is the unique id of the PEP and as such
  // it's not supposed to be changed. There is a lock flag in the MYON_ModelComponent but how to use
  // it is an impenetrable mystery to me. Anyway, if the user does set the id, it will be ignored
  // because we don't copy it back to the XML.
  const auto uuid = m_impl->GetPropertyValue(MYON_RDK_PEP_ID).AsUuid();
  SetId(uuid);
}

MYON_PostEffect::MYON_PostEffect(const MYON_PostEffect& pep)
  :
  MYON_ModelComponent(MYON_ModelComponent::Type::PostEffect, pep)
{
  m_impl = new CImpl(pep.m_impl->m_node, pep.Type());
  operator = (pep);
}

MYON_PostEffect::~MYON_PostEffect()
{
  delete m_impl;
  m_impl = nullptr;
}

MYON_PostEffect::Types MYON_PostEffect::Type(void) const
{
  return m_impl->m_type;
}

const MYON_PostEffect& MYON_PostEffect::operator = (const MYON_PostEffect& pep)
{
  MYON_ModelComponent::operator = (pep);

  m_impl->m_type = pep.m_impl->m_type;
  m_impl->m_node = pep.m_impl->m_node;

  return *this;
}

MYON_wString MYON_PostEffect::LocalName(void) const
{
  return m_impl->GetPropertyValue(MYON_RDK_PEP_NAME);
}

bool MYON_PostEffect::IsVisible(void) const
{
  return m_impl->GetPropertyValue(MYON_RDK_PEP_ON);
}

bool MYON_PostEffect::IsActive(void) const
{
  return m_impl->GetPropertyValue(MYON_RDK_PEP_SHOWN);
}

MYON_XMLVariant MYON_PostEffect::GetParameter(const wchar_t* param_name) const
{
  MYON_XMLVariant value;
  value.SetNull();

  auto* node = m_impl->m_node.GetNodeAtPath(MYON_RDK_PEP_STATE);
  if (nullptr != node)
  {
    MYON_XMLParameters p(*node);
    p.GetParam(param_name, value);
  }

  return value;
}

bool MYON_PostEffect::SetParameter(const wchar_t* param_name, const MYON_XMLVariant& param_value)
{
  auto* node = m_impl->m_node.GetNodeAtPath(MYON_RDK_PEP_STATE);
  if (nullptr == node)
    return false;

  MYON_XMLParameters p(*node);

  MYON_XMLVariant current_value;
  if (!p.GetParam(param_name, current_value))
    return false; // If you can't get it, you can't set it.

  if (!p.SetParam(param_name, param_value))
    return false;

  return true;
}

const MYON_XMLNode& MYON_PostEffect::XML(void) const
{
  return m_impl->m_node;
}

MYON__UINT32 MYON_PostEffect::DataCRC(MYON__UINT32 crc) const
{
  crc = MYON_ModelComponent::DataCRC(crc);

  return m_impl->m_node.DataCRC(crc);
}

const MYON_PostEffect* MYON_PostEffect::FromModelComponentRef(const MYON_ModelComponentReference& ref,
                                                          const MYON_PostEffect* none_return_value) // Static.
{
  const auto* pep = MYON_PostEffect::Cast(ref.ModelComponent());
  if (nullptr != pep)
    return pep;

  return none_return_value;
}

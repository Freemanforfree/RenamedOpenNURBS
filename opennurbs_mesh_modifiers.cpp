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

MYON_UUID MYON_MeshModifier::PlugInId(void) // Static.
{
  static const MYON_UUID uuid =
  {
    // {f293de5c-d1ff-467a-9bd1-cac8ec4b2e6b}
    0xf293de5c, 0xd1ff, 0x467a, {0x9b, 0xd1, 0xca, 0xc8, 0xec, 0x4b, 0x2e, 0x6b}
  };

  return uuid;
}

class MYON_MeshModifier::CImpl : public MYON_InternalXMLImpl
{
public:
  CImpl() { }
  CImpl(const MYON_XMLNode& n) { Node() = n; }
};

MYON_MeshModifier::MYON_MeshModifier()
{
  m_impl = new CImpl;
}

MYON_MeshModifier::MYON_MeshModifier(const MYON_XMLNode& node)
{
  m_impl = new CImpl(node);
}

MYON_MeshModifier::~MYON_MeshModifier()
{
  delete m_impl;
  m_impl = nullptr;
}

MYON_XMLNode* MYON_MeshModifier::AddChildXML(MYON_XMLRootNode& root) const
{
  MYON_XMLNode* mm_node = root.AttachChildNode(new MYON_XMLNode(L""));
  if (nullptr != mm_node)
  {
    *mm_node = m_impl->Node();
  }

  return mm_node;
}

////////////////////////////////////////////////////////////////
//
// Displacement
//
////////////////////////////////////////////////////////////////

MYON_OBJECT_IMPLEMENT(MYON_DisplacementUserData, MYON_UserData, "B8C04604-B4EF-43b7-8C26-1AFB8F1C54EB");

MYON_UUID MYON_DisplacementUserData::Uuid(void)
{
  static const MYON_UUID uuid = { 0x8224a7c4, 0x5590, 0x4ac4, { 0xa3, 0x2c, 0xde, 0x85, 0xdc, 0x2f, 0xfd, 0xae } };
  return uuid;
}

MYON_DisplacementUserData::MYON_DisplacementUserData()
{
  m_userdata_uuid = Uuid();

  m_application_uuid = MYON_MeshModifier::PlugInId();

  SetToDefaults();
}

MYON_DisplacementUserData::MYON_DisplacementUserData(const MYON_DisplacementUserData& ud)
  :
  MYON_XMLUserData(ud) // CRITICAL - Be sure to call base class.
{
  m_userdata_uuid = Uuid();

  m_application_uuid = MYON_MeshModifier::PlugInId();

  // DO NOT SET OTHER MYON_UserData fields
  // In particular, do not set m_userdata_copycount
  *this = ud;
}

const MYON_DisplacementUserData& MYON_DisplacementUserData::operator = (const MYON_DisplacementUserData& ud)
{
  if (this != &ud)
    MYON_XMLUserData::operator = (ud);

  return *this;
}

bool MYON_DisplacementUserData::GetDescription(MYON_wString& s)
{
  s = L"Displacement object data";
  return true;
}

void MYON_DisplacementUserData::SetToDefaults(void) const
{
  auto& ud = const_cast<MYON_DisplacementUserData&>(*this);
  ud.Clear();

  MYON_Displacement::Defaults d;

  MYON_XMLNode* root = ud.XMLRootForWrite().AttachChildNode(new MYON_XMLNode(MYON_DISPLACEMENT_ROOT));

  MYON_XMLParameters p(*root);
  p.SetParam(MYON_DISPLACEMENT_ON, false);
  p.SetParam(MYON_DISPLACEMENT_CHANNEL, d.ChannelNumber());
  p.SetParam(MYON_DISPLACEMENT_BLACK_POINT, d.BlackPoint());
  p.SetParam(MYON_DISPLACEMENT_WHITE_POINT, d.WhitePoint());
  p.SetParam(MYON_DISPLACEMENT_SWEEP_PITCH, d.SweepPitch());
  p.SetParam(MYON_DISPLACEMENT_REFINE_STEPS, d.RefineStepCount());
  p.SetParam(MYON_DISPLACEMENT_REFINE_SENSITIVITY, d.RefineSensitivity());
  p.SetParam(MYON_DISPLACEMENT_TEXTURE, MYON_nil_uuid);
  p.SetParam(MYON_DISPLACEMENT_FACE_COUNT_LIMIT_ENABLED, false);
  p.SetParam(MYON_DISPLACEMENT_FACE_COUNT_LIMIT, d.FaceLimit());
  p.SetParam(MYON_DISPLACEMENT_POST_WELD_ANGLE, d.PostWeldAngle());
  p.SetParam(MYON_DISPLACEMENT_MESH_MEMORY_LIMIT, d.MeshMemoryLimit());
  p.SetParam(MYON_DISPLACEMENT_FAIRING_ENABLED, false);
  p.SetParam(MYON_DISPLACEMENT_FAIRING_AMOUNT, d.FairingAmount());
  p.SetParam(MYON_DISPLACEMENT_SUB_OBJECT_COUNT, 0);
  p.SetParam(MYON_DISPLACEMENT_SWEEP_RES_FORMULA, int(d.SweepResolutionFormula()));

  MYON_XMLNode* sub_node = new MYON_XMLNode(MYON_DISPLACEMENT_SUB);
  root->AttachChildNode(sub_node);

  MYON_XMLParameters psub(*sub_node);
  psub.SetParam(MYON_DISPLACEMENT_SUB_INDEX, -1);
  psub.SetParam(MYON_DISPLACEMENT_SUB_ON, false);
  psub.SetParam(MYON_DISPLACEMENT_SUB_TEXTURE, MYON_nil_uuid);
  psub.SetParam(MYON_DISPLACEMENT_SUB_CHANNEL, d.ChannelNumber());
  psub.SetParam(MYON_DISPLACEMENT_SUB_BLACK_POINT, d.BlackPoint());
  psub.SetParam(MYON_DISPLACEMENT_SUB_WHITE_POINT, d.WhitePoint());
}

void MYON_DisplacementUserData::ReportVersionError(void) const
{
  MYON_ERROR("MYON_DisplacementUserData: version error");
}

bool MYON_DisplacementUserData::Transform(const MYON_Xform& xform)
{
  return MYON_XMLUserData::Transform(xform);
}

bool MYON_DisplacementUserData::Read(MYON_BinaryArchive& archive)
{
  if (!MYON_XMLUserData::Read(archive))
    return false;

  const int archive3dmVersion = archive.Archive3dmVersion();
  if (archive3dmVersion < 60)
  {
    MYON_XMLNode* root = XMLRootForWrite().GetNamedChild(MYON_DISPLACEMENT_ROOT);
    if (root != nullptr)
    {
      MYON_XMLParameters p(*root);
      MYON_XMLVariant v;
      if (!p.GetParam(MYON_DISPLACEMENT_SWEEP_RES_FORMULA, v))
      {
        p.SetParam(MYON_DISPLACEMENT_SWEEP_RES_FORMULA, 1);
      }
    }
  }

  return true;
}

class MYON_Displacement::CImplDSP
{
public:
  MYON_SimpleArray<SubItem*> m_subs;
};

MYON_Displacement::MYON_Displacement()
  :
  MYON_MeshModifier(MYON_XMLNode(MYON_DISPLACEMENT_ROOT))
{
  m_impl_dsp = new CImplDSP;
}

MYON_Displacement::MYON_Displacement(const MYON_XMLNode& dsp_node)
{
  m_impl_dsp = new CImplDSP;

  // Iterate over the displacement node looking at each child node's name. If the child
  // node is a sub-item node, create a sub-item object to hold the sub-item XML. Otherwise add
  // a copy of the child node to a new displacement node.
  MYON_XMLNode new_dsp_node(dsp_node.TagName());

  auto it = dsp_node.GetChildIterator();
  while (MYON_XMLNode* child_node = it.GetNextChild())
  {
    if (MYON_DISPLACEMENT_SUB == child_node->TagName())
    {
      m_impl_dsp->m_subs.Append(new SubItem(*child_node));
    }
    else
    {
      new_dsp_node.AttachChildNode(new MYON_XMLNode(*child_node));
    }
  }

  // Copy the new displacement node to our node. It only contains displacement XML with no sub-item nodes.
  m_impl->Node() = new_dsp_node;
}

MYON_Displacement::MYON_Displacement(const MYON_Displacement& dsp)
{
  m_impl_dsp = new CImplDSP;
  operator = (dsp);
}

MYON_Displacement::~MYON_Displacement()
{
  DeleteAllSubItems();

  delete m_impl_dsp;
  m_impl_dsp = nullptr;
}

const MYON_Displacement& MYON_Displacement::operator = (const MYON_Displacement& dsp)
{
  if (this != &dsp)
  {
    SetOn                    (dsp.On());
    SetTexture               (dsp.Texture());
    SetMappingChannel        (dsp.MappingChannel());
    SetBlackPoint            (dsp.BlackPoint());
    SetWhitePoint            (dsp.WhitePoint());
    SetInitialQuality        (dsp.InitialQuality());
    SetFinalMaxFacesOn       (dsp.FinalMaxFacesOn());
    SetFinalMaxFaces         (dsp.FinalMaxFaces());
    SetFairingOn             (dsp.FairingOn());
    SetFairing               (dsp.Fairing());
    SetPostWeldAngle         (dsp.PostWeldAngle());
    SetMeshMemoryLimit       (dsp.MeshMemoryLimit());
    SetRefineSteps           (dsp.RefineSteps());
    SetRefineSensitivity     (dsp.RefineSensitivity());
    SetSweepResolutionFormula(dsp.SweepResolutionFormula());
  }

  return *this;
}

bool MYON_Displacement::operator == (const MYON_Displacement& dsp) const
{
  if (On()                     != dsp.On())                     return false;
  if (Texture()                != dsp.Texture())                return false;
  if (MappingChannel()         != dsp.MappingChannel())         return false;
  if (BlackPoint()             != dsp.BlackPoint())             return false;
  if (WhitePoint()             != dsp.WhitePoint())             return false;
  if (InitialQuality()         != dsp.InitialQuality())         return false;
  if (FinalMaxFacesOn()        != dsp.FinalMaxFacesOn())        return false;
  if (FinalMaxFaces()          != dsp.FinalMaxFaces())          return false;
  if (FairingOn()              != dsp.FairingOn())              return false;
  if (Fairing()                != dsp.Fairing())                return false;
  if (PostWeldAngle()          != dsp.PostWeldAngle())          return false;
  if (MeshMemoryLimit()        != dsp.MeshMemoryLimit())        return false;
  if (RefineSteps()            != dsp.RefineSteps())            return false;
  if (RefineSensitivity()      != dsp.RefineSensitivity())      return false;
  if (SweepResolutionFormula() != dsp.SweepResolutionFormula()) return false;

  return true;
}

bool MYON_Displacement::operator != (const MYON_Displacement& dsp) const
{
  return !(operator == (dsp));
}

MYON_XMLNode* MYON_Displacement::AddChildXML(MYON_XMLRootNode& root) const
{
  MYON_XMLNode* dsp_node = MYON_MeshModifier::AddChildXML(root);
  if (nullptr != dsp_node)
  {
    auto it = GetSubItemIterator();
    SubItem* sub_item = nullptr;
    while (nullptr != (sub_item = it.Next()))
    {
      MYON_XMLNode* sub_node = dsp_node->AttachChildNode(new MYON_XMLNode(L""));
      sub_item->ToXML(*sub_node);
    }
  }

  return dsp_node;
}

bool MYON_Displacement::On(void) const
{
  return m_impl->GetParameter(MYON_DISPLACEMENT_ON, false).AsBool();
}

void MYON_Displacement::SetOn(bool b)
{
  m_impl->SetParameter(MYON_DISPLACEMENT_ON, b);
}

MYON_UUID MYON_Displacement::Texture(void) const
{
  return m_impl->GetParameter(MYON_DISPLACEMENT_TEXTURE, MYON_nil_uuid).AsUuid();
}

void MYON_Displacement::SetTexture(const MYON_UUID& id)
{
  m_impl->SetParameter(MYON_DISPLACEMENT_TEXTURE, id);
}

int MYON_Displacement::MappingChannel(void) const
{
  return m_impl->GetParameter(MYON_DISPLACEMENT_CHANNEL, 0).AsInteger();
}

void MYON_Displacement::SetMappingChannel(int c)
{
  m_impl->SetParameter(MYON_DISPLACEMENT_CHANNEL, c);
}

double MYON_Displacement::BlackPoint(void) const
{
  return m_impl->GetParameter(MYON_DISPLACEMENT_BLACK_POINT, 0.0).AsDouble();
}

void MYON_Displacement::SetBlackPoint(double b)
{
  m_impl->SetParameter(MYON_DISPLACEMENT_BLACK_POINT, b);
}

double MYON_Displacement::WhitePoint(void) const
{
  return m_impl->GetParameter(MYON_DISPLACEMENT_WHITE_POINT, 1.0).AsDouble();
}

void MYON_Displacement::SetWhitePoint(double w)
{
  m_impl->SetParameter(MYON_DISPLACEMENT_WHITE_POINT, w);
}

int MYON_Displacement::InitialQuality(void) const
{
  return m_impl->GetParameter(MYON_DISPLACEMENT_SWEEP_PITCH, 1000).AsInteger();
}

void MYON_Displacement::SetInitialQuality(int q)
{
  m_impl->SetParameter(MYON_DISPLACEMENT_SWEEP_PITCH, q);
}

bool MYON_Displacement::FinalMaxFacesOn(void) const
{
  return m_impl->GetParameter(MYON_DISPLACEMENT_FACE_COUNT_LIMIT_ENABLED, false).AsBool();
}

void MYON_Displacement::SetFinalMaxFacesOn(bool b)
{
  m_impl->SetParameter(MYON_DISPLACEMENT_FACE_COUNT_LIMIT_ENABLED, b);
}

int MYON_Displacement::FinalMaxFaces(void) const
{
  return m_impl->GetParameter(MYON_DISPLACEMENT_FACE_COUNT_LIMIT, 10000).AsInteger();
}

void MYON_Displacement::SetFinalMaxFaces(int f)
{
  m_impl->SetParameter(MYON_DISPLACEMENT_FACE_COUNT_LIMIT, f);
}

bool MYON_Displacement::FairingOn(void) const
{
  return m_impl->GetParameter(MYON_DISPLACEMENT_FAIRING_ENABLED, false).AsBool();
}

void MYON_Displacement::SetFairingOn(bool b)
{
  m_impl->SetParameter(MYON_DISPLACEMENT_FAIRING_ENABLED, b);
}

int MYON_Displacement::Fairing(void) const
{
  return m_impl->GetParameter(MYON_DISPLACEMENT_FAIRING_AMOUNT, 4).AsInteger();
}

void MYON_Displacement::SetFairing(int f)
{
  m_impl->SetParameter(MYON_DISPLACEMENT_FAIRING_AMOUNT, f);
}

double MYON_Displacement::PostWeldAngle(void) const
{
  return m_impl->GetParameter(MYON_DISPLACEMENT_POST_WELD_ANGLE, 40.0).AsDouble();
}

void MYON_Displacement::SetPostWeldAngle(double a)
{
  m_impl->SetParameter(MYON_DISPLACEMENT_POST_WELD_ANGLE, a);
}

int MYON_Displacement::MeshMemoryLimit(void) const
{
  return m_impl->GetParameter(MYON_DISPLACEMENT_MESH_MEMORY_LIMIT, 512).AsInteger();
}

void MYON_Displacement::SetMeshMemoryLimit(int l)
{
  m_impl->SetParameter(MYON_DISPLACEMENT_MESH_MEMORY_LIMIT, l);
}

int MYON_Displacement::RefineSteps(void) const
{
  return m_impl->GetParameter(MYON_DISPLACEMENT_REFINE_STEPS, 1).AsInteger();
}

void MYON_Displacement::SetRefineSteps(int s)
{
  m_impl->SetParameter(MYON_DISPLACEMENT_REFINE_STEPS, s);
}

double MYON_Displacement::RefineSensitivity(void) const
{
  return m_impl->GetParameter(MYON_DISPLACEMENT_REFINE_SENSITIVITY, 0.5).AsDouble();
}

void MYON_Displacement::SetRefineSensitivity(double s)
{
  m_impl->SetParameter(MYON_DISPLACEMENT_REFINE_SENSITIVITY, s);
}

MYON_Displacement::SweepResolutionFormulas MYON_Displacement::SweepResolutionFormula(void) const
{
  const auto def = SweepResolutionFormulas::Default;
  const int v = m_impl->GetParameter(MYON_DISPLACEMENT_SWEEP_RES_FORMULA, int(def)).AsInteger();
  return SweepResolutionFormulas(v);
}

void MYON_Displacement::SetSweepResolutionFormula(SweepResolutionFormulas f)
{
  m_impl->SetParameter(MYON_DISPLACEMENT_SWEEP_RES_FORMULA, int(f));
}

MYON_Displacement::SubItemIterator MYON_Displacement::GetSubItemIterator(void) const
{
  return SubItemIterator(*this);
}

MYON_Displacement::SubItem& MYON_Displacement::AddSubItem(void)
{
  MYON_XMLNode node(MYON_DISPLACEMENT_SUB);
  auto* sub = new SubItem(node);
  m_impl_dsp->m_subs.Append(sub);

  return *sub;
}

void MYON_Displacement::DeleteSubItem(int face_index)
{
  const int index = FindSubItemArrayIndex(face_index);
  if (index >= 0)
  {
    delete m_impl_dsp->m_subs[index];
    m_impl_dsp->m_subs.Remove(index);
  }
}

void MYON_Displacement::DeleteAllSubItems(void)
{
  for (int i = 0; i < m_impl_dsp->m_subs.Count(); i++)
  {
    delete m_impl_dsp->m_subs[i];
  }

  m_impl_dsp->m_subs.Destroy();
}

int MYON_Displacement::FindSubItemArrayIndex(int face_index) const
{
  for (int i = 0; i < m_impl_dsp->m_subs.Count(); i++)
  {
    const auto* sub = m_impl_dsp->m_subs[i];
    if (sub->FaceIndex() == face_index)
      return i;
  }

  return -1;
}

MYON_Displacement::SubItem* MYON_Displacement::FindSubItem(int face_index) const
{
  const int index = FindSubItemArrayIndex(face_index);
  if (index < 0)
    return nullptr;

  return m_impl_dsp->m_subs[index];
}

class MYON_Displacement::SubItem::CImpl : public MYON_InternalXMLImpl
{
public:
  CImpl(const MYON_XMLNode& sub_node) { Node() = sub_node; }
};

MYON_Displacement::SubItem::SubItem(const MYON_XMLNode& sub_node)
{
  m_impl = new CImpl(sub_node);
}

MYON_Displacement::SubItem::~SubItem()
{
  delete m_impl;
  m_impl = nullptr;
}

const MYON_Displacement::SubItem& MYON_Displacement::SubItem::operator = (const SubItem& sub)
{
  if (this != &sub)
  {
    SetOn             (sub.On());
    SetFaceIndex      (sub.FaceIndex());
    SetTexture        (sub.Texture());
    SetMappingChannel (sub.MappingChannel());
    SetBlackPoint     (sub.BlackPoint());
    SetWhitePoint     (sub.WhitePoint());
  }

  return *this;
}

bool MYON_Displacement::SubItem::operator == (const SubItem& sub) const
{
  if (On()             != sub.On())             return false;
  if (FaceIndex()      != sub.FaceIndex())      return false;
  if (Texture()        != sub.Texture())        return false;
  if (MappingChannel() != sub.MappingChannel()) return false;
  if (BlackPoint()     != sub.BlackPoint())     return false;
  if (WhitePoint()     != sub.WhitePoint())     return false;

  return true;
}

bool MYON_Displacement::SubItem::operator != (const SubItem& c) const
{
  return !(operator == (c));
}

int MYON_Displacement::SubItem::FaceIndex(void) const
{
  return m_impl->GetParameter(MYON_DISPLACEMENT_SUB_INDEX, -1).AsInteger();
}

void MYON_Displacement::SubItem::SetFaceIndex(int i)
{
  m_impl->SetParameter(MYON_DISPLACEMENT_SUB_INDEX, i);
}

bool MYON_Displacement::SubItem::On(void) const
{
  return m_impl->GetParameter(MYON_DISPLACEMENT_SUB_ON, false).AsBool();
}

void MYON_Displacement::SubItem::SetOn(bool b)
{
  m_impl->SetParameter(MYON_DISPLACEMENT_SUB_ON, b);
}

MYON_UUID MYON_Displacement::SubItem::Texture(void) const
{
  return m_impl->GetParameter(MYON_DISPLACEMENT_SUB_TEXTURE, false).AsUuid();
}

void MYON_Displacement::SubItem::SetTexture(const MYON_UUID& id)
{
  m_impl->SetParameter(MYON_DISPLACEMENT_SUB_TEXTURE, id);
}

int MYON_Displacement::SubItem::MappingChannel(void) const
{
  return m_impl->GetParameter(MYON_DISPLACEMENT_SUB_CHANNEL, 0).AsInteger();
}

void MYON_Displacement::SubItem::SetMappingChannel(int c)
{
  m_impl->SetParameter(MYON_DISPLACEMENT_SUB_CHANNEL, c);
}

double MYON_Displacement::SubItem::BlackPoint(void) const
{
  return m_impl->GetParameter(MYON_DISPLACEMENT_SUB_BLACK_POINT, 0.0).AsDouble();
}

void MYON_Displacement::SubItem::SetBlackPoint(double b)
{
  m_impl->SetParameter(MYON_DISPLACEMENT_SUB_BLACK_POINT, b);
}

double MYON_Displacement::SubItem::WhitePoint(void) const
{
  return m_impl->GetParameter(MYON_DISPLACEMENT_SUB_WHITE_POINT, 1.0).AsDouble();
}

void MYON_Displacement::SubItem::SetWhitePoint(double w)
{
  m_impl->SetParameter(MYON_DISPLACEMENT_SUB_WHITE_POINT, w);
}

void MYON_Displacement::SubItem::ToXML(MYON_XMLNode& node) const
{
  node = m_impl->Node();
}

class MYON_Displacement::SubItemIterator::CImpl
{
public:
  CImpl(const MYON_Displacement& dsp) : m_dsp(dsp) { }

  SubItem* Next(void)
  {
    if (m_index >= m_dsp.m_impl_dsp->m_subs.Count())
      return nullptr;

    return m_dsp.m_impl_dsp->m_subs[m_index++];
  }

  const MYON_Displacement& m_dsp;
  int m_index = 0;
};

MYON_Displacement::SubItemIterator::SubItemIterator(const MYON_Displacement& dsp)
{
  m_impl = new CImpl(dsp);
}

MYON_Displacement::SubItemIterator::~SubItemIterator()
{
  delete m_impl;
  m_impl = nullptr;
}

MYON_Displacement::SubItem* MYON_Displacement::SubItemIterator::Next(void)
{
  return m_impl->Next();
}

MYON_UUID MYON_Displacement::Uuid(void) const
{
  // The unique id of the mesh modifier is the same as the id of its user data.
  return MYON_DisplacementUserData::Uuid();
}

int    MYON_Displacement::Defaults::RefineStepCount(void)   { return 1; }
int    MYON_Displacement::Defaults::FairingAmount(void)     { return 4; }
int    MYON_Displacement::Defaults::FaceLimit(void)         { return 10000; }
int    MYON_Displacement::Defaults::ChannelNumber(void)     { return 1; }
int    MYON_Displacement::Defaults::MeshMemoryLimit(void)   { return 512; }
double MYON_Displacement::Defaults::BlackPoint(void)        { return 0.0; }
double MYON_Displacement::Defaults::WhitePoint(void)        { return 1.0; }
double MYON_Displacement::Defaults::SweepPitch(void)        { return 1000.0; }
double MYON_Displacement::Defaults::RefineSensitivity(void) { return 0.5; }
double MYON_Displacement::Defaults::PostWeldAngle(void)     { return 40.0; }
double MYON_Displacement::Defaults::AbsoluteTolerance(void) { return 0.001; }

MYON_Displacement::SweepResolutionFormulas MYON_Displacement::Defaults::SweepResolutionFormula(void)
{
  return SweepResolutionFormulas::Default;
}

////////////////////////////////////////////////////////////////
//
// Edge Softening
//
////////////////////////////////////////////////////////////////

MYON_OBJECT_IMPLEMENT(MYON_EdgeSofteningUserData, MYON_UserData, "CB5EB395-BF1B-4112-8F2F-F728FCE8169C");

MYON_UUID MYON_EdgeSofteningUserData::Uuid(void)
{
  static const MYON_UUID uuid = { 0x8cbe6160, 0x5cbd, 0x4b4d, { 0x8c, 0xd2, 0x7c, 0xe0, 0xa7, 0xc8, 0xc2, 0xd8 } };
  return uuid;
}

MYON_EdgeSofteningUserData::MYON_EdgeSofteningUserData()
{
  m_userdata_uuid = Uuid();

  m_application_uuid = MYON_MeshModifier::PlugInId();

  SetToDefaults();
}

MYON_EdgeSofteningUserData::MYON_EdgeSofteningUserData(const MYON_EdgeSofteningUserData& ud)
  :
  MYON_XMLUserData(ud) // CRITICAL - Be sure to call base class.
{
  m_userdata_uuid = Uuid();

  m_application_uuid = MYON_MeshModifier::PlugInId();

  // DO NOT SET OTHER MYON_UserData fields
  // In particular, do not set m_userdata_copycount
  *this = ud;
}

bool MYON_EdgeSofteningUserData::GetDescription(MYON_wString& s)
{
  s = L"EdgeSoftening object data";
  return true;
}

const MYON_EdgeSofteningUserData& MYON_EdgeSofteningUserData::operator = (const MYON_EdgeSofteningUserData& ud)
{
  if (this != &ud)
    MYON_XMLUserData::operator = (ud);

  return *this;
}

void MYON_EdgeSofteningUserData::SetToDefaults(void) const
{
  auto& ud = const_cast<MYON_EdgeSofteningUserData&>(*this);
  ud.Clear();

  MYON_XMLProperty prop;
  MYON_XMLNode* root = ud.XMLRootForWrite().AttachChildNode(new MYON_XMLNode(MYON_EDGE_SOFTENING_ROOT));

  MYON_EdgeSoftening::Defaults d;

  MYON_XMLParameters p(*root);
  p.SetParam(MYON_EDGE_SOFTENING_ON, false);
  p.SetParam(MYON_EDGE_SOFTENING_SOFTENING, d.Softening());
  p.SetParam(MYON_EDGE_SOFTENING_CHAMFER, d.Chamfer());
  p.SetParam(MYON_EDGE_SOFTENING_UNWELD, d.Faceted());
  p.SetParam(MYON_EDGE_SOFTENING_FORCE_SOFTENING, d.ForceSoftening());
  p.SetParam(MYON_EDGE_SOFTENING_EDGE_THRESHOLD, d.EdgeAngleThreshold());
}

void MYON_EdgeSofteningUserData::ReportVersionError(void) const
{
  MYON_ERROR("MYON_EdgeSofteningUserData: version error");
}

MYON_EdgeSoftening::MYON_EdgeSoftening()
  :
  MYON_MeshModifier(MYON_XMLNode(MYON_EDGE_SOFTENING_ROOT))
{
}

MYON_EdgeSoftening::MYON_EdgeSoftening(const MYON_XMLNode& node)
  :
  MYON_MeshModifier(node)
{
}

MYON_EdgeSoftening::MYON_EdgeSoftening(const MYON_EdgeSoftening& es)
{
  operator = (es);
}

const MYON_EdgeSoftening& MYON_EdgeSoftening::operator = (const MYON_EdgeSoftening& es)
{
  if (this != &es)
  {
    SetOn                (es.On());
    SetSoftening         (es.Softening());
    SetChamfer           (es.Chamfer());
    SetFaceted           (es.Faceted());
    SetForceSoftening    (es.ForceSoftening());
    SetEdgeAngleThreshold(es.EdgeAngleThreshold());
  }

  return *this;
}

bool MYON_EdgeSoftening::operator == (const MYON_EdgeSoftening& es) const
{
  if (On()                 != es.On())                 return false;
  if (Softening()          != es.Softening())          return false;
  if (Chamfer()            != es.Chamfer())            return false;
  if (Faceted()            != es.Faceted())            return false;
  if (ForceSoftening()     != es.ForceSoftening())     return false;
  if (EdgeAngleThreshold() != es.EdgeAngleThreshold()) return false;

  return true;
}

bool MYON_EdgeSoftening::operator != (const MYON_EdgeSoftening& es) const
{
  return !(operator == (es));
}

bool MYON_EdgeSoftening::On(void) const
{
  return m_impl->GetParameter(MYON_EDGE_SOFTENING_ON, false).AsBool();
}

void MYON_EdgeSoftening::SetOn(bool b)
{
  m_impl->SetParameter(MYON_EDGE_SOFTENING_ON, b);
}

double MYON_EdgeSoftening::Softening(void) const
{
  return m_impl->GetParameter(MYON_EDGE_SOFTENING_SOFTENING, 0.1).AsDouble();
}

void MYON_EdgeSoftening::SetSoftening(double d)
{
  m_impl->SetParameter(MYON_EDGE_SOFTENING_SOFTENING, d);
}

bool MYON_EdgeSoftening::Chamfer(void) const
{
  return m_impl->GetParameter(MYON_EDGE_SOFTENING_CHAMFER, false).AsBool();
}

void MYON_EdgeSoftening::SetChamfer(bool b)
{
  m_impl->SetParameter(MYON_EDGE_SOFTENING_CHAMFER, b);
}

bool MYON_EdgeSoftening::Faceted(void) const
{
  return m_impl->GetParameter(MYON_EDGE_SOFTENING_UNWELD, false).AsBool();
}

void MYON_EdgeSoftening::SetFaceted(bool b)
{
  m_impl->SetParameter(MYON_EDGE_SOFTENING_UNWELD, b);
}

double MYON_EdgeSoftening::EdgeAngleThreshold(void) const
{
  return m_impl->GetParameter(MYON_EDGE_SOFTENING_EDGE_THRESHOLD, 5.0).AsDouble();
}

void MYON_EdgeSoftening::SetEdgeAngleThreshold(double d)
{
  m_impl->SetParameter(MYON_EDGE_SOFTENING_EDGE_THRESHOLD, d);
}

bool MYON_EdgeSoftening::ForceSoftening(void) const
{
  return m_impl->GetParameter(MYON_EDGE_SOFTENING_FORCE_SOFTENING, false).AsBool();
}

void MYON_EdgeSoftening::SetForceSoftening(bool b)
{
  m_impl->SetParameter(MYON_EDGE_SOFTENING_FORCE_SOFTENING, b);
}

MYON_UUID MYON_EdgeSoftening::Uuid(void) const
{
  // The unique id of the mesh modifier is the same as the id of its user data.
  return MYON_EdgeSofteningUserData::Uuid();
}

bool   MYON_EdgeSoftening::Defaults::Chamfer(void)            { return false; }
bool   MYON_EdgeSoftening::Defaults::Faceted(void)            { return false; }
bool   MYON_EdgeSoftening::Defaults::ForceSoftening(void)     { return false; }
double MYON_EdgeSoftening::Defaults::Softening(void)          { return 0.1; }
double MYON_EdgeSoftening::Defaults::EdgeAngleThreshold(void) { return 5.0; }

////////////////////////////////////////////////////////////////
//
// Thickening
//
////////////////////////////////////////////////////////////////

MYON_OBJECT_IMPLEMENT(MYON_ThickeningUserData, MYON_UserData, "AA03D9C3-4CCF-4431-A06E-25F38CF3913F");

MYON_UUID MYON_ThickeningUserData::Uuid(void)
{
  static const MYON_UUID uuid = { 0x6aa7ccc3, 0x2721, 0x410f, { 0xaa, 0x56, 0xe8, 0xab, 0x4f, 0x3e, 0xce, 0x67 } };
  return uuid;
}

MYON_ThickeningUserData::MYON_ThickeningUserData()
{
  m_userdata_uuid = Uuid();

  m_application_uuid = MYON_MeshModifier::PlugInId();

  SetToDefaults();
}

MYON_ThickeningUserData::MYON_ThickeningUserData(const MYON_ThickeningUserData& ud)
  :
  MYON_XMLUserData(ud) // CRITICAL - Be sure to call base class.
{
  m_userdata_uuid = Uuid();

  m_application_uuid = MYON_MeshModifier::PlugInId();

  // DO NOT SET OTHER MYON_UserData fields
  // In particular, do not set m_userdata_copycount
  *this = ud;
}

const MYON_ThickeningUserData& MYON_ThickeningUserData::operator = (const MYON_ThickeningUserData& ud)
{
  if (this != &ud)
    MYON_XMLUserData::operator = (ud);

  return *this;
}

void MYON_ThickeningUserData::SetToDefaults(void) const
{
  auto& ud = const_cast<MYON_ThickeningUserData&>(*this);
  ud.Clear();

  MYON_XMLProperty prop;
  MYON_XMLNode* root = ud.XMLRootForWrite().AttachChildNode(new MYON_XMLNode(MYON_THICKENING_ROOT));

  MYON_Thickening::Defaults d;

  MYON_XMLParameters p(*root);
  p.SetParam(MYON_THICKENING_ON, false);
  p.SetParam(MYON_THICKENING_SOLID, d.Solid());
  p.SetParam(MYON_THICKENING_BOTH_SIDES, d.BothSides());
  p.SetParam(MYON_THICKENING_OFFSET_ONLY, d.OffsetOnly());
  p.SetParam(MYON_THICKENING_DISTANCE, d.Distance());
}

bool MYON_ThickeningUserData::GetDescription(MYON_wString& s)
{
  s = L"Thickness object data";
  return true;
}

void MYON_ThickeningUserData::ReportVersionError(void) const
{
  MYON_ERROR("MYON_ThickeningUserData: version error");
}

bool MYON_ThickeningUserData::Transform(const MYON_Xform& xform)
{
  return MYON_XMLUserData::Transform(xform);
}

MYON_Thickening::MYON_Thickening()
  :
  MYON_MeshModifier(MYON_XMLNode(MYON_THICKENING_ROOT))
{
}

MYON_Thickening::MYON_Thickening(const MYON_XMLNode& node)
  :
  MYON_MeshModifier(node)
{
}

MYON_Thickening::MYON_Thickening(const MYON_Thickening& t)
{
  operator = (t);
}

const MYON_Thickening& MYON_Thickening::operator = (const MYON_Thickening& t)
{
  if (this != &t)
  {
    SetOn        (t.On());
    SetDistance  (t.Distance());
    SetSolid     (t.Solid());
    SetOffsetOnly(t.OffsetOnly());
    SetBothSides (t.BothSides());
  }

  return *this;
}

bool MYON_Thickening::operator == (const MYON_Thickening& t) const
{
  if (On()         != t.On())         return false;
  if (Distance()   != t.Distance())   return false;
  if (Solid()      != t.Solid())      return false;
  if (OffsetOnly() != t.OffsetOnly()) return false;
  if (BothSides()  != t.BothSides())  return false;

  return true;
}

bool MYON_Thickening::operator != (const MYON_Thickening& t) const
{
  return !(operator == (t));
}

bool MYON_Thickening::On(void) const
{
  return m_impl->GetParameter(MYON_THICKENING_ON, false).AsBool();
}

void MYON_Thickening::SetOn(bool b)
{
  m_impl->SetParameter(MYON_THICKENING_ON, b);
}

double MYON_Thickening::Distance(void) const
{
  return m_impl->GetParameter(MYON_THICKENING_DISTANCE, 0.1).AsDouble();
}

void MYON_Thickening::SetDistance(double d)
{
  m_impl->SetParameter(MYON_THICKENING_DISTANCE, d);
}

bool MYON_Thickening::Solid(void) const
{
  return m_impl->GetParameter(MYON_THICKENING_SOLID, true).AsBool();
}

void MYON_Thickening::SetSolid(bool b)
{
  m_impl->SetParameter(MYON_THICKENING_SOLID, b);
}

bool MYON_Thickening::OffsetOnly(void) const
{
  return m_impl->GetParameter(MYON_THICKENING_OFFSET_ONLY, false).AsBool();
}

void MYON_Thickening::SetOffsetOnly(bool b)
{
  m_impl->SetParameter(MYON_THICKENING_OFFSET_ONLY, b);
}

bool MYON_Thickening::BothSides(void) const
{
  return m_impl->GetParameter(MYON_THICKENING_BOTH_SIDES, false).AsBool();
}

void MYON_Thickening::SetBothSides(bool b)
{
  m_impl->SetParameter(MYON_THICKENING_BOTH_SIDES, b);
}

MYON_UUID MYON_Thickening::Uuid(void) const
{
  // The unique id of the mesh modifier is the same as the id of its user data.
  return MYON_ThickeningUserData::Uuid();
}

bool   MYON_Thickening::Defaults::Solid(void)      { return true; }
bool   MYON_Thickening::Defaults::BothSides(void)  { return false; }
bool   MYON_Thickening::Defaults::OffsetOnly(void) { return false; }
double MYON_Thickening::Defaults::Distance(void)   { return 0.1; }

////////////////////////////////////////////////////////////////
//
// CurvePiping
//
////////////////////////////////////////////////////////////////

MYON_OBJECT_IMPLEMENT(MYON_CurvePipingUserData, MYON_UserData, "2D5AFEA9-F458-4079-992F-C2D405D9383B");

MYON_UUID MYON_CurvePipingUserData::Uuid(void)
{
  static const MYON_UUID uuid = { 0x2b1a758e, 0x7cb1, 0x45ab, { 0xa5, 0xbf, 0xdf, 0xcd, 0x6d, 0x3d, 0x13, 0x6d } };
  return uuid;
}

MYON_CurvePipingUserData::MYON_CurvePipingUserData()
{
  m_userdata_uuid = Uuid();

  m_application_uuid = MYON_MeshModifier::PlugInId();

  SetToDefaults();
}

MYON_CurvePipingUserData::MYON_CurvePipingUserData(const MYON_CurvePipingUserData& ud)
  :
  MYON_XMLUserData(ud) // CRITICAL - Be sure to call base class.
{
  m_userdata_uuid = Uuid();

  m_application_uuid = MYON_MeshModifier::PlugInId();

  // DO NOT SET OTHER MYON_UserData fields
  // In particular, do not set m_userdata_copycount
  *this = ud;
}

const MYON_CurvePipingUserData& MYON_CurvePipingUserData::operator = (const MYON_CurvePipingUserData& ud)
{
  if (this != &ud)
    MYON_XMLUserData::operator = (ud);

  return *this;
}

void MYON_CurvePipingUserData::SetToDefaults(void) const
{
  auto& ud = const_cast<MYON_CurvePipingUserData&>(*this);
  ud.Clear();

  MYON_XMLProperty prop;
  MYON_XMLNode* root = ud.XMLRootForWrite().AttachChildNode(new MYON_XMLNode(MYON_CURVE_PIPING_ROOT));

  MYON_CurvePiping::Defaults d;

  MYON_XMLParameters p(*root);
  p.SetParam(MYON_CURVE_PIPING_ON, false);
  p.SetParam(MYON_CURVE_PIPING_RADIUS, d.Radius());
  p.SetParam(MYON_CURVE_PIPING_SEGMENTS, d.Segments());
  p.SetParam(MYON_CURVE_PIPING_ACCURACY, d.Accuracy());
  p.SetParam(MYON_CURVE_PIPING_WELD, !d.Faceted()); // 'Weld' is the inverse of 'Faceted'.
  p.SetParam(MYON_CURVE_PIPING_CAP_TYPE, L"dome");
}

bool MYON_CurvePipingUserData::GetDescription(MYON_wString& s)
{
  s = L"CurvePiping object data";
  return true;
}

void MYON_CurvePipingUserData::ReportVersionError(void) const
{
  MYON_ERROR("MYON_CurvePipingUserData: version error");
}

bool MYON_CurvePipingUserData::Transform(const MYON_Xform& xform)
{
  return MYON_XMLUserData::Transform(xform);
}

MYON_CurvePiping::MYON_CurvePiping()
  :
  MYON_MeshModifier(MYON_XMLNode(MYON_CURVE_PIPING_ROOT))
{
}

MYON_CurvePiping::MYON_CurvePiping(const MYON_XMLNode& node)
  :
  MYON_MeshModifier(node)
{
}

MYON_CurvePiping::MYON_CurvePiping(const MYON_CurvePiping& cp)
{
  operator = (cp);
}

const MYON_CurvePiping& MYON_CurvePiping::operator = (const MYON_CurvePiping& cp)
{
  if (this != &cp)
  {
    SetRadius  (cp.Radius());
    SetSegments(cp.Segments());
    SetFaceted (cp.Faceted());
    SetAccuracy(cp.Accuracy());
    SetCapType (cp.CapType());
  }

  return *this;
}

bool MYON_CurvePiping::operator == (const MYON_CurvePiping& cp) const
{
  if (Radius()   != cp.Radius())   return false;
  if (Segments() != cp.Segments()) return false;
  if (Faceted()  != cp.Faceted())  return false;
  if (Accuracy() != cp.Accuracy()) return false;
  if (CapType()  != cp.CapType())  return false;

  return true;
}

bool MYON_CurvePiping::operator != (const MYON_CurvePiping& cp) const
{
  return !(operator == (cp));
}

bool MYON_CurvePiping::On(void) const
{
  return m_impl->GetParameter(MYON_CURVE_PIPING_ON, false).AsBool();
}

void MYON_CurvePiping::SetOn(bool b)
{
  m_impl->SetParameter(MYON_CURVE_PIPING_ON, b);
}

double MYON_CurvePiping::Radius(void) const
{
  return m_impl->GetParameter(MYON_CURVE_PIPING_RADIUS, 1.0).AsDouble();
}

void MYON_CurvePiping::SetRadius(double d)
{
  m_impl->SetParameter(MYON_CURVE_PIPING_RADIUS, d);
}

int MYON_CurvePiping::Segments(void) const
{
  return m_impl->GetParameter(MYON_CURVE_PIPING_SEGMENTS, 16).AsInteger();
}

void MYON_CurvePiping::SetSegments(int s)
{
  m_impl->SetParameter(MYON_CURVE_PIPING_SEGMENTS, s);
}

bool MYON_CurvePiping::Faceted(void) const
{
  // 'Weld' is the inverse of 'Faceted'.
  return !m_impl->GetParameter(MYON_CURVE_PIPING_WELD, true).AsBool();
}

void MYON_CurvePiping::SetFaceted(bool b)
{
  // 'Weld' is the inverse of 'Faceted'.
  m_impl->SetParameter(MYON_CURVE_PIPING_WELD, !b);
}

int MYON_CurvePiping::Accuracy(void) const
{
  return m_impl->GetParameter(MYON_CURVE_PIPING_ACCURACY, 50).AsInteger();
}

void MYON_CurvePiping::SetAccuracy(int a)
{
  m_impl->SetParameter(MYON_CURVE_PIPING_ACCURACY, std::min(100, std::max(0, a)));
}

static const wchar_t* CapTypeToString(MYON_CurvePiping::CapTypes ct)
{
  switch (ct)
  {
    case MYON_CurvePiping::CapTypes::Flat:
      return MYON_CURVE_PIPING_FLAT;
    case MYON_CurvePiping::CapTypes::Box:
      return MYON_CURVE_PIPING_BOX;
    case MYON_CurvePiping::CapTypes::Dome:
      return MYON_CURVE_PIPING_DOME;
    case MYON_CurvePiping::CapTypes::None:
      return MYON_CURVE_PIPING_NONE;
  }

  return MYON_CURVE_PIPING_NONE;
}

MYON_CurvePiping::CapTypes MYON_CurvePiping::CapType(void) const
{
  const MYON_wString s = m_impl->GetParameter(MYON_CURVE_PIPING_CAP_TYPE, L"").AsString();

  if (s == MYON_CURVE_PIPING_FLAT) return MYON_CurvePiping::CapTypes::Flat;
  if (s == MYON_CURVE_PIPING_BOX)  return MYON_CurvePiping::CapTypes::Box;
  if (s == MYON_CURVE_PIPING_DOME) return MYON_CurvePiping::CapTypes::Dome;

  return MYON_CurvePiping::CapTypes::None;
}

void MYON_CurvePiping::SetCapType(CapTypes ct)
{
  const wchar_t* s = CapTypeToString(ct);
  m_impl->SetParameter(MYON_CURVE_PIPING_CAP_TYPE, s);
}

MYON_UUID MYON_CurvePiping::Uuid(void) const
{
  // The unique id of the mesh modifier is the same as the id of its user data.
  return MYON_CurvePipingUserData::Uuid();
}

bool   MYON_CurvePiping::Defaults::Faceted(void)  { return false; }
int    MYON_CurvePiping::Defaults::Segments(void) { return 16; }
int    MYON_CurvePiping::Defaults::Accuracy(void) { return 50; }
double MYON_CurvePiping::Defaults::Radius(void)   { return 1.0; }

MYON_CurvePiping::CapTypes MYON_CurvePiping::Defaults::CapType(void)
{
  return CapTypes::Dome;
}

////////////////////////////////////////////////////////////////
//
// Shut-lining
//
////////////////////////////////////////////////////////////////

MYON_OBJECT_IMPLEMENT(MYON_ShutLiningUserData, MYON_UserData, "429DCD06-5643-4254-BDE8-C0557F8FD083");

MYON_UUID MYON_ShutLiningUserData::Uuid(void)
{
  static const MYON_UUID uuid = { 0x7506ebe, 0x1d69, 0x4345, { 0x9f, 0xd, 0x2b, 0x9a, 0xa1, 0x90, 0x6e, 0xef } };
  return uuid;
}

MYON_ShutLiningUserData::MYON_ShutLiningUserData()
{
  m_userdata_uuid = Uuid();

  m_application_uuid = MYON_MeshModifier::PlugInId();

  SetToDefaults();
}

MYON_ShutLiningUserData::MYON_ShutLiningUserData(const MYON_ShutLiningUserData& ud)
  :
  MYON_XMLUserData(ud) // CRITICAL - Be sure to call base class.
{
  m_userdata_uuid = Uuid();

  m_application_uuid = MYON_MeshModifier::PlugInId();

  // DO NOT SET OTHER MYON_UserData fields
  // In particular, do not set m_userdata_copycount
  *this = ud;
}

const MYON_ShutLiningUserData& MYON_ShutLiningUserData::operator = (const MYON_ShutLiningUserData& ud)
{
  if (this != &ud)
    MYON_XMLUserData::operator = (ud);

  return *this;
}

void MYON_ShutLiningUserData::SetToDefaults(void) const
{
  auto& ud = const_cast<MYON_ShutLiningUserData&>(*this);
  ud.Clear();

  MYON_XMLProperty prop;
  MYON_XMLNode* root = ud.XMLRootForWrite().AttachChildNode(new MYON_XMLNode(MYON_SHUTLINING_ROOT));

  MYON_XMLParameters p(*root);
  p.SetParam(MYON_SHUTLINING_ON, false);
  p.SetParam(MYON_SHUTLINING_FACETED, MYON_ShutLining::Defaults::Faceted());
  p.SetParam(MYON_SHUTLINING_AUTO_UPDATE, MYON_ShutLining::Defaults::AutoUpdate());
  p.SetParam(MYON_SHUTLINING_FORCE_UPDATE, MYON_ShutLining::Defaults::ForceUpdate());
}

bool MYON_ShutLiningUserData::GetDescription(MYON_wString& s)
{
  s = L"ShutLining object data";
  return true;
}

void MYON_ShutLiningUserData::ReportVersionError(void) const
{
  MYON_ERROR("MYON_ShutLiningUserData: version error");
}

bool MYON_ShutLiningUserData::Transform(const MYON_Xform& xform)
{
  return MYON_XMLUserData::Transform(xform);
}

class MYON_ShutLining::CImplSL
{
public:
  MYON_SimpleArray<Curve*> m_curves;
};

MYON_ShutLining::MYON_ShutLining()
  :
  MYON_MeshModifier(MYON_XMLNode(MYON_SHUTLINING_ROOT))
{
  m_impl_sl = new CImplSL;
}

MYON_ShutLining::MYON_ShutLining(const MYON_XMLNode& sl_node)
{
  m_impl_sl = new CImplSL;

  // Iterate over the shut-lining node looking at each child node's name. If the child
  // node is a curve node, create a curve object to hold the curve XML. Otherwise add
  // a copy of the child node to a new shut-lining node.
  MYON_XMLNode new_sl_node(sl_node.TagName());

  auto it = sl_node.GetChildIterator();
  while (MYON_XMLNode* child_node = it.GetNextChild())
  {
    if (MYON_SHUTLINING_CURVE == child_node->TagName())
    {
      m_impl_sl->m_curves.Append(new Curve(*child_node));
    }
    else
    {
      new_sl_node.AttachChildNode(new MYON_XMLNode(*child_node));
    }
  }

  // Copy the new shut-lining node to our node. It only contains shut-lining XML with no curve nodes.
  m_impl->Node() = new_sl_node;
}

MYON_ShutLining::MYON_ShutLining(const MYON_ShutLining& sl)
{
  m_impl_sl = new CImplSL;
  operator = (sl);
}

MYON_ShutLining::~MYON_ShutLining()
{
  DeleteAllCurves();

  delete m_impl_sl;
  m_impl_sl = nullptr;
}

const MYON_ShutLining& MYON_ShutLining::operator = (const MYON_ShutLining& sl)
{
  if (this != &sl)
  {
    // Copy basic parameters.
    SetOn(sl.On());
    SetFaceted(sl.Faceted());
    SetAutoUpdate(sl.AutoUpdate());
    SetForceUpdate(sl.ForceUpdate());

    // Copy curves.
    DeleteAllCurves();
    auto it = sl.GetCurveIterator();
    while (Curve* sl_curve = it.Next())
    {
      Curve& new_curve = AddCurve();
      new_curve = *sl_curve;
    }
  }

  return *this;
}

MYON_ShutLining::Curve* MYON_ShutLining::FindCurve(const MYON_UUID& id) const
{
  auto it = GetCurveIterator();
  while (Curve* curve = it.Next())
  {
    if (curve->Id() == id)
      return curve;
  }

  return nullptr;
}

bool MYON_ShutLining::operator == (const MYON_ShutLining& sl) const
{
  if (On()          != sl.On())          return false;
  if (Faceted()     != sl.Faceted())     return false;
  if (AutoUpdate()  != sl.AutoUpdate())  return false;
  if (ForceUpdate() != sl.ForceUpdate()) return false;

  auto it1 = GetCurveIterator();
  while (Curve* curve = it1.Next())
  {
    const Curve* sl_curve = sl.FindCurve(curve->Id());
    if (nullptr == sl_curve)
      return false;
  }

  auto it2 = sl.GetCurveIterator();
  while (Curve* sl_curve = it2.Next())
  {
    const Curve* curve = FindCurve(sl_curve->Id());
    if ((nullptr == curve) || (*curve != *sl_curve))
      return false;
  }

  return true;
}

bool MYON_ShutLining::operator != (const MYON_ShutLining& sl) const
{
  return !(operator == (sl));
}

bool MYON_ShutLining::On(void) const
{
  return m_impl->GetParameter(MYON_SHUTLINING_ON, false).AsBool();
}

void MYON_ShutLining::SetOn(bool b)
{
  m_impl->SetParameter(MYON_SHUTLINING_ON, b);
}

bool MYON_ShutLining::Faceted(void) const
{
  return m_impl->GetParameter(MYON_SHUTLINING_FACETED, false).AsBool();
}

void MYON_ShutLining::SetFaceted(bool b)
{
  m_impl->SetParameter(MYON_SHUTLINING_FACETED, b);
}

bool MYON_ShutLining::AutoUpdate(void) const
{
  return m_impl->GetParameter(MYON_SHUTLINING_AUTO_UPDATE, false).AsBool();
}

void MYON_ShutLining::SetAutoUpdate(bool b)
{
  m_impl->SetParameter(MYON_SHUTLINING_AUTO_UPDATE, b);
}

bool MYON_ShutLining::ForceUpdate(void) const
{
  return m_impl->GetParameter(MYON_SHUTLINING_FORCE_UPDATE, false).AsBool();
}

void MYON_ShutLining::SetForceUpdate(bool b)
{
  m_impl->SetParameter(MYON_SHUTLINING_FORCE_UPDATE, b);
}

MYON_ShutLining::CurveIterator MYON_ShutLining::GetCurveIterator(void) const
{
  return CurveIterator(*this);
}

MYON_ShutLining::Curve& MYON_ShutLining::AddCurve(void)
{
  MYON_XMLNode* curve_node = m_impl->Node().AttachChildNode(new MYON_XMLNode(MYON_SHUTLINING_CURVE));
  Curve* curve = new Curve(*curve_node);
  m_impl_sl->m_curves.Append(curve);

  return *curve;
}

void MYON_ShutLining::DeleteAllCurves(void)
{
  for (int i = 0; i < m_impl_sl->m_curves.Count(); i++)
  {
    delete m_impl_sl->m_curves[i];
  }

  m_impl_sl->m_curves.Destroy();
}

MYON_XMLNode* MYON_ShutLining::AddChildXML(MYON_XMLRootNode& root) const
{
  MYON_XMLNode* sl_node = MYON_MeshModifier::AddChildXML(root);

  for (int i = 0; i < m_impl_sl->m_curves.Count(); i++)
  {
    MYON_XMLNode* curve_node = sl_node->AttachChildNode(new MYON_XMLNode(L""));
    m_impl_sl->m_curves[i]->ToXML(*curve_node);
  }

  return sl_node;
}

MYON_UUID MYON_ShutLining::Uuid(void) const
{
  // The unique id of the mesh modifier is the same as the id of its user data.
  return MYON_ShutLiningUserData::Uuid();
}

bool   MYON_ShutLining::Defaults::Faceted(void)     { return false; }
bool   MYON_ShutLining::Defaults::AutoUpdate(void)  { return false; }
bool   MYON_ShutLining::Defaults::ForceUpdate(void) { return false; }
double MYON_ShutLining::Defaults::Tolerance(void)   { return 0.001; }

class MYON_ShutLining::Curve::CImpl final
{
public:
  CImpl(const MYON_XMLNode& n) : m_node(n) { }

  MYON_XMLVariant GetParam(const wchar_t* param_name, const MYON_XMLVariant& def) const
  {
    const MYON_XMLNode* child_node = m_node.GetNamedChild(param_name);
    if (nullptr != child_node)
      return child_node->GetDefaultProperty().GetValue();

    return def;
  }

  void SetParam(const wchar_t* param_name, const MYON_XMLVariant& v)
  {
    const MYON_XMLNode* child_node = m_node.CreateNodeAtPath(param_name);
    child_node->GetDefaultProperty().SetValue(v);
  }

public:
  MYON_XMLNode m_node;
};

MYON_ShutLining::Curve::Curve(const MYON_XMLNode& curve_node)
{
  m_impl = new CImpl(curve_node);
}

MYON_ShutLining::Curve::~Curve()
{
  delete m_impl;
  m_impl = nullptr;
}

const MYON_ShutLining::Curve& MYON_ShutLining::Curve::operator = (const MYON_ShutLining::Curve& c)
{
  if (this != &c)
  {
    SetId     (c.Id());
    SetEnabled(c.Enabled());
    SetRadius (c.Radius());
    SetProfile(c.Profile());
    SetPull   (c.Pull());
    SetIsBump (c.IsBump());
  }

  return *this;
}

bool MYON_ShutLining::Curve::operator == (const Curve& c) const
{
  if (Id()      != c.Id())      return false;
  if (Enabled() != c.Enabled()) return false;
  if (Radius()  != c.Radius())  return false;
  if (Profile() != c.Profile()) return false;
  if (Pull()    != c.Pull())    return false;
  if (IsBump()  != c.IsBump())  return false;

  return true;
}

bool MYON_ShutLining::Curve::operator != (const Curve& c) const
{
  return !(operator == (c));
}

MYON_UUID MYON_ShutLining::Curve::Id(void) const
{
  return m_impl->GetParam(MYON_SHUTLINING_CURVE_UUID, MYON_nil_uuid);
}

void MYON_ShutLining::Curve::SetId(const MYON_UUID& id)
{
  m_impl->SetParam(MYON_SHUTLINING_CURVE_UUID, id);
}

double MYON_ShutLining::Curve::Radius(void) const
{
  return m_impl->GetParam(MYON_SHUTLINING_CURVE_RADIUS, 1.0);
}

void MYON_ShutLining::Curve::SetRadius(double d)
{
  m_impl->SetParam(MYON_SHUTLINING_CURVE_RADIUS, d);
}

int MYON_ShutLining::Curve::Profile(void) const
{
  return m_impl->GetParam(MYON_SHUTLINING_CURVE_PROFILE, 0);
}

void MYON_ShutLining::Curve::SetProfile(int p)
{
  m_impl->SetParam(MYON_SHUTLINING_CURVE_PROFILE, p);
}

bool MYON_ShutLining::Curve::Enabled(void) const
{
  return m_impl->GetParam(MYON_SHUTLINING_CURVE_ENABLED, false);
}

void MYON_ShutLining::Curve::SetEnabled(bool b)
{
  m_impl->SetParam(MYON_SHUTLINING_CURVE_ENABLED, b);
}

bool MYON_ShutLining::Curve::Pull(void) const
{
  return m_impl->GetParam(MYON_SHUTLINING_CURVE_PULL, false);
}

void MYON_ShutLining::Curve::SetPull(bool b)
{
  m_impl->SetParam(MYON_SHUTLINING_CURVE_PULL, b);
}

bool MYON_ShutLining::Curve::IsBump(void) const
{
  return m_impl->GetParam(MYON_SHUTLINING_CURVE_IS_BUMP, false);
}

void MYON_ShutLining::Curve::SetIsBump(bool b)
{
  m_impl->SetParam(MYON_SHUTLINING_CURVE_IS_BUMP, b);
}

void MYON_ShutLining::Curve::ToXML(MYON_XMLNode& node) const
{
  node = m_impl->m_node;
}

bool   MYON_ShutLining::Curve::Defaults::Enabled(void) { return true; }
bool   MYON_ShutLining::Curve::Defaults::Pull(void)    { return true; }
bool   MYON_ShutLining::Curve::Defaults::IsBump(void)  { return false; }
int    MYON_ShutLining::Curve::Defaults::Profile(void) { return 1; }
double MYON_ShutLining::Curve::Defaults::Radius(void)  { return 0.1; }

class MYON_ShutLining::CurveIterator::CImpl
{
public:
  CImpl(const MYON_ShutLining& sl) : m_sl(sl) { }

  Curve* Next(void)
  {
    if (m_index >= m_sl.m_impl_sl->m_curves.Count())
      return nullptr;

    return m_sl.m_impl_sl->m_curves[m_index++];
  }

  const MYON_ShutLining& m_sl;
  int m_index = 0;
};

MYON_ShutLining::CurveIterator::CurveIterator(const MYON_ShutLining& sl)
{
  m_impl = new CImpl(sl);
}

MYON_ShutLining::CurveIterator::~CurveIterator()
{
  delete m_impl;
  m_impl = nullptr;
}

MYON_ShutLining::Curve* MYON_ShutLining::CurveIterator::Next(void)
{
  return m_impl->Next();
}

// MYON_MeshModifiers

class MYON_MeshModifiers::CImpl
{
public:
  ~CImpl();

  void LoadFromXML(const MYON_XMLRootNode& root);
  void SaveToXML(MYON_XMLRootNode& root) const;

  void DeleteAll(void);

  MYON_Displacement * m_displacement   = nullptr;
  MYON_EdgeSoftening* m_edge_softening = nullptr;
  MYON_Thickening   * m_thickening     = nullptr;
  MYON_CurvePiping  * m_curve_piping   = nullptr;
  MYON_ShutLining   * m_shut_lining    = nullptr;
};

void MYON_MeshModifiers::CImpl::LoadFromXML(const MYON_XMLRootNode& root)
{
  auto it = root.GetChildIterator();
  while (MYON_XMLNode* child_node = it.GetNextChild())
  {
    const MYON_wString& s = child_node->TagName();

    if (MYON_DISPLACEMENT_ROOT == s)
    {
      m_displacement = new MYON_Displacement(*child_node);
    }
    else
    if (MYON_EDGE_SOFTENING_ROOT == s)
    {
      m_edge_softening = new MYON_EdgeSoftening(*child_node);
    }
    else
    if (MYON_THICKENING_ROOT == s)
    {
      m_thickening = new MYON_Thickening(*child_node);
    }
    else
    if (MYON_CURVE_PIPING_ROOT == s)
    {
      m_curve_piping = new MYON_CurvePiping(*child_node);
    }
    else
    if (MYON_SHUTLINING_ROOT == s)
    {
      m_shut_lining = new MYON_ShutLining(*child_node);
    }
  }
}

MYON_MeshModifiers::CImpl::~CImpl()
{
  DeleteAll();
}

void MYON_MeshModifiers::CImpl::DeleteAll(void)
{
  if (nullptr != m_displacement)
  {
    delete m_displacement;
    m_displacement = nullptr;
  }

  if (nullptr != m_edge_softening)
  {
    delete m_edge_softening;
    m_edge_softening = nullptr;
  }

  if (nullptr != m_thickening)
  {
    delete m_thickening;
    m_thickening = nullptr;
  }

  if (nullptr != m_curve_piping)
  {
    delete m_curve_piping;
    m_curve_piping = nullptr;
  }

  if (nullptr != m_shut_lining)
  {
    delete m_shut_lining;
    m_shut_lining = nullptr;
  }
}

MYON_MeshModifiers::MYON_MeshModifiers()
{
  m_impl = new CImpl;
}

MYON_MeshModifiers::~MYON_MeshModifiers()
{
  delete m_impl;
  m_impl = nullptr;
}

void MYON_MeshModifiers::LoadFromXML(const MYON_XMLRootNode& node)
{
  m_impl->LoadFromXML(node);
}

const MYON_MeshModifiers& MYON_MeshModifiers::operator = (const MYON_MeshModifiers& mm)
{
  m_impl->DeleteAll();

  if (nullptr != mm.m_impl->m_displacement)
    m_impl->m_displacement = new MYON_Displacement(*mm.m_impl->m_displacement);

  if (nullptr != mm.m_impl->m_edge_softening)
    m_impl->m_edge_softening = new MYON_EdgeSoftening(*mm.m_impl->m_edge_softening);

  if (nullptr != mm.m_impl->m_thickening)
    m_impl->m_thickening = new MYON_Thickening(*mm.m_impl->m_thickening);

  if (nullptr != mm.m_impl->m_curve_piping)
    m_impl->m_curve_piping = new MYON_CurvePiping(*mm.m_impl->m_curve_piping);

  if (nullptr != mm.m_impl->m_shut_lining)
    m_impl->m_shut_lining = new MYON_ShutLining(*mm.m_impl->m_shut_lining);

  return *this;
}

MYON_Displacement* MYON_MeshModifiers::Displacement(bool allow_creation)
{
  if ((nullptr == m_impl->m_displacement) && allow_creation)
    m_impl->m_displacement = new MYON_Displacement;

  return m_impl->m_displacement;
}

MYON_EdgeSoftening* MYON_MeshModifiers::EdgeSoftening(bool allow_creation)
{
  if ((nullptr == m_impl->m_edge_softening) && allow_creation)
    m_impl->m_edge_softening = new MYON_EdgeSoftening;

  return m_impl->m_edge_softening;
}

MYON_Thickening* MYON_MeshModifiers::Thickening(bool allow_creation)
{
  if ((nullptr == m_impl->m_thickening) && allow_creation)
    m_impl->m_thickening = new MYON_Thickening;

  return m_impl->m_thickening;
}

MYON_CurvePiping* MYON_MeshModifiers::CurvePiping(bool allow_creation)
{
  if ((nullptr == m_impl->m_curve_piping) && allow_creation)
    m_impl->m_curve_piping = new MYON_CurvePiping;

  return m_impl->m_curve_piping;
}

MYON_ShutLining* MYON_MeshModifiers::ShutLining(bool allow_creation)
{
  if ((nullptr == m_impl->m_shut_lining) && allow_creation)
    m_impl->m_shut_lining = new MYON_ShutLining;

  return m_impl->m_shut_lining;
}

void CreateMeshModifiersFromXML(const MYONX_Model& model, int archive_3dm_version)
{
  MYONX_ModelComponentIterator cit(model, MYON_ModelComponent::Type::ModelGeometry);

  for (const MYON_ModelComponent* component = cit.FirstComponent(); nullptr != component; component = cit.NextComponent())
  {
    MYON_3dmObjectAttributes* attr = GetComponentAttributes(*component);
    if (nullptr == attr)
      continue; // No attributes on component.

    // Get the entire XML off of the attributes user data.
    MYON_wString xml;
    ::GetMeshModifierObjectInformation(*attr, xml, archive_3dm_version);
    if (xml.IsEmpty())
      continue; // No XML found on the component's attributes.

    MYON_XMLRootNode root;
    if (MYON_XMLRootNode::ReadError != root.ReadFromStream(xml))
    {
      attr->MeshModifiers().LoadFromXML(root);
    }
  }
}

void CreateXMLFromMeshModifiers(const MYONX_Model& model, int archive_3dm_version)
{
  MYONX_ModelComponentIterator cit(model, MYON_ModelComponent::Type::ModelGeometry);

  for (const MYON_ModelComponent* component = cit.FirstComponent(); nullptr != component; component = cit.NextComponent())
  {
    MYON_3dmObjectAttributes* attr = GetComponentAttributes(*component);
    if (nullptr == attr)
      continue; // No attributes on component.

    MYON_MeshModifiers& mm = attr->MeshModifiers();
    SetMeshModifierObjectInformation(*attr, mm.Displacement(),  archive_3dm_version);
    SetMeshModifierObjectInformation(*attr, mm.EdgeSoftening(), archive_3dm_version);
    SetMeshModifierObjectInformation(*attr, mm.Thickening(),    archive_3dm_version);
    SetMeshModifierObjectInformation(*attr, mm.CurvePiping(),   archive_3dm_version);
    SetMeshModifierObjectInformation(*attr, mm.ShutLining(),    archive_3dm_version);
  }
}

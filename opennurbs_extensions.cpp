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

const MYONX_ErrorCounter MYONX_ErrorCounter::operator+ (
  const MYONX_ErrorCounter& rhs
  )
{
  MYONX_ErrorCounter sum;
  sum.m_failure_count = m_failure_count + rhs.m_failure_count;
  sum.m_error_count = m_error_count + rhs.m_error_count;
  sum.m_warning_count = m_warning_count + rhs.m_warning_count;


  sum.m_state_bit_field = m_state_bit_field | rhs.m_state_bit_field;
  sum.m_opennurbs_library_error_count
    = (m_opennurbs_library_error_count < rhs.m_opennurbs_library_error_count)
    ? rhs.m_opennurbs_library_error_count
    : m_opennurbs_library_error_count;
  sum.m_opennurbs_library_warning_count
    = (m_opennurbs_library_warning_count < rhs.m_opennurbs_library_warning_count)
    ? rhs.m_opennurbs_library_warning_count
    : m_opennurbs_library_warning_count;

  return sum;
}


const MYONX_ErrorCounter MYONX_ErrorCounter::operator+= (const MYONX_ErrorCounter& rhs)
{
  m_failure_count += rhs.m_failure_count;
  m_error_count += rhs.m_error_count;
  m_warning_count += rhs.m_warning_count;

  m_state_bit_field |= rhs.m_state_bit_field;
  if (m_opennurbs_library_error_count < rhs.m_opennurbs_library_error_count)
    m_opennurbs_library_error_count = rhs.m_opennurbs_library_error_count;
  if (m_opennurbs_library_warning_count < rhs.m_opennurbs_library_warning_count)
    m_opennurbs_library_warning_count = rhs.m_opennurbs_library_warning_count;

  return *this;
}


unsigned int MYONX_ErrorCounter::FailureCount() const
{
  return this->m_failure_count;
}

unsigned int MYONX_ErrorCounter::ErrorCount() const
{
  return this->m_error_count;
}

unsigned int MYONX_ErrorCounter::WarningCount() const
{
  return this->m_warning_count;
}

unsigned int MYONX_ErrorCounter::TotalCount() const
{
  return FailureCount() + ErrorCount() + WarningCount();
}

unsigned int MYONX_ErrorCounter::IncrementFailureCount()
{
  return ++m_failure_count; // <- Good location for a debugger breakpoint.
}

unsigned int MYONX_ErrorCounter::IncrementErrorCount()
{
  return ++m_error_count; // <- Good location for a debugger breakpoint.
}

unsigned int MYONX_ErrorCounter::IncrementWarningCount()
{
  return ++m_warning_count; // <- Good location for a debugger breakpoint.
}

void MYONX_ErrorCounter::ClearLibraryErrors()
{
  m_opennurbs_library_error_count = MYON_GetErrorCount();
  m_state_bit_field |= 1;
}

unsigned int MYONX_ErrorCounter::AddLibraryErrors()
{
  const bool bActive = (0 != (1 & m_state_bit_field));
  const unsigned int count0 = m_opennurbs_library_error_count;
  ClearLibraryErrors();
  const unsigned int count
    = bActive
    ? (m_opennurbs_library_error_count - count0)
    : 0U;
  if (bActive && count > 0)
    m_error_count += count; // <- Good location for a debugger breakpoint.
  return count;
}

void MYONX_ErrorCounter::ClearLibraryWarnings()
{
  m_opennurbs_library_warning_count = MYON_GetWarningCount();
  m_state_bit_field |= 2;
}

unsigned int MYONX_ErrorCounter::AddLibraryWarnings()
{
  const bool bActive = (0 != (2 & m_state_bit_field));
  const unsigned int count0 = m_opennurbs_library_warning_count;
  ClearLibraryWarnings();
  const unsigned int count
    = bActive
    ? (m_opennurbs_library_warning_count - count0)
    : 0U;
  if (bActive && count>0)
    m_warning_count += count; // <- Good location for a debugger breakpoint.
  return count;
}

void MYONX_ErrorCounter::ClearLibraryErrorsAndWarnings()
{
  ClearLibraryErrors();
  ClearLibraryWarnings();
}

unsigned int MYONX_ErrorCounter::AddLibraryErrorsAndWarnings()
{
  return AddLibraryErrors() + AddLibraryWarnings();
}

void MYONX_ErrorCounter::Dump(MYON_TextLog& text_log) const
{
  text_log.Print(
      "%u failures, %u errors, %u warnings",
      m_failure_count,
      m_error_count,
      m_warning_count
    );
}

#if defined(OPENNURBS_EXPORTS)

////////////////////////////////////////////////////////////////////////////////
//
// When openNURBS is used as a Microsoft Windows DLL, it is possible 
// for new/delete to allocate memory in one executable and delete
// it in another.  Because Microsoft Windows has incompatible memory 
// managers in its plethora of C libraries and the choice of which
// C library actually gets used depends on the code generation 
// options you choose,  we get lots of support questions asking
// about hard to trace crashes.
//
// If you are using openNURBS as a Windows DLL, you are sure you know
// what you are doing, and you promise never to ask for support, then
// feel free to delete these overrides.
//
//
#if defined(MYON_COMPILER_MSC)
#pragma message( " --- OpenNURBS overriding MYONX_Model new and delete" )
#endif

// MYONX_Model_UserData new/delete

void* MYONX_Model_UserData::operator new(size_t sz)
{
  // MYONX_Model_UserData new
  return onmalloc(sz);
}

void MYONX_Model_UserData::operator delete(void* p)
{
  // MYONX_Model_UserData delete
  onfree(p);
}

void* MYONX_Model_UserData::operator new[] (size_t sz)
{
  // MYONX_Model_UserData array new
  return onmalloc(sz);
}

void MYONX_Model_UserData::operator delete[] (void* p)
{
  // MYONX_Model_UserData array delete
  onfree(p);
}

void* MYONX_Model_UserData::operator new(size_t, void* p)
{
  // MYONX_Model_UserData placement new
  return p;
}

void MYONX_Model_UserData::operator delete(void*, void*)
{
  // MYONX_Model_UserData placement delete
  return;
}

// MYONX_Model new/delete

void* MYONX_Model::operator new(size_t sz)
{
  // MYONX_Model new
  return onmalloc(sz);
}

void MYONX_Model::operator delete(void* p)
{
  // MYONX_Model delete
  onfree(p);
}

void* MYONX_Model::operator new[] (size_t sz)
{
  // MYONX_Model array new
  return onmalloc(sz);
}

void MYONX_Model::operator delete[] (void* p)
{
  // MYONX_Model array delete
  onfree(p);
}

void* MYONX_Model::operator new(size_t, void* p)
{
  // MYONX_Model placement new
  return p;
}

void MYONX_Model::operator delete(void*, void*)
{
  // MYONX_Model placement delete
  return;
}

#endif

//
//
////////////////////////////////////////////////////////////////////////////////

class MYONX_ModelComponentReferenceLink
{
public:
  MYONX_ModelComponentReferenceLink() = default;
  ~MYONX_ModelComponentReferenceLink() = default;
  MYONX_ModelComponentReferenceLink(const MYONX_ModelComponentReferenceLink&) = default;
  MYONX_ModelComponentReferenceLink& operator=(const MYONX_ModelComponentReferenceLink&) = default;

public:
  // m_mcr in this class is the "first"  std::shared_ptr<MYON_ModelComponent> that manages
  // the referenced MYON_ModelComponent and is what insures MYON_ModelComponent alive inside
  // and MYONX_Model for the duration of the model's lifetime.
  //
  // m_sn = MYON_ModelComponent.RuntimeSerialNumber() so sn lookup can be safely used to find runtime pointer.
  // When m_mcr.ModelComponent() is not nullptr, then m_sn = m_mcr.ModelComponent()->RuntimeSerialNumber().
  MYON_ModelComponentReference m_mcr;
  MYON__UINT64 m_sn = 0;
  MYONX_ModelComponentReferenceLink* m_next = nullptr;
  MYONX_ModelComponentReferenceLink* m_prev = nullptr;
};

enum class RenderContentKinds { Material, Environment, Texture };

MYON_3dmObjectAttributes* GetComponentAttributes(const MYON_ModelComponent& component)
{
  // To have attributes, the component must be an MYON_ModelGeometryComponent.
  const auto* mgc = MYON_ModelGeometryComponent::Cast(&component);
  if (nullptr == mgc)
    return nullptr; // Wrong type of component.

  return mgc->ExclusiveAttributes();
}

class MYONX_ModelPrivate final
{
public:
  MYONX_ModelPrivate(MYONX_Model& m);
  ~MYONX_ModelPrivate();

  using EmbeddedFileMap = std::unordered_map<std::wstring, std::wstring>;

  bool GetRDKDocumentXML(MYON_wString& xml, bool embedded_files, int archive_3dm_version) const;
  MYONX_Model_UserData* GetRDKDocumentUserData(int archive_3dm_version) const;
  void PopulateDefaultRDKDocumentXML(MYON_XMLRootNode& root) const;
  bool PopulateRDKComponents(int archive_3dm_version);
  bool UpdateRDKUserData(int archive_3dm_version);
  bool CreateRenderContentFromXML(class MYON_XMLNode& model_node, RenderContentKinds kind);
  bool CreatePostEffectsFromXML(MYON_XMLNode& model_node, MYON_PostEffect::Types type);
  bool CreateXMLFromRenderContent(MYON_XMLNode& model_node, RenderContentKinds kind) const;
  bool CreateXMLFromPostEffects(MYON_XMLNode& model_node, MYON_PostEffect::Types type) const;
  bool SetRDKDocumentInformation(const wchar_t* xml, MYONX_Model_UserData& docud, int archive_3dm_version) const;
  MYON_XMLNode* GetRenderContentSectionNode(MYON_XMLNode& model_node, RenderContentKinds kind) const;
  MYON_XMLNode* GetPostEffectSectionNode(MYON_XMLNode& model_node, MYON_PostEffect::Types type) const;
  static void RemoveAllEmbeddedFiles(MYONX_Model& model);
  static bool GetEntireRDKDocument(const MYONX_Model_UserData& docud, MYON_wString& xml, MYONX_Model* model);

public:
  MYONX_Model& m_model;
  MYON__UINT64 m_model_content_version_number = 0;
  MYON_ClassArray<MYONX_Model::MYONX_ModelComponentList> m_mcr_lists;
};

MYON_InternalXMLImpl::~MYON_InternalXMLImpl()
{
  if (nullptr != m_local_node)
  {
    delete m_local_node;
    m_local_node = nullptr;
  }
}

MYON_XMLNode& MYON_InternalXMLImpl::Node(void) const
{
  // If the model node pointer is set, return that. This is a pointer to a node owned by the MYONX_Model which
  // contains the entire RDK document XML. This is used by model objects (Ground Plane, etc.) that are owned
  // by the MYONX_Model. In the case of Ground Plane etc, it's a pointer into the MYONX_Model's XML.
  // In the case of decals, it's a pointer into the decal collection's XML.
  if (nullptr != m_model_node)
    return *m_model_node;

  // Since the model node is not set, we need a local node to hold the XML. If one has not been created yet,
  // create it. The local node is owned by this object. This case occurs for free-floating copies of model
  // objects and also for free-floating copies of decals and mesh modifiers. This node only contains the XML
  // data that's relevant to the object it's for, not the entire XML.
  if (nullptr == m_local_node)
    m_local_node = new MYON_XMLNode(NameOfRootNode());

  return *m_local_node;
}

MYON_wString MYON_InternalXMLImpl::NameOfRootNode(void) const
{
  return MYON_XMLRootNode().TagName();
}

MYON_XMLVariant MYON_InternalXMLImpl::GetParameter(const wchar_t* path_to_node, const wchar_t* param_name, const MYON_XMLVariant& def) const
{
  return InternalGetParameter(path_to_node, param_name, L"", def);
}

MYON_XMLVariant MYON_InternalXMLImpl::GetParameter_NoType(const wchar_t* path_to_node, const wchar_t* param_name, const wchar_t* default_type, const MYON_XMLVariant& def) const
{
  return InternalGetParameter(path_to_node, param_name, default_type, def);
}

MYON_XMLVariant MYON_InternalXMLImpl::InternalGetParameter(const wchar_t* path_to_node, const wchar_t* param_name, const wchar_t* default_type, const MYON_XMLVariant& def) const
{
  std::lock_guard<std::recursive_mutex> lg(m_mutex);

  const MYON_XMLNode* node_read = Node().GetNodeAtPath(path_to_node);
  if (nullptr == node_read)
    return def;

  MYON_XMLVariant value;
  MYON_XMLParameters p(*node_read);
  p.SetDefaultReadType(default_type);
  if (!p.GetParam(param_name, value))
    return def;

  return value;
}

bool MYON_InternalXMLImpl::SetParameter(const wchar_t* path_to_node, const wchar_t* param_name, const MYON_XMLVariant& value)
{
  return InternalSetParameter(path_to_node, param_name, true, value);
}

bool MYON_InternalXMLImpl::SetParameter_NoType(const wchar_t* path_to_node, const wchar_t* param_name, const MYON_XMLVariant& value)
{
  return InternalSetParameter(path_to_node, param_name, false, value);
}

bool MYON_InternalXMLImpl::InternalSetParameter(const wchar_t* path_to_node, const wchar_t* param_name, bool write_type, const MYON_XMLVariant& value)
{
  std::lock_guard<std::recursive_mutex> lg(m_mutex);

  bool success = false;

  MYON_XMLNode* node_write = Node().CreateNodeAtPath(path_to_node);
  if (nullptr != node_write)
  {
    MYON_XMLParameters p(*node_write);
    p.SetWriteTypeProperty(write_type);
    if (nullptr != p.SetParam(param_name, value))
      success = true;
  }

  return success;
}

MYONX_Model::MYONX_Model()
{
  m_private = new MYONX_ModelPrivate(*this);
}

MYONX_Model::~MYONX_Model()
{
  Reset();

  delete m_private;
  m_private = nullptr;
}

void MYONX_Model::Reset()
{
  m_3dm_file_version = 0;
  m_3dm_opennurbs_version = 0;
  m_sStartSectionComments = MYON_String::EmptyString;
  m_properties = MYON_3dmProperties::Empty;
  m_settings = MYON_3dmSettings::Default;

  for (unsigned int i = 0; i < m_userdata_table.UnsignedCount(); i++)
  {
    delete m_userdata_table[i];
  }
  m_userdata_table.Destroy();

  for (int i = 0; i < m_private->m_mcr_lists.Count(); i++)
  {
    MYONX_ModelComponentList& list = m_private->m_mcr_lists[i];

    MYONX_ModelComponentReferenceLink* mcr_link = list.m_first_mcr_link;
    while (nullptr != mcr_link)
    {
      mcr_link->m_mcr = MYON_ModelComponentReference::Empty;
      mcr_link = mcr_link->m_next;
    }

    list.m_first_mcr_link = nullptr;
    list.m_last_mcr_link = nullptr;
  }
  m_mcr_sn_map.EmptyList();
  m_mcr_link_fsp.ReturnAll();

  m_default_line_pattern = MYON_ModelComponentReference::CreateConstantSystemComponentReference(MYON_Linetype::Continuous);
  m_default_layer = MYON_ModelComponentReference::CreateConstantSystemComponentReference(MYON_Layer::Default);
  m_default_text_style = MYON_ModelComponentReference::CreateConstantSystemComponentReference(MYON_TextStyle::Default);
  m_default_dimension_style = MYON_ModelComponentReference::CreateConstantSystemComponentReference(MYON_DimStyle::Default);
  
  m_manifest.Reset();

  m_original_to_manifest_map = MYON_ManifestMap::Empty;
  m_manifest_to_original_map = MYON_ManifestMap::Empty;

  m_model_geometry_bbox = MYON_BoundingBox::UnsetBoundingBox;
  m_render_light_bbox = MYON_BoundingBox::UnsetBoundingBox;

  if (nullptr != m_model_user_string_list)
  {
    delete m_model_user_string_list;
    m_model_user_string_list = nullptr;
  }
}

void MYONX_Model::Internal_ComponentTypeBoundingBox(
  const MYON_ModelComponent::Type component_type,
  MYON_BoundingBox& bbox
  ) const
{
  if (false == bbox.IsValid())
  {
    MYON_BoundingBox local_bbox;
    for (
      const MYONX_ModelComponentReferenceLink* link = Internal_ComponentListConst(component_type).m_first_mcr_link;
      nullptr != link; 
      link = link->m_next
      )
    {
      const MYON_ModelComponent* model_component = link->m_mcr.ModelComponent();
      if (nullptr == model_component)
        continue;
      if (component_type != model_component->ComponentType())
        continue;
      const MYON_ModelGeometryComponent* model_geometry = MYON_ModelGeometryComponent::Cast(model_component);
      if (nullptr == model_geometry)
        continue;
      const MYON_3dmObjectAttributes* attributes = model_geometry->Attributes(nullptr);
      if (nullptr != attributes && attributes->IsInstanceDefinitionObject())
        continue;

      const MYON_Geometry* geometry = model_geometry->Geometry(nullptr);
      if (nullptr == geometry)
        continue;

      local_bbox.Union(geometry->BoundingBox());
    }
    bbox = local_bbox;
  }
  return;
}

MYON_BoundingBox MYONX_Model::ModelGeometryBoundingBox() const
{
  Internal_ComponentTypeBoundingBox(MYON_ModelComponent::Type::ModelGeometry,m_model_geometry_bbox);
  return m_model_geometry_bbox;
}

MYON_BoundingBox MYONX_Model::RenderLightBoundingBox() const
{
  Internal_ComponentTypeBoundingBox(MYON_ModelComponent::Type::RenderLight,m_render_light_bbox);
  return m_render_light_bbox;
}

const MYON_ComponentManifest& MYONX_Model::Manifest() const
{
  return m_manifest;
}

const MYON_ManifestMap& MYONX_Model::OriginalToModelMap() const
{
  return m_original_to_manifest_map;
}

const MYON_ManifestMap& MYONX_Model::ModelToOriginalMap() const
{
  return m_manifest_to_original_map;
}

bool MYONX_Model::ValdateComponentIdAndName(
  MYON_ModelComponent::Type component_type,
  const MYON_UUID& candidate_id,
  const MYON_UUID& component_parent_id,
  const wchar_t* candidate_name,
  bool bResolveIdConflict,
  bool bResolveNameConflict,
  MYON_UUID& model_id,
  MYON_wString& model_name
  ) const
{
  for (;;)
  {
    if (false == MYON_ModelComponent::ComponentTypeIsValid(component_type))
    {
      MYON_ERROR("Invalid component_type parameter.");
      break;
    }

    const bool bIndexRequired = MYON_ModelComponent::IndexRequired(component_type);
    const unsigned int count = m_manifest.ComponentIndexLimit(component_type);
    if (bIndexRequired && count >= 0x7FFFFFFFU)
    {
      MYON_ERROR("Unable to create model component index.");
      break;
    }
    
    const bool bIdAvailable = m_manifest.IdIsAvailable(candidate_id);
    const bool bCreateId = MYON_UuidIsNil(candidate_id) || (false == bIdAvailable && bResolveIdConflict);
    if (false == bIdAvailable && false == bCreateId)
    {
      break;
    }

    MYON_wString name(candidate_name);
    name.TrimLeftAndRight();
    const bool bUniqueNameReqired = MYON_ModelComponent::UniqueNameRequired(component_type);
    if ( bUniqueNameReqired )
    {
      const MYON_UUID name_parent_id
        = MYON_ModelComponent::UniqueNameIncludesParent(component_type)
        ? component_parent_id
        : MYON_nil_uuid;
      MYON_NameHash name_hash = MYON_NameHash::Create(name_parent_id, name);
      if (name_hash.IsInvalidNameHash())
      {
        if (false == bResolveNameConflict)
        {
          MYON_ERROR("Invalid candidate_name parameter.");
          break;
        }
        name = MYON_wString::EmptyString;
        name_hash = MYON_NameHash::Create(name_parent_id, name);
      }

      const bool bNameIsValid = name.IsNotEmpty() && m_manifest.NameIsAvailable(component_type, name_hash);
      if (false == bNameIsValid )
      {
        // we need to create a unique and non-empty name
        if (false == bResolveNameConflict)
        {
          // not allowed to modify name
          break;
        }

        name = m_manifest.UnusedName(component_type, component_parent_id, nullptr, name, nullptr, MYON_UNSET_UINT_INDEX, nullptr);
        if (name.IsEmpty())
        {
          MYON_ERROR("Unable to create component name.");
          break;
        }
      }
    }

    model_id = bCreateId ? MYON_CreateId() : candidate_id;
    model_name = name;
    return true;
  }

  model_id = MYON_nil_uuid;
  model_name = MYON_wString::EmptyString;
  return false;
}

unsigned int MYONX_Model::ComponentIndexLimit(
    MYON_ModelComponent::Type component_type
    ) const
{
  return m_manifest.ComponentIndexLimit(component_type);
}

unsigned int MYONX_Model::ActiveAndDeletedComponentCount(
    MYON_ModelComponent::Type component_type
    ) const
{
  return m_manifest.ActiveAndDeletedComponentCount(component_type);
}

unsigned int MYONX_Model::ActiveComponentCount(
    MYON_ModelComponent::Type component_type
    ) const
{
  return m_manifest.ActiveComponentCount(component_type);
}

unsigned int MYONX_Model::DeletedComponentCount(
    MYON_ModelComponent::Type component_type
    ) const
{
  return m_manifest.DeletedComponentCount(component_type);
}

MYON_ModelComponentReference MYONX_Model::ComponentFromIndex(
  MYON_ModelComponent::Type component_type,
  int component_model_index
  ) const
{
  if (component_model_index >= 0)
  {
    return ComponentFromUnsignedIndex(component_type,(unsigned int)component_model_index);
  }
  return MYON_ModelComponentReference::Empty;
}

MYON_ModelComponentReference MYONX_Model::ComponentFromUnsignedIndex(
  MYON_ModelComponent::Type component_type,
  unsigned int component_model_index
  ) const
{
  MYON__UINT64 sn = m_manifest.ItemFromIndex(component_type,component_model_index).ComponentRuntimeSerialNumber();
  MYONX_ModelComponentReferenceLink* link = Internal_ModelComponentLinkFromSerialNumber(sn);
  return (nullptr != link)
    ? link->m_mcr
    : MYON_ModelComponentReference::Empty;
}

MYON_ModelComponentReference MYONX_Model::ComponentFromId(
  MYON_ModelComponent::Type component_type,
  MYON_UUID component_model_id
  ) const
{
  MYON__UINT64 sn = m_manifest.ItemFromId(component_type, component_model_id).ComponentRuntimeSerialNumber();
  MYONX_ModelComponentReferenceLink* link = Internal_ModelComponentLinkFromSerialNumber(sn);
  return (nullptr != link)
    ? link->m_mcr
    : MYON_ModelComponentReference::Empty;
}

const MYON_ModelComponentReference& MYONX_Model::ComponentFromRuntimeSerialNumber(
  MYON__UINT64 runtime_serial_number
) const
{
  MYONX_ModelComponentReferenceLink* link = Internal_ModelComponentLinkFromSerialNumber(runtime_serial_number);
  return (nullptr != link)
    ? link->m_mcr
    : MYON_ModelComponentReference::Empty;
}

MYON_ModelComponentReference MYONX_Model::ModelGeometryFromId(
  MYON_UUID model_object_id
) const
{
  return ComponentFromId(MYON_ModelComponent::Type::ModelGeometry, model_object_id);
}

const MYON_ModelGeometryComponent& MYONX_Model::ModelGeometryComponentFromId(
  MYON_UUID model_object_id
) const
{
  const MYON_ModelGeometryComponent* p = MYON_ModelGeometryComponent::Cast(ModelGeometryFromId(model_object_id).ModelComponent());
  return (nullptr != p) ? *p : MYON_ModelGeometryComponent::Unset;
}

MYON_ModelComponentReference MYONX_Model::ComponentFromName(
  MYON_ModelComponent::Type component_type,
  MYON_UUID component_parent_id,
  const wchar_t* component_model_name
  ) const
{
  const MYON_UUID name_parent_id 
    = MYON_ModelComponent::UniqueNameIncludesParent(component_type)
    ? component_parent_id
    : MYON_nil_uuid;
  const bool bIgnoreCase = MYON_ModelComponent::UniqueNameIgnoresCase(component_type);
  const MYON_NameHash component_model_name_hash = MYON_NameHash::Create(name_parent_id,component_model_name,bIgnoreCase);
  return ComponentFromNameHash(component_type,component_model_name_hash);
}

MYON_ModelComponentReference MYONX_Model::ComponentFromNameHash(
  MYON_ModelComponent::Type component_type,
  const MYON_NameHash& component_model_name_hash
  ) const
{
  MYON__UINT64 sn = m_manifest.ItemFromNameHash(component_type, component_model_name_hash).ComponentRuntimeSerialNumber();
  MYONX_ModelComponentReferenceLink* link = Internal_ModelComponentLinkFromSerialNumber(sn);
  return (nullptr != link)
    ? link->m_mcr
    : MYON_ModelComponentReference::Empty;
}


MYON_ModelComponentReference MYONX_Model::ImageFromIndex(
  int image_model_index
  ) const
{
  MYON_ModelComponentReference cr = ComponentFromIndex(MYON_ModelComponent::Type::Image, image_model_index);
  return cr;
}

MYON_ModelComponentReference MYONX_Model::ImageFromId(
  MYON_UUID image_id
  ) const
{
  MYON_ModelComponentReference cr = ComponentFromId(MYON_ModelComponent::Type::Image, image_id);
  return cr;
}

MYON_ModelComponentReference MYONX_Model::ImageFromFileFullPath(
  const wchar_t* image_file_full_path_name
  ) const
{
  MYON_FileReference file_reference;
  file_reference.SetFullPath(image_file_full_path_name,false);
  return ImageFromFileReference(file_reference);
}

MYON_ModelComponentReference MYONX_Model::ImageFromFileContent(
  const MYON_ContentHash& image_file_content_hash
  ) const
{
  MYON_FileReference file_reference;
  file_reference.SetContentHash(image_file_content_hash);
  return ImageFromFileReference(file_reference);
}

MYON_ModelComponentReference MYONX_Model::ImageFromFileReference(
  const MYON_FileReference& file_reference
  ) const
{
  const MYON_wString full_path_name(file_reference.FullPath());
  const bool bCheckFullPath = full_path_name.IsNotEmpty();

  const MYON_ContentHash file_content_hash = file_reference.ContentHash();
  const bool bCheckContentHash = file_content_hash.IsSet();
  
  if (false == bCheckFullPath && false == bCheckContentHash)
    return MYON_ModelComponentReference::Empty;

  MYONX_ModelComponentIterator it(*this,MYON_ModelComponent::Type::Image);

  for ( MYON_ModelComponentReference cr = it.FirstComponentReference(); false == cr.IsEmpty(); cr = it.NextComponentReference())
  {
    const MYON_Bitmap* image = MYON_Bitmap::Cast(cr.ModelComponent());
    if (nullptr == image)
      continue;
    const MYON_FileReference& image_file_reference = image->FileReference();

    if (bCheckFullPath)
    {
      if (false == full_path_name.EqualPath(image_file_reference.FullPath()))
        continue;
    }

    if (bCheckContentHash)
    {
      if (0 != MYON_ContentHash::CompareContent(file_content_hash, image_file_reference.ContentHash()))
        continue;
    }

    return cr;
  }

  return MYON_ModelComponentReference::Empty;
}


MYON_ModelComponentReference MYONX_Model::LinePatternFromIndex(
  int layer_model_index
  ) const
{
  MYON_ModelComponentReference cr = ComponentFromIndex(MYON_ModelComponent::Type::LinePattern,layer_model_index);
  return cr.IsEmpty() ? m_default_layer : cr;
}

MYON_ModelComponentReference MYONX_Model::LinePatternFromId(
  MYON_UUID layer_model_id
  ) const
{
  MYON_ModelComponentReference cr = ComponentFromId(MYON_ModelComponent::Type::LinePattern,layer_model_id);
  return cr.IsEmpty() ? m_default_layer : cr;
}

MYON_ModelComponentReference MYONX_Model::LinePatternFromName(
  const wchar_t* line_pattern_name
  ) const
{
  MYON_ModelComponentReference cr = ComponentFromName(MYON_ModelComponent::Type::LinePattern,MYON_nil_uuid,line_pattern_name);
  return cr.IsEmpty() ? m_default_layer : cr;
}

MYON_ModelComponentReference MYONX_Model::LinePatternFromNameHash(
    MYON_NameHash line_pattern_model_name_hash
    ) const
{
  MYON_ModelComponentReference cr = ComponentFromNameHash(MYON_ModelComponent::Type::LinePattern,line_pattern_model_name_hash);
  return cr.IsEmpty() ? m_default_layer : cr;
}

MYON_ModelComponentReference MYONX_Model::LinePatternFromAttributes(
  const MYON_3dmObjectAttributes& attributes
  ) const
{
  int line_pattern_index = MYON_Linetype::Continuous.Index();

  switch ( attributes.LinetypeSource() )
  {
  case MYON::linetype_from_layer:
    if (attributes.m_layer_index >= 0)
    {
      const MYON_Layer* layer = MYON_Layer::Cast(LayerFromIndex(attributes.m_layer_index).ModelComponent());
      if ( nullptr != layer )
        line_pattern_index = layer->LinetypeIndex();
    }
    break;
  case MYON::linetype_from_object:
    line_pattern_index = attributes.m_linetype_index;
    break;
  case MYON::linetype_from_parent:
    line_pattern_index = attributes.m_linetype_index;
    // TODO: if object is an instance definition, get linetype
    //       from instance references.
    break;
  }

  return LinePatternFromIndex(line_pattern_index);
}

MYON_ModelComponentReference MYONX_Model::LinePatternFromLayerIndex(
  int layer_index
) const
{
  MYON_ModelComponentReference layer_component = LayerFromIndex(layer_index);
  const MYON_Layer* layer = MYON_Layer::Cast(layer_component.ModelComponent());
  if (nullptr == layer)
    layer = &MYON_Layer::Default;
  return LinePatternFromIndex(layer->m_linetype_index);
}

MYON_ModelComponentReference MYONX_Model::LayerFromIndex(
  int layer_model_index
  ) const
{
  MYON_ModelComponentReference cr = ComponentFromIndex(MYON_ModelComponent::Type::Layer,layer_model_index);
  return cr.IsEmpty() ? m_default_layer : cr;
}

MYON_ModelComponentReference MYONX_Model::LayerFromId(
  MYON_UUID layer_model_id
  ) const
{
  MYON_ModelComponentReference cr = ComponentFromId(MYON_ModelComponent::Type::Layer,layer_model_id);
  return cr.IsEmpty() ? m_default_layer : cr;
}

MYON_ModelComponentReference MYONX_Model::LayerFromName(
  MYON_UUID layer_parent_id,
  const wchar_t* layer_model_name
  ) const
{
  MYON_ModelComponentReference cr = ComponentFromName(MYON_ModelComponent::Type::Layer,layer_parent_id,layer_model_name);
  return cr.IsEmpty() ? m_default_layer : cr;
}

MYON_ModelComponentReference MYONX_Model::LayerFromNameHash(
  const MYON_NameHash& layer_model_name_hash
  ) const
{
  MYON_ModelComponentReference cr = ComponentFromNameHash(MYON_ModelComponent::Type::Layer,layer_model_name_hash);
  return cr.IsEmpty() ? m_default_layer : cr;
}

MYON_ModelComponentReference MYONX_Model::LayerFromAttributes( 
  const MYON_3dmObjectAttributes& attributes
  ) const
{
  return LayerFromIndex(attributes.m_layer_index);
}

MYON_ModelComponentReference MYONX_Model::DimensionStyleFromIndex(
  int dimension_style_model_index
  ) const
{
  MYON_ModelComponentReference cr = ComponentFromIndex(MYON_ModelComponent::Type::DimStyle,dimension_style_model_index);
  return cr.IsEmpty() ? DefaultDimensionStyle() : cr;
}

MYON_ModelComponentReference MYONX_Model::DimensionStyleFromId(
  MYON_UUID dimension_style_model_id
  ) const
{
  MYON_ModelComponentReference cr = ComponentFromId(MYON_ModelComponent::Type::DimStyle,dimension_style_model_id);
  return cr.IsEmpty() ? DefaultDimensionStyle() : cr;
}

MYON_ModelComponentReference MYONX_Model::DimensionStyleFromName(
  const wchar_t* dimension_style_model_name
  ) const
{
  MYON_ModelComponentReference cr = ComponentFromName(MYON_ModelComponent::Type::DimStyle,MYON_nil_uuid,dimension_style_model_name);
  return cr.IsEmpty() ? DefaultDimensionStyle() : cr;
}

MYON_ModelComponentReference MYONX_Model::DimensionStyleFromNameHash(
  MYON_NameHash dimension_style_model_name_hash
  ) const
{
  MYON_ModelComponentReference cr = ComponentFromNameHash(MYON_ModelComponent::Type::DimStyle,dimension_style_model_name_hash);
  return cr.IsEmpty() ? DefaultDimensionStyle() : cr;
}

MYON_UUID MYONX_Model::CurrentDimensionStyleId() const
{
  const MYON_UUID id = m_settings.CurrentDimensionStyleId();
  if (MYON_nil_uuid == id)
    return id;
  if (id == MYON_DimStyle::SystemDimstyleFromId(id).Id())
    return id;
  const MYON_ModelComponentReference cr = ComponentFromId(MYON_ModelComponent::Type::DimStyle,id);
  if (false == cr.IsEmpty())
    return id;
  return MYON_nil_uuid;
}

MYON_ModelComponentReference MYONX_Model::CurrentDimensionStyle() const
{
  return DimensionStyleFromId(CurrentDimensionStyleId());
}

bool MYONX_Model::SetCurrentDimensionStyleId(
  MYON_UUID dimension_style_id
)
{
  for (;;)
  {
    if (MYON_nil_uuid == dimension_style_id)
      break;
    if (dimension_style_id == MYON_DimStyle::SystemDimstyleFromId(dimension_style_id).Id())
      break;
    const MYON_ModelComponentReference cr = ComponentFromId(MYON_ModelComponent::Type::DimStyle,dimension_style_id);
    if ( nullptr != MYON_DimStyle::Cast(cr.ModelComponent()) ) 
      break;
    MYON_ERROR("Invalid dimension_style_id parameter.");
    return false;
  }
  m_settings.SetCurrentDimensionStyleId(dimension_style_id);
  return true;
}

MYON_ModelComponentReference MYONX_Model::DefaultDimensionStyle() const
{
  return m_default_dimension_style;
}

static bool Internal_DimStyleHasFont(
  const MYON_ModelComponentReference& mcr,
  unsigned int managed_font_sn,
  double model_space_text_scale,
  bool bIgnoreSystemDimStyles
)
{
  for (;;)
  {
    if (0 == managed_font_sn)
      break;
    const MYON_DimStyle* dim_style_component = MYON_DimStyle::Cast(mcr.ModelComponent());
    if (nullptr == dim_style_component)
      break;
    if (managed_font_sn != dim_style_component->Font().ManagedFontSerialNumber())
      break; // wrong font
    if (bIgnoreSystemDimStyles && dim_style_component->IsSystemComponent())
      break; // no system components
    if (dim_style_component->ParentIdIsNotNil())
      break; // no overrides
    if (model_space_text_scale > 0.0 && !(model_space_text_scale == dim_style_component->DimScale()))
      break;
    return true;
  }
  return false;
}

MYON_ModelComponentReference MYONX_Model::FirstDimensionStyleFromManagedFontSerialNumber(
  unsigned int managed_font_serial_number,
  double model_space_text_scale,
  bool bIgnoreSystemDimStyles
) const
{
  if (Internal_DimStyleHasFont(m_default_dimension_style, managed_font_serial_number, model_space_text_scale, bIgnoreSystemDimStyles))
    return m_default_dimension_style;

  for (
    MYONX_ModelComponentReferenceLink* link = Internal_ComponentListConst(MYON_ModelComponent::Type::DimStyle).m_first_mcr_link;
    nullptr != link;
    link = link->m_next
    )
  {
    if (Internal_DimStyleHasFont(link->m_mcr, managed_font_serial_number, model_space_text_scale, bIgnoreSystemDimStyles))
      return link->m_mcr;
  }

  return MYON_ModelComponentReference::Empty;
}


MYON_ModelComponentReference MYONX_Model::FirstDimensionStyleFromFont(
  const MYON_Font* font,
  double model_space_text_scale,
  bool bIgnoreSystemDimStyles
  ) const
{
  const MYON_Font* managed_font
    = (nullptr != font)
    ? font->ManagedFont()
    : nullptr;

  const unsigned int managed_font_sn
    = (nullptr != managed_font)
    ? managed_font->ManagedFontSerialNumber()
    : 0;

  if ( 0 == managed_font_sn )
  {
    MYON_ERROR("Invalid font parameter");
    return MYON_ModelComponentReference::Empty;
  }

  return FirstDimensionStyleFromManagedFontSerialNumber(managed_font_sn, model_space_text_scale, bIgnoreSystemDimStyles);
}

MYON_ModelComponentReference MYONX_Model::DimensionStyleWithFontCharacteristics(
  const MYON_Font& font_characteristics,
  double model_space_text_scale
)
{
  // search for existing dimension style
  const bool bIgnoreSystemComponents = true;
  MYON_ModelComponentReference existing_mcr = this->FirstDimensionStyleFromFont(&font_characteristics, model_space_text_scale, bIgnoreSystemComponents);
  const MYON_DimStyle* dim_style = MYON_DimStyle::Cast(existing_mcr.ModelComponent());
  if (nullptr != dim_style)
    return existing_mcr;
  
  // create new dimension style
  const MYON_DimStyle* default_dim_style = MYON_DimStyle::Cast(this->DimensionStyleFromId(this->m_settings.CurrentDimensionStyleId()).ModelComponent());
  MYON_DimStyle* new_dim_style = MYON_DimStyle::CreateFromFont(&font_characteristics, model_space_text_scale, default_dim_style, &this->Manifest(), nullptr);
  if (nullptr != new_dim_style)
  {
    const bool bResolveIdAndNameConflicts = true;
    return this->AddManagedModelComponent(new_dim_style, bResolveIdAndNameConflicts);
  }

  return MYON_ModelComponentReference::Empty;
}


MYON_ModelComponentReference MYONX_Model::RemoveModelComponent(
  MYON_ModelComponent::Type component_type,
  MYON_UUID component_id
  )
{
  const MYON_ComponentManifestItem item = m_manifest.ItemFromId(component_id);
  if (item.IsUnset())
  {
    MYON_ERROR("Invalid component_id parameter.");
    return MYON_ModelComponentReference::Empty;
  }
  if (MYON_ModelComponent::Type::Unset != component_type && component_type != item.ComponentType())
  {
    MYON_ERROR("Invalid model_component type.");
    return MYON_ModelComponentReference::Empty;
  }
  if (!m_manifest.RemoveComponent(item.Id()))
  {
    MYON_ERROR("Unable to remove component from manifest.");
    return MYON_ModelComponentReference::Empty;
  }

  MYONX_ModelComponentReferenceLink* mcr_link = Internal_ModelComponentLinkFromSerialNumber(item.ComponentRuntimeSerialNumber());
  if (nullptr == mcr_link)
  {
    MYON_ERROR("component not in model.");
    return MYON_ModelComponentReference::Empty;
  }
  MYON_ModelComponentReference mcr = mcr_link->m_mcr;
  Internal_RemoveModelComponentReferenceLink(mcr_link);
  return mcr;
}


MYON_ModelComponentReference MYONX_Model::AddManagedModelComponent(
  class MYON_ModelComponent* managed_model_component
)
{
  return AddManagedModelComponent(managed_model_component,true);
}

int MYONX_Model::AddLayer(
  const wchar_t* layer_name,
  MYON_Color layer_color
)
{
  MYON_Layer layer;  
  const MYON_wString unused_name = m_manifest.UnusedName(layer.ComponentType(), layer.ParentId(), layer_name, nullptr, nullptr, 0, nullptr);
  layer.SetName(unused_name);  
  if ( layer_color != MYON_Color::UnsetColor)
    layer.SetColor(layer_color);
  const MYON_ModelComponentReference mr = AddModelComponent(layer, true);
  const MYON_Layer* managed_layer = MYON_Layer::FromModelComponentRef(mr, nullptr);
  int layer_index = (nullptr != managed_layer) ? managed_layer->Index() : MYON_UNSET_INT_INDEX;
  if ( layer_index < 0 )
  {
    MYON_ERROR("failed to add layer.");
  }
  return layer_index;
}

int MYONX_Model::AddDefaultLayer(
  const wchar_t* layer_name,
  MYON_Color layer_color
)
{
  MYON_UUID default_layer_id = m_settings.CurrentLayerId();
  int default_layer_index = m_settings.CurrentLayerIndex();
  for ( int pass = 0; pass < 2; pass++ )
  {
    if (0 == pass)
    {
      if (MYON_nil_uuid == default_layer_id)
        continue;
    }
    else 
    {
      if (MYON_UNSET_INT_INDEX == default_layer_index)
        continue;
    }
    MYON_ModelComponentReference mcr
      = (0 == pass)
      ? LayerFromId(default_layer_id)
      : LayerFromIndex(default_layer_index);

    const MYON_Layer* layer = MYON_Layer::FromModelComponentRef(mcr, nullptr);
    if (nullptr == layer)
      continue;
    if (false == layer->IsSystemComponent() && layer->Index() >= 0 && layer->ParentIdIsNil() && layer->IsVisible() && false == layer->IsLocked() )
    {
      m_settings.SetCurrentLayerId(layer->Id());
      return layer->Index();
    }
    MYON_Layer default_layer(*layer);
    default_layer.ClearId();
    default_layer.ClearIndex();
    default_layer.ClearParentId();
    default_layer.SetVisible(true);
    default_layer.SetLocked(false);

    if (nullptr == layer_name || 0 == layer_name[0])
      layer_name = layer->NameAsPointer();
    default_layer.SetName(m_manifest.UnusedName(default_layer.ComponentType(), MYON_nil_uuid, layer_name, nullptr, nullptr, 0, nullptr) );

    if (MYON_Color::UnsetColor != layer_color)
      default_layer.SetColor(layer_color);

    const MYON_ModelComponentReference mr = AddModelComponent(default_layer, true);
    const MYON_Layer* managed_layer = MYON_Layer::FromModelComponentRef(mr, nullptr);
    if (nullptr != managed_layer && managed_layer->Index() >= 0 && false == managed_layer->IsSystemComponent())
    {
      m_settings.SetCurrentLayerId(managed_layer->Id());
      return managed_layer->Index();
    }
  }

  int layer_index = AddLayer(layer_name, layer_color);
  if (layer_index >= 0)
  {
    const MYON_ModelComponentReference mr = LayerFromIndex(layer_index);
    const MYON_Layer* managed_layer = MYON_Layer::FromModelComponentRef(mr, nullptr);
    if (nullptr != managed_layer && managed_layer->Index() >= 0 && false == managed_layer->IsSystemComponent())
    {
      m_settings.SetCurrentLayerId(managed_layer->Id());
      return managed_layer->Index();
    }
  }

  MYON_ERROR("Failed to add default layer.");
  return MYON_UNSET_INT_INDEX;  
}

int MYONX_Model::AddDefaultDimensionStyle(
  const wchar_t* dimension_style_name,
  MYON::LengthUnitSystem length_unit_system,
  double model_tolerance
)
{
  const MYON_DimStyle* source_dimstyle = nullptr;

  MYON_UUID dimstyle_id = m_settings.CurrentDimensionStyleId();
  if (MYON_nil_uuid != dimstyle_id)
  {
    const MYON_DimStyle* dimstyle = MYON_DimStyle::FromModelComponentRef(DimensionStyleFromId(dimstyle_id),nullptr);
    if ( nullptr != dimstyle && dimstyle->ParentIdIsNil() )
    {
      if ( dimstyle->Index() < 0 || dimstyle->IsSystemComponent() )
        source_dimstyle = dimstyle;
      else
        return dimstyle->Index();
    }
    else
    {
      source_dimstyle = &MYON_DimStyle::SystemDimstyleFromId(dimstyle_id);
      if (dimstyle_id != source_dimstyle->Id())
        source_dimstyle = nullptr;
    }
  }

  if (nullptr == source_dimstyle)
  {
    bool bIsMetricLengthUnit = false;
    bool bIsUnitedStatesLengthUnit = false;
    for (int pass = 0; pass < 3; pass++)
    {
      if (1 == pass)
        length_unit_system = m_settings.m_ModelUnitsAndTolerances.m_unit_system.UnitSystem();
      else if (pass > 1)
        length_unit_system = MYON_3dmSettings::Default.m_ModelUnitsAndTolerances.m_unit_system.UnitSystem();
      bIsMetricLengthUnit = MYON::IsMetricLengthUnit(length_unit_system);
      bIsUnitedStatesLengthUnit = bIsMetricLengthUnit ? false : MYON::IsUnitedStatesCustomaryLengthUnit(length_unit_system);
      if (bIsMetricLengthUnit || bIsUnitedStatesLengthUnit)
        break;
    }
    for (int pass = 0; pass < 2; pass++)
    {
      if ( model_tolerance > 0.0 )
        break;
      if (1 == pass)
        model_tolerance = m_settings.m_ModelUnitsAndTolerances.m_absolute_tolerance;
      else if (pass > 1)
        model_tolerance = MYON_3dmSettings::Default.m_ModelUnitsAndTolerances.m_absolute_tolerance;
    }

    const MYON_DimStyle* system_dimstyle = nullptr;
    if (bIsMetricLengthUnit)
    {
      double meters_per_unit = MYON::UnitScale(length_unit_system, MYON::LengthUnitSystem::Meters);
      double tolerance_mm
        = model_tolerance > 0.0
        ? model_tolerance*MYON::UnitScale(length_unit_system, MYON::LengthUnitSystem::Millimeters)
        : 0.0;
      system_dimstyle =
        (tolerance_mm <= 1.0 || meters_per_unit < 1.0)
        ? &MYON_DimStyle::DefaultMillimeterSmall
        : &MYON_DimStyle::DefaultMillimeterLarge;
    }
    else if (bIsUnitedStatesLengthUnit)
    {
      system_dimstyle =
        (MYON::LengthUnitSystem::Feet == length_unit_system)
        ? &MYON_DimStyle::DefaultFootInchArchitecture
        : &MYON_DimStyle::DefaultInchDecimal;
    }
    else
    {
      system_dimstyle = &MYON_DimStyle::Default;
    }

    source_dimstyle = system_dimstyle;
  }

  MYON_DimStyle* default_dimstyle = new MYON_DimStyle(*source_dimstyle);
  default_dimstyle->ClearIndex();
  default_dimstyle->ClearParentId();
  default_dimstyle->SetId();
  default_dimstyle->SetName(m_manifest.UnusedName(*default_dimstyle));

  MYON_ModelComponentReference mcr = AddManagedModelComponent(default_dimstyle, true);
  const MYON_DimStyle* model_dimstyle = MYON_DimStyle::FromModelComponentRef(mcr,nullptr);
  if (nullptr == model_dimstyle)
  {
    MYON_ERROR("Failed to add default dimstyle.");
    return MYON_UNSET_INT_INDEX;
  }
  m_settings.SetCurrentDimensionStyleId(model_dimstyle->Id());
  return model_dimstyle->Index();
}

MYON_ModelComponentReference MYONX_Model::AddManagedModelComponent(
  class MYON_ModelComponent* managed_model_component,
  bool bResolveIdAndNameConflicts
)
{
  const bool bManagedComponent = true;
  const bool bUpdateComponentIdentification = true;
  return AddModelComponentForExperts(managed_model_component, bManagedComponent, bResolveIdAndNameConflicts, bUpdateComponentIdentification);
}


MYON_ModelComponentReference MYONX_Model::AddModelComponent(
  const class MYON_ModelComponent& model_component
)
{
  return AddModelComponent(model_component, true);
}

MYON_ModelComponentReference MYONX_Model::AddModelComponent(
  const class MYON_ModelComponent& model_component,
  bool bResolveIdAndNameConflicts
  )
{
  const MYON_ModelComponent::Type component_type = model_component.ComponentType();
  if (!MYON_ModelComponent::ComponentTypeIsValidAndNotMixed(component_type))
  {
    MYON_ERROR("Invalid model_component parameter.");
    return MYON_ModelComponentReference::Empty;
  }

  MYON_UUID id;
  MYON_wString name;
  if (false == ValdateComponentIdAndName(component_type, model_component.Id(), model_component.ParentId(), model_component.Name(), bResolveIdAndNameConflicts, bResolveIdAndNameConflicts, id, name))
  {
    MYON_ERROR("Invalid model_component id or name.");
    return MYON_ModelComponentReference::Empty;
  }
  
  MYON_ModelComponent* candidate_model_component = nullptr;
  if (MYON_ModelComponent::Type::RenderLight   == component_type ||
      MYON_ModelComponent::Type::ModelGeometry == component_type)
  {
    const MYON_ModelGeometryComponent* model_geometry = MYON_ModelGeometryComponent::Cast(&model_component);   
    if (nullptr != model_geometry)
    {
      const MYON_Geometry* geometry = model_geometry->Geometry(nullptr);
      candidate_model_component = MYON_ModelGeometryComponent::Create(*geometry, model_geometry->Attributes(nullptr), nullptr);
    }
  }
  else
  {
    // Something simple like MYON_Layer, MYON_DimStyle, MYON_RenderContent, etc
    candidate_model_component = model_component.Duplicate();
  }

  bool bManagedComponent = true;
  bool bUpdateComponentIdentification = true;
  return Internal_AddModelComponent(candidate_model_component, id, model_component.ParentId(), name, bManagedComponent, bUpdateComponentIdentification);
}

MYON_ModelComponentReference MYONX_Model::AddModelComponentForExperts(
  MYON_ModelComponent* model_component,
  bool bManagedComponent,
  bool bResolveIdAndNameConflicts,
  bool bUpdateComponentIdentification
  )
{
  if (nullptr == model_component)
  {
    MYON_ERROR("model_component parameter is nullptr.");
    return MYON_ModelComponentReference::Empty;
  }

  const MYON_ModelComponent::Type component_type = model_component->ComponentType();
  if (!MYON_ModelComponent::ComponentTypeIsValidAndNotMixed(component_type))
  {
    MYON_ERROR("Invalid model_component->ComponentType() value.");
    return MYON_ModelComponentReference::Empty;
  }

  const MYON_UUID parent_id = model_component->ParentId();
  MYON_UUID id = MYON_nil_uuid;
  MYON_wString name;
  if (false == ValdateComponentIdAndName(component_type, model_component->Id(), parent_id, model_component->Name(), bResolveIdAndNameConflicts, bResolveIdAndNameConflicts && bUpdateComponentIdentification, id, name))
  {
    MYON_ERROR("Invalid model_component id or name.");
    return MYON_ModelComponentReference::Empty;
  }

  return Internal_AddModelComponent(model_component, id, parent_id, name, bManagedComponent, bUpdateComponentIdentification);
}

MYON_ModelComponentReference MYONX_Model::AddModelGeometryComponentForExperts(
  bool bManageGeometry,
  class MYON_Object* geometry_object,
  bool bManageAttributes,
  class MYON_3dmObjectAttributes* attributes,
  bool bResolveIdAndNameConflicts
  )
{
  if ( nullptr == MYON_Geometry::Cast(geometry_object) )
  {
    MYON_ERROR("Invalid geometry_object parameter.");
    return MYON_ModelComponentReference::Empty;
  }

  if (nullptr == attributes)
  {
    bManageAttributes = true;
    bResolveIdAndNameConflicts = true;
    MYON_Light* light = MYON_Light::Cast(geometry_object);
    if (nullptr != light
      && MYON_nil_uuid != light->m_light_id 
      && false == m_manifest.IdIsAvailable(light->m_light_id)
      )
    {
      light->m_light_id = MYON_nil_uuid;
    }
    const MYON_Layer* default_layer
      = (MYON_nil_uuid == this->m_settings.CurrentLayerId())
      ? nullptr
      : MYON_Layer::FromModelComponentRef(LayerFromId(m_settings.CurrentLayerId()),nullptr);
    if (nullptr == default_layer)
    {
      MYONX_ModelComponentIterator layer_it(*this, MYON_ModelComponent::Type::Layer);
      for (const MYON_Layer* layer = MYON_Layer::Cast(layer_it.FirstComponent()); nullptr != layer; layer = MYON_Layer::Cast(layer_it.NextComponent()) )
      {
        if (layer->IsVisible() && false == layer->IsLocked() && layer->ParentIdIsNil())
        {
          default_layer = layer;
          break;
        }
      }
    }

    attributes = new MYON_3dmObjectAttributes();
    if (nullptr != light)
    {
      attributes->m_uuid = light->m_light_id;
      attributes->m_name = light->m_light_name;
    }
    attributes->m_layer_index
      = (nullptr == default_layer)
      ? MYON_Layer::Default.Index()
      : default_layer->Index();
  }
  else if ( MYON_nil_uuid != attributes->m_uuid && false == m_manifest.IdIsAvailable(attributes->m_uuid))
  {
    if (bResolveIdAndNameConflicts)
      attributes->m_uuid = MYON_nil_uuid;
    else
    {
      MYON_ERROR("attributes->m_uuid is not valid or is in use.");
      return MYON_ModelComponentReference::Empty;
    }
  }

  MYON_ModelGeometryComponent* model_geometry_component = MYON_ModelGeometryComponent::CreateForExperts(
    bManageGeometry,
    geometry_object,
    bManageAttributes,
    attributes,
    nullptr
  );

  if ( nullptr == model_geometry_component )
    return MYON_ModelComponentReference::Empty;

  MYON_ModelComponentReference model_component_reference = AddManagedModelComponent(model_geometry_component, bResolveIdAndNameConflicts);

  if (model_component_reference.IsEmpty())
    delete model_geometry_component;

  return model_component_reference;
}


MYON_ModelComponentReference MYONX_Model::AddModelGeometryComponent(
  const class MYON_Object* geometry_object,
  const class MYON_3dmObjectAttributes* attributes
)
{
  return AddModelGeometryComponent(geometry_object, attributes, true);
}

MYON_ModelComponentReference MYONX_Model::AddModelGeometryComponent(
  const class MYON_Object* geometry_object,
  const class MYON_3dmObjectAttributes* attributes,
  bool bResolveIdAndNameConflicts
  )
{
  if ( nullptr == MYON_Geometry::Cast(geometry_object) )
  {
    MYON_ERROR("Invalid geometry_object parameter.");
    return MYON_ModelComponentReference::Empty;
  }

  MYON_UUID id = MYON_nil_uuid;
  if (nullptr != attributes && MYON_nil_uuid != attributes->m_uuid )
  {
    if (m_manifest.IdIsAvailable(attributes->m_uuid))
      id = attributes->m_uuid;
    else if (false == bResolveIdAndNameConflicts)
    {
      MYON_ERROR("attributes->m_uuid is invalid or in use in this model.");
      return MYON_ModelComponentReference::Empty;
    }
  }

  MYON_3dmObjectAttributes* managed_attributes = nullptr;
  MYON_Object* managed_geometry_object = geometry_object->Duplicate();
  if (nullptr != attributes)
  {
    managed_attributes = new MYON_3dmObjectAttributes(*attributes);
    managed_attributes->m_uuid = id;
  }

  const bool bManageGeometry = true;
  const bool bManageAttributes = true;
  return AddModelGeometryComponentForExperts(
    bManageGeometry,
    managed_geometry_object,
    bManageAttributes,
    managed_attributes,
    bResolveIdAndNameConflicts
  );
}


MYON_ModelComponentReference MYONX_Model::AddManagedModelGeometryComponent(
  class MYON_Object* managed_geometry_object,
  class MYON_3dmObjectAttributes* managed_attributes
)
{
  return AddManagedModelGeometryComponent(managed_geometry_object, managed_attributes, true);
}

MYON_ModelComponentReference MYONX_Model::AddManagedModelGeometryComponent(
  class MYON_Object* managed_geometry_object,
  class MYON_3dmObjectAttributes* managed_attributes,
  bool bResolveIdAndNameConflicts
)
{
  if ( nullptr == MYON_Geometry::Cast(managed_geometry_object) )
  {
    MYON_ERROR("Invalid managed_geometry_object parameter.");
    return MYON_ModelComponentReference::Empty;
  }
  const bool bManageGeometry = true;
  const bool bManageAttributes = true;
  return AddModelGeometryComponentForExperts(
    bManageGeometry,
    managed_geometry_object,
    bManageAttributes,
    managed_attributes,
    bResolveIdAndNameConflicts
  );
}

MYONX_ModelComponentReferenceLink* MYONX_Model::Internal_ModelComponentLinkFromSerialNumber(
  MYON__UINT64 model_component_runtime_serial_number
  ) const
{
  const struct MYON_SerialNumberMap::SN_ELEMENT* e = m_mcr_sn_map.FindSerialNumber(model_component_runtime_serial_number);
  return (MYONX_ModelComponentReferenceLink*)((nullptr != e) ? e->m_value.m_u.ptr : nullptr);
}

MYONX_ModelComponentReferenceLink* MYONX_Model::Internal_AddModelComponentReference(
  MYON_ModelComponentReference mcr
  )
{
  const MYON_ModelComponent* model_component = mcr.ModelComponent();
  if (nullptr == model_component)
  {
    MYON_ERROR("Invalid mcr parameter - mcr.ModelComponent() is nullptr.");
    return nullptr;
  }

  const MYON_ModelComponent::Type component_type = model_component->ComponentType();
  if (MYON_ModelComponent::Type::Unset == component_type || MYON_ModelComponent::Type::Mixed == component_type)
  {
    MYON_ERROR("Invalid component type");
    return nullptr;
  }
  
  MYONX_ModelComponentReferenceLink* mcr_link = Internal_ModelComponentLinkFromSerialNumber(model_component->RuntimeSerialNumber());
  if (nullptr != mcr_link)
  {
    // This component was already added.
    return mcr_link;
  }

  struct MYON_SerialNumberMap::SN_ELEMENT* e = m_mcr_sn_map.AddSerialNumber(model_component->RuntimeSerialNumber());
  if (nullptr == e)
  {
    MYON_ERROR("m_mcr_sn_map.AddSerialNumber(model_component->RuntimeSerialNumber()) failed.");
    return nullptr;
  }

  if ( 0 == m_mcr_link_fsp.SizeofElement())
    m_mcr_link_fsp.Create(sizeof(*mcr_link),0,0);

  e->m_value.m_u.ptr = m_mcr_link_fsp.AllocateDirtyElement();
    
  mcr_link = new(e->m_value.m_u.ptr) MYONX_ModelComponentReferenceLink();
  mcr_link->m_mcr = mcr;

  MYONX_Model::MYONX_ModelComponentList& list = Internal_ComponentList( component_type );
  if (component_type != list.m_component_type)
  {
    MYON_ERROR("Internal_ComponentList(component_type) failed");
  }
  else
  {
    if (nullptr == list.m_first_mcr_link)
    {
      list.m_first_mcr_link = mcr_link;
      mcr_link->m_prev = nullptr;
    }
    else
    {
      mcr_link->m_prev = list.m_last_mcr_link;
      list.m_last_mcr_link->m_next = mcr_link;
    }
    mcr_link->m_next = nullptr;
    list.m_last_mcr_link = mcr_link;

    list.m_count++;
  }

  return mcr_link;
}


void MYONX_Model::Internal_RemoveModelComponentReferenceLink(
class MYONX_ModelComponentReferenceLink* mcr_link
  )
{
  if (nullptr == mcr_link)
    return;

  const MYON_ModelComponent* model_component = mcr_link->m_mcr.ModelComponent();

  if ( nullptr == model_component )
    return;

  m_mcr_sn_map.RemoveSerialNumberAndId(model_component->ReferenceModelSerialNumber());

  mcr_link->m_mcr = MYON_ModelComponentReference::Empty;

  MYONX_Model::MYONX_ModelComponentList& list = Internal_ComponentList(model_component->ComponentType());
  if (list.m_count > 0)
  {
    if (mcr_link->m_prev)
      mcr_link->m_prev->m_next = mcr_link->m_next;
    else
      list.m_first_mcr_link = mcr_link->m_next;
    if (mcr_link->m_next)
      mcr_link->m_next->m_prev = mcr_link->m_prev;
    else
      list.m_last_mcr_link = mcr_link->m_prev;
    list.m_count--;
  }

  mcr_link->m_prev = nullptr;
  mcr_link->m_next = nullptr;

  m_mcr_link_fsp.ReturnElement(mcr_link);
}

MYONX_Model::MYONX_ModelComponentList& MYONX_Model::Internal_ComponentList(
  MYON_ModelComponent::Type component_type
  )
{
  const int i = static_cast<unsigned int>(component_type);
  return
    (i < MYONX_Model::m_private->m_mcr_lists.Count())
    ? m_private->m_mcr_lists[i]
    : m_private->m_mcr_lists[0];
}

const MYONX_Model::MYONX_ModelComponentList& MYONX_Model::Internal_ComponentListConst(
  MYON_ModelComponent::Type component_type
  ) const
{
  const int i = static_cast<unsigned int>(component_type);
  return
    (i < MYONX_Model::m_private->m_mcr_lists.Count())
    ? m_private->m_mcr_lists[i]
    : m_private->m_mcr_lists[0];
}

MYON_ModelComponentReference MYONX_Model::Internal_AddModelComponent(
  MYON_ModelComponent* model_component,
  MYON_UUID id,
  MYON_UUID name_parent_id,
  const MYON_wString& name,
  bool bManagedComponent,
  bool bUpdateComponentIdentification
  )
{
  for (;;)
  {
    if (nullptr == model_component)
    {
      MYON_ERROR("Invalid model_component parameter.");
      break;
    }

    if (model_component->IsSystemComponent())
    {
      MYON_ERROR("Invalid model_component parameter.");
      break;
    }
    
    const MYON_ModelComponent::Type component_type = model_component->ComponentType();
    if (!MYON_ModelComponent::ComponentTypeIsValidAndNotMixed(component_type))
    {
      MYON_ERROR("Invalid model_component parameter.");
      break;
    }

    const int original_index = model_component->Index();
    const MYON_UUID original_id = model_component->Id();
    const bool bIsEmbeddedFileReference = (MYON_ModelComponent::Type::Image == component_type);
    const MYON_Bitmap* embedded_file
      = bIsEmbeddedFileReference
      ? MYON_Bitmap::Cast(model_component)
      : nullptr;

    const MYON_NameHash original_name_hash 
      = bIsEmbeddedFileReference
      ? (nullptr == embedded_file ? MYON_NameHash::UnsetNameHash : MYON_NameHash::CreateFilePathHash(embedded_file->FileReference()) )
      : model_component->NameHash();

    const MYON_NameHash name_hash 
      = bIsEmbeddedFileReference
      ? original_name_hash
      : MYON_NameHash::Create(name_parent_id,name,MYON_ModelComponent::UniqueNameIgnoresCase(component_type));

    int manifest_item_index = MYON_UNSET_INT_INDEX;
    const bool bIndexRequired = MYON_ModelComponent::IndexRequired(component_type);
    if (bIndexRequired)
    {
      manifest_item_index = m_manifest.ComponentIndexLimit(component_type);
      if (manifest_item_index >= 0x7FFFFFFF && bUpdateComponentIdentification)
      {
        MYON_ERROR("Unable to set model_component_index.");
        break;
      }
    }
    const bool bUpdateIndex = (bUpdateComponentIdentification && bIndexRequired && original_index != (int)manifest_item_index);
    const bool bUpdateId = (bUpdateComponentIdentification && !(original_id == id));
    const bool bUpdateName = (false == bIsEmbeddedFileReference && bUpdateComponentIdentification && original_name_hash != name_hash);

    if (bUpdateIndex && model_component->IndexIsLocked())
    {
      MYON_ERROR("Unable to set component index.");
      break;
    }
    if (bUpdateId && model_component->IdIsLocked())
    {
      MYON_ERROR("Unable to set component id.");
      break;
    }
    if (bUpdateName && model_component->NameIsLocked())
    {
      MYON_ERROR("Unable to set component name.");
      break;
    }

    if (bUpdateIndex && false == model_component->SetIndex((int)manifest_item_index))
    {
      MYON_ERROR("model_component->SetIndex(...) failed.");
      break;
    }
    if(bUpdateId && false == model_component->SetId(id) )
    {
      MYON_ERROR("model_component->SetId(...) failed.");
      break;
    }
    if(bUpdateName && false == model_component->SetName(name) )
    {
      MYON_ERROR("model_component->SetName(...) failed.");
      break;
    }

    if (bUpdateComponentIdentification)
    {
      MYON_ModelGeometryComponent* geometry_component = MYON_ModelGeometryComponent::Cast(model_component);
      if (nullptr != geometry_component)
      {
        const MYON_Light* light = MYON_Light::Cast(geometry_component->Geometry(nullptr));
        if (nullptr != light)
        {
          if (id != light->m_light_id)
            const_cast<MYON_Light*>(light)->m_light_id = id;
          if (name != light->m_light_name)
            const_cast<MYON_Light*>(light)->m_light_name = name;
        }
        const MYON_3dmObjectAttributes* attributes = geometry_component->Attributes(nullptr);
        if (nullptr != attributes)
        {
          if (id != attributes->m_uuid)
            const_cast<MYON_3dmObjectAttributes*>(attributes)->m_uuid = id;
          if (name != attributes->m_name)
            const_cast<MYON_3dmObjectAttributes*>(attributes)->m_name = name;
        }
      }
    }

    Internal_IncrementModelContentVersionNumber();

    const class MYON_ComponentManifestItem& manifest_item = m_manifest.AddComponentToManifest(
      model_component->ComponentType(),
      model_component->RuntimeSerialNumber(),
      id,
      name_hash
      );
    
    if (manifest_item.IsUnset())
    {
      MYON_ERROR("Unable to update model manifest.");
      break;
    }

    MYON_ManifestMapItem original_to_manifest;
    if ( original_to_manifest.SetSourceIdentification(model_component) && original_to_manifest.SetDestinationIdentification(&manifest_item))
    {
      m_original_to_manifest_map.AddMapItem(original_to_manifest);
    }

    MYON_ManifestMapItem manifest_to_original;
    if ( manifest_to_original.SetSourceIdentification(&manifest_item) && manifest_to_original.SetDestinationIdentification(model_component) )
    {
      m_manifest_to_original_map.AddMapItem(manifest_to_original);
    }

    if (bIndexRequired)
    {
      if (model_component->Index() != manifest_item.Index())
      {
        MYON_ERROR("Unexpected manifest_item_index value.");
        if (bUpdateIndex && false == model_component->SetIndex((int)(manifest_item_index)))
        {
          MYON_ERROR("model_component->SetIndex(...) failed.");
          break;
        }
      }
    }
    else
    {
      if (MYON_ComponentManifest::UnsetComponentIndex != model_component->Index() )
      {
        MYON_ERROR("Unexpected model_component->Index() value.");
        if (bUpdateIndex && false == model_component->SetIndex(MYON_ComponentManifest::UnsetComponentIndex))
        {
          MYON_ERROR("model_component->SetIndex(...) failed.");
          break;
        }
      }
      if ( MYON_UNSET_INT_INDEX != manifest_item.Index() )
      {
        MYON_ERROR("Unexpected manifest_item_index value.");
      }
    }

    if (MYON_ModelComponent::Type::ModelGeometry == model_component->ComponentType())
    {
      MYON_ModelGeometryComponent* model_geometry = MYON_ModelGeometryComponent::Cast(model_component);
      if (nullptr != model_geometry)
      {
        // TEMPORARY - until the id and name can be removed from MYON_3dmObjectAttributes,
        // they need to be synchronized with the ones on MYON_ModelGeometryComponent.
        MYON_3dmObjectAttributes* attributes = const_cast< MYON_3dmObjectAttributes* >(model_geometry->Attributes(nullptr));
        if (nullptr != attributes)
        {
          if ( bUpdateId )
            attributes->m_uuid = model_geometry->Id();
          if (bUpdateName)
            attributes->m_name = model_geometry->Name();
        }
      }
    }
    
    if (bUpdateComponentIdentification)
    {
      // continue even when UpdateReferencedComponents() returns false
      model_component->UpdateReferencedComponents(MYON_ComponentManifest::Empty,m_manifest,m_original_to_manifest_map);
    }

    MYON_ModelComponentReference model_component_reference = MYON_ModelComponentReference::CreateForExperts(model_component,bManagedComponent);
    Internal_AddModelComponentReference(model_component_reference);
    return model_component_reference;
  }

  return MYON_ModelComponentReference::Empty;
}

/* MYON_DEPRECATED */ MYON_ModelComponentReference MYONX_Model::RenderMaterialFromAttributes(const MYON_3dmObjectAttributes& attr) const
{
  return MaterialFromAttributes(attr);
}

/* MYON_DEPRECATED */ MYON_ModelComponentReference MYONX_Model::RenderMaterialFromLayerIndex(int index) const
{
  return MaterialFromLayerIndex(index);
}

/* MYON_DEPRECATED */ MYON_ModelComponentReference MYONX_Model::RenderMaterialFromIndex(int index) const
{
  return MaterialFromIndex(index);
}

/* MYON_DEPRECATED */ MYON_ModelComponentReference MYONX_Model::RenderMaterialFromId(MYON_UUID id) const
{
  return MaterialFromId(id);
}

MYON_ModelComponentReference MYONX_Model::MaterialFromAttributes(
  const MYON_3dmObjectAttributes& attributes
  ) const
{
  switch ( attributes.MaterialSource() )
  {
  case MYON::material_from_layer:
    return MaterialFromLayerIndex( attributes.m_layer_index );
    break;

  case MYON::material_from_object:
    return MaterialFromIndex( attributes.m_material_index );
    break;

  case MYON::material_from_parent:
    // TODO: If object is an idef, get material from iref attributes.
    return MaterialFromIndex( attributes.m_material_index );
    break;
  }

  return m_default_render_material;
}

MYON_ModelComponentReference MYONX_Model::MaterialFromLayerIndex(
  int layer_index
  ) const
{
  int material_index = MYON_Layer::FromModelComponentRef(
    LayerFromIndex(layer_index),
    &MYON_Layer::Default
    )->RenderMaterialIndex();
  return MaterialFromIndex(material_index);
}

MYON_ModelComponentReference MYONX_Model::MaterialFromIndex(
  int material_index
  ) const
{
  MYON_ModelComponentReference cr = ComponentFromIndex(MYON_ModelComponent::Type::Material, material_index);
  return cr.IsEmpty() ? m_default_render_material : cr;
}

MYON_ModelComponentReference MYONX_Model::MaterialFromId(
  MYON_UUID material_id
  ) const
{
  MYON_ModelComponentReference cr = ComponentFromId(MYON_ModelComponent::Type::Material, material_id);
  return cr.IsEmpty() ? m_default_render_material : cr;
}

MYON_Color MYONX_Model::WireframeColorFromAttributes(
  const MYON_3dmObjectAttributes& attributes
  ) const
{
  MYON_Color color = MYON_UNSET_COLOR;

  switch ( attributes.ColorSource() )
  {
  case MYON::color_from_layer:
    color = MYON_Layer::FromModelComponentRef( LayerFromIndex(attributes.m_layer_index), &MYON_Layer::Default )->Color();
    break;

  case MYON::color_from_object:
    color = attributes.m_color;
    break;

  case MYON::color_from_material:
    color = MYON_Material::FromModelComponentRef( MaterialFromAttributes(attributes), &MYON_Material::Default)->Diffuse();
    break;

  case MYON::color_from_parent:
    color = attributes.m_color;
    // TODO: if object is an instance definition, get color
    //       from instance references.
    break;
  }

  if ( color == MYON_UNSET_COLOR )
    color.SetRGB(128,128,128);

  return color;
}

void MYONX_DumpView( MYON_TextLog& dump, const MYON_3dmView& view )
{
  view.Dump(dump);
}

void MYONX_Model::DumpSummary( MYON_TextLog& dump ) const
{
  dump.Print("File version: %u\n",m_3dm_file_version);
  if (false == dump.IsTextHash())
  {
    dump.Print("File openNURBS version: %u\n", m_3dm_opennurbs_version);
    if (m_3dm_file_byte_count > 0)
      dump.Print("File length: %llu bytes\n", m_3dm_file_byte_count);

    if (m_sStartSectionComments.Length() > 0)
    {
      dump.Print("Start section comments:\n");
      dump.PushIndent();
      dump.PrintWrappedText(static_cast<const char*>(m_sStartSectionComments));
      dump.PopIndent();
      dump.Print("\n");
    }
  
    m_properties.Dump(dump);

    dump.Print("\n");
  }

  m_settings.Dump(dump);

  dump.Print("\n");

  dump.Print("Contents:\n");
  dump.PushIndent();
  dump.Print("%u embedded images\n",Internal_ComponentListConst(MYON_ModelComponent::Type::Image).m_count);
  dump.Print("%u materials\n",Internal_ComponentListConst(MYON_ModelComponent::Type::RenderMaterial).m_count);
  dump.Print("%u line patterns\n",Internal_ComponentListConst(MYON_ModelComponent::Type::LinePattern).m_count);
  dump.Print("%u text styles\n",Internal_ComponentListConst(MYON_ModelComponent::Type::TextStyle).m_count);
  dump.Print("%u annotation styles\n",Internal_ComponentListConst(MYON_ModelComponent::Type::DimStyle).m_count);
  dump.Print("%u hatch patterns\n",Internal_ComponentListConst(MYON_ModelComponent::Type::HatchPattern).m_count);
  dump.Print("%u layers\n",Internal_ComponentListConst(MYON_ModelComponent::Type::Layer).m_count);
  dump.Print("%u groups\n",Internal_ComponentListConst(MYON_ModelComponent::Type::Group).m_count);
  dump.Print("%u lights\n",Internal_ComponentListConst(MYON_ModelComponent::Type::RenderLight).m_count);
  dump.Print("%u model geometry objects\n",Internal_ComponentListConst(MYON_ModelComponent::Type::ModelGeometry).m_count);
  if (false == dump.IsTextHash())
  {
    dump.Print("%u user data objects\n", m_userdata_table.UnsignedCount());
  }
  dump.PopIndent();
}

void MYONX_Model::DumpComponentList( 
  MYON_ModelComponent::Type component_type,
  MYON_TextLog& text_log
  ) const
{
  const MYON_wString type_name_string = MYON_ModelComponent::ComponentTypeToString(component_type);
  const wchar_t* type_name = static_cast<const wchar_t*>(type_name_string);
  unsigned int i = 0;
  for ( 
    const MYONX_ModelComponentReferenceLink* link = Internal_ComponentListConst(component_type).m_first_mcr_link;
    nullptr != link;
    link = link->m_next
    )
  {
    text_log.Print(L"%ls %d:\n",type_name,i);
    const MYON_ModelComponent* model_component = link->m_mcr.ModelComponent();
  text_log.PushIndent();
    if ( nullptr == model_component )
      text_log.Print(L"nullptr\n");
    else
      model_component->Dump(text_log);
    link->m_mcr.ModelComponent();
    text_log.PopIndent();
    i++;
  }
}

void MYONX_Model::DumpUserDataTable( MYON_TextLog& dump) const
{
  int i;
  for ( i = 0; i < m_userdata_table.Count(); i++ )
  {
    const MYONX_Model_UserData* ud = m_userdata_table[i];
    if ( nullptr == ud)
      continue;
    dump.Print("User Data Table %d:\n",i);
    dump.PushIndent();
    dump.Print("uuid = "); dump.Print(ud->m_uuid); dump.Print("\n");
    ud->m_goo.Dump(dump);    
    dump.PopIndent();
  }
}


void MYONX_Model::Dump(MYON_TextLog& dump) const
{
  dump.Print("Model summary:\n");
  dump.PushIndent();
  DumpSummary(dump);
  dump.PopIndent();

  dump.PrintNewLine();
  DumpComponentLists(dump);

  if ( false == dump.IsTextHash() )
  {
    dump.Print("User data table:\n");
    dump.PushIndent();
    DumpUserDataTable(dump);
    dump.PopIndent();
    dump.PrintNewLine();
  }
}

void MYONX_Model::DumpComponentLists( MYON_TextLog& dump ) const
{
  const MYON_ModelComponent::Type table_types[] =
  {
    MYON_ModelComponent::Type::Image,
    MYON_ModelComponent::Type::TextureMapping,
    MYON_ModelComponent::Type::Material,
    MYON_ModelComponent::Type::LinePattern,
    MYON_ModelComponent::Type::Layer,
    MYON_ModelComponent::Type::Group,
    MYON_ModelComponent::Type::TextStyle,
    MYON_ModelComponent::Type::DimStyle,
    MYON_ModelComponent::Type::RenderLight,
    MYON_ModelComponent::Type::HatchPattern,
    MYON_ModelComponent::Type::ModelGeometry,
    MYON_ModelComponent::Type::HistoryRecord,
    MYON_ModelComponent::Type::RenderContent,
    MYON_ModelComponent::Type::EmbeddedFile,
    MYON_ModelComponent::Type::PostEffect,
    MYON_ModelComponent::Type::Unset // list terminator
  };

  for (unsigned i = 0; MYON_ModelComponent::Type::Unset != table_types[i]; i++)
  {
    MYON_wString type_name = MYON_ModelComponent::ComponentTypeToString(table_types[i]);
    dump.Print(L"%ls table:\n",static_cast<const wchar_t*>(type_name));
    dump.PushIndent();
    DumpComponentList(table_types[i],dump);
    dump.PopIndent();
    dump.Print("\n");
  }
}

MYON_SHA1_Hash MYONX_Model::ContentHash(
) const
{
  const bool bRemapIds = true;
  MYON_TextHash hash_log;
  hash_log.SetIdRemap(bRemapIds);
  Dump(hash_log);
  return hash_log.Hash();
}

class MYON__CIndexPair
{
public:
  static int CompareOldIndex( const MYON__CIndexPair* a, const MYON__CIndexPair* b );
  static int CompareOldAndNewIndex( const MYON__CIndexPair* a, const MYON__CIndexPair* b );
  int m_old_index;  // value in model.m_..._table[m_table_index].m_..._index; (Read from file)
  int m_new_index;  // index in model.m_..._table[] array
};

int MYON__CIndexPair::CompareOldIndex( const MYON__CIndexPair* a, const MYON__CIndexPair* b )
{
  return (a->m_old_index - b->m_old_index);
}

int MYON__CIndexPair::CompareOldAndNewIndex( const MYON__CIndexPair* a, const MYON__CIndexPair* b )
{
  int i;
  if ( 0 == (i = a->m_old_index - b->m_old_index) )
    i = a->m_new_index - b->m_new_index;
  return i;
}

bool MYONX_Model::Read(const char* filename, MYON_TextLog* error_log)
{
  bool rc = false;

  if (nullptr != filename)
  {
    FILE* fp = MYON::OpenFile(filename, "rb");
    if ( 0 != fp )
    {
      MYON_BinaryFile file(MYON::archive_mode::read3dm,fp);
      rc = Read(file, error_log);
      MYON::CloseFile(fp);
    }
  }

  return rc;
}

bool MYONX_Model::Read(const wchar_t* filename, MYON_TextLog* error_log)
{
  bool rc = false;

  if (nullptr != filename)
  {
    FILE* fp = MYON::OpenFile(filename, L"rb");
    if ( 0 != fp )
    {
      MYON_BinaryFile file(MYON::archive_mode::read3dm,fp);
      rc = Read(file, error_log);
      MYON::CloseFile(fp);
    }
  }

  return rc;
}

bool MYONX_Model::IncrementalReadBegin( 
  MYON_BinaryArchive& archive,
  bool bManageComponents,
  unsigned int table_filter,
  MYON_TextLog* error_log
  )
{
  Reset();

  const bool bResolveIdAndNameConflicts = true;
  const bool bUpdateComponentIdentification = true;

  int rc;

  if ( 0 == table_filter )
    table_filter = 0xFFFFFFFF; // read everything

  // STEP 1: REQUIRED - Read start section
  if ( !archive.Read3dmStartSection( &m_3dm_file_version, m_sStartSectionComments ) )
  {
    return false;
  }

  // STEP 2: REQUIRED - Read properties section
  if ( !archive.Read3dmProperties( m_properties ) )
  {
    return false;
  }

  // version of opennurbs used to write the file.
  m_3dm_opennurbs_version = archive.ArchiveOpenNURBSVersion();

  // STEP 3: REQUIRED - Read setting section
  if ( !archive.Read3dmSettings( m_settings ) )
  {
    return false;
  }

  // STEP 4: REQUIRED - Read embedded bitmap table
  if ( archive.BeginRead3dmBitmapTable() )
  {
    if ( 0 != (static_cast<unsigned int>(MYON_3dmArchiveTableType::bitmap_table) & table_filter) )
    {
      for(;;)
      {
        MYON_Bitmap* bitmap = nullptr;
        rc = archive.Read3dmBitmap(&bitmap);
        if ( rc==0 )
          break; // end of bitmap table
        if (rc < 0)
          break;
        if ( AddModelComponentForExperts(bitmap,bManageComponents,bResolveIdAndNameConflicts,bUpdateComponentIdentification).IsEmpty() )
          delete bitmap;
      }
    }

    // If BeginRead3dmBitmapTable() returns true, 
    // then you MUST call EndRead3dmBitmapTable().
    if ( !archive.EndRead3dmBitmapTable() )
    {
      return false;
    }
  }

  // STEP 5: REQUIRED - Read texture mapping table
  if ( archive.BeginRead3dmTextureMappingTable() )
  {
    if ( 0 != (static_cast<unsigned int>(MYON_3dmArchiveTableType::texture_mapping_table) & table_filter) )
    {
      for(;;)
      {
        MYON_TextureMapping* texture_mapping = nullptr;
        rc = archive.Read3dmTextureMapping(&texture_mapping);
        if ( rc==0 )
          break; // end of texture_mapping table
        if ( rc < 0 ) 
          break;
        if (AddModelComponentForExperts(texture_mapping, bManageComponents,bResolveIdAndNameConflicts,bUpdateComponentIdentification).IsEmpty())
          delete texture_mapping;
      }
    }
    
    // If BeginRead3dmTextureMappingTable() returns true, 
    // then you MUST call EndRead3dmTextureMappingTable().
    if ( !archive.EndRead3dmTextureMappingTable() )
    {
      return false;
    }
  }


  // STEP 6: REQUIRED - Read render material table
  if ( archive.BeginRead3dmMaterialTable() )
  {
    const MYON_UUID settings_current_id = m_settings.CurrentMaterialId();
    const int settings_current_index = m_settings.CurrentMaterialIndex();
    bool bSetCurrentById = !(MYON_nil_uuid == settings_current_id);
    bool bSetCurrentByIndex = false == bSetCurrentById && settings_current_index >= 0;
    m_settings.SetCurrentMaterialId(MYON_nil_uuid);
    
    if ( 0 != (static_cast<unsigned int>(MYON_3dmArchiveTableType::material_table) & table_filter) )
    {
      for(;;)
      {
        MYON_Material* material = nullptr;
        rc = archive.Read3dmMaterial(&material);
        if ( rc==0 )
          break; // end of material table
        if (rc < 0)
          break;
        // index or id might be modified by AddModelComponentForExperts()
        const bool bSetAsCurrent =
          (bSetCurrentById && settings_current_id == material->Id())
          || (bSetCurrentByIndex && settings_current_index == material->Index());
        if ( AddModelComponentForExperts(material,bManageComponents,bResolveIdAndNameConflicts,bUpdateComponentIdentification).IsEmpty() )
        {
          delete material;
        }
        else if (bSetAsCurrent)
        {
          m_settings.SetCurrentMaterialId(material->Id());
          bSetCurrentById = false;
          bSetCurrentByIndex = false;
        }
      }
    }

    // If BeginRead3dmMaterialTable() returns true, 
    // then you MUST call EndRead3dmMaterialTable().
    if ( !archive.EndRead3dmMaterialTable() )
    {
      return false;
    }
  }


  // STEP 7: REQUIRED - Read line type table
  if ( archive.BeginRead3dmLinetypeTable() )
  {
    const MYON_UUID settings_current_id = m_settings.CurrentLinePatternId();
    const int settings_current_index = m_settings.CurrentLinePatternIndex();
    bool bSetCurrentById = !(MYON_nil_uuid == settings_current_id);
    bool bSetCurrentByIndex = false == bSetCurrentById && settings_current_index >= 0;
    m_settings.SetCurrentLinePatternId(MYON_nil_uuid);

    if ( 0 != (static_cast<unsigned int>(MYON_3dmArchiveTableType::linetype_table) & table_filter) )
    {
      for(;;)
      {
        MYON_Linetype* line_pattern = nullptr;
        rc = archive.Read3dmLinetype(&line_pattern);
        if ( rc==0 )
          break; // end of linetype table
        if (rc < 0)
          break;
        // index or id might be modified by AddModelComponentForExperts()
        const bool bSetAsCurrent =
          (bSetCurrentById && settings_current_id == line_pattern->Id())
          || (bSetCurrentByIndex && settings_current_index == line_pattern->Index());
        if ( AddModelComponentForExperts(line_pattern,bManageComponents,bResolveIdAndNameConflicts,bUpdateComponentIdentification).IsEmpty() )
        {
          delete line_pattern;
        }
        else if (bSetAsCurrent)
        {
          m_settings.SetCurrentLinePatternId(line_pattern->Id());
          bSetCurrentById = false;
          bSetCurrentByIndex = false;
        }
      }
    }

    // If BeginRead3dmLinetypeTable() returns true, 
    // then you MUST call EndRead3dmLinetypeTable().
    if ( !archive.EndRead3dmLinetypeTable() )
    {
      return false;
    }
  }

  // STEP 8: REQUIRED - Read layer table
  if ( archive.BeginRead3dmLayerTable() )
  {
    const MYON_UUID settings_current_id = m_settings.CurrentLayerId();
    const int settings_current_index = m_settings.CurrentLayerIndex();
    bool bSetCurrentById = !(MYON_nil_uuid == settings_current_id);
    bool bSetCurrentByIndex = false == bSetCurrentById && settings_current_index >= 0;
    m_settings.SetCurrentLayerId(MYON_nil_uuid);

    if ( 0 != (static_cast<unsigned int>(MYON_3dmArchiveTableType::layer_table) & table_filter) )
    {
      for(;;)
      {
        MYON_Layer* layer = nullptr;
        rc = archive.Read3dmLayer(&layer);
        if ( rc==0 )
          break; // end of layer table
        if (rc < 0)
          break;
        // index or id might be modified by AddModelComponentForExperts()
        const bool bSetAsCurrent =
          (bSetCurrentById && settings_current_id == layer->Id())
          || (bSetCurrentByIndex && settings_current_index == layer->Index());
        if (AddModelComponentForExperts(layer, bManageComponents,bResolveIdAndNameConflicts,bUpdateComponentIdentification).IsEmpty())
        {
          delete layer;
        }
        else if (bSetAsCurrent)
        {
          m_settings.SetCurrentLayerId(layer->Id());
          bSetCurrentById = false;
          bSetCurrentByIndex = false;
        }
      }
    }

    // If BeginRead3dmLayerTable() returns true, 
    // then you MUST call EndRead3dmLayerTable().
    if ( !archive.EndRead3dmLayerTable() )
    {
      return false;
    }

    MYON_UUID current_layer_id = m_settings.CurrentLayerId();
    const MYON_Layer* current_layer 
      = (MYON_nil_uuid == current_layer_id)
      ? nullptr
      : MYON_Layer::FromModelComponentRef(LayerFromId(current_layer_id), nullptr);
    if (nullptr == current_layer)
    {
      current_layer_id = MYON_nil_uuid;
      int layer_count = 0;
      MYONX_ModelComponentIterator layer_it(*this, MYON_ModelComponent::Type::Layer);
      for (const MYON_Layer* layer = MYON_Layer::Cast(layer_it.FirstComponent()); nullptr != layer; layer = MYON_Layer::Cast(layer_it.NextComponent()))
      {
        layer_count++;
        if (layer->IsVisible() && false == layer->IsLocked())
        {
          current_layer = layer;
          if ( layer->ParentIdIsNil() )
            break;
        }
      }
      if (nullptr == current_layer && layer_count > 0)
      {
        current_layer = MYON_Layer::Cast(layer_it.FirstComponent());
      }
      if (nullptr != current_layer)
        m_settings.SetCurrentLayerId(current_layer->Id());
      else
        AddDefaultLayer(nullptr, MYON_Color::UnsetColor);
    }
  }

  // STEP 9: REQUIRED - Read group table
  if ( archive.BeginRead3dmGroupTable() )
  {
    if ( 0 != (static_cast<unsigned int>(MYON_3dmArchiveTableType::group_table) & table_filter) )
    {
      for(;;)
      {
        MYON_Group* group = nullptr;
        rc = archive.Read3dmGroup(&group);
        if ( rc==0 )
          break; // end of group table
        if (rc < 0)
          break;
        if (AddModelComponentForExperts(group, bManageComponents,bResolveIdAndNameConflicts,bUpdateComponentIdentification).IsEmpty())
          delete group;
      }
    }
    
    // If BeginRead3dmGroupTable() returns true, 
    // then you MUST call EndRead3dmGroupTable().
    if ( !archive.EndRead3dmGroupTable() )
    {
      return false;
    }
  }

  // STEP 11: REQUIRED - Read dimstyle table
  if ( archive.BeginRead3dmDimStyleTable() )
  {
    const MYON_UUID settings_current_id = m_settings.CurrentDimensionStyleId();
    const int settings_current_index = m_settings.CurrentDimensionStyleIndex();
    bool bSetCurrentById = !(MYON_nil_uuid == settings_current_id);
    bool bSetCurrentByIndex = false == bSetCurrentById && settings_current_index >= 0;
    m_settings.SetCurrentDimensionStyleId(MYON_nil_uuid);

    if ( 0 != (static_cast<unsigned int>(MYON_3dmArchiveTableType::dimension_style_table) & table_filter) )
    {
      for(;;)
      {
        MYON_DimStyle* dimension_style = nullptr;
        rc = archive.Read3dmDimStyle(&dimension_style);
        if ( rc==0 )
          break; // end of dimstyle table
        if ( rc < 0 ) 
          break;
        // index or id might be modified by AddModelComponentForExperts()
        const bool bSetAsCurrent =
          (bSetCurrentById && settings_current_id == dimension_style->Id())
          || (bSetCurrentByIndex && settings_current_index == dimension_style->Index());
        if (AddModelComponentForExperts(dimension_style, bManageComponents,bResolveIdAndNameConflicts,bUpdateComponentIdentification).IsEmpty())
        {
          delete dimension_style;
        }
        else if (bSetAsCurrent)
        {
          m_settings.SetCurrentDimensionStyleId(dimension_style->Id());
          bSetCurrentById = false;
          bSetCurrentByIndex = false;
        }
      }
    }
    
    // If BeginRead3dmDimStyleTable() returns true, 
    // then you MUST call EndRead3dmDimStyleTable().
    if ( !archive.EndRead3dmDimStyleTable() )
    {
      return false;
    }
  }

  // STEP 12: REQUIRED - Read render lights table
  if ( archive.BeginRead3dmLightTable() )
  {
    if ( 0 != (static_cast<unsigned int>(MYON_3dmArchiveTableType::light_table) & table_filter) )
    {
      for(;;)
      {
        MYON_ModelGeometryComponent* model_light = nullptr;
        rc = archive.Read3dmModelLight(&model_light);
        if (rc == 0)
          break; // end of light table
        if ( rc < 0 ) 
          break;
        if (AddModelComponentForExperts(model_light, bManageComponents,bResolveIdAndNameConflicts,bUpdateComponentIdentification).IsEmpty())
          delete model_light;
      }
    }

    // If BeginRead3dmLightTable() returns true, 
    // then you MUST call EndRead3dmLightTable().
    if ( !archive.EndRead3dmLightTable() )
    {
      return false;
    }
  }

  // STEP 13 - read hatch pattern table
  if ( archive.BeginRead3dmHatchPatternTable() )
  {
    const MYON_UUID settings_current_id = m_settings.CurrentHatchPatternId();
    const int settings_current_index = MYON_UNSET_INT_INDEX;
    bool bSetCurrentById = !(MYON_nil_uuid == settings_current_id);
    bool bSetCurrentByIndex = false;
    m_settings.SetCurrentHatchPatternId(MYON_nil_uuid);

    if ( 0 != (static_cast<unsigned int>(MYON_3dmArchiveTableType::hatchpattern_table) & table_filter) )
    {
      for(;;)
      {
        MYON_HatchPattern* hatch_pattern = nullptr;
        rc = archive.Read3dmHatchPattern(&hatch_pattern);
        if ( rc==0 )
          break; // end of hatchpattern table
        if ( rc < 0 ) 
          break;
        // index or id might be modified by AddModelComponentForExperts()
        const bool bSetAsCurrent =
          (bSetCurrentById && settings_current_id == hatch_pattern->Id())
          || (bSetCurrentByIndex && settings_current_index == hatch_pattern->Index());
        if (AddModelComponentForExperts(hatch_pattern, bManageComponents,bResolveIdAndNameConflicts,bUpdateComponentIdentification).IsEmpty())
        {
          delete hatch_pattern;
        }
        else if (bSetAsCurrent)
        {
          m_settings.SetCurrentHatchPatternId(hatch_pattern->Id());
          bSetCurrentById = false;
          bSetCurrentByIndex = false;
        }
      }
    }
    
    // If BeginRead3dmHatchPatternTable() returns true, 
    // then you MUST call EndRead3dmHatchPatternTable().
    if ( !archive.EndRead3dmHatchPatternTable() )
    {
      return false;
    }
  }

  // STEP 14: REQUIRED - Read instance definition table
  if ( archive.BeginRead3dmInstanceDefinitionTable() )
  {
    if ( 0 != (static_cast<unsigned int>(MYON_3dmArchiveTableType::instance_definition_table) & table_filter) )
    {
      for(;;)
      {
        MYON_InstanceDefinition* instance_definition = nullptr;
        rc = archive.Read3dmInstanceDefinition(&instance_definition);
        if ( rc==0 )
          break; // end of instance definition table
        if ( rc < 0 ) 
          break;
        if (AddModelComponentForExperts(instance_definition, bManageComponents,bResolveIdAndNameConflicts,bUpdateComponentIdentification).IsEmpty())
          delete instance_definition;
      }
    }
    
    // If BeginRead3dmInstanceDefinitionTable() returns true, 
    // then you MUST call EndRead3dmInstanceDefinitionTable().
    if ( !archive.EndRead3dmInstanceDefinitionTable() )
    {
      return false;
    }
  }

  return (0 == archive.CriticalErrorCount());
}

bool MYONX_Model::IncrementalReadModelGeometry(
  MYON_BinaryArchive& archive,
  bool bManageModelGeometryComponent,
  bool bManageGeometry,
  bool bManageAttributes,
  unsigned int object_filter,
  MYON_ModelComponentReference& model_component_reference
  )
{
  model_component_reference = MYON_ModelComponentReference::Empty;

  MYON_3dmArchiveTableType active_table = archive.Active3dmTable();
  if (MYON_3dmArchiveTableType::Unset == active_table)
  {
    MYON_3dmArchiveTableType previous_table = archive.Previous3dmTable();
    if (MYON_3dmArchiveTableType::Unset == previous_table)
    {
      // Yokel probably forgot to call IncrementalReadBegin()
      MYON_ERROR("IncrementalReadBegin() must be called before IncrementalReadModelGeometry().");
      return false;
    }
    
    if (static_cast<unsigned int>(previous_table) >= static_cast<unsigned int>(MYON_3dmArchiveTableType::object_table))
    {
      // Yokel either read or skipped reading the geometry table.
      MYON_ERROR("Too late to read the geoemtry table.");
      return false;
    }

    if (false == archive.BeginRead3dmObjectTable())
    {
      MYON_ERROR("Geoemtry table cannot be read from archive.");
      return false;
    }
    active_table = archive.Active3dmTable();
    if (active_table != MYON_3dmArchiveTableType::object_table)
    {
      MYON_ERROR("Catestrophic geoemtry table reading error.");
      return false;
    }
  }  
  else if (active_table != MYON_3dmArchiveTableType::object_table)
  {
    // Yokel is calling IncrementalReadModelGeometry() at the wrong time.
    MYON_ERROR("IncrementalReadModelGeometry() cannot be called while reading another part of the 3dm archive.");
    return false;
  }

  MYON_3dmArchiveTableStatus object_table_status = archive.Archive3dmTableStatus(MYON_3dmArchiveTableType::object_table);
  if (MYON_3dmArchiveTableType::object_table != object_table_status.m_table_type)
  {
    MYON_ERROR("Catestrophic geoemtry table reading error.");
    return false;
  }

  switch(object_table_status.m_state)
  {
  case MYON_3dmArchiveTableStatus::TableState::Started:
  case MYON_3dmArchiveTableStatus::TableState::InProgress:
    break;
  case MYON_3dmArchiveTableStatus::TableState::Finished:
    {
      MYON_ERROR("Geometry table has already been read from archive.");
      return false;
    }
  default:
    {
      MYON_ERROR("Geometry table reading error.");
      return false;
    }
  }

  for(;;)
  {
    MYON_ModelGeometryComponent* model_geometry = nullptr;
    int rc = archive.Read3dmModelGeometryForExperts(bManageGeometry,bManageAttributes,&model_geometry,object_filter);
    if ( rc <= 0 )
    {
      // end of object table or error reading
      // If BeginRead3dmObjectTable() returns true, 
      // then you MUST call EndRead3dmObjectTable().
      archive.EndRead3dmObjectTable();
      return (0==rc);
    }

    if (2 == rc && 0 != object_filter)
    {
      if ( nullptr != model_geometry)
        delete model_geometry;
      continue; // item was intentionally skipped.
    }

    model_component_reference = AddModelComponentForExperts(model_geometry,bManageModelGeometryComponent,true,true);

    if (model_component_reference.IsEmpty())
      continue;

    // return the read object.
    break;
  }

  return true;
}

bool MYONX_Model::IncrementalReadFinish(
    MYON_BinaryArchive& archive,
    bool bManageComponents,
    unsigned int table_filter,
    MYON_TextLog* error_log
    )
{
  int rc;

  const bool bResolveIdAndNameConflicts = true;
  const bool bUpdateComponentIdentification = true;

  if ( 0 == table_filter )
    table_filter = 0xFFFFFFFF; // read everything

  // STEP 16: Read history table
  if ( archive.BeginRead3dmHistoryRecordTable() )
  {
    if ( 0 != (static_cast<unsigned int>(MYON_3dmArchiveTableType::historyrecord_table) & table_filter) )
    {
      for(;;) 
      {
        MYON_HistoryRecord* pHistoryRecord = nullptr;
        rc = archive.Read3dmHistoryRecord(pHistoryRecord);
        if ( rc == 0 )
          break; // end of history record table
        if ( rc < 0 ) 
          break;
        if ( AddModelComponentForExperts(pHistoryRecord,bManageComponents,bResolveIdAndNameConflicts,bUpdateComponentIdentification).IsEmpty() )
        {
          delete pHistoryRecord;
          continue;
        }
      }
    }
    
    // If BeginRead3dmHistoryRecordTable() returns true, 
    // then you MUST call EndRead3dmHistoryRecordTable().
    if ( !archive.EndRead3dmHistoryRecordTable() )
    {
      return false;
    }
  }

  // STEP 17: OPTIONAL - Read user tables as anonymous goo
  // If you develop a plug-ins or application that uses OpenNURBS files,
  // you can store anything you want in a user table.
  for(;;)
  {
    if ( archive.Archive3dmVersion() <= 1 )
    {
      // no user tables in version 1 archives.
      break;
    }

    {
      MYON__UINT32 tcode = 0;
      MYON__INT64 big_value = 0;
      if ( !archive.PeekAt3dmBigChunkType(&tcode,&big_value) )
        break;
      if ( TCODE_USER_TABLE != tcode )
        break;
    }
    MYON_UUID plugin_id = MYON_nil_uuid;
    bool bGoo = false;
    int usertable_3dm_version = 0;
    unsigned int usertable_opennurbs_version = 0;
    if ( !archive.BeginRead3dmUserTable( plugin_id, &bGoo, &usertable_3dm_version, &usertable_opennurbs_version ) )
    {
      // attempt to skip bogus user table
      const MYON__UINT64 pos0 = archive.CurrentPosition();
      MYON__UINT32 tcode = 0;
      MYON__INT64 big_value = 0;
      if  ( !archive.BeginRead3dmBigChunk(&tcode,&big_value) )
        break;
      if ( !archive.EndRead3dmChunk() )
        break;
      const MYON__UINT64 pos1 = archive.CurrentPosition();
      if (pos1 <= pos0)
        break;
      if ( TCODE_USER_TABLE != tcode )
        break;

      continue; // skip this bogus user table
    }

    if ( 
      nullptr == m_model_user_string_list
      && plugin_id == MYON_CLASS_ID(MYON_DocumentUserStringList) 
      )
    {
      // Read the document user strings (key-value pairs) as
      // a user table with plug-in id = MYON_CLASS_ID(MYON_DocumentUserStringList)
      MYON_Object* p = 0;
      archive.ReadObject(&p);
      m_model_user_string_list = MYON_DocumentUserStringList::Cast(p);
      if ( 0 == m_model_user_string_list )
      {
        MYON_ERROR("The document user string information in the file is damaged.");
        if ( 0 != p )
          delete p;
          p = 0;
      }
    } 
    else if ( 0 != (static_cast<unsigned int>(MYON_3dmArchiveTableType::user_table) & table_filter) )
    {
      // read user data tables as anonymous goo
      MYONX_Model_UserData* model_ud = new MYONX_Model_UserData();
      model_ud->m_uuid = plugin_id;
      model_ud->m_usertable_3dm_version = usertable_3dm_version;
      model_ud->m_usertable_opennurbs_version = usertable_opennurbs_version;
      if ( !archive.Read3dmAnonymousUserTable( usertable_3dm_version, usertable_opennurbs_version, model_ud->m_goo ) )
      {
        delete model_ud;
        break;
      }
      m_userdata_table.Append(model_ud);
    }

    // If BeginRead3dmObjectTable() returns true, 
    // then you MUST call EndRead3dmUserTable().
    if ( !archive.EndRead3dmUserTable() )
    {
      break;
    }
  }

  // STEP 18: OPTIONAL - check for end mark
  size_t file_length = 0;
  if ( !archive.Read3dmEndMark(&file_length) )
  {
    if ( archive.Archive3dmVersion() != 1 ) 
    {
      // some v1 files are missing end-of-archive markers
    }
  }
  else
    m_3dm_file_byte_count = file_length;
  return (0 == archive.CriticalErrorCount());
}


bool MYONX_Model::Read( 
    const char* filename,
    unsigned int table_filter,
    unsigned int model_object_type_filter,
    MYON_TextLog* error_log
    )
{
  const MYON_wString wfilename_buffer(filename);
  const wchar_t* wfilename = static_cast< const wchar_t* >(wfilename_buffer);
  return Read(wfilename,table_filter,model_object_type_filter,error_log);
}

bool MYONX_Model::Read( 
    const wchar_t* filename,
    unsigned int table_filter,
    unsigned int model_object_type_filter,
    MYON_TextLog* error_log
    )
{
  bool bCallReset = true;
  bool rc = false;

  if ( 0 != filename )
  {
    FILE* fp = MYON::OpenFile(filename,L"rb");
    if ( 0 != fp )
    {
      bCallReset = false;
      MYON_BinaryFile file(MYON::archive_mode::read3dm,fp);
      rc = Read(file, table_filter, model_object_type_filter, error_log);
      MYON::CloseFile(fp);
    }
  }

  if ( bCallReset )
    Reset();

  return rc;
}

bool MYONX_Model::Read(MYON_BinaryArchive& archive, unsigned int table_filter,
                     unsigned int model_object_type_filter, MYON_TextLog* error_log)
{
  // STEPS 1 to 14: REQUIRED.
  const bool bManageComponents = true;
  IncrementalReadBegin(archive, bManageComponents, table_filter, error_log);
  if (0 != archive.CriticalErrorCount())
    return false;

  // STEP 15: REQUIRED - Read object (geometry and annotation) table.
  if (0 == (static_cast<unsigned int>(MYON_3dmArchiveTableType::object_table) & table_filter))
  {
    const bool bManageGeometry = true;
    const bool bManageAttributes = true;
    for (;;)
    {
      MYON_ModelComponentReference model_geometry_reference;

      if (!IncrementalReadModelGeometry(archive, bManageComponents, bManageGeometry, bManageAttributes,
                                        model_object_type_filter, model_geometry_reference))
        break; // Catastrophic error.

      if (model_geometry_reference.IsEmpty())
        break; // No more geometry.
    }

    if (0 != archive.CriticalErrorCount())
      return false;
  }

  IncrementalReadFinish(archive, bManageComponents, table_filter, error_log);
  if (0 != archive.CriticalErrorCount())
    return false;

  if (0 != archive.BadCRCCount())
    return false;

  // Having read the model data, populate the RDK components.
  const int archive_3dm_version = archive.Archive3dmVersion();
  m_private->PopulateRDKComponents(archive_3dm_version);

  return true;
}

bool MYONX_Model::Read(MYON_BinaryArchive& archive, MYON_TextLog* error_log)
{
  unsigned int table_filter = 0; // read every table
  unsigned int model_object_type_filter = 0; // read every type of object
  return Read(archive, table_filter, model_object_type_filter, error_log);
}

bool MYONX_Model::Write(const char* filename, int version, MYON_TextLog* error_log) const
{
  bool rc = false;

  if (nullptr != filename && 0 != filename[0])
  {
    FILE* fp = MYON::OpenFile(filename, "wb");
    if (nullptr != fp)
    {
      MYON_BinaryFile file(MYON::archive_mode::write3dm, fp);
      const MYON_wString wFileName(filename);
      file.SetArchiveFullPath(static_cast<const wchar_t*>(wFileName));
      rc = Write(file, version, error_log);
      MYON::CloseFile(fp);
    }
  }

  return rc;
}

bool MYONX_Model::Write(const wchar_t* filename, int version, MYON_TextLog* error_log) const
{
  bool rc = false;

  if (nullptr != filename && 0 != filename[0])
  {
    FILE* fp = MYON::OpenFile(filename, L"wb");
    if (nullptr != fp)
    {
      MYON_BinaryFile file(MYON::archive_mode::write3dm, fp);
      file.SetArchiveFullPath(filename);
      rc = Write(file, version, error_log);
      MYON::CloseFile(fp);
    }
  }

  return rc;
}

bool MYONX_Model::Write(MYON_BinaryArchive& archive, int version, MYON_TextLog* error_log) const
{
  m_private->UpdateRDKUserData(version);

  if ( 0 != version )
  {
    if (  version < 2
      ||  version > MYON_BinaryArchive::CurrentArchiveVersion()
      || (version < 50 && version > MYON_BinaryArchive::CurrentArchiveVersion()/10)
      || (version >= 50 && 0 != (version % 10))
      )
    {
      version = 0;
      if ( error_log) error_log->Print("MYONX_Model::Write version parameter = %d; it must be 0, or >= 2 and <= %d, or a multiple of 10 >= 50 and <= %d.\n",
                      version,MYON_BinaryArchive::CurrentArchiveVersion()/10,MYON_BinaryArchive::CurrentArchiveVersion());
    }
  }

  if ( !archive.WriteMode() )
  {
    // You passed in a bogus archive. You must pass MYON::archive_mode::write3dm to the archive constructor.
    if ( error_log) error_log->Print("MYONX_Model::Write archive.Mode() is not MYON::archive_mode::write3dm.\n"
                    "See MYONX_Model::Write example in the header file.\n");
    return false;
  }

  bool ok;

  // START SECTION
  ok = archive.Write3dmStartSection( version, static_cast< const char* >(m_sStartSectionComments) );
  if ( !ok )
  {
    // make sure your archive was created with MYON::archive_mode::write3dm mode.
    if ( error_log) error_log->Print("MYONX_Model::Write archive.Write3dmStartSection() failed.\n"
                    "Your archive is not properly initialized\n"
                    "(make sure you passed MYON::archive_mode::write3dm to the constructor),\n"
                    "a file is locked, a disk is locked, or something along those lines.\n");
    return false;
  }

  // PROPERTIES SECTION
  if ( m_properties.m_RevisionHistory.m_revision_count == 0 )
    const_cast<MYONX_Model*>(this)->m_properties.m_RevisionHistory.NewRevision();

  ok = archive.Write3dmProperties( m_properties );
  if ( !ok )
  {
    // make sure m_properties is valid
    if ( error_log) error_log->Print("MYONX_Model::Write archive.Write3dmProperties() failed.\n"
                    "Your m_properties information is not valid or basic file writing failed.\n"
                    );
    return false;
  }

  // SETTINGS SECTION
  ok = archive.Write3dmSettings( m_settings );
  if ( !ok )
  {
    // make sure m_settings is valid
    if ( error_log) error_log->Print("MYONX_Model::Write archive.Write3dmSettings() failed.\n"
                    "Your m_settings information is not valid or basic file writing failed.\n");
    return false;
  }

  // BITMAP TABLE
  ok = archive.BeginWrite3dmBitmapTable();
  if ( !ok )
  {
    if ( error_log) error_log->Print("MYONX_Model::Write archive.BeginWrite3dmBitmapTable() failed.\n");
    return false;
  }
  
  for( 
    class MYONX_ModelComponentReferenceLink* link = Internal_ComponentListConst(MYON_ModelComponent::Type::Image).m_first_mcr_link;
    nullptr != link && ok;
    link = link->m_next
    )
  {
    ok = archive.Write3dmImageComponent(link->m_mcr);
    if ( !ok )
    {
      if ( error_log) error_log->Print("MYONX_Model::Write archive.Write3dmImageComponent() failed.\n");
    }
  } 

  if ( !archive.EndWrite3dmBitmapTable() )
  {
    if ( error_log) error_log->Print("MYONX_Model::Write archive.EndWrite3dmBitmapTable() failed.\n");
    return false;
  }
  if (!ok)
    return false;


  // TEXTURE MAPPING TABLE
  if ( archive.Archive3dmVersion() >= 4 )
  {
    ok = archive.BeginWrite3dmTextureMappingTable();
    if ( !ok )
    {
      if ( error_log) error_log->Print("MYONX_Model::Write archive.BeginWrite3dmTextureMappingTable() failed.\n");
      return false;
    }

    for( 
      class MYONX_ModelComponentReferenceLink* link = Internal_ComponentListConst(MYON_ModelComponent::Type::TextureMapping).m_first_mcr_link;
      nullptr != link && ok;
      link = link->m_next
      )
    {
      ok = archive.Write3dmTextureMappingComponent(link->m_mcr);
      if ( !ok )
      {
        if ( error_log) error_log->Print("MYONX_Model::Write archive.TextureMapping() failed.\n");
      }
    } 

    if ( !archive.EndWrite3dmTextureMappingTable() )
    {
      if ( error_log) error_log->Print("MYONX_Model::Write archive.EndWrite3dmTextureMappingTable() failed.\n");
      return false;
    }
    if (!ok)
      return false;
  }

  // MATERIAL TABLE
  ok = archive.BeginWrite3dmMaterialTable();
  if ( !ok )
  {
    if ( error_log) error_log->Print("MYONX_Model::Write archive.BeginWrite3dmMaterialTable() failed.\n");
    return false;
  }

  for( 
    class MYONX_ModelComponentReferenceLink* link = Internal_ComponentListConst(MYON_ModelComponent::Type::RenderMaterial).m_first_mcr_link;
    nullptr != link && ok;
    link = link->m_next
    )
  {
    ok = archive.Write3dmMaterialComponent(link->m_mcr);
    if ( !ok )
    {
      if ( error_log) error_log->Print("MYONX_Model::Write archive.Write3dmMaterialComponent() failed.\n");
    }
  } 

  if ( !archive.EndWrite3dmMaterialTable() )
  {
    if ( error_log) error_log->Print("MYONX_Model::Write archive.EndWrite3dmMaterialTable() failed.\n");
    return false;
  }
  if (!ok)
    return false;

  // LINETYPE TABLE
  if ( archive.Archive3dmVersion() >= 4 )
  {
    ok = archive.BeginWrite3dmLinetypeTable();
    if ( !ok )
    {
      if ( error_log) error_log->Print("MYONX_Model::Write archive.BeginWrite3dmLinetypeTable() failed.\n");
      return false;
    }
    for( 
      class MYONX_ModelComponentReferenceLink* link = Internal_ComponentListConst(MYON_ModelComponent::Type::LinePattern).m_first_mcr_link;
      nullptr != link && ok;
      link = link->m_next
      )
    {
      ok = archive.Write3dmLinePatternComponent(link->m_mcr);
      if ( !ok )
      {
        if ( error_log) error_log->Print("MYONX_Model::Write archive.Write3dmLinePatternComponent() failed.\n");
      }
    }
    if ( !archive.EndWrite3dmLinetypeTable() )
    {
      if ( error_log) error_log->Print("MYONX_Model::Write archive.EndWrite3dmLinetypeTable() failed.\n");
      return false;
    }
    if (!ok)
      return false;
  }

  // LAYER TABLE
  ok = archive.BeginWrite3dmLayerTable();
  if ( !ok )
  {
    // make sure m_settings is valid
    if ( error_log) error_log->Print("MYONX_Model::Write archive.BeginWrite3dmLayerTable() failed.\n");
    return false;
  }
  unsigned int layer_count = 0;
  for( 
    class MYONX_ModelComponentReferenceLink* link = Internal_ComponentListConst(MYON_ModelComponent::Type::Layer).m_first_mcr_link;
    nullptr != link && ok;
    link = link->m_next
    )
  {
    ok = archive.Write3dmLayerComponent(link->m_mcr);
    if (!ok)
    {
      if (error_log) error_log->Print("MYONX_Model::Write archive.Write3dmLayerComponent() failed.\n");
    }
    else
      layer_count++;
  }
  if (0 == layer_count && ok)
  {
    MYON_Layer layer(MYON_Layer::Default);
    layer.SetId();
    layer.SetIndex(0);
    ok = archive.Write3dmLayer(layer);
    if (!ok)
    {
      if (error_log) error_log->Print("MYONX_Model::Write archive.Write3dmLayer() failed.\n");
    }
  }
  if ( !archive.EndWrite3dmLayerTable() )
  {
    if ( error_log) error_log->Print("MYONX_Model::Write archive.EndWrite3dmLayerTable() failed.\n");
    return false;
  }
  if (!ok)
    return false;

  // GROUP TABLE
  ok = archive.BeginWrite3dmGroupTable();
  if ( !ok )
  {
    // make sure m_settings is valid
    if ( error_log) error_log->Print("MYONX_Model::Write archive.BeginWrite3dmGroupTable() failed.\n");
    return false;
  }
  for( 
    class MYONX_ModelComponentReferenceLink* link = Internal_ComponentListConst(MYON_ModelComponent::Type::Group).m_first_mcr_link;
    nullptr != link && ok;
    link = link->m_next
    )
  {
    ok = archive.Write3dmGroupComponent(link->m_mcr);
    if ( !ok )
    {
      if ( error_log) error_log->Print("MYONX_Model::Write archive.Write3dmGroupComponent() failed.\n");
    }
  } 
  if ( !archive.EndWrite3dmGroupTable() )
  {
    if ( error_log) error_log->Print("MYONX_Model::Write archive.EndWrite3dmGroupTable() failed.\n");
    return false;
  }
  if (!ok)
    return false;

  // DIMSTYLE TABLE
  if ( archive.Archive3dmVersion() >= 3 )
  {
    ok = archive.BeginWrite3dmDimStyleTable();
    if ( !ok )
    {
      // make sure m_settings is valid
      if ( error_log) error_log->Print("MYONX_Model::Write archive.BeginWrite3dmDimStyleTable() failed.\n");
      return false;
    }
    for( 
      class MYONX_ModelComponentReferenceLink* link = Internal_ComponentListConst(MYON_ModelComponent::Type::DimStyle).m_first_mcr_link;
      nullptr != link;
      link = link->m_next
      )
    {
      ok = archive.Write3dmDimStyleComponent(link->m_mcr);
      if (!ok)
      {
        if ( error_log) error_log->Print("MYONX_Model::Write archive.Write3dmDimStyleComponent() failed.\n");
      }
    }
    if ( !archive.EndWrite3dmDimStyleTable() )
    {
      if ( error_log) error_log->Print("MYONX_Model::Write archive.EndWrite3dmDimStyleTable() failed.\n");
      return false;
    }
    if (!ok)
      return false;
  }

  // LIGHT TABLE
  ok = archive.BeginWrite3dmLightTable();
  if ( !ok )
  {
    // make sure m_settings is valid
    if ( error_log) error_log->Print("MYONX_Model::Write archive.BeginWrite3dmLightTable() failed.\n");
    return false;
  }
  for( 
    class MYONX_ModelComponentReferenceLink* link = Internal_ComponentListConst(MYON_ModelComponent::Type::RenderLight).m_first_mcr_link;
    nullptr != link;
    link = link->m_next
    )
  {
    ok = archive.Write3dmModelLightComponent(link->m_mcr);
    if ( !ok )
    {
      if ( error_log) error_log->Print("MYONX_Model::Write archive.Write3dmModelLightComponent() failed.\n");
    }
  }
  if ( !archive.EndWrite3dmLightTable() )
  {
    if ( error_log) error_log->Print("MYONX_Model::Write archive.EndWrite3dmLightTable() failed.\n");
    return false;
  }
  if (!ok)
    return false;

  // HATCH PATTERN TABLE
  if ( archive.Archive3dmVersion() >= 4 )
  {
    ok = archive.BeginWrite3dmHatchPatternTable();
    if ( !ok )
    {
      if ( error_log) error_log->Print("MYONX_Model::Write archive.BeginWrite3dmHatchPatternTable() failed.\n");
      return false;
    }

    for( 
      class MYONX_ModelComponentReferenceLink* link = Internal_ComponentListConst(MYON_ModelComponent::Type::HatchPattern).m_first_mcr_link;
      nullptr != link;
      link = link->m_next
      )
    {
      ok = archive.Write3dmHatchPatternComponent(link->m_mcr);
      if ( !ok )
      {
        if ( error_log) error_log->Print("MYONX_Model::Write archive.Write3dmHatchPatternComponent() failed.\n");
      }
    }

    if ( !archive.EndWrite3dmHatchPatternTable() )
    {
      if ( error_log) error_log->Print("MYONX_Model::Write archive.EndWrite3dmHatchPatternTable() failed.\n");
      return false;
    }
    if (!ok)
      return false;
  }

  // INSTANCE DEFINITION TABLE
  if ( archive.Archive3dmVersion() >= 3 )
  {
    ok = archive.BeginWrite3dmInstanceDefinitionTable();
    if ( !ok )
    {
      // make sure m_settings is valid
      if ( error_log) error_log->Print("MYONX_Model::Write archive.BeginWrite3dmInstanceDefinitionTable() failed.\n");
      return false;
    }
    for( 
      class MYONX_ModelComponentReferenceLink* link = Internal_ComponentListConst(MYON_ModelComponent::Type::InstanceDefinition).m_first_mcr_link;
      nullptr != link;
      link = link->m_next
      )
    {
      ok = archive.Write3dmInstanceDefinitionComponent(link->m_mcr);
      if ( !ok )
      {
        if ( error_log) error_log->Print("MYONX_Model::Write archive.Write3dmInstanceDefinitionComponent() failed.\n");
      }
    }
    if ( !archive.EndWrite3dmInstanceDefinitionTable() )
    {
      if ( error_log) error_log->Print("MYONX_Model::Write archive.EndWrite3dmInstanceDefinitionTable() failed.\n");
      return false;
    }
    if (!ok)
      return false;
  }

  // OBJECT TABLE
  ok = archive.BeginWrite3dmObjectTable();
  if ( !ok )
  {
    if ( error_log) error_log->Print("MYONX_Model::Write archive.BeginWrite3dmObjectTable() failed.\n");
    return false;
  }
  for( 
    class MYONX_ModelComponentReferenceLink* link = Internal_ComponentListConst(MYON_ModelComponent::Type::ModelGeometry).m_first_mcr_link;
    nullptr != link;
    link = link->m_next
    )
  {
    ok = archive.Write3dmModelGeometryComponent(link->m_mcr);
    if ( !ok )
    {
      if ( error_log)
        error_log->Print("MYONX_Model::Write archive.Write3dmModelGeometryComponent() failed.\n");
    }
  }
  if ( !archive.EndWrite3dmObjectTable() )
  {
    if ( error_log) 
      error_log->Print("MYONX_Model::Write archive.EndWrite3dmObjectTable() failed.\n");
    return false;
  }
  if (!ok)
    return false;

  // HISTORY RECORD TABLE
  if ( archive.Archive3dmVersion() >= 4 )
  {
    ok = archive.BeginWrite3dmHistoryRecordTable();
    if ( !ok )
    {
      if ( error_log) error_log->Print("MYONX_Model::Write archive.BeginWrite3dmHistoryRecordTable() failed.\n");
      return false;
    }

    for( 
      class MYONX_ModelComponentReferenceLink* link = Internal_ComponentListConst(MYON_ModelComponent::Type::HistoryRecord).m_first_mcr_link;
      nullptr != link;
      link = link->m_next
      )
    {
      ok = archive.Write3dmHistoryRecordComponent(link->m_mcr);
      if ( !ok )
      {
        if ( error_log) error_log->Print("MYONX_Model::Write archive.Write3dmistoryRecordComponent() failed.\n");
      }
    }


    if( !archive.EndWrite3dmHistoryRecordTable() )
    {
      if ( error_log) error_log->Print("MYONX_Model::Write archive.EndWrite3dmHistoryTable() failed.\n");
      return false;
    }
    if (!ok)
      return false;
  }

  // STEP 17: - write user tables (plug-in info, etc.)
  if (archive.ArchiveContains3dmTable(MYON_3dmArchiveTableType::user_table))
  {
    if (nullptr != m_model_user_string_list && m_model_user_string_list->UserStringCount() > 0)
    {
      // Write the document user strings (key-value pairs) as
      // a user table with plug-in id = 
      MYON_UUID model_user_string_plugin_id = MYON_CLASS_ID(MYON_DocumentUserStringList);
      if (archive.BeginWrite3dmUserTable(model_user_string_plugin_id, false, 0, 0))
      {
        archive.WriteObject(m_model_user_string_list);
        archive.EndWrite3dmUserTable();
      }
    }

    // USER DATA TABLE
    for (int i = 0; ok && i < m_userdata_table.Count(); i++)
    {
      const MYONX_Model_UserData* model_ud = m_userdata_table[i];
      if (nullptr == model_ud)
        continue;
      if (MYON_UuidIsNotNil(model_ud->m_uuid))
      {
        if (!archive.Write3dmAnonymousUserTableRecord(
          model_ud->m_uuid,
          model_ud->m_usertable_3dm_version,
          model_ud->m_usertable_opennurbs_version,
          model_ud->m_goo)
          )
        {
          continue;
        }
      }
    }
  }

  if ( !archive.Write3dmEndMark() )
  {
    ok = false;
    if ( error_log) error_log->Print("MYONX_Model::Write archive.Write3dmEndMark() failed.\n");
  }

  return ok;
}

int MYONX_Model::UsesIDef(
        const MYON_InstanceRef& iref,
        MYON_UUID idef_uuid
        ) const
{
  // get id of idef we are looking for
  if ( MYON_UuidIsNil(idef_uuid) )
    return 0;

  // id of idef that defines iref
  MYON_UUID iref_idef_uuid = iref.m_instance_definition_uuid;
  if ( 0 == MYON_UuidCompare( idef_uuid, iref_idef_uuid ) )
    return 1;

  MYON_ModelComponentReference idef_reference = ComponentFromId(MYON_ModelComponent::Type::InstanceDefinition,iref_idef_uuid);
  const MYON_InstanceDefinition* iref_idef = MYON_InstanceDefinition::FromModelComponentRef(idef_reference,nullptr);

  if ( nullptr == iref_idef )
    return -1; // invalid id.


  // set iref_list[] = list of all nested instance references in iref_idef.
  MYON_SimpleArray<const MYON_InstanceRef*> iref_list(256);
  const MYON_SimpleArray<MYON_UUID>& iref_idef_object_uuid = iref_idef->InstanceGeometryIdList();
  for ( unsigned j = 0; j < iref_idef_object_uuid.UnsignedCount(); j++ )
  {
    MYON_ModelComponentReference component_reference = ComponentFromId(MYON_ModelComponent::Type::InstanceDefinition,iref_idef_object_uuid[j]);
    const MYON_ModelGeometryComponent* model_geometry = MYON_ModelGeometryComponent::Cast(component_reference.ModelComponent());
    if ( nullptr == model_geometry )
      continue;
    const MYON_Geometry* geometry = model_geometry->Geometry(nullptr);
    if ( nullptr == geometry )
      continue;
    if ( geometry->ObjectType() != MYON::instance_reference )
      continue;
    const MYON_InstanceRef* pNestedIRef = MYON_InstanceRef::Cast(geometry);
    if ( nullptr == pNestedIRef )
      continue;
    if ( 0 == MYON_UuidCompare( idef_uuid, pNestedIRef->m_instance_definition_uuid ) )
      return 2;
    iref_list.Append(pNestedIRef);
  }

  // test the nested instance references to see if they use idef_index.
  unsigned int i1 = 0;
  int depth = 3;
  for ( depth=3; i1 < iref_list.UnsignedCount(); depth++ )
  {
    const unsigned int i0 = i1;
    i1 = iref_list.UnsignedCount();
    for ( unsigned int i = i0; i < i1; i++ )
    {
      const MYON_InstanceRef* pNestedIRef = iref_list[i];
      if ( nullptr == pNestedIRef )
        continue;
      MYON_ModelComponentReference nested_idef_reference = ComponentFromId(MYON_ModelComponent::Type::InstanceDefinition,pNestedIRef->m_instance_definition_uuid);
      const MYON_InstanceDefinition* nested_idef = MYON_InstanceDefinition::FromModelComponentRef(nested_idef_reference,nullptr);
      if ( nullptr == nested_idef )
        continue;
      const MYON_SimpleArray<MYON_UUID>& nested_idef_object_uuid = nested_idef->InstanceGeometryIdList();
      for ( unsigned int j = 0; j < nested_idef_object_uuid.UnsignedCount(); j++ )
      {
        MYON_ModelComponentReference component_reference = ComponentFromId(MYON_ModelComponent::Type::InstanceDefinition,nested_idef_object_uuid[j]);
        const MYON_ModelGeometryComponent* model_geometry = MYON_ModelGeometryComponent::Cast(component_reference.ModelComponent());
        if ( nullptr == model_geometry )
          continue;
        const MYON_Geometry* geometry = model_geometry->Geometry(nullptr);
        if ( nullptr == geometry )
          continue;
        if ( geometry->ObjectType() != MYON::instance_reference )
          continue;
        const MYON_InstanceRef* pNestedIRef_local = MYON_InstanceRef::Cast(geometry);
        if ( nullptr == pNestedIRef_local )
          continue;
        if ( 0 == MYON_UuidCompare( idef_uuid, pNestedIRef_local->m_instance_definition_uuid ) )
          return depth;
        iref_list.Append(pNestedIRef_local);
      }
    }
    if ( i1 > 10000 && i1 < iref_list.UnsignedCount() )
      return -2; // probably have a circular reference
  }

  return 0;
}

MYON__UINT64 MYONX_Model::ModelContentVersionNumber() const
{
  return m_private->m_model_content_version_number;
}

void MYONX_Model::Internal_IncrementModelContentVersionNumber()
{
  m_private->m_model_content_version_number++;
}

bool MYONX_Model::SetDocumentUserString( const wchar_t* key, const wchar_t* string_value )
{
  if (nullptr == key || 0 == key[0] )
    return false;
  if ( nullptr == m_model_user_string_list )
    m_model_user_string_list = new MYON_DocumentUserStringList();
  return m_model_user_string_list->SetUserString(key,string_value);
}

bool MYONX_Model::GetDocumentUserString( 
  const wchar_t* key,
  MYON_wString& string_value 
  ) const
{
  if ( nullptr != m_model_user_string_list )
    return m_model_user_string_list->GetUserString(key,string_value);
  string_value = MYON_wString::EmptyString;
  return false;
}

int MYONX_Model::GetDocumentUserStrings(
  MYON_ClassArray<MYON_UserString>& user_strings
  ) const
{
  if ( nullptr != m_model_user_string_list )
    return m_model_user_string_list->GetUserStrings(user_strings);
  return 0;
}

MYONX_ModelComponentIterator::MYONX_ModelComponentIterator(
  const MYONX_Model& model,
  MYON_ModelComponent::Type component_type
  )
  : m_component_type(component_type)
  , m_model(&model)
{}

const MYONX_Model* MYONX_ModelComponentIterator::Model() const
{
  return m_model;
}

const class MYONX_Model::MYONX_ModelComponentList* MYONX_ModelComponentIterator::Internal_List() const
{
  if (nullptr != m_list)
    return m_list;

  if ( nullptr == m_model )
    return nullptr;

  m_list = &m_model->Internal_ComponentListConst(m_component_type);
  
  return m_list;
}

void MYONX_ModelComponentIterator::Internal_SetLink(
  const class MYONX_ModelComponentReferenceLink* link
  ) const
{
  // m_model is never nullptr when this function is called
  m_model_content_version = m_model->ModelContentVersionNumber();
  m_link = link;
  if (nullptr == m_link)
  {
    m_current_component_sn = 0;
    m_next_component_sn = 0;
    m_prev_component_sn = 0;
    m_current_component_weak_ref = MYON_ModelComponentWeakReference::Empty;
    m_model_content_version = 0;
  }
  else
  {
    m_current_component_sn = link->m_sn;
    m_next_component_sn = (nullptr != link->m_next) ? link->m_next->m_sn : 0;
    m_prev_component_sn = (nullptr != link->m_prev) ? link->m_prev->m_sn : 0;
    m_current_component_weak_ref = link->m_mcr;
  }
}

void MYONX_ModelComponentIterator::Internal_SetLink(
  MYON__UINT64 model_component_sn
  ) const
{
}


MYON_ModelComponentReference MYONX_ModelComponentIterator::FirstComponentReference()
{
  return MYON_ModelComponentReference(FirstComponentWeakReference());
}

MYON_ModelComponentReference MYONX_ModelComponentIterator::LastComponentReference()
{
  return MYON_ModelComponentReference(LastComponentWeakReference());
}

MYON_ModelComponentReference MYONX_ModelComponentIterator::CurrentComponentReference() const
{
  return MYON_ModelComponentReference(CurrentComponentWeakReference());
}

MYON_ModelComponentReference MYONX_ModelComponentIterator::NextComponentReference()
{
  return MYON_ModelComponentReference(NextComponentWeakReference());
}

MYON_ModelComponentReference MYONX_ModelComponentIterator::PreviousComponentReference()
{
  return MYON_ModelComponentReference(PreviousComponentWeakReference());
}



MYON_ModelComponentWeakReference MYONX_ModelComponentIterator::FirstComponentWeakReference()
{
  const MYONX_Model::MYONX_ModelComponentList* list = Internal_List();
  Internal_SetLink((nullptr != list) ? list->m_first_mcr_link : nullptr);
  return m_current_component_weak_ref;
}

MYON_ModelComponentWeakReference MYONX_ModelComponentIterator::LastComponentWeakReference()
{
  const MYONX_Model::MYONX_ModelComponentList* list = Internal_List();
  Internal_SetLink((nullptr != list) ? list->m_last_mcr_link : nullptr);
  return m_current_component_weak_ref;
}

MYON_ModelComponentWeakReference MYONX_ModelComponentIterator::CurrentComponentWeakReference() const
{
  // unchanged ModelContentVersionNumber() means that m_link is safe to dreference.
  // Otherwise use sn for safe reset.
  if (m_model_content_version != m_model->ModelContentVersionNumber() )
    Internal_SetLink(m_model->Internal_ModelComponentLinkFromSerialNumber(m_current_component_sn));
  return m_current_component_weak_ref;
}

MYON_ModelComponentWeakReference MYONX_ModelComponentIterator::NextComponentWeakReference()
{
  if ( nullptr == m_list )
    return FirstComponentReference();

  if ( nullptr == m_link )
    return MYON_ModelComponentReference::Empty;

  if (m_model_content_version == m_model->ModelContentVersionNumber() && nullptr != m_link )
  {
    // unchanged ModelContentVersionNumber() means that m_link is safe to dreference.
    m_link = m_link->m_next;
    if (nullptr == m_link)
    {
      m_prev_component_sn = m_current_component_sn;
      m_current_component_sn = 0;
      m_next_component_sn = 0;
      m_current_component_weak_ref = MYON_ModelComponentWeakReference::Empty;
    }
    else
    {
      m_current_component_sn = m_link->m_sn;
      m_next_component_sn = (nullptr != m_link->m_next) ? m_link->m_next->m_sn : 0;
      m_prev_component_sn = (nullptr != m_link->m_prev) ? m_link->m_prev->m_sn : 0;
      m_current_component_weak_ref = m_link->m_mcr;
    }
  }
  else if ( 0 != m_next_component_sn )
  {
    // Otherwise m_link is not safe to dereference.
    // Use slower serial number lookup.
    Internal_SetLink(m_model->Internal_ModelComponentLinkFromSerialNumber(m_next_component_sn));
  }
  else
  {
    m_link = nullptr;
    m_current_component_sn = 0;
    m_current_component_weak_ref = MYON_ModelComponentWeakReference::Empty;
  }

  return m_current_component_weak_ref;
}

MYON_ModelComponentWeakReference MYONX_ModelComponentIterator::PreviousComponentWeakReference()
{
  if ( nullptr == m_list )
    return LastComponentReference();

  if ( nullptr == m_link )
    return MYON_ModelComponentReference::Empty;

  if (m_model_content_version == m_model->ModelContentVersionNumber() && nullptr != m_link )
  {
    m_link = m_link->m_prev;
    if (nullptr == m_link)
    {
      m_next_component_sn = m_current_component_sn;
      m_current_component_sn = 0;
      m_prev_component_sn = 0;
      m_current_component_weak_ref = MYON_ModelComponentWeakReference::Empty;
    }
    else
    {
      m_current_component_sn = m_link->m_sn;
      m_next_component_sn = (nullptr != m_link->m_next) ? m_link->m_next->m_sn : 0;
      m_prev_component_sn = (nullptr != m_link->m_prev) ? m_link->m_prev->m_sn : 0;
      m_current_component_weak_ref = m_link->m_mcr;
    }
  }
  else if ( 0 != m_prev_component_sn )
  {
    Internal_SetLink(m_model->Internal_ModelComponentLinkFromSerialNumber(m_prev_component_sn));
  }
  else
  {
    m_link = nullptr;
    m_current_component_sn = 0;
    m_current_component_weak_ref = MYON_ModelComponentReference::Empty;
  }

  return m_current_component_weak_ref;
}

const MYON_ModelComponent* MYONX_ModelComponentIterator::FirstComponent()
{
  return FirstComponentReference().ModelComponent();
}

const MYON_ModelComponent* MYONX_ModelComponentIterator::LastComponent()
{
  return LastComponentReference().ModelComponent();
}

const MYON_ModelComponent* MYONX_ModelComponentIterator::CurrentComponent() const
{
  return CurrentComponentReference().ModelComponent();
}

const MYON_ModelComponent* MYONX_ModelComponentIterator::NextComponent()
{
  return NextComponentReference().ModelComponent();
}

const MYON_ModelComponent* MYONX_ModelComponentIterator::PreviousComponent()
{
  return PreviousComponentReference().ModelComponent();
}

unsigned int MYONX_ModelComponentIterator::ActiveComponentCount() const
{
  return (nullptr != m_list) ? m_list->m_count : 0;
}

class MYON_TextLogNull : public MYON_TextLog
{
public:
  MYON_TextLogNull() = default;
  ~MYON_TextLogNull() = default;

  void AppendText(const char*) override {}
  void AppendText(const wchar_t*) override {}
};

const char* MYONX_ModelTest::TestTypeToString(MYONX_ModelTest::Type test_type)
{
 switch (test_type)
  {
  case MYONX_ModelTest::Type::Unset:
    return "Unset";
    break;
  case MYONX_ModelTest::Type::Read:
    return "Read";
    break;
  case MYONX_ModelTest::Type::ReadWrite:
    return "ReadWrite";
    break;
  case MYONX_ModelTest::Type::ReadWriteRead:
    return "ReadWriteRead";
    break;
  case MYONX_ModelTest::Type::ReadWriteReadCompare:
    return "ReadWriteReadCompare";
    break;
  default:
    break;
  }

  MYON_ERROR("Invalid test_type parameter.");
  return "Invalid test_type parameter";
}


const wchar_t* MYONX_ModelTest::TestTypeToWideString(MYONX_ModelTest::Type test_type)
{
  switch (test_type)
  {
  case MYONX_ModelTest::Type::Unset:
    return L"Unset";
    break;
  case MYONX_ModelTest::Type::Read:
    return L"Read";
    break;
  case MYONX_ModelTest::Type::ReadWrite:
    return L"ReadWrite";
    break;
  case MYONX_ModelTest::Type::ReadWriteRead:
    return L"ReadWriteRead";
    break;
  case MYONX_ModelTest::Type::ReadWriteReadCompare:
    return L"ReadWriteReadCompare";
    break;
  default:
    break;
  }

  MYON_ERROR("Invalid test_type parameter.");
  return L"Invalid test_type parameter";
}

const char* MYONX_ModelTest::ResultToString(MYONX_ModelTest::Result result)
{
  switch (result)
  {
  case MYONX_ModelTest::Result::Unset:
    return "Unset";
    break;
  case MYONX_ModelTest::Result::Fail:
    return "Fail";
    break;
  case MYONX_ModelTest::Result::Errors:
    return "Errors";
    break;
  case MYONX_ModelTest::Result::Warnings:
    return "Warnings";
    break;
  case MYONX_ModelTest::Result::Pass:
    return "Pass";
    break;
  case MYONX_ModelTest::Result::Skip:
    return "Skip";
    break;
  default:
    break;
  }

  MYON_ERROR("Invalid result parameter.");
  return "Invalid result parameter";
}

const wchar_t* MYONX_ModelTest::ResultToWideString(MYONX_ModelTest::Result result)
{
  switch (result)
  {
  case MYONX_ModelTest::Result::Unset:
    return L"Unset";
    break;
  case MYONX_ModelTest::Result::Fail:
    return L"Fail";
    break;
  case MYONX_ModelTest::Result::Errors:
    return L"Errors";
    break;
  case MYONX_ModelTest::Result::Warnings:
    return L"Warnings";
    break;
  case MYONX_ModelTest::Result::Pass:
    return L"Pass";
    break;
  case MYONX_ModelTest::Result::Skip:
    return L"Skip";
    break;
  default:
    break;
  }

  MYON_ERROR("Invalid result parameter.");
  return L"Invalid result parameter";

}


MYONX_ModelTest::Result MYONX_ModelTest::WorstResult(
  MYONX_ModelTest::Result a,
  MYONX_ModelTest::Result b
)
{
  if (MYONX_ModelTest::Result::Unset == a && MYONX_ModelTest::Result::Unset != b)
    return b;
  if (MYONX_ModelTest::Result::Unset != a && MYONX_ModelTest::Result::Unset == b)
    return a;
  return
    (static_cast<unsigned char>(a) < static_cast<unsigned char>(b))
    ? a : b;
}

MYONX_ModelTest::Result MYONX_ModelTest::ResultFromErrorCounter(
  MYONX_ErrorCounter error_count,
  MYONX_ModelTest::Result no_errors_result
)
{
  if (error_count.FailureCount() > 0)
    return MYONX_ModelTest::Result::Fail;
  if (error_count.ErrorCount() > 0)
    return MYONX_ModelTest::Result::Errors;
  if (error_count.WarningCount() > 0)
    return MYONX_ModelTest::Result::Warnings;
  return no_errors_result;
}


void MYONX_ModelTest::Internal_BeginTest()
{
  *this = MYONX_ModelTest::Unset;

  m_test_result = MYONX_ModelTest::Result::Unset;
  const size_t count = sizeof(m_test_results) / sizeof(m_test_results[0]);
  for (size_t i = 0; i < count; i++)
  {
    m_test_results[i] = MYONX_ModelTest::Result::Unset;
  }

  m_error_count = MYONX_ErrorCounter::Zero;
  for (size_t i = 0; i < count; i++)
  {
    m_error_counts[i] = MYONX_ErrorCounter::Zero;
  }
  m_error_counts[0].ClearLibraryErrorsAndWarnings();
  m_error_count.ClearLibraryErrorsAndWarnings();

  m_current_test_index = 0;

  for (int i = 0; i < 3; i++)
  {
    m_model_3dm_file_version[i] = 0;
    m_model_hash[i] = MYON_SHA1_Hash::ZeroDigest;
  }
}


void MYONX_ModelTest::Internal_EndCurrentTest()
{
  if (m_current_test_index > 0 && MYONX_ModelTest::Result::Unset == m_test_results[m_current_test_index])
  {
    m_error_counts[m_current_test_index].AddLibraryErrorsAndWarnings();
    m_test_results[m_current_test_index] = MYONX_ModelTest::WorstResult(m_test_results[m_current_test_index], MYONX_ModelTest::ResultFromErrorCounter(m_error_counts[m_current_test_index], MYONX_ModelTest::Result::Pass));
    m_error_counts[0].ClearLibraryErrorsAndWarnings();
  }
}

void MYONX_ModelTest::Internal_BeginNextTest(
  MYONX_ModelTest::Type test_type
  )
{
  m_error_counts[0].AddLibraryErrorsAndWarnings();

  //const size_t count = sizeof(m_test_results) / sizeof(m_test_results[0]);
  const unsigned int test_index = static_cast<unsigned char>(test_type);
  
  if ( test_index > m_current_test_index  )
  {
    Internal_EndCurrentTest();
    m_current_test_index = test_index;
    m_test_results[m_current_test_index] = MYONX_ModelTest::Result::Unset;
    m_error_counts[m_current_test_index] = MYONX_ErrorCounter::Zero;
    m_error_counts[m_current_test_index].ClearLibraryErrorsAndWarnings();
  }
}

bool MYONX_ModelTest::Internal_TallyTestResults()
{
  const size_t count = sizeof(m_test_results) / sizeof(m_test_results[0]);
  m_test_results[0] = MYONX_ModelTest::WorstResult(m_test_results[0], MYONX_ModelTest::Result::Pass);
  for (size_t i = 0; i < count; i++)
  {
    m_test_results[i] = MYONX_ModelTest::WorstResult(m_test_results[i], MYONX_ModelTest::ResultFromErrorCounter(m_error_counts[i], MYONX_ModelTest::Result::Unset));
    m_test_result = MYONX_ModelTest::WorstResult(m_test_result, m_test_results[i]);
    m_error_count += m_error_counts[i];
  }

  m_error_count.ClearLibraryErrorsAndWarnings();

  m_test_result = MYONX_ModelTest::WorstResult(m_test_result, MYONX_ModelTest::ResultFromErrorCounter(m_error_count, MYONX_ModelTest::Result::Unset));

  return (MYONX_ModelTest::Result::Pass == m_test_result);
}

MYONX_ModelTest::Type MYONX_ModelTest::TestType() const
{
  return m_test_type;
}

const MYON_wString MYONX_ModelTest::Source3dmFilePath() const
{
  return m_source_3dm_file_path;
}

const MYON_wString MYONX_ModelTest::TextLogSource3dmFilePath() const
{
  return 
    m_text_log_3dm_file_path.IsNotEmpty() 
    ? m_text_log_3dm_file_path 
    : Source3dmFilePath();
}


unsigned int MYONX_ModelTest::Source3dmFileVersion() const
{
  return m_model_3dm_file_version[0];
}

bool MYONX_ModelTest::SkipCompare(unsigned int source_3dm_file_version)
{
  const bool bSkipCompare
    = (source_3dm_file_version >= 1 && source_3dm_file_version < 50);
  return bSkipCompare;
}

MYONX_ModelTest::Result MYONX_ModelTest::TestResult() const
{
  return m_test_result;
}
 
MYONX_ModelTest::Result MYONX_ModelTest::TestResult(
  MYONX_ModelTest::Type test_type
)
{
  const unsigned int i = static_cast<unsigned char>(m_test_type);
  return m_test_results[i];
}


MYONX_ErrorCounter MYONX_ModelTest::ErrorCounter() const
{
  return m_error_count;
}
 

MYONX_ErrorCounter MYONX_ModelTest::ErrorCounter(
  MYONX_ModelTest::Type test_type
) const
{
  const unsigned int i = static_cast<unsigned char>(m_test_type);
  return m_error_counts[i];
}

bool MYONX_ModelTest::ReadTest(
    const char* file_path,
    MYONX_ModelTest::Type test_type,
    bool bKeepModels,
    const char* text_log_file_path,
    MYON_TextLog* text_log
)
{
  Internal_BeginTest();

  m_test_type = test_type;

  MYON_TextLogNull devnull;
  if (nullptr == text_log)
    text_log = &MYON_TextLog::Null;

  FILE* fp = nullptr;
  for (;;)
  {
    if (nullptr == file_path || 0 == file_path[0])
    {
      m_error_counts[0].IncrementFailureCount();
      text_log->Print("file_path was the empty string.");
      break;
    }

    fp = MYON_FileStream::Open3dmToRead(file_path);
    if (nullptr == fp)
    {
      m_error_counts[0].IncrementFailureCount();
      text_log->Print("MYON_FileStream::Open(%s, \"rb\") failed.",file_path);
      break;
    }

    MYON_BinaryFile archive(MYON::archive_mode::read3dm, fp);
    archive.SetArchiveFullPath(MYON_wString(file_path));

    MYON_wString wide_text_log_file_path(text_log_file_path);
    Internal_ReadTest(archive, test_type, bKeepModels, wide_text_log_file_path, text_log);
    break;
  }

  if (nullptr != fp)
  {
    if (0 != MYON_FileStream::Close(fp))
    {
      text_log->Print("MYON_FileStream::Close(%s) failed.", file_path);
      m_error_counts[0].IncrementErrorCount();
    }
  }

  return Internal_TallyTestResults();
}


bool MYONX_ModelTest::ReadTest(
  const wchar_t* file_path,
  MYONX_ModelTest::Type test_type,
  bool bKeepModels,
  const wchar_t* text_log_file_path,
  MYON_TextLog* text_log
)
{
  Internal_BeginTest();

  MYON_TextLogNull devnull;
  if (nullptr == text_log)
    text_log = &MYON_TextLog::Null;

  FILE* fp = nullptr;
  for (;;)
  {
    if (nullptr == file_path || 0 == file_path[0])
    {
      m_error_counts[0].IncrementFailureCount();
      text_log->Print("file_path was the empty string.");
      break;
    }

    fp = MYON_FileStream::Open3dmToRead(file_path);
    if (nullptr == fp)
    {
      m_error_counts[0].IncrementFailureCount();
      text_log->Print(L"MYON_FileStream::Open(%ls, L\"rb\") failed.",file_path);
      break;
    }

    MYON_BinaryFile archive(MYON::archive_mode::read3dm, fp);
    archive.SetArchiveFullPath(file_path);
    Internal_ReadTest(archive, test_type, bKeepModels, text_log_file_path, text_log);
    break;
  }

  if (nullptr != fp)
  {
    if (0 != MYON_FileStream::Close(fp))
    {
      text_log->Print(L"MYON_FileStream::Close(%ls) failed.",file_path);
      m_error_counts[0].IncrementErrorCount();
    }
  }

  return Internal_TallyTestResults();
}

bool MYONX_ModelTest::ReadTest(
  FILE* fp,
  MYONX_ModelTest::Type test_type,
  bool bKeepModels,
  const wchar_t* text_log_file_path,
  MYON_TextLog* text_log
)
{
  Internal_BeginTest();

  MYON_TextLogNull devnull;
  if (nullptr == text_log)
    text_log = &MYON_TextLog::Null;

  for (;;)
  {
    if (nullptr == fp)
    {
      m_error_counts[0].IncrementFailureCount();
      text_log->Print("fp is nullptr.");
      break;
    }

    MYON_BinaryFile archive(MYON::archive_mode::read3dm, fp);

    Internal_ReadTest(archive, test_type, bKeepModels, text_log_file_path, text_log);
    break;
  }

  return Internal_TallyTestResults();
}

static bool InternalCleanPass(
  MYONX_ModelTest::Result result,
  MYONX_ErrorCounter error_counter
)
{
  return (MYONX_ModelTest::Result::Pass == result && 0 == error_counter.TotalCount());
}

static void InternalDumpResultAndErrorCount(
  MYONX_ModelTest::Result result,
  MYONX_ErrorCounter error_counter,
  MYON_TextLog& text_log
)
{
  text_log.Print("%s", MYONX_ModelTest::ResultToString(result));
  if (false == InternalCleanPass(result,error_counter))
  {
    text_log.Print(": ");
    error_counter.Dump(text_log);
  }
  text_log.PrintNewLine();
}

bool MYONX_ModelTest::DumpModel(const MYONX_Model* model, MYON_TextLog& text_log)
{
  if (nullptr == model || model->Manifest().ActiveComponentCount(MYON_ModelComponent::Type::Unset) <= 0)
    return false;

  MYON_TextHash hash_log;
  hash_log.SetIdRemap(true);
  hash_log.SetOutputTextLog(&text_log);
  model->Dump(hash_log);
  const MYON_SHA1_Hash dump_hash = hash_log.Hash();
  text_log.PrintNewLine();
  text_log.Print("Model Hash: ");
  dump_hash.Dump(text_log);
  text_log.PrintNewLine();

  return (false == dump_hash.IsZeroDigestOrEmptyContentHash());
}

std::shared_ptr<MYONX_Model> MYONX_ModelTest::SourceModel() const
{
  return m_model[0];
}

std::shared_ptr<MYONX_Model> MYONX_ModelTest::ReadWriteReadModel() const
{
  const auto source_model = SourceModel();
  if (nullptr != source_model)
  {
    for (int i = 1; i < 3; i++)
    {
      const MYONX_Model* copy_model = m_model[i].get();
      if (nullptr == copy_model)
        continue;
      if (copy_model->m_3dm_file_version != source_model->m_3dm_file_version)
        continue;
      return m_model[i];
    }
  }
  std::shared_ptr<MYONX_Model> nullsp;
  return nullsp;
}

static const MYON_wString Internal_DumpModelfileName(
  const MYON_wString source_3dm_file_path,
  bool bSourceModel
)
{
  MYON_wString file_name_stem = MYON_FileSystemPath::FileNameFromPath(source_3dm_file_path,false);
  if (file_name_stem.IsEmpty())
    return MYON_wString::EmptyString;
  MYON_wString text_file_path = MYON_FileSystemPath::VolumeAndDirectoryFromPath(source_3dm_file_path);
  text_file_path += file_name_stem;
  text_file_path += L"_ONX_ModelTest_";
  if (bSourceModel)
    text_file_path += L"original";
  else
    text_file_path += L"copy";

#if defined(MYON_RUNTIME_WIN)
#if defined(MYON_64BIT_RUNTIME)
  text_file_path += L"_Win64";
#elif defined(MYON_32BIT_RUNTIME)
  text_file_path += L"_Win32";
#else
  text_file_path += L"_Win";
#endif
#elif defined(MYON_RUNTIME_APPLE_MACOS)
  text_file_path += L"_MacOS";
#elif defined(MYON_RUNTIME_APPLE_IOS)
  text_file_path += L"_iOS";
#elif defined(MYON_RUNTIME_APPLE)
  text_file_path += L"_AppleOS";
#elif defined(MYON_RUNTIME_ANDROID)
  text_file_path += L"_AndroidOS";
#endif

#if defined(MYON_DEBUG)
  text_file_path += L"Debug";
#else
  text_file_path += L"Release";
#endif

  text_file_path += L".txt";
  return text_file_path;
}

bool MYONX_ModelTest::DumpSourceModel() const
{
  const MYON_wString text_file_path = Internal_DumpModelfileName(m_source_3dm_file_path,true);
  return DumpSourceModel(text_file_path);
}

bool MYONX_ModelTest::DumpSourceModel(const wchar_t* text_file_full_path) const
{
  bool rc = false;
  FILE* fp = nullptr;
  for (;;)
  {
    if (nullptr == text_file_full_path || 0 == text_file_full_path[0])
      break;
    fp = MYON_FileStream::Open(text_file_full_path, L"w");
    if (nullptr == fp)
      break;
    const auto model = SourceModel();
    if (nullptr == model)
      break;
    if (model->Manifest().ActiveComponentCount(MYON_ModelComponent::Type::Unset) <= 0)
      break;
    MYON_TextLog text_log(fp);
    rc = DumpSourceModel(text_log);
    break;
  }
  if (nullptr != fp)
    MYON_FileStream::Close(fp);
  return rc;
}

bool MYONX_ModelTest::DumpSourceModel(MYON_TextLog& text_log) const
{
  return MYONX_ModelTest::DumpModel(SourceModel().get(), text_log);
}

bool MYONX_ModelTest::DumpReadWriteReadModel() const
{
  const MYON_wString text_file_path = Internal_DumpModelfileName(m_source_3dm_file_path,false);
  return DumpReadWriteReadModel(text_file_path);
}

bool MYONX_ModelTest::DumpReadWriteReadModel(const wchar_t* text_file_full_path) const
{
  bool rc = false;
  FILE* fp = nullptr;
  for (;;)
  {
    if (nullptr == text_file_full_path || 0 == text_file_full_path[0])
      break;
    fp = MYON_FileStream::Open(text_file_full_path, L"w");
    if (nullptr == fp)
      break;
    const auto model = ReadWriteReadModel();
    if (nullptr == model)
      break;
    if (model->Manifest().ActiveComponentCount(MYON_ModelComponent::Type::Unset) <= 0)
      break;
    MYON_TextLog text_log(fp);
    rc = DumpReadWriteReadModel(text_log);
    break;
  }

  if (nullptr != fp)
    MYON_FileStream::Close(fp);

  return rc;
}

bool MYONX_ModelTest::DumpReadWriteReadModel(MYON_TextLog& text_log) const
{
  return MYONX_ModelTest::DumpModel(ReadWriteReadModel().get(), text_log);
}

void MYONX_ModelTest::Dump(MYON_TextLog& text_log) const
{
  const MYONX_ModelTest::Type test_type = TestType();

  text_log.Print("Test type: %s\n", MYONX_ModelTest::TestTypeToString(test_type));

  //const MYON_wString source_archive = Source3dmFilePath();
  const MYON_wString test_log_source_archive = TextLogSource3dmFilePath();
  text_log.Print(L"Source 3dm file path: %ls\n", static_cast<const wchar_t*>(test_log_source_archive));
  text_log.Print(L"Source 3dm file version: %u\n", Source3dmFileVersion());

  text_log.Print("Result: ");
  InternalDumpResultAndErrorCount(m_test_result, m_error_count, text_log);

  //const int i_rwrcompare = static_cast<const unsigned char>(MYONX_ModelTest::Type::ReadWriteReadCompare);
  const bool bSkipCompare
    = MYONX_ModelTest::SkipCompare(Source3dmFileVersion())
    && MYONX_ModelTest::Type::ReadWriteReadCompare == test_type;
  const unsigned int imax
    = bSkipCompare
    ? static_cast<const unsigned char>(MYONX_ModelTest::Type::ReadWriteRead)
    : static_cast<const unsigned char>(test_type);
  bool bSkipDetails = InternalCleanPass(m_test_result, m_error_count);
  for (unsigned int i = 0; i <= imax && bSkipDetails; i++)
  {
    bSkipDetails = InternalCleanPass(m_test_results[i], m_error_counts[i]);
  }  

  if (bSkipDetails)
  {
    if (bSkipCompare)
    {
      text_log.PushIndent();
      text_log.Print("Compare test skipped because source file version is too old.\n");
      text_log.PopIndent();
    }
    return;
  }

  text_log.PushIndent();
  for(;;)
  {
    unsigned int i = 0;
    text_log.Print("Initialization: ");
    InternalDumpResultAndErrorCount(m_test_results[i], m_error_counts[i], text_log);
    if (i >= imax)
      break;

    i++;
    text_log.Print("Read source file: ");
    InternalDumpResultAndErrorCount(m_test_results[i], m_error_counts[i], text_log);
    if (i >= imax)
      break;

    i++;
    text_log.Print("Write temporary files: ");
    InternalDumpResultAndErrorCount(m_test_results[i], m_error_counts[i], text_log);
    if (i >= imax)
      break;

    i++;
    text_log.Print("Read temporary files: ");
    InternalDumpResultAndErrorCount(m_test_results[i], m_error_counts[i], text_log);
    if (i >= imax)
      break;

    i++;
    text_log.Print("Compare models from source and temporary file: ");
    InternalDumpResultAndErrorCount(m_test_results[i], m_error_counts[i], text_log);
    if (i >= imax)
      break;

    break;
  }
  text_log.PopIndent();  
}

bool MYONX_ModelTest::ReadTest(
  MYON_BinaryArchive& archive,
  MYONX_ModelTest::Type test_type,
  bool bKeepModels,
  const wchar_t* text_log_file_path,
  MYON_TextLog* text_log
)
{
  Internal_BeginTest();
  Internal_ReadTest(archive, test_type, bKeepModels, text_log_file_path, text_log);
  return Internal_TallyTestResults();
}

void MYONX_ModelTest::Internal_ReadTest(
  MYON_BinaryArchive& archive,
  MYONX_ModelTest::Type test_type,
  bool bKeepModels,
  const wchar_t* text_log_file_path,
  MYON_TextLog* text_log
)
{
  m_test_type = test_type;
  m_source_3dm_file_path = archive.ArchiveFullPath();
  m_text_log_3dm_file_path = text_log_file_path;

  const unsigned int current_3dm_file_version = (unsigned int)MYON_BinaryArchive::CurrentArchiveVersion();

  MYON_TextLogNull devnull;
  if (nullptr == text_log)
    text_log = &MYON_TextLog::Null;

  for (;;)
  {
    Internal_BeginNextTest(MYONX_ModelTest::Type::Read);

    if ( MYON::archive_mode::read3dm != archive.Mode() )
    {
      m_error_counts[0].IncrementFailureCount();
      text_log->Print("archive.Mode() must be MYON::archive_mode::read3dm.");
      break;
    }

    MYONX_Model* model0 = new MYONX_Model();
    std::shared_ptr<MYONX_Model> model0_sp = std::shared_ptr<MYONX_Model>(model0);
    if (bKeepModels)
      this->m_model[0] = model0_sp;

    MYON_String text_log_3dm_archive_name = TextLogSource3dmFilePath();
    if (text_log_3dm_archive_name.IsEmpty())
    {
      text_log_3dm_archive_name = "archive";
    }

    const MYON_String read0_description
      = MYON_String::FormatToString("MYONX_Model.Read(%s,...)", static_cast<const char*>(text_log_3dm_archive_name));

    // read the original file
    text_log->Print("Calling %s ...\n", static_cast<const char*>(read0_description));
    text_log->PushIndent();
    MYONX_ErrorCounter read0_error_counter;
    read0_error_counter.ClearLibraryErrorsAndWarnings();
    const bool bRead0 = model0->Read(archive, text_log);
    read0_error_counter.AddLibraryErrorsAndWarnings();
    m_model_3dm_file_version[0] = model0->m_3dm_file_version;

    text_log->PopIndent();

    if (false == bRead0)
    {
      m_error_counts[m_current_test_index].IncrementFailureCount();
      text_log->Print("%s failed.\n", static_cast<const char*>(read0_description));
      break;
    }
    else
    {
      text_log->Print("... %s ", static_cast<const char*>(read0_description));
      if ( 0 == read0_error_counter.TotalCount() )
        text_log->Print("succeeded.");
      else
      {
        text_log->Print("finished. ");
        read0_error_counter.Dump(*text_log);
      }
      text_log->PrintNewLine();

    }
    text_log->PushIndent();
    text_log->Print("Source model 3dm file version: %d", model0->m_3dm_file_version);
    text_log->PrintNewLine();
    m_model_hash[0] = model0->ContentHash();
    text_log->Print("Source model hash: ");
    m_model_hash[0].Dump(*text_log);
    text_log->PrintNewLine();
    text_log->PopIndent();

    if ( MYONX_ModelTest::Type::Read == test_type)
      break;
    Internal_EndCurrentTest();
    Internal_BeginNextTest(MYONX_ModelTest::Type::ReadWrite);

    //const unsigned int original_model_3dm_file_version = (unsigned int)(model0->m_3dm_file_version);

    // Write original_model to a termporary archive using "buffer" for storage. 
    MYON_Buffer temporary_buffer[2];
    const unsigned int temporary_buffer_3dm_version[2] = { current_3dm_file_version - 10, current_3dm_file_version };

    for (int buffer_index = 0; buffer_index < 2; buffer_index++)
    {
      MYON_BinaryArchiveBuffer temporary_archive(MYON::archive_mode::write3dm, &temporary_buffer[buffer_index]);

      const MYON_String write1_description
        = MYON_String::FormatToString(
          "MYONX_Model.Write( temporary_archive version %d, ...)",
          temporary_buffer_3dm_version[buffer_index]);

      text_log->Print("Calling %s ...\n", static_cast<const char*>(write1_description));
      text_log->PushIndent();
      MYONX_ErrorCounter write1_error_counter;
      write1_error_counter.ClearLibraryErrorsAndWarnings();
      bool bWrite1 = model0->Write(temporary_archive, temporary_buffer_3dm_version[buffer_index], text_log);
      write1_error_counter.AddLibraryErrorsAndWarnings();
      text_log->PopIndent();

      if (false == bWrite1)
      {
        m_error_counts[m_current_test_index].IncrementFailureCount();
        text_log->Print("%s failed.\n", static_cast<const char*>(write1_description));
        break;
      }
      else
      {
        text_log->Print("... %s ", static_cast<const char*>(write1_description));
        if ( 0 == write1_error_counter.TotalCount() )
          text_log->Print("succeeded.");
        else
        {
          text_log->Print("finished. ");
          write1_error_counter.Dump(*text_log);
        }
        text_log->PrintNewLine();
      }
    }

    // no longer need model0
    model0 = nullptr;

    if ( MYONX_ModelTest::Type::ReadWrite == test_type)
      break;
    Internal_EndCurrentTest();
    Internal_BeginNextTest(MYONX_ModelTest::Type::ReadWriteRead);

    // read models from the temporary archives
    for (int buffer_index = 0; buffer_index < 2; buffer_index++)
    {
      MYON_BinaryArchiveBuffer temporary_archive(MYON::archive_mode::read3dm, &temporary_buffer[buffer_index]);
      const MYON_String read1_description
        = MYON_String::FormatToString(
          "MYONX_Model.Read( temporary_archive version %d, ...)",
          temporary_buffer_3dm_version[buffer_index]);

      text_log->Print("Calling %s ...\n", static_cast<const char*>(read1_description));
      text_log->PushIndent();

      MYONX_Model* model1 = new MYONX_Model();
      std::shared_ptr<MYONX_Model> model1_sp = std::shared_ptr<MYONX_Model>(model1);
      if (bKeepModels)
        this->m_model[buffer_index+1] = model1_sp;

      MYONX_ErrorCounter read1_error_counter;
      read1_error_counter.ClearLibraryErrorsAndWarnings();
      const bool bRead1 = model1->Read(temporary_archive, text_log);
      read1_error_counter.AddLibraryErrorsAndWarnings();      
      m_model_3dm_file_version[buffer_index + 1] = model1->m_3dm_file_version;

      text_log->PopIndent();

      if (false == bRead1)
      {
        m_error_counts[m_current_test_index].IncrementFailureCount();
        text_log->Print("%s failed.\n", static_cast<const char*>(read1_description));
        break;
      }
      else
      {
        text_log->Print("... %s ", static_cast<const char*>(read1_description));
        if ( 0 == read1_error_counter.TotalCount())
          text_log->Print("succeeded.");
        else
        {
          text_log->Print("finished. ");
          read1_error_counter.Dump(*text_log);
        }
        text_log->PrintNewLine();
      }
      text_log->PushIndent();
      text_log->Print("Temporary model %d 3dm file version: %d",buffer_index+1,model1->m_3dm_file_version);
      text_log->PrintNewLine();
      m_model_hash[buffer_index+1] = model1->ContentHash();
      text_log->Print("Temporary model %d hash: ",buffer_index+1);
      m_model_hash[buffer_index+1].Dump(*text_log);
      text_log->PrintNewLine();
      text_log->PopIndent();
    }

    if ( MYONX_ModelTest::Type::ReadWrite == test_type)
      break;
    Internal_EndCurrentTest();
    Internal_BeginNextTest(MYONX_ModelTest::Type::ReadWriteReadCompare);

    bool bSkippedCompare = true;
    for (int buffer_index = 0; buffer_index < 2; buffer_index++)
    {
      if (m_model_3dm_file_version[0] != m_model_3dm_file_version[buffer_index+1])
        continue;
      if (m_model_3dm_file_version[0] != temporary_buffer_3dm_version[buffer_index])
        continue;

      bSkippedCompare = false;
      if (m_model_hash[0] != m_model_hash[buffer_index+1])
      {
        m_error_counts[m_current_test_index].IncrementFailureCount();
        text_log->Print("The source model and temporary model %d are different.\n",buffer_index+1);
        break;
      }
      else
      {
        text_log->Print("The source model and temporary model %d are identical.\n",buffer_index+1);
      }
    }

    unsigned int compare_test_index = m_current_test_index;
    if (bSkippedCompare)
      this->m_test_results[compare_test_index] = MYONX_ModelTest::Result::Skip;
            
    break;
  }

  Internal_EndCurrentTest();
}

/*  +----------------------------------------------------------------------------+
    |                                                                            |
    |                   RDK version 4 document archive format                    |
    |                                                                            |
    +================+==========================+================================+
    | Type           | Usage                    | MYON_BinaryArchive function      |
    +================+==========================+================================+
    | MYON__INT32      | RDK document version     | ReadInt()  / WriteInt()        |
    +----------------+--------------------------+--------------------------------+
    | MYON__INT32      | Length of UTF8 XML block | ReadInt()  / WriteInt()        |
    +----------------+--------------------------+--------------------------------+
    | UTF8 chars     | Entire RDK document XML  | ReadChar() / WriteChar()       |
    +----------------+--------------------------+--------------------------------+
    | MYON__INT32      | Number of embedded files | ReadInt()  / WriteInt()        |
    +----------------+--------------------------+--------------------------------+

     If the number of embedded files is not zero, then for each embedded file:

    +----------------+--------------------------+--------------------------------+
    | String         | Full path to file        | ReadString() / WriteString()   |
    +----------------+--------------------------+--------------------------------+
    | Bytes          | Compressed embedded file | Read / WriteCompressedBuffer() |
    +----------------+--------------------------+--------------------------------+
*/

static const wchar_t* RenderContentKindString(RenderContentKinds kind)
{
  switch (kind)
  {
  case RenderContentKinds::Material:    return MYON_KIND_MATERIAL;
  case RenderContentKinds::Environment: return MYON_KIND_ENVIRONMENT;
  case RenderContentKinds::Texture:     return MYON_KIND_TEXTURE;
  }

  MYON_ASSERT(false);
  return L"";
}

static const wchar_t* PostEffectTypeString(MYON_PostEffect::Types type)
{
  switch (type)
  {
    case MYON_PostEffect::Types::Early:
      return MYON_RDK_PEP_TYPE_EARLY;
    case MYON_PostEffect::Types::ToneMapping:
      return MYON_RDK_PEP_TYPE_TONE;
    case MYON_PostEffect::Types::Late:
      return MYON_RDK_PEP_TYPE_LATE;
    case MYON_PostEffect::Types::Unset:
      break;
  }

  MYON_ASSERT(false);
  return L"";
}

extern int MYON_ComponentManifestImpl_TableCount(void);

MYONX_ModelPrivate::MYONX_ModelPrivate(MYONX_Model& m)
  :
  m_model(m)
{
  // If this assert fires, you must change the TableCount enum in opennurbs_archive_manifest.cpp
  // to be the same number as MYON_ModelComponent::Type::NumOf.
  MYON_ASSERT(int(MYON_ModelComponent::Type::NumOf) == MYON_ComponentManifestImpl_TableCount());

  for (unsigned int i = 0; i < int(MYON_ModelComponent::Type::NumOf); i++)
  {
    MYONX_Model::MYONX_ModelComponentList& list = m_mcr_lists.AppendNew();
    list.m_component_type = MYON_ModelComponent::ComponentTypeFromUnsigned(i);
  }
}

MYONX_ModelPrivate::~MYONX_ModelPrivate()
{
}

MYONX_Model_UserData* MYONX_ModelPrivate::GetRDKDocumentUserData(int archive_3dm_version) const
{
  // Try to find existing RDK document user data.
  for (int i = 0; i < m_model.m_userdata_table.Count(); i++)
  {
    auto* pUserData = m_model.m_userdata_table[i];
    if (nullptr != pUserData)
    {
      if (::IsRDKDocumentInformation(*pUserData))
        return pUserData; // Found it.
    }
  }

  // Not found, so create it.
  auto* ud = new MYONX_Model_UserData;
  ud->m_goo.m_typecode = TCODE_USER_RECORD;
  ud->m_uuid = RdkPlugInId();
  ud->m_usertable_3dm_version = archive_3dm_version;
  ud->m_usertable_opennurbs_version = MYON::Version();

  MYON_XMLRootNode root;
  PopulateDefaultRDKDocumentXML(root);
  SetRDKDocumentInformation(root.String(), *ud, archive_3dm_version);

  m_model.m_userdata_table.Append(ud);

  return ud;
}

void MYONX_ModelPrivate::PopulateDefaultRDKDocumentXML(MYON_XMLRootNode& root) const
{
  // Populate default render content kinds.
  GetRenderContentSectionNode(root, RenderContentKinds::Material);
  GetRenderContentSectionNode(root, RenderContentKinds::Environment);
  GetRenderContentSectionNode(root, RenderContentKinds::Texture);
}

bool MYONX_ModelPrivate::GetRDKDocumentXML(MYON_wString& xml, bool embedded_files, int archive_3dm_version) const
{
  // Gets the entire RDK document XML as a string in 'xml'. If 'embedded_files' is true,
  // MYON_EmbeddedFile objects are created for each embedded file.

  const MYONX_Model_UserData* pUserData = GetRDKDocumentUserData(archive_3dm_version);
  if (nullptr != pUserData)
  {
    MYONX_Model* model = embedded_files ? &m_model : nullptr;
    if (GetEntireRDKDocument(*pUserData, xml, model))
      return true;
  }

  return false;
}

static bool ContentIsKind(const MYON_RenderContent* pContent, RenderContentKinds kind)
{
  switch (kind)
  {
  case RenderContentKinds::Material:    return nullptr != MYON_RenderMaterial   ::Cast(pContent);
  case RenderContentKinds::Environment: return nullptr != MYON_RenderEnvironment::Cast(pContent);
  case RenderContentKinds::Texture:     return nullptr != MYON_RenderTexture    ::Cast(pContent);
  }

  return false;
}

MYON_XMLNode* MYONX_ModelPrivate::GetPostEffectSectionNode(MYON_XMLNode& docNode, MYON_PostEffect::Types type) const
{
  MYON_wString s = MYON_RDK_DOCUMENT  MYON_RDK_SLASH  MYON_RDK_SETTINGS  MYON_RDK_SLASH  MYON_RDK_POST_EFFECTS  MYON_RDK_SLASH;
  s += PostEffectTypeString(type);

  return docNode.CreateNodeAtPath(s);
}

MYON_XMLNode* MYONX_ModelPrivate::GetRenderContentSectionNode(MYON_XMLNode& docNode, RenderContentKinds kind) const
{
  MYON_wString s = MYON_RDK_DOCUMENT  MYON_RDK_SLASH;
  s += RenderContentKindString(kind);
  s += MYON_RDK_POSTFIX_SECTION;

  return docNode.CreateNodeAtPath(s);
}

bool MYONX_ModelPrivate::CreateRenderContentFromXML(MYON_XMLNode& model_node, RenderContentKinds kind)
{
  const MYON_XMLNode* rc_section_node = GetRenderContentSectionNode(model_node, kind);
  if (nullptr == rc_section_node)
    return false;

  auto it = rc_section_node->GetChildIterator();

  const MYON_XMLNode* rc_node = it.GetNextChild();
  while (nullptr != rc_node)
  {
    MYON_RenderContent* rc = NewRenderContentFromNode(*rc_node);
    if (nullptr != rc)
    {
      const auto ref = m_model.AddModelComponent(*rc);
      auto* model_rc = MYON_RenderContent::Cast(ref.ModelComponent());
      if (nullptr != model_rc)
      {
        SetModel(*model_rc, m_model);
      }

      delete rc;
    }

    rc_node = it.GetNextChild();
  }

  return true;
}

bool MYONX_ModelPrivate::CreateXMLFromRenderContent(MYON_XMLNode& model_node, RenderContentKinds kind) const
{
  MYON_XMLNode* rc_section_node = GetRenderContentSectionNode(model_node, kind);
  if (nullptr == rc_section_node)
    return false;

  rc_section_node->RemoveAllChildren();

  MYONX_ModelComponentIterator it(m_model, MYON_ModelComponent::Type::RenderContent);
  const MYON_ModelComponent* component = it.FirstComponent();
  while (nullptr != component)
  {
    const auto* rc = MYON_RenderContent::Cast(component);
    if (nullptr != rc)
    {
      if (ContentIsKind(rc, kind))
      {
        SetRenderContentNodeRecursive(*rc, *rc_section_node);
      }
    }

    component = it.NextComponent();
  }

  return true;
}

bool MYONX_ModelPrivate::CreatePostEffectsFromXML(MYON_XMLNode& doc_root_node, MYON_PostEffect::Types type)
{
  MYON_XMLNode* pep_section_node = GetPostEffectSectionNode(doc_root_node, type);
  if (nullptr == pep_section_node)
    return false;

  auto it = pep_section_node->GetChildIterator();

  MYON_XMLNode* pPostEffectNode = it.GetNextChild();
  while (nullptr != pPostEffectNode)
  {
    MYON_PostEffect pep(*pPostEffectNode, type);
    const MYON_ModelComponentReference ref = m_model.AddModelComponent(pep);
    auto* pep_in_model = MYON_PostEffect::Cast(ref.ModelComponent());
    if (nullptr != pep_in_model)
    {
      SetModel(*pep_in_model, m_model);
    }

    pPostEffectNode = it.GetNextChild();
  }

  return true;
}

bool MYONX_ModelPrivate::CreateXMLFromPostEffects(MYON_XMLNode& doc_root_node, MYON_PostEffect::Types type) const
{
  MYON_XMLNode* pep_section_node = GetPostEffectSectionNode(doc_root_node, type);
  if (nullptr == pep_section_node)
    return false;

  MYONX_ModelComponentIterator it(m_model, MYON_ModelComponent::Type::PostEffect);
  const MYON_ModelComponent* pComponent = it.FirstComponent();
  while (nullptr != pComponent)
  {
    const auto* pep = MYON_PostEffect::Cast(pComponent);
    if ((nullptr != pep) && (pep->Type() == type))
    {
      MYON_XMLNode* pep_node = FindPostEffectNodeForId(*pep_section_node, pep->Id());
      if (nullptr != pep_node)
      {
        *pep_node = pep->XML();
      }
    }

    pComponent = it.NextComponent();
  }

  return true;
}

bool MYONX_ModelPrivate::PopulateRDKComponents(int archive_3dm_version)
{
  // Get the entire RDK document XML. This includes not only render contents
  // but also Sun, GroundPlane and other RDK document data. Ignore embedded files.
  MYON_wString xml;
  if (!GetRDKDocumentXML(xml, true, archive_3dm_version))
    return false;

  // Read the entire XML into the document node.
  MYON_XMLNode& doc_node = m_model.m_settings.m_RenderSettings.RdkDocNode();
  const auto read = doc_node.ReadFromStream(xml, false, true);
  if (MYON_XMLNode::ReadError == read)
    return false;

  // Create the render contents from the relevant nodes.
  CreateRenderContentFromXML(doc_node, RenderContentKinds::Material);
  CreateRenderContentFromXML(doc_node, RenderContentKinds::Environment);
  CreateRenderContentFromXML(doc_node, RenderContentKinds::Texture);

  // Create the post effects from the relevant nodes.
  CreatePostEffectsFromXML(doc_node, MYON_PostEffect::Types::Early);
  CreatePostEffectsFromXML(doc_node, MYON_PostEffect::Types::ToneMapping);
  CreatePostEffectsFromXML(doc_node, MYON_PostEffect::Types::Late);

  // Create the mesh modifiers.
  CreateMeshModifiersFromXML(m_model, archive_3dm_version);

  return true;
}

bool MYONX_ModelPrivate::UpdateRDKUserData(int archive_3dm_version)
{
  if (0 == archive_3dm_version)
    archive_3dm_version = MYON_BinaryArchive::CurrentArchiveVersion();

  MYON_XMLNode& doc_node = m_model.m_settings.m_RenderSettings.RdkDocNode();

  // For each kind, convert the render content hierarchy to fresh XML.
  CreateXMLFromRenderContent(doc_node, RenderContentKinds::Material);
  CreateXMLFromRenderContent(doc_node, RenderContentKinds::Environment);
  CreateXMLFromRenderContent(doc_node, RenderContentKinds::Texture);

  // For each type, convert the post effects to fresh XML.
  CreateXMLFromPostEffects(doc_node, MYON_PostEffect::Types::Early);
  CreateXMLFromPostEffects(doc_node, MYON_PostEffect::Types::ToneMapping);
  CreateXMLFromPostEffects(doc_node, MYON_PostEffect::Types::Late);

  // Convert the mesh modifier collection to fresh XML.
  CreateXMLFromMeshModifiers(m_model, archive_3dm_version);

  // Get the RDK document user data.
  MYONX_Model_UserData* pUserData = GetRDKDocumentUserData(archive_3dm_version);
  if (nullptr == pUserData)
    return false; // Shouldn't happen because we were able to get the XML earlier.

  // Get the entire document XML as a string and set it to the user data.
  MYON_wString xml = doc_node.String();
  pUserData->m_usertable_3dm_version = archive_3dm_version;
  SetRDKDocumentInformation(xml, *pUserData, archive_3dm_version);

  return true;
}

bool IsRDKDocumentInformation(const MYONX_Model_UserData& docud)
{
  return (0 == MYON_UuidCompare(RdkPlugInId(), docud.m_uuid)) && (docud.m_goo.m_value >= 4) && (nullptr != docud.m_goo.m_goo);
}

bool MYONX_Model::IsRDKDocumentInformation(const MYONX_Model_UserData& docud)
{
  return ::IsRDKDocumentInformation(docud);
}

static size_t ArchiveLengthUpToEmbeddedFiles(size_t utf8_length)
{
  //                    version             utf8_length         utf8_buf
  const size_t length = sizeof(MYON__INT32) + sizeof(MYON__INT32) + utf8_length;
  MYON_ASSERT(length <= INT_MAX);

  return length;
}

bool MYONX_Model::GetRDKEmbeddedFiles(const MYONX_Model_UserData& docud, MYON_ClassArray<MYON_wString>& paths, MYON_SimpleArray<unsigned char*>& embedded_files_as_buffers)
{
  MYON_SimpleArray<size_t> dummy;
  return ::GetRDKEmbeddedFiles(docud, paths, embedded_files_as_buffers, dummy);
}

bool MYONX_Model::GetRDKEmbeddedFiles(const MYONX_Model_UserData& docud, MYON_ClassArray<MYON_wString>& paths, MYON_SimpleArray<unsigned char*>& embedded_files_as_buffers, MYON_SimpleArray<size_t>& buffer_sizes) // Static.
{
  return ::GetRDKEmbeddedFiles(docud, paths, embedded_files_as_buffers, buffer_sizes);
}

bool MYONX_Model::GetRDKDocumentInformation(const MYONX_Model_UserData& docud, MYON_wString& xml) // Static.
{
  return MYONX_ModelPrivate::GetEntireRDKDocument(docud, xml, nullptr);
}

static int SeekArchiveToEmbeddedFiles(MYON_Read3dmBufferArchive& archive, int goo_length)
{
  // Skips over the bulk of the archive to arrive at the embedded files.
  // Then reads the number of embedded files and returns it. The archive
  // is then ready to read the first embedded file.

  if (!archive.ReadMode())
    return 0;

  // Read the version number. Must be 4.
  int version = 0;
  if (!archive.ReadInt(&version) || (4 != version))
    return 0;

  // Read the UTF8 data length.
  int utf8_length = 0;
  if (!archive.ReadInt(&utf8_length))
    return 0;

  // Validate the length.
  if (utf8_length <= 0)
    return 0;

  const auto length_so_far = ArchiveLengthUpToEmbeddedFiles(utf8_length);
  if (length_so_far > size_t(goo_length))
    return 0; // Sanity check.

  // Seek past the UTF8 data.
  if (!archive.SeekForward(utf8_length))
    return 0;

  // Read the number of embedded files.
  int num_embedded_files = 0;
  if (!archive.ReadInt(&num_embedded_files))
    return 0;

  return num_embedded_files;
}

static bool SeekArchivePastCompressedBuffer(MYON_BinaryArchive& archive)
{
  // WARNING: This function has intimate knowledge of how MYON_BinaryArchive::WriteCompressedBuffer() works.
  // It is my opinion that this function should really be a method on MYON_BinaryArchive since only that
  // class should know how its own implementation works. JohnC, 2022AD.

  if (!archive.ReadMode())
    return false;

  bool rc = false;
  unsigned int buffer_crc0 = 0;
  char method = 0;

  size_t sizeof__outbuffer;
  if (!archive.ReadCompressedBufferSize(&sizeof__outbuffer))
    return false;

  if (0 == sizeof__outbuffer)
    return true;

  if (!archive.ReadInt(&buffer_crc0)) // 32 bit crc of uncompressed buffer
    return false;

  if (!archive.ReadChar(&method))
    return false;

  if (method != 0 && method != 1)
    return false;

  switch (method)
  {
  case 0: // uncompressed
    rc = archive.SeekForward(sizeof__outbuffer);
    break;

  case 1: // compressed
  {
    MYON__UINT32 tcode = 0;
    MYON__INT64  big_value = 0;
    rc = archive.BeginRead3dmBigChunk(&tcode, &big_value);
    if (rc)
      rc = archive.EndRead3dmChunk();
  }
  break;
  }

  return rc;
}

static bool ReadEmbeddedFilePathsFromArchive(MYON_Read3dmBufferArchive& archive, int count, MYON_ClassArray<MYON_wString>& paths)
{
  // Reads the embedded file paths and skips over the data for 'count' embedded files. Must be called with
  // the archive position at the start of the first embedded file, right after the count of embedded files.
  if (0 == count)
    return false;

  for (int i = 0; i < count; i++)
  {
    MYON_wString sPath;
    if (!archive.ReadString(sPath))
      return false;

    paths.Append(sPath);

    SeekArchivePastCompressedBuffer(archive);
  }

  return paths.Count() > 0;
}

bool MYONX_Model::GetRDKEmbeddedFilePaths(const MYONX_Model_UserData& docud, MYON_ClassArray<MYON_wString>& paths)
{
  if (!::IsRDKDocumentInformation(docud))
    return false;

  MYON_Read3dmBufferArchive archive(docud.m_goo.m_value, docud.m_goo.m_goo, false, docud.m_usertable_3dm_version, docud.m_usertable_opennurbs_version);

  const int count = SeekArchiveToEmbeddedFiles(archive, docud.m_goo.m_value);
  if (!ReadEmbeddedFilePathsFromArchive(archive, count, paths))
    return false;

  return true;
}

bool GetRDKEmbeddedFiles(const MYONX_Model_UserData& docud, MYON_ClassArray<MYON_wString>& paths, MYON_SimpleArray<unsigned char*>& embedded_files_as_buffers, MYON_SimpleArray<size_t>& buffer_sizes)
{
  if (!::IsRDKDocumentInformation(docud))
    return false;

  MYON_Read3dmBufferArchive a(docud.m_goo.m_value, docud.m_goo.m_goo, false, docud.m_usertable_3dm_version, docud.m_usertable_opennurbs_version);

  const int count = SeekArchiveToEmbeddedFiles(a, docud.m_goo.m_value);
  if (0 == count)
    return false;

  int unpacked = 0;

  for (int i = 0; i < count; i++)
  {
    MYON_wString sPath;
    if (!a.ReadString(sPath))
      return false;

    size_t size;
    if (!a.ReadCompressedBufferSize(&size))
      return false;

    auto* buffer = new unsigned char[size];
    bool bFailedCRC = false;
    if (a.ReadCompressedBuffer(size, buffer, &bFailedCRC))
    {
      if (!bFailedCRC)
      {
        embedded_files_as_buffers.Append(buffer);
        paths.Append(sPath);
        buffer_sizes.Append(size);
        unpacked++;
      }
      else
      {
        delete[] buffer;
      }
    }
  }

  return unpacked > 0;
}

bool MYONX_Model::GetRDKEmbeddedFile(const MYONX_Model_UserData& docud, const wchar_t* path, MYON_SimpleArray<unsigned char>& bytes)
{
  if (!::IsRDKDocumentInformation(docud))
    return false;

  MYON_Read3dmBufferArchive archive(docud.m_goo.m_value, docud.m_goo.m_goo, false, docud.m_usertable_3dm_version, docud.m_usertable_opennurbs_version);

  const auto count = SeekArchiveToEmbeddedFiles(archive, docud.m_goo.m_value);
  if (0 == count)
    return false;

  bool found = false;
  for (int i = 0; i < count; i++)
  {
    MYON_wString sPath;
    if (!archive.ReadString(sPath))
      break;

    if (0 == sPath.ComparePath(path))
    {
      size_t size;
      if (!archive.ReadCompressedBufferSize(&size))
        break;

      bytes.Destroy();
      bytes.Reserve(size);

      bool bFailedCRC = false;
      bool bRet = archive.ReadCompressedBuffer(size, bytes.Array(), &bFailedCRC);

      if (!bRet || bFailedCRC)
        break;

      bytes.SetCount((int)size);

      found = true;
      break;
    }
    else
    {
      SeekArchivePastCompressedBuffer(archive);
    }
  }

  return found;
}

void MYONX_ModelPrivate::RemoveAllEmbeddedFiles(MYONX_Model& model)
{
  MYON_SimpleArray<MYON_UUID> a;
  const auto type = MYON_ModelComponent::Type::EmbeddedFile;

  MYONX_ModelComponentIterator it(model, type);
  const MYON_ModelComponent* pComponent = it.FirstComponent();
  while (nullptr != pComponent)
  {
    a.Append(pComponent->Id());

    pComponent = it.NextComponent();
  }

  for (int i = 0; i < a.Count(); i++)
  {
    model.RemoveModelComponent(type, a[i]);
  }
}

bool MYONX_ModelPrivate::GetEntireRDKDocument(const MYONX_Model_UserData& docud, MYON_wString& xml, MYONX_Model* model) // Static.
{
  if (!::IsRDKDocumentInformation(docud))
    return false;

  MYON_Read3dmBufferArchive archive(docud.m_goo.m_value, docud.m_goo.m_goo, false, docud.m_usertable_3dm_version, docud.m_usertable_opennurbs_version);

  int version = 0;
  if (!archive.ReadInt(&version))
    return false;

  if (1 == version)
  {
    // Version 1 was a UTF-16 string.
    if (!archive.ReadString(xml))
      return false;
  }
  else
  if ((3 == version) || (4 == version))
  {
    // Version 4 files are exactly the same as version 3, but with the addition of embedded files.

    // Version 3 and 4 is a UTF-8 string.
    int utf8_length = 0;
    if (!archive.ReadInt(&utf8_length))
      return false;

    if (utf8_length <= 0)
      return false;

    const size_t length_so_far = ArchiveLengthUpToEmbeddedFiles(utf8_length);
    if (length_so_far > size_t(docud.m_goo.m_value))
      return false; // Sanity check.

    MYON_String s;
    s.SetLength(utf8_length);
    if (!archive.ReadChar(utf8_length, s.Array()))
      return false;

    if (s.IsNotEmpty())
    {
      const char* sArray = s.Array();
      unsigned int error_status = 0;
      auto wideLength = MYON_ConvertUTF8ToWideChar(false, sArray, -1, 0, 0, &error_status, 0, 0, 0);
      if ((wideLength > 0) && (0 == error_status))
      {
        xml.SetLength(wideLength);
        MYON_ConvertUTF8ToWideChar(false, sArray, -1, xml.Array(), wideLength+1, &error_status, 0, 0, 0);
      }

      if (0 != error_status)
      {
        MYON_ERROR("RDK xml document settings is not a valid UTF-8 string.");
      }
    }

    if (nullptr != model)
    {
      RemoveAllEmbeddedFiles(*model);

      if (4 == version)
      {
        // Read the number of embedded files.
        int num_embedded_files = 0;
        if (!archive.ReadInt(&num_embedded_files))
          return false;

        // Create an MYON_EmbeddedFile object for each embedded file.
        for (int i = 0; i < num_embedded_files; i++)
        {
          MYON_EmbeddedFile ef;

          if (!ef.Read(archive))
            return false;

          model->AddModelComponent(ef);
        }
      }
    }
  }

  return xml.Length() > 0;
}

bool MYONX_ModelPrivate::SetRDKDocumentInformation(const wchar_t* xml, MYONX_Model_UserData& docud, int archive_3dm_version) const
{
  MYON_Write3dmBufferArchive archive(0, 0, docud.m_usertable_3dm_version, docud.m_usertable_opennurbs_version);

  // Write the version.
  int version = 4; // Where do I get this from?
  if (!archive.WriteInt(version))
    return false;

  // Convert the XML to UTF8 and write it to the archive.
  unsigned int error_status = 0;
  const int utf8_length = MYON_ConvertWideCharToUTF8(false, xml, -1, 0, 0, &error_status, 0, 0, 0);

  { // BEGIN UTF8
    auto utf8 = std::unique_ptr<char[]>(new char[utf8_length]);
    char* utf8_buf = utf8.get();

    MYON_ConvertWideCharToUTF8(false, xml, -1, utf8_buf, utf8_length, &error_status, 0, 0, 0);

    // Write the length of the UTF8 data.
    if (!archive.WriteInt(utf8_length))
      return false;

    // Write the UTF8 data.
    if (!archive.WriteChar(size_t(utf8_length), utf8_buf))
      return false;
  } // END UTF8

  const auto length_so_far = ArchiveLengthUpToEmbeddedFiles(utf8_length);
  MYON_ASSERT(archive.SizeOfArchive() == length_so_far); // Sanity check.

  // Write the number of embedded files.
  const auto num_embedded_files = int(m_model.ActiveComponentCount(MYON_ModelComponent::Type::EmbeddedFile));
  if (!archive.WriteInt(num_embedded_files))
    return false;

  // Write the embedded files to the archive.
  MYONX_ModelComponentIterator it(m_model, MYON_ModelComponent::Type::EmbeddedFile);
  const MYON_ModelComponent* pComponent = it.FirstComponent();
  while (nullptr != pComponent)
  {
    const auto* embedded_file = MYON_EmbeddedFile::Cast(pComponent);
    if (nullptr != embedded_file)
    {
      embedded_file->Write(archive);
    }

    pComponent = it.NextComponent();
  }

  // Delete the old goo.
  if (nullptr != docud.m_goo.m_goo)
    onfree(docud.m_goo.m_goo);

  // Allocate the new goo and copy the archive to it.
  const auto length_goo = archive.SizeOfArchive();
  docud.m_goo.m_goo = (unsigned char*)onmalloc(length_goo);
  docud.m_goo.m_value = int(length_goo);
  memcpy(docud.m_goo.m_goo, archive.Buffer(), length_goo);

  return true;
}

// Object user data

static MYON_UserData* RDKObjectUserDataHelper(const MYON_UserData* objectud)
{
  if (nullptr == objectud)
      return nullptr;

  if (0 == MYON_UuidCompare(objectud->m_application_uuid, RdkPlugInId()))
  {
    if (0 != MYON_UuidCompare(objectud->m_userdata_uuid, MYON_RdkUserData::Uuid()))
        return nullptr; // Not RDK user data.

    return const_cast<MYON_UserData*>(objectud); // RDK user data -- used for decals.
  }

  return nullptr;
}

static bool IsMeshModifierObjectUserData(MYON_UserData& objectud)
{
  if (0 == MYON_UuidCompare(objectud.m_application_uuid, MYON_MeshModifier::PlugInId()))
    return true; // User data from Displacement plug-in.

  return false;
}

bool MYONX_Model::IsRDKObjectInformation(const MYON_UserData& objectud) // Static.
{
  return nullptr != RDKObjectUserDataHelper(&objectud);
}

static bool CreateArchiveBufferFromXML(const MYON_wString& xml, MYON_Buffer& buf, int archive_3dm_version)
{
  const auto archive_opennurbs_version_number = MYON::Version(); // I don't know if this is correct.

  MYON_Write3dmBufferArchive archive(0, 0, archive_3dm_version, archive_opennurbs_version_number);

  int version = 2; // Not sure if this is correct.
  if (!archive.WriteInt(version))
    return false;

  const wchar_t* wsz = static_cast<const wchar_t*>(xml);

  unsigned int error_status = 0;
  const int num_chars = MYON_ConvertWideCharToUTF8(false, wsz, -1, nullptr, 0, &error_status, 0, 0, nullptr);

  auto p = std::unique_ptr<char[]>(new char[size_t(num_chars) + 1]);
  char* pBuffer = p.get();
  MYON_ConvertWideCharToUTF8(false, wsz, -1, pBuffer, num_chars + 1, &error_status, 0, 0, nullptr);

  if (0 != error_status)
  {
    MYON_ERROR("XML is not a valid UTF-8 string.");
    return false;
  }

  int len = num_chars * sizeof(char);
  if (!archive.WriteInt(len))
    return false;

  if (!archive.WriteChar(len, pBuffer))
    return false;

  buf.Write(archive.SizeOfArchive(), archive.Buffer());
  buf.SeekFromStart(0);

  return true;
}

bool SetXMLToUserData(const MYON_wString& xml, MYON_UserData& ud, int archive_3dm_version)
{
  MYON_Buffer buf;
  if (!CreateArchiveBufferFromXML(xml, buf, archive_3dm_version))
    return false;

  MYON_BinaryArchiveBuffer arc(MYON::archive_mode::read, &buf);
  ud.Read(arc);

  return true;
}

bool SetRDKObjectInformation(MYON_Object& object, const MYON_wString& xml, int archive_3dm_version)
{
  // Create a buffer from the XML.
  MYON_Buffer buf;
  if (!CreateArchiveBufferFromXML(xml, buf, archive_3dm_version))
    return false;

  const auto archive_opennurbs_version = MYON::Version(); // I don't know if this is correct.

  // Create an archive from the buffer.
  MYON_BinaryArchiveBuffer archive(MYON::archive_mode::read, &buf);
  archive.SetArchive3dmVersion(archive_3dm_version);
  MYON_SetBinaryArchiveOpenNURBSVersion(archive, archive_opennurbs_version);

  // Try to find existing user data.
  MYON_UserData* rdk_ud = nullptr;

  for (MYON_UserData* ud = object.FirstUserData(); (nullptr != ud) && (nullptr == rdk_ud); ud = ud->Next())
  {
    rdk_ud = RDKObjectUserDataHelper(ud);
  }

  if (nullptr != rdk_ud)
  {
    // Found it, so read the archive into it.
    rdk_ud->Read(archive);
  }
  else
  {
    // No user data found; create a new one and read the archive into it.
    auto* ud = new MYON_RdkUserData;
    ud->Read(archive);

    if (!object.AttachUserData(ud))
    {
      delete ud;
      return false;
    }
  }

  return true;
}

static bool GetRDKObjectInformation(const MYON_Object& object, MYON_wString& xml, int archive_3dm_version)
{
  if (0 == archive_3dm_version)
    archive_3dm_version = MYON_BinaryArchive::CurrentArchiveVersion();

  xml.SetLength(0);

  const MYON_UserData* rdk_ud = nullptr;
  const auto* ud = MYON_UserData::Cast(&object);
  if (nullptr != ud)
  {
    rdk_ud = RDKObjectUserDataHelper(ud);
  }
  else
  {
    for (ud = object.FirstUserData(); (nullptr != ud) && (nullptr == rdk_ud); ud = ud->Next())
    {
      rdk_ud = RDKObjectUserDataHelper(ud);
    }
  }

  if (nullptr == rdk_ud)
    return false;

  MYON_Buffer buf;
  MYON_BinaryArchiveBuffer arc(MYON::archive_mode::write, &buf);
  rdk_ud->Write(arc);

  const auto sizeof_buffer = buf.Size();

  auto p = std::unique_ptr<MYON__UINT8[]>(new MYON__UINT8[sizeof_buffer]);
  MYON__UINT8* buffer = p.get();
  buf.SeekFromStart(0);
  buf.Read(sizeof_buffer, buffer);

  const unsigned int archive_opennurbs_version_number = MYON::Version();
  MYON_Read3dmBufferArchive archive(sizeof_buffer, buffer, false, archive_3dm_version, archive_opennurbs_version_number);

  int version = 0;
  if (!archive.ReadInt(&version))
    return false;

  if (1 == version)
  {
    // Version 1 was a UTF-16 string.
    if (!archive.ReadString(xml))
      return false;
  }
  else
  if (2 == version)
  {
    // Version 2 is a UTF-8 string.
    int len = 0;
    if (!archive.ReadInt(&len))
      return false;

    if (len <= 0)
      return false;

    if (size_t(len + 4) > sizeof_buffer) // JohnC asks: What does the 4 signify?
      return false;

    MYON_SimpleArray<char> s;
    s.Reserve(size_t(len) + 1);
    s.SetCount(len + 1);
    s[len] = 0;

    char* sArray = s.Array();
    if (nullptr == sArray)
      return false;

    if (!archive.ReadChar(len, sArray))
      return false;

    if (0 == sArray[0])
      return false;

    unsigned int error_status = 0;
    const int num_chars = MYON_ConvertUTF8ToWideChar(false, sArray, -1, 0, 0, &error_status, 0, 0, 0);
    if ((num_chars > 0) && (0 == error_status))
    {
      xml.SetLength(size_t(num_chars) + 2);
      MYON_ConvertUTF8ToWideChar(false, sArray, -1, xml.Array(), num_chars + 1, &error_status, 0, 0, 0);
      xml.SetLength(num_chars);
    }
    else
    {
      xml.SetLength(0);
      MYON_ERROR("RDK xml object information is not a valid UTF-8 string.");
    }
  }

  return xml.Length() > 0;
}

bool GetEntireDecalXML(const MYON_3dmObjectAttributes& attr, MYON_XMLRootNode& xmlOut)
{
  // Get the entire XML off of the attributes user data. At the moment (V8) this can only contain decals.
  MYON_wString xml;
  if (!GetRDKObjectInformation(attr, xml, 0))
    return false;  // No XML on attributes.

  // Read the XML into a root node.
  if (MYON_XMLNode::ReadError == xmlOut.ReadFromStream(xml))
    return false; // Failed to read XML.

  return true;
}

static bool GetMeshModifierUserDataXML(MYON_UserData& ud, MYON_wString& xml, int archive_3dm_version)
{
  MYON_Buffer buf;
  MYON_BinaryArchiveBuffer arc(MYON::archive_mode::write, &buf);
  ud.Write(arc);

  const MYON__UINT64 sizeof_buffer = buf.Size();

  auto p = std::unique_ptr<MYON__UINT8[]>(new MYON__UINT8[sizeof_buffer]);
  MYON__UINT8* buffer = p.get();
  buf.SeekFromStart(0);
  buf.Read(sizeof_buffer, buffer);

  const auto archive_opennurbs_version_number = MYON::Version();
  MYON_Read3dmBufferArchive archive(sizeof_buffer, buffer, false, archive_3dm_version, archive_opennurbs_version_number);

  int version = 0;
  if (!archive.ReadInt(&version))
    return false;

  if (1 == version)
  {
    // Version 1 was a UTF-16 string.
    if (!archive.ReadString(xml))
      return false;
  }
  else
  if (2 == version)
  {
    // Version 2 is a UTF-8 string.
    int len = 0;
    if (!archive.ReadInt(&len))
      return false;

    if (len <= 0)
      return false;

    if (size_t(len + 4) > sizeof_buffer) // JohnC asks: What does the 4 signify?
      return false;

    MYON_SimpleArray<char> s;
    s.Reserve(size_t(len) + 1);
    s.SetCount(len + 1);
    s[len] = 0;

    char* sArray = s.Array();
    if (nullptr == sArray)
      return false;

    if (!archive.ReadChar(len, sArray))
      return false;

    if (0 == sArray[0])
      return false;

    unsigned int error_status = 0;
    const int num_chars = MYON_ConvertUTF8ToWideChar(false, sArray, -1, 0, 0, &error_status, 0, 0, 0);
    if ((num_chars > 0) && (0 == error_status))
    {
      xml.SetLength(size_t(num_chars) + 2);
      MYON_ConvertUTF8ToWideChar(false, sArray, -1, xml.Array(), num_chars + 1, &error_status, 0, 0, 0);
      xml.SetLength(num_chars);
    }
    else
    {
      xml.SetLength(0);
      MYON_ERROR("Mesh modifier xml object information is not a valid UTF-8 string.");
    }
  }

  return true;
}

bool MYONX_Model::GetRDKObjectInformation(const MYON_Object& object, MYON_wString& xml) // Static.
{
  // Deprecated; only for backward compatibility.
  return ::GetRDKObjectInformation(object, xml, MYON_BinaryArchive::CurrentArchiveVersion());
}

bool GetMeshModifierObjectInformation(const MYON_Object& object, MYON_wString& xml, int archive_3dm_version)
{
  xml = L"";

  // The mesh modifiers are stored in separate user data items. We must get each one's
  // XML and combine it into a single XML node containing the entire information for all
  // mesh modifiers on this object.

  MYON_XMLRootNode entire;

  MYON_UserData* ud = object.FirstUserData();
  while (nullptr != ud)
  {
    if (IsMeshModifierObjectUserData(*ud))
    {
      MYON_wString ud_xml;
      GetMeshModifierUserDataXML(*ud, ud_xml, archive_3dm_version);

      MYON_XMLRootNode root;
      root.ReadFromStream(ud_xml);
      MYON_XMLNode* mm_node = root.FirstChild();
      if (nullptr != mm_node)
      {
        root.DetachChild(*mm_node);
        entire.AttachChildNode(mm_node);
      }
    }

    ud = ud->Next();
  }

  if (entire.FirstChild() == nullptr)
    return false;

  // Return the entire XML for all mesh modifiers on this object.
  xml = entire.String();

  return true;
}

static MYON_UserData* GetMeshModifierUserData(MYON_Object& object, const MYON_UUID& uuid_mm)
{
  MYON_UserData* existing_ud = object.FirstUserData();
  while (nullptr != existing_ud)
  {
    if (existing_ud->m_userdata_uuid == uuid_mm)
      return existing_ud;

    existing_ud = existing_ud->Next();
  }

  // Not found so create it.
  MYON_XMLUserData* new_ud = nullptr;

       if (uuid_mm == MYON_DisplacementUserData::Uuid())  new_ud = new MYON_DisplacementUserData;
  else if (uuid_mm == MYON_EdgeSofteningUserData::Uuid()) new_ud = new MYON_EdgeSofteningUserData;
  else if (uuid_mm == MYON_ThickeningUserData::Uuid())    new_ud = new MYON_ThickeningUserData;
  else if (uuid_mm == MYON_CurvePipingUserData::Uuid())   new_ud = new MYON_CurvePipingUserData;
  else if (uuid_mm == MYON_ShutLiningUserData::Uuid())    new_ud = new MYON_ShutLiningUserData;
  else MYON_ASSERT(false);

  if (nullptr != new_ud)
  {
    new_ud->SetToDefaults(); // This doesn't work because the XML gets overwritten by the cached XML.
    // In fact, having cached XML in this and decals is the reason why the systems are wrong.
    // NO! LIGHT BULB MOMENT says that's not why it's wrong. Something else is wrong here.
    // I'm about to fix the decals to not have cached XML and directly use the user data. NO! After
    // that I'll do the same for mesh modifiers. NO! Then, this should work because there is no longer
    // an XML cache to overwrite the new defaults in the user data's XML. NO!

    if (!object.AttachUserData(new_ud))
    {
      delete new_ud;
      new_ud = nullptr;
    }
  }

  return new_ud;
}

void SetMeshModifierObjectInformation(MYON_Object& object, const MYON_MeshModifier* mm, int archive_3dm_version)
{
  if (nullptr == mm)
    return; // Can't create user data for non-existent mesh modifiers.

  MYON_XMLRootNode root;
  mm->AddChildXML(root);

  MYON_UserData* ud = GetMeshModifierUserData(object, mm->Uuid());
  if (nullptr != ud)
  {
    SetXMLToUserData(root.String(), *ud, archive_3dm_version);
  }
}

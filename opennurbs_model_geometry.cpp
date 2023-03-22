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

#if !defined(MYON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// MYON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, MYON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error MYON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

MYON_OBJECT_IMPLEMENT(MYON_ModelGeometryComponent,MYON_ModelComponent,"29D1B827-41CE-45C1-B265-0686AA391DAE");

const MYON_ModelGeometryComponent* MYON_ModelGeometryComponent::FromModelComponentRef(
  const class MYON_ModelComponentReference& model_component_reference,
  const MYON_ModelGeometryComponent* none_return_value
  )
{
  const MYON_ModelGeometryComponent* p = MYON_ModelGeometryComponent::Cast(model_component_reference.ModelComponent());
  return (nullptr != p) ? p : none_return_value;
}

bool MYON_ModelGeometryComponent::UpdateReferencedComponents(
  const class MYON_ComponentManifest& source_manifest,
  const class MYON_ComponentManifest& destination_manifest,
  const class MYON_ManifestMap& manifest_map
  )
{
  bool bGeometryUpdated = false;
  for (;;)
  {
    MYON_Object* geometry = m_geometry_sp.get();
    if (nullptr == geometry)
    {
      bGeometryUpdated = true;
      break;
    }
    bGeometryUpdated = geometry->UpdateReferencedComponents(source_manifest,destination_manifest,manifest_map);
    break;
  }

  bool bAttributesUpdated = false;
  for (;;)
  {
    MYON_3dmObjectAttributes* attributes = m_attributes_sp.get();
    if (nullptr == attributes)
    {
      bAttributesUpdated = true;
      break;
    }
    if (&MYON_3dmObjectAttributes::Unset == attributes)
      return false;
    if (&MYON_3dmObjectAttributes::DefaultAttributes == attributes)
      return false;

    bAttributesUpdated = attributes->UpdateReferencedComponents(source_manifest,destination_manifest,manifest_map);
    break;
  }

  return bGeometryUpdated && bAttributesUpdated;
}


static MYON_ModelComponent::Type Internal_MYON_ModelGeometry_TypeFilter(MYON_ModelComponent::Type type)
{
  switch (type)
  {
  case MYON_ModelComponent::Type::Unset:
    return type;
    break;
  case MYON_ModelComponent::Type::Image:
    break;
  case MYON_ModelComponent::Type::TextureMapping:
    break;
  case MYON_ModelComponent::Type::RenderMaterial:
    break;
  case MYON_ModelComponent::Type::LinePattern:
    break;
  case MYON_ModelComponent::Type::Layer:
    break;
  case MYON_ModelComponent::Type::Group:
    break;
  case MYON_ModelComponent::Type::TextStyle:
    break;
  case MYON_ModelComponent::Type::DimStyle:
    break;
  case MYON_ModelComponent::Type::RenderLight:
    return type;
    break;
  case MYON_ModelComponent::Type::HatchPattern:
    break;
  case MYON_ModelComponent::Type::InstanceDefinition:
    break;
  case MYON_ModelComponent::Type::ModelGeometry:
    return type;
    break;
  case MYON_ModelComponent::Type::HistoryRecord:
    break;
  case MYON_ModelComponent::Type::Mixed:
    break;
  default:
    break;
  }

  MYON_ERROR("Invalid MYON_ModelComponent::Type for MYON_ModelGeometryComponent.");
  return MYON_ModelComponent::Type::Unset;
}

MYON_ModelGeometryComponent::MYON_ModelGeometryComponent() MYON_NOEXCEPT
  : MYON_ModelComponent(Internal_MYON_ModelGeometry_TypeFilter(MYON_ModelComponent::Type::Unset))
{}

MYON_ModelGeometryComponent::MYON_ModelGeometryComponent( MYON_ModelComponent::Type type ) MYON_NOEXCEPT
  : MYON_ModelComponent(Internal_MYON_ModelGeometry_TypeFilter(type))
{}

MYON_ModelGeometryComponent::~MYON_ModelGeometryComponent()
{
  // This destructor is explictily implemented to insure m_geometry_sp 
  // and m_attributes_sp are destroyed by the same c-runtime
  // that creates them.
}

MYON_ModelGeometryComponent::MYON_ModelGeometryComponent(const MYON_ModelGeometryComponent& src) 
  : MYON_ModelComponent(Internal_MYON_ModelGeometry_TypeFilter(src.ComponentType()), src)
  , m_geometry_sp(src.m_geometry_sp)
  , m_attributes_sp(src.m_attributes_sp)
{}

MYON_ModelGeometryComponent& MYON_ModelGeometryComponent::operator=(const MYON_ModelGeometryComponent& src)
{
  if ( this != &src )
  {
    MYON_ModelComponent::operator=(src);
    m_geometry_sp.reset();
    m_geometry_sp = src.m_geometry_sp;
    m_attributes_sp.reset();
    m_attributes_sp = src.m_attributes_sp;
    SetComponentType(Internal_MYON_ModelGeometry_TypeFilter(src.ComponentType()));
  }
  return *this;
}

MYON_ModelGeometryComponent* MYON_ModelGeometryComponent::CreateForExperts(
  bool bManageGeometry,
  MYON_Object* geometry_object,
  bool bManageAttributes,
  MYON_3dmObjectAttributes* attributes,
  MYON_ModelGeometryComponent* model_geometry_component
  )
{
  MYON_ModelComponent::Type component_type;
  MYON_Geometry* geometry = MYON_Geometry::Cast(geometry_object);
  MYON_Light* light = MYON_Light::Cast(geometry);

  if (nullptr != light)
    component_type = MYON_ModelComponent::Type::RenderLight;
  else if (nullptr != geometry)
    component_type = MYON_ModelComponent::Type::ModelGeometry;
  else
    component_type = MYON_ModelComponent::Type::Unset;

  if (nullptr == attributes)
  {
    bManageAttributes = true;
    attributes = new MYON_3dmObjectAttributes();
    if (nullptr != light)
    {
      attributes->m_uuid = light->m_light_id;
      attributes->m_name = light->m_light_name;
    }
  }

  if ( MYON_nil_uuid == attributes->m_uuid )
    attributes->m_uuid = MYON_CreateId();

  if (nullptr != light)
  {
    light->m_light_id = attributes->m_uuid;
    light->m_light_name = attributes->m_name;
  }

  if ( nullptr == model_geometry_component)
    model_geometry_component = new MYON_ModelGeometryComponent(component_type);

  model_geometry_component->m_geometry_sp
    = bManageGeometry
    ? MYON_MANAGED_SHARED_PTR(MYON_Geometry,geometry)
    : MYON_UNMANAGED_SHARED_PTR(MYON_Geometry,geometry);


  model_geometry_component->m_attributes_sp
    = bManageAttributes
    ? MYON_MANAGED_SHARED_PTR(MYON_3dmObjectAttributes,attributes)
    : MYON_UNMANAGED_SHARED_PTR(MYON_3dmObjectAttributes,attributes);

  model_geometry_component->SetId(attributes->m_uuid);
  if ( attributes->m_name.IsNotEmpty() )
    model_geometry_component->SetName(attributes->m_name);

  return model_geometry_component;
}

MYON_ModelGeometryComponent* MYON_ModelGeometryComponent::Create(
  const class MYON_Object& geometry_object,
  const class MYON_3dmObjectAttributes* attributes,
  MYON_ModelGeometryComponent* model_geometry_component
  )
{
  const bool bManageGeometry = true;
  MYON_Object* managed_geometry_object = geometry_object.Duplicate();
  const bool bManageAttributes = (nullptr != attributes);
  MYON_3dmObjectAttributes* managed_attributes = (nullptr != attributes) ? attributes->Duplicate() : nullptr;
  return MYON_ModelGeometryComponent::CreateForExperts(bManageGeometry,managed_geometry_object,bManageAttributes,managed_attributes,model_geometry_component);
}

MYON_ModelGeometryComponent* MYON_ModelGeometryComponent::CreateManaged(
  MYON_Object* geometry_object,
  MYON_3dmObjectAttributes* object_attributes,
  MYON_ModelGeometryComponent* model_geometry_component
  )
{
  bool bManageGeometry = true;
  bool bManageAttributes = true;
  return MYON_ModelGeometryComponent::CreateForExperts(bManageGeometry,geometry_object,bManageAttributes,object_attributes,model_geometry_component);
}

#if defined(MYON_HAS_RVALUEREF)
MYON_ModelGeometryComponent::MYON_ModelGeometryComponent( MYON_ModelGeometryComponent&& src) MYON_NOEXCEPT
  : MYON_ModelComponent(std::move(src))
  , m_geometry_sp(std::move(src.m_geometry_sp))
  , m_attributes_sp(std::move(src.m_attributes_sp))
{}

MYON_ModelGeometryComponent& MYON_ModelGeometryComponent::operator=(MYON_ModelGeometryComponent&& src)
{
  if ( this != &src )
  {
    m_geometry_sp.reset();
    m_attributes_sp.reset();
    MYON_ModelComponent::operator=(std::move(src));
    m_geometry_sp = std::move(src.m_geometry_sp);
    m_attributes_sp = std::move(src.m_attributes_sp);
  }
  return *this;
}
#endif

bool MYON_ModelGeometryComponent::IsEmpty() const
{
  return (nullptr == m_geometry_sp.get());
}

bool MYON_ModelGeometryComponent::IsInstanceDefinitionGeometry() const
{
  if (nullptr != m_geometry_sp.get())
  {
    const MYON_3dmObjectAttributes* attributes = m_attributes_sp.get();
    return (nullptr != attributes && attributes->IsInstanceDefinitionObject() );
  }
  return false;
}


const MYON_Geometry* MYON_ModelGeometryComponent::Geometry(
  const MYON_Geometry* no_geometry_return_value
  ) const
{
  const MYON_Geometry* ptr = m_geometry_sp.get();
  return (nullptr != ptr) ? ptr : no_geometry_return_value;
}

const MYON_3dmObjectAttributes* MYON_ModelGeometryComponent::Attributes(
  const MYON_3dmObjectAttributes* no_attributes_return_value
  ) const
{
  const MYON_3dmObjectAttributes* ptr = m_attributes_sp.get();
  return (nullptr != ptr) ? ptr : no_attributes_return_value;
}

MYON_Geometry* MYON_ModelGeometryComponent::ExclusiveGeometry() const
{
  return
    (1 == m_geometry_sp.use_count())
    ? m_geometry_sp.get()
    : nullptr;
}

MYON_3dmObjectAttributes* MYON_ModelGeometryComponent::ExclusiveAttributes() const
{
  return
    (1 == m_attributes_sp.use_count())
    ? m_attributes_sp.get()
    : nullptr;
}

void MYON_ModelGeometryComponent::Dump( MYON_TextLog& text_log ) const
{
  MYON_ModelComponent::Dump(text_log);

  text_log.Print("Attributes:\n");
  text_log.PushIndent();
  const MYON_3dmObjectAttributes* attributes = Attributes(nullptr);
  if (nullptr == attributes)
    text_log.Print("Unset\n");
  else
  {
    attributes->Dump(text_log);
    // attributes user data
    const MYON_UserData* ud = attributes->FirstUserData();
    while (0 != ud)
    {
      text_log.Print("Attributes user data:\n");
      text_log.PushIndent();
      ud->Dump(text_log);
      text_log.PopIndent();
      ud = ud->Next();
    }
  }
  text_log.PopIndent();

  text_log.Print("Geometry:\n");
  text_log.PushIndent();
  const MYON_Object* geometry = Geometry(nullptr);
  if (nullptr == geometry)
    text_log.Print("Unset\n");
  else
  {
    geometry->Dump(text_log);
    // geometry user data
    const MYON_UserData* ud = geometry->FirstUserData();
    while (0 != ud)
    {
      text_log.Print("Geometry user data:\n");
      text_log.PushIndent();
      ud->Dump(text_log);
      text_log.PopIndent();
      ud = ud->Next();
    }
  }
  text_log.PopIndent();
}

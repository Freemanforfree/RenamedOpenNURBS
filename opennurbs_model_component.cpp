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

MYON_OBJECT_IMPLEMENT(MYON_ModelComponent,MYON_Object,"CF37CFB5-8E6F-435F-98FE-D20FA9E7DB6F");


bool MYON_ModelComponent::IsValid(
  MYON_TextLog* text_log
  ) const
{
  return ( m_runtime_serial_number > 0 );
}

const MYON_wString MYON_ModelComponent::ComponentTypeToString(
  MYON_ModelComponent::Type component_type
  )
{
  switch (component_type)
  {
  case MYON_ModelComponent::Type::Unset:
    return MYON_wString("Unset");
  case MYON_ModelComponent::Type::ModelGeometry:
    return MYON_wString("ModelGeometry");
  case MYON_ModelComponent::Type::Image:
    return MYON_wString("EmbeddedFile");
  case MYON_ModelComponent::Type::LinePattern:
    return MYON_wString("LinePattern");
  case MYON_ModelComponent::Type::Layer:
    return MYON_wString("Layer");
  case MYON_ModelComponent::Type::Group:
    return MYON_wString("Group");
  case MYON_ModelComponent::Type::TextStyle:
    return MYON_wString("TextStyle");
  case MYON_ModelComponent::Type::DimStyle:
    return MYON_wString("AnnotationStyle");
  case MYON_ModelComponent::Type::HatchPattern:
    return MYON_wString("HatchPattern");
  case MYON_ModelComponent::Type::InstanceDefinition:
    return MYON_wString("InstanceDefinition");
  case MYON_ModelComponent::Type::HistoryRecord:
    return MYON_wString("HistoryRecord");
  case MYON_ModelComponent::Type::TextureMapping:
    return MYON_wString("TextureMapping");
  case MYON_ModelComponent::Type::Material:
    return MYON_wString("RenderMaterial");
  case MYON_ModelComponent::Type::RenderLight:
    return MYON_wString("RenderLight");
  case MYON_ModelComponent::Type::RenderContent:
    return MYON_wString("RenderContent");
  case MYON_ModelComponent::Type::EmbeddedFile:
    return MYON_wString("EmbeddedFile");
  case MYON_ModelComponent::Type::PostEffect:
    return MYON_wString("PostEffect");
  case MYON_ModelComponent::Type::Mixed:
    return MYON_wString("Mixed");
  default:
    MYON_ERROR("invalid m_component_type value");
  }

  return MYON_wString::EmptyString;
}

MYON_ModelComponentTypeIterator::MYON_ModelComponentTypeIterator() MYON_NOEXCEPT
{
  const size_t types_capacity = sizeof(m_types)/sizeof(m_types[0]);
  for (size_t k = 0; k < types_capacity; k++)
    m_types[k] = MYON_ModelComponent::Type::Unset;
}

MYON_ModelComponentTypeIterator::MYON_ModelComponentTypeIterator(
  size_t type_count,
  const MYON_ModelComponent::Type* types
) MYON_NOEXCEPT
{
  const size_t types_capacity = sizeof(m_types)/sizeof(m_types[0]);
  m_type_count
    = (nullptr != types && type_count > 0)
    ? (int)((type_count < types_capacity) ? type_count : types_capacity)
    : 0;
  memset(m_types, 0, sizeof(m_types));
  for (int i = 0; i < m_type_count; i++)
    m_types[i] = types[i];
  for (size_t k = (size_t)m_type_count; k < types_capacity; k++)
    m_types[k] = MYON_ModelComponent::Type::Unset;
}

unsigned int MYON_ModelComponentTypeIterator::TypeCount() const
{
  return (unsigned int)m_type_count;
}

MYON_ModelComponent::Type MYON_ModelComponentTypeIterator::FirstType()
{
  m_current_index = MYON_UNSET_INT_INDEX;
  return NextType();
}

MYON_ModelComponent::Type MYON_ModelComponentTypeIterator::LastType()
{
  m_current_index = MYON_UNSET_INT_INDEX;
  return PreviousType();
}

MYON_ModelComponent::Type MYON_ModelComponentTypeIterator::NextType()
{
  if (m_type_count > 0)
  {
    if (MYON_UNSET_INT_INDEX == m_current_index)
      m_current_index = 0;
    else if (m_current_index < m_type_count)
      m_current_index++;
  }
  return CurrentType();
}
  
MYON_ModelComponent::Type MYON_ModelComponentTypeIterator::PreviousType()
{
  if (m_type_count > 0)
  {
    if (MYON_UNSET_INT_INDEX == m_current_index)
      m_current_index = m_type_count-1;
    else if (m_current_index >= 0)
      m_current_index--;
  }
  return CurrentType();
}

MYON_ModelComponent::Type MYON_ModelComponentTypeIterator::CurrentType() const
{
  return (m_current_index >= 0 && m_current_index < m_type_count) ? m_types[m_current_index] : MYON_ModelComponent::Type::Unset;
}

void MYON_ModelComponent::Dump(
  MYON_TextLog& text_log
  ) const
{
  const char* sUnset = "unset";
  if (false == text_log.IsTextHash())
  {
    // m_runtime_serial_number depends on how many components have
    // been contructed before this one. It must be suppressed when
    // calculating content hash values from text logs.
    text_log.Print("Model component %llu\n", m_runtime_serial_number);
  }
  text_log.PushIndent();

  // model serial number CRhinoDoc::RuntimeSerialNumber()
  text_log.Print("Model serial number = ");
  if (ModelSerialNumberIsSet())
  {
    const unsigned int model_sn = ModelSerialNumber();
    text_log.Print("%u", model_sn);
    text_log.PrintNewLine();
    // worksession reference model
    const unsigned int reference_model_sn = ReferenceModelSerialNumber();
    if (reference_model_sn > 0)
    {
      text_log.Print("Reference model = %u\n", reference_model_sn);
    }
    // linked instance defintion model
    const unsigned int idef_model_sn = InstanceDefinitionModelSerialNumber();
    if (idef_model_sn > 0)
    {
      text_log.Print("Instance definition = %u\n", idef_model_sn);
    }
  }
  else
  {
    text_log.Print(sUnset);
    text_log.PrintNewLine();
  }

  text_log.Print("Type = ");
  if (this->ComponentTypeIsSet())
  {
    text_log.Print(MYON_ModelComponent::ComponentTypeToString(m_component_type));
  }
  else
  {
    text_log.Print(sUnset);
  }
  text_log.PrintNewLine();

  text_log.Print("Id = ");
  if (IdIsSet())
    text_log.Print(Id());
  else
    text_log.Print(sUnset);
  text_log.PrintNewLine();


  text_log.Print("Index = ");
  if (IndexIsSet())
    text_log.Print("%d", Index());
  else
    text_log.Print(sUnset);
  text_log.PrintNewLine();

  text_log.Print("Name = ");
  if (NameIsSet())
    text_log.Print(L"%ls", static_cast<const wchar_t*>(m_component_name));
  else
    text_log.Print(sUnset);
  text_log.PrintNewLine();

  const MYON__UINT64 content_version_number = ContentVersionNumber();
  if (0 != content_version_number && false == text_log.IsTextHash())
  {
    // content_version_number values vary depending on how
    // the actual content was specified, even if the final content
    // is identical. It must must be suppressed when calculating 
    // content hash values from text logs.

    // Use PRIu64 to portably print 64-bit unsigned int in decimal format.
    text_log.Print("Content version number = %" PRIu64 "\n",content_version_number);
  }

  text_log.PopIndent();
}

void MYON_ModelComponentReference::Dump(
  MYON_TextLog& text_log
  ) const
{
  const MYON_ModelComponent* model_component = ModelComponent();
  if ( nullptr == model_component )
    text_log.Print("Empty MYON_ModelComponentReference\n");
  else
    model_component->Dump(text_log);
}

unsigned int MYON_ModelComponent::SizeOf() const
{
  return (unsigned int)((sizeof(*this) - sizeof(MYON_Object)) + MYON_Object::SizeOf());
}

MYON__UINT32 MYON_ModelComponent::DataCRC(
  MYON__UINT32 current_remainder
  ) const
{
  current_remainder = MYON_CRC32(current_remainder, sizeof(m_locked_status), &m_locked_status);
  current_remainder = MYON_CRC32(current_remainder, sizeof(m_set_status), &m_set_status);
  if (ModelSerialNumberIsSet())
  {
    current_remainder = MYON_CRC32(current_remainder, sizeof(m_model_serial_number), &m_model_serial_number);
    current_remainder = MYON_CRC32(current_remainder, sizeof(m_reference_model_serial_number), &m_reference_model_serial_number);
    current_remainder = MYON_CRC32(current_remainder, sizeof(m_linked_idef_serial_number), &m_linked_idef_serial_number);
  }
  if (IdIsSet())
    current_remainder = MYON_CRC32(current_remainder, sizeof(m_component_id), &m_component_id);
  if (ComponentTypeIsSet())
    current_remainder = MYON_CRC32(current_remainder, sizeof(m_component_type), &m_component_type);
  if (IndexIsSet())
    current_remainder = MYON_CRC32(current_remainder, sizeof(m_component_index), &m_component_index);
  if (NameIsSet())
  {
    MYON_NameHash name_hash = NameHash();
    current_remainder = MYON_CRC32(current_remainder, sizeof(name_hash), &name_hash);
  }
  return current_remainder;
}

MYON_UUID MYON_ModelComponent::ModelObjectId() const
{
  return Id();
}

static MYON__UINT64 MYON_ModelComponentContentVersionNumberOne(
  MYON__UINT64 runtime_serial_number
  )
{
  // make it unlikely for classes created sequentually to have the same content version numbers.
  MYON__UINT64 s = MYON_CRC32(0, sizeof(runtime_serial_number), &runtime_serial_number );
  return (runtime_serial_number*0x100000000ULL | s);
}

MYON_ModelComponent::MYON_ModelComponent
(
  MYON__UINT64 runtime_serial_number
) MYON_NOEXCEPT
  : m_runtime_serial_number(runtime_serial_number)
  , m_content_version_number(MYON_ModelComponentContentVersionNumberOne(m_runtime_serial_number))
{}

MYON_ModelComponent::MYON_ModelComponent() MYON_NOEXCEPT
  : m_runtime_serial_number(++MYON_ModelComponent::Internal_RuntimeSerialNumberGenerator)
  , m_content_version_number(MYON_ModelComponentContentVersionNumberOne(m_runtime_serial_number))
{}

static const MYON_ModelComponent Unset;

MYON_ModelComponent::Type MYON_ModelComponent::ComponentTypeFromUnsigned(
  unsigned int component_type_as_unsigned
  )
{
  switch (component_type_as_unsigned)
  {
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ModelComponent::Type::Unset);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ModelComponent::Type::Image);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ModelComponent::Type::TextureMapping);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ModelComponent::Type::Material);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ModelComponent::Type::LinePattern);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ModelComponent::Type::Layer);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ModelComponent::Type::Group);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ModelComponent::Type::TextStyle);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ModelComponent::Type::DimStyle);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ModelComponent::Type::RenderLight);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ModelComponent::Type::HatchPattern);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ModelComponent::Type::InstanceDefinition);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ModelComponent::Type::ModelGeometry);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ModelComponent::Type::HistoryRecord);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ModelComponent::Type::RenderContent);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ModelComponent::Type::EmbeddedFile);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ModelComponent::Type::PostEffect);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_ModelComponent::Type::Mixed);
  }

  MYON_ERROR("component_type_as_unsigned has invalid value.");
  return MYON_ModelComponent::Type::Unset;
}

bool MYON_ModelComponent::WriteModelComponentAttributes(
  class MYON_BinaryArchive& archive,
  unsigned int attributes_filter
  ) const
{
  const int major_version = 1;
  const int minor_version = 0;
  if (!archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,major_version,minor_version))
    return false;
  bool rc = false;

  for (;;)
  {
    unsigned int bit;
    bit = MYON_ModelComponent::Attributes::IdAttribute & attributes_filter;
    const bool bWriteId = (0 != bit && IdIsSet());

    bit = MYON_ModelComponent::Attributes::ParentIdAttribute & attributes_filter;
    const bool bWriteParentId = (0 != bit && ParentIdIsSet());

    bit = MYON_ModelComponent::Attributes::IndexAttribute & attributes_filter;
    const bool bWriteIndex = (0 != bit && IndexIsSet());

    bit = MYON_ModelComponent::Attributes::NameAttribute & attributes_filter;
    const bool bWriteName = (0 != bit && NameIsSet());

    bit = MYON_ModelComponent::Attributes::ComponentStatusAttribute & attributes_filter;
    const bool bWriteComponentStatus = (0 != bit && ModelComponentStatusIsSet());

    MYON__UINT32 write_bits = 0U;
    if ( bWriteId )
      write_bits |= 0x01U;
    if ( bWriteParentId )
      write_bits |= 0x02U;
    if ( bWriteIndex )
      write_bits |= 0x04U;
    if ( bWriteName )
      write_bits |= 0x08U;
    if ( bWriteComponentStatus )
      write_bits |= 0x10U;

    if (!archive.WriteInt(write_bits))
      break;
 
    if (bWriteId)
    {
      if (!archive.WriteUuid(Id()))
        break;
    }

    if (bWriteParentId)
    {
      if (!archive.WriteUuid(ParentId()))
        break;
    }

    if (bWriteIndex)
    {
      if (!archive.Write3dmReferencedComponentIndex(*this))
        break;
    }


    if (bWriteName)
    {
      if (!archive.WriteString(Name()))
        break;
    }
    
    if (bWriteComponentStatus)
    {
      const MYON_ComponentStatus component_status = ModelComponentStatus();

      const MYON__UINT32 locked_bit = 0x01;
      const MYON__UINT32 hidden_bit = 0x02;
      // Additional states can be saved if that is reasonable.

      const MYON__UINT32 mask 
        = locked_bit 
        | hidden_bit;
      MYON__UINT32 value = 0U;
      if ( component_status.IsLocked() )
        value |= locked_bit;
      if ( component_status.IsHidden() )
        value |= hidden_bit;
      if (!archive.WriteInt(mask))
        break;
      if (!archive.WriteInt(value))
        break;
    }
    
    rc = true;
    break;
  }

  if (!archive.EndWrite3dmChunk())
    rc = false;
  return rc;
}

bool MYON_ModelComponent::ReadModelComponentAttributes(
  class MYON_BinaryArchive& archive
  )
{
  int major_version = 10;
  int minor_version = 0;
  if (!archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_version,&minor_version))
    return false;

  bool rc = false;
  for (;;)
  {
    if ( 1 != major_version)
      break;

    MYON__UINT32 write_bits = 0U;
    if (!archive.ReadInt(&write_bits))
      break;

    const bool bReadId = (0 != (write_bits & 0x01U));
    const bool bReadParentId (0 != (write_bits & 0x02U));
    const bool bReadIndex (0 != (write_bits & 0x04U));
    const bool bReadName (0 != (write_bits & 0x08U));
    const bool bReadComponentStatus = (0 != (write_bits & 0x10U));

    if (bReadId)
    {
      MYON_UUID id = Id();
      if (!archive.ReadUuid(id))
        break;
      SetId(id);
    }

    if (bReadParentId)
    {
      MYON_UUID parent_id = ParentId();
      if (!archive.ReadUuid(parent_id))
        break;
      SetParentId(parent_id);
    }

    if (bReadIndex)
    {
      int component_index = Index();
      if (!archive.ReadInt(&component_index))
        break;
      SetIndex(component_index);
    }


    if (bReadName)
    {
      MYON_wString name;
      if (!archive.ReadString(name))
        break;
      SetName(name);
    }
    
    if (bReadComponentStatus)
    {
      MYON__UINT32 mask = 0U;
      if (!archive.ReadInt(&mask))
        break;
      MYON__UINT32 value = 0U;
      if (!archive.ReadInt(&value))
        break;

      const MYON__UINT32 locked_bit = 0x01;
      const MYON__UINT32 hidden_bit = 0x02;
      // Additional states can be saved if that is reasonable.

      const MYON_ComponentStatus component_status0 = ModelComponentStatus();
      MYON_ComponentStatus component_status = component_status0;
      if (0 != (mask & locked_bit))
      {
        const bool bIsLocked = (0 != (value & locked_bit));
        component_status.SetLockedState(bIsLocked);
      }
      if (0 != (mask & hidden_bit))
      {
        const bool bIsHidden = (0 != (value & hidden_bit));
        component_status.SetHiddenState(bIsHidden);
      }

      if ( component_status != component_status0 )
        SetModelComponentStatus(component_status);
    }
    
    rc = true;
    break;
  }

  if (!archive.EndRead3dmChunk())
    rc = false;
  return rc;
}


unsigned int MYON_ModelComponent::CopyFrom(
  const MYON_ModelComponent& src,
  unsigned int attributes_filter
  )
{
  unsigned int rc = 0;
  unsigned int bit;
  bool b;

  // Erase unused bits.
  attributes_filter &= MYON_ModelComponent::Attributes::AllAttributes;

  // Cannot set locked attributes.
  bit = m_locked_status;
  attributes_filter &= ~bit;

  if ( 0 == attributes_filter )
    return rc; // nothing to change
  
  bit = MYON_ModelComponent::Attributes::ModelSerialNumberAttribute & attributes_filter;
  if (0 != bit)
  {    
    b = (src.ModelSerialNumberIsSet())
      ? SetModelSerialNumber(src.ModelSerialNumber(),src.ReferenceModelSerialNumber(),src.InstanceDefinitionModelSerialNumber())
      : ClearModelSerialNumber();
    if (b)
      rc |= bit;
  }
  
  // component type must be set before parent id
  bit = MYON_ModelComponent::Attributes::TypeAttribute & attributes_filter;
  if (0 != bit)
  {
    b = (src.ComponentTypeIsSet())
      ? SetComponentType(src.ComponentType())
      : ClearComponentType();
    if (b)
      rc |= bit;
  }

  bit = MYON_ModelComponent::Attributes::IdAttribute & attributes_filter;
  if (0 != bit)
  {
    b = (src.IdIsSet())
      ? SetId(src.Id())
      : ClearId();
    if (b)
      rc |= bit;
  }

  bit = MYON_ModelComponent::Attributes::ParentIdAttribute & attributes_filter;
  if (0 != bit)
  {
    b = (src.ParentIdIsSet())
      ? SetParentId(src.ParentId())
      : ClearParentId();
    if (b)
      rc |= bit;
  }

  bit = MYON_ModelComponent::Attributes::IndexAttribute & attributes_filter;
  if (0 != bit)
  {
    b = (src.IndexIsSet())
      ? SetIndex(src.Index())
      : ClearIndex();
    if (b)
      rc |= bit;
  }

  bit = MYON_ModelComponent::Attributes::NameAttribute & attributes_filter;
  if (0 != bit)
  {
    b = (src.NameIsSet())
      ? SetName(src.Name())
      : ClearName();
    if (b)
      rc |= bit;
  }

  bit = MYON_ModelComponent::Attributes::ComponentStatusAttribute & attributes_filter;
  if (0 != bit)
  {
    b = (src.ModelComponentStatusIsSet())
      ? SetModelComponentStatus(src.ModelComponentStatus())
      : ClearModelComponentStatus();
    if (b)
      rc |= bit;
  }

  return rc;
}

MYON_ModelComponent::MYON_ModelComponent(const MYON_ModelComponent& src)
  : MYON_Object(src)
  , m_runtime_serial_number(++MYON_ModelComponent::Internal_RuntimeSerialNumberGenerator)
  , m_content_version_number(MYON_ModelComponentContentVersionNumberOne(m_runtime_serial_number))
{
  CopyFrom(src,MYON_ModelComponent::Attributes::AllAttributes);

  // The Type attribute is a special case.  Locking it in the copy
  // constructor allows the model components that are derived from
  // MYON_ModelComponent to use the default copy constructor.
  const unsigned int bit = MYON_ModelComponent::Attributes::TypeAttribute;
  if (0 != (bit & m_set_status) 
    && MYON_ModelComponent::ComponentTypeIsValid(m_component_type)
    && src.ComponentTypeIsLocked()
    )
    m_locked_status |= bit;
}

MYON_ModelComponent& MYON_ModelComponent::operator=(const MYON_ModelComponent& src)
{
  if (this != &src)
  {
    // copy user data
    MYON_Object::operator=(src); 

    // copy MYON_ModelComponent fields
    CopyFrom(src,MYON_ModelComponent::Attributes::AllAttributes);
  }
  return *this;
}

MYON_ModelComponent::MYON_ModelComponent(
  MYON_ModelComponent::Type component_type,
  const MYON_ModelComponent& src
  ) MYON_NOEXCEPT
  : MYON_Object(src)
  , m_runtime_serial_number(++MYON_ModelComponent::Internal_RuntimeSerialNumberGenerator)
  , m_content_version_number(MYON_ModelComponentContentVersionNumberOne(m_runtime_serial_number))
  , m_component_type(component_type)
{
  if (MYON_ModelComponent::Type::Unset != m_component_type)
  {
    // set and lock m_component_type
    m_set_status = MYON_ModelComponent::Attributes::TypeAttribute;
    m_locked_status = MYON_ModelComponent::Attributes::TypeAttribute;
  }
  CopyFrom(src,MYON_ModelComponent::Attributes::AllAttributes);
}


MYON_ModelComponent::MYON_ModelComponent(
  MYON_ModelComponent::Type component_type
  ) MYON_NOEXCEPT
  : m_runtime_serial_number(++MYON_ModelComponent::Internal_RuntimeSerialNumberGenerator)
  , m_content_version_number(MYON_ModelComponentContentVersionNumberOne(m_runtime_serial_number))
  , m_component_type(component_type)
{
  if (MYON_ModelComponent::Type::Unset != m_component_type)
  {
    m_locked_status = MYON_ModelComponent::Attributes::TypeAttribute;
    m_set_status = MYON_ModelComponent::Attributes::TypeAttribute;
  }
}

MYON__UINT64 MYON_ModelComponent::NextRuntimeSerialNumber()
{
  const MYON__UINT64 last_runtime_serial_number(MYON_ModelComponent::Internal_RuntimeSerialNumberGenerator);
  return (last_runtime_serial_number + 1);
}

MYON__UINT64 MYON_ModelComponent::RuntimeSerialNumber() const
{
  return m_runtime_serial_number;
}

MYON__UINT64 MYON_ModelComponent::ContentVersionNumber() const
{
  return m_content_version_number;
}

void MYON_ModelComponent::IncrementContentVersionNumber() const
{
  m_content_version_number++;
}

bool MYON_ModelComponent::EraseIdentification(
  bool bIgnoreLocks
  )
{
  if (false == bIgnoreLocks)
  {
    if (IdIsLocked() && !(MYON_ModelComponent::Unset.Id() == m_component_id))
    {
      MYON_ERROR("Cannot erase id.");
      return false;
    }
    if (ParentIdIsLocked() && !(MYON_ModelComponent::Unset.Id() == m_component_parent_id))
    {
      MYON_ERROR("Cannot erase parent id.");
      return false;
    }
    if (NameIsLocked() && MYON_ModelComponent::Unset.Name() != m_component_name)
    {
      MYON_ERROR("Cannot erase name.");
      return false;
    }
    if (IndexIsLocked() && MYON_ModelComponent::Unset.Index() != m_component_index)
    {
      MYON_ERROR("Cannot erase index.");
      return false;
    }
  }

  m_component_id = MYON_ModelComponent::Unset.Id();
  m_component_parent_id = MYON_ModelComponent::Unset.Id();
  m_component_index = MYON_ModelComponent::Unset.Index();
  m_component_name = MYON_ModelComponent::Unset.Name();
  m_component_name_hash = MYON_ModelComponent::Unset.NameHash();

  return true;
}


bool MYON_ModelComponent::IsSystemComponent() const
{
  return (
    0 == m_content_version_number
    && 0 != (MYON_ModelComponent::Attributes::SystemComponentAttribute & m_locked_status)
    );
}

bool MYON_ModelComponent::SetAsSystemComponent()
{
  return Internal_SetAsSystemComponent(false);
}

bool MYON_ModelComponent::SetAsUnsetSystemComponent()
{
  return Internal_SetAsSystemComponent(true);
}

bool MYON_ModelComponent::Internal_SetAsSystemComponent(
  bool bUnsetSystemComponent
  )
{
  if (m_locked_status == 0xFFFFU)
  {
    MYON_ERROR("component is already a system component.");
    return true;
  }

  const bool bIndexRequired = MYON_ModelComponent::IndexRequired(m_component_type);
  const bool bUniqueNameRequired = MYON_ModelComponent::UniqueNameRequired(m_component_type);
  const bool bUniqueNameIncludesParent = MYON_ModelComponent::UniqueNameIncludesParent(m_component_type);

  for (;;)
  {
    if (ParentIdIsNotNil())
      break;

    if (bUniqueNameIncludesParent != ParentIdIsSet())
      break;
    
    if (IdIsNil())
    {
      if (!bUnsetSystemComponent)
        break;
    }
    else
    {
      if (bUnsetSystemComponent)
        break;
    }
    
    if ( bIndexRequired && false == bUnsetSystemComponent )
    {
      if (false == IndexIsSet())
        break;
      if (Index() >= 0)
        break;
      if (Index() <= MYON_UNSET_INT_INDEX)
        break;
    }
    else
    {
      if (IndexIsSet())
        break;
    }
    
    if ( MYON_ModelComponent::m_component_status != m_component_status ) 
      break;

    if (bUniqueNameRequired && false == bUnsetSystemComponent)
    {
      if (false == NameIsNotEmpty())
        break;
      if (false == NameHash().IsValidAndNotEmpty())
        break;
    }
    else
    {
      if (NameIsSet())
        return false;
      if (false == NameHash().IsEmptyNameHash())
        break;
    }

    m_locked_status = 0xFFFFU;
    m_content_version_number = 0;
    return true;
  }

  MYON_ERROR("Invalid settings for a system component.");
  return false;
}

///////////////////////////////////////////////////////////////////////////////
//
// Model Serial Number interface
//

unsigned int MYON_ModelComponent::ModelSerialNumber() const
{
  return m_model_serial_number;
}

unsigned int MYON_ModelComponent::ReferenceModelSerialNumber() const
{
  return m_reference_model_serial_number;
}

unsigned int MYON_ModelComponent::InstanceDefinitionModelSerialNumber() const
{
  return m_linked_idef_serial_number;
}

bool MYON_ModelComponent::SetModelSerialNumber(
  unsigned int model_serial_number
  )
{
  return SetModelSerialNumber(
    model_serial_number,
    MYON_ModelComponent::Unset.ReferenceModelSerialNumber(),
    MYON_ModelComponent::Unset.InstanceDefinitionModelSerialNumber()
    );
}

bool MYON_ModelComponent::SetModelSerialNumber(
  unsigned int model_serial_number,
  unsigned int reference_model_serial_number,
  unsigned int instance_definition_model_serial_number
  )
{
  const unsigned int bit = MYON_ModelComponent::Attributes::ModelSerialNumberAttribute;
  if (0 != (bit & m_locked_status))
    return false;

  m_model_serial_number = model_serial_number;
  m_reference_model_serial_number = reference_model_serial_number;
  m_linked_idef_serial_number = instance_definition_model_serial_number;
  m_set_status |= bit;
  return true;

}

bool MYON_ModelComponent::ClearModelSerialNumber()
{
  const unsigned int bit = MYON_ModelComponent::Attributes::ModelSerialNumberAttribute;
  return (bit == ClearModelComponentAttributes(bit));
}

void MYON_ModelComponent::LockModelSerialNumber()
{
  m_locked_status |= MYON_ModelComponent::Attributes::ModelSerialNumberAttribute;
}

bool MYON_ModelComponent::ModelSerialNumberIsLocked() const
{
  const unsigned int bit = MYON_ModelComponent::Attributes::ModelSerialNumberAttribute;
  return (0 != (bit & m_locked_status));
}

bool MYON_ModelComponent::ModelSerialNumberIsSet() const
{
  const unsigned int bit = MYON_ModelComponent::Attributes::ModelSerialNumberAttribute;
  return (0 != (bit & m_set_status));
}

bool MYON_ModelComponent::IsReferenceComponent() const
{
  return (0 != m_reference_model_serial_number || 0 != m_linked_idef_serial_number);
}


///////////////////////////////////////////////////////////////////////////////
//
// Name interface
//
//

const wchar_t* MYON_ModelComponent::GetName(
    MYON_wString& component_name
    ) const
{
  component_name = static_cast<const wchar_t*>(m_component_name);
  return static_cast<const wchar_t*>(component_name);
}

const MYON_wString MYON_ModelComponent::Name() const
{
  return NameIsSet() ? m_component_name : MYON_wString::EmptyString;
}

const MYON_wString MYON_ModelComponent::DeletedName() const
{
  return DeletedNameIsSet() ? m_component_name : MYON_wString::EmptyString;
}

const wchar_t* MYON_ModelComponent::NameAsPointer() const
{
  return NameIsSet() ? m_component_name : MYON_wString::EmptyString;
}

const MYON_wString MYON_ModelComponent::RemoveAllReferencePrefixDelimiters(
  const wchar_t* name
)
{
  if ( nullptr == name || 0 == name[0] )
    return MYON_wString::EmptyString;

  MYON_wString local(name);
  local.Replace(MYON_ModelComponent::ReferencePrefixDelimiter, nullptr);

  return local.IsNotEmpty() ? local : MYON_wString::EmptyString;
}

static const MYON_wString Internal_RemoveTrailingNameToken(
  const wchar_t* name,
  const MYON_wString& token
)
{
  if ( nullptr == name || 0 == name[0] || token.IsEmpty() )
    return MYON_wString::EmptyString;

  MYON_wString local(name);

  wchar_t* s0 = local.Array();
  if (nullptr == s0)
    return MYON_wString::EmptyString;

  // remove any reference name delimiters
  const unsigned int d_length = token.Length();
  const unsigned int local_length = (unsigned int)local.Length();
  if (local_length < d_length )
    return local;

  if (MYON_wString::EqualOrdinal(s0 + (local_length - d_length), token, false))
  {
    local.SetLength((local_length - d_length));
  }

  return local;
}



const MYON_wString MYON_ModelComponent::RemoveTrailingReferencePrefixDelimiter(
  const wchar_t* name
)
{
  return Internal_RemoveTrailingNameToken(name, MYON_ModelComponent::ReferencePrefixDelimiter);
}

const MYON_wString MYON_ModelComponent::RemoveTrailingReferencePrefixSeparator(
  const wchar_t* name
)
{
  return Internal_RemoveTrailingNameToken(name, MYON_ModelComponent::ReferencePrefixSeparator);
}

const MYON_wString MYON_ModelComponent::RemoveTrailingNamePathSeparator(
  const wchar_t* name
)
{
  return Internal_RemoveTrailingNameToken(name, MYON_ModelComponent::NamePathSeparator);
}

//const MYON_wString MYON_ModelComponent::WorksessionReferenceNamePrefix(
//  const wchar_t* reference_prefix,
//  bool bAppendReferenceNamePrefixDelimiter
//)
//{
//  MYON_wString s(reference_prefix);
//
//  for (;;)
//  {
//    const int length0 = s.Length();
//    s.TrimLeftAndRight();
//    s.TrimLeftAndRight(L"[]");
//    const int length1 = s.Length();
//    if (length1 <= 0 || length1 >= length0)
//      break;
//  }
//
//  if (s.IsEmpty())
//    return MYON_wString::EmptyString;
//
//  MYON_wString worksession_prefix = L"[ ";
//  worksession_prefix += s;
//  worksession_prefix += L" ]";
//  return worksession_prefix;
//}

static const wchar_t* InternalStringBeginsWithToken(
  const wchar_t* str,
  const wchar_t* token
)
{
  if (nullptr == str || nullptr == token || 0 == token[0])
    return nullptr;
  while (*str == *token && 0 != *token)
  {
    str++;
    token++;
  }
  return (0 == *token) ? str : nullptr;
}

const wchar_t* MYON_ModelComponent::IsReferencePrefixDelimiter(
  const wchar_t* s
)
{
  return InternalStringBeginsWithToken(s, MYON_ModelComponent::ReferencePrefixDelimiter);
}

const wchar_t* MYON_ModelComponent::IsReferencePrefixSeparator(
  const wchar_t* s
)
{
  return InternalStringBeginsWithToken(s, MYON_ModelComponent::ReferencePrefixSeparator);
}

const wchar_t* MYON_ModelComponent::IsNamePathSeparator(
  const wchar_t* s
)
{
  return InternalStringBeginsWithToken(s, MYON_ModelComponent::NamePathSeparator);
}

void MYON_ModelComponent::SplitName(
  const wchar_t* name,
  MYON_wString& reference_prefix,
  MYON_wString& name_parent,
  MYON_wString& name_leaf
)
{
  MYON_wString local(name);
  wchar_t* str = local.Array();
  wchar_t* tail = str + local.Length();

  reference_prefix = MYON_wString::EmptyString;
  name_parent = MYON_wString::EmptyString;
  name_leaf = MYON_wString::EmptyString;
  if (nullptr == str || 0 == str[0])
    return;

  bool bHaveReference = false;
  bool bHaveLeaf = false;
  while (tail > str)
  {
    tail--;

    if (false == bHaveLeaf)
    {
      const wchar_t* s = MYON_ModelComponent::IsNamePathSeparator(tail);
      if (nullptr != s)
      {
        bHaveLeaf = true;
        name_leaf = s;
        *tail = 0;
        continue;
      }
    }

    if (false == bHaveReference)
    {
      const wchar_t* s = MYON_ModelComponent::IsReferencePrefixDelimiter(tail);
      if (nullptr != s)
      {
        bHaveReference = true;
        if (false == bHaveLeaf)
        {
          bHaveLeaf = true;
          name_leaf = s;
        }
        else
        {
          name_parent = s;
        }
        *tail = 0;
        reference_prefix = MYON_ModelComponent::RemoveAllReferencePrefixDelimiters(str);
        break;
      }
    }
  }

  if (false == bHaveReference)
  {
    if (false == bHaveLeaf)
      name_leaf = name;
    else
      name_parent = str;
  }

  reference_prefix.TrimLeftAndRight();
  name_parent.TrimLeftAndRight();
  name_leaf.TrimLeftAndRight();
}

const MYON_wString MYON_ModelComponent::NameReferencePrefix(const wchar_t * name)
{
  if (nullptr == name || 0 == name[0])
    return MYON_wString::EmptyString;
  const wchar_t* tail = name;
  while (0 != *tail)
    tail++;

  while (tail > name)
  {
    tail--;
    const wchar_t* s = MYON_ModelComponent::IsReferencePrefixDelimiter(tail);
    if (nullptr != s)
    {
      MYON_wString name_reference = name;
      name_reference.SetLength(tail - name);
      name_reference = MYON_ModelComponent::RemoveAllReferencePrefixDelimiters(name_reference);
      name_reference.TrimLeftAndRight();
      return name_reference;
    }
  }

  return MYON_wString::EmptyString;  
}

const MYON_wString MYON_ModelComponent::NameParent(
  const wchar_t * name, 
  bool bIncludeReference
)
{
  if (nullptr == name || 0 == name[0])
    return MYON_wString::EmptyString;

  if (false == bIncludeReference)
  {
    MYON_wString name_reference;
    MYON_wString name_parent;
    MYON_wString name_leaf;
    SplitName(name, name_reference, name_parent, name_leaf);
    return name_parent;
  }

  const wchar_t* tail = name;
  while (0 != *tail)
    tail++;

  while (tail > name)
  {
    tail--;
    const wchar_t* s = MYON_ModelComponent::IsNamePathSeparator(tail);
    if (nullptr != s)
    {
      MYON_wString name_parent = name;
      name_parent.SetLength(tail - name);
      name_parent.TrimLeftAndRight();
      if (bIncludeReference)
        return name_parent;
    }
  }

  return MYON_wString::EmptyString;    
}

const MYON_wString MYON_ModelComponent::NameLeaf(
  const wchar_t* name
)
{
  if (nullptr == name || 0 == name[0])
    return MYON_wString::EmptyString;
  const wchar_t* tail = name;
  while (0 != *tail)
    tail++;

  while( tail > name )
  {
    tail--;

    const wchar_t* s = MYON_ModelComponent::IsNamePathSeparator(tail);
    if ( nullptr == s )
      s = MYON_ModelComponent::IsReferencePrefixDelimiter(tail);
    if (nullptr != s)
    {
      MYON_wString name_leaf = s;
      name_leaf.TrimLeftAndRight();
      return name_leaf; // can be empty when name ends in a delimiter or separator or spaces
    }
  }

  MYON_wString name_leaf = name;
  name_leaf.TrimLeftAndRight();

  return name_leaf;
}

const MYON_wString MYON_ModelComponent::RemoveReferencePrefix(const wchar_t * name)
{
  if (nullptr == name || 0 == name[0])
    return MYON_wString::EmptyString;
  const wchar_t* tail = name;
  while (0 != *tail)
    tail++;

  while (tail > name)
  {
    tail--;
    const wchar_t* s = MYON_ModelComponent::IsReferencePrefixDelimiter(tail);
    if (nullptr != s)
    {
      MYON_wString name_no_reference = s;
      name_no_reference.TrimLeftAndRight();
      return name_no_reference;
    }
  }

  MYON_wString name_no_reference(name);
  name_no_reference.TrimLeftAndRight();
  return name_no_reference;
}


const MYON_NameHash& MYON_ModelComponent::Internal_NameHash() const
{
  if (m_component_name_hash.IsEmptyNameHash() && m_component_name.IsNotEmpty())
  {
    // lazy evaulation because SHA-1 calculation takes appreciable time

    // For components whose names are in a tree structure, like layers,
    // the parent id must be included.
    // For all other components, the parent id must be ignored.
    const MYON_UUID name_parent_id 
      = MYON_ModelComponent::UniqueNameIncludesParent(m_component_type)
      ? m_component_parent_id 
      : MYON_nil_uuid;
    const bool bIgnoreCase = MYON_ModelComponent::UniqueNameIgnoresCase(m_component_type);
    m_component_name_hash = MYON_NameHash::Create(name_parent_id,m_component_name,bIgnoreCase);
  }
  return m_component_name_hash;
}

const MYON_NameHash& MYON_ModelComponent::NameHash() const
{
  return NameIsSet() ? Internal_NameHash() : MYON_NameHash::EmptyNameHash;
}

const MYON_NameHash MYON_ModelComponent::NewNameHash(
  const wchar_t* new_name
) const
{
  MYON_wString local_buffer(new_name);
  local_buffer.TrimLeftAndRight();
  if (local_buffer.IsNotEmpty() && false == MYON_ModelComponent::IsValidComponentName(local_buffer))
  {
    return MYON_NameHash::UnsetNameHash;
  }

  const MYON_UUID name_parent_id 
    = MYON_ModelComponent::UniqueNameIncludesParent(ComponentType())
    ? Id()
    : MYON_nil_uuid;
  return MYON_NameHash::Create(name_parent_id, local_buffer);
}

const MYON_NameHash& MYON_ModelComponent::DeletedNameHash() const
{
  return DeletedNameIsSet() ? Internal_NameHash() : MYON_NameHash::EmptyNameHash;
}

bool MYON_ModelComponent::NameIsEmpty() const
{
  // should return true when the name is not set.
  return NameIsSet() ? m_component_name.IsEmpty() : true;
}

bool MYON_ModelComponent::NameIsNotEmpty() const
{
  return NameIsSet() ? m_component_name.IsNotEmpty() : false;
}

bool MYON_ModelComponent::ChangeName(
  const wchar_t* new_name,
  class MYON_ComponentManifest* manifest
)
{
  const MYON_ModelComponent::Type component_type = ComponentType();

  if (nullptr != manifest)
  {
    if (manifest->ItemFromId(component_type, Id()).IsUnset())
    {
      MYON_ERROR("component is not in the manifest.");
      return false;
    }
  }

  const bool bUniqueNameRequired = MYON_ModelComponent::UniqueNameRequired(component_type);
  MYON_wString local_name(new_name);
  local_name.TrimLeftAndRight();
  new_name = local_name;
  const MYON_UUID name_parent_id
    = MYON_ModelComponent::UniqueNameIncludesParent(component_type)
    ? ParentId()
    : MYON_nil_uuid;
  const MYON_NameHash new_name_hash = MYON_NameHash::Create(name_parent_id,new_name);
  if (local_name.IsEmpty() )
  {
    if (bUniqueNameRequired)
    {
      MYON_ERROR("This component cannot have an empty name.");
      return false;
    }
  }
  else if (MYON_ModelComponent::IsValidComponentName(new_name))
  {
    MYON_ERROR("Invalid component name.");
    return false;
  }
  else if (bUniqueNameRequired && nullptr != manifest )
  {
    if (manifest->ItemFromNameHash(component_type, new_name_hash).IsValid())
    {
      // name is already being used.
      return false;
    }
  }

  if ( 
    nullptr != manifest
    && manifest->ChangeComponentName(Id(), component_type, ParentId(), new_name).IsUnset() 
    )
  {
    MYON_ERROR("Unable to update manifest.");
    return false;
  }

  const bool rc
    = (local_name.IsEmpty())
    ? ClearName()
    : SetName(new_name);

  if (false == rc)
  {
    MYON_ERROR("Unable to change component name.");
    return false;
  }

  return true;
}

bool MYON_ModelComponent::SetName(
  const wchar_t* component_name
  )
{
  MYON_wString local_buffer(component_name);
  local_buffer.TrimLeftAndRight();

  const unsigned int bit = MYON_ModelComponent::Attributes::NameAttribute;
  if (0 != (bit & m_locked_status))
    return false;

  if (local_buffer.IsNotEmpty() && false == MYON_ModelComponent::IsValidComponentName(local_buffer))
  {
    MYON_ERROR("Invalid component_name parameter.");
    return false;
  }

  const bool bContentChange = (false == NameIsSet() || false == m_component_name.EqualOrdinal(local_buffer,false));

  m_component_name = local_buffer;

  // lazy evaluation is used to set m_component_name_hash
  m_component_name_hash = MYON_NameHash::EmptyNameHash;

  m_set_status |= bit;
  m_set_status &= ~(MYON_ModelComponent::Attributes::DeletedNameAttribute);

  if ( bContentChange )
    IncrementContentVersionNumber();

  return true;
}

bool MYON_ModelComponent::SetLocalizedSystemComponentName(
  const wchar_t* system_component_localized_name
  )
{
  if (false == IsSystemComponent())
  {
    MYON_ERROR("not a system component.");
    return false;
  }
  if (NameIsLocked())
  {
    MYON_ERROR("name cannot be changed.");
    return false;
  }
  MYON_wString local_buffer(system_component_localized_name);
  local_buffer.TrimLeftAndRight();
  system_component_localized_name = local_buffer;
  if (nullptr == system_component_localized_name || 0 == system_component_localized_name[0])
  {
    MYON_ERROR("system_component_localized_name cannot be empty.");
    return false;
  }
  const MYON_NameHash name_hash = MYON_NameHash::Create(ParentId(),system_component_localized_name);
  if (false == name_hash.IsValidAndNotEmpty())
  {
    MYON_ERROR("system_component_localized_name is not valid.");
    return false;
  }

  m_component_name = local_buffer;
  m_component_name_hash = name_hash;
  return SetAsSystemComponent();
}

bool MYON_ModelComponent::ClearName()
{
  const unsigned int bit = (MYON_ModelComponent::Attributes::NameAttribute | MYON_ModelComponent::Attributes::DeletedNameAttribute);
  return (bit == ClearModelComponentAttributes(bit));
}

void MYON_ModelComponent::LockName()
{
  m_locked_status |= MYON_ModelComponent::Attributes::NameAttribute;

  // If name is deleted, erase it. 
  m_set_status &= ~(MYON_ModelComponent::Attributes::DeletedNameAttribute);
  if (0 == (MYON_ModelComponent::Attributes::NameAttribute & m_set_status))
  {    
    m_component_name_hash = MYON_NameHash::EmptyNameHash;
    m_component_name = MYON_wString::EmptyString;
  }
}

void MYON_ModelComponent::LockAllSettingsExceptName()
{
  const unsigned int name_bit = MYON_ModelComponent::Attributes::NameAttribute;
  unsigned int mask = ~name_bit;
  mask &= MYON_ModelComponent::Attributes::AllAttributes;
  m_locked_status |= mask;
}

bool MYON_ModelComponent::NameIsLocked() const
{
  const unsigned int bit = MYON_ModelComponent::Attributes::NameAttribute;
  return (0 != (bit & m_locked_status));
}

bool MYON_ModelComponent::NameIsSet() const
{
  const unsigned int bit = MYON_ModelComponent::Attributes::NameAttribute;
  return (0 != (bit & m_set_status));
}

bool MYON_ModelComponent::DeletedNameIsSet() const
{
  const unsigned int bit = MYON_ModelComponent::Attributes::DeletedNameAttribute;
  return (0 != (bit & m_set_status));
}

bool MYON_ModelComponent::DeleteName()
{
  if ( NameIsLocked() )
    return false; // cannot modify a locked name.

  if ( false == NameIsSet() )
    return false; // nothing to delete.
  
  const MYON__UINT16 name_bit = (MYON__UINT16)MYON_ModelComponent::Attributes::NameAttribute;
  const MYON__UINT16 deleted_name_bit = (MYON__UINT16)MYON_ModelComponent::Attributes::DeletedNameAttribute;
  m_set_status &= ~(name_bit | deleted_name_bit);

  // do this for all m_component_name contents including empty names.
  // There is a difference between name being "unset" and name being "set to empty".
  //
  // name is now "deleted"
  m_set_status |= deleted_name_bit;
  IncrementContentVersionNumber();

  return DeletedNameIsSet();
}

bool MYON_ModelComponent::UndeleteName()
{
  if ( NameIsLocked() )
    return false; // cannot modify a locked name.

  if ( false == DeletedNameIsSet() )
    return false; // nothing to delete.
  
  const MYON__UINT16 name_bit = (MYON__UINT16)MYON_ModelComponent::Attributes::NameAttribute;
  const MYON__UINT16 deleted_name_bit = (MYON__UINT16)MYON_ModelComponent::Attributes::DeletedNameAttribute;
  m_set_status &= ~(name_bit | deleted_name_bit);

  // do this for all m_component_name contents including empty names.
  // There is a difference between name being "unset" and name being "set to empty".
  //
  // name is now "undeleted"
  m_set_status |= name_bit;
  IncrementContentVersionNumber();

  return NameIsSet();
}

int MYON_ModelComponent::CompareName(
  const wchar_t* other_name
  ) const
{
  return CompareName(ParentId(), other_name );
}

int MYON_ModelComponent::CompareName(
  const MYON_UUID& other_parent_id,
  const wchar_t* other_name
  ) const
{
  int rc
    = (MYON_ModelComponent::UniqueNameIncludesParent(m_component_type) && NameIsSet())
    ? MYON_UuidCompare(m_component_parent_id, other_parent_id)
    : 0;
  return
    (0 != rc)
    ? rc
    : MYON_wString::CompareAttributeName(Name(), other_name);
}

int MYON_ModelComponent::CompareId(
  const MYON_ModelComponent& a,
  const MYON_ModelComponent& b
  )
{
  return MYON_UuidCompare(a.m_component_id, b.m_component_id);
}

int MYON_ModelComponent::CompareNameAndId(
  const MYON_ModelComponent& a,
  const MYON_ModelComponent& b
  )
{
  int rc = MYON_ModelComponent::CompareName(a, b);
  if (0 == rc)
    rc = MYON_ModelComponent::CompareId(a, b);
  return rc;
}

int MYON_ModelComponent::CompareIdAndName(
  const MYON_ModelComponent& a,
  const MYON_ModelComponent& b
  )
{
  int rc = MYON_ModelComponent::CompareId(a, b);
  if (0 == rc)
    rc = MYON_ModelComponent::CompareName(a, b);
  return rc;
}


int MYON_ModelComponent::CompareName(
  const MYON_ModelComponent& a,
  const MYON_ModelComponent& b
  )
{
  const MYON_UUID a_parent_id 
    = MYON_ModelComponent::UniqueNameIncludesParent(a.m_component_type) && a.NameIsSet()
    ? a.ParentId()
    : MYON_nil_uuid;
  const MYON_UUID b_parent_id 
    = MYON_ModelComponent::UniqueNameIncludesParent(b.m_component_type) && b.NameIsSet()
    ? b.ParentId()
    : MYON_nil_uuid;

  int rc = MYON_UuidCompare(a_parent_id,b_parent_id);

  return 
    (0 != rc)
    ? rc
    : MYON_wString::CompareAttributeName(a.m_component_name,b.m_component_name);
}

int MYON_ModelComponent::CompareNameExact(
  const wchar_t* other_name
  ) const
{
  return CompareNameExact(ParentId(),other_name);
}

int MYON_ModelComponent::CompareNameExact(
  const MYON_UUID& other_parent_id,
  const wchar_t* other_name
  ) const
{
  int rc = MYON_ModelComponent::CompareName(other_parent_id,other_name);
  return 
    (0 != rc)
    ? rc
    : MYON_wString::CompareOrdinal(m_component_name,other_name,false);
}

int MYON_ModelComponent::CompareNameExact(
  const MYON_ModelComponent& a,
  const MYON_ModelComponent& b
  )
{
  int rc = MYON_ModelComponent::CompareName(a,b);
  return 
    (0 != rc)
    ? rc
    : MYON_wString::CompareOrdinal(a.m_component_name,b.m_component_name,false);
}

///////////////////////////////////////////////////////////////////////////////
//
// Type interface
//

MYON_ModelComponent::Type MYON_ModelComponent::ComponentType() const
{
  return m_component_type;
}

bool MYON_ModelComponent::SetComponentType(
  MYON_ModelComponent::Type component_type
  )
{
  const unsigned int bit = MYON_ModelComponent::Attributes::TypeAttribute;
  if (0 != (bit & m_locked_status))
    return false;

  const bool bContentChange = (m_component_type != component_type);

  m_component_type = component_type;
  m_set_status |= bit;

  if ( bContentChange )
    IncrementContentVersionNumber();

  return true;
}

bool MYON_ModelComponent::ClearComponentType()
{
  const unsigned int bit = MYON_ModelComponent::Attributes::TypeAttribute;
  return (bit == ClearModelComponentAttributes(bit));
}

void MYON_ModelComponent::LockComponentType()
{
  m_locked_status |= MYON_ModelComponent::Attributes::TypeAttribute;
}

bool MYON_ModelComponent::ComponentTypeIsLocked() const
{
  const unsigned int bit = MYON_ModelComponent::Attributes::TypeAttribute;
  return (0 != (bit & m_locked_status));
}

bool MYON_ModelComponent::ComponentTypeIsSet() const
{
  const unsigned int bit = MYON_ModelComponent::Attributes::TypeAttribute;
  return (0 != (bit & m_set_status));
}

bool MYON_ModelComponent::ComponentTypeIsValid(
  MYON_ModelComponent::Type component_type
  )
{
  return (
    MYON_ModelComponent::ComponentTypeFromUnsigned(static_cast<unsigned int>(component_type)) == component_type
    && MYON_ModelComponent::Type::Unset != component_type
    );
}

bool MYON_ModelComponent::ComponentTypeIsValidAndNotMixed(
  MYON_ModelComponent::Type component_type
  )
{
  return 
    MYON_ModelComponent::ComponentTypeIsValid(component_type)
    && MYON_ModelComponent::Type::Mixed != component_type;
}

bool MYON_ModelComponent::IsValidComponentName(
  const MYON_wString& candidate_component_name
  )
{
  return MYON_ModelComponent::IsValidComponentName(
    (size_t)candidate_component_name.Length(),
    static_cast<const wchar_t*>(candidate_component_name)
    );

}

bool MYON_ModelComponent::IsValidComponentName(
  size_t length,
  const wchar_t* candidate_component_name
  )
{
  return 
    (length > 0 
    && length <= 2147483645
    && length == (size_t)MYON_wString::Length(candidate_component_name,length+1)
    && MYON_ModelComponent::IsValidComponentName(candidate_component_name)
    );
}



bool MYON_ModelComponent::IsValidComponentName(
  const class MYON_ComponentManifest&  model_manfest,
  const MYON_ModelComponent& model_component,
  bool bPermitReferencePrefix,
  MYON_wString& valid_name
)
{
  valid_name = MYON_wString::EmptyString;
  const MYON_ModelComponent::Type model_component_type = model_component.ComponentType();

  const MYON_wString original_name = model_component.Name();

  bool rc = false;
  for (;;)
  {
    if (false == MYON_ModelComponent::IsValidComponentName(original_name))
    {
      break;
    }

    const bool bUniqueNameRequired = MYON_ModelComponent::UniqueNameRequired(model_component_type);
    if (bPermitReferencePrefix && false == bUniqueNameRequired)
    {
      rc = true;
      break;
    }

    const class MYON_ComponentManifestItem& item
      = (bUniqueNameRequired)
      ? model_manfest.ItemFromName(&model_component)
      : MYON_ComponentManifestItem::UnsetItem;

    const bool bItemFromNameIsOK
      =  (item.ComponentRuntimeSerialNumber() == model_component.RuntimeSerialNumber())
      || item.IsUnset()
      || item.IsSystemComponent()
      ;

    if ( bItemFromNameIsOK && bPermitReferencePrefix)
    {
      rc = true;
      break;
    }

    MYON_wString ref;
    MYON_wString parent;
    MYON_wString leaf;
    MYON_ModelComponent::SplitName(
      original_name,
      ref,
      parent,
      leaf
    );
    if (bItemFromNameIsOK && ref.IsEmpty())
    {
      rc = true;
      break;
    }

    if (false == MYON_ModelComponent::IsValidComponentName(leaf))
      break;

    const class MYON_ComponentManifestItem& leaf_item
      = (bUniqueNameRequired)
      ? model_manfest.ItemFromName(model_component_type,model_component.ParentId(),leaf)
      : MYON_ComponentManifestItem::UnsetItem;

    if (leaf_item.IsUnset() || leaf_item.IsSystemComponent())
    {
      valid_name = leaf;
      break;
    }

    valid_name = model_manfest.UnusedName(
      model_component.ComponentType(),
      model_component.ParentId(),
      leaf,
      nullptr,
      nullptr,
      0,
      nullptr
    );

    break;
  }

  if (rc)
  {
    valid_name = original_name;
  }
  else if (valid_name.IsEmpty())
  {
    valid_name = model_manfest.UnusedName(
      model_component_type,
      model_component.ParentId(),
      nullptr,
      nullptr,
      nullptr,
      0,
      nullptr
    );
  }

  return rc;
}

static bool Internal_IsValidComponentNameCodePoint(MYON__UINT32 unicode_code_point)
{
  return
    unicode_code_point >= MYON_UnicodeCodePoint::MYON_Space
    && 0 != MYON_IsValidUnicodeCodePoint(unicode_code_point)
    && 0 == MYON_IsUnicodeC1ControlCodePoint(unicode_code_point)
    ;
}

bool MYON_ModelComponent::IsValidComponentNameFirstCodePoint(
  MYON__UINT32 unicode_code_point
)
{
  return
    Internal_IsValidComponentNameCodePoint(unicode_code_point)
    && 0 == MYON_IsUnicodeSpaceCodePoint(unicode_code_point)
    && unicode_code_point != '('
    && unicode_code_point != ')'
    && unicode_code_point != '['
    && unicode_code_point != ']'
    && unicode_code_point != '{'
    && unicode_code_point != '}';
}

bool MYON_ModelComponent::IsValidComponentNameFirstCodePoint(
  int length,
  const wchar_t* candidate_component_name
)
{
  if (nullptr == candidate_component_name || 0 == candidate_component_name[0])
    return false;

  if (length < 0)
  {
    for (length = 0; 0 != candidate_component_name[length]; length++)
    {
      if (length > 8)
        break; // 8 elements is plenty for all possible wchar_t encodings.
    }
  }

  MYON_UnicodeErrorParameters e = MYON_UnicodeErrorParameters::FailOnErrors;
  e.m_error_code_point = 0;
  MYON__UINT32 unicode_code_point = 0;
  MYON_DecodeWideChar(candidate_component_name, length, &e, &unicode_code_point);
  return MYON_ModelComponent::IsValidComponentNameFirstCodePoint(unicode_code_point);
}

bool MYON_ModelComponent::IsValidComponentName(
  const wchar_t* candidate_component_name 
  )
{
  const bool bPermitInternalSpaces = true;


  if (nullptr == candidate_component_name || 0 == candidate_component_name[0])
    return false;

  // the first character in a component name cannot be a bracket, space or below
  const MYON__UINT32 c2[2] = { (MYON__UINT32)(candidate_component_name[0]), (MYON__UINT32)(candidate_component_name[1]) };
  const MYON__UINT32 first_code_point
    = MYON_IsValidUTF32Value(c2[0])
    ? c2[0]
    : (MYON_IsValidUTF16SurrogatePair(c2[0], c2[1]) ? MYON_DecodeUTF16SurrogatePair(c2[0], c2[1],0): 0)
    ;
  bool rc = IsValidComponentNameFirstCodePoint(first_code_point);

  bool bLastCharWasSpace = false;
  for ( const wchar_t* name = candidate_component_name; 0 != *name && rc; name++ )
  {
    const MYON__UINT32 c = (MYON__UINT32)(name[0]);
    if (0 == Internal_IsValidComponentNameCodePoint(c))
    {
      if (MYON_IsValidUTF16SurrogatePair(c, (MYON__UINT32)(name[1])))
      {
        // all code points that come from surrogate pairs are permitted
        bLastCharWasSpace = false;
        name++;
        continue;
      }
      return false;
    }
    if (MYON_IsUnicodeSpaceCodePoint(c))
    {
      rc = bPermitInternalSpaces;
      bLastCharWasSpace = true;
    }
    else
      bLastCharWasSpace = false;
  }

  return (rc && !bLastCharWasSpace) ? true : false;
}

bool MYON_ModelComponent::IndexRequired(MYON_ModelComponent::Type component_type)
{
  switch (component_type)
  {
  case MYON_ModelComponent::Type::Image:
  case MYON_ModelComponent::Type::TextureMapping:
  case MYON_ModelComponent::Type::RenderContent: // JohnC: I have no idea how to decide what to return.
  case MYON_ModelComponent::Type::EmbeddedFile:  // JohnC: I have no idea how to decide what to return.
  case MYON_ModelComponent::Type::PostEffect:    // JohnC: I have no idea how to decide what to return.
  case MYON_ModelComponent::Type::Material:      // MYON_Layer references render material by index
  case MYON_ModelComponent::Type::LinePattern:   // MYON_Layer references line pattern by index
  case MYON_ModelComponent::Type::Layer:
  case MYON_ModelComponent::Type::Group:
  case MYON_ModelComponent::Type::TextStyle:
  case MYON_ModelComponent::Type::DimStyle:
  case MYON_ModelComponent::Type::RenderLight:
  case MYON_ModelComponent::Type::HatchPattern:
  case MYON_ModelComponent::Type::InstanceDefinition:
    return true;

  case MYON_ModelComponent::Type::ModelGeometry:
  case MYON_ModelComponent::Type::HistoryRecord:
    return false;

  default:
    MYON_ERROR("Invalid component_type parameter.");
    return false;
  }
}

bool MYON_ModelComponent::UniqueNameIncludesParent(MYON_ModelComponent::Type component_type)
{
  // [Giulio] If this table is changed, then also RhinoCommon documentation in
  // Rhino.DocObjects.ModelComponent.FindName() / FindNameHash() will require appropriate tweaks.

  switch (component_type)
  {
  case MYON_ModelComponent::Type::Layer:
    return true;

  case MYON_ModelComponent::Type::Image:
  case MYON_ModelComponent::Type::TextureMapping:
  case MYON_ModelComponent::Type::Material:
  case MYON_ModelComponent::Type::RenderContent: // JohnC: TODO: Giulio's tweak above. Left for the wrapping phase.
  case MYON_ModelComponent::Type::EmbeddedFile:  // JohnC: TODO: Giulio's tweak above. Left for the wrapping phase.
  case MYON_ModelComponent::Type::PostEffect:    // JohnC: TODO: Giulio's tweak above. Left for the wrapping phase.
  case MYON_ModelComponent::Type::LinePattern:
  case MYON_ModelComponent::Type::Group:
  case MYON_ModelComponent::Type::TextStyle:
  case MYON_ModelComponent::Type::DimStyle:
  case MYON_ModelComponent::Type::RenderLight:
  case MYON_ModelComponent::Type::HatchPattern:
  case MYON_ModelComponent::Type::InstanceDefinition:
  case MYON_ModelComponent::Type::ModelGeometry:
  case MYON_ModelComponent::Type::HistoryRecord:
    return false;

  default:
    MYON_ERROR("Invalid component_type parameter.");
    return false;
  }
}

bool MYON_ModelComponent::UniqueNameRequired(MYON_ModelComponent::Type component_type)
{
  // [Giulio] If this table is changed, then also 
  // Rhino.DocObjects.ModelComponent.ModelComponentTypeRequiresUniqueName()
  // will need the same tweak.

  switch (component_type)
  {
  case MYON_ModelComponent::Type::LinePattern:
  case MYON_ModelComponent::Type::Layer:
  case MYON_ModelComponent::Type::Group:
  case MYON_ModelComponent::Type::TextStyle:
  case MYON_ModelComponent::Type::DimStyle:
  case MYON_ModelComponent::Type::HatchPattern:
  case MYON_ModelComponent::Type::InstanceDefinition:
  case MYON_ModelComponent::Type::EmbeddedFile: ///////////////// Giulio
    return true;

  case MYON_ModelComponent::Type::Image:
  case MYON_ModelComponent::Type::TextureMapping:
  case MYON_ModelComponent::Type::Material:
  case MYON_ModelComponent::Type::RenderContent: // JohnC: TODO: Giulio's tweak above. Left for the wrapping phase.
  case MYON_ModelComponent::Type::PostEffect:    // JohnC: TODO: Giulio's tweak above. Left for the wrapping phase.
  case MYON_ModelComponent::Type::RenderLight:
  case MYON_ModelComponent::Type::ModelGeometry:
  case MYON_ModelComponent::Type::HistoryRecord:
    return false;

  default:
    MYON_ERROR("Invalid component_type parameter.");
    return false;
  }
}

bool MYON_ModelComponent::UniqueNameIgnoresCase(MYON_ModelComponent::Type component_type)
{
  // This function must return true for components like MYON_Material that do not require a unique name.
  return (
    // NO // MYON_ModelComponent::UniqueNameRequired(component_type) &&
    MYON_ModelComponent::Type::Group != component_type
    );
}

///////////////////////////////////////////////////////////////////////////////
//
// Id interface
//
bool MYON_ModelComponent::SetIdentification(
  const class MYON_ComponentManifestItem& manifest_item,
  const wchar_t* manifest_name,
  bool bSetId,
  bool bSetParentId,
  bool bSetName,
  bool bSetIndex
  )
{
  bool rc = true;

  if (bSetId && false == (manifest_item.Id() == Id()))
  {
    if (false == SetId(manifest_item.Id()))
    {
      MYON_ERROR("id cannot be set");
      rc = false;
    }
  }

  const MYON_UUID parent_id 
    = bSetParentId
    ? manifest_item.NameHash().ParentId()
    : MYON_nil_uuid;

  if (bSetParentId && false == (parent_id == ParentId()))
  {
    if (false == SetParentId(parent_id))
    {
      MYON_ERROR("parent id cannot be set");
      rc = false;
    }
  }

  if (bSetName && NameHash() != manifest_item.NameHash())
  {
    if ( manifest_item.NameHash() != MYON_NameHash::Create(manifest_item.NameHash().ParentId(),manifest_name) )
    {
      MYON_ERROR("manifest_name parameter is not valid.");
      rc = false;
    }
    else if (false == SetName(manifest_name) )
    {
      MYON_ERROR("name cannot be set");
      rc = false;
    }
  }

  if (bSetIndex && Index() != manifest_item.Index() )
  {
    if (false == SetIndex(manifest_item.Index()))
    {
      MYON_ERROR("index cannot be set");
      rc = false;
    }
  }

  return rc;
}

///////////////////////////////////////////////////////////////////////////////
//
// Id interface
//

const MYON_UUID& MYON_ModelComponent::Id() const
{
  return m_component_id;
}

bool MYON_ModelComponent::IdIsNil() const
{
  return (MYON_nil_uuid == m_component_id);
}

bool MYON_ModelComponent::IdIsNotNil() const
{
  return !(MYON_nil_uuid == m_component_id);
}

bool MYON_ModelComponent::SetId(
  const MYON_UUID& component_id
  )
{
  const unsigned int bit = MYON_ModelComponent::Attributes::IdAttribute;
  if (0 != (bit & m_locked_status))
    return false;

  const bool bContentChange = (false == (m_component_id == component_id));
  m_component_id = component_id;
  m_set_status |= bit;
  if ( bContentChange )
    IncrementContentVersionNumber();

  return true;
}

const MYON_UUID& MYON_ModelComponent::IfIdIsNilSetId()
{
  return
    ( MYON_nil_uuid == m_component_id)
    ? SetId()
    : m_component_id;
}

const MYON_UUID& MYON_ModelComponent::SetId()
{
  if (false == IdIsLocked())
  {
    SetId(MYON_CreateId());
    return m_component_id;
  }
  return MYON_nil_uuid;
}

const MYON_UUID& MYON_ModelComponent::SetAndLockId()
{
  const MYON_UUID& new_id = SetId();
  if (!(MYON_nil_uuid == new_id))
    LockId();
  return new_id;
}

bool MYON_ModelComponent::ClearId()
{
  const unsigned int bit = MYON_ModelComponent::Attributes::IdAttribute;
  return (bit == ClearModelComponentAttributes(bit));
}

void MYON_ModelComponent::LockId()
{
  m_locked_status |= MYON_ModelComponent::Attributes::IdAttribute;
}

bool MYON_ModelComponent::IdIsLocked() const
{
  const unsigned int bit = MYON_ModelComponent::Attributes::IdAttribute;
  return (0 != (bit & m_locked_status));
}

bool MYON_ModelComponent::IdIsSet() const
{
  const unsigned int bit = MYON_ModelComponent::Attributes::IdAttribute;
  return (0 != (bit & m_set_status));
}


///////////////////////////////////////////////////////////////////////////////
//
// Component Status interface
//
//

MYON_ComponentStatus MYON_ModelComponent::ModelComponentStatus() const
{
  return m_component_status;
}

bool MYON_ModelComponent::SetModelComponentStatus(
  MYON_ComponentStatus component_status
  )
{
  const unsigned int bit = MYON_ModelComponent::Attributes::ComponentStatusAttribute;
  if (0 != (bit & m_locked_status))
    return false;

  const bool bContentChange = (m_component_status != component_status);

  m_component_status = component_status;
  m_set_status |= bit;

  if ( bContentChange )
    IncrementContentVersionNumber();

  return true;
}

bool MYON_ModelComponent::ClearModelComponentStatus()
{
  const unsigned int bit = MYON_ModelComponent::Attributes::ComponentStatusAttribute;
  return (bit == ClearModelComponentAttributes(bit));
}

void MYON_ModelComponent::LockModelComponentStatus()
{
  m_locked_status |= MYON_ModelComponent::Attributes::ComponentStatusAttribute;
}

bool MYON_ModelComponent::ModelComponentStatusIsLocked() const
{
  const unsigned int bit = MYON_ModelComponent::Attributes::ComponentStatusAttribute;
  return (0 != (bit & m_locked_status));
}

bool MYON_ModelComponent::ModelComponentStatusIsSet() const
{
  const unsigned int bit = MYON_ModelComponent::Attributes::ComponentStatusAttribute;
  return (0 != (bit & m_set_status));
}

bool MYON_ModelComponent::IsDeleted() const
{
  return m_component_status.IsDeleted();
}

bool MYON_ModelComponent::SetDeletedModelComponentState(
  bool bDeleted,
  MYON_ComponentManifest* manifest
  )
{
  bDeleted = bDeleted ? true : false; // inusre exact "true"
  MYON_ComponentStatus component_status = ModelComponentStatus();
  if (bDeleted == component_status.IsDeleted())
  {
    // nothing to change
    MYON_ERROR("model_component deleted status = bDeleted");
  }
  else
  {
    component_status.SetDeletedState(bDeleted);
    if (false == SetModelComponentStatus(component_status))
    {
      MYON_ERROR("cannot change component deleted state.");
    }
  }

  const MYON_ComponentManifestItem& manifest_item
    = (nullptr != manifest)
    ? ((bDeleted || MYON_nil_uuid == m_component_id) ? manifest->ItemFromComponentRuntimeSerialNumber(m_runtime_serial_number) : manifest->ItemFromId(m_component_id))
    : MYON_ComponentManifestItem::UnsetItem;
  
  const bool bComponentInManifest 
    = nullptr != manifest 
    && manifest_item.Id() == m_component_id
    && (false == bDeleted || manifest_item.ComponentRuntimeSerialNumber() == this->m_runtime_serial_number);
    
  if (nullptr != manifest && false == bComponentInManifest )
  {
    MYON_ERROR("component is not in manifest.");
  }

  if (bDeleted)
  {
    // delete component name
    if (DeletedNameIsSet())
    {
      MYON_ERROR("component name is already deleted");
    }
    else if (NameIsSet())
    {
      if (false == DeleteName())
      {
        MYON_ERROR("component name cannot be deleted.");
      }
    }

    if (bComponentInManifest)
    {
      // delete manifest item
      if (manifest_item.IsDeleted())
      {
        MYON_ERROR("manifest item deleted status is already true.");
      }
      else
      {
        const MYON_ComponentManifestItem& deleted_manifest_item = manifest->DeleteComponent(m_runtime_serial_number);
        if (false == deleted_manifest_item.IsDeleted())
        {
          MYON_ERROR("unable to change manifest item deleted status to true.");
        }
      }
    }
  }
  else
  {
    // undelete component name
    MYON_wString candidate_name;
    if (NameIsSet())
    {
      MYON_ERROR("model_component name is already set");
      candidate_name = Name();
    }
    else if (DeletedNameIsSet())
    {
      candidate_name = DeletedName();
      if (false == UndeleteName())
      {
        MYON_ERROR("cannot model_component deleted name cannot be restored.");
      }
    }

    if (bComponentInManifest)
    {
      // undelete manifest item
      if (false == manifest_item.IsDeleted())
      {
        MYON_ERROR("doc_manifest item deleted status is already false.");
      }
      else
      {
        // When an component is being replaced, it is common for the old an new component
        // to have the same id but different RuntimeSerialNumber() values.
        MYON_wString assigned_name;
        const MYON_ComponentManifestItem& undeleted_manifest_item = manifest->UndeleteComponentAndChangeRuntimeSerialNumber(
          Id(),
          ParentId(),
          RuntimeSerialNumber(),
          candidate_name,
          assigned_name
          );
        if (false != undeleted_manifest_item.IsDeleted())
        {
          MYON_ERROR("unable to change manifest item deleted status to false.");
        }
        SetName(assigned_name);
      }
    }
  }

  return (bDeleted == IsDeleted());
}

bool MYON_ModelComponent::IsLocked() const
{
  return m_component_status.IsLocked();
}

void MYON_ModelComponent::SetLockedModelComponentState(
  bool bLockedState
  )
{
  MYON_ComponentStatus component_status = ModelComponentStatus();
  if (bLockedState != component_status.IsLocked())
  {
    component_status.SetLockedState(bLockedState);
    if (false == SetModelComponentStatus(component_status))
    {
      MYON_ERROR("cannot change component locked state.");
    }
  }
}


bool MYON_ModelComponent::IsHidden() const
{
  return m_component_status.IsHidden();
}

void MYON_ModelComponent::SetHiddenModelComponentState(
  bool bHiddenState
  )
{
  MYON_ComponentStatus component_status = ModelComponentStatus();
  if (bHiddenState != component_status.IsHidden())
  {
    component_status.SetHiddenState(bHiddenState);
    if (false == SetModelComponentStatus(component_status))
    {
      MYON_ERROR("cannot change component hidden state.");
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Index interface
//
//  The component index is intended to be unique among components with identical
//  types in the context of a model.
//

int MYON_ModelComponent::Index() const
{
  return m_component_index;
}

int MYON_ModelComponent::Index(
  int unset_index_value
  ) const
{
  return IndexIsSet() ? m_component_index : unset_index_value;
}

bool MYON_ModelComponent::SetIndex(
  int component_index
  )
{
  const unsigned int bit = MYON_ModelComponent::Attributes::IndexAttribute;
  if (0 != (bit & m_locked_status))
    return false;

  const bool bContentChange = (m_component_index !=  component_index);

  m_component_index = component_index;
  m_set_status |= bit;

  if ( bContentChange )
    IncrementContentVersionNumber();

  return true;
}

bool MYON_ModelComponent::ClearIndex()
{
  const unsigned int bit = MYON_ModelComponent::Attributes::IndexAttribute;
  return (bit == ClearModelComponentAttributes(bit));
}

void MYON_ModelComponent::LockIndex()
{
  m_locked_status |= MYON_ModelComponent::Attributes::IndexAttribute;
}

bool MYON_ModelComponent::IndexIsLocked() const
{
  const unsigned int bit = MYON_ModelComponent::Attributes::IndexAttribute;
  return (0 != (bit & m_locked_status));
}

bool MYON_ModelComponent::IndexIsSet() const
{
  const unsigned int bit = MYON_ModelComponent::Attributes::IndexAttribute;
  return (0 != (bit & m_set_status));
}  




///////////////////////////////////////////////////////////////////////////////
//
// ParentId interface
//
//  The component parent_id is intended to be unique among components with identical
//  types in the context of a model.
//

const MYON_UUID& MYON_ModelComponent::ParentId() const
{
  return m_component_parent_id;
}

bool MYON_ModelComponent::ParentIdIsNil() const
{
  return (MYON_nil_uuid == m_component_parent_id);
}

bool MYON_ModelComponent::ParentIdIsNotNil() const
{
  return (!(MYON_nil_uuid == m_component_parent_id));
}


bool MYON_ModelComponent::SetParentId(
  const MYON_UUID& component_parent_id
  )
{
  const unsigned int bit = MYON_ModelComponent::Attributes::ParentIdAttribute;
  if (0 != (bit & m_locked_status))
    return false;

  const bool bContentChange = (m_component_parent_id !=  component_parent_id);

  m_component_parent_id = component_parent_id;
  m_set_status |= bit;

  if (bContentChange)
  {
    if (MYON_ModelComponent::UniqueNameIncludesParent(m_component_type))
    {
      // The name hash includes the parent id for this type of component (like layers).
      // Lazy evaluation is used to set m_component_name_hash.
      m_component_name_hash = MYON_NameHash::EmptyNameHash;
    }
    IncrementContentVersionNumber();
  }

  return true;
}

bool MYON_ModelComponent::ClearParentId()
{
  const unsigned int bit = MYON_ModelComponent::Attributes::ParentIdAttribute;
  return (bit == ClearModelComponentAttributes(bit));
}

void MYON_ModelComponent::LockParentId()
{
  m_locked_status |= MYON_ModelComponent::Attributes::ParentIdAttribute;
}

bool MYON_ModelComponent::ParentIdIsLocked() const
{
  const unsigned int bit = MYON_ModelComponent::Attributes::ParentIdAttribute;
  return (0 != (bit & m_locked_status));
}

bool MYON_ModelComponent::ParentIdIsSet() const
{
  const unsigned int bit = MYON_ModelComponent::Attributes::ParentIdAttribute;
  return (0 != (bit & m_set_status));
}  


///////////////////////////////////////////////////////////////////////////////
//
// General attributes interface
//
//

unsigned int MYON_ModelComponent::ClearModelComponentAttributes(
  unsigned int attributes_filter
  )
{
  const unsigned int locked_status = (unsigned int)m_locked_status;
  unsigned int rc = 0;
  unsigned int bit;

  // Cannot modify locked attributes.
  attributes_filter &= ~locked_status;

  const unsigned int set_status0 = m_set_status;

  bit = MYON_ModelComponent::Attributes::ModelSerialNumberAttribute & attributes_filter;
  if ( 0 != bit )
  {
    m_model_serial_number = MYON_ModelComponent::Unset.m_model_serial_number;
    m_reference_model_serial_number = MYON_ModelComponent::Unset.m_reference_model_serial_number;
    m_linked_idef_serial_number = MYON_ModelComponent::Unset.m_linked_idef_serial_number;
    m_set_status &= ~bit;
    rc |= bit;
  }

  bit = MYON_ModelComponent::Attributes::IdAttribute & attributes_filter;
  if ( 0 != bit )
  {
    m_component_id = MYON_ModelComponent::Unset.m_component_id;
    m_set_status ^= bit;
    rc |= bit;
  }

  bit = MYON_ModelComponent::Attributes::ParentIdAttribute & attributes_filter;
  if ( 0 != bit )
  {
    m_component_parent_id = MYON_ModelComponent::Unset.m_component_id;
    m_set_status &= ~bit;
    rc |= bit;
  }

  bit = MYON_ModelComponent::Attributes::TypeAttribute & attributes_filter;
  if ( 0 != bit )
  {
    m_component_type = MYON_ModelComponent::Unset.m_component_type;
    m_set_status &= ~bit;
    rc |= bit;
  }

  bit = MYON_ModelComponent::Attributes::ComponentStatusAttribute & attributes_filter;
  if ( 0 != bit )
  {
    m_component_status = MYON_ModelComponent::Unset.m_component_status;
    m_set_status &= ~bit;
    rc |= bit;
  }

  bit = MYON_ModelComponent::Attributes::IndexAttribute & attributes_filter;
  if ( 0 != bit )
  {
    m_component_index = MYON_ModelComponent::Unset.m_component_index;
    m_set_status &= ~bit;
    rc |= bit;
  }

  bit = MYON_ModelComponent::Attributes::NameAttribute & attributes_filter;
  if ( 0 != bit )
  {
    if (0 == (m_set_status & MYON_ModelComponent::Attributes::DeletedNameAttribute))
    {
      m_component_name = MYON_wString::EmptyString;
      m_component_name_hash = MYON_NameHash::EmptyNameHash;
    }
    m_set_status &= ~bit;
    rc |= bit;
  }

  bit = MYON_ModelComponent::Attributes::DeletedNameAttribute & attributes_filter;
  if ( 0 != bit )
  {
    if (0 == (m_set_status & MYON_ModelComponent::Attributes::NameAttribute))
    {
      m_component_name = MYON_wString::EmptyString;
      m_component_name_hash = MYON_NameHash::EmptyNameHash;
    }
    m_set_status &= ~bit;
    rc |= bit;
  }

  if ( set_status0 != m_set_status )
    IncrementContentVersionNumber();

  return rc;
}

bool MYON_BinaryArchive::ReadModelComponentAttributes(
  MYON_ModelComponent& model_component,
  unsigned int* attributes_filter
  )
{
  unsigned int read_bits = 0;
  if ( nullptr != attributes_filter )
    *attributes_filter = read_bits;

  if (m_SetModelComponentSerialNumbers)
  {
    model_component.SetModelSerialNumber(
      this->m_model_serial_number,
      this->m_reference_model_serial_number,
      this->m_instance_definition_model_serial_number
      );
  }

  int major_version = 0;
  int minor_version = 0;
  if (false == BeginRead3dmChunk(TCODE_MODEL_ATTRIBUTES_CHUNK,&major_version,&minor_version))
    return false;

  bool rc = false;
  for (;;)
  {
    if ( 1 != major_version )
      break;

    unsigned int bit;
    unsigned char read_status;

    bit = MYON_ModelComponent::Attributes::ModelSerialNumberAttribute;
    read_status = 0;
    if (!ReadChar(&read_status))
      break;
    if (2 == read_status)
      model_component.ClearModelSerialNumber();
    else if (1 == read_status)
    {
      unsigned int model_serial_number = 0;
      unsigned int reference_model_serial_number = 0;
      unsigned int instance_definition_model_serial_number = 0;
      if ( !ReadInt(&model_serial_number) )
        break;
      if ( !ReadInt(&reference_model_serial_number) )
        break;
      if ( !ReadInt(&instance_definition_model_serial_number) )
        break;
      model_component.SetModelSerialNumber(
        model_serial_number,
        reference_model_serial_number,
        instance_definition_model_serial_number
        );
      read_bits |= bit;
    }

    bit = MYON_ModelComponent::Attributes::IdAttribute;
    read_status = 0;
    if (!ReadChar(&read_status))
      break;
    if (2 == read_status)
    {
      model_component.ClearId();
    }
    else if (1 == read_status)
    {
      MYON_UUID component_id = MYON_nil_uuid;
      if ( !ReadUuid(component_id) )
        break;
      model_component.SetId(component_id);
      read_bits |= bit;
    }

    bit = MYON_ModelComponent::Attributes::TypeAttribute;
    read_status = 0;
    if (!ReadChar(&read_status))
      break;
    if (2 == read_status)
      model_component.ClearComponentType();
    else if (1 == read_status)
    {
      unsigned int component_type_as_unsigned = 0;
      if ( !ReadInt(&component_type_as_unsigned) )
        break;
      model_component.SetComponentType(MYON_ModelComponent::ComponentTypeFromUnsigned(component_type_as_unsigned));
      read_bits |= bit;
    }

    bit = MYON_ModelComponent::Attributes::IndexAttribute;
    read_status = 0;
    if (!ReadChar(&read_status))
      break;
    if (2 == read_status)
      model_component.ClearIndex();
    else if (1 == read_status)
    {
      int component_index = 0;
      if ( !ReadInt(&component_index) )
        break;
      model_component.SetIndex(component_index);
      read_bits |= bit;
    }

    bit = MYON_ModelComponent::Attributes::NameAttribute;
    read_status = 0;
    if (!ReadChar(&read_status))
      break;
    if (2 == read_status)
      model_component.ClearName();
    else if (1 == read_status)
    {
      MYON_wString component_name;
      if ( !ReadString(component_name) )
        break;
      model_component.SetName(static_cast<const wchar_t*>(component_name));
      read_bits |= bit;
    }

    rc = true;
    break;
  }

  if ( nullptr != attributes_filter)
    *attributes_filter = read_bits;

  if (false == EndRead3dmChunk())
    rc = false;

  return rc;
}

class MYON_ModelComponentOverrideStackElement
{
public:
  MYON_ModelComponentOverrideStackElement(
    const MYON_ModelComponent& original_model_component,
    const MYON_ModelComponent& model_component_override
    )
    : m_original_model_component(original_model_component)
    , m_model_component_override(model_component_override)
  {}
  ~MYON_ModelComponentOverrideStackElement() = default;

  const MYON_ModelComponent& m_original_model_component;
  const MYON_ModelComponent& m_model_component_override;
  MYON_ModelComponentOverrideStackElement* m_next = nullptr;

private:
  MYON_ModelComponentOverrideStackElement(const MYON_ModelComponentOverrideStackElement&) = delete;
  MYON_ModelComponentOverrideStackElement& operator=(const MYON_ModelComponentOverrideStackElement&) = delete;
};

bool MYON_BinaryArchive::WriteModelComponentAttributes(
  const class MYON_ModelComponent& model_component,
  unsigned int attributes_filter
  )
{
  if (false == BeginWrite3dmChunk(TCODE_MODEL_ATTRIBUTES_CHUNK,1,0))
    return false;

  bool rc = false;
  for (;;)
  {
    unsigned int bit = MYON_ModelComponent::Attributes::ModelSerialNumberAttribute & attributes_filter;
    unsigned char write_status = (0 != bit) ? 1 : 0;
    if ( 0 != write_status && false == model_component.ModelSerialNumberIsSet() )
      write_status = 2;
    if (!WriteChar(write_status))
      break;
    if (1 == write_status )
    {
      const unsigned int model_serial_number = model_component.ModelSerialNumber();
      const unsigned int reference_model_serial_number = model_component.ReferenceModelSerialNumber();
      const unsigned int instance_definition_model_serial_number = model_component.InstanceDefinitionModelSerialNumber();
      if ( !WriteInt(model_serial_number) )
        break;
      if ( !WriteInt(reference_model_serial_number) )
        break;
      if ( !WriteInt(instance_definition_model_serial_number) )
        break;
    }

    bit = MYON_ModelComponent::Attributes::IdAttribute & attributes_filter;
    write_status = (0 != bit) ? 1 : 0;
    if ( 0 != write_status && false == model_component.IdIsSet() )
      write_status = 2;
    if (!WriteChar(write_status))
      break;
    if (1 == write_status )
    {
      MYON_UUID component_id = model_component.Id();
      if ( !WriteUuid(component_id) )
        break;
    }

    bit = MYON_ModelComponent::Attributes::TypeAttribute & attributes_filter;
    write_status = (0 != bit) ? 1 : 0;
    if ( 0 != write_status && false == model_component.ComponentTypeIsSet() )
      write_status = 2;
    if (!WriteChar(write_status))
      break;
    if (1 == write_status )
    {
      unsigned int component_type_as_unsigned = static_cast<unsigned int>(model_component.ComponentType());
      if ( !WriteInt(component_type_as_unsigned) )
        break;
    }

    bit = MYON_ModelComponent::Attributes::IndexAttribute & attributes_filter;
    write_status = (0 != bit) ? 1 : 0;
    if ( 0 != write_status && false == model_component.IndexIsSet() )
      write_status = 2;
    if (!WriteChar(write_status))
      break;
    if (1 == write_status )
    {
      if ( !Write3dmReferencedComponentIndex( model_component ) )
        break;
    }

    bit = MYON_ModelComponent::Attributes::NameAttribute & attributes_filter;
    write_status = (0 != bit) ? 1 : 0;
    if ( 0 != write_status && false == model_component.NameIsSet() )
      write_status = 2;
    if (!WriteChar(write_status))
      break;
    if (1 == write_status )
    {
      if (false == this->WriteModelComponentName(model_component) )
        break;
    }

    rc = true;
    break;
  }

  if (false == EndWrite3dmChunk())
    rc = false;

  return rc;
}

bool MYON_BinaryArchive::Write3dmReferencedComponentIndex(
  const MYON_ModelComponent& model_component
  )
{
  if (ReferencedComponentIndexMapping())
  {
    // During writing, the m_manifest member stores 
    // the model id and index as the "Component" values and
    // the archive id and index as the "Manifest" values.
    const MYON_UUID component_id = model_component.Id();
    return
      (MYON_nil_uuid == component_id)
      ? Write3dmReferencedComponentIndex(model_component.ComponentType(), model_component.Index())
      : Write3dmReferencedComponentIndex(model_component.ComponentType(), component_id);
  }

  // no component mapping.
  return WriteInt(model_component.Index());
}

bool MYON_BinaryArchive::Read3dmReferencedComponentIndexArray(
  MYON_ModelComponent::Type component_type,
  MYON_SimpleArray<int>& component_index_array
)
{
  if (!ReadArray(component_index_array))
    return false;
  if (false == ReferencedComponentIndexMapping())
    return true; // true means continue reading

  int count = 0;
  for (int i = 0; i < component_index_array.Count(); i++)
  {
    int archive_index = component_index_array[i];
    if (MYON_ModelComponent::Type::TextStyle == component_type
      && archive_index >= 0 
      && archive_index < m_text_style_to_dim_style_archive_index_map.Count()
      )
    {
      const MYON_2dex ts_to_ds = m_text_style_to_dim_style_archive_index_map[archive_index];
      if (archive_index == ts_to_ds.i && ts_to_ds.j >= 0)
      {
        archive_index = ts_to_ds.j;
        component_type = MYON_ModelComponent::Type::DimStyle;
      }
    }
    int component_index = archive_index;
    component_index_array[i] = 0;
    const MYON_ManifestMapItem& map_item = ManifestMap().MapItemFromSourceIndex(component_type, archive_index);
    if (false == map_item.SourceAndDestinationAreSet() || MYON_UNSET_INT_INDEX == map_item.DestinationIndex())
    {
      MYON_ERROR("Unable to update component reference index.");
      continue;
    }
    component_index = map_item.DestinationIndex();
    component_index_array[count++] = component_index;
  }
  component_index_array.SetCount(count);

  return true; // true means continue reading
}

bool MYON_BinaryArchive::Read3dmReferencedComponentIndex(
  MYON_ModelComponent::Type component_type,
  int* component_index
  )
{
  if (nullptr != component_index)
    *component_index = MYON_UNSET_INT_INDEX;

  int archive_component_index = MYON_UNSET_INT_INDEX;
  if (!ReadInt(&archive_component_index))
    return false;
  
  if (archive_component_index < 0 || false == ReferencedComponentIndexMapping() )
  {
    // system component with a persistent negative index
    if (nullptr != component_index)
      *component_index = archive_component_index;
    return true;
  }

  int updated_component_index = archive_component_index;

  if (MYON_ModelComponent::Type::TextStyle == component_type)
  {
    component_type = MYON_ModelComponent::Type::DimStyle;
    const int archive_text_style_index = archive_component_index;
    archive_component_index = MYON_UNSET_INT_INDEX;
    if (archive_text_style_index >= 0
      && archive_text_style_index < m_text_style_to_dim_style_archive_index_map.Count()
      )
    {
      const MYON_2dex ts_to_ds = m_text_style_to_dim_style_archive_index_map[archive_text_style_index];
      if (archive_text_style_index == ts_to_ds.i && ts_to_ds.j >= 0)
      {
        archive_component_index = ts_to_ds.j;
      }
    }
  }

  if (MYON_ModelComponent::Type::DimStyle == component_type)
  {
    if (this->Archive3dmVersion() <= 50)
    {
      if (nullptr != component_index)
        *component_index = archive_component_index;

      return true;
    }
    const int archive_dim_style_index = archive_component_index;
    if (archive_dim_style_index >= 0
      && archive_dim_style_index < m_archive_dim_style_table.Count()
      )
    {
      const MYON_DimStyle* archive_dim_style = m_archive_dim_style_table[archive_dim_style_index];
      if (nullptr != archive_dim_style)
      {
        if (archive_dim_style->ParentIdIsNotNil())
        {
          // override dimstyle - not in manifests or dimstyle table.
          if (nullptr != component_index)
            *component_index = archive_component_index;

          return true;
        }

        const MYON_ComponentManifestItem& item = m_manifest.ItemFromId(MYON_ModelComponent::Type::DimStyle,archive_dim_style->Id());
        if ( item.Id() == archive_dim_style->Id() )
        {
          archive_component_index = item.Index();
        }
      }
    }
  }

  const MYON_ManifestMapItem& map_item = m_manifest_map.MapItemFromSourceIndex(component_type,archive_component_index);
  if (map_item.SourceAndDestinationAreSet() && MYON_UNSET_INT_INDEX != map_item.DestinationIndex())
  {
    updated_component_index = map_item.DestinationIndex();
  }
  else
  {
    MYON_ERROR("Unable to update component index.");

    // Get something that might work when possible
    switch (component_type)
    {
    case MYON_ModelComponent::Type::Image:
      break;
    case MYON_ModelComponent::Type::TextureMapping:
      break;
    case MYON_ModelComponent::Type::Material:
      updated_component_index = MYON_Material::Default.Index();
      break;
    case MYON_ModelComponent::Type::LinePattern:
      updated_component_index = MYON_Linetype::Continuous.Index();
      break;
    case MYON_ModelComponent::Type::Layer:
      break;
    case MYON_ModelComponent::Type::Group:
      break;

    case MYON_ModelComponent::Type::TextStyle:
    case MYON_ModelComponent::Type::DimStyle:
      if (nullptr != m_archive_current_dim_style)
      {
        const MYON_ManifestMapItem& archive_current_dim_style_map_item = m_manifest_map.MapItemFromSourceIndex(MYON_ModelComponent::Type::DimStyle,m_archive_current_dim_style->Index());
        if (archive_current_dim_style_map_item.SourceAndDestinationAreSet()
          && MYON_UNSET_INT_INDEX != archive_current_dim_style_map_item.DestinationIndex()
          )
        {
          updated_component_index = archive_current_dim_style_map_item.DestinationIndex();
        }
      }
      break;

    case MYON_ModelComponent::Type::RenderLight:
      break;
    case MYON_ModelComponent::Type::HatchPattern:
      break;
    case MYON_ModelComponent::Type::InstanceDefinition:
      break;
    case MYON_ModelComponent::Type::ModelGeometry:
      break;
    case MYON_ModelComponent::Type::HistoryRecord:
      break;
    default:
      break;
    }
  }

  if (nullptr != component_index)
    *component_index = updated_component_index;

  return true; // true indicates the read worked, even if the component index cannot be updated.
}

bool MYON_BinaryArchive::Write3dmReferencedComponentIndex(
  MYON_ModelComponent::Type component_type,
  int model_index
  )
{
  // When writing 3dm archives, the model index needs to be converted
  // to an archive index.  This function is used when the model component
  // id is not known (for example, when writing MYON_Layer material and
  // line pattern indexes.
  int archive_index = model_index;

  if (model_index >= 0)
  {
    if (MYON_ModelComponent::Type::TextStyle == component_type)
    {
      // In version 6, August 2016, text style information was moved to MYON_DimStyle.
      //
      // In order to write files that can be read by earlier versions of Rhino,
      // a text style table that is parallel to the dimstyle table is saved in the 3d archive.

      if (MYON_3dmArchiveTableType::dimension_style_table == m_3dm_active_table)
      {
        // The text style index of a dimstyle is being written. This is needed
        // so V5 and V6 Rhino WIPs built before August 2016 will be able to read
        // the file.
        const int text_style_count = m_manifest.ActiveComponentCount(MYON_ModelComponent::Type::TextStyle);
        const int dim_style_count = m_manifest.ActiveComponentCount(MYON_ModelComponent::Type::DimStyle);
        if (dim_style_count > 0 && dim_style_count <= text_style_count)
          archive_index = dim_style_count - 1;
        else if (text_style_count > 0)
          archive_index = 0;
        else
          archive_index = -1;
        return WriteInt(archive_index);
      }

      if (MYON_3dmArchiveTableType::text_style_table != m_3dm_active_table)
      {
        // We are finished writing the text style and dim style tables.
        // There is a parallel set of text styles and dimension styles.
        // To get the right index in the archive for older versions of Rhino, change the component type to dimstyle.
        component_type = MYON_ModelComponent::Type::DimStyle;
      }
    }

    if ( ReferencedComponentIndexMapping() )
    {
      const MYON_ManifestMapItem& map_item = ManifestMap().MapItemFromSourceIndex(component_type, model_index);
      if (
        component_type == map_item.ComponentType()
        && MYON_UNSET_INT_INDEX != map_item.DestinationIndex()
        )
      {
        archive_index = map_item.DestinationIndex();
      }
      else
      {
        MYON_ERROR("unable to convert model index to archive index.");
        archive_index = model_index;
      }
    }
  }
  return WriteInt(archive_index);
}

bool MYON_BinaryArchive::Write3dmReferencedComponentIndex(
  MYON_ModelComponent::Type component_type,
  MYON_UUID model_id
  )
{
  // When writing 3dm archives, the model index needs to be converted
  // to an archive index.  This function is used when the model component
  // id is known.
  int archive_index = MYON_UNSET_INT_INDEX;
  if (MYON_nil_uuid == model_id)
  {
    MYON_ERROR("model_id is nil.");
  }
  else
  {
    const MYON_ManifestMapItem& map_item = ManifestMap().MapItemFromSourceId(model_id);
    if (component_type != map_item.ComponentType() 
      || false == map_item.SourceIsSet()
      )
    {
      MYON_ERROR("model_id not in arcive manifest map.");
    }
    else if (false == ReferencedComponentIndexMapping())
    {
      // component reference mapping is disabled (rare)
      archive_index = map_item.SourceIndex();
    }
    else
    {
      // adjust component reference to use the index in the archive (common)
      if (MYON_UNSET_INT_INDEX != map_item.DestinationIndex())
      {
        archive_index = map_item.DestinationIndex();
      }
      else
      {
        MYON_ERROR("Unable to get archive component reference index.");
      }
    }
  }
  return WriteInt(archive_index);
}

bool MYON_BinaryArchive::ReferencedComponentIndexMapping() const
{
  return m_bReferencedComponentIndexMapping;
}

void MYON_BinaryArchive::SetReferencedComponentIndexMapping(
  bool bEnableReferencedComponentIndexMapping
  )
{
  const bool b = bEnableReferencedComponentIndexMapping ? true : false;
  if (b != m_bReferencedComponentIndexMapping)
    m_bReferencedComponentIndexMapping = b;
}

bool MYON_BinaryArchive::Write3dmReferencedComponentId(
  MYON_ModelComponent::Type component_type,
  MYON_UUID model_component_id
)
{
  return WriteUuid(model_component_id);
}

bool MYON_BinaryArchive::Write3dmReferencedComponentId(
  const MYON_ModelComponent& model_component
)
{
  return WriteUuid(model_component.Id());
}

bool MYON_BinaryArchive::Read3dmReferencedComponentId(
  MYON_ModelComponent::Type component_type,
  MYON_UUID* component_id
)
{
  MYON_UUID id = MYON_nil_uuid;
  const bool rc = ReadUuid(id);

  MYON_UUID default_id = MYON_nil_uuid;

  bool bConvertToModelId = true;
  switch (component_type)
  {
  case MYON_ModelComponent::Type::Unset:
    bConvertToModelId = false;
    break;
  case MYON_ModelComponent::Type::Image:
    break;
  case MYON_ModelComponent::Type::TextureMapping:
    break;
  case MYON_ModelComponent::Type::Material:
    break;
  case MYON_ModelComponent::Type::RenderContent: // JohnC: I have no idea what to do here.
    break;
  case MYON_ModelComponent::Type::EmbeddedFile:  // JohnC: I have no idea what to do here.
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
    default_id = this->ArchiveCurrentDimStyleId();
    break;
  case MYON_ModelComponent::Type::RenderLight:
    break;
  case MYON_ModelComponent::Type::HatchPattern:
    break;
  case MYON_ModelComponent::Type::InstanceDefinition:
    break;
  case MYON_ModelComponent::Type::ModelGeometry:
    break;
  case MYON_ModelComponent::Type::HistoryRecord:
    break;
  case MYON_ModelComponent::Type::Mixed:
    bConvertToModelId = false;
    break;
  default:
    bConvertToModelId = false;
    break;
  }

  if (false == rc || MYON_nil_uuid == id )
  {
    id = default_id;
  }
  else if ( MYON_nil_uuid != id && bConvertToModelId )
  {
    const MYON_ComponentManifestItem& item = this->Manifest().ItemFromId(component_type, id);
    if (component_type == item.ComponentType() && item.Id() == id )
    {
      // expected ...
      const MYON_ManifestMapItem& map_item = this->ManifestMap().MapItemFromSourceId(id);
      if (component_type == map_item.ComponentType()
        && map_item.SourceId() == id
        && MYON_nil_uuid != map_item.DestinationId()
        )
      {
        // ... everything is as it should be. Assign id used in model.
        id = map_item.DestinationId();
      }
      // else 
      // {
      //   Either the id was unchanged or the reading code failed 
      //   to set as (source,destination) pair in the manifest map.
      //   It's not an error condition.
      // }
    }
    else
    {
      // complicated situations 
      switch (component_type)
      {
      case MYON_ModelComponent::Type::Unset:
        break;
      case MYON_ModelComponent::Type::Image:
        break;
      case MYON_ModelComponent::Type::TextureMapping:
        break;
      case MYON_ModelComponent::Type::Material:
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
        if (MYON_ModelComponent::Type::DimStyle != item.ComponentType() 
          && MYON_DimStyle::SystemDimstyleFromId(id).Id() != id
          )
        {
          const unsigned int fixed_version = MYON_VersionNumberConstruct(6, 0, 2017, 1, 15, 0);
          const unsigned int archive_version = this->ArchiveOpenNURBSVersion();
          if (archive_version >= fixed_version)
          {
            // Should not happen
            MYON_ERROR("dimstyle id not in archive.");
          }
          id = default_id;
        }
        break;
      case MYON_ModelComponent::Type::RenderLight:
        break;
      case MYON_ModelComponent::Type::HatchPattern:
        break;
      case MYON_ModelComponent::Type::InstanceDefinition:
        break;
      case MYON_ModelComponent::Type::ModelGeometry:
        break;
      case MYON_ModelComponent::Type::HistoryRecord:
        break;
      case MYON_ModelComponent::Type::Mixed:
        break;
      default:
        break;
      }
    }
  }

  if (nullptr != component_id)
    *component_id = id;

  return rc;
}

bool MYON_BinaryArchive::ReferencedComponentIdMapping() const
{
  return m_bReferencedComponentIdMapping;
}

void MYON_BinaryArchive::SetReferencedComponentIdMapping(
  bool bEnableReferenceComponentIdMapping
)
{
  m_bReferencedComponentIdMapping
    = bEnableReferenceComponentIdMapping
    ? true
    : false;
}

const MYON_ComponentManifest& MYON_BinaryArchive::Manifest() const
{
  return m_manifest;
}

const MYON_ManifestMap& MYON_BinaryArchive::ManifestMap() const
{
  return m_manifest_map;
}

bool MYON_BinaryArchive::UpdateManifestMapItemDestination(
  const class MYON_ManifestMapItem& map_item
  )
{
  if (MYON::archive_mode::read3dm != this->Mode())
  {
    MYON_ERROR("archive mode != MYON::archive_mode::read3dm");
    return false;
  }
  if (map_item.SourceIsUnset())
  {
    MYON_ERROR("map_item source information is not set.");
    return false;
  }

  const bool bIgnoreSourceIndex = (MYON_ModelComponent::Type::Group == map_item.ComponentType());

  return m_manifest_map.UpdatetMapItemDestination(map_item, bIgnoreSourceIndex);
}


bool MYON_BinaryArchive::Internal_Write3dmUpdateManifest(
  const MYON_ModelComponent& model_component
  )
{
  bool rc = false;
  for (;;)
  {
    const MYON_ComponentManifestItem& manifest_item = m_manifest.AddComponentToManifest(model_component, false, nullptr);
    if (manifest_item.IsUnset())
      break;
    MYON_ManifestMapItem map_item;
    if (!map_item.SetSourceIdentification(&model_component))
      break;
    if (!map_item.SetDestinationIdentification(&manifest_item))
      break;

    rc = m_manifest_map.AddMapItem(map_item);
    break;
  }
  if (!rc)
  {
    MYON_ERROR("Unable to update archive manifest and map.");
  }
  return rc;
}

bool MYON_BinaryArchive::Internal_Write3dmLightOrGeometryUpdateManifest(
  MYON_ModelComponent::Type component_type,
  MYON_UUID component_id,
  int component_index,
  const MYON_wString& component_name
  )
{
  bool rc = false;
  for (;;)
  {
    const MYON__UINT64 no_serial_number_yet = 0;
    const MYON_NameHash component_name_hash = MYON_NameHash::Create(MYON_nil_uuid, component_name);
    const MYON_ComponentManifestItem& manifest_item = m_manifest.AddComponentToManifest(component_type, no_serial_number_yet, component_id, component_name_hash);
    if (manifest_item.IsUnset())
      break;

    if (MYON_ModelComponent::IndexRequired(component_type))
    {
      if (component_index < 0)
      {
        MYON_ERROR("component_index should have value >= 0");
        component_index = manifest_item.Index();
      }
    }
    else
    {
      if (MYON_UNSET_INT_INDEX != component_index)
      {
        MYON_ERROR("component_index should have value MYON_UNSET_INT_INDEX");
        component_index = MYON_UNSET_INT_INDEX;
      }
    }

    MYON_ManifestMapItem map_item;
    if (!map_item.SetSourceIdentification(component_type, component_id, component_index))
      break;
    if (!map_item.SetDestinationIdentification(&manifest_item))
      break;

    rc = m_manifest_map.AddMapItem(map_item);
    break;
  }
  if (!rc)
  {
    MYON_ERROR("Unable to update archive manifest and map.");
  }
  return rc;
}

bool MYON_BinaryArchive::Internal_Read3dmUpdateManifest(
  MYON_ModelComponent& model_component
  )
{
  const bool bResolveIdAndNameCollisions = true;
  bool rc = false;
  for (;;)
  {
    const MYON_ComponentManifestItem& item_from_id
      = model_component.IdIsNotNil()
      ? m_manifest.ItemFromId(model_component.Id())
      : MYON_ComponentManifestItem::UnsetItem;

    const MYON_ComponentManifestItem& item_from_name
      = ( model_component.NameIsNotEmpty() && MYON_ModelComponent::UniqueNameRequired(model_component.ComponentType()) )
      ? m_manifest.ItemFromName(&model_component)
      : MYON_ComponentManifestItem::UnsetItem;

    if (item_from_id.IsValid())
    {
      // id conflict - this is bad
      MYON_WARNING("id conflict - 3dm file is not valid.");
      if (item_from_id.ComponentType() == model_component.ComponentType())
      {
        // duplicate components - first one wins and no remapping the id
        model_component.ClearId();
        model_component.SetId(); // generates new id
      }
    }

    if (item_from_name.IsValid() && false == item_from_name.IsSystemComponent() )
    {
      // name conflict but not id conflict - still bad
      // first name wins - no remapping
      if (false == item_from_id.IsValid())
      {
        // one warning is enough
        MYON_WARNING("name conflict - 3dm file is not valid.");
      }
      model_component.SetName(m_manifest.UnusedName(model_component));
    }

    const bool bIndexRequired = MYON_ModelComponent::IndexRequired(model_component.ComponentType());
    const int archive_index = model_component.Index();

    MYON_wString assigned_name;
    const MYON_ComponentManifestItem& manifest_item = m_manifest.AddComponentToManifest(model_component, bResolveIdAndNameCollisions, &assigned_name);
    if (manifest_item.IsUnset())
      break;
    if (bResolveIdAndNameCollisions && manifest_item.IsValid())
    {
      if (manifest_item.Id() != model_component.Id())
      {
        MYON_WARNING("new id assigned to model component.");
        model_component.SetId(manifest_item.Id());
      }
      if (false == MYON_wString::EqualOrdinal(model_component.Name(),assigned_name,false))
      {
        MYON_WARNING("new name assigned to model component.");
        model_component.SetName(assigned_name);
      }

      // In damaged files, the index values are a mess.
      // Fix them here.
      const int manifest_index = manifest_item.Index();
      const int assigned_index
        = bIndexRequired
        ? (manifest_index >= 0 ? manifest_index : archive_index)
        : MYON_UNSET_INT_INDEX;

      if (assigned_index != archive_index)
      {
        if (bIndexRequired) // This is a good location for a debugger breakpoint when debugging file reading index issues.
          model_component.SetIndex(manifest_item.Index());
        else
          model_component.ClearIndex();
      }
    }

    MYON_ComponentManifestItem source_item(manifest_item);
    if (MYON_ModelComponent::Type::Group == model_component.ComponentType()
      && archive_index >= 0
      && manifest_item.Index() >= 0
      && archive_index != manifest_item.Index()
      )
    {
      // group index values are not always the zero based index of the
      // group element's location in the archive.
      source_item.SetIndex(archive_index);
    }

    MYON_ManifestMapItem map_item;
    // Rhino will update destination identification if it changes when added to the model.
    if (!map_item.SetSourceIdentification(&source_item))
      break;
    if (!map_item.SetDestinationIdentification(&model_component))
      break;
    rc = m_manifest_map.AddMapItem(map_item);
    break;
  }
  if (!rc)
  {
    MYON_ERROR("Unable to update archive manifest and map.");
  }
  return rc;
}

bool MYON_BinaryArchive::Internal_Read3dmLightOrGeometryUpdateManifest(
  MYON_ModelComponent::Type component_type,
  MYON_UUID component_id,
  int component_index,
  const MYON_wString& component_name
  )
{
  bool rc = false;
  for (;;)
  {
    const MYON__UINT64 no_serial_number_yet = 0;
    const MYON_NameHash component_name_hash = MYON_NameHash::Create(MYON_nil_uuid, component_name);
    const MYON_ComponentManifestItem& manifest_item = m_manifest.AddComponentToManifest(component_type, no_serial_number_yet, component_id, component_name_hash);
    if (manifest_item.IsUnset())
      break;

    if (MYON_ModelComponent::IndexRequired(component_type))
    {
      if (component_index < 0)
      {
        MYON_ERROR("component_index should have value >= 0");
        component_index = manifest_item.Index();
      }
    }
    else
    {
      if (MYON_UNSET_INT_INDEX != component_index)
      {
        MYON_ERROR("component_index should have value MYON_UNSET_INT_INDEX");
        component_index = MYON_UNSET_INT_INDEX;
      }
    }

    MYON_ManifestMapItem map_item;
    // Rhino will update destination if it changes id or index
    if (!map_item.SetDestinationIdentification(component_type, component_id, component_index))
      break;
    if (!map_item.SetSourceIdentification(&manifest_item))
      break;

    rc = m_manifest_map.AddMapItem(map_item);
    break;
  }
  if (!rc)
  {
    MYON_ERROR("Unable to update archive manifest and map.");
  }
  return rc;

}


bool MYON_BinaryArchive::AddManifestMapItem(
	const class MYON_ManifestMapItem& map_item
  )
{
  if (false == map_item.SourceAndDestinationAreSet() )
  {
    MYON_ERROR("map_item source and destination are not set.");
    return false;
  }
  const bool bIndexRequired = MYON_ModelComponent::IndexRequired(map_item.ComponentType());
  if (bIndexRequired && map_item.SourceIndex() < 0)
  {
    MYON_ERROR("map_item.m_source_component_index is not set.");
    return false;
  }
  switch (Mode())
  {
  case MYON::archive_mode::read3dm:
    {
      const MYON_ComponentManifestItem& archive_item_from_id = m_manifest.ItemFromId(map_item.SourceId());
      if (
        map_item.ComponentType() != archive_item_from_id.ComponentType()
        || map_item.SourceId() != archive_item_from_id.Id()
        || map_item.SourceIndex() != archive_item_from_id.Index()
        )
      {
        MYON_ERROR("map_item.m_source_component_id is not in the archive manifest.");
        return false;
      }
      if (bIndexRequired)
      {
        const MYON_ComponentManifestItem& archive_item_from_index = m_manifest.ItemFromIndex(map_item.ComponentType(), map_item.SourceIndex());
        if (
          map_item.ComponentType() != archive_item_from_index.ComponentType()
          || map_item.SourceId() != archive_item_from_index.Id()
          || map_item.SourceIndex() != archive_item_from_index.Index()
          )
        {
          MYON_ERROR("map_item.m_source_component_index is not in the archive manifest.");
          return false;
        }
      }
    }
    break;
  case MYON::archive_mode::write3dm:
    {
      const MYON_ComponentManifestItem& archive_item_from_id = m_manifest.ItemFromId(map_item.DestinationId());
      if (
        map_item.ComponentType() != archive_item_from_id.ComponentType()
        || map_item.DestinationId() != archive_item_from_id.Id()
        || map_item.DestinationIndex() != archive_item_from_id.Index()
        )
      {
        MYON_ERROR("map_item.m_destination_component_id is not in the archive manifest.");
        return false;
      }
      if (bIndexRequired)
      {
        const MYON_ComponentManifestItem& archive_item_from_index = m_manifest.ItemFromIndex(map_item.ComponentType(), map_item.DestinationIndex());
        if (
          map_item.ComponentType() != archive_item_from_index.ComponentType()
          || map_item.DestinationId() != archive_item_from_index.Id()
          || map_item.DestinationIndex() != archive_item_from_index.Index()
          )
        {
          MYON_ERROR("map_item.m_destination_component_index is not in the archive manifest.");
          return false;
        }
      }
    }
    break;

  default:
    break;
  }

  if (!m_manifest_map.AddMapItem(map_item))
  {
    MYON_ERROR("m_manifest_map.AddMapItem(map_item) failed.");
  }

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//
//

// Explicit implementation to insure m_sp is completely managed in the openurbs DLL.
MYON_ModelComponentWeakReference::MYON_ModelComponentWeakReference() MYON_NOEXCEPT
{}

// Explicit implementation to insure m_sp is completely managed in the openurbs DLL.
MYON_ModelComponentWeakReference::~MYON_ModelComponentWeakReference()
{}

// Explicit implementation to insure m_sp is completely managed in the openurbs DLL.
MYON_ModelComponentWeakReference::MYON_ModelComponentWeakReference(const MYON_ModelComponentWeakReference& src) MYON_NOEXCEPT
  : m_wp(src.m_wp)
{}

// Explicit implementation to insure m_sp is completely managed in the openurbs DLL.
MYON_ModelComponentWeakReference& MYON_ModelComponentWeakReference::operator=(const MYON_ModelComponentWeakReference& src)
{
  if ( this != &src)
    m_wp = src.m_wp;
  return *this;
}
  
#if defined(MYON_HAS_RVALUEREF)
MYON_ModelComponentWeakReference::MYON_ModelComponentWeakReference( MYON_ModelComponentWeakReference&& src ) MYON_NOEXCEPT
  : m_wp(std::move(src.m_wp))
{}

MYON_ModelComponentWeakReference& MYON_ModelComponentWeakReference::operator=(MYON_ModelComponentWeakReference&& src)
{
  if ( this != &src )
  {
    m_wp.reset();
    m_wp = std::move(src.m_wp);
  }
  return *this;
}

#endif

MYON_ModelComponentWeakReference::MYON_ModelComponentWeakReference(const MYON_ModelComponentReference& src)  MYON_NOEXCEPT
  : m_wp(src.m_sp)
{}

MYON_ModelComponentWeakReference& MYON_ModelComponentWeakReference::operator=(const MYON_ModelComponentReference& src)
{
  m_wp = src.m_sp;
  return *this;
}


//////////////////////////////////////////////////////////////////////////////////////////////
//
//
//

// Explicit implementation to insure m_sp is completely managed in the openurbs DLL.
MYON_ModelComponentReference::MYON_ModelComponentReference() MYON_NOEXCEPT
{}

// Explicit implementation to insure m_sp is completely managed in the openurbs DLL.
MYON_ModelComponentReference::~MYON_ModelComponentReference()
{}

// Explicit implementation to insure m_sp is completely managed in the openurbs DLL.
MYON_ModelComponentReference::MYON_ModelComponentReference(const MYON_ModelComponentReference& src) MYON_NOEXCEPT
  : m_sp(src.m_sp)
{}

// Explicit implementation to insure m_sp is completely managed in the openurbs DLL.
MYON_ModelComponentReference& MYON_ModelComponentReference::operator=(const MYON_ModelComponentReference& src)
{
  if ( this != &src)
    m_sp = src.m_sp;
  return *this;
}
  
#if defined(MYON_HAS_RVALUEREF)
MYON_ModelComponentReference::MYON_ModelComponentReference( MYON_ModelComponentReference&& src ) MYON_NOEXCEPT
  : m_sp(std::move(src.m_sp))
{}

MYON_ModelComponentReference& MYON_ModelComponentReference::operator=(MYON_ModelComponentReference&& src)
{
  if ( this != &src )
  {
    m_sp.reset();
    m_sp = std::move(src.m_sp);
  }
  return *this;
}

#endif

MYON_ModelComponentReference::MYON_ModelComponentReference(const MYON_ModelComponentWeakReference& src) MYON_NOEXCEPT
  : m_sp(src.m_wp.lock())
{
  // NOTE WELL: 
  //   std::shared_ptr<T>(std::weak_ptr<T>) throws an exception when weak_ptr is empty.
  //   std::shared_ptr<T>(std::weak_ptr<T>.lock()) constructs and empty shared_ptr when weak_ptr is empty.
}


MYON_ModelComponentReference& MYON_ModelComponentReference::operator=(const MYON_ModelComponentWeakReference& src)
{
  m_sp = src.m_wp.lock();
  return *this;
}


MYON_ModelComponentReference::MYON_ModelComponentReference(
  std::shared_ptr<MYON_ModelComponent>& sp
  ) MYON_NOEXCEPT
  : m_sp(sp)
{}

MYON_ModelComponentReference& MYON_ModelComponentReference::operator=(std::shared_ptr<MYON_ModelComponent>& sp)
{
  m_sp = sp;
  return *this;
}

MYON_ModelComponentReference MYON_ModelComponentReference::CreateConstantSystemComponentReference(
  const class MYON_ModelComponent& system_model_component
  ) MYON_NOEXCEPT
{
  if ( system_model_component.IsSystemComponent())
    return CreateForExperts(const_cast<MYON_ModelComponent*>(&system_model_component),false);

  MYON_ERROR("Invalid system_model_component parameter.");
  return MYON_ModelComponentReference::Empty;
}


MYON_ModelComponentReference MYON_ModelComponentReference::CreateForExperts(
  class MYON_ModelComponent* model_component,
  bool bManagedComponentReference
  ) MYON_NOEXCEPT
{
  if ( nullptr == model_component )
    return MYON_ModelComponentReference::Empty;
  if (bManagedComponentReference && model_component->IsSystemComponent())
  {
    // If you get this error, you have const cast a system component
    // and there will be a hard to diagnose crash when the the
    // last shared pointer attempts to delete the constant system component.
    //
    // You should be using MYON_ModelComponentReference::CreateConstantSystemComponentReference.
    MYON_ERROR("The future attempt to delete model_component will crash the application.");
    return MYON_ModelComponentReference::Empty;
  }
  MYON_ModelComponentReference r;
  r.m_sp 
    = bManagedComponentReference
    ? MYON_MANAGED_SHARED_PTR(MYON_ModelComponent,model_component) // default calls delete model_component
    : MYON_UNMANAGED_SHARED_PTR(MYON_ModelComponent,model_component);
  return r;
}

const class MYON_ModelComponent* MYON_ModelComponentReference::ModelComponent() const MYON_NOEXCEPT
{
  return m_sp.get();
}

class MYON_ModelComponent* MYON_ModelComponentReference::ExclusiveModelComponent() const MYON_NOEXCEPT
{
  return 
    (1 == m_sp.use_count())
    ? m_sp.get()
    : nullptr;
}

MYON__UINT64 MYON_ModelComponentReference::ModelComponentRuntimeSerialNumber() const MYON_NOEXCEPT
{
  const MYON_ModelComponent* model_component = m_sp.get(); 
  return (nullptr != model_component) ? model_component->RuntimeSerialNumber() : 0;
}

const MYON_UUID MYON_ModelComponentReference::ModelComponentId() const MYON_NOEXCEPT
{
  const MYON_ModelComponent* model_component = m_sp.get(); 
  return (nullptr != model_component) ? model_component->Id() : MYON_nil_uuid;
}

int MYON_ModelComponentReference::ModelComponentIndex() const MYON_NOEXCEPT
{
  const MYON_ModelComponent* model_component = m_sp.get(); 
  return (nullptr != model_component) ? model_component->Index() : MYON_UNSET_INT_INDEX;
}

const MYON_NameHash MYON_ModelComponentReference::ModelComponentNameHash() const MYON_NOEXCEPT
{
  const MYON_ModelComponent* model_component = m_sp.get(); 
  return (nullptr != model_component) ? model_component->NameHash() : MYON_NameHash::UnsetNameHash;
}

bool MYON_ModelComponentReference::IsEmpty() const MYON_NOEXCEPT
{
  return (nullptr == m_sp.get());
}

unsigned int MYON_ModelComponentReference::ReferenceCount() const MYON_NOEXCEPT
{
  return (unsigned int)(m_sp.use_count());
}

MYON_ModelComponentContentMark::MYON_ModelComponentContentMark(
  const MYON_ModelComponent& model_component
)
{
  Set(model_component);
}

MYON_ModelComponentContentMark::MYON_ModelComponentContentMark(
  const MYON_ModelComponent* model_component
)
{
  Set(model_component);
}

void MYON_ModelComponentContentMark::Set(
  const class MYON_ModelComponent& model_component
)
{
  m_component_id = model_component.Id();
  m_component_serial_number = model_component.RuntimeSerialNumber();
  m_component_content_version_number = model_component.ContentVersionNumber();
  m_component_type = model_component.ComponentType();
}

void MYON_ModelComponentContentMark::Set(
  const class MYON_ModelComponent* model_component
)
{
  if (nullptr == model_component)
    *this = MYON_ModelComponentContentMark::Unset;
  else
    Set(*model_component);
}

bool MYON_ModelComponentContentMark::EqualContent(
  const class MYON_ModelComponent& model_component
) const
{
  const MYON_ModelComponentContentMark rhs(model_component);
  return EqualContent(*this, rhs);
}

bool MYON_ModelComponentContentMark::EqualContent(
  const class MYON_ModelComponent* model_component
) const
{
  const MYON_ModelComponentContentMark rhs(model_component);
  return EqualContent(*this, rhs);
}
  
bool MYON_ModelComponentContentMark::EqualContent(
  const MYON_ModelComponentContentMark& lhs,
  const MYON_ModelComponentContentMark& rhs
)
{
  return
    lhs.m_component_serial_number == rhs.m_component_serial_number
    && lhs.m_component_content_version_number == rhs.m_component_content_version_number
    && lhs.m_component_id == rhs.m_component_id
    && lhs.m_component_type == rhs.m_component_type
    ;
}

bool operator==(
  const MYON_ModelComponentContentMark& lhs,
  const MYON_ModelComponentContentMark& rhs
  )
{
  return MYON_ModelComponentContentMark::EqualContent(lhs, rhs);
}

bool operator!=(
const MYON_ModelComponentContentMark& lhs,
const MYON_ModelComponentContentMark& rhs
)
{
  return MYON_ModelComponentContentMark::EqualContent(lhs, rhs) ? false : true;
}

MYON_UUID MYON_ModelComponentContentMark::ComponentId() const
{
  return m_component_id;
}

MYON__UINT64 MYON_ModelComponentContentMark::ComponentRuntimeSerialNumber() const
{
  return m_component_serial_number;
}

MYON__UINT64 MYON_ModelComponentContentMark::ComponentContentVersionNumber() const
{
  return m_component_content_version_number;
}

MYON_ModelComponent::Type MYON_ModelComponentContentMark::ComponentType() const
{
  return m_component_type;
}

bool MYON_ModelComponentContentMark::IsSet() const
{
  return (0 != m_component_serial_number);
}

bool MYON_ModelComponentContentMark::IsUnset() const
{
  return (0 == m_component_serial_number);
}

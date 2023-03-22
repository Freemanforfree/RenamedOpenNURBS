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

MYON_OBJECT_IMPLEMENT( MYON_Group, MYON_ModelComponent, "721D9F97-3645-44c4-8BE6-B2CF697D25CE" );

const MYON_Group * MYON_Group::FromModelComponentRef(
  const MYON_ModelComponentReference & model_component_reference, 
  const MYON_Group * none_return_value
  )
{
  const MYON_Group* group = MYON_Group::Cast(model_component_reference.ModelComponent());
  return (nullptr != group) ? group : none_return_value;
}

MYON_Group::MYON_Group() MYON_NOEXCEPT
  : MYON_ModelComponent(MYON_ModelComponent::Type::Group)
{}

MYON_Group::MYON_Group(const MYON_Group& src)
  : MYON_ModelComponent(MYON_ModelComponent::Type::Group, src)
{}

//////////////////////////////////////////////////////////////////////
//
// MYON_Object overrides

bool MYON_Group::IsValid( MYON_TextLog* text_log ) const
{
  return (IdIsNotNil() && NameIsSet() && Index() >= 0);
}

void MYON_Group::Dump( MYON_TextLog& text_log ) const
{
  MYON_ModelComponent::Dump(text_log);
}

bool MYON_Group::Internal_WriteV5(
       MYON_BinaryArchive& file // serialize definition to binary archive
     ) const
{
  bool rc = file.Write3dmChunkVersion(1,1);
  // version 1.0 fields
  if (rc) rc = file.Write3dmReferencedComponentIndex(MYON_ModelComponent::Type::Group,Index());
  if (rc) rc = file.WriteString(Name());
  // version 1.1 fields
  if (rc) rc = file.WriteUuid(Id());
  return rc;
}

bool MYON_Group::Internal_ReadV5(
       MYON_BinaryArchive& file // restore definition from binary archive
     )
{
  *this = MYON_Group::Unset;

  int major_version = 0;
  int minor_version = 0;
  bool rc = file.Read3dmChunkVersion(&major_version,&minor_version);
  if ( major_version == 1 ) 
  {
    int group_index = Index();
    if (rc) rc = file.ReadInt( &group_index );
    if (rc) SetIndex(group_index);
    MYON_wString group_name;
    if (rc) rc = file.ReadString( group_name );
    if (rc)
      SetName(group_name);
    if ( minor_version >= 1 )
    {
      MYON_UUID group_id = MYON_nil_uuid;
      if (rc) rc = file.ReadUuid( group_id );
      if (rc) SetId(group_id);
    }
    if (rc && IdIsNil() )
    {
      // modern times require unique ids.
      SetId();
    }
  }
  else
    rc = false;
  return rc;
}

bool MYON_Group::Write(
  MYON_BinaryArchive& archive // serialize definition to binary archive
  ) const
{
  return Internal_WriteV5(archive);
}

bool MYON_Group::Read(
  MYON_BinaryArchive& archive // restore definition from binary archive
  )
{
  return Internal_ReadV5(archive);
}


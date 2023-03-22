/* $NoKeywords: $ */
/*
//
// Copyright (c) 1993-2012 Robert McNeel & Associates. All rights reserved.
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
*/

#include "opennurbs.h"

#if !defined(MYON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// MYON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, MYON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error MYON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

///////////////////////////////////////////////////////////////
//
// This file is the user data grave yard.
// It contains code necessary to read and destroy obsolete user data
// that was saved in 3dm archives at some point.
// If this code were deleted, then the obsolete user data would persist
// as "goo" for a long time after it is no longer needed.
//

static bool MYON_Internal_ReadObsoleteUserDataAnonymouseChunk(MYON_BinaryArchive& archive)
{
  // If the obsolete user data's Write()/Read() function wrapped all the contents in
  // an anonymous chunk (which is the suggested best practice), then
  // this skip over everthing in the chunk and not generate any file read warnings or errors.
  int major_version = 0;
  int minor_version = 0;
  bool rc = archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_version,&minor_version);
  if ( !rc )
    return false;

  // Skip all obsolete information

  const bool bSupressPartiallyReadChunkWarning = true;
  if ( !archive.EndRead3dmChunk(bSupressPartiallyReadChunkWarning) )
    rc = false;

  return rc;
}

class /*NEVER EXPORT THIS CLASS DEFINITION*/ MYON_Internal_ObsoleteUserData : public MYON_UserData
{
  MYON_OBJECT_DECLARE(MYON_Internal_ObsoleteUserData);

public:
  MYON_Internal_ObsoleteUserData() = default;
  ~MYON_Internal_ObsoleteUserData() = default;
  MYON_Internal_ObsoleteUserData(const MYON_Internal_ObsoleteUserData& src) = default;
  MYON_Internal_ObsoleteUserData& operator=(const MYON_Internal_ObsoleteUserData& src) = default;

public:
  bool Archive() const override
  {
    // NEVER WRITE
    return false;
  }

  bool Read(MYON_BinaryArchive& binary_archive) override
  {
    return MYON_Internal_ReadObsoleteUserDataAnonymouseChunk(binary_archive);
  }

  bool DeleteAfterRead(
    const class MYON_BinaryArchive& archive,
    class MYON_Object* parent_object
  ) const override
  {
    return true;
  }

  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override
  {
    return true;
  }

  bool GetDescription(MYON_wString& description) override
  {
    description = L"OBSOLETE user data";
    return true;
  }
};

MYON_OBJECT_IMPLEMENT(MYON_Internal_ObsoleteUserData,MYON_UserData,"EDA27090-1DDD-4E4D-AB0C-75CCF0216565");



/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

class /*NEVER EXPORT THIS CLASS DEFINITION*/ MYON_OBSOLETE_IDefLayerSettingsUserData : public MYON_Internal_ObsoleteUserData
{
  // This class was used in V5 files - the information it saved never worked correctly.
  MYON_OBJECT_DECLARE(MYON_OBSOLETE_IDefLayerSettingsUserData);

public:
  MYON_OBSOLETE_IDefLayerSettingsUserData()
  {
    m_userdata_uuid = MYON_CLASS_ID(MYON_OBSOLETE_IDefLayerSettingsUserData);
    m_application_uuid = MYON_opennurbs5_id;
    m_userdata_copycount = 0; // never copy this user data
  }

  ~MYON_OBSOLETE_IDefLayerSettingsUserData() = default;

  MYON_OBSOLETE_IDefLayerSettingsUserData(const MYON_OBSOLETE_IDefLayerSettingsUserData& src)
    : MYON_Internal_ObsoleteUserData(src)
  {
    m_userdata_uuid = MYON_CLASS_ID(MYON_OBSOLETE_IDefLayerSettingsUserData);
    m_application_uuid = MYON_opennurbs5_id;
    m_userdata_copycount = 0; // never copy this user data
  }

  MYON_OBSOLETE_IDefLayerSettingsUserData& operator=(const MYON_OBSOLETE_IDefLayerSettingsUserData& src) = default;

public:
  bool GetDescription(MYON_wString& description) override
  {
    description = L"OBSOLETE MYON_OBSOLETE_IDefLayerSettingsUserData";
    return true;
  }
};

MYON_OBJECT_IMPLEMENT(MYON_OBSOLETE_IDefLayerSettingsUserData,MYON_UserData,"11EE2C1F-F90D-4C6A-A7CD-EC8532E1E32D");

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////


class /*NEVER EXPORT THIS CLASS DEFINITION*/ MYON_OBSOLETE_LayerSettingsUserData : public MYON_Internal_ObsoleteUserData
{
  // This class was used in V5 files - the information it saved never worked correctly.
  MYON_OBJECT_DECLARE(MYON_OBSOLETE_LayerSettingsUserData);

public:
  MYON_OBSOLETE_LayerSettingsUserData()
  {
    m_userdata_uuid = MYON_CLASS_ID(MYON_OBSOLETE_LayerSettingsUserData);
    m_application_uuid = MYON_opennurbs5_id;
    m_userdata_copycount = 0; // never copy this user data
  }
  ~MYON_OBSOLETE_LayerSettingsUserData() = default;
  MYON_OBSOLETE_LayerSettingsUserData(const MYON_OBSOLETE_LayerSettingsUserData& src)
    : MYON_Internal_ObsoleteUserData(src)
  {
    m_userdata_uuid = MYON_CLASS_ID(MYON_OBSOLETE_LayerSettingsUserData);
    m_application_uuid = MYON_opennurbs5_id;
    m_userdata_copycount = 0; // never copy this user data
  }
  MYON_OBSOLETE_LayerSettingsUserData& operator=(const MYON_OBSOLETE_LayerSettingsUserData&) = default;

public:
  bool GetDescription(MYON_wString& description) override
  {
    description = L"OBSOLETE MYON_OBSOLETE_LayerSettingsUserData";
    return true;
  }
};

MYON_OBJECT_IMPLEMENT(MYON_OBSOLETE_LayerSettingsUserData,MYON_UserData,"BFB63C09-4BC7-4727-89BB-7CC754118200");

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////


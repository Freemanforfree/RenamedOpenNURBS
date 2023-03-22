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

MYON_VIRTUAL_OBJECT_IMPLEMENT(MYON_UserData,MYON_Object,"850324A7-050E-11d4-BFFA-0010830122F0");

MYON_UserData::MYON_UserData()
            : m_userdata_uuid(MYON_nil_uuid), 
              m_application_uuid(MYON_nil_uuid),
              m_userdata_copycount(0),
              m_userdata_xform(MYON_Xform::IdentityTransformation),
              m_userdata_owner(0),
              m_userdata_next(0)
{}

MYON_UserData::MYON_UserData(const MYON_UserData& src) 
            : MYON_Object(src),
              m_userdata_uuid(src.m_userdata_uuid), 
              m_application_uuid(src.m_application_uuid),
              m_userdata_copycount(src.m_userdata_copycount),
              m_userdata_xform(src.m_userdata_xform),
              m_userdata_owner(0), // do not copy owner
              m_userdata_next(0)   // do not copy next
{
  if ( m_userdata_copycount) 
  {
    m_userdata_copycount++;
    if ( !m_userdata_copycount )
      m_userdata_copycount = 1;
  }
}

//virtual
bool MYON_UserData::Archive() const
{
  return false;
}

//virtual
bool MYON_UserData::WriteToArchive(
  const class MYON_BinaryArchive& archive,
  const class MYON_Object* parent_object
  ) const
{
  return Archive() ? true : false;
}

// virtual
bool MYON_UserData::DeleteAfterWrite(
  const MYON_BinaryArchive& archive,
  const MYON_Object* parent_object
  ) const
{
  return false;
}

// virtual
bool MYON_UserData::DeleteAfterRead(
  const MYON_BinaryArchive& archive,
  MYON_Object* parent_object
  ) const
{
  return false;
}

//virtual
bool MYON_UserData::Transform(const MYON_Xform& x )
{
  m_userdata_xform = x*m_userdata_xform;
  return true;
}

MYON_UserData& MYON_UserData::operator=(const MYON_UserData& src)
{
  // 16 January 2004 Dale Lear
  //    Do not copy the m_userdata_uuid, m_application_uuid,
  //    m_userdata_owner, or m_userdata_next values.
  //    The m_userdata_uuid and m_application_uuid are
  //    set when the class is constructed and should not be 
  //    changed.  The m_userdata_owner and m_userdata_next
  //    values are set when the user data is attached
  //    to a parent object.
  if ( this != &src ) 
  {
    MYON_Object::operator=(src);
    m_userdata_copycount = src.m_userdata_copycount;
    m_userdata_xform = src.m_userdata_xform;
    if ( 0 != m_userdata_copycount )
    {
      m_userdata_copycount++;
      if ( !m_userdata_copycount )
        m_userdata_copycount = 1;
    }
  }

  return *this;
}

MYON_UserData::~MYON_UserData()
{
  if ( 0 != m_userdata_owner ) 
  {
    // remove this piece of user data from owner->m_userdata_list
    m_userdata_owner->DetachUserData(this);
    m_userdata_owner = 0;
  }
}

void MYON_UserData::Dump( MYON_TextLog& text_log ) const
{
  text_log.Print("User Data:\n");
  text_log.PushIndent();

  // print class name and class uuid
  MYON_Object::Dump(text_log);

  // developer's user data description
  MYON_wString description;
  const_cast<MYON_UserData*>(this)->GetDescription(description);
  if ( description.IsEmpty() )
    description = L"none";
  const wchar_t* ws = static_cast< const wchar_t* >(description);
  text_log.Print("user data description: %ls\n",ws);
  text_log.Print("user data uuid: ");
  text_log.Print(m_userdata_uuid);
  text_log.Print("\n");
  text_log.Print("user data copy count: %d\n",this->m_userdata_copycount);

  // archive setting
  text_log.Print("user data saved in 3dm archive: %s\n",Archive() ? "yes" : "no");

  text_log.PopIndent();
}

unsigned int MYON_UserData::SizeOf() const
{
  unsigned int sz = MYON_Object::SizeOf();
  sz += (sizeof(*this) - sizeof(MYON_Object));
  return sz;
}

bool MYON_UserData::IsValid( MYON_TextLog* text_log ) const
{
  if ( 0 == MYON_UuidCompare( &m_userdata_uuid, &MYON_nil_uuid ) )
  {
    if ( 0 != text_log )
    {
      text_log->Print("invalid userdata - m_userdata_uuid = nil\n");
    }
    return false;
  }

  if ( 0 == MYON_UuidCompare( m_userdata_uuid, MYON_UserData::ClassId()->Uuid() ) )
  {
    if ( 0 != text_log )
    {
      text_log->Print("invalid userdata - m_userdata_uuid in use. Use guidgen to get a unique id.\n");
    }
    return false;
  }

  if ( Archive() && 0 == MYON_UuidCompare( ClassId()->Uuid(), MYON_UserData::ClassId()->Uuid() ) )
  {
    // 8 January 2004 Dale Lear:
    //  I added this test to help developers remember to use
    //  the MYON_DECLARE_OBJECT/MYON_IMPLEMENT_OBJECT macros when
    //  they create user data that gets archived.
    if ( 0 != text_log )
    {
      text_log->Print("invalid userdata - classes derived from MYON_UserData that get saved in 3dm archives must have a class id and name defined by MYON_OBJECT_DECLARE/MYON_OBJECT_IMPLEMENT.\n");
    }
    return false;
  }

  return true;
}

MYON_Object* MYON_UserData::Owner() const
{
  return m_userdata_owner;
}

MYON_UserData* MYON_UserData::Next() const
{
  return m_userdata_next;
}

MYON_UUID MYON_UserData::UserDataClassUuid() const
{
  const MYON_ClassId* this_rtti = ClassId();

  if (this_rtti == &MYON_CLASS_RTTI(MYON_UnknownUserData))
    return ((MYON_UnknownUserData*)this)->m_unknownclass_uuid;

  if (this_rtti == &MYON_CLASS_RTTI(MYON_ObsoleteUserData))
    return ((MYON_ObsoleteUserData*)this)->m_archive_class_uuid;

  return this_rtti->Uuid();
}

bool MYON_UserData::IsUnknownUserData() const
{
  return (ClassId() == &MYON_CLASS_RTTI(MYON_UnknownUserData))?true:false;
}

bool MYON_UserData::GetDescription( MYON_wString& description )
{
  return true;
}


/////////////////////////////////////////////////////////////////
//
// 
//

MYON_OBJECT_IMPLEMENT(MYON_ObsoleteUserData, MYON_UserData, "7C9305E5-947A-4E46-B102-8016849FABB1");

MYON_ObsoleteUserData::MYON_ObsoleteUserData()
  : m_archive_class_uuid(MYON_nil_uuid)
{}

MYON_ObsoleteUserData::~MYON_ObsoleteUserData()
{}

MYON_ObsoleteUserData::MYON_ObsoleteUserData(const MYON_ObsoleteUserData& src)
  : MYON_UserData(src)
  , m_archive_class_uuid(MYON_nil_uuid)
{}

MYON_ObsoleteUserData& MYON_ObsoleteUserData::operator=(const MYON_ObsoleteUserData& src)
{
  if (this != &src)
  {
    MYON_UserData::operator=(src);
    m_archive_class_uuid = src.m_archive_class_uuid;
  }
  return *this;
}


//
//
//
/////////////////////////////////////////////////////////////////

MYON_OBJECT_IMPLEMENT(MYON_UnknownUserData,MYON_UserData,"850324A8-050E-11d4-BFFA-0010830122F0");

MYON_UnknownUserData::MYON_UnknownUserData() 
: m_unknownclass_uuid(MYON_nil_uuid)
, m_sizeof_buffer(0)
, m_buffer(0)
, m_3dm_version(0)
, m_3dm_opennurbs_version_number(0)
{}

MYON_UnknownUserData::MYON_UnknownUserData(const MYON_UnknownUserData& src) 
: MYON_UserData(src)
, m_unknownclass_uuid(MYON_nil_uuid)
, m_sizeof_buffer(0)
, m_buffer(0)
, m_3dm_version(0)
, m_3dm_opennurbs_version_number(0)
{
  if ( m_userdata_copycount > 0 && src.m_sizeof_buffer > 0 && src.m_buffer ) 
  {
    // For most kinds of user data except MYON_UnknownUserData,
    // m_userdata_uuid is set by the constructor and should not
    // be copied from src (which may be a derived class).  However,
    // for MYON_UnknownUserData, the value of m_userdata_uuid is 
    // varies because it is set by the missing userdata class. 
    // So it has to be copied here.
    m_userdata_uuid = src.m_userdata_uuid;

    m_unknownclass_uuid = src.m_unknownclass_uuid;
    m_sizeof_buffer = src.m_sizeof_buffer;
    m_buffer = onmemdup( src.m_buffer, src.m_sizeof_buffer);
    m_3dm_version = src.m_3dm_version;
    m_3dm_opennurbs_version_number = src.m_3dm_opennurbs_version_number;
  }
}

MYON_UnknownUserData& MYON_UnknownUserData::operator=(const MYON_UnknownUserData& src)
{
  if ( this != &src ) 
  {
    m_sizeof_buffer = 0;
    if ( 0 != m_buffer )
    {
      onfree(m_buffer);
      m_buffer = 0;
    }

    // MYON_UserData::operator= handles setting m_userdata_copycount and
    // m_userdata_xform.
    MYON_UserData::operator=(src);

    // For most kinds of user data except MYON_UnknownUserData,
    // m_userdata_uuid and m_application_uuid are set by the
    // constructor and should not be altered by an operator=.  
    // However, for MYON_UnknownUserData, the value of m_userdata_uuid 
    // and m_application_uuid vary because they are set by the
    // missing userdata class.  So they have to be copied here.
    m_userdata_uuid = src.m_userdata_uuid;
    m_application_uuid = src.m_application_uuid; // fix added 26 January 2010

    if ( m_userdata_copycount > 0 && src.m_sizeof_buffer > 0 && src.m_buffer ) 
    {
      m_unknownclass_uuid = src.m_unknownclass_uuid;
      m_sizeof_buffer = src.m_sizeof_buffer;
      m_buffer = onmemdup( src.m_buffer, src.m_sizeof_buffer);
      m_3dm_version = src.m_3dm_version;
      m_3dm_opennurbs_version_number = src.m_3dm_opennurbs_version_number;
    }
    else 
    {
      // The unknown user data is not supposed to copy
      m_userdata_uuid = MYON_nil_uuid;
      m_unknownclass_uuid = MYON_nil_uuid;
      m_sizeof_buffer = 0;
      m_buffer = 0;
      m_3dm_version = 0;
      m_3dm_opennurbs_version_number = 0;
    }
  }
  return *this;
}

MYON_UnknownUserData::~MYON_UnknownUserData()
{
  if ( m_buffer )
    onfree(m_buffer);
}

unsigned int MYON_UnknownUserData::SizeOf() const
{
  return MYON_UserData::SizeOf() 
    + (sizeof(MYON_UnknownUserData)-sizeof(MYON_UserData))
    + m_sizeof_buffer;
}

bool MYON_UnknownUserData::GetDescription( MYON_wString& s )
{
  s = "Unknown user data. (Definition of class was not available when object was read.)";
  return true;
}

bool MYON_UnknownUserData::IsValid( MYON_TextLog* text_log ) const
{
  bool rc = MYON_UserData::IsValid(text_log);
  
  // valid unknown user data must have something in it
  if (rc) 
    rc = (m_sizeof_buffer>0);
  
  if (rc) 
    rc = (m_buffer != nullptr);
  
  if (rc)
  {
    // the unknown class uuid cannot be nil
    if (MYON_nil_uuid == m_unknownclass_uuid)
      rc = false;
  }
  
  if (rc) 
  {
    // the unknown class uuid cannot be the MYON_UnknownUserData class uuid
    const MYON_UUID MYON_UnknownUserData_classuuid = MYON_CLASS_ID(MYON_UnknownUserData);
    if ( m_unknownclass_uuid == MYON_UnknownUserData_classuuid )
      rc = false;
  }

  return rc;
}

void MYON_UnknownUserData::Dump( MYON_TextLog& dump ) const
{
  MYON_UserData::Dump(dump);
  dump.PushIndent();
  dump.Print( "unknown class uuid: ");
  dump.Print( m_unknownclass_uuid );
  dump.Print( "\n");
  dump.Print( "Data size in 3dm archive: %d bytes\n",m_sizeof_buffer);
  dump.PopIndent();
}

bool MYON_UnknownUserData::Archive() const
{
  // 22 January 2004 Dale Lear
  //   Since unknown userdata is only created when
  //   an archive is read, it has to be saved.
  return true;
}

bool MYON_UnknownUserData::Write( MYON_BinaryArchive& file ) const
{
  return file.WriteByte(m_sizeof_buffer,m_buffer);
}

bool MYON_UnknownUserData::Read( MYON_BinaryArchive& file )
{
  m_buffer = onrealloc( m_buffer, m_sizeof_buffer );
  m_3dm_version = file.Archive3dmVersion();
  return file.ReadByte(m_sizeof_buffer,m_buffer);
}


class MYON_UnknownUserDataArchive : public MYON_BinaryArchive
{
  // This class is used to define an MYON_BinaryArchive that can be used
  // in MYON_UnknownUserData::Convert() to initialize a MYON_UserData class
  // from a memory buffer.
public:
  MYON_UnknownUserDataArchive( const MYON_UnknownUserData& );
  ~MYON_UnknownUserDataArchive();

protected:
  // MYON_BinaryArchive overrides
  MYON__UINT64 Internal_CurrentPositionOverride() const override;
  bool Internal_SeekFromCurrentPositionOverride(int byte_offset) override;
  bool Internal_SeekToStartOverride() override;

public:
  // MYON_BinaryArchive overrides
  bool AtEnd() const override;

protected:
  // MYON_BinaryArchive overrides
  size_t Internal_ReadOverride( size_t, void* ) override; // return actual number of bytes read (like fread())
  size_t Internal_WriteOverride( size_t, const void* ) override;
  bool Flush() override;

private:
  MYON_UnknownUserDataArchive();

  size_t m_sizeof_buffer = 0;
  const unsigned char* m_buffer = nullptr;
  size_t m_buffer_position = 0;
};

MYON_UnknownUserDataArchive::MYON_UnknownUserDataArchive(const MYON_UnknownUserData& ud) : MYON_BinaryArchive(MYON::archive_mode::read3dm)
{
  SetArchive3dmVersion(ud.m_3dm_version);
  m_sizeof_buffer = ud.m_sizeof_buffer;
  m_buffer = (const unsigned char*)ud.m_buffer;
  m_buffer_position = 0;
}

MYON_UnknownUserDataArchive::~MYON_UnknownUserDataArchive()
{
}

MYON__UINT64 MYON_UnknownUserDataArchive::Internal_CurrentPositionOverride() const
{
  return (MYON__UINT64)m_buffer_position;
}

bool MYON_UnknownUserDataArchive::Internal_SeekFromCurrentPositionOverride( int offset )
{
  bool rc = false;
  if ( offset >= 0 || m_buffer_position >= ((size_t)(-offset)) )
  {
    size_t newpos = m_buffer_position + offset;
    if ( newpos < m_sizeof_buffer ) 
    {
      m_buffer_position = newpos;
      rc = true;
    }
  }
  return rc;
}

bool MYON_UnknownUserDataArchive::Internal_SeekToStartOverride()
{
  m_buffer_position = 0;
  return true;
}

bool MYON_UnknownUserDataArchive::AtEnd() const
{
  return (m_buffer_position >= m_sizeof_buffer) ? true : false;
}

size_t MYON_UnknownUserDataArchive::Internal_ReadOverride( size_t count, void* buffer )
{
  size_t maxcount = 0;

  if ( m_sizeof_buffer > m_buffer_position ) 
  {
    maxcount = m_sizeof_buffer - m_buffer_position;
  }

  if ( count > maxcount )
  {
    count = maxcount;
  }

  if ( count > 0 ) 
  {
    memcpy( buffer, m_buffer+m_buffer_position, count );
    m_buffer_position += count;
  }

  return count;
}

size_t MYON_UnknownUserDataArchive::Internal_WriteOverride( size_t, const void* )
{
  // MYON_UnknownUserDataArchive does not support Write() and Flush()
  return 0;
}

bool MYON_UnknownUserDataArchive::Flush()
{
  // MYON_UnknownUserDataArchive does not support Write() and Flush()
  return false;
}

MYON_UserData* MYON_UnknownUserData::Convert() const
{
  MYON_UserData* ud = nullptr;
  if ( IsValid() ) {
    const MYON_ClassId* pID = MYON_ClassId::ClassId( m_unknownclass_uuid );
    // if pID is nullptr, it means the definiton of the unknown user data
    // is still not available
    if ( pID ) {
      // The class definition has been dynamically loaded since the
      // user data was read from an archive.  Use the class's Read()
      // to convert the buffer into a valid class
      MYON_Object* pObject = pID->Create();
      if ( pObject ) {
        ud = MYON_UserData::Cast(pObject);
        if ( !ud )
          delete pObject;
        else 
        {
          // use class's Read() function to initialize class members from buffer
          MYON_UnknownUserDataArchive file(*this);
          // copy values that would be set by reading the base class
          ud->m_userdata_copycount = m_userdata_copycount;
          ud->m_userdata_xform = m_userdata_xform;
          ud->Read(file);
        }
      }
    }
  }
  return ud;
}


unsigned int MYON_UserDataHolder::MoveUserDataFrom( const MYON_Object& source_object )
{
  PurgeUserData();
  unsigned int item_count = MoveUserData(const_cast< MYON_Object& >(source_object), MYON_nil_uuid, MYON_Object::UserDataConflictResolution::source_object,true);
  return item_count;
}

unsigned int MYON_UserDataHolder::CopyUserDataFrom( const MYON_Object& source_object, MYON_UUID user_data_item_id )
{
  PurgeUserData();
  unsigned int item_count = CopyUserData(source_object, user_data_item_id, MYON_Object::UserDataConflictResolution::source_object);
  return item_count;
}


unsigned int MYON_UserDataHolder::MoveUserDataTo(
  const MYON_Object& destination_object,
  bool bAppend
  )
{
  if ( !bAppend )
    const_cast< MYON_Object& >(destination_object).PurgeUserData();

  const MYON_Object::UserDataConflictResolution userdata_conflict_resolution
    = bAppend
    ? MYON_Object::UserDataConflictResolution::destination_object
    : MYON_Object::UserDataConflictResolution::source_object;
  return MoveUserDataTo(destination_object,MYON_nil_uuid,userdata_conflict_resolution);
}

unsigned int MYON_UserDataHolder::MoveUserDataTo(
  const MYON_Object& destination_object,
  MYON_UUID user_data_item_id,
  MYON_Object::UserDataConflictResolution userdata_conflict_resolution
  )
{
  unsigned int moved_count = const_cast< MYON_Object& >(destination_object).MoveUserData(*this, user_data_item_id, userdata_conflict_resolution,true);
  return moved_count;

}


bool MYON_UserDataHolder::IsValid( MYON_TextLog* text_log ) const
{
  return true;
}


/////////////////////////////////////////////////////////////////
//
// Built in tool for attaching arbitrary string userdata
// to any opennurbs object.  The savvy user will probably
// use XML in the string.


MYON_UserString::MYON_UserString()
{
}

MYON_UserString::~MYON_UserString()
{
}


bool MYON_UserString::Write(MYON_BinaryArchive& archive) const
{
  bool rc = archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,0);
  if (!rc)
    return false;

  for(;;)
  {
    rc = archive.WriteString(m_key);
    if (!rc) break;
    rc = archive.WriteString(m_string_value);
    if (!rc) break;

    break;
  }

  if ( !archive.EndWrite3dmChunk() )
    rc = false;

  return rc;
}

bool MYON_UserString::Read(MYON_BinaryArchive& archive)
{
  m_key.Empty();
  m_string_value.Empty();

  int major_version = 0;
  int minor_version = 0;
  bool rc = archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_version,&minor_version);
  if (!rc)
    return false;

  for(;;)
  {
    rc = ( 1 == major_version );
    if (!rc) break;
    rc = archive.ReadString(m_key);
    if (!rc) break;
    rc = archive.ReadString(m_string_value);
    if (!rc) break;

    break;
  }

  if ( !archive.EndRead3dmChunk() )
    rc = false;

  return rc;
}

void MYON_UserString::Dump(MYON_TextLog& text_log) const
{
  const wchar_t* ws = static_cast< const wchar_t* >(m_key);
  if ( !ws )
    ws = L"";
  text_log.Print("Key: %ls\n", ws);
  
  ws = static_cast< const wchar_t* >(m_string_value);
  if ( !ws )
    ws = L"";
  text_log.Print("Value: %ls\n",ws);
}

MYON_OBJECT_IMPLEMENT(MYON_UserStringList,MYON_UserData,"CE28DE29-F4C5-4faa-A50A-C3A6849B6329");

MYON_UserStringList::MYON_UserStringList()
{
  m_userdata_uuid = MYON_CLASS_ID(MYON_UserStringList);
  m_application_uuid = MYON_opennurbs4_id; // opennurbs.dll reads/writes this userdata
                                         // The id must be the version 4 id because
                                         // V5 SaveAs V4 needs to work.
  m_userdata_copycount = 1;
}

MYON_UserStringList::~MYON_UserStringList()
{
}

bool MYON_UserStringList::GetDescription( MYON_wString& description )
{
  description.Format(L"User text (%d entries)",m_e.Count());
  return true;
}

bool MYON_UserStringList::Archive() const
{
  return true;
}

unsigned int MYON_UserStringList::SizeOf() const
{
  unsigned int sz = MYON_UserData::SizeOf();
  sz += sizeof(*this) - sizeof(MYON_UserData);
  sz += m_e.SizeOfArray();
  int i = m_e.Count();
  while (i--)
    sz += m_e[i].m_string_value.Length()*sizeof(wchar_t);
  return sz;
}


MYON__UINT32 MYON_UserStringList::DataCRC(MYON__UINT32 current_remainder) const
{
  int count = m_e.Count();
  for ( int i = 0; i < count; i++ )
  {
    current_remainder = m_e[i].m_key.DataCRC(current_remainder);
    current_remainder = m_e[i].m_string_value.DataCRC(current_remainder);
  }
  return current_remainder;
}


void MYON_UserStringList::Dump( MYON_TextLog& text_log ) const
{
  int i, count = m_e.Count();
  text_log.Print("%d entries\n",count);
  text_log.PushIndent();
  for ( i = 0; i < count; i++ )
  {
    m_e[i].Dump(text_log);
  }
  text_log.PopIndent();
}

bool MYON_UserStringList::Write(MYON_BinaryArchive& archive) const
{
  bool rc = archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,0);
  if ( !rc )
    return false;

  for(;;)
  {
    int count = m_e.Count();
    rc = archive.WriteInt(count);
    if(!rc) break;

    for ( int i = 0; i < count && rc; i++ )
    {
      rc = m_e[i].Write(archive);
    }
    if (!rc) break;

    break;
  }

  if ( !archive.EndWrite3dmChunk() )
    rc = false;

  return rc;
}

bool MYON_UserStringList::Read(MYON_BinaryArchive& archive)
{
  int major_version = 0;
  int minor_version = 0;
  bool rc = archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_version,&minor_version);
  if ( !rc )
    return false;

  for(;;)
  {
    rc = (1 == major_version);
    if (!rc) break;

    int count = 0;
    rc = archive.ReadInt(&count);
    if(!rc) break;

    for ( int i = 0; i < count; i++ )
    {
      rc = m_e.AppendNew().Read(archive);
      if ( !rc) 
      {
        m_e.Remove();
        break;
      }
    }
    if (!rc) break;

    break;
  }

  if ( !archive.EndRead3dmChunk() )
    rc = false;

  return rc;
}


bool MYON_UserStringList::SetUserString( const wchar_t* key, const wchar_t* string_value )
{
  if ( !key || !key[0] )
    return false;

  int i, count = m_e.Count();
  for (i = 0; i < count; i++ )
  {
    if ( !m_e[i].m_key.CompareOrdinal(key,true) )
    {
      if ( string_value && string_value[0] )
      {
        m_e[i].m_string_value = string_value;
      }
      else
      {
        m_e.Remove(i);
      }
      m_userdata_copycount++;
      return true;
    }
  }

  if ( string_value && string_value[0] )
  {
    MYON_UserString& e = m_e.AppendNew();
    e.m_key = key;
    e.m_string_value = string_value;
    m_userdata_copycount++;
    return true;
  }

  return false;
}

bool MYON_UserStringList::GetUserString( const wchar_t* key, MYON_wString& string_value ) const
{
  if ( key && key[0] )
  {
    int i, count = m_e.Count();
    for (i = 0; i < count; i++ )
    {
      if ( !m_e[i].m_key.CompareOrdinal(key,true) )
      {
        string_value = m_e[i].m_string_value;
        return true;
      }
    }
  }

  string_value = MYON_wString::EmptyString;
  return false;
}

static int cmp_hash_2dex_ij(const void* a, const void* b)
{
  const int* ai = (const int*)a;
  const int* bi = (const int*)b;
  // 26 January 2012 Dale Lear
  //    Part of the fix for http://dev.mcneel.com/bugtrack/?q=97693
  //
  //    The "i" values are actually 32 bit hashes of a string
  //    and are often large.  The sign of (ai[0] - bi[0]) cannot
  //    be used to compare ai[0] and bi[0] because integer
  //    overflow occures with values that are large.
  //
  ////// NO!
  //////if ( 0 == (rc = ai[0] - bi[0]) )
  //////  rc = ai[1] - bi[1];
  //////return rc;

  if ( ai[0] < bi[0] )
    return -1;
  if ( ai[0] > bi[0] )
    return 1;
  if ( ai[1] < bi[1] )
    return -1;
  if ( ai[1] > bi[1] )
    return 1;
  return 0;
}

int MYON_UserStringList::SetUserStrings( int count, const MYON_UserString* us, bool bReplace )
{
  int added_count = 0;
  int i;

  if ( count <= 0 || 0 == us )
    return 0;

  if ( 1 == count )
  {
    // skip the hash table hoo haa
    if (  us[0].m_key.IsEmpty() )
      return 0;
    for ( i = 0; i < m_e.Count(); i++ )
    {
      if ( m_e[i].m_key.CompareOrdinal(us[0].m_key, true ) )
        continue;
      if ( bReplace )
      {
        if ( us[0].m_string_value.IsEmpty() )
          m_e.Remove(i);
        else
          m_e[i] = us[0];
        added_count++;
      }
      break;
    }
    return added_count;
  }

  size_t k0, k1;
  int count0 = m_e.Count();
  size_t count0_plus_count = (size_t)(count0 + count);
  MYON_2dex* hash = (MYON_2dex*)onmalloc( (count0_plus_count + count)*sizeof(hash[0]) );
  MYON_2dex* hash1 =  hash + (count0_plus_count);
  const MYON_2dex* h;
  int deleted_count = 0;

  for ( i = 0; i < count0; i++ )
  {
    hash[i].i = (int)m_e[i].m_key.DataCRCLower(0);
    hash[i].j = i;
  }

  for ( i = 0; i < count; i++ )
  {
    hash1[i].i = (int)us[i].m_key.DataCRCLower(0);
    hash1[i].j = i;
    hash[i+count0].i = hash1[i].i;
    hash[i+count0].j = hash1[i].j+count0;
  }
  MYON_qsort(hash,count0_plus_count,sizeof(hash[0]),cmp_hash_2dex_ij);

  m_e.Reserve(count0+count);
  for ( i = 0; i < count; i++)
  {
    if ( us[i].m_key.IsEmpty() )
      continue;

    // Set k0, k1 so that hash[k0]....,hash[k1-1] are 
    // the hash[] entries keys with the same hash code
    // as us[i].m_key.
    h = MYON_BinarySearch2dexArray(hash1[i].i,hash,count0_plus_count);
    if ( 0 == h )
    {
      MYON_ERROR("There is a bug in this function.");
      continue;
    }
    k0 = h-hash;
    while ( k0 > 0 && h[-1].i == h[0].i )
    {
      // set h = first element in hash[] with this hash code.
      k0--;
      h--;
    }
    for (k1 = k0+1; k1 < count0_plus_count; k1++ )
    {
      if ( hash[k1].i != hash[k0].i )
        break;
      if ( hash[k1].j > i+count0 )
        break;
    }

    if ( hash[k0].j >= count0 )
    {
      // There are no entries in m_e[] with key matching hash,
      // so us[i].m_key is not present in m_e.
      if ( !us[i].m_string_value.IsEmpty() )
      {
        hash[k0].j = count0++;
        m_e.Append(us[i]);
        added_count++;
      }
      continue;
    }

    for (/* empty init*/; k0 < k1; k0++ )
    {
      if ( hash[k0].j < count0 )
      {
        if ( m_e[hash[k0].j].m_key.CompareOrdinal(us[i].m_key,true) )
          continue; // different keys with same hash
        if ( bReplace )
        {
          m_e[hash[k0].j] = us[i];
          added_count++;
          if ( us[i].m_string_value.IsEmpty() )
            deleted_count++;
        }
        break;
      }
    }

    if ( k0 >= k1 )
    {
      // hash is unique up to this point, so us[i].m_key is unique, 
      // so we add it if it is valid.
      if ( !us[i].m_string_value.IsEmpty() )
      {
        hash[k0].j = count0++;
        m_e.Append(us[i]);
        added_count++;
      }
    }
  }

  onfree(hash);

  // remove deleted items.
  i = m_e.Count();
  while ( i-- > 0 && deleted_count > 0 )
  {
    if ( m_e[i].m_string_value.IsEmpty() )
    {
      m_e.Remove(i);
      deleted_count--;
    }
  }

  return added_count;
}



MYON_UserStringList* MYON_UserStringList::FromObject(
    const MYON_Object* p
    )
{
  return p 
         ? MYON_UserStringList::Cast(p->GetUserData(MYON_CLASS_ID(MYON_UserStringList)))
         : 0;
}

bool MYON_Object::SetUserString( const wchar_t* key, const wchar_t* string_value )
{
  MYON_UserStringList* us = MYON_UserStringList::FromObject(this);

  bool b = false;
  if ( nullptr == us )
  {
    us = new MYON_UserStringList();
    if ( !AttachUserData(us) )
    {
      delete us;
      us = nullptr;
    }
    else
    {
      b = true;
    }
  }

  if ( us )
  {
    if ( us->SetUserString(key,string_value) )
    {
      if ( b && 2 == us->m_userdata_copycount )
      {
        // user data is brand new - roll back the 
        // m_userdata_copycount++ that happens in 
        // SetUserString().
        us->m_userdata_copycount = 1;
      }
      b = true;
    }
    else if ( b )
    {
      // user data was new-ed up and has nothing in it
      // because the input was bogus.
      delete us;
      us = nullptr;
      b = false;
    }
  }
  return b;
}


int MYON_Object::SetUserStrings( int count, const MYON_UserString* user_strings, bool bReplace )
{
  if ( 0 == count || 0 == user_strings )
    return 0;

  int add_count = 0;
  int del_count = 0;
  for ( int i = 0; i < count; i++ )
  {
    if ( user_strings[i].m_key.IsEmpty() )
      continue;
    if ( user_strings[i].m_string_value.IsEmpty() )
      del_count++;
    else
      add_count++;
  }
  if ( 0 == add_count && 0 == del_count )
    return 0;

  MYON_UserStringList* us = MYON_UserStringList::FromObject(this);
  if ( !us && add_count > 0)
  {
    us = new MYON_UserStringList();
    if ( !AttachUserData(us) )
    {
      delete us;
      us = 0;
    }
  }

  return us ? us->SetUserStrings(count,user_strings,bReplace ) : 0;
}


bool MYON_Object::GetUserString( const wchar_t* key, MYON_wString& string_value ) const
{
  const MYON_UserStringList* us = MYON_UserStringList::FromObject(this);
  if (nullptr != us)
    return us->GetUserString(key,string_value);

  // because key and string_value might be the same, don't empty string_value until it is no longer needed.
  string_value = MYON_wString::EmptyString;
  return false;
}

int MYON_Object::UserStringCount() const
{
  const MYON_UserStringList* us = MYON_UserStringList::FromObject(this);
  return ( 0 != us )
         ? us->m_e.Count()
         : 0;
}


int MYON_Object::GetUserStrings( 
  MYON_ClassArray<MYON_UserString>& user_strings 
  ) const
{
  const int count0 = user_strings.Count();
  const MYON_UserStringList* us = MYON_UserStringList::FromObject(this);
  if ( us )
    user_strings.Append(us->m_e.Count(),us->m_e.Array());

  return user_strings.Count() - count0;
}

int MYON_Object::GetUserStringKeys( 
  MYON_ClassArray<MYON_wString>& user_string_keys 
  ) const
{
  const int count0 = user_string_keys.Count();
  const MYON_UserStringList* us = MYON_UserStringList::FromObject(this);
  if ( us )
  {
    user_string_keys.Reserve( count0 + us->m_e.Count() );
    for (int i = 0; i < us->m_e.Count(); i++ )
    {
      user_string_keys.Append(us->m_e[i].m_key);
    }
  }

  return user_string_keys.Count() - count0;
}

MYON_OBJECT_IMPLEMENT(MYON_DocumentUserStringList,MYON_Object,"06F3218E-F5EC-4f6c-B74C-14583F0ED7BC");

MYON_DocumentUserStringList::MYON_DocumentUserStringList()
{
}

MYON_DocumentUserStringList::~MYON_DocumentUserStringList()
{
}

bool MYON_DocumentUserStringList::IsValid( MYON_TextLog* text_log ) const
{
  return true;
}

void MYON_DocumentUserStringList::Dump( MYON_TextLog& ) const
{
}

MYON__UINT32 MYON_DocumentUserStringList::DataCRC(MYON__UINT32 current_remainder) const
{
  const MYON_UserStringList* us = MYON_UserStringList::FromObject(this);
  if ( us )
    current_remainder = us->DataCRC(current_remainder);
  return current_remainder;
}

bool MYON_DocumentUserStringList::Write(MYON_BinaryArchive& binary_archive) const
{
  //  The key/value pairs are saved as MYON_UserStringList user data.

  // A single char with value 1 is written to permit adding additional
  // information later.  In that case change the 1 to a 2, uncomment the
  // begin block code, and the IO will still work.
  //
  unsigned char c = 1;
  bool rc = binary_archive.WriteChar(c);

  ////rc = binary_archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,0);
  ////if (!rc)
  ////  return false;
  ////for(;;)
  ////{
  ////  rc = binary_archive.Write...();
  ////  if (!rc) break;

  ////  break;
  ////}
  ////if ( !binary_archive.EndWrite3dmChunk() )
  ////  rc = false;

  return rc;
}

bool MYON_DocumentUserStringList::Read(MYON_BinaryArchive& binary_archive)
{
  //  The key/value pairs are saved as MYON_UserStringList user data.

  unsigned char c = 0;
  bool rc =  binary_archive.ReadChar(&c);
  if ( !rc || c < 1 || c > 2 )
    return false;

  if ( 2 == c )
  {
    // The code in this if(2==c) will not be used unless
    // the 1 in Write is changed to a 2.  This code is here
    // so old versions of Rhino will be able to skip over
    // any new information that is added at a later date.
    int major_version = 0;
    int minor_version = 0;
    rc = binary_archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_version,&minor_version);
    if (!rc)
      return false;
    for(;;)
    {
      rc = (1 == major_version);
      if (!rc) break;

      ////rc = binary_archive.Read(...);
      ////if (!rc) break;

      break;
    }
    if (!binary_archive.EndRead3dmChunk() )
      rc = false;
  }

  return rc;
}

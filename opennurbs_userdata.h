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

#if !defined(OPENNURBS_USERDATA_INC_)
#define OPENNURBS_USERDATA_INC_

class MYON_CLASS MYON_UserData : public MYON_Object
{
  MYON_OBJECT_DECLARE(MYON_UserData);
public:
  MYON_UserData();
  MYON_UserData(const MYON_UserData&);
  MYON_UserData& operator=(const MYON_UserData&);

  //////////
  // The destructor automatically removes the user data
  // from MYON_Object::m_userdata_list.
  ~MYON_UserData();

  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;

  /*
  Description:
    Overrides virtual MYON_Object::Dump().
    Prints class name, description, and uuid.
  Parameters:
    text_log - [in] Information is sent to this text log.
  Remarks:
  */
  void Dump( MYON_TextLog& text_log ) const override;

  /*
  Description:
    Overrides virtual MYON_Object::SizeOf().
  Returns:
    Approximate number of bytes this class uses.
  */
  unsigned int SizeOf() const override;

  ////////
  // Returns object that owns the user data
  MYON_Object* Owner() const;

  ////////
  // Used for traversing list of user data attached
  // to an object.
  MYON_UserData* Next() const;

  ////////
  // Returns the class id which is not necessarily the 
  // same as m_userdata_uuid.
  MYON_UUID UserDataClassUuid() const; 

  //////////
  // Returns true if the user data is anonymous.  This happens
  // when the user data class is not defined at the time the
  // user data is read from an archive.  For example, if a class
  // derived from MYON_UserData is defined in application A
  // but is not defined in application B, then the class can be
  // defined when an archive is written by A but not exist when
  // an archive is read by B.  In this case, the
  // user data is not lost, it is just read as MYON_UnknownUserData
  // by application B.  If application B saves the parent
  // object in an archive, the unknown user data is resaved in
  // a form that can be read by application A.
  bool IsUnknownUserData() const;

  /*
  Parameters:
    description - [out] description of user data shown in 
                        object properties dump.
  Returns:
    True if user data class is ready.
  */
  virtual 
  bool GetDescription( MYON_wString& description );

  /*
  Description:
    If Archive() returns true, m_application_uuid is not nil, 
    and the virtual Read() and Write() are functions are overridden,
    then this user data will be written to and read from 3dm archives.

  Returns:
    true if user data should be saved in binary archives.
    false if the user data should not be saved in binary archives.

  Remarks:
    The default implementation returns false.  If you override
    MYON_UserData::Archive so that it returns true, then your 
    constructor must set m_application_uuid, you must override
    the virtual MYON_Object::Read and MYON_Object::Write functions and
    you must CAREFULLY TEST your code.

    MYON_UserData requires expert programming and testing skills.

    If you need to know more details about the archive or 
    parent object to determine if the userdata should be saved,
    then override WriteToArchive().

    YOU SHOULD READ AND UNDERSTAND EVERY COMMENT IN THIS 
    HEADER FILE IN BEFORE ATTEMPTING TO USE MYON_UserData.
  */
  virtual 
  bool Archive() const; 

  /*
  Description:
    If WriteToArchive() returns true, m_application_uuid is not nil, 
    and the virtual Read() and Write() are functions are overridden,
    then this user data will be written to and read from the 
    identified archive.
    
  Parameters:
    archive - [in]
      The archive being written to. Typically, you will test
      archive.Archive3dmVersion() to deterime if your userdata
      should be saved.
    parent_object - [in]
      The object managing this userdata.
  Returns:
    true if user data should be saved in the binary archives.
    false if the user data should not be saved in binary archives.

  Remarks:
    The default implementation calls the legacy Archive() function.
  */
  virtual
  bool WriteToArchive(
    const class MYON_BinaryArchive& archive,
    const class MYON_Object* parent_object
    ) const;

  /*
  Description:
    DeleteAfterWrite() is used when opennurbs is writing earlier 
    versions of 3dm archives that used some type of user data that
    has since become obsolete.

  Parameters:
    archive - [in]
      archive that will be written to.  
      If needed, you can inspect the version of 3dm archive this
      is being saved and other information that you may need to 
      determine the appropriate return value.
    parent_object - [in]
      If needed, you can inspect the parent object to determine 
      the appropriate return value.

  Returns:
    True if the user data should be written the next
    time the parent object is saved to a 3dm archive and
    then deleted.

  Remarks:
    Typically, DeleteAfterWrite() is used in the situation where
    1) User data was used to add information to an opennurbs class
    whose data fields could not be modified because the SDK
    was fixed at the time.
    2) Once the class data fields could be modified, the new data
    fields were added to the class and the user data from step 1
    became obsolete.
    3) The class's Write function is called and the value of
    MYON_BinaryArchive::Archive3dmVersion() corresponds to
    the version of the 3dm archive that was being saved in
    step 1. The write function fills in and attaches the obsolete
    user data to the class.  When MYON_BinaryArchive::WriteObject()
    writes the obsolete user data to the earlier version file,
    it then deletes it.
  */
  virtual
  bool DeleteAfterWrite(
    const class MYON_BinaryArchive& archive,
    const class MYON_Object* parent_object
    ) const;

  /*
  Description:
    DeleteAfterRead() is used when opennurbs is reading earlier
    versions of 3dm archives that used some type of user data that
    has since become obsolete.

  Parameters:
    archive - [in]
      archive that was read from.
      If needed, you can inspect the version of 3dm archive this
      is being saved and other information that you may need to
      determine the appropriate return value.
    parent_object - [in]
      If needed, you can inspect the parent object to determine
      the appropriate return value.

  Returns:
    True if the user data should be deleted because the 
    information it contains has been added to the parent
    object using the methods that are current.
  */
  virtual
  bool DeleteAfterRead( 
    const class MYON_BinaryArchive& archive,
    class MYON_Object* parent_object
    ) const;

  /*
  Description:
    If Transform() return false, then the userdata is destroyed when 
    its parent object is transformed.  The default Transform() 
    updates m_userdata_xform and returns true. 
    Carefully read the comments above m_userdata_xform
  */
  virtual 
  bool Transform( const MYON_Xform& ); 

  /*
  Description:
    This uuid is the value that must be passed to 
    MYON_Object::GetUserData() to retrieve 
    this piece of user data.
  */
  MYON_UUID m_userdata_uuid;

  /*
  Description:
    This uuid is used to identify the application that
    created this piece of user data.  In the case of
    Rhino, this is the id of the plug-in that created
    the user data. User data with a nil application id
    will not be saved in 3dm archives.
  */
  MYON_UUID m_application_uuid;

  ////////
  // If m_userdata_copycount is 0, user data is not copied when 
  // object is copied.  If > 0, user data is copied and m_copycount
  // is incremented when parent object is copied. The user data's 
  // operator=() is used to copy.  
  // The default MYON_UserData::MYON_UserData() constructor sets 
  // m_userdata_copycount to zero.
  unsigned int m_userdata_copycount;  

  ////////
  // Updated if user data is attached to a piece of geometry that is
  // transformed and the virtual MYON_UserData::Transform() is not 
  // overridden.  If you override MYON_UserData::Transform() and want
  // m_userdata_xform to be updated, then call the 
  // MYON_UserData::Transform() in your override.
  // The default constructor sets m_userdata_xform to the identity.
  MYON_Xform m_userdata_xform; 

private: // don't look and don't touch - these may change
  friend class MYON_Object;
  friend int MYON_BinaryArchive::ReadObject( MYON_Object** );
  friend bool MYON_BinaryArchive::WriteObject( const MYON_Object& );
  friend bool MYON_BinaryArchive::ReadObjectUserData( MYON_Object& );
  friend bool MYON_BinaryArchive::WriteObjectUserData( const MYON_Object& );
  MYON_Object* m_userdata_owner; 
  MYON_UserData* m_userdata_next;
};

class MYON_CLASS MYON_UnknownUserData : public MYON_UserData
{
  MYON_OBJECT_DECLARE(MYON_UnknownUserData);
  // used to hold user data when the application class is not loaded
  // at time data is read
public:
  MYON_UnknownUserData();
  MYON_UnknownUserData(const MYON_UnknownUserData&);
  ~MYON_UnknownUserData();
  MYON_UnknownUserData& operator=(const MYON_UnknownUserData&);

  // MYON_Object overrides

  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;
  void Dump( MYON_TextLog& ) const override;
  bool Write( MYON_BinaryArchive& ) const override;
  bool Read( MYON_BinaryArchive& ) override;

  unsigned int SizeOf() const override; // return amount of memory used by user data
  bool GetDescription( MYON_wString& ) override; // description of user data
  bool Archive() const override; 

  // Convert unknown user data to actual user data.  Useful if
  // definition of actual user data is dynamically linked after
  // archive containing user data is read.
  MYON_UserData* Convert() const;

  /*
  Description:
    This is the uuid of the missing class.  This uuid
    is the 3rd parameter to the MYON_OBJECT_IMPLEMENT()
    macro of the missing class.
  */
  MYON_UUID m_unknownclass_uuid;
  int m_sizeof_buffer;
  void* m_buffer;

  // These version numbers are set when unknown user data is read
  // from a file record the version of the 3dm archive and the 
  // version of opennurbs that were used when the plug-in wrote
  // the user data.
  //   This information was added in to V5 opennurbs 200910190.
  // For files written with earlier versions of opennurbs, these
  // values are set from the archive containing the user data. 
  // The purpose of this version information is to have it accompany
  // unknown user data so that if is is eventually read by the plug-in
  // an MYON_BinaryArchive with correct version information can be
  // passed to the plug-in's reading code.  In archives, these values
  // are stored in the TCODE_USER_TABLE_RECORD_HEADER chunk.
  int m_3dm_version; // 3dm archive version (0,1,2,3,4,5,50,...)
  

  // In V5 and earlier, m_3dm_opennurbs_version had the format YYYYMMDDN
  // For V6 the unsigned int value is calculated by MYON_VersionNumberConstruct()
  // and has the high bit set (it will be negative if used as a signed int).
  // When writing files in previous version formats (V5 or earlier) it is important
  // to write a YYYYMMDDN version number in the file.  Use MYON_VersionNumberParse()
  // get the YYYY, MM, DD and N values from m_3dm_opennurbs_version.
  unsigned int m_3dm_opennurbs_version_number;
};

class MYON_CLASS MYON_ObsoleteUserData : public MYON_UserData
{
  MYON_OBJECT_DECLARE(MYON_ObsoleteUserData);
  // used to write obsolete user data when earlier versions
  // of 3dm archives are written and the class id for the
  // earlier version of the user data is still in use
  // in the current version of opennurbs.
public:
  MYON_ObsoleteUserData();
  virtual ~MYON_ObsoleteUserData();
  MYON_ObsoleteUserData(const MYON_ObsoleteUserData&);
  MYON_ObsoleteUserData& operator=(const MYON_ObsoleteUserData&);

  // This is the user data class id that will be saved in the 3dm archive.
  MYON_UUID m_archive_class_uuid;
};

// Do not export this class
// It is used internally to read and write 3dm achives with versions < 60.
class MYON_RdkMaterialInstanceIdObsoleteUserData : public MYON_ObsoleteUserData
{
  // NO MYON_OBJECT_DECLARE() for classes derived from MYON_ObsoleteUserData
private:
  static const MYON_UUID m_archive_class_id_ctor;
  static const MYON_UUID m_archive_userdata_uuid_ctor;
  static const MYON_UUID m_archive_application_uuid_ctor;
  static const unsigned int m_userdata_copycount_ctor;
  static const MYON_Xform m_userdata_xform_ctor;

public:
  static bool IsRdkMaterialInstanceIdUserData(
    MYON_UUID class_id,
    MYON_UUID userdata_id,
    MYON_UUID app_id,
    MYON_Object* object
    );

  MYON_RdkMaterialInstanceIdObsoleteUserData();
  virtual ~MYON_RdkMaterialInstanceIdObsoleteUserData();
  MYON_RdkMaterialInstanceIdObsoleteUserData(const MYON_RdkMaterialInstanceIdObsoleteUserData&);
  MYON_RdkMaterialInstanceIdObsoleteUserData& operator=(const MYON_RdkMaterialInstanceIdObsoleteUserData&);

  // virtual MYON_Object override
  bool Read(
    MYON_BinaryArchive&
    ) override;

  // virtual MYON_Object override
  bool Write(
    MYON_BinaryArchive&
    ) const override;

  // virtual MYON_UserData override
  bool GetDescription(MYON_wString& description) override;

  // virtual MYON_UserData override
  bool WriteToArchive(
    const class MYON_BinaryArchive& archive,
    const class MYON_Object* parent_object
    ) const override;

  // virtual MYON_UserData override
  bool DeleteAfterWrite(
    const MYON_BinaryArchive& archive,
    const MYON_Object* parent_object
    ) const override;

  // virtual MYON_UserData override
  bool DeleteAfterRead(
    const MYON_BinaryArchive& archive,
    MYON_Object* parent_object
    ) const override;

  MYON_UUID m_rdk_material_instance_id;
};

class MYON_CLASS MYON_UserStringList : public MYON_UserData
{
  MYON_OBJECT_DECLARE(MYON_UserStringList);
public:

  MYON_UserStringList();
  ~MYON_UserStringList();

  static MYON_UserStringList* FromObject(
    const MYON_Object*
    );

  // override virtual MYON_Object::Dump function
  void Dump( MYON_TextLog& text_log ) const override;

  // override virtual MYON_Object::SizeOf function
  unsigned int SizeOf() const override;

  // override virtual MYON_Object::DataCRC function
  MYON__UINT32 DataCRC(MYON__UINT32 current_remainder) const override;

  // override virtual MYON_Object::Write function
  bool Write(MYON_BinaryArchive& binary_archive) const override;

  // override virtual MYON_Object::Read function
  bool Read(MYON_BinaryArchive& binary_archive) override;

  // override virtual MYON_UserData::GetDescription function
  bool GetDescription( MYON_wString& description ) override;

  // override virtual MYON_UserData::Archive function
  bool Archive() const override; 

  /*
  Description:
    Add, replace or remove a user string.
  Parameters:
    key - [in]
      must be a non-empty string.  If an entry with the same key
      (case insensitive compares are used) exists, the existing
      entry is updated.
    string_value - [in]
      If string_value is empty and an entry with a matching key
      exists, the entry is deleted.
  Returns:
    True if the key is valid.
  */
  bool SetUserString( const wchar_t* key, const wchar_t* string_value );

  bool GetUserString( const wchar_t* key, MYON_wString& string_value ) const;

  /*
  Description:
    Append entries to the user string list
  Parameters:
    count - [in]
      number of element in us[] array
    us - [in]
      entries to append.
    bReplace - [in]
      If bReplace is true, then existing entries with the same key are
      updated with the new entry's value.  If bReplace is false, then
      existing entries are not updated.
  Returns:
    Number of entries added, deleted, or modified.
  */
  int SetUserStrings( int count, const MYON_UserString* us, bool bReplace );

  MYON_ClassArray<MYON_UserString> m_e;
};

class MYON_CLASS MYON_UserDataHolder : public MYON_Object
{
public:

  /*
  Description:
    Transfers the user data from source_object to "this".
    When MoveUserDataFrom() returns source_object will not 
    have any user data.  If "this" had user data when 
    MoveUserDataFrom() was called, then that user data is
    destroyed.
  Parameters:
    source_object - [in] The "const" is a lie.  It is
      there because, in practice the source object is frequently
      const and const_cast ends up being excessively used.
  Returns:
    Number of user data items that were moved from source_object to "this" MYON_UserDataHolder.
  */
  unsigned int MoveUserDataFrom(
    const MYON_Object& source_object
    );


  /*
  Description:
    Copies the data from source_object with copy_count > 0 to "this" MYON_UserDataHolder.
  Parameters:
    source_object - [in]
      This object and it's user data are not modified.
    user_data_item_id - [in]
      If not nil, then only userdata with this item id will be coped
  Returns:
    Number of user data items that were copied from source_object to "this" MYON_UserDataHolder.
  */
   unsigned int CopyUserDataFrom(
    const MYON_Object& source_object,
    MYON_UUID user_data_item_id
    );

  /*
  Description:
    Moves the user data on "this" MYON_UserDataHolder to destination_object.
    When MoveUserDataTo() returns "this" MYON_UserDataHolder will not have any user data.
  Parameters:
    destination_object - [in] The "const" is a lie.  It is
      there because, in practice the source object is generally
      const and const_cast ends up being constantly used.
    bAppend - [in]
      true:
        Existing user data on destination_object is left unchanged.
        MoveUserDataTo( destination_object, true ) is identical to calling
        MoveUserDataTo( destination_object, MYON_Object::UserDataConflictResolution::destination_object).
      false:
        Existing user data on destination_object is destroyed.
  Returns:
    Number of user data items moved from "this" MYON_UserDataHolder to destination_object.
  */
  unsigned int MoveUserDataTo(
    const MYON_Object& destination_object,
    bool bAppend 
    );

  /*
  Description:
    Moves the user data on "this" MYON_UserDataHolder to destination_object.
    When MoveUserDataTo() returns "this" MYON_UserDataHolder will not have any user data.
  Parameters:
    destination_object - [in]
      The "const" is a lie.  It is there because, in practice the source object is generally
      const and const_cast ends up being constantly used.
    user_data_item_id - [in]
      If not nil, then only user data items with this id will be considered for moving.
    userdata_conflict_resolution - [in]
      If destination_object and "this" MYON_UserDataHolder have the same
      type of user data item, then userdata_conflict_resolution
      is used to determine if that destination_object user data item
      is replaced with the one on "this" MYON_UserDataHolder.
  Returns:
    Number of user data items moved from "this" MYON_UserDataHolder to destination_object.
  */
  unsigned int MoveUserDataTo(
    const MYON_Object& destination_object,
    MYON_UUID user_data_item_id,
    MYON_Object::UserDataConflictResolution userdata_conflict_resolution
    );


  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;
};

/*
Description:
  An MYON_DocumentUserStringList object is saved in the list of user
  tables.  The Rhino SetDocumentText and GetDocumentText
  commands use the MYON_Object SetUserString, GetUserString,
  GetUserStrings, GetUserStringKeys functions on an 
  MYON_DocumentUserStringList class to manage the tag-value pairs of 
  strings.
*/
class MYON_CLASS MYON_DocumentUserStringList : public MYON_Object
{
  MYON_OBJECT_DECLARE(MYON_DocumentUserStringList);
public:
  MYON_DocumentUserStringList();
  ~MYON_DocumentUserStringList();

  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;
  void Dump( MYON_TextLog& ) const override;
  MYON__UINT32 DataCRC(MYON__UINT32 current_remainder) const override;
  bool Write(MYON_BinaryArchive& binary_archive) const override;
  bool Read(MYON_BinaryArchive& binary_archive) override;

  // Use the
  //   MYON_Object::SetUserString()
  //   MYON_Object::GetUserString()
  //   MYON_Object::GetUserStrings()
  //   MYON_Object::GetUserStringKeys()
  //   MYON_Object::UserStringCount()
  // functions to access and modify user string information.
};

#endif

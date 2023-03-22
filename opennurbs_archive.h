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

#if !defined(MYON_ARCHIVE_INC_)
#define MYON_ARCHIVE_INC_


/////////////////////////////////////////////////////////////////////
//
// MYON_Buffer 
//

typedef void (*MYON_Buffer_ErrorHandler)(class MYON_Buffer*);

class MYON_CLASS MYON_Buffer
{
public:
  MYON_Buffer();
  ~MYON_Buffer();

  MYON_Buffer(const MYON_Buffer& src);
  MYON_Buffer& operator=(const MYON_Buffer& src);

  /*
  Description:
    Compare contents of buffers.
  Parameters:
    a - [in]
    b - [in]
  Returns:
    -1: a < b
     0: a == b
     1: a > b
  */
  static int Compare( const MYON_Buffer& a, const MYON_Buffer& b );

  void Destroy();
  void EmergencyDestroy();

  /*
  Returns:
    True if Size() == CurrentPosition().
  Remarks:
    It is possible to seek beyond the end of the buffer.
    In this case, the current position will be past the end
    of the buffer and AtEnd() will return false.
  */
  bool AtEnd() const;

  /*
  Returns:
    Number of bytes currently in the buffer.
  Remarks:
    It is possible to seek beyond the end of the buffer.
    In this case, the current position will be past the end
    of the buffer and CurrentPosition() will be greater than
    Size().
  */
  MYON__UINT64 Size() const;

  /*
  Returns:
    32-bit CRC of the buffer contents.
  Remarks:
    
  */
  MYON__UINT32 CRC32( MYON__UINT32 current_remainder ) const;


  /*
  Returns:
    Current position in the buffer.
  Remarks:
    It is possible to seek beyond the end of the buffer.
    In this case, the current position will be past the end
    of the buffer and CurrentPosition() will be greater than
    Size().
  */
  MYON__UINT64 CurrentPosition() const;
  
  /*
  Parameters:
    size - [in]
      number of bytes to write.
    buffer - [in]
      values to write.
  Returns:
    Number of bytes written buffer.
  */
  MYON__UINT64 Write( MYON__UINT64 size, const void* buffer );

  /*
  Parameters:
    size - [in]
      number of bytes to read.
    buffer - [out]
      read values are returned in buffer.
  Returns:
    Number of bytes read into buffer. For example, 
    if CurrentPosition() <= Size() and 
    size > (Size() - CurrentPosition()) and
    buffer is not null, then the value
    (Size() - CurrentPosition()) is returned.
  Remarks:
    If the size parameter is zero, then nothing is done.
    When CurrentPosition() <= Size(), attempts to read more 
    than (Size() - CurrentPosition()) bytes do not generate 
    an error. When CurrentPosition() > Size(), any attempt
    to read generates an error.
  */
  MYON__UINT64 Read( MYON__UINT64 size, void* buffer );

  enum
  {
    seek_from_beginning_of_file = 0,
    seek_from_current_position = 1,
    seek_from_end_of_file = 2
  };

  /*
  Parameters:
    offset - [in]
      number of bytes to seek from origin
    origin - [in]
      initial position.
        0 (SEEK_SET) Seek from beginning of file.
        1 (SEEK_CUR) Seek from current position.
        2 (SEEK_END) Seek from end of file.
  Returns:
    True if successful.
    False if the seek would result in a file position
    before the beginning of the file. If false is
    returned, the current position is not changed.
  Remarks:
    Seeking beyond the end of the buffer is succeeds.
    Seeking before the beginning of the buffer fails.
  */
  bool Seek( 
    MYON__INT64 offset, 
    int origin 
    );

  /*
  Parameters:
    offset - [in] (>= 0)
      number of bytes to seek from the start of the buffer.
  Returns:
    True if successful.
    False if the seek would result in a file position
    before the beginning of the file. If false is
    returned, the current position is not changed.
  Remarks:
    Seeking beyond the end of the buffer is succeeds.
    Seeking before the beginning of the buffer fails.
  */
  bool SeekFromStart( MYON__INT64 offset ); 

  /*
  Parameters:
    offset - [in]
      number of bytes to seek from the current position.
  Returns:
    True if successful.
    False if the seek would result in a file position
    before the beginning of the file. If false is
    returned, the current position is not changed.
  Remarks:
    Seeking beyond the end of the buffer is succeeds.
    Seeking before the beginning of the buffer fails.
  */
  bool SeekFromCurrentPosition( MYON__INT64 offset ); 

  /*
  Parameters:
    offset - [in]
      number of bytes to seek from the end fo the buffer.
  Returns:
    True if successful.
    False if the seek would result in a file position
    before the beginning of the file. If false is
    returned, the current position is not changed.
  Remarks:
    Seeking beyond the end of the buffer is succeeds.
    Seeking before the beginning of the buffer fails.
  */
  bool SeekFromEnd( MYON__INT64 offset ); 

  /*
  Parameters:
    buffer_size - [in]
      new size of buffer.
  Returns:
    True if successful.    
  Remarks:
    The current position is not changed and may be beyond the
    end of the file. Use Seek to set the current position after
    calling ChangeSize().
  */
  bool ChangeSize( MYON__UINT64 buffer_size );

  /*
  Description:
    Return unused memory to heap.
  Remarks:
    Call this function after creating an MYON_Buffer that will persist for
    and extended amount of time. There are never more than 16 pages of
    unused memory (16*4096 bytes on most computers) in an MYON_Buffer.
    Compact() can be called at any time, but calling Compact() the then
    writing at the end of the buffer is not an efficient use of time
    or memory.
  */
  bool Compact();

  /*
  Returns
    True if the MYON_Buffer is valid.
  */
  bool IsValid( const MYON_TextLog* text_log ) const;

  /*
  Returns:
    Value that identifies most recent error.
    0: no error
    1: attempt to seek to a negative position
  */
  MYON__UINT32 LastError() const;
  
  void ClearLastError();

  MYON_Buffer_ErrorHandler ErrorHandler() const;
  
  void SetErrorHandler(MYON_Buffer_ErrorHandler error_handler);

  /*
  Description:
    Use WriteToBinaryArchive() to save an entire MYON_Buffer inside
    a binary archive.  Use ReadFromBinaryArchive() to retrieve
    the MYON_Buffer from the MYON_BinaryArchive.
  */
  bool WriteToBinaryArchive( MYON_BinaryArchive& ) const;

  /*
  Description:
    Use ReadFromBinaryArchive() to retrieve an entire MYON_Buffer
    that was written using WriteToBinaryArchive().
  */
  bool ReadFromBinaryArchive( MYON_BinaryArchive& );

  /*
  Description:
    Compress this buffer

  Parameters:
    compressed_buffer - [out]
      (The reference can be *this)
  
  Example:

        // compress a buffer in place
        MYON_Buffer buffer;
        buffer = ...;
        if ( !buffer.Compress(buffer) )
        {
           // compression failed
        }
        else
        {
           // buffer is now compressed
        }

  Returns:
    True if successful.  False if failed.
  */
  bool Compress( MYON_Buffer& compressed_buffer ) const;

  /*
  Description:
    Uncompress this buffer which must have been compressed using
    MYON_Buffer::Compress().

  Parameters:
    uncompressed_buffer - [out]
      (The reference can be *this)

  Example:
        // silly example that compresses and then uncompresses a buffer in place
        // to show how to call the functions.
        MYON_Buffer buffer;
        buffer = ...; // buffer is in it uncompressed form
        if ( buffer.Compress(buffer) )
        {
           // buffer is now compressed
           if ( buffer.Uncompress(buffer) )
           {
              // buffer is uncompressed again.
           }
        }

  Returns:
    True if successful.  False if failed.
  */
  bool Uncompress( MYON_Buffer& uncompressed_buffer ) const;

private:

  MYON__UINT64 m_buffer_size; // total number of bytes in the buffer
  MYON__UINT64 m_current_position;

  struct MYON_BUFFER_SEGMENT* m_first_segment;
  struct MYON_BUFFER_SEGMENT* m_last_segment;
  struct MYON_BUFFER_SEGMENT* m_current_segment;
  bool SetCurrentSegment(bool);
  void Copy( const MYON_Buffer& );

  MYON_Buffer_ErrorHandler m_error_handler;

  MYON__UINT32 m_last_error;
  unsigned char m_reserved[12];
};

/////////////////////////////////////////////////////////////////////
//
// MYON_BinaryArchive 
//      virtual class for CPU independent serialization
//
// MYON_BinaryFile
//      simple class for CPU independent binary file I/O
//      includes optional CRC support
//

struct MYON_3DM_CHUNK
{
  size_t m_offset; // In read or write_using_fseek mode, this is the
                   // file position of first byte after chunk's length.
                   // In write_using_buffer mode, this of the m_buffer[]
                   // position of first byte after chunk's length.
  unsigned int m_typecode;
  int m_value;
  int m_do_length; // true if chunk is a long chunk with length
  MYON__UINT16 m_do_crc16; // 16 bit CRC using CCITT polynomial
  MYON__UINT16 m_crc16;
  MYON__UINT32 m_do_crc32; // 32 bit CRC
  MYON__UINT32 m_crc32;
};

class MYON_CLASS MYON_3DM_BIG_CHUNK
{
public:
  MYON_3DM_BIG_CHUNK() = default;
  ~MYON_3DM_BIG_CHUNK() = default;
  MYON_3DM_BIG_CHUNK(const MYON_3DM_BIG_CHUNK&) = default;
  MYON_3DM_BIG_CHUNK& operator=(const MYON_3DM_BIG_CHUNK&) = default;

public:
  MYON__UINT64 m_start_offset=0; // When reading or writing 3dm archives, this is the
                             // archive offset (file position) of first byte of 
                             // chunk information content.
  
  MYON__UINT64 m_end_offset=0; // When writing 3dm archives, this is the archive 
                           // offset (file position) of the byte immediately after
                           // the farthest successful write. 
                           // When reading 3dm archives, this the archive offset
                           // of the first byte after the chunk's information content.
                           // When reading, a 16 bit or 32 bit CRC can follow the chunk
                           // information content.
                           // During ordinary reading and writing, valid seek target 
                           // positions satisfy 
                           // m_start_offset <= seek target pos <= m_end_offset.

  /*
  Returns:
    Number of bytes in the chunk, including bytes used to store CRC values.
    0 for short chunks.
    0 for chunks currently being written.
  Remarks:
    For chunks being read,
     m_start_offset + Length() = m_end_offset + SizeofCRC().
  */
  MYON__UINT64 Length() const;

  /*
  Parameters:
    current_position - [in]
      Value of MYON_BinaryArchive.CurrentPosition()

  Returns:
    Number of bytes that can be read when MYON_BinaryArchive ReadMode() is true.
  */
  MYON__UINT64 LengthRemaining(
    MYON__UINT64 current_position
  ) const;

  /*
  Returns:
    0: no CRC
    4: 32 bit CRC (4 bytes)
    2: 16 bit CRC (2 bytes)
  */
  MYON__UINT64 SizeofCRC() const;

  MYON__INT64 m_big_value=0;
  MYON__UINT32 m_typecode=0;  
  MYON__UINT8 m_bLongChunk=0; // true if chunk is a long chunk and m_big_value is a length.

private:
  MYON__UINT8 m_reserved1=0;
  MYON__UINT8 m_reserved2=0;
  MYON__UINT8 m_reserved3=0;

public:
  // CRC settings
  MYON__UINT8 m_do_crc16=0; // true (1) if we are calculating 16 bit CRC
  MYON__UINT8 m_do_crc32=0; // true (1) if we are calculating 32 bit CRC
  MYON__UINT16 m_crc16=0; // current 16 bit CRC value
  MYON__UINT32 m_crc32=0; // current 32 bit CRC value
};

bool MYON_IsLongChunkTypecode(MYON__UINT32 typecode);

bool MYON_IsShortChunkTypecode(MYON__UINT32 typecode);

#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_3DM_CHUNK>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_3DM_BIG_CHUNK>;
#endif

// Used int MYON_3dmProperties::Read() to set MYON_BinaryArchive.m_3dm_opennurbs_version
// Do not call directly. 
void MYON_SetBinaryArchiveOpenNURBSVersion(MYON_BinaryArchive&,unsigned int);

class MYON_CLASS MYON_UserDataItemFilter
{
public:
  MYON_UserDataItemFilter();

  MYON_UserDataItemFilter(
    MYON_UUID application_id,
    bool bSerialize
    );

  MYON_UserDataItemFilter(
    MYON_UUID application_id,
    MYON_UUID item_id,
    bool bSerialize
    );

  static int Compare(
    const class MYON_UserDataItemFilter*,
    const class MYON_UserDataItemFilter*
    );

  // The application id can be the id for a plug-in, Rhino or opennurbs
  MYON_UUID m_application_id;

  // The item id for object user data is the value of MYON_UserData.m_userdata_uuid.
  // The item id for user table is the application id.
  // A nil item id indicates the setting is applied to all object user data 
  // and user table information for the specified application.
  MYON_UUID m_item_id;

  // If application id and item id match and m_bSerializeEnabled,
  // does not match, then the MYON_UserDataItemFilter with the
  // largest value of m_precedence is used.
  unsigned int m_precedence;

  // bSerializationEnabled is true if reading and writing are permitted.
  // bSerializationEnabled is false if reading and writing are prevented.
  bool m_bSerialize;
};

#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_UserDataItemFilter>;
#endif

class MYON_CLASS MYON_ComponentManifest
{
public:
  // The default constructor would work prfectly,
  // except there is a bug in Apple's CLANG that
  // requires either an explicitly implemented constructor
  // or an explicitly implemented copy constructor together
  // with a hack to initialize the static MYON_ComponentManifest::Empty.
  // Apple CLANG BUG // MYON_ComponentManifest() = default;
  MYON_ComponentManifest() MYON_NOEXCEPT;

  ~MYON_ComponentManifest();

  static const MYON_ComponentManifest Empty;

  void Reset();

  enum : int
  {
    UnsetComponentIndex = MYON_UNSET_INT_INDEX
  };

private:
  MYON_ComponentManifest(const MYON_ComponentManifest&) = delete;
  MYON_ComponentManifest& operator=(const MYON_ComponentManifest&) = delete;

public:

  /*
    Total number of items in the manifest, including items referencing system components and deleted items.
  */
  unsigned int ItemCount() const;

  /*
  Parameters:
    component_type - [in]
      If component_type is MYON_ModelComponent::Type::Unset or MYON_ModelComponent::Type::Mixed,
      then the every explicit component type is counted.
  Returns:
    Total number of model components of the specified type in this manifest. 
  Remarks:
    The count includes active, deleted, and system components.
  */
  unsigned int TotalComponentCount(
    MYON_ModelComponent::Type component_type
    ) const;

  /*
  Parameters:
    component_type - [in]
      If component_type is MYON_ModelComponent::Type::Unset or MYON_ModelComponent::Type::Mixed,
      then the every explicit component type is counted.
  Returns:
    Number of model components of the specified type in this manifest. 
  Remarks:
    The count includes active and deleted components.
    The count does not include system components (those added by calling AddSystemComponentToManifest()).
  */
  unsigned int ActiveAndDeletedComponentCount(
    MYON_ModelComponent::Type component_type
    ) const;

  /*
  Parameters:
    component_type - [in]
      If component_type is MYON_ModelComponent::Type::Unset or MYON_ModelComponent::Type::Mixed,
      then the every explicit component type is counted.
  Returns:
    Number of active model components of the specified type in this manifest. 
  Remarks:
    The count does not include deleted components (IsDeleted() = true).
    The count does not include system components (those added by calling AddSystemComponentToManifest()).
  */
  unsigned int ActiveComponentCount(
    MYON_ModelComponent::Type component_type
    ) const;

  /*
  Parameters:
    component_type - [in]
      If component_type is MYON_ModelComponent::Type::Unset or MYON_ModelComponent::Type::Mixed,
      then the every explicit component type is counted.
  Returns:
    Number of model components of the specified type in this manifest that have IsDeleted() = true.
  Remarks:
    System components cannot be deleted.
  */
  unsigned int DeletedComponentCount(
    MYON_ModelComponent::Type component_type
    ) const;

  unsigned int SystemComponentCount(
    MYON_ModelComponent::Type component_type
    ) const;

  /*
  Parameters:
    component_type - [in]
  Returns:
    If the component type is indexed, then all current manifest indices
    for the component_type are >= 0 and < ComponentIndexLimit().
    Otherwise 0 is returned.
  */
  int ComponentIndexLimit(
    MYON_ModelComponent::Type component_type
    ) const;

  /*
  Description:
    Add a component to this manifest.
    If the id is not set or not unique, the component will not be added.
    If a unique name is required and the name is not set or not unique,
    the component will not be added.
  Parameters:
    component - [in]
      If you want to update the component id, index and name values to 
      match the ones assigned in the manifest, then call 
      component.SetIdentification(manifest_item),
      where manifest_item is the information returned by this function.
    bResolveIdAndNameCollisions - [in]
      If false, then the component parameter id must not be used in the
      manifest and, when required, the name must be set and unique.
      If true and a new id or name is required, one will be assigned.
      Note that the component parameter is const and its id and name
      are not modified.
    assigned_name - [out]
      If not null, the assigned name is returned here.
  Returns:
    If an item is added to this manifest, then the assigned
    identification information is returned. 
    Otherwise MYON_ComponentManifestItem::Unset is returned.
    Note the manifest index is generally different from component.Index().
  Remarks:
    Use 
  */
  const class MYON_ComponentManifestItem& AddComponentToManifest(
    const class MYON_ModelComponent& component,
    bool bResolveIdAndNameCollisions,
    MYON_wString* assigned_name
    );

  const class MYON_ComponentManifestItem& AddSystemComponentToManifest(
    const class MYON_ModelComponent& component
    );


  /*
  Description:
    Add a component to this manifest.
  Parameters:
    component_type - [in]
      Type of component.
    component_serial_number - [in]
      0 or the component's unique runtime serial number (MYON_ModelComponent::RuntimeSerialNumber()).
    component_id - [in]
    component_name_hash - [in]
      If the the component type requires a unique name and the name
      is not valid or in use, the component will not be added.
  Returns:
    If an item is added to this manifest, then the identification
    information is returned.
    Otherwise MYON_ComponentManifestItem::Unset is returned.
    Note: 
    The manifest index is assigned to components that require an index.
  */
  const class MYON_ComponentManifestItem& AddComponentToManifest(
    MYON_ModelComponent::Type component_type,
    MYON__UINT64 component_serial_number,
    MYON_UUID component_id,
    const MYON_NameHash& component_name_hash
    );

  /*
  Description:
    Add a component to this manifest.  
    If the id is not set or in use, then a new one will be assigned.
    If the component type requires a unique name and the name is not set or in use,
    then a new one will be assigned.
  Parameters:
    component_type - [in]
      Type of component.
    component_serial_number - [in]
      0 or the component's unique runtime serial number (MYON_ModelComponent::RuntimeSerialNumber()).
    component_id - [in]
      If the id is nil or in use, a new id will be assigned.
    component_name_hash - [in]
      If the the component type requires a unique name and the name
      is not valid or in use, the component will not be added.
    original_name - [in/out]
      If a new name needs to be assigned, the input value will be used
      as a candidate and then as the root. Passing in the current name
      is a good choice. The output value is the final assigned name.
  Returns:
    If an item is added to this manifest, then the identification
    information is returned.
    Otherwise MYON_ComponentManifestItem::Unset is returned.
  */
  const class MYON_ComponentManifestItem& AddComponentToManifest(
    MYON_ModelComponent::Type component_type,
    MYON__UINT64 component_serial_number,
    MYON_UUID component_parent_id,
    MYON_UUID component_id,
    const MYON_NameHash& component_name_hash,
    const wchar_t* candidate_name,
    MYON_wString& assigned_name
    );

  const class MYON_ComponentManifestItem& AddComponentToManifest(
    MYON_ModelComponent::Type component_type,
    MYON__UINT64 component_serial_number,
    MYON_UUID component_parent_id,
    MYON_UUID component_id,
    const wchar_t* original_name,
    MYON_wString& assigned_name
    );


  /*
  Description:
    Modify a manifest items's component name
  Parameters:
    item_id - [in]
      Identifies the manifest item to modify.
    component_parent_id - [in]
      MYON_ModelComponent.ParentId() value.
      When MYON_ModelComponent::UniqueNameIncludesParent(component_type) is true,
      it is critical that component_parent_id be set correctly.
    name - [in]
      new name
  Returns:
    True if name was modified.
  */
  const class MYON_ComponentManifestItem& ChangeComponentName(
    MYON_UUID item_id,
    MYON_ModelComponent::Type component_type,
    MYON_UUID component_parent_id,
    const wchar_t* component_name
    );

  /*
  Description:
    Modify a manifest items's component name
  Parameters:
    component - [in]
      The component that is in the manifest with the new name set.
  Returns:
    True if name was modified.
  */
  const class MYON_ComponentManifestItem& ChangeComponentName(
    const class MYON_ModelComponent& component
    );

  /*
  Description:
    A function for expert users to directly set the
    component's name hash. Generally, it is better
    to use the ChangeComponentName() functions.
  Parameters:
    item_id - [in]
      Identifies the manifest item to modify.
    component_name_hash - [in]
      new name hash
  */
  const class MYON_ComponentManifestItem& ChangeComponentNameHash(
    MYON_UUID item_id,
    const MYON_NameHash& component_name_hash
    );

  /*
  Description:
    Modify a manifest items's component m_component_runtime_serial_number,
    m_original_index, m_original_id, and m_name_hash values.
  Parameters:
    manifest_id - [in]
      identifies the manifest item to modify
    component_runtime_serial_number - [in]      
  */
  const class MYON_ComponentManifestItem& ChangeComponentRuntimeSerialNumber(
    MYON_UUID item_id,
    MYON__UINT64 component_runtime_serial_number
    );

  /*
  Description:
    Set a component's status to deleted.
  */
  const class MYON_ComponentManifestItem& DeleteComponent(
    MYON_UUID item_id
    );

  const class MYON_ComponentManifestItem& DeleteComponent(
    MYON__UINT64 component_runtime_serial_number
    );

  /*
  Description:
    Undelete a previously deleted component.
  */
  const class MYON_ComponentManifestItem& UndeleteComponent(
    MYON_UUID item_id,
    MYON_UUID parent_id,
    const wchar_t* candidate_name,
    MYON_wString& assigned_name
    );

  /*
  Description:
    Undelete a previously deleted component with the same id and
    change the serial number to new_component_runtime_serial_number.
  Remarks:
    Often when an object is modified, the original and new
    object have the same id but different serial numbers. The original is
    deleted. When the item is undeleted for the object, the runtime
    serial number needs to be udated.
  */
  const class MYON_ComponentManifestItem& UndeleteComponentAndChangeRuntimeSerialNumber(
    MYON_UUID item_id,
    MYON_UUID parent_id,
    MYON__UINT64 new_component_runtime_serial_number,
    const wchar_t* candidate_name,
    MYON_wString& assigned_name
    );

  bool RemoveComponent(
    const MYON_ModelComponent& component
    );

  bool RemoveComponent(
    MYON__UINT64 component_runtime_serial_number
    );

  bool RemoveComponent(
    MYON_UUID item_id
    );

  bool RemoveIndexedComponent(
    MYON_ModelComponent::Type component_type,
    int item_index
    );

  bool RemoveAllComponents(
    MYON_ModelComponent::Type component_type,
    bool bResetManifestIndex
    );
  
  /*
  Description:
    Get a name that is currently not used in this manifest as either a component
    or manifest name.
  Parameters:
    component_type - [in]
      MYON_ModelComponent::ComponentTypeIsValidAndNotMixed(component_type) must be true.
    component_parent_id - [in]
      If MYON_ModelComponent::UniqueNameIncludesParent(component_type) is true and
      candidate_name is not empty, then the component parent id must be accurate.
      This is the case for MYON_Layer names. 
      Otherwise, you may pass MYON_nil_uuid.
    candidate_name - [in]
      If candidate_name is a valid and not it use,
      then unused_component_name = candidate_name.
      If MYON_ModelComponent::UniqueNameIncludesParent(component_type) is true and
      candidate_name is not empty, then component_parent_id must be accurate.
      This is the case for MYON_Layer names.
    base_name - [in]
      If base_name is empty or not valid, 
      then MYON_ModelComponent::ComponentTypeToString(component_type) is used as base_name
    suffix_separator - [in]
      empty or the string to place between base_name and the suffix when searching for an
      unused name.
    suffix0 - [in]
      If a suffix needs to be appended, the search for a 
      unused name begins with the suffix values suffix0+1.
    suffix_value - [out]
      If nullptr != suffix_value, the value used to generate the 
      unique name suffix is returned.
  Returns:
    An component name that is not used in this manifest.
  Remarks:
    If candidate_name could not be used, then it has the form 
    base_name + suffix_separator + X, where X is an integer > suffix0.
  */
  const MYON_wString UnusedName(
    MYON_ModelComponent::Type component_type,
    MYON_UUID component_parent_id,
    const wchar_t* candidate_name,
    const wchar_t* base_name,
    const wchar_t* suffix_separator,
    unsigned int suffix0,
    unsigned int* suffix_value
    ) const;

  /*
  Description:
    Get a name that is currently not used in this manifest as either a component
    or manifest name.
  Parameters:
    model_component - [in]
      The component type, id, parent id, and candidate name parameters for the
      more complicated version of UnusedName() are taken from this parameter.
  Returns:
    An component name that is not used in this manifest.
  Remarks:
    If candidate_name could not be used, then it has the form 
    base_name + suffix_separator + X, where X is an integer > suffix0.
  */
  const MYON_wString UnusedName(
    const MYON_ModelComponent& model_component
    ) const;

  /*
  Parameters:
    component_type - [in]
      MYON_ModelComponent::ComponentTypeIsValidAndNotMixed(component_type)
      must be true or false will be returned.
    candidate_name_hash - [in]
      candidate_name_hash.IsValidAndNotEmpty()
      must be true or false will be returned.
  Returns:
    True if the candidate_name_hash a hash of a valid, non-empty name and the
    name is currently not used as either a component or manifest name value.
  */
  bool NameIsAvailable(
    MYON_ModelComponent::Type component_type,
    const MYON_NameHash& candidate_name_hash
    ) const;

  /*
  Description:
    Get an id that is not currently used in this manifest
  Parameters:
    component_type - [in]
      MYON_ModelComponent::ComponentTypeIsValidAndNotMixed(component_type) must be true.
    candidate_id
      If candidate_id is valid component id and not in use,
      then its value is returned.
  Returns:
    An id that is valid and currently not used in this MYON_Manifest as 
    either a component or a manifest id value.
  Remarks:
    If candidate_id cannot be used, then MYON_CreateId() is used to create a new id.
  */
  MYON_UUID UnusedId(
    MYON_UUID candidate_id
    ) const;

  /*
  Returns:
    True if the id is valid and currently not used in this MYON_Manifest as 
    either a component or a manifest id value.
  */
  bool IdIsAvailable(
    MYON_UUID id
    ) const;
  
  //////////////////////////////////////////////////////////////////
  //
  // Query tools to get item identificaion information
  //
  //    
  const class MYON_ComponentManifestItem& ItemFromId(
    MYON_UUID item_id
    ) const;

  const class MYON_ComponentManifestItem& ItemFromComponentRuntimeSerialNumber(
    MYON__UINT64 component_runtime_serial_number
    ) const;

  /*
  Description:
    Returns the item if it has the required component type and id.
  Remarks:
    Every item has a unique manifest id.  The component_type
    parameter is provided if an additional check needs to be
    made on component type.
  */
  const class MYON_ComponentManifestItem& ItemFromId(
    MYON_ModelComponent::Type component_type,
    MYON_UUID item_id
    ) const;

  /*
  Parameters:
    component_type - [in]
    model_component - [in]
      The value of MYON_ModelComponent::UniqueNameIgnoresCase(component_type) must be used
      when creating the name hash (group names are case sensitive).
  */
  const class MYON_ComponentManifestItem& ItemFromName(
    const class MYON_ModelComponent* model_component
    ) const;

  /*
  Parameters:
    component_type - [in]
    parent_id - [in]
      If  MYON_ModelComponent::UniqueNameIncludesParent(component_type) is true, 
      then the parent_id must be used to calculate the name hash 
      (layer names require parent ids).
  */
  const class MYON_ComponentManifestItem& ItemFromName(
    MYON_ModelComponent::Type component_type,
    MYON_UUID parent_id,
    const wchar_t* name
    ) const;

  /*
  Parameters:
    component_type - [in]
    component_name_hash - [in]
      The value of MYON_ModelComponent::UniqueNameIgnoresCase(component_type) must be used
      when creating the name hash (group names are case sensitive).

      If  MYON_ModelComponent::UniqueNameIncludesParent(component_type) is true, 
      then the parent_id must be used to calculate the name hash 
      (layer names require parent ids).
  */
  const class MYON_ComponentManifestItem& ItemFromNameHash(
    MYON_ModelComponent::Type component_type,
    const MYON_NameHash& component_name_hash
    ) const;

  const class MYON_ComponentManifestItem& ItemFromIndex(
    MYON_ModelComponent::Type component_type,
    int item_index
    ) const;

  const class MYON_ComponentManifestItem& ItemFromUnsignedIndex(
    MYON_ModelComponent::Type component_type,
    unsigned int unsigned_item_index
    ) const;

  const class MYON_ComponentManifestItem& SystemItemFromNameHash(
    MYON_ModelComponent::Type component_type,
    const MYON_NameHash& system_item_name_hash
    ) const;

  const class MYON_ComponentManifestItem& SystemItemFromIndex(
    MYON_ModelComponent::Type component_type,
    int system_item_index
    ) const;

  const class MYON_ComponentManifestItem* FirstItem(
    MYON_ModelComponent::Type component_type
    ) const;

  const class MYON_ComponentManifestItem* LastItem(
    MYON_ModelComponent::Type component_type
    ) const;

  /*
  Returns:
    Next item in the manifest with the same component type.
  */
  const class MYON_ComponentManifestItem* NextItem(
    const class MYON_ComponentManifestItem* item
    ) const;

  /*
  Returns:
    Previous item in the manifest with the same component type.
  */
  const class MYON_ComponentManifestItem* PreviousItem(
    const class MYON_ComponentManifestItem* item
    ) const;

  /*
  Returns:
    Next item in the manifest with the same component type.
  */
  const class MYON_ComponentManifestItem* NextItem(
    MYON_UUID manifest_item_id
    ) const;

  /*
  Returns:
    Previous item in the manifest with the same component type.
  */
  const class MYON_ComponentManifestItem* PreviousItem(
    MYON_UUID manifest_item_id
    ) const;

  /*
  Description:
    This number is incremented every time the manifest changes.
  */
  MYON__UINT64 ManifestContentVersionNumber() const;

private:
  const class MYON_ComponentManifestItem* Internal_AddItem(
    class MYON_ComponentManifestItem& item,
    MYON_UUID component_parent_id,
    bool bResolveIdAndNameCollisions,
    const wchar_t* candidate_name,
    MYON_wString* assigned_name
    );

private:
  class MYON_ComponentManifestImpl* Impl() const;
  mutable class MYON_ComponentManifestImpl* m_impl = nullptr;
};

class MYON_CLASS MYON_ComponentManifestItem
{
public:
  static const MYON_ComponentManifestItem UnsetItem;

  static int CompareComponentType(
    const MYON_ComponentManifestItem* a, 
    const MYON_ComponentManifestItem* b
    );

  static int CompareId(
    const MYON_ComponentManifestItem*const* a, 
    const MYON_ComponentManifestItem*const* b
    );

  static int CompareNameHash(
    const MYON_ComponentManifestItem*const* a,
    const MYON_ComponentManifestItem*const* b
    );

  static int CompareIndex(
    const MYON_ComponentManifestItem*const* a, 
    const MYON_ComponentManifestItem*const* b
    );

public:
  // Assigns component type, index, id and name hash
  MYON_ComponentManifestItem(
    const class MYON_ModelComponent& component
    );

  MYON_ComponentManifestItem(
    const class MYON_ModelComponent& component,
    const MYON_UUID& manifest_id,
    const class MYON_NameHash& manifest_name_hash
    );

  MYON_ComponentManifestItem(
    MYON_ModelComponent::Type component_type,
    MYON__UINT64 m_component_runtime_serial_number,
    const MYON_UUID& manifest_id,
    const class MYON_NameHash& manifest_name_hash
    );

  MYON_ComponentManifestItem(
    const class MYON_ModelComponent& component,
    int manifest_index,
    const MYON_UUID& manifest_id,
    const class MYON_NameHash& manifest_name_hash
    );

  MYON_ComponentManifestItem(
    MYON_ModelComponent::Type component_type,
    MYON__UINT64 m_component_runtime_serial_number,
    int manifest_index,
    const MYON_UUID& manifest_id,
    const class MYON_NameHash& manifest_name_hash
    );

  MYON_ComponentManifestItem() = default;
  ~MYON_ComponentManifestItem() = default;
  MYON_ComponentManifestItem(const MYON_ComponentManifestItem&) = default;
  MYON_ComponentManifestItem& operator=(const MYON_ComponentManifestItem&) = default;

public:
  /*
  Returns:
    true if m_component_type is not MYON_ModelComponent::Type::Unset
    and the m_manifest_id is not nil.
  */
  bool IsValid() const;

  /*
  Returns:
    true if m_component_type is MYON_ModelComponent::Type::Unset
    or the m_manifest_id is nil.
  */
  bool IsUnset() const;
  
  /*
  Returns:
    true if the item is in a deleted state.
    Name is erased. 
    The component can be found by component serial number, id, or index.
  */
  bool IsDeleted() const;

  /*
  Returns:
    true if the item is a constant system component.
  */
  bool IsSystemComponent() const;

public:
  /*
  Return:
    item component type. MYON_ModelComponent::Type::Unset if it is not set.
  */
  MYON_ModelComponent::Type ComponentType() const;

  void SetComponentType(
    MYON_ModelComponent::Type component_type
  );

public:
  /*
  Return:
    item id. MYON_nil_uuid if is not set.
  */
  MYON_UUID Id() const;

  void SetId(
    MYON_UUID id
  );

public:
  /*
  Return:
    item component runtime serial number. 0 if it is not set.
  */
  MYON__UINT64 ComponentRuntimeSerialNumber() const;

  void SetComponentRuntimeSerialNumber(
    MYON__UINT64 component_runtime_serial_number
  );

public:
  /*
  Return:
    item name hash. MYON_NameHash::UnsetNameHash if is not set.
  */
  const MYON_NameHash& NameHash() const;

  void SetNameHash(
    const MYON_NameHash& name_hash
  );

public:
  /*
  Return:
    item index. MYON_UNSET_INT_INDEX if it is not set.
  */
  int Index() const;

  void SetIndex(
    int index
  );

private:
  friend class MYON_ComponentManifestImpl;

  void Internal_SetDeletedState(
    bool bDeleted
    );

private:
  MYON__UINT32 m_status_bits = 0;
  MYON_ModelComponent::Type m_component_type = MYON_ModelComponent::Type::Unset;
  MYON__UINT8 m_reserved1 = 0;
  MYON__UINT16 m_reserved2 = 0;
  MYON__UINT32 m_reserved3 = 0;
  int m_index = MYON_UNSET_INT_INDEX;
  MYON__UINT64 m_component_runtime_serial_number = 0;
  MYON_UUID m_id = MYON_nil_uuid;
  MYON_NameHash m_name_hash = MYON_NameHash::UnsetNameHash;
};

class MYON_CLASS MYON_ManifestMapItem
{
public:
  MYON_ManifestMapItem() = default;
  ~MYON_ManifestMapItem() = default;
  MYON_ManifestMapItem(const MYON_ManifestMapItem&) = default;
  MYON_ManifestMapItem& operator=(const MYON_ManifestMapItem&) = default;

public:
  static const MYON_ManifestMapItem Unset;
  
  /*
  Description:
    Compares type, indices and ids.
  */
  static int Compare(
    const MYON_ManifestMapItem& a,
    const MYON_ManifestMapItem& b
  );

  static int CompareTypeAndSourceId(
    const MYON_ManifestMapItem& a,
    const MYON_ManifestMapItem& b
  );

  static int CompareTypeAndDestinationId(
    const MYON_ManifestMapItem& a,
    const MYON_ManifestMapItem& b
  );

  static int CompareTypeAndSourceIdAndIndex(
    const MYON_ManifestMapItem& a,
    const MYON_ManifestMapItem& b
  );

  static int CompareTypeAndDestinationIdAndIndex(
    const MYON_ManifestMapItem& a,
    const MYON_ManifestMapItem& b
  );

  static int CompareTypeAndSourceIndex(
    const MYON_ManifestMapItem& a,
    const MYON_ManifestMapItem& b
  );

  static int CompareTypeAndDestinationIndex(
    const MYON_ManifestMapItem& a,
    const MYON_ManifestMapItem& b
  );

  /*
  Description:
    32-bit hash for use in source id hash tables
  */
  static MYON__UINT32 SourceIdHash32(
    const MYON_UUID& source_component_id
  );

  /*
  Description:
    32-bit hash for use in source index hash tables
  */
  static MYON__UINT32 SourceIndexHash32(
    MYON_ModelComponent::Type component_type,
    int source_component_index
  );

  /*
  Returns:
    True if 
    m_component_type is not MYON_ModelComponent::Type::Unset
    and m_source_component_id is not nil
    and m_destination_component_id is not nil
    and no index is required or m_source_component_index and m_destination_component_index
    are not MYON_UNSET_INT_INDEX.
  */
  bool SourceAndDestinationAreSet() const;

  bool SourceOrDestinationIsUnset() const;

  /*
  Returns:
    True if 
    m_component_type is not MYON_ModelComponent::Type::Unset
    and m_source_component_id is not nil
    and no index is required or m_source_component_index is not MYON_UNSET_INT_INDEX.
  */
  bool SourceIsSet() const;

  bool SourceIsUnset() const;

  /*
  Returns:
    True if 
    m_component_type is not MYON_ModelComponent::Type::Unset
    and m_destination_component_id is not nil
    and no index is required or m_destination_component_index is not MYON_UNSET_INT_INDEX.
  */  
  bool DestinationIsSet() const;

  bool DestinationIsUnset() const;

  /*
  Returns:
    True if destination_manifest contains a manifest item that matches
    m_component_type, m_destination_component_id, and m_destination_component_index.
  */
  bool DestinationInManifest(
    const MYON_ComponentManifest& destination_manifest
    ) const;

  /*
  Returns:
    True if destination_manifest contains a manifest item that matches
    m_component_type, m_source_component_id, and m_source_component_index.
  */
  bool SourceInManifest(
    const MYON_ComponentManifest& source_manifest
    ) const;

  MYON_ManifestMapItem SwapSourceAndDestiation() const;

  MYON_ModelComponent::Type ComponentType() const;
  const MYON_UUID& SourceId() const;
  const MYON_UUID& DestinationId() const;
  int SourceIndex() const;
  int DestinationIndex() const;
  
  bool ClearSourceIdentification();

  bool ClearDestinationIdentification();

  /*
  Description:
    Set type and source identification.
  Parameters:
    component_type - [in]
    source_id - [in]
    source_index - [in]
  Returns:
    True if set.
    False destination type is set and different from component_type.
  */
  bool SetSourceIdentification(
    MYON_ModelComponent::Type component_type,
    MYON_UUID source_id,
    int source_index
    );

  /*
  Description:
    Set type and destination identification.
  Parameters:
    component_type - [in]
    source_id - [in]
    source_index - [in]
  Returns:
    True if set.
    False destination type is set and different from component_type.
  */  
  bool SetDestinationIdentification(
    MYON_ModelComponent::Type component_type,
    MYON_UUID destination_id,
    int destination_index
    );

  /*
  Description:
    Set type and source identification to model_component identification.
  Parameters:
    model_component - [in]
  Returns:
    True if set.
    False destination type is set and different from model_component->ComponentType().
  */
  bool SetSourceIdentification(
    const class MYON_ModelComponent* model_component
    );

  /*
  Description:
    Set type and destination identification to model_component identification.
  Parameters:
    model_component - [in]
  Returns:
    True if set.
    False source type is set and different from model_component->ComponentType().
  */
  bool SetDestinationIdentification(
    const class MYON_ModelComponent* model_component
    );

  /*
  Description:
    Set type and source identification to manifest_item identification.
  Parameters:
    manifest_item - [in]
  Returns:
    True if set.
    False destination type is set and different from manifest_item->ComponentType().
  */

  bool SetSourceIdentification(
    const class MYON_ComponentManifestItem* manifest_item
    );

  /*
  Description:
    Set type and destination identification to manifest_item identification.
  Parameters:
    manifest_item - [in]
  Returns:
    True if set.
    False source type is set and different from manifest_item->ComponentType().
  */
  bool SetDestinationIdentification(
    const class MYON_ComponentManifestItem* manifest_item
    );

  /*
  Description:
    Copy type and source identification from map_item.
  Parameters:
    map_item - [in]
  Returns:
    True if set.
    False destination type is set and different from map_item->ComponentType().
  */
  bool SetSourceIdentification(
    const class MYON_ManifestMapItem* map_item
    );

  /*
  Description:
    Copy type and destination identification from map_item.
  Parameters:
    map_item - [in]
  Returns:
    True if set.
    False source type is set and different from map_item->ComponentType().
  */
  bool SetDestinationIdentification(
    const class MYON_ManifestMapItem* map_item
    );

private:
  bool Internal_SetSourceOrDestinationIdentification(
    unsigned int which_identification, // 0 = source, 1 = destination
    MYON_ModelComponent::Type component_type,
    MYON_UUID id,
    int index
  );

private:
  MYON_ModelComponent::Type m_component_type = MYON_ModelComponent::Type::Unset;
private:
  unsigned int m_reserved = 0;
private:
  int m_source_index = MYON_UNSET_INT_INDEX;
  int m_destination_index = MYON_UNSET_INT_INDEX;
private:
  MYON_UUID m_source_id = MYON_nil_uuid;
  MYON_UUID m_destination_id = MYON_nil_uuid;
};

MYON_DECL
bool operator==(const MYON_ManifestMapItem& lhs,const MYON_ManifestMapItem& rhs);

MYON_DECL
bool operator!=(const MYON_ManifestMapItem& lhs,const MYON_ManifestMapItem& rhs);


/*
Description:
  MYON_ManifestIdentificationMap is used to record a map from
  a source manifest to a destination manifest when the index or id
  values change. This is common when reading and writing archives
  and when merging models.
*/
class MYON_CLASS MYON_ManifestMap
{
public:
  // The default constructor would work prfectly,
  // except there is a bug in Apple's CLANG that
  // requires either an explicitly implemented constructor
  // or an explicitly implemented copy constructor together
  // with a hack to initialize the static MYON_ComponentManifest::Empty.
  // Apple CLANG BUG // MYON_ManifestMap() = default;
  MYON_ManifestMap() MYON_NOEXCEPT;

  ~MYON_ManifestMap();
  MYON_ManifestMap(const MYON_ManifestMap&);
  MYON_ManifestMap& operator=(const MYON_ManifestMap&);

public:
  static const MYON_ManifestMap Empty;
    
public:
  bool AddMapItem(
    const class MYON_ManifestMapItem& map_item
    );

  /*
  Parameters:
    map_item - [in]
      The source settings must exactly match source settings of an existing map.
      The destination settings are the new values to assign.
  Return:
    True if a mapping was successfully updated (even when the destation settings did not change).
  */
  bool UpdatetMapItemDestination(
    const class MYON_ManifestMapItem& map_item
    );

  /*
  Parameters:
    map_item - [in]
      The source settings must exactly match source settings of an existing map.
      The destination settings are the new values to assign.
    bIgnoreSourceIndex - [in]
      If true, the value of map_item.SourceIndex() is ignored.
      Otherwise, it must exactly match the source index setting of an existing map.
  Return:
    True if a mapping was successfully updated (even when the destation settings did not change).
  */
  bool UpdatetMapItemDestination(
    const class MYON_ManifestMapItem& map_item,
    bool bIgnoreSourceIndex
    );

  const class MYON_ManifestMapItem& MapItemFromSourceId(
    const MYON_UUID& source_item_id
    ) const;

  const class MYON_ManifestMapItem& MapItemFromSourceIndex(
    MYON_ModelComponent::Type component_type,
    int source_component_index
    ) const;

  bool GetAndValidateDestinationIndex(
    MYON_ModelComponent::Type component_type,
    int source_component_index,
    const MYON_ComponentManifest& destination_manifest,
    int* destination_component_index
    ) const;

  bool GetAndValidateDestinationIndex(
    MYON_ModelComponent::Type component_type,
    const MYON_UUID& source_component_id,
    const MYON_ComponentManifest& destination_manifest,
    int* destination_component_index
    ) const;

  bool GetAndValidateDestinationId(
    MYON_ModelComponent::Type component_type,
    const MYON_UUID& source_component_id,
    const MYON_ComponentManifest& destination_manifest,
    MYON_UUID* destination_component_id
    ) const;

  /*
  Returns:
    True if there are no MYON_ManifestMapItem elements.
  */
  bool IsEmpty() const;

  /*
  Returns:
    True if there is at least one MYON_ManifestMapItem element.
  */
  bool IsNotEmpty() const;

  /*
  Returns:
    Number of map items.
  Remarks:
    Some of these items may not change id or index.
  */
  unsigned int MapItemCount() const;

private:
  class MYON_ManifestMapImpl* Impl();
  class MYON_ManifestMapImpl* m_impl = nullptr;
};


enum class MYON_3dmArchiveTableType : unsigned int
{
  // The values of the table_type enums must increase in the order 
  // the corresponding tables appear in well formed 3dm archives
  // and the bitwise or of distinct values must be zero because
  // bitfield filters are used in some reading operations.

  Unset                     = 0,

  // First section in any 3dm archive.
  start_section             = 0x00000001U,

  properties_table          = 0x00000002U,
  settings_table            = 0x00000004U,
  bitmap_table              = 0x00000008U,
  texture_mapping_table     = 0x00000010U,
  material_table            = 0x00000020U,
  linetype_table            = 0x00000040U,
  layer_table               = 0x00000080U,
  group_table               = 0x00000100U,
  text_style_table          = 0x00000200U,
  leader_style_table        = 0x00000400U,
  dimension_style_table     = 0x00000800U,
  light_table               = 0x00001000U,
  hatchpattern_table        = 0x00002000U,
  instance_definition_table = 0x00004000U,
  object_table              = 0x00008000U, 
  historyrecord_table       = 0x00010000U,
  user_table                = 0x00020000U,

  // Last section in any 3dm archive.
  end_mark                  = 0x40000000U
};


/*
Description:
  Context for an annotation object. This context is required when
  converting current annotation objects to and from formats used
  in earlier versions and is typically used when reading and 
  writing 3dm archives.
*/
class MYON_CLASS MYON_3dmAnnotationContext
{
public:
  MYON_3dmAnnotationContext() = default;
  ~MYON_3dmAnnotationContext();
  MYON_3dmAnnotationContext(const MYON_3dmAnnotationContext&);
  MYON_3dmAnnotationContext& operator=(const MYON_3dmAnnotationContext&);

public:
  static const MYON_3dmAnnotationContext Default;

public:
  MYON::active_space ViewContext() const;
  
  void SetViewContext(
    MYON::active_space
  );

  MYON::LengthUnitSystem ModelLengthUnitSystem() const;
  
  void SetModelLengthUnitSystem(
    MYON::LengthUnitSystem model_length_unit_system
  );

  MYON::LengthUnitSystem PageLengthUnitSystem() const;
  
  void SetPageLengthUnitSystem(
    MYON::LengthUnitSystem page_length_unit_system
  );

  const class MYON_3dmAnnotationSettings& AnnotationSettings() const;

  /*
  Parameters:
    annotation_settings - [in]
      Annotation settings that are externally managed and will exist
      during the lifetime of the MYON_3dmAnnotationContext class instance.
  */
  void SetReferencedAnnotationSettings(
    const class MYON_3dmAnnotationSettings* annotation_settings
  );

  /*
  Parameters:
    annotation_settings - [in]
      A copy of annotation_settings is stored and manged by the MYON_3dmAnnotationContext class instance.
  */
  void SetManagedAnnotationSettings(
    const class MYON_3dmAnnotationSettings& annotation_settings
  );

  /*
  Returns:
    True if the annotation settings have been explicitly set.
  */
  bool AnnotationSettingsAreSet() const;

  /*
  This is the dimstyle the annotation object is question is using.
  It can be a "base" dimstyle from the dimstyle table or an
  "override" style attached used by a single instance of an annnotation
  object.
  */
  const class MYON_DimStyle& DimStyle() const;
  
  const class MYON_DimStyle& ParentDimStyle() const;

  /*
  Parameters:
    dim_style - [in]
      A dimension style that is externally managed and will exist
      during the lifetime of the MYON_3dmAnnotationContext class instance.
  */
  void SetReferencedDimStyle(
    const class MYON_DimStyle* parent_dim_style,
    const class MYON_DimStyle* override_dim_style,
    int V5_3dm_archive_index
  );

  /*
  Parameters:
    dim_style - [in]
      A copy of a dim_style is stored and manged by the MYON_3dmAnnotationContext class instance.
  */
  void SetManagedDimStyle(
    const class MYON_DimStyle& parent_dim_style,
    const class MYON_DimStyle* override_dim_style,
    int V5_3dm_archive_index
  );

  void UpdateReferencedDimStyle(
    const class MYON_DimStyle* old_pointer,
    const class MYON_DimStyle* new_pointer
  );

  /*
  Returns:
    True if the dimension style has been explicitly set.
  */
  bool DimStyleIsSet() const;
  
  /*
  Returns:
    If the dimstyle is not set or it has a nil parent id, then DimStyleId() is returned.
    Otherwise the parent id is returned.
  */
  MYON_UUID ParentDimStyleId() const;

  /*
  Returns:
    3dm archive dimension style table index to use when writing a V5 3dm archive.
    This is often different from DimStyle().Index().
  */
  int V5_ArchiveDimStyleIndex() const;

  /*
  Parameters:
    bRequireSetOverrides - [in]
      true if explicit overrides are required.
  Returns:
    true if the context dim style is an override style (parent id is not nil) and
    it has overrides or bRequireSetOverrides is false.
  */
  bool IsOverrideDimStyle() const;

  const class MYON_BinaryArchive* BinaryArchive() const;
  
  /*
  Parameters:
    binary_archive - [in]
      Binary archive that is externally managed and will exist
      during the lifetime of the MYON_3dmAnnotationContext class instance.
  */
  void SetReferencedBinaryArchive(
    const class MYON_BinaryArchive* binary_archive
  );
  
  /*
  Returns:
    True if the the target binary archive is set.
  */
  bool BinaryArchiveIsSet() const;
  
private:
  const class MYON_BinaryArchive* m_binary_archive = nullptr;

  // V6 table dimstyle. If an override dimstyle is in use,
  // this is the "parent dimstyle" referenced by the override.
  const class MYON_DimStyle* m_parent_dim_style = nullptr;
  class MYON_DimStyle* m_managed_parent_dim_style = nullptr;

  const class MYON_DimStyle* m_override_dim_style = nullptr;
  class MYON_DimStyle* m_managed_override_dim_style = nullptr;

  const class MYON_3dmAnnotationSettings* m_annotation_settings = nullptr;
  class MYON_3dmAnnotationSettings* m_managed_annotation_settings = nullptr;
  MYON::active_space  m_view_context = MYON::active_space::no_space;
  MYON::LengthUnitSystem m_model_length_unit_system = MYON::LengthUnitSystem::None;
  MYON::LengthUnitSystem m_page_length_unit_system = MYON::LengthUnitSystem::None;

  // V5 archive dim style index
  int m_V5_3dm_archive_dim_style_index = MYON_UNSET_INT_INDEX;

private:
  void Internal_CopyFrom(const MYON_3dmAnnotationContext& src);
  void Internal_Destroy();
};


class MYON_CLASS MYON_3dmArchiveTableStatus
{
public:
  MYON_3dmArchiveTableStatus() = default;
  ~MYON_3dmArchiveTableStatus() = default;
  MYON_3dmArchiveTableStatus(const MYON_3dmArchiveTableStatus&) = default;
  MYON_3dmArchiveTableStatus& operator=(const MYON_3dmArchiveTableStatus&) = default;

  static const MYON_3dmArchiveTableStatus Unset;

  MYON_3dmArchiveTableType m_table_type = MYON_3dmArchiveTableType::Unset;

  // number of table items
  unsigned int m_item_count = 0; 

  // Number of crc errors found during archive reading.
  // If > 0, then the archive is corrupt. See the table 
  // status information below to determine where the 
  // errors occurred.
  unsigned int m_crc_error_count = 0;

  // Number of other types of serious errors found during archive reading
  // or writing.
  // If > 0, then the archive is corrupt. See the table status information
  // below to determine where the errors occurred.
  unsigned int m_critical_error_count = 0;

  // Number of other types of serious errors found during archive reading.
  // If > 0, then the archive is corrupt. See the table status information
  // below to determine where the errors occurred.
  unsigned int m_recoverable_error_count = 0;
  
  enum class TableState : unsigned int 
  {
    Unset      = 0U,
    Started    = 1U, // began to read the table
    InProgress = 2U,
    Finished   = 3U, // finished reading the table
    NotFound   = 4U  // the table could not be located during reading
  };

  MYON_3dmArchiveTableStatus::TableState m_state = MYON_3dmArchiveTableStatus::TableState::Unset;
};

class MYON_CLASS MYON_BinaryArchive // use for generic serialization of binary data
{
public:
  MYON_BinaryArchive( MYON::archive_mode );
  virtual ~MYON_BinaryArchive();

protected:
  virtual
  MYON__UINT64 Internal_CurrentPositionOverride( // current offset (in bytes) into archive ( like ftell() )
    ) const = 0;

  virtual
  bool Internal_SeekFromCurrentPositionOverride( // seek from current position ( like fseek( ,SEEK_CUR) )
      int // byte offset ( >= -CurrentPostion() )
    ) = 0;

  virtual
  bool Internal_SeekToStartOverride(  // seek from current position ( like fseek(0 ,SEEK_SET) )
    ) = 0;

public:
  /*
  Returns:
    True if current position is at the end of the archive.
  */
  virtual
  bool AtEnd() const = 0;

public:
  /*
  Returns:
    Number of bytes from start of archive to the current position.
  */
  MYON__UINT64 CurrentPosition() const;

  /*
  Description:
    Set current position to bytes_from_start many bytes from the start of the archive.
  Parameters:
    bytes_from_start - [in]
  Returns:
    True: successful
    False: failure
  Remarks:
    Similar to fseek( ,SEEK_SET)
  */
  bool SeekFromStart(
      MYON__UINT64 bytes_from_start
    );
  
  /*
  Description:
    Increase the archive's current position by bytes_forward from the current position.
  Parameters:
    bytes_forward - [in]
  Returns:
    True: successful
    False: failure
  */
  bool SeekForward(
    MYON__UINT64 bytes_forward
  );

  /*
  Description:
    Decrease the archive's current position by bytes_backward from the current position.
  Parameters:
    bytes_backward - [in]
  Returns:
    True: successful
    False: failure
  */
  bool SeekBackward(
    MYON__UINT64 bytes_backward
  );

private:
  bool Internal_SeekCur(
    bool bFowrard,
    MYON__UINT64 offset
  );

public:

  /*
  Description:
    Tool for swapping bytes when doing I/O on
    using big endian CPUs.
  Remarks:
    3dm files are always saved with little endian byte order.
  See Also:
    MYON_BinaryArchive::Endian
  */
  static
  bool ToggleByteOrder(
    size_t, // number of elements
    size_t, // size of element (2,4, or 8)
    const void*,  // source buffer
    void*         // destination buffer (can be same a source buffer)
    );

  static
  const char* TypecodeName( unsigned int tcode );

  static
  char* MYON_TypecodeParse( unsigned int tcode, char* typecode_name, size_t max_length );
  
  /*
  Returns:
     Endian-ness of the cpu reading this file.
  Remarks:
    3dm files are always saved with little endian byte order.
  */
  MYON::endian Endian() const; // endian-ness of cpu

  bool ReadByte( size_t, void* ); // must fail if mode is not read or readwrite

  bool WriteByte( size_t, const void* ); // must fail if mode is not write or readwrite

  /*
  Description:
    Expert user function that uses Read() to load a buffer.
  Parameters:
    sizeof_buffer - [in] number of bytes to attempt to read.
    buffer - [out] read bytes are stored in this buffer
  Returns:
    Number of bytes actually read, which may be less than
    sizeof_buffer if the end of file is encountered.
  */
  MYON__UINT64 ReadBuffer( MYON__UINT64 sizeof_buffer, void* buffer );

  /*
  Description:
    Expert user function to control CRC calculation while reading and writing.
    Typically this is used when seeking around and reading/writing information
    in non-serial order.
  Parameters:
    bEnable  - [in]
  Returns:
    Current state of CRC calculation.  Use the returned value to restore the
    CRC calculation setting after you are finished doing your fancy pants
    expert IO.
  */
  bool EnableCRCCalculation( bool bEnable );

  // ReadCompressedBuffer()/WriteCompressedBuffer() use zlib 1.1.3
  // to inflate/deflate the data buffer.
  // Care must be used to get an endian independent file.  
  // See MYON_Mesh::Read()/MYON_Mesh::Write() for an example of an endian
  // independent use of compression. See also ToggleByteOrder() and Endian().
  //
  // To read data archived by WriteCompressedBuffer( sizeof_buffer, buffer )
  // do something like:
  //
  //   size_t sizeof_buffer = 0;
  //   ReadCompressedBufferSize(&sizeof_buffer);
  //   buffer = something with sizeof_buffer bytes.
  //   int bFailedCRC = false;
  //   bool ok = ReadCompressedBuffer( sizeof_buffer, buffer, &bFailedCRC );
  //


  /*
  Description:
    Red the size of a compressed buffer.
  Parameters:
    sizeof__outbuffer - [out] size of the uncompressed buffer in bytes
  Returns:
    True if read was successful.
  */
  bool ReadCompressedBufferSize( size_t* sizeof__outbuffer );

  /*
  Description:
    Read compressed information from an archive and uncompress it.
  Parameters:
    sizeof__outbuffer - [in] size of the uncompressed buffer in bytes
    outbuffer - [out] uncompressed buffer returned here
    bFailedCRC - [out] true if cyclic redundancy check fails
                      on uncompressed buffer

  Example:

          size_t sizeof_buffer = 0;
          ReadCompressedBufferSize(&sizeof_buffer);
          buffer = ...; // something with sizeof_buffer bytes.
          int bFailedCRC = false;
          bool ok = ReadCompressedBuffer( sizeof_buffer, buffer, &bFailedCRC );

  Returns:
    True if read was successful.  You need to check the value
    of bFailedCRC to see if the information that was read is valid.
  Remarks:
    Write your archive write/read code as if compression is always enabled.
    Do not vary what get written or read based on the value of UseBufferCompression().
  */
  bool ReadCompressedBuffer(
          size_t sizeof__outbuffer,
          void* outbuffer,
          bool* bFailedCRC
          );

  /*
  Description:
    Compress buffer and write the compressed information to the archive.
  Parameters:
    sizeof__inbuffer - [in] size of the uncompressed buffer in bytes
    inbuffer - [in] uncompressed buffer
  Returns:
    True if write was successful.
  Remarks:
    Write your archive write/read code as if compression is always enabled.
    Do not vary what get written or read based on the value of UseBufferCompression().
  */
  bool WriteCompressedBuffer(
    size_t sizeof__inbuffer,
    const void* inbuffer
    );

  bool ReadBool( bool* );

	bool ReadChar(    // Read an array of 8 bit chars
			size_t,       // number of chars to read
			char*    
			);  
	bool ReadChar(    // Read a single 8 bit char
			char*    
			);  

  bool ReadChar(    // Read an array of 8 bit signed chars
			size_t,       // number of chars to read
			MYON__INT8*    
			);  
	bool ReadChar(    // Read an array of 8 bit unsigned chars
			size_t,       // number of unsigned chars to read
			MYON__UINT8*    
			);  
	bool ReadChar(    // Read a single 8 bit signed char
			MYON__INT8*    
			);  
	bool ReadChar(    // Read a single 8 bit unsigned char
			MYON__UINT8*    
			);  

	bool ReadShort(   // Read an array of 16 bit shorts
			size_t,       // number of shorts to read
			MYON__INT16*    
			);  
	bool ReadShort(   // Read an array of 16 bit unsigned shorts
			size_t,       // number of shorts to read
			MYON__UINT16*    
			);  
	bool ReadShort(   // Read a single 16 bit short
			MYON__INT16*    
			);  
	bool ReadShort(   // Read a single 16 bit unsigned short
			MYON__UINT16*    
			);  

	bool ReadInt( // Read an array of 32 bit integers
			size_t,	      // number of ints to read
			MYON__INT32*      
			); 
	bool ReadInt( // Read an array of 32 bit integers
			size_t,	      // number of ints to read
			MYON__UINT32*      
			); 
	bool ReadInt( // Read a single 32 bit integer
			MYON__INT32*      
			); 
	bool ReadInt( // Read a single 32 bit unsigned integer
			MYON__UINT32*      
			); 

	bool ReadBigInt( // Read an array of 64 bit integers
			size_t,	      // number of ints to read
			MYON__INT64*      
			); 
	bool ReadBigInt( // Read an array of 64 bit integers
			size_t,	      // number of ints to read
			MYON__UINT64*      
			); 
	bool ReadBigInt( // Read a single 64 bit integer
			MYON__INT64*      
			); 
	bool ReadBigInt( // Read a single 64 bit unsigned integer
			MYON__UINT64*      
			); 


  MYON_DEPRECATED_MSG("Please use MYON_BinaryArchive::ReadInt")
	bool ReadLong( // Read an array of 32 bit integers
			size_t,	      // number of ints to read
			long*      
			); 
  MYON_DEPRECATED_MSG("Please use MYON_BinaryArchive::ReadInt")
	bool ReadLong( // Read an array of 32 bit integers
			size_t,	      // number of ints to read
			unsigned long*      
			); 
  MYON_DEPRECATED_MSG("Please use MYON_BinaryArchive::ReadInt")
	bool ReadLong( // Read a single 32 bit integer
			long*      
			); 
  MYON_DEPRECATED_MSG("Please use MYON_BinaryArchive::ReadInt")
	bool ReadLong( // Read a single 32 bit unsigned integer
			unsigned long*      
			); 

  MYON_DEPRECATED_MSG
  (
    "MYON_BinaryArchive::ReadSize is deprecated because it truncates to 32-bits.\n"
    " - If you are updating existing code use 'ReadInt' instead.\n"
    " - Else if you need to write a 64-bit size_t please call 'ReadBigSize'"
  )
  bool ReadSize( // Read a single 32 bit size_t
			size_t*
			); 

  bool ReadBigSize( size_t* ); // 64 bits
  
  bool ReadBigTime( time_t* ); // UCT seconds since 1 January 1970 (64 bits)


	bool ReadFloat(   // Read an array of floats
			size_t,       // number of floats
			float*
			);
	bool ReadFloat(   // Read a single float
			float*
			);
	bool ReadDouble(  // Read an array of IEEE doubles
			size_t,       // number of doubles
			double*
			);
	bool ReadDouble(  // Read a single double
			double*
			);

  bool ReadColor(
    MYON_Color&
    );

  bool ReadColor(
    MYON_4fColor&
  );

  bool ReadPoint (
    MYON_2dPoint&
    );
  bool ReadPoint (
    MYON_3dPoint&
    );
  bool ReadPoint (
    MYON_4dPoint&
    );
  bool ReadVector (
    MYON_2dVector&
    );
  bool ReadVector (
    MYON_3dVector&
    );

  bool ReadBoundingBox(MYON_BoundingBox&);

  bool ReadXform(MYON_Xform&);

  bool ReadPlaneEquation(MYON_PlaneEquation&);

  bool ReadPlane(MYON_Plane&);

  bool ReadLine(MYON_Line&);

  bool ReadArc(MYON_Arc&);

  bool ReadCircle(MYON_Circle&);

  bool ReadInterval( MYON_Interval& );

  bool ReadUuid( MYON_UUID& );

  bool ReadDisplayMaterialRef( MYON_DisplayMaterialRef& );

  bool ReadLinetypeSegment( MYON_LinetypeSegment& );

  // All times are stored in coordinated universal time
  // ( a.k.a GMT, UTC ).  Use ANSI C time() and gmtime() calls.
  bool ReadTime( struct tm& );

  /*
  Parameters:
    str_array_count - [out]
      Number of elements in the string array. All MYON_BinaryArchive string
      WriteString() functions write a null terminator to the file and
      the null terminator is included in the count. This means that
      if a string has a non-zero element, then str_array_count >= 2.
  Remarks:
    Modify your code to use ReadStringUTF8ElementCount() when reading
    UTF-8 encoded strings and ReadStringUTF16ElementCount()
    when reading UTF-16 encoded strings.
  */
  MYON_DEPRECATED_MSG("Use either ReadStringUTF8ElementCount() or ReadStringUTF16ElementCount()")
  bool ReadStringSize(
      size_t* str_array_count
      );

  /*
  Parameters:
    string_utf8_element_count - [out]
      Number of bytes in the string array. All MYON_BinaryArchive string
      WriteString() functions write a null terminator to the file and
      the null terminator is included in string_element_count. This means
      that if opennurbs wrote the string, either string_element_count = 0
      or string_element_count >= 2.
  */
  bool ReadStringUTF8ElementCount(
    size_t* string_utf8_element_count
    );

  /*
  Parameters:
    string_utf16_element_count - [out]
      Number of elements in the string array. All MYON_BinaryArchive string
      WriteString() functions write a null terminator to the file and
      the null terminator is included in string_element_count. This means
      that if opennurbs wrote the string, either string_element_count = 0
      or string_element_count >= 2.
  */
  bool ReadStringUTF16ElementCount(
    size_t* string_utf16_element_count
    );


  /*
  Parameters:
    str_array_count - [in]
      Number of char elements in str_array[], including the null
      terminator.  The value of str_array_count is returned by
      ReadCharStringElementCount().
    str_array - [in/out]
      Pass in an array with at least str_array_count elements.
      If true is returned and str_array_count > 0,
      then str_array[str_array_count-1] = 0. All strings with
      char elements written by Rhino are UTF-8 encoded
      unicode strings.
  */
  bool ReadString(
      size_t str_array_count,
      char* str_array
      );

  /*
  Parameters:
    str_array_count - [in]
      Number of unsigned char elements in str_array[], including
      the null terminator. The value of str_array_count is returned
      by ReadCharStringElementCount().
    str_array - [in/out]
      Pass in an array with at least str_array_count elements.
      If true is returned and str_array_count > 0,
      then str_array[str_array_count-1] = 0. All strings with
      unsigned char elements written by Rhino are UTF-8 encoded 
      unicode strings.
  */
  bool ReadString(
      size_t str_array_count,
      unsigned char* str_array
      );

  /*
  Parameters:
    str_array_count - [in]
      Number of unsigned short elements in str_array[],
      including the null terminator. The value of 
      str_array_count is returned by ReadWideCharStringElementCount().
    str_array - [in/out]
      Pass in an array with at least str_array_count elements.
      If true is returned and str_array_count > 0,
      then str_array[str_array_count-1] = 0. All strings with
      unsigned short elements written by Rhino are UTF-16 encoded
      unicode strings.
  */
  bool ReadString(
      size_t str_array_count,
      unsigned short*  str_array
      );

  bool ReadString( MYON_String& sUTF8 );

  bool ReadString( MYON_wString& s );

  bool ReadComponentIndex( MYON_COMPONENT_INDEX& );

  bool ReadArray( MYON_SimpleArray<bool>& );
  bool ReadArray( MYON_SimpleArray<char>&);

  bool ReadArray( MYON_SimpleArray<MYON__INT8>&);
  bool ReadArray( MYON_SimpleArray<MYON__INT16>&);
  bool ReadArray( MYON_SimpleArray<MYON__INT32>&);
  bool ReadArray( MYON_SimpleArray<MYON__UINT8>&);
  bool ReadArray( MYON_SimpleArray<MYON__UINT16>&);
  bool ReadArray( MYON_SimpleArray<MYON__UINT32>&);
  bool ReadArray( MYON_SimpleArray<float>& );
  bool ReadArray( MYON_SimpleArray<double>& );

  bool ReadArray( MYON_SimpleArray<MYON_Color>& );
  bool ReadArray( MYON_SimpleArray<MYON_2dPoint>& );
  bool ReadArray( MYON_SimpleArray<MYON_3dPoint>& );
  bool ReadArray( MYON_SimpleArray<MYON_4dPoint>& );
  bool ReadArray( MYON_SimpleArray<MYON_2dVector>& );
  bool ReadArray( MYON_SimpleArray<MYON_3dVector>& );
  bool ReadArray( MYON_SimpleArray<MYON_Xform>& );
  bool ReadArray( MYON_SimpleArray<MYON_2fPoint>& );
  bool ReadArray( MYON_SimpleArray<MYON_3fPoint>& );
  bool ReadArray( MYON_SimpleArray<MYON_4fPoint>& );
  bool ReadArray( MYON_SimpleArray<MYON_2fVector>& );
  bool ReadArray( MYON_SimpleArray<MYON_3fVector>& );
  bool ReadArray( MYON_SimpleArray<MYON_UUID>& );
  bool ReadArray( MYON_SimpleArray<MYON_UuidIndex>& );
  bool ReadArray( MYON_SimpleArray<MYON_UuidPtr>& );
  bool ReadArray( MYON_SimpleArray<MYON_SurfaceCurvature>& );
  bool ReadArray( MYON_ClassArray<MYON_String>& );
  bool ReadArray( MYON_ClassArray<MYON_wString>& );
  bool ReadArray( MYON_SimpleArray<MYON_DisplayMaterialRef>& );
  bool ReadArray( MYON_SimpleArray<MYON_LinetypeSegment>& );  
  bool ReadArray( MYON_SimpleArray<MYON_MappingChannel>& );
  bool ReadArray( MYON_ClassArray<MYON_MaterialRef>& );
  bool ReadArray( MYON_ClassArray<MYON_MappingRef>& );
  bool ReadArray( MYON_ClassArray<class MYON_ObjRef>& );
  bool ReadArray( MYON_SimpleArray<class MYON_ObjRef_IRefID>& );
  bool ReadArray( MYON_SimpleArray<class MYON_ClippingPlaneInfo>& );
  bool ReadArray( MYON_ObjectArray<class MYON_Layer>& );
  bool ReadArray( MYON_SimpleArray<class MYON_Layer*>& );

  bool WriteBool( bool );

#if defined(MYON_COMPILER_MSC) && defined(NDEBUG)
  // Work around Release build optimization bug in Visual Studio 2017.
  __declspec(noinline)
#endif
  bool WriteBoolTrue();

#if defined(MYON_COMPILER_MSC) && defined(NDEBUG)
  // Work around Release build optimization bug in Visual Studio 2017.
  __declspec(noinline)
#endif
  bool WriteBoolFalse();

  bool WriteChar(    // Write an array of 8 bit chars
			size_t,       // number of chars to write
			const char*    
			);  
	bool WriteChar(    // Write a single 8 bit char
			char
			);  

  bool WriteChar(    // Write an array of 8 bit signed chars
			size_t,       // number of chars to write
			const MYON__INT8*
			);  
	bool WriteChar(    // Write an array of 8 bit unsigned chars
			size_t,       // number of unsigned chars to write
			const MYON__UINT8*
			);  
	bool WriteChar(    // Write a single 8 bit signed char
			MYON__INT8
			);  
	bool WriteChar(    // Write a single 8 bit unsigned char
			MYON__UINT8
			);  

	bool WriteShort(   // Write an array of 16 bit shorts
			size_t,       // number of shorts to write
			const MYON__INT16*
			);  
	bool WriteShort(   // Write an array of 16 bit unsigned shorts
			size_t,       // number of shorts to write
			const MYON__UINT16*
			);  
	bool WriteShort(   // Write a single 16 bit short
			MYON__INT16
			);  
	bool WriteShort(   // Write a single 16 bit unsigned short
			MYON__UINT16
			);  

	bool WriteInt( // Write an array of 32 bit integers
			size_t,	      // number of ints to write
			const MYON__INT32*
			); 
	bool WriteInt( // Write an array of 32 bit integers
			size_t,	      // number of ints to write
			const MYON__UINT32*
			); 
	bool WriteInt( // Write a single 32 bit integer
			MYON__INT32    
			); 
	bool WriteInt( // Write a single 32 bit unsigned integer
			MYON__UINT32
			); 

	bool WriteBigInt( // Write an array of 64 bit integers
			size_t,	      // number of ints to write
			const MYON__INT64*      
			); 
	bool WriteBigInt( // Write an array of 64 bit integers
			size_t,	      // number of ints to write
			const MYON__UINT64*      
			); 
	bool WriteBigInt( // Write a single 64 bit integer
			MYON__INT64    
			); 
	bool WriteBigInt( // Write a single 64 bit unsigned integer
			MYON__UINT64
			); 

  MYON_DEPRECATED_MSG("Please use MYON_BinaryArchive::WriteInt")
	bool WriteLong( // Write an array of 32 bit integers
			size_t,	      // number of ints to write
			const long*      
			); 
  MYON_DEPRECATED_MSG("Please use MYON_BinaryArchive::WriteInt")
	bool WriteLong( // Write an array of 32 bit integers
			size_t,	      // number of ints to write
			const unsigned long*      
			); 
  MYON_DEPRECATED_MSG("Please use MYON_BinaryArchive::WriteInt")
	bool WriteLong( // Write a single 32 bit integer
			long    
			); 
  MYON_DEPRECATED_MSG("Please use MYON_BinaryArchive::WriteInt")
	bool WriteLong( // Write a single 32 bit unsigned integer
			unsigned long
			); 

  MYON_DEPRECATED_MSG
  (
    "MYON_BinaryArchive::WriteSize is deprecated because it truncates to 32-bits.\n"
    " - If you are updating existing code use 'WriteInt' instead.\n"
    " - Else if you need to write a 64-bit size_t please call 'WriteBigSize'"
  )
	bool WriteSize( // Write a single 32 bit size_t
			size_t
			); 

  bool WriteBigSize( size_t ); // 64 bits 
  
  bool WriteBigTime( time_t ); // UCT seconds since 1 January 1970 (64 bits)

	bool WriteFloat(   // Write a number of IEEE floats
			size_t,       // number of doubles
			const float*
			);
	bool WriteFloat(   // Write a single float
			float
			);
	bool WriteDouble(  // Write a single double
      size_t,
			const double*
			);
	bool WriteDouble(  // Write a single double
			double
			);

  bool WriteColor (
    const MYON_Color&
    );

  bool WriteColor(
    const MYON_4fColor&
  );

  bool WritePoint (
    const MYON_2dPoint&
    );
  bool WritePoint (
    const MYON_3dPoint&
    );
  bool WritePoint (
    const MYON_4dPoint&
    );
  bool WriteVector (
    const MYON_2dVector&
    );
  bool WriteVector (
    const MYON_3dVector&
    );

  bool WriteBoundingBox(const MYON_BoundingBox&);

  bool WriteXform(const MYON_Xform&);

  bool WritePlaneEquation(const MYON_PlaneEquation&);

  bool WritePlane(const MYON_Plane&);

  bool WriteLine(const MYON_Line&);

  bool WriteArc(const MYON_Arc&);

  bool WriteCircle(const MYON_Circle&);

  bool WriteInterval( const MYON_Interval& );

  bool WriteUuid( const MYON_UUID& );

  bool WriteDisplayMaterialRef( const MYON_DisplayMaterialRef& );

  bool WriteLinetypeSegment( const MYON_LinetypeSegment& );

  // All times are stored in universal coordinated time
  // ( a.k.a GMT, UCT ).  Use ANSI C time() and gmtime() calls.
  bool WriteTime( const struct tm& );

  /*
  Parameters:
    sUTF8 - [in]
      A null terminated UTF-8 encoded unicode string.
  Remarks:
    To read a string written with WriteString(const char*),
    call ReadStringUTF8ElementCount(&string_utf8_element_count)
    to get the number of char elements written in the file,
    obtain a buffer with at least string_utf8_element_count
    char elements and then call 
    ReadString(string_utf8_element_count,buffer) to read the
    char elements.

    If 0 == sUTF8 or 0 == SUTF8[0], a 4 byte int with
    value = 0 is written, otherwise a 4 byte int with
    value = strlen + 1 is written, followed by the string,
    followed by the null terminator.
  */
  bool WriteString(
      const char* sUTF8         
      );

  /*
  Parameters:
    sUTF8 - [in]
      A null terminated UTF-8 encoded unicode string.
  Remarks:
    To read a string written with WriteString(const unsigned char*),
    call ReadStringUTF8ElementCount(&string_utf8_element_count) to
    get the number of unsigned char elements written in the file,
    obtain a buffer with at least string_utf8_element_count
    unsigned char elements and then call 
    ReadString(string_utf8_element_count,buffer) to read the 
    unsigned charelements.

    If 0 == sUTF8 or 0 == SUTF8[0], a 4 byte int with
    value = 0 is written, otherwise a 4 byte int with
    value = strlen + 1 is written, followed by the string,
    followed by the null terminator.
  */
  bool WriteString(
      const unsigned char* sUTF8
      );

  /*
  Parameters:
    sUTF16 - [in]
      A null terminated UTF-16 encoded unicode string.
  Remarks:
    To read a string written with WriteString(const unsigned short*),
    call ReadStringUTF16ElementCount(&string_utf16_element_count) to
    get the number of unsigned short elements written in the file,
    obtain a buffer with at least string_utf16_element_count
    unsigned short elements and then call 
    ReadString(string_utf16_element_count,buffer) to read the
    unsigned short elements.

    If 0 == sUTF8 or 0 == SUTF8[0], a 4 byte int with
    value = 0 is written, otherwise a 4 byte int with
    value = strlen + 1 is written, followed by the string,
    followed by the null terminator.
  */
  bool WriteUTF16String(
      const unsigned short* sUTF16
      );

  /*
  Description:
    Write a wide string as a UTF-8 encoded string.
  */
  bool WriteWideString(
    const wchar_t* sWideChar,
    int sWideChar_count
    );
  
  /*
  Description:
    Write a wide string as a UTF-8 encoded string.
  */
  bool WriteWideString(
    const MYON_wString& wide_string
    );
  
  /*
  Description:
    Read a wide string written with the WriteWideString() function.
  */
  bool ReadWideString(
    MYON_wString& wide_string
    );
  
  bool WriteString( const MYON_String& sUTF8 );

  bool WriteString( const MYON_wString& s);

  bool WriteComponentIndex( const MYON_COMPONENT_INDEX& );

  bool WriteArray( const MYON_SimpleArray<bool>& );
  bool WriteArray( const MYON_SimpleArray<char>& );
  
  bool WriteArray( const MYON_SimpleArray<MYON__INT8>& );
  bool WriteArray( const MYON_SimpleArray<MYON__INT16>& );
  bool WriteArray( const MYON_SimpleArray<MYON__INT32>& );
  bool WriteArray(const MYON_SimpleArray<MYON__UINT8>&);
  bool WriteArray(const MYON_SimpleArray<MYON__UINT16>&);
  bool WriteArray(const MYON_SimpleArray<MYON__UINT32>&);

  bool WriteArray( const MYON_SimpleArray<float>& );
  bool WriteArray( const MYON_SimpleArray<double>& );

  bool WriteArray( const MYON_SimpleArray<MYON_Color>& );

  bool WriteArray( const MYON_SimpleArray<MYON_2dPoint>& );
  bool WriteArray( const MYON_SimpleArray<MYON_3dPoint>& );
  bool WriteArray( const MYON_SimpleArray<MYON_4dPoint>& );
  bool WriteArray( const MYON_SimpleArray<MYON_2dVector>& );
  bool WriteArray( const MYON_SimpleArray<MYON_3dVector>& );

  bool WriteArray( const MYON_SimpleArray<MYON_2fPoint>& );
  bool WriteArray( const MYON_SimpleArray<MYON_3fPoint>& );
  bool WriteArray( const MYON_SimpleArray<MYON_4fPoint>& );
  bool WriteArray( const MYON_SimpleArray<MYON_2fVector>& );
  bool WriteArray( const MYON_SimpleArray<MYON_3fVector>& );
  bool WriteArray( const MYON_SimpleArray<MYON_Xform>& );
  bool WriteArray( const MYON_SimpleArray<MYON_UUID>& );
  bool WriteArray( const MYON_SimpleArray<MYON_UuidIndex>& );
  bool WriteArray( const MYON_SimpleArray<MYON_UuidPtr>& );
  bool WriteArray( const MYON_SimpleArray<MYON_SurfaceCurvature>& );
  bool WriteArray( const MYON_ClassArray<MYON_String>& );
  bool WriteArray( const MYON_ClassArray<MYON_wString>& );
  bool WriteArray( const MYON_SimpleArray<MYON_DisplayMaterialRef>& );
  bool WriteArray( const MYON_SimpleArray<MYON_LinetypeSegment>& );  
  bool WriteArray( const MYON_SimpleArray<MYON_MappingChannel>& );
  bool WriteArray( const MYON_ClassArray<MYON_MaterialRef>& );
  bool WriteArray( const MYON_ClassArray<MYON_MappingRef>& );
  bool WriteArray( const MYON_ClassArray<class MYON_ObjRef>& );
  bool WriteArray( const MYON_SimpleArray<class MYON_ObjRef_IRefID>& );
  bool WriteArray( const MYON_SimpleArray<class MYON_ClippingPlaneInfo>& );
  bool WriteArray( int count, const class MYON_Layer* );
  bool WriteArray( int count, const class MYON_Layer*const* );

  /////////////////////////////////////////////////////
  //
  // Read/Write classes derived from MYON_Object
  //

  /*
  Description:
    Reads and object from a 3dm archive;
  Parameters:
    ppObject - [out]  object is allocated and a pointer to the
                      allocated object is returned as *ppObject;
  Returns:
    0: failure - unable to read object because of file IO problems
    1: success
    3: unable to read object because it's UUID is not registered
       this could happen in cases where old code is attempting to read
       new objects.
  */
  int ReadObject( 
         MYON_Object** ppObject
         );


  /*
  Description:
    Reads and object from a 3dm archive.
  Parameters:
    object - [in] The value of object.MYON_ClassId()->Uuid() must
                  exactly match the class uuid in of the next
                  object in the archive.
  Returns:
    0: failure - unable to read object because of file IO problems.
    1: success
    2: unable to read object because the class id in the archive
       did not match pObject->ClassId.
  */
  int ReadObject( 
         MYON_Object& object
         );

  bool WriteObject( const MYON_Object* ); // writes object definition
  bool WriteObject( const MYON_Object& ); // writes object definition

private:
  bool Internal_WriteObject(
    const MYON_Object& model_object
  );
  bool Internal_WriteV5AnnotationObject(
    const class MYON_Annotation& V6_annotation,
    const class MYON_3dmAnnotationContext* annotation_context
  );
  bool Internal_WriteV2AnnotationObject(
    const class MYON_OBSOLETE_V5_Annotation& V5_annotation,
    const class MYON_3dmAnnotationContext* annotation_context
  );
public:


  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  //
  // 3DM Interface - ignore if not reading/writing a 3DM file
  //                 this is here so that the infrastructure
  //                 for writing 3dm archives is available for
  //                 any type of serialization device.
  //

  /*
  Description:
    Specify which types of objects (MYON_Brep, MYON_Extrusion, MYON_SubD, ...)
    save render meshes in the 3dm file.

  Parameters:
    object_type_flags - [in]
      The bits in object_type_flags correspond to MYON::object_type values
      and identify the object types the setting will be applied to.

  Remarks:
    Saving render meshes increases file size, sometimes dramatically.

    Creating MYON_Brep analysis meshes is often slow. 
    Disable saving MYON_Brep analysis meshes when IO speed or file size is 
    a critical issue, the time expense of recreating the MYON_Brep meshes
    when the file is read is acceptable, and the file will be read by Rhino. 
    Enable when the file size is not an issue or the file will be used by other 
    applications that may not be able to create meshes.

    Creating MYON_Extrusion meshes is fast. Disable when IO speed or file size
    is an issue and the file will be read by Rhino.  Enable when the file
    will be used by other applications that may not be able to create meshes.

    Creating MYON_SubD meshes is fast. Disable when IO speed or file size
    is an issue and the file will be read by Rhino.  Enable when the file
    will be used by other applications that may not be able to create meshes.
  */
  void EnableSave3dmRenderMeshes( 
    unsigned int object_type_flags,
    bool bSave3dmRenderMeshes 
    );

  /*
  Description:
    Specify which types of objects (MYON_Brep, MYON_Extrusion, MYON_SubD, ...)
    save render meshes in the 3dm file.
  Returns:
    The bits in the return value correspond to MYON::object_type values
    and identify the object types save analysis meshes in the 3dm file.
  */  
  unsigned int Save3dmRenderMeshObjectTypeFlags() const;

  /*
  Parameters:
    object_type - [in]
  Returns:
    true if render meshes for the specified object type will be 
    saved in the .3dm file.
  */  
  bool Save3dmRenderMesh(
    MYON::object_type object_type
    ) const;

  /*
  Description:
    Specify which types of objects (MYON_Brep, MYON_Extrusion, MYON_SubD, ...)
    save analysis meshes in the 3dm file.
  
  Parameters:
    object_type_flags - [in]
      The bits in object_type_flags correspond to MYON::object_type values
      and identify the object types the setting will be applied to.

  Remarks:
    Saving analysis meshes increases file size, sometimes dramatically.

    Creating MYON_Brep analysis meshes is often slow. 
    Disable saving MYON_Brep analysis meshes when IO speed or file size is 
    a critical issue, the time expense of recreating the MYON_Brep meshes
    when the file is read is acceptable, and the file will be read by Rhino. 
    Enable when the file size is not an issue or the file will be used by other 
    applications that may not be able to create meshes.

    Creating MYON_Extrusion meshes is fast. Disable when IO speed or file size
    is an issue and the file will be read by Rhino.  Enable when the file
    will be used by other applications that may not be able to create meshes.

    Creating MYON_SubD meshes is fast. Disable when IO speed or file size
    is an issue and the file will be read by Rhino.  Enable when the file
    will be used by other applications that may not be able to create meshes.
  */
  void EnableSave3dmAnalysisMeshes(
    unsigned int object_type_flags,
    bool bSave3dmAnalysisMeshes 
    );
  
  void SetSave3dmPreviewImage(
    bool bSave3dmPreviewImage
  );
  
  /*
  Returns:
    true: (default)
      If a preview image is included in the MYON_3dmProperties information, it will be saved.
    false: 
      A preview imae, if it exists, will not be saved in the 3dm archive.
      This reduces archive size. 
      When Save3dmPreviewImage() is false, generating a preview image can be skipped.
  */
  bool Save3dmPreviewImage() const;

  /*
  Description:
    Control when some information, like preview images and mesh information, is
    compressed when writing 3dm archives. The default is true. 
    In special situations when the storage media is extremely fast and large file size
    is not a concern, disabling buffer compression can reduce file write time.
  Parameters:
    bUseBufferCompression - [in]
  Remarks:
    The default is true.
  */
  void SetUseBufferCompression(
    bool bUseBufferCompression
  );

  /*
  Returns:
    true: (default)
      Some information, including preview images and mesh information is compressed when
      writing 3dm archives. This reduces, sometimes dramatically, the size
      of the 3dm archive.
    false: 
      No compression is performed. This increases, sometimes dramatically, the size
      of the 3dm archive.
      In special situations when the storage media is extremely fast and large file size
      is not a concern, disabling buffer compression can reduce file write time.
  */
  bool UseBufferCompression() const;


  /*
  Description:
    Specify which types of objects (MYON_Brep, MYON_Extrusion, MYON_SubD, ...)
    save analysis meshes in the 3dm file.
  Returns:
    The bits in the return value correspond to MYON::object_type values
    and identify the object types save analysis meshes in the 3dm file.
  */  
  unsigned int Save3dmAnalysisMeshObjectTypeFlags() const;

  /*
  Parameters:
    object_type - [in]
  Returns:
    true if analysis meshes for the specified object type will be 
    saved in the .3dm file.
  */  
  bool Save3dmAnalysisMesh(
    MYON::object_type object_type
    ) const;


  /*
  Returns:
    True if all user data and user tables should be read or written.
    False if some or no user data or user tables should be read or written.
  Remarks:
    AllUserDataSerializationIsEnabled() = (false == ShouldSerializeNoUserData() && false == ShouldSerializeSomeUserData())
  */
  bool ShouldSerializeAllUserData() const;

  /*
  Returns:
    True if no user data and user tables should be read or written.
    False if some or all user data or user tables should be read or written.
  Remarks:
    SerializeNoUserData() = (false == ShouldSerializeAllUserData() && false == ShouldSerializeSomeUserData())
  */
  bool ShouldSerializeNoUserData() const;

  /*
  Returns:
    True if some but not all user data or user tables should be
    read or written.
    False if all user data or no user data should be read or written.
  Remarks:
    SerializeSomeUserData() = (false == ShouldSerializeAllUserData() && false == ShouldSerializeNoUserData())

    Use ShouldSerializeUserDataItem(application_id,item_id) to
    determine if a specific object user data or user table should 
    be read or written.    
  */
  bool ShouldSerializeSomeUserData() const;

  /*
  Description:
    Determine if an application's (plug-in's) object user data 
    or user table should be read or written.
  Parameters:
    application_id - [in]
      The application id (often a plug-in id) for the object user data
      or user table.
    item_id - [in]
      item_id identifies which user data items should be read or written.
      - To determine if a specific type of object user data should
        be read or written, pass the value of MYON_UserData.m_userdata_uuid.
      - To determine if the user table for the application should
        be read or written, pass application_id.
      - To determine if all object user data and the user table
        for the application should be read or written, pass nil.
  Returns:
    True if the identified user data or user table should be read or written.
  */
  bool ShouldSerializeUserDataItem(
    MYON_UUID application_id,
    MYON_UUID item_id
    ) const;

  /*
  Description:
    Specify the serialization option for object user data and user tables 
    that are not explicitly set by SetShouldSerializeUserDataItem().
  Parameters:
    bSerialize - [in]
  Remarks:
    If no setting is specified, all user data is read and written.
  */
  bool SetShouldSerializeUserDataDefault(
    bool bSerialize
    );

  bool ShouldSerializeUserDataDefault() const;


  /*
  Description:
    Specify if an application's (plug-in's) object user data
    or user table should be read or written.
  Parameters:
    application_id - [in]
      The application id (often a plug-in id) for the object user data
      or user table.
    item_id - [in]
      item_id identifies which user data items should be read or written.
      - To determine if a specific type of object user data should
        be read or written, pass the value of MYON_UserData.m_userdata_uuid.
      - To determine if the user table for the application should
        be read or written, pass application_id.
      - To determine if all object user data and the user table
        for the application should be read or written, pass nil.
    bSerializeUserDataItem - [in]
      True to enable reading and writing of the specified item. 
      False to disable reading and writing of the specified item. 
  Returns:
    True if the input was valid and the setting was applied. 
    This function will not apply any settings after reading 
    or writing begins.
  */
  bool SetShouldSerializeUserDataItem(
    MYON_UUID application_id,
    MYON_UUID item_id,
    bool bSerializeUserDataItem
    );

  /*
  Description:
    Determine if an object has user data that should be written.
  Parameters:
    object - [in]
  Returns:
    True if object has user data that should be written.
  */
  bool ObjectHasUserDataToWrite(
    const class MYON_Object* object
    ) const;

  bool ShouldWriteUserDataItem(
    const class MYON_Object* object,
    const class MYON_UserData* object_user_data
    ) const;

  /*
  Remarks:
    In a stable commercially released Rhino version N, CurrentArchiveVersion() = 10*N.
    In "early" Rhino N WIP, CurrentArchiveVersion() = 10*(N-1).
    In "later" Rhino N WIP, CurrentArchiveVersion() = 10*N.
  Returns:
    The current 3dm archive version that is saved by Rhino.
  */
  static int CurrentArchiveVersion();

  /*
  Description:
    As time passes, more tables have been added to 3dm archives.
  Parameters:
    table - [in]
  Returns:
    True if this archive has the specified table
  */
  bool ArchiveContains3dmTable(
    MYON_3dmArchiveTableType table
    ) const;

  /*
  Parameters:
    archive_3dm_version - [in]
      1,2,3,4,5,50,60,70,...
    opennurbs_library_version - [in]
      a number > 100000000
  */
  static bool ArchiveContains3dmTable(
    MYON_3dmArchiveTableType table,
    unsigned int archive_3dm_version,
    unsigned int opennurbs_library_version
    );

  bool WriteModelComponentName(
    const MYON_ModelComponent& model_component
  );

  ///////////////////////////////////////////////////////////////////
  // Step 1: REQUIRED - Write/Read Start Section
  //

  
  /*
  Description:
    In rare cases, experts testing handling of corrupt 3dm files need to
    write a 3dm archive that is corrupt. In this rare testing situation,
    those experts should call IntentionallyWriteCorrupt3dmStartSectionForExpertTesting()
    exactly one time before they begin writing the file. The 32 byte identifier will
    replace the 1st 3 spaces with a capital X to mimic a file that became corrupt
    while residing on storage media.
  */
  void IntentionallyWriteCorrupt3dmStartSectionForExpertTesting();

  /*
  Parameters:
    version - [in]
       0, 2, 3, 4, 5, 50 or 60 (5 is treated as 50)
       
       If version is 0, then the value of MYON_BinaryArchive::CurrentArchiveVersion()
       is used.

       Use either 0 or the value of MYON_BinaryArchive::CurrentArchiveVersion()
       for the version parameter when you want your code to write the most 
       up to date file version. 

    sStartSectionComment - [in]
      nullptr or a UTF-8 encoded string with application name, et cetera.
      This information is primarily used when debugging files
      that contain problems.  McNeel and Associates stores
      application name, application version, compile date, 
      and the OS in use when file was written.
  */
  bool Write3dmStartSection( 
    int version,
    const char* sStartSectionComment
    );

  /*
  Parameters:
    version - [out]
       .3dm file version (2, 3, 4, 5, 50, 60)
    sStartSectionComment - [out]
      UTF-8 encoded string passed to Write3dmStartSection()
    destination_manifest - [in]
      manifest of the destination model
  */
  bool Read3dmStartSection( 
    int* version,
    MYON_String& sStartSectionComment
    );

  /*
  Returns:
    A copy of the start section comment written to or read from the archive.
    If this function is called before Write3dmStartSection() or Read3dmStartSection(),
    it returns MYON_String:EmptyString;
  */
  const MYON_String& Archive3dmStartSectionComment() const;

  ///////////////////////////////////////////////////////////////////
  // Step 2: REQUIRED - Write/Read properties table
  //
  bool Write3dmProperties(
        const class MYON_3dmProperties&
        );
  bool Read3dmProperties(
        class MYON_3dmProperties&
        );

  /*
  Returns:
    A copy of the MYON_3dmProperties information written to or read from the archive.
    If this function is called before Write3dmProperties() or Read3dmProperties(),
    it returns MYON_3dmProperties:Empty;
  */
  const class MYON_3dmProperties& Archive3dmProperties() const;

  ///////////////////////////////////////////////////////////////////
  // Step 3: REQUIRED - Write/Read settings table
  //
  bool Write3dmSettings(
        const class MYON_3dmSettings&
        );
  bool Read3dmSettings(
        class MYON_3dmSettings&
        );
  
  /*
  Returns:
    A copy of the MYON_3dmSettings information written to or read from the archive.
    If this function is called before Write3dmSettings() or Read3dmSettings(),
    it returns MYON_3dmSettings:Default;
  */
  const class MYON_3dmSettings& Archive3dmSettings() const;

  ///////////////////////////////////////////////////////////////////
  // Step 4: REQUIRED - Write/Read bitmap table (it can be empty)
  //
  bool BeginWrite3dmBitmapTable();
  bool Write3dmImageComponent( 
    const class MYON_ModelComponentReference& model_component_reference
    );
  bool Write3dmImageComponent( 
    const class MYON_ModelComponent* model_component 
    );
  bool Write3dmBitmap( const class MYON_Bitmap& );
  bool EndWrite3dmBitmapTable();

  bool BeginRead3dmBitmapTable();
  int  Read3dmBitmap(   // returns 0 at end of bitmap table
                        //         1 bitmap successfully read
            class MYON_Bitmap** // bitmap returned here
            );
  bool EndRead3dmBitmapTable();

  ///////////////////////////////////////////////////////////////////
  // Step 5: REQUIRED - Write/Read texture mapping table (it can be empty)
  //
  bool BeginWrite3dmTextureMappingTable();
  bool Write3dmTextureMappingComponent( 
    const class MYON_ModelComponentReference& model_component_reference
    );
  bool Write3dmTextureMappingComponent( 
    const class MYON_ModelComponent* model_component 
    );
  bool Write3dmTextureMapping( const class MYON_TextureMapping& );
  bool EndWrite3dmTextureMappingTable();

  bool BeginRead3dmTextureMappingTable();
  int  Read3dmTextureMapping( // returns 0 at end of table
            class MYON_TextureMapping** // testuremapping returned here
            );
  bool EndRead3dmTextureMappingTable();

  ///////////////////////////////////////////////////////////////////
  // Step 6: REQUIRED - Write/Read render material table (it can be empty)
  //
  bool BeginWrite3dmMaterialTable();
  bool Write3dmMaterialComponent( 
    const class MYON_ModelComponentReference& model_component_reference
    );
  bool Write3dmMaterialComponent( 
    const class MYON_ModelComponent* model_component 
    );
  bool Write3dmMaterial( const class MYON_Material& );
  bool EndWrite3dmMaterialTable();

  bool BeginRead3dmMaterialTable();
  int  Read3dmMaterial( // returns 0 at end of table
            class MYON_Material** // material returned here
            );
  bool EndRead3dmMaterialTable();

  ///////////////////////////////////////////////////////////////////
  // Step 7: REQUIRED - Write/Read linetype table (it can be empty)
  //
  bool BeginWrite3dmLinetypeTable();
  bool Write3dmLinePatternComponent( 
    const class MYON_ModelComponentReference& model_component_reference
    );
  bool Write3dmLinePatternComponent( 
    const class MYON_ModelComponent* model_component 
    );
  bool Write3dmLinetype( 
    const class MYON_Linetype& line_pattern
    );
  bool EndWrite3dmLinetypeTable();

  bool BeginRead3dmLinetypeTable();
  int  Read3dmLinetype(
  class MYON_Linetype**
    );
  bool EndRead3dmLinetypeTable();

  ///////////////////////////////////////////////////////////////////
  // Step 8: REQUIRED - Write/Read layer table (it can be empty)
  //
  bool BeginWrite3dmLayerTable();
  bool Write3dmLayerComponent( 
    const class MYON_ModelComponentReference& model_component_reference
    );
  bool Write3dmLayerComponent( 
    const class MYON_ModelComponent* model_component 
    );
  bool Write3dmLayer( const MYON_Layer& );
  bool EndWrite3dmLayerTable();

  bool BeginRead3dmLayerTable();
  int  Read3dmLayer( // returns 0 at end of table
            MYON_Layer** // layer returned here
            );
  bool EndRead3dmLayerTable();

  ///////////////////////////////////////////////////////////////////
  // Step 9: REQUIRED - Write/Read group table (it can be empty)
  //
  bool BeginWrite3dmGroupTable();
  bool Write3dmGroupComponent( 
    const class MYON_ModelComponentReference& model_component_reference
    );
  bool Write3dmGroupComponent( 
    const class MYON_ModelComponent* model_component 
    );
  bool Write3dmGroup( const class MYON_Group& );
  bool EndWrite3dmGroupTable();

  bool BeginRead3dmGroupTable();

  // Description:
  //   Reads groups from group table.  If the group definition is
  //   read, a group is created by calling new MYON_Group(),
  //   initialized with values stored in the archive, and 
  //   returned.
  //
  // Parameters:
  //   ppGroup - If the group definition is
  //   read, a group is created by calling new MYON_Group(),
  //   initialized with values stored in the archive, and 
  //   a pointer to the new group is returned in *ppGroup.
  //
  // Returns:
  //
  //   @untitled table
  //   0     at the end of the group table
  //   1     group definition was successfully read
  //   -1    archive is corrupt at this point
  //
  // Example:
  //   Calls to Read3dmGroup need to be bracketed by calls
  //   to BeginRead3dmGroupTable() / EndRead3dmGroupTable().
  //
  //           archive.BeginRead3dmGroupTable();
  //           MYON_Group* pGroup;
  //           int rc = 1;
  //           while(rc==1)
  //           { //
  //             pGroup = 0;
  //             archive.Read3dmGroup(&pGroup);
  //             if ( pGroup )
  //               do something with pGroup
  //           } //
  //           archive.EndRead3dmGroupTable();
  //      
  int  Read3dmGroup(
            class MYON_Group** // ppGroup
            );

  bool EndRead3dmGroupTable();


  ///////////////////////////////////////////////////////////////////////
  ////// Step 10: REQUIRED - Write/Read text_style table (it can be empty)
  //////
  ////MYON_DEPRECATED_MSG("remove call. Text style information is now part of MYON_DimStyle.")
  ////bool BeginWrite3dmTextStyleTable();
  ////////bool Write3dmTextStyleComponent( 
  ////////  const class MYON_ModelComponentReference& model_component_reference
  ////////  );
  ////////bool Write3dmTextStyleComponent( 
  ////////  const class MYON_ModelComponent* model_component 
  ////////  );
  ////MYON_DEPRECATED_MSG("remove call. Text style information is now part of MYON_DimStyle.")
  ////bool Write3dmTextStyle( 
  ////  const class MYON_TextStyle& 
  ////  );
  ////MYON_DEPRECATED_MSG("remove call. Text style information is now part of MYON_DimStyle.")
  ////bool EndWrite3dmTextStyleTable();

private:
  ////bool Internal_BeginWrite3dmTextStyleTable();
  bool Internal_Write3dmTextStyle( 
    const class MYON_TextStyle& 
    );
  /////bool Internal_EndWrite3dmTextStyleTable();

public:

  //////MYON_DEPRECATED_MSG("remove call. Text style information is now part of MYON_DimStyle.")
  //////bool BeginRead3dmTextStyleTable();

  //////MYON_DEPRECATED_MSG("remove call. Text style information is now part of MYON_DimStyle.")
  //////int Read3dmTextStyle(
  //////          class MYON_TextStyle** // ppTextStyle
  //////          );

  //////MYON_DEPRECATED_MSG("remove call. Text style information is now part of MYON_DimStyle.")
  //////bool EndRead3dmTextStyleTable();

private:
  int Internal_Read3dmTextStyle(
    class MYON_TextStyle** // ppTextStyle
  );
public:

  ///////////////////////////////////////////////////////////////////
  // Step 11: REQUIRED - Write/Read dimstyle table (it can be empty)
  //
  bool BeginWrite3dmDimStyleTable();
  
  bool Write3dmDimStyleComponent( 
    const class MYON_ModelComponentReference& model_component_reference
    );
  bool Write3dmDimStyleComponent( 
    const class MYON_ModelComponent* model_component 
    );
  bool Write3dmDimStyle( const class MYON_DimStyle& );

  bool EndWrite3dmDimStyleTable();

private:
  bool Internal_Write3dmDimStyle( const class MYON_DimStyle&, bool bUpdateManifest );
public:

  bool BeginRead3dmDimStyleTable();

  // Description:
  //   Reads annotation dimension styles from dimension style table.
  //   If the dimension style definition is read, 
  //   a dimension style is created by calling new MYON_DimStyle(),
  //   initialized with values stored in the archive, and 
  //   returned.
  //
  // Parameters:
  //   ppDimStyle - If the dimstyle definition is
  //   read, a dimstyle is created by calling new MYON_DimStyle(),
  //   initialized with values stored in the archive, and 
  //   a pointer to the new dimstyle is returned in *ppDimStyle.
  //
  // Returns:
  //
  //   @untitled table
  //   0     at the end of the dimension style table
  //   1     dimension style definition was successfully read
  //   -1    archive is corrupt at this point
  //
  // Example:
  //   Calls to Read3dmDimStyle need to be bracketed by calls
  //   to BeginRead3dmDimStyleTable() / EndRead3dmDimStyleTable().
  //
  //           archive.BeginRead3dmDimStyleTable();
  //           int rc = 1;
  //           MYON_DimStyle* pDimStyle;
  //           while(rc==1)
  //           { //
  //             pDimStyle = 0;
  //             archive.Read3dmDimStyle(&pDimStyle);
  //             if ( pDimStyle )
  //               do something with pDimStyle
  //           } //
  //           archive.EndRead3dmDimStyleTable();
  //      
  int Read3dmDimStyle(
    class MYON_DimStyle** ppDimStyle
    );

private:
  int Internal_Read3dmDimStyle(
    class MYON_DimStyle** ppDimStyle
    );

  void Internal_ConvertTextStylesToDimStyles();

  double Internal_ArchiveModelSpaceTextScale() const;

  const MYON_DimStyle* Internal_ArchiveCurrentDimStyle();

public:
  bool EndRead3dmDimStyleTable();

  /*
    Internal_Read3dmDimStyleOverrides() is a public function on MYON_BinaryArchive because 
    it must be called from MYON_Annotation::Internal_ReadAnnotation().
    There is no other reason to call this function.
  */
public:
  bool Internal_Read3dmDimStyleOverrides(
    class MYON_Annotation& annotation,
    bool bFromDimStyleTable
  );

  /*
    Internal_Write3dmDimStyleOverrides() is a public function on MYON_BinaryArchive because 
    it must be called from MYON_Annotation::Internal_WriteAnnotation().
    There is no other reason to call this function.
  */
public:
  bool Internal_Write3dmDimStyleOverrides(
    const class MYON_Annotation& annotation,
    const class MYON_DimStyle* dim_style_overrides
  );

public:
  ///////////////////////////////////////////////////////////////////
  // Step 12: REQUIRED - Write/Read render light table (it can be empty)
  //
  bool BeginWrite3dmLightTable();
  bool Write3dmModelLightComponent( 
    const class MYON_ModelComponentReference& model_component_reference
    );
  bool Write3dmModelLightComponent( 
    const class MYON_ModelGeometryComponent* model_light
    );
  bool Write3dmLight(
    const class MYON_Light&,
    const class MYON_3dmObjectAttributes* // can be nullptr
    );
  bool EndWrite3dmLightTable();

  bool BeginRead3dmLightTable();

  // Call either Read3dmModelLight or Read3dmLight
  /*
  Parameters:
    model_light - [out]
      MYON_ModelGeometryComponent returned here.
      nullptr returned at end of the table.
    object_filter - [in]
      optional filter made by setting MYON::object_type bits
  Returns:
     0 at end of object table
     1 if object is read
     2 if object is skipped because it does not match filter
    -1 if file is corrupt
  */
  int Read3dmModelLight(
    class MYON_ModelGeometryComponent** model_light
    );

  int  Read3dmLight(
    class MYON_Light** light,
    class MYON_3dmObjectAttributes* attributes
    );

  bool EndRead3dmLightTable();


  ///////////////////////////////////////////////////////////////////
  // Step 13: REQUIRED - Write/Read hatch pattern table (it can be empty)
  //
  bool BeginWrite3dmHatchPatternTable();
  bool Write3dmHatchPatternComponent( 
    const class MYON_ModelComponentReference& model_component_reference
    );
  bool Write3dmHatchPatternComponent( 
    const class MYON_ModelComponent* model_component 
    );
  bool Write3dmHatchPattern( const class MYON_HatchPattern&);
  bool EndWrite3dmHatchPatternTable();

  bool BeginRead3dmHatchPatternTable();
  int  Read3dmHatchPattern(class MYON_HatchPattern**);
  bool EndRead3dmHatchPatternTable();

  ///////////////////////////////////////////////////////////////////
  // Step 14: REQUIRED - Write/Read instance definition table (it can be empty)
  //
  bool BeginWrite3dmInstanceDefinitionTable();
  bool Write3dmInstanceDefinitionComponent( 
    const class MYON_ModelComponentReference& model_component_reference
    );
  bool Write3dmInstanceDefinitionComponent( 
    const class MYON_ModelComponent* model_component 
    );
  bool Write3dmInstanceDefinition( const class MYON_InstanceDefinition& );
  bool EndWrite3dmInstanceDefinitionTable();

  bool BeginRead3dmInstanceDefinitionTable();

  /*
   Description:
     Reads instance definitions from instance definition table.
  
   Parameters:
     ppInstanceDefinition - If an instance definition is
     read, an instance definition is created by calling new 
     MYON_InstanceDefinition(), initialized with values stored
     in the archive, and a pointer to the new instance definition
     is returned in *ppInstanceDefinition.
  
   Returns:
  
     @untitled table
     0     at the end of the instance definition table
     1     instance definition was successfully read
     -1    archive is corrupt at this point
  
   Example:
     Calls to Read3dmInstanceDefinition need to be bracketed by calls
     to BeginRead3dmInstanceDefinitionTable() / EndRead3dmInstanceDefinitionTable().
  
             archive.BeginRead3dmInstanceDefinitionTable();
             int rc = 1;
             MYON_InstanceDefinition* pInstanceDefinition;
             while(rc==1)
             { 
               pInstanceDefinition = 0;
               archive.Read3dmInstanceDefinition(&pInstanceDefinition);
               if ( pInstanceDefinition )
                 do something with pInstanceDefinition
             } 
             archive.EndRead3dmInstanceDefinitionTable();
  */      
  int Read3dmInstanceDefinition(
    class MYON_InstanceDefinition** // ppInstanceDefinition
    );

  bool EndRead3dmInstanceDefinitionTable();

  ///////////////////////////////////////////////////////////////////
  // Step 15: REQUIRED - Write/Read geometry and annotation table (it can be empty)
  //
  bool BeginWrite3dmObjectTable();
  bool Write3dmModelGeometryComponent( 
    const class MYON_ModelComponentReference& model_component_reference
    );
  bool Write3dmModelGeometryComponent( 
    const class MYON_ModelGeometryComponent* model_geometry
    );
  bool Write3dmObject( 
         const MYON_Object&,
         const MYON_3dmObjectAttributes* // optional
         );
  bool EndWrite3dmObjectTable();

  bool BeginRead3dmObjectTable();

  // Call either Read3dmModelGeometry or Read3dmObject
  /*
  Parameters:
    model_geometry - [out]
      MYON_ModelGeometryComponent returned here.
      nullptr returned at end of the table.
    object_filter - [in]
      optional filter made by setting MYON::object_type bits
  Returns:
     0 at end of object table
     1 if object is read
     2 if object is skipped because it does not match filter
    -1 if file is corrupt
  */
  int Read3dmModelGeometry(
    class MYON_ModelGeometryComponent** model_geometry,
    unsigned int object_filter = 0
    );

  /*
  Parameters:
    bManageGeometry - [in]
      true: model_geometry will reference count and delete the MYON_Geometry pointer.
      false: The caller must delete the MYON_Geometry pointer.
    bManageAttributes - [in]
      true: model_geometry will reference count and delete the MYON_3dmObjectAttributes pointer.
      false: The caller must delete the MYON_3dmObjectAttributes pointer.
    model_geometry - [out]
      MYON_ModelGeometryComponent returned here.
      nullptr returned at end of the table.
    object_filter - [in]
      optional filter made by setting MYON::object_type bits
      0 = no filter.
  Returns:
     0 at end of object table
     1 if object is read
     2 if object is skipped because it does not match filter
    -1 if file is corrupt
  */
  int Read3dmModelGeometryForExperts(
    bool bManageGeometry,
    bool bManageAttributes,
    class MYON_ModelGeometryComponent** model_geometry,
    unsigned int object_filter
    );

  /*
  Parameters:
    model_object - [out]
      nullptr returned at end of the table.
    attributes - [out]
      If not nullptr, then attributes are returned here
    object_filter - [in]
      optional filter made by setting MYON::object_type bits
  Returns:
     0 at end of object table
     1 if object is read
     2 if object is skipped because it does not match filter
    -1 if file is corrupt
  */
  int Read3dmObject(
    MYON_Object** model_object,
    MYON_3dmObjectAttributes* attributes,
    unsigned int object_filter = 0
    );

private:
  /*
  Description:
    In rare cases one object must be converted into another.
    Examples include reading obsolete objects and converting them into their 
    current counterpart, converting WIP objects into a proxy for a commercial build, 
    and converting a proxy object into a WIP object for a WIP build.
  */
  MYON_Object* Internal_ConvertObject(
    const MYON_Object* archive_object,
    const MYON_3dmObjectAttributes* attributes
  ) const;

public:

  bool EndRead3dmObjectTable();

  ///////////////////////////////////////////////////////////////////
  // Step 16: REQUIRED - Write/Read history record table (it can be empty)
  //
  bool BeginWrite3dmHistoryRecordTable();
  bool Write3dmHistoryRecordComponent( 
    const class MYON_ModelComponentReference& model_component_reference
    );
  bool Write3dmHistoryRecordComponent( 
    const class MYON_ModelComponent* model_component 
    );
  bool Write3dmHistoryRecord( 
         const class MYON_HistoryRecord&
         );
  bool EndWrite3dmHistoryRecordTable();

  bool BeginRead3dmHistoryRecordTable();

  /*
  Returns:
           0 at end of object table
           1 if object is read
          -1 if file is corrupt
  */
  int  Read3dmHistoryRecord(
          class MYON_HistoryRecord*&
          );
  bool EndRead3dmHistoryRecordTable();

  ///////////////////////////////////////////////////////////////////
  // Step 17: OPTIONAL - Write/Read 0 or more user tables
  //

  /*
  Description:
    Write the user table header information that must precede
    the user table information written by a plug-in.
  Parameters:
    plugin_id - [in]
    bSavingGoo - [in]
      Set to false if a plug-in will be used to write
      the user table.  Set to true if a user table written by
      a missing plug-in is being resaved. In this case,
      goo_3dm_version and goo_opennurbs_version must also be
      set.  In practice, you should use Write3dmAnonymousUserTableRecord()
      to handle writing "goo" and use this function only when
      the plug-in in present.
    goo_3dm_version - [in]
      If bSavingGoo is false, this parameter must be zero and
      MYON_BinaryArchive::Archive3dmVersion() will be used.
      If bSavingGoo is true, this parameter must be the version of 
      the 3dm archive (1,2,3,4,5,50,...) the plug-in code used to 
      write the user table.
    goo_opennurbs_version - [in]
      If bSavingGoo is false, this parameter must be zero and
      MYON_BinaryArchive::ArchiveOpenNURBSVersion() will be used.
      If bSavingGoo is true, this parameter must be the version
      of the opennurbs the plug-in code used to write the 
      user table.
  Returns:
    True if the the user information can be written.
    False if user information should not be written.
  */
  bool BeginWrite3dmUserTable(
    MYON_UUID plugin_id,
    bool bSavingGoo,
    int goo_3dm_version,
    unsigned int goo_opennurbs_version
    );

  bool EndWrite3dmUserTable();

  /*
  Description:
    If Read3dmAnaonymousUserTable() was used to read MYON_3dmGoo because a 
    plug-in was not present, then use Write3dmAnonymousUserTableRecord()
    to put than information back into the archive.
    Write3dmAnonymousUserTableRecord() writes the entire record.
    Do NOT call BeginWrite3dmUserTable() / EndWrite3dmUserTable() when
    using Write3dmAnonymousUserTableRecord().
  Parameters:
    plugin_id - [in]
    goo_version - [in]
      The version of the archive (1,2,3,4,5,50,...) that was used when
      the plug-in wrote the user table.
    goo_opennurbs_version - [in]
      The version of opennurbs ( YYYMMDDN ) that was used when the 
      plug-in wrote the user table.
    goo - [in]
  Returns:
    True if the goo was written.
    False if skipped because it could not be robustly saved.
  */
  bool Write3dmAnonymousUserTableRecord( 
    MYON_UUID plugin_id,
    int goo_3dm_version,
    unsigned int goo_opennurbs_version,
    const class MYON_3dmGoo& goo
    );

  MYON_DEPRECATED_MSG("use BeginWrite3dmUserTable(plugin_id, bSavingGoo, 3dm_version, opennurbs_version)")
  bool BeginWrite3dmUserTable( const MYON_UUID& );

  MYON_DEPRECATED_MSG("use Write3dmAnonymousUserTableRecord(plugin_id, ..., goo)")
  bool Write3dmAnonymousUserTable( const class MYON_3dmGoo& );

  /*
  Parameters:
    plugin_id - [out] 
      id of plug-in that wrote the user table
    bLastSavedAsGoo - [out] 
      True if this table was saved into this archive as goo because
      the plug-in was not present at the time of the save.
    archive_3dm_version - [out]
      Version of the archive the plug-in wrote to.  When bLastSavedAsGoo
      is true, this number can be different from Archive3dmVersion().
    archive_opennurbs_version - [out]
      Version of opennurbs the plug-in used to write the archive.  
      When bLastSavedAsGoo is true, this number can be different 
      from ArchiveOpenNURBSVersion().     
  Returns:
    False when there are no more user tables or an IO error occurs.
  */
  bool BeginRead3dmUserTable(
    MYON_UUID& plugin_id,
    bool* bLastSavedAsGoo,
    int* archive_3dm_version,
    unsigned int* archive_opennurbs_version
    );

  /*
  Description:
    If the plug-in that wrote the user table is not present and you need
    to read and resave the user table, then use Read3dmAnonymousUserTable()
    to load the information into "goo".
    If you do not need to resave the information, then simply call EndRead3dmUserTable()
    to skip over this table.
  */
  bool Read3dmAnonymousUserTable( 
    int archive_3dm_version,
    unsigned int archive_opennurbs_version,
    MYON_3dmGoo& goo
    );

  bool EndRead3dmUserTable();

  ///////////////////////////////////////////////////////////////////
  // Step 18: REQUIRED when writing / OPTIONAL when reading
  //         Write end of file marker.  This information is primarily
  //         used when debugging files to make sure the end of the file
  //         hasn't been cut off.
  //

  // Description:
  //   Writes a TCODE_ENDOFFILE chunk that contains the number
  //   of bytes in the archive.
  //
  // Returns:
  //   true if successful, false if unable to write to archive.
  bool Write3dmEndMark();

  // Description:
  //   Checks for a TCODE_ENDOFFILE chunk at the current position.
  //   If it finds one, it reads it and returns the number
  //   of bytes in the archive.  Comparing this number with
  //   the current file position can help detect files that
  //   have been damaged by losing sections.
  //
  // Parameters:
  //   sizeof_archive - [out] number of bytes written to archive
  //
  // Returns:
  //   true if successful, false if unable to find or read
  //   a TCODE_ENDOFFILE chunk.
  bool Read3dmEndMark( 
           size_t* // sizeof_archive
           );

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  // Low level tools to  Write/Read chunks. See opennurbs_3dm.h for details
  // about the structure of chunks.  Every chunk must begin with a
  // call to BeginWrite/ReadChunk().
  // If BeginWriteChunk()/BeginReadChunk() returns true, then
  // you must call EndWrite/ReadChunk() or cease using the archive.

  // Description:
  //   Writes a chunk header containing 4 byte typecode and value.
  //
  // Parameters:
  //   typecode - [in] a TCODE_* number from opennurbs_3dm.h
  //   value    - [in] if (typecode&TCODE_SHORT) is nonzero, then
  //              this is the value to be saved.  Otherwise, pass
  //              a zero and the EndWrite3dmChunk() call will
  //              store the length of the chunk.
  //
  // Returns:
  //   true if write was successful.
  bool BeginWrite3dmChunk(
        unsigned int, // typecode
        int // value
        );

  bool BeginWrite3dmBigChunk(
        MYON__UINT32 typecode,
        MYON__INT64 value
        );

  /*
  Description:
    Begins writing a chunk.
  Parameters:
    tcode - [in] chunk's typecode from opennurbs_3dm.h.  This cannot be a short tcode.
    major_version - [in] ( >= 1)
    minor_version - [in] ( >= 0 )
  Returns:
    True if input was valid and chunk was started.  In this case
    You must call EndWrite3dmChunk(), even if something goes wrong
    while you attempt to write the contents of the chunk.
    False if input was not valid or the write failed.
  */
  bool BeginWrite3dmChunk(
        unsigned int tcode,
        int major_version,
        int minor_version
        );

  /*
  Description:
    If version >= 0, calls BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,version).
  */
  bool BeginWrite3dmAnonymousChunk(
    int version
    );


  // updates length in chunk header
  bool EndWrite3dmChunk();

  bool Write3dmGoo( const MYON_3dmGoo& ); // call to write "goo"

  //MYON_DEPRECATED_MSG("use BeginRead3dmBigChunk")
  //bool BeginRead3dmChunk(
  //      unsigned int*,   // typecode from opennurbs_3dm.h
  //      int*             // value
  //      );

  // When the end of the 3dm file is reached, BeginReadChunk() will
  // return true with a typecode of TCODE_ENDOFFILE.
  bool BeginRead3dmBigChunk(
        unsigned int*,   // typecode from opennurbs_3dm.h
        MYON__INT64*       // value
        );
  /*
  Description:
    Begins reading a chunk that must be in the archive at this location.
  Parameters:
    expected_tcode - [in] chunk's typecode from opennurbs_3dm.h
    major_version - [out] 
    minor_version - [out] 
  Returns:
    True if beginning of the chunk was read.  In this case
    You must call EndRead3dmChunk(), even if something goes wrong
    while you attempt to read the interior of the chunk.
    False if the chunk did not exist at the current location in the file.
  */
  bool BeginRead3dmChunk(
        unsigned int expected_tcode,
        int* major_version,
        int* minor_version
        );

  /*
  Description:
    Calls BeginWRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_version,&minor_version),
    checks that 1 == major_version, minor_version >= 0 and returns the value
    of minor_version as version.
  Parameters:
    version - [out]
  */
  bool BeginRead3dmAnonymousChunk(
    int* version
    );

  /*
  Description:
    Calling this will skip rest of stuff in chunk if it was only partially read.
  Parameters:
    bSupressPartiallyReadChunkWarning - [in]
      Generally, a call to MYON_WARNING is made when a chunk is partially
      read.  If bSupressPartiallyReadChunkWarning is true, then
      no warning is issued for partially read chunks.
  */
  bool EndRead3dmChunk(); 
  bool EndRead3dmChunk(bool bSupressPartiallyReadChunkWarning); 


  ///////////////////////////////////////////////////////////////////
  //
  // Tools for dictionary IO (used in .NET)
  //

  /*
  Description:
    Begins writing a dictionary.
  Parameters:
    dictionary_id - [in]
    version - [in]
      It is suggested that you use MYON_VersionNumberConstruct() to create
      a version number.
    dictionary_name - [in]
      You may pass nullptr.
  Remarks:
    Begins a new chunk with tcode TCODE_DICTIONARY and then writes
    a TCODE_DICTIONARY_ID chunk containing the id, version and name.
    After calling this function, you may either write entries by
    calling
      BeginWriteDictionaryEntry(); 
      write entry definition...
      EndWriteDictionaryEntry();
    or you may finish writing the dictionary by calling
      EndWriteDictionary();
  */
  bool BeginWriteDictionary(
          MYON_UUID dictionary_id,
          unsigned int version,
          const wchar_t* dictionary_name
          );
  /*
  Description:
    Begins writing a dictionary entry.
  Parameters:
    de_type - [in]
    entry_name - [in]
  Returns:
    true 
      Entry header was written and you must call EndWriteDictionary()
      after writing the entry data.
    false 
      Failed to write entry header.  Do not call EndWriteDictionary().
  Remarks:
    Begins a new chunk with tcode TCODE_DICTIONARY_ENTRY,
    then writes the int, and then writes the string.
  */
  bool EndWriteDictionary();

  /*
  Description:
    Begins writing a dictionary entry.
  Parameters:
    de_type - [in]
    entry_name - [in]
  Returns:
    true 
      Entry header was written and you must call EndWriteDictionary()
      after writing the entry data.
    false 
      Failed to write entry header.  Do not call EndWriteDictionary().
  Remarks:
    Begins a new chunk with tcode TCODE_DICTIONARY_ENTRY,
    then writes the int, and then writes the string.
  */
  bool BeginWriteDictionaryEntry(
          int de_type, 
          const wchar_t* entry_name
          );
  bool EndWriteDictionaryEntry();

  bool BeginReadDictionary(
          MYON_UUID* dictionary_id,
          unsigned int* version,
          MYON_wString& dictionary_name
          );
  bool EndReadDictionary();

  /*
  Description:
    Begin reading a dictionary entry.
  Parameters:
    de_type - [out]
    entry_name - [out]
  Returns:
    0: serious IO error
    1: success
        read information and then call EndReadDictionaryEntry()
    2: at end of dictionary
  */
  int BeginReadDictionaryEntry(
          int* de_type, 
          MYON_wString& entry_name
          );
  bool EndReadDictionaryEntry();

  bool Read3dmGoo( MYON_3dmGoo& ); // Call to read "goo"

  MYON_DEPRECATED_MSG("use PeekAt3dmBigChunkType")
  bool PeekAt3dmChunkType( // does not change file position
        unsigned int*,   // typecode from opennurbs_3dm.h
        int*             // value
        );

  bool PeekAt3dmBigChunkType( // does not change file position
        MYON__UINT32* typecode,
        MYON__INT64* big_value
        );

  bool Seek3dmChunkFromStart( 
        // beginning at the start of the active chunk, search portion of
        // archive included in active chunk for the start of a subchunk 
        // with the specified type.
        // if true is returned, then the position is set so the next call to
        // BeginRead3dmChunk() will read a chunk with the specified typecode
        unsigned int    // typecode from opennurbs_3dm.h
        );
  bool Seek3dmChunkFromCurrentPosition( 
        // beginning at the current position, search portion of archive
        // included in active chunk for the start of a subchunk with the
        // specified type.
        // if true is returned, then the position is set so the next call to
        // BeginRead3dmChunk() will read a chunk with the specified typecode
        unsigned int    // typecode from opennurbs_3dm.h
        );

  // A chunk version is a single byte that encodes a major.minor 
  // version number.  Useful when creating I/O code for 3dm chunks
  // that may change in the future.  Increment the minor version 
  // number if new information is added to the end of the chunk. 
  // Increment the major version if the format of the chunk changes
  // in some other way.
  bool Write3dmChunkVersion(
    int, // major // 0 to 15
    int  // minor // 0 to 16
    );
  bool Read3dmChunkVersion(
    int*, // major // 0 to 15
    int*  // minor // 0 to 16
    );

  /*
  Description:
    Low level tool to writes user data attached to the 
    object.  This function should never be called
    directly.
  Parameters:
    object - [in]
  Returns:
    True if successful.
  */
  bool WriteObjectUserData( const MYON_Object& object );

  /*
  Description:
    Low level tool to read user data and attach it to
    the object.  This function should never be called
    directly.
  Parameters:
    object - [in/out]
  Returns:
    True if successful.
  */
  bool ReadObjectUserData( MYON_Object& object );

  /*
  Description:
    If a 3dm archive is being read or written, then this is the
    version of the 3dm archive format (1, 2, 3, 4, 5, 50, 60, ...).
  Returns:
    @untitle table
    0     a 3dm archive is not being read/written
    1     a version 1 3dm archive is being read/written
    2     a version 2 3dm archive is being read/written
    3     a version 3 3dm archive is being read/written
    4     a version 4 3dm archive is being read/written
    5     an old version 5 3dm archive is being read
    50    a version 5 3dm archive is being read/written
    60    a version 6 3dm archive is being read/written
    70    a version 7 3dm archive is being read/written
    ...
  See Also:
    MYON_BinaryArchive::ArchiveOpenNURBSVersion
  */
  int Archive3dmVersion() const;

  /*
  Description:
    If a 3dm archive is being read, then this is the version
    of openNURBS that was used to write the archive.  This value
    is only available after MYON_BinaryArchive::Read3dmProperties
    is called.
  See Also:
    MYON_BinaryArchive::Archive3dmVersion
    MYON_BinaryArchive::Read3dmProperties
  Returns:
    Version of openNURBS used to write the archive.  The openNURBS
    version is the value returned by MYON::Version.
  See Also:
    MYON::Version
    MYON_BinaryArchive::Read3dmProperties
    MYON_BinaryArchive::Archive3dmVersion
  Remarks:
    This value is rarely needed.  You probably want to
    use MYON_BinaryArchive::Archive3dmVersion.
  */
  unsigned int ArchiveOpenNURBSVersion() const;
  
  /*
  Returns:
    The runtime environment where the archive was created.
  Remarks:
    When reading an archive, compare the values of 
    MYON_BinaryArchive::ArchiveRuntimeEnvironment()
    and 
    MYON::CurrentRuntimeEnvironment() 
    to determine if adjustments need to be made to resources provided
    by runtime environments, like fonts.
  */
  MYON::RuntimeEnvironment ArchiveRuntimeEnvironment() const;

  const MYON_DimStyle& ArchiveCurrentDimStyle() const;
  const int ArchiveCurrentDimStyleIndex() const;
  const MYON_UUID ArchiveCurrentDimStyleId() const;

  /*
  Description:
    If a 3dm archive is being written to a version 2,3,4 or 50 format,
    then new format opennurbs version numbers need to be saved in the
    old YYYYMMDDN format.  This function returns the value that should
    be written in the file.
  Parameters:
    archive_3dm_version - [in]
      Version of the file that is being written (2, 3, 4, 50, 60, ...)
    opennurbs_version - [in]
      opennurbs version number
  Returns:
    Value to save in the file.
  */
  static unsigned int ArchiveOpenNURBSVersionToWrite(
    unsigned int archive_3dm_version,
    unsigned int opennurbs_version
    );

  /*
  Description:
    When a 3dm archive is saved from an MFC application that
    supports Windows linking/embedding, the first 5kb to 1mb
    of the file contains information that is put there by MFC.
    ArchiveStartOffset() returns the offset into the file where
    the 3dm archive actually begins. The call to 
    MYON_BinaryArchive::Read3dmStartSection() calculates this
    offset and stores the value in m_3dm_start_section_offset.
  Returns:
    Offset into the binary "file" where the actual 3dm archive
    begins.
  Remarks:
    Generally, this value can be ignored. This function is
    a diagnostice tool that is used to analyzed damaged files.
  */
  size_t ArchiveStartOffset() const;

  /*
  Description:
    Expert user function for reading damaged files.
  Parameters:
    chunk - [out] current chunk.
  Returns:
    Level of the chunk or 0 if there is no current
    chunk.
  */
  int GetCurrentChunk(MYON_3DM_CHUNK& chunk) const;
  int GetCurrentChunk(MYON_3DM_BIG_CHUNK& big_chunk) const;

  /*
  Description:
    Expert user function for reading damaged files.  The search starts
    at the beginning of the file.
  Parameters:
    tcode_table - [in] typecode of the table
    tcode_record - [in] typecode of the record
    class_uuid - [in] id of the opennurbs class in the record
    min_length_data - [in] minimum size of the opennurbs class data
  Returns:
    True if the table start is found.  In this case the current
    position of the archive is at the start of the table and
    the standared BeginRead3dm...Table() function can be used.
    False if the table start is not found.
  */
  bool FindTableInDamagedArchive(
          unsigned int tcode_table,
          unsigned int tcode_record,
          MYON_UUID class_uuid,
          int min_length_data
          );

  /*
  Description:
    Expert user function for studying contents of a file.
    The primary use is as an aid to help dig through files
    that have been damaged (bad disks, transmission errors, etc.)
    If an error is found, a line that begins with the word
    "ERROR" is printed.
  Parameters:
    text_log - [in] place to print information
    recursion_depth - [in] simply a counter
        to aid in debugging.
  Returns:
    0 if something went wrong, otherwise the typecode
    of the chunk that was just studied.
  */
  unsigned int 
  Dump3dmChunk(
        MYON_TextLog& text_log, 
        int recursion_depth = 0
        );

  enum class eStorageDeviceError : unsigned int
  {
    None = 0,
    
    // values from 1 through 0xFFFFFFF0 are used for IO device 
    // specific exceptions that terminate reading or writing.

    WriteFailed = 0xFFFFFFF1, // writing to device failed
    SeekFailedDuringWriting = 0xFFFFFFF2, // virtual Seek() failed during writing
    ReadFailed = 0xFFFFFFF8,  // reading from device failed
    SeekFailedDuringReading = 0xFFFFFFF9,  // virtual Seek() failed during reading
    UnknownDeviceError = 0xFFFFFFFFU
  };

  static MYON_BinaryArchive::eStorageDeviceError StorageDeviceErrorFromUnsigned(
    unsigned int storage_device_error_as_unsigned
    );

  /*
  Description:
    An error terminated reading or writing
  Returns:
    0: no error terminiated reading or writing
    !=0: See the MYON_BinaryArchive::DeviceErrorType for values
  */
  unsigned int StorageDeviceError() const;

private:
  /*
  Description:
    Works like the C runtrim fread().
  Returns:
    actual number of bytes read (like fread())
  */
  size_t Read(size_t, void*);

protected:
  /*
  Remarks:
    In some unusual situations when reading old or damaged files, a read may fail.
    Call MaskReadError( MYON__UINT64 sizeof_request, MYON__UINT64 sizeof_read )
    before calling MYON_ERROR().
  */
  virtual size_t Internal_ReadOverride( size_t, void* ) = 0; 

private:
  /*
  Description:
    Works like the C runtrim fwrite().
  Returns:
    actual number of bytes written (like fwrite())
  */
  size_t Write( size_t, const void* );
protected:
  virtual size_t Internal_WriteOverride( size_t, const void* ) = 0;

public:
  /*
  Description:
    Force Write() to flush any buffered data to physical archive.
  Returns:
    True if successful or if there is nothing to flush.  False if
    information could not be flushed.
  */
  virtual bool Flush() = 0;

  /*
  Description:
    When MYON_BinaryArchive::ReadObject() encounters userdata and
    the user data class id is not present,  LoadUserDataApplication
    is called to load the application that created user data.
  Returns:
    0 - could not load the application
    1 - successfully loaded the application
    2 - the application was already loaded
  */
  virtual
  int LoadUserDataApplication( 
    MYON_UUID application_id 
    );

  bool SetArchive3dmVersion(int);
  bool SetOpenNURBS3dmVersion(unsigned int v);

  /*
  Description:
    A non-zero storage device error terminates reading or writing.
    See the MYON_BinaryArchive::eStorageDeviceError for values.
  Parameter:
    storage_device_error - [in]
      A non-zero code that identifies an error the terminates 
      reading or writing.  
      See MYON_BinaryArchive::CriticalErrorCodes for values
  Remarks:
    Once set, the storage_device_error value cannot be changed.
  */
  void SetStorageDeviceError(
    MYON_BinaryArchive::eStorageDeviceError storage_device_error
    );
  void SetStorageDeviceError(
    unsigned int storage_device_error
    );

private:
  // 16 bit integer IO
  bool WriteInt8( size_t, const MYON__INT8* );
  bool ReadInt8( size_t, MYON__INT8* );

  // 16 bit integer IO
  bool WriteInt16( size_t, const MYON__INT16* );
  bool ReadInt16( size_t, MYON__INT16* );

  // 32 bit integer IO
  bool WriteInt32( size_t, const MYON__INT32* );
  bool ReadInt32( size_t, MYON__INT32* );

  // 64 bit integer IO
  bool WriteInt64( size_t, const MYON__INT64* );
  bool ReadInt64(  size_t, MYON__INT64* );

  bool BeginWrite3dmTable( 
    unsigned int // tcode
    );
  bool EndWrite3dmTable( 
    unsigned int // tcode
    );
  bool BeginRead3dmTable( 
    unsigned int // tcode
    );
  bool EndRead3dmTable( 
    unsigned int // tcode
    );
  
  bool Read3dmV1Layer( MYON_Layer*& );
  int  Read3dmV1Light(  // returns 0 at end of light table
                      //         1 light successfully read
                      //        -1 if file is corrupt
            MYON_Light**, // light returned here
            MYON_3dmObjectAttributes* // optional - if NOT nullptr, object attributes are
                                    //            returned here
            );
  int Read3dmV1Material( MYON_Material** );
  int  Read3dmV1Object( // returns 0 at end of object table
                      //         1 if object is read
                      //         2 if object is skipped because it does not match filter
                      //        -1 if file is corrupt
          MYON_Object**, // object returned here (nullptr if skipped)
          MYON_3dmObjectAttributes*, // optional - if NOT nullptr, object attributes are
                                   //            returned here
          unsigned int = 0 // optional filter made by setting MYON::object_type bits
          );  // returns nullptr at end of object table

  bool Read3dmV1AttributesOrMaterial( 
            MYON_3dmObjectAttributes*,    // attributes,
            MYON_Material*,      // material,
            bool&,             // bHaveMat
            unsigned int,      // end_mark_tcode 
            class MYON__3dmV1_XDATA* = 0 // v1 "xdata"
            );
  bool Read3dmV1String( MYON_String& );
  int  Read3dmV1LayerIndex( const char* ) const;

public:
  // helpers for reading V1 objects
  bool ReadV1_TCODE_RH_POINT(MYON_Object**,MYON_3dmObjectAttributes*);
  bool ReadV1_TCODE_MESH_OBJECT(MYON_Object**,MYON_3dmObjectAttributes*);
  bool ReadV1_TCODE_LEGACY_CRV(MYON_Object**,MYON_3dmObjectAttributes*);
  bool ReadV1_TCODE_LEGACY_FAC(MYON_Object**,MYON_3dmObjectAttributes*);
  bool ReadV1_TCODE_LEGACY_SHL(MYON_Object**,MYON_3dmObjectAttributes*);
  bool ReadV1_TCODE_RHINOIO_OBJECT_NURBS_CURVE(MYON_Object**,MYON_3dmObjectAttributes*);
  bool ReadV1_TCODE_RHINOIO_OBJECT_NURBS_SURFACE(MYON_Object**,MYON_3dmObjectAttributes*);
  bool ReadV1_TCODE_RHINOIO_OBJECT_BREP(MYON_Object**,MYON_3dmObjectAttributes*);
  bool ReadV1_TCODE_ANNOTATION(unsigned int,MYON_Object**,MYON_3dmObjectAttributes*);

private:
  void UpdateCRC( size_t, const void* );
  int ReadObjectHelper(MYON_Object**);

  int m_3dm_version = 0; // 1,2,3,4,5 (obsolete 32-bit chunk sizes)
                         // 50,60,70,... (64-bit chunk sizes)

  int m_3dm_v1_layer_index = 0;
  int m_3dm_v1_material_index = 0;

  


protected:
  unsigned int ErrorMessageMask() const;
  /*
  Parameters:
    sizeof_request - [in] 
      value of count parameter passed to virtual Read() function.
    sizeof_read - [in]
      number of bytes actually read by the virtual Read() function.
  Returns:
    True if a call to Read() is permitted to ask for more bytes
    than are left in the file.  This value varies as the file
    is read and must be checked at each failure.
  */
  bool MaskReadError( MYON__UINT64 sizeof_request, MYON__UINT64 sizeof_read ) const;

private:


  // When a 3DM archive is read, m_3dm_opennurbs_version records the version of
  // OpenNURBS used to create the archive.  Otherwise, m_3dm_opennurbs_version
  // is zero.
  //
  // Read3dmProperties() sets this to the version of OpenNURBS that was
  // used to write file file.  If the file was created using a version
  // of OpenNURBS before 200012210, this number will be zero.
  //
  // Write3dmProperties() stores the value returned by MYON::Version() in
  // the archive's properties table.
  friend void MYON_SetBinaryArchiveOpenNURBSVersion(MYON_BinaryArchive&,unsigned int);
  unsigned int m_3dm_opennurbs_version = 0;

  MYON::RuntimeEnvironment m_archive_runtime_environment = MYON::RuntimeEnvironment::Unset;

  // When a 3dm archive is saved from an MFC application that supports
  // Windows linking/embedding, the first 5kb to 1mb of the file contains
  // information that is put there by MFC.  m_3dm_start_section_offset
  // records the offset into the file where the 3dm archive actually begins.
  size_t m_3dm_start_section_offset = 0;

  /*Read3dmTableRecordBegin
  m_3dm_previous_table = 3dm archive table that was most recently read/written.
  m_3dm_active_table = 3dm archive table currently being read/written
  */
  MYON_3dmArchiveTableType m_3dm_previous_table = MYON_3dmArchiveTableType::Unset;
  MYON_3dmArchiveTableType m_3dm_active_table = MYON_3dmArchiveTableType::Unset;
  // If reading/writing a table fails, m_3dm_failed_table identifies the first failure.
  MYON_3dmArchiveTableType m_3dm_first_failed_table = MYON_3dmArchiveTableType::Unset;

  int m_user_data_depth = 0; // > 0 when user data is being read or written

  // 3dm archive status information
  class MYON_3dmTableStatusLink* m_3dm_table_status_list = nullptr;

private:
  bool Internal_Write3dmUpdateManifest(
    const MYON_ModelComponent& model_component
    );
  bool Internal_Write3dmLightOrGeometryUpdateManifest(
    MYON_ModelComponent::Type component_type,
    MYON_UUID component_id,
    int component_index,
    const MYON_wString & component_name
    );
  bool Internal_Read3dmUpdateManifest(
    MYON_ModelComponent& model_component
    );
  bool Internal_Read3dmLightOrGeometryUpdateManifest(
    MYON_ModelComponent::Type component_type,
    MYON_UUID component_id,
    int component_index,
    const MYON_wString & component_name
    );

private:
  bool Internal_IncrementCurrentPosition(
    MYON__UINT64 delta
  );
  bool Internal_DecrementCurrentPosition(
    MYON__UINT64 delta
  );
  MYON__UINT64 m_current_positionX = 0;

  /*
  Description:
    Increments m_crc_error_count and active table m_crc_error_count.
  */
  void Internal_ReportCRCError();
  
  unsigned int m_crc_error_count = 0; // number of chunks that have a bad crc
  
  /*
  Description:
    Increments m_critical_error_count and active table m_critical_error_count.
  */
  void Internal_ReportCriticalError();

  // Number of critical errors.  These errors are more serious than a CRC error.
  // If a critical error occurs, the information being read or written is 
  // so corrupted that chunk accounting is failing or the calling code is deeply flawed. 
  unsigned int m_critical_error_count = 0;

  // MYON_BinaryArchive::eStorageDeviceError values are used to set
  // m_storage_device_error.
  // MYON_BinaryArchive::StorageDeviceError() returns the value.
  unsigned int m_storage_device_error = 0;

  // The bits in m_error_message_mask are used to mask errors
  // when we know we are doing something that may generate an
  // error.
  //
  // bit 0x00000001
  //   Setting this bit masks an error when attempting to read 4 bytes
  //   at the end of a file.
  //   V1 files do not have a table structure and are read using
  //   multiple passes and there are valid situations where a 
  //   4 byte read is attempted at the end of a file.
  //   This situation also occurs when a damaged file is missing a table
  //   or contains tables in the wrong order and the table must be searched
  //   for by typecode.
  //
  // bit 0x00000002
  //   Some v1 files do not have an end mark.  When reading
  //   these v1 files bit 0x02 is set.
  //
  // bit 0x00000004
  //   Requested read may go beyond end of file.
  //   One situation where this happens is when a table is not at the 
  //   expected location in a file, 
  unsigned int m_error_message_mask = 0;


    
  MYON__UINT64 m_3dm_end_mark_length = 0;
  
  bool Begin3dmTable(
    MYON::archive_mode expected_mode,
    MYON_3dmArchiveTableType table
    );
  bool End3dmTable(
    MYON_3dmArchiveTableType table,
    bool bSuccess
    );
  void Internal_Increment3dmTableItemCount();
  bool Read3dmTableRecord(
    MYON_3dmArchiveTableType table,
    void** ptr
    );
  bool Internal_Begin3dmTableRecord(
    MYON_3dmArchiveTableType table
    );

public:
  /*
  Returns:
    Archive read/write mode
  */
  MYON::archive_mode Mode() const;

  /*
  Returns:
    True if Mode() is an archive reading mode.
  */
  bool ReadMode() const;

  /*
  Returns:
    True if Mode() is an archive writing mode.
  */
  bool WriteMode() const;

  /*
  Returns:
    True if Mode() is not set to a valid read or write mode.
  */
  bool UnsetMode() const;

  /*
  Returns:
    If a 3dm archive is being read or written, the value of the archive 
    section (table) being read is returned.
    MYON_3dmArchiveTableType::Unset is returned if a table is
    not actively being read or written.
  Remarks:
    Use MYON_BinaryArchive::Mode() to determine if a binary archive is being
    read or written.
    Use MYON_BinaryArchive::Previous3dmTable() to determine the most recent
    table that was successfully read and finished.
  */
  MYON_3dmArchiveTableType Active3dmTable() const;

  static MYON_ModelComponent::Type TableComponentType(
    MYON_3dmArchiveTableType table_type
  );

  /*
  Returns:
    If a 3dm archive is being read or written, the value of the most
    recently read or written archive section (table) is returned.
  Remarks:
    Use MYON_BinaryArchive::Mode() to determine if a binary archive is being
    read or written.
  */
  MYON_3dmArchiveTableType Previous3dmTable() const;

  /*
  Returns:
    If a 3dm archive is being read or written and a failure occurs,
    the first archive section (table) that failed to read or write
    is returned.
  */
  MYON_3dmArchiveTableType FirstFailed3dmTable() const;

  /*
  Returns:
    Number of chunks read with a bad CRC
  */
  unsigned int BadCRCCount() const;

  /*
  Returns:
    Number of critical errors
  */
  unsigned int CriticalErrorCount() const;

  const MYON_3dmArchiveTableStatus Archive3dmTableStatus(
    MYON_3dmArchiveTableType table_type
    );

private:

  MYON_3dmArchiveTableType TableTypeFromTypecode( unsigned int ); // table type from tcode

  MYON_SimpleArray<MYON_3DM_BIG_CHUNK> m_chunk;

  // stack of chunks
  bool PushBigChunk( MYON__UINT32 typecode, MYON__INT64 value );

  bool WriteChunkTypecode( MYON__UINT32 );
  bool ReadChunkTypecode( MYON__UINT32* );
  bool WriteChunkValue( MYON__UINT32 typecode, MYON__INT64 );
  bool WriteChunkLength( MYON__UINT64 );
  bool ReadChunkValue( MYON__UINT32 typecode, MYON__INT64* value64 );
  bool FindMisplacedTable( 
        MYON__UINT64 filelength,
        const MYON__UINT32 table_tocde,
        const MYON__UINT32 table_record_record,
        const MYON_UUID class_uuid,
        const MYON__UINT64 min_length_data
        );

  bool ReadObjectUserDataAnonymousChunk(
          const MYON__UINT64 length_TCODE_ANONYMOUS_CHUNK,
          const int archive_3dm_version,
          const unsigned int archive_opennurbs_version,
          class MYON_UserData* ud );

public:
  size_t SizeofChunkLength() const;

private:
  bool WriteEOFSizeOfFile( MYON__UINT64 );
  bool ReadEOFSizeOfFile( MYON__UINT64* );

  bool m_bDoChunkCRC = false; // true if active chunk crc status should be checked
                      // and updated.
  bool m_bChunkBoundaryCheck = false;

public:
  /*
  Returns:
    true:
      All read, write, and seek operations check to make sure they stay within
      the current chunk boundary.
  */
  bool ChunkBoundaryCheck() const;

  /*
  Parameters:
    bChunkBoundaryCheck - [in]
      true:
        All read, write, and seek operations check to make sure they stay within
        the current chunk boundary.
  */
  void SetChunkBoundaryCheck(
    bool bChunkBoundaryCheck
  );

  
private:
  class MYON_CompressorImplementation* m_compressor = nullptr;
  class MYON_CompressorImplementation& Compressor();

  // returns number of bytes written
  size_t WriteDeflate(
        size_t,         // sizeof uncompressed input data
        const void*  // uncompressed input data
        );
  bool ReadInflate(
        size_t,  // sizeof uncompressed input data
        void* // buffer to hold uncompressed data
        );
  bool CompressionInit();
  void CompressionEnd();

private:
  // endian-ness of the cpu reading this file.
  // 3dm files are always saved with little endian byte order.
  const MYON::endian m_endian = MYON::Endian();

  const MYON::archive_mode m_mode = MYON::archive_mode::unset_archive_mode;

  // user data and user table reading and writing filter
  // If m_user_data_filter is empty, then all user data and user tables are read/written.
  // If m_user_data_filter is not empty, then the first element has both ids=nil, precedence=0,
  // and m_bSerialize = default setting. If there are any elements after the first element,
  // the must have m_application_id != nil and the value of m_bSerialize overrides the 
  // default setting.  If there are multiple elements with the same application and item id,
  // the most recently added element is used.
  MYON_SimpleArray< MYON_UserDataItemFilter > m_user_data_filter;
  
  /*
  Description:
    Sorts m_user_data_filter so items are ordered by 
    application id (nil is first) and precedence (low to high)
  */
  void SortUserDataFilter();

private:
  // 3dm write options

  // bits corresponded to MYON::object_type flags.
  // If the bit is set, then the mesh will be saved in the 3dm file.
  // (RhinoCommon: if default is changed, sync with File3dmWriteOptions.RenderMeshesFlags)
  MYON__UINT32 m_save_3dm_render_mesh_flags = 0xFFFFFFFFU;
  MYON__UINT32 m_save_3dm_analysis_mesh_flags = 0xFFFFFFFFU;

  bool m_bSave3dmPreviewImage = true;

  bool m_bUseBufferCompression = true;

  bool m_bReservedA = false;
  bool m_bReservedB = false;
  bool m_bReservedC = false;
  bool m_bReservedD = false;
  bool m_bReservedE = false;
  bool m_bReservedF = false;

public:
  /*
  Description:
    Specify model serial number attributes to assign to MYON_ModelComponent
    classes when they are read.
  */
  void SetModelSerialNumber(
    unsigned int model_serial_number,
    unsigned int reference_model_serial_number,
    unsigned int instance_definition_model_serial_number
    );

  /*
  Description:
    Clear() information set by SetModelSerialNumber() do not modify
    MYON_ModelComponent model serial number information when the classes
    are read.
  */  
  void ClearModelSerialNumber();

  /*
  Parameters:
    bCheckForRemappedIds - [in]
      true if the archive is reading in a situation where component ids may get remapped.
  */
  void SetCheckForRemappedIds(
    bool bCheckForRemappedIds
  );

  /*
  Returns:
    True if the archive is reading in a situation where component ids may get remapped.
  */
  bool CheckForRemappedIds() const;

  unsigned int ModelSerialNumber() const;
  unsigned int ReferenceModelSerialNumber() const;
  unsigned int InstanceDefinitionModelSerialNumber() const;
  
  /*
  Description:
    Writes the attributes identified by the component_filter parameter.
  Parameters:
    model_component - [in]
    attributes_filter - [in]
      A bitfield that determines which attributes will be written.
  Returns:
    false: critical failure.
    true: writing can continue.
  */
  bool WriteModelComponentAttributes(
    const class MYON_ModelComponent& model_component,
    unsigned int attributes_filter
    );

  /*
  Description:
    Reads the attributes the Write() function writes.
  Parameters:
    model_component - [in/out]
    component_filter - [out]
      A bitfield that reports which attributes were read.
      If the corresponding component on model_component is locked,
      the read value is discarded.
  Returns:
    false: critical failure.
    true: reading can continue.  
  Remarks:
    If locked attributes are read, thire values are ignored.
  */
  bool ReadModelComponentAttributes(
    MYON_ModelComponent& model_component,
    unsigned int* attributes_filter
    );

  /*
  Description:
    When writing archives, the index of the component in the model is
    often different than the index of the component in the archive.
    WriteComponentIndex converts the model id or index into 
    an archive index and writes the archive index value.
  Remarks:
    During writing, the m_manifest member stores 
    the model id and index as the "Component" value and
    the 3dm archive id index as the "Manifest" value.
  */
  bool Write3dmReferencedComponentIndex(
    MYON_ModelComponent::Type component_type,
    int model_component_index
    );

  /*
  Description:
    When writing archives, the index of the component in the model is
    often different than the index of the component in the archive.
    WriteComponentIndex converts the model id or index into 
    an archive index and writes the archive index value.
  Remarks:
    During writing, the m_manifest member stores 
    the model id and index as the "Component" value and
    the 3dm archive id index as the "Manifest" value.
  */
  bool Write3dmReferencedComponentIndex(
    MYON_ModelComponent::Type component_type,
    MYON_UUID model_component_id
    );

  /*
  Description:
    When writing archives, the index of the component in the model is
    often different than the index of the component in the archive.
    WriteComponentIndex converts the model id or index into 
    an archive index and writes the archive index value.
  Remarks:
    During writing, the m_manifest member stores 
    the model id and index as the "Component" value and
    the 3dm archive id index as the "Manifest" value.
  */
  bool Write3dmReferencedComponentIndex(
    const MYON_ModelComponent& model_component
    );

  /*
  Description:
    When reading 3dm archives, model component indexes in the archive and
    in the destination model are typically different.
    This function basically reads and reverses the steps that WriteArchiveComponentIndex()
    uses to adjust and write a model component index.
  Parameters:
    component_type - [in]
      Type of the referenced component.
    component_index - [out]
      component reference index
  Returns:
    false - catestrophic read failure.
  */
  bool Read3dmReferencedComponentIndex(
    MYON_ModelComponent::Type component_type,
    int* component_index
    );

  bool Read3dmReferencedComponentIndexArray(
    MYON_ModelComponent::Type component_type,
    MYON_SimpleArray<int>& component_index_array
    );

  /*
  Returns:
    True: (default state)
      Read3dmReferencedComponentIndex() and Write3dmReferencedComponentIndex() will automatically
      adjust components index references so they are valid.
    False: (uncommon)
      Read3dmReferencedComponentIndex() and Write3dmReferencedComponentIndex() will not
      adjust components index references so they are valid.
  */
  bool ReferencedComponentIndexMapping() const;

  /*
  Description:
    Set the archive's ReferencedComponentIndexMapping() state.
  Parameters:
    bEnableReferenceComponentIndexMapping - [in]
    True: (default state)
      Read3dmReferencedComponentIndex() and Write3dmReferencedComponentIndex() will automatically
      adjust components index references so they are valid.
    False: (uncommon)
      Read3dmReferencedComponentIndex() and Write3dmReferencedComponentIndex() will not
      adjust components index references so they are valid. This is only used with the
      component being read or written is not the model but is a copy of one
      in a different model (linked instance definitions being the common situation).
  */
  void SetReferencedComponentIndexMapping(
    bool bEnableReferenceComponentIndexMapping
    );

  /*
  Description:
    WriteComponentId converts the model ID into 
    an archive ID and writes the archive Id value.
    Generally, the ID of the component in the model is 
    identical to the ID of the component in the archive.
    In rare situations this is not the case.
  Remarks:
    During writing, the m_manifest member stores 
    the model ID as the "Component" value and
    the 3dm archive ID as the "Manifest" value.
  */
  bool Write3dmReferencedComponentId(
    MYON_ModelComponent::Type component_type,
    MYON_UUID model_component_id
    );

  bool Write3dmReferencedComponentId(
    const MYON_ModelComponent& model_component
    );

  /*
  Description:
    When reading 3dm archives, the model component ID in the archive
    and in the destination model are often identical, but sometimes
    different. For example, the when the same template is used
    to create multiple models and files and the models from those files
    are merged into a single file, there will be ID collisions.
    For components that are identified by name, like layers and dimension styles,
    this is not a problem. For components like instance definitions that have
    a more complicated set of merging rules, it is critical that
    references to instance definition ids be updated from values in the archive
    to values in the model.
    uses to adjust and write a model component Id.
  Parameters:
    component_type - [in]
      Type of the referenced component.
    component_id - [out]
      component reference ID
  Returns:
    false - catestrophic read failure.
  */
  bool Read3dmReferencedComponentId(
    MYON_ModelComponent::Type component_type,
    MYON_UUID* component_id
    );

  /*
  Returns:
    True: (default state)
      Read3dmReferencedComponentId() and Write3dmReferencedComponentId() will automatically
      adjust components Id references so they are valid.
    False: (uncommon)
      Read3dmReferencedComponentId() and Write3dmReferencedComponentId() will not
      adjust components Id references so they are valid.
  */
  bool ReferencedComponentIdMapping() const;

  /*
  Description:
    Set the archive's ReferencedComponentIdMapping() state.
  Parameters:
    bEnableReferenceComponentIdMapping - [in]
    True: (default state)
      Read3dmReferencedComponentId() and Write3dmReferencedComponentId() will automatically
      adjust components Id references so they are valid.
    False: (uncommon)
      Read3dmReferencedComponentId() and Write3dmReferencedComponentId() will not
      adjust components Id references so they are valid. This is only used with the
      component being read or written is not the model but is a copy of one
      in a different model (linked instance definitions being the common situation).
  */
  void SetReferencedComponentIdMapping(
    bool bEnableReferenceComponentIdMapping
    );


public:
  // Reading and writing operations fill in the manifest.
  // MYON_ComponentManifest query tools can be used to look up
  // model and archive index and id information.
  //
  // The component and manifest id values are always identical
  // during reading and writing.
  //
  // When writing, the component indices are model indices
  // and the manifest indices are the archive indices that
  // were written in the file.
  //
  // When reading, the component indices are "index" values read
  // from the archive and the manifest indices are the order they
  // were read. When files are valid, these indices are the same.
  //
  // After reading is complete, the application can use 
  // MYON_ComponentManifest::UpdateManifestItem() to convert
  // the component index and id values to model index and
  // id values.
  const class MYON_ComponentManifest& Manifest() const;
  const class MYON_ManifestMap& ManifestMap() const;
  bool AddManifestMapItem(
    const class MYON_ManifestMapItem& map_item
    );

  /*
  Description:
    When an application is reading an archive and changes the
    index or id of a model component as it is added to the model,
    then it needs to update the manifest map item destination settings.
  Parameters:
    map_item - [in]
      The source type, index and id match what was read from the 3dm archive.
      The destination index and id are the values assigned by the
      application reading the 3dm archive.
  */
  bool UpdateManifestMapItemDestination(
    const class MYON_ManifestMapItem& map_item
    );

private:
  // Reading:
  //   m_manifest is a list of what has been read from the 3dm archive.
  //   m_manifest_map is a map from the 3dm archive index and id to the
  //   model index and id. The map is maintained by the application
  //   reading the file calling AddManifestMapItem() when read items
  //   are added to the model.
  // Writing:
  //   m_manifest is a list of what has been written to the 3dm archive.
  //   m_manifest_map maps model index and id to 3dm archive index and id.
  //   m_manifest_map is automatically maintained by the MYON_BinaryArchive
  //   writing code because the index and id changes happen internally
  //   in 3dm archive writing functions.
  MYON_ComponentManifest m_manifest;
  MYON_ManifestMap m_manifest_map;

  // True: (default state)
  //  Read3dmReferencedComponentIndex() and Write3dmReferencedComponentIndex() will automatically
  //  adjust component index references so they are valid.
  // False: (uncommon)
  //  Read3dmReferencedComponentIndex() and Write3dmReferencedComponentIndex() will not
  //  adjust component index references so they are valid.
  bool m_bReferencedComponentIndexMapping = true;

  // True: (default state)
  //  Read3dmReferencedComponentId() and Write3dmReferencedComponentId() will automatically
  //  adjust component id references so they are valid.
  // False: (uncommon)
  //  Read3dmReferencedComponentId() and Write3dmReferencedComponentId() will not
  //  adjust component id references so they are valid.
  bool m_bReferencedComponentIdMapping = true;

private:
  // If the archive is a file system item (file), then
  // these strings specify the name of the file
  MYON_wString m_archive_file_name;
  MYON_wString m_archive_directory_name;
  MYON_wString m_archive_full_path; // = archive_directory_name + path separator + archive_file_name

  // If the archive is being read, this is the name
  // of the file where it was written.
  // If false = MYON_wString::EqualPath(m_archive_full_path,m_archive_saved_as_full_path),
  // then file has been moved or copied since it was saved.
  // When reading a file, this value is set by MYON_BinaryArchive::Read3dmProperties()
  // When writing a file, this value is set by SetArchiveFullPath().
  MYON_wString m_archive_saved_as_full_path;

  /*
    MYON_BinaryArchive::Read3dmProperties() sets m_bArchiveMoved to true if
    the 3dm archive being read is not in the same file system location as where
    it was written.  This piece of information is useful when attempting to find
    referenced files that are not where they were when the 3dm archive was saved.
  */
  bool m_b3dmArchiveMoved = false;

public:
  const MYON_wString& ArchiveFileName() const;
  const MYON_wString& ArchiveDirectoryName() const;
  const MYON_wString& ArchiveFullPath() const;
  const MYON_wString& ArchiveSavedAsFullPath() const;

  const wchar_t* ArchiveFileNameAsPointer() const;
  const wchar_t* ArchiveDirectoryNameAsPointer() const;
  const wchar_t* ArchiveFullPathAsPointer() const;
  const wchar_t* ArchiveSavedAsFullPathPointer() const;

  /*
  Returns:
    true if the 3dm archive being read is not in the same file system 
    location as where is was saved.
  */
  bool ArchiveFileMoved() const;

  /*
  Parameters:
    archive_full_path - [in]
      full path to file being read or written
  */
  void SetArchiveFullPath(
    const wchar_t* archive_full_path
    );

  /*
  Parameters:
    archive_directory_name - [in]
      full path file being written
    archive_file_name - [in]
      name of file being written
  */
  void SetArchiveFullPath(
    const wchar_t* archive_directory_name,
    const wchar_t* archive_file_name
    );

private:
  bool m_SetModelComponentSerialNumbers = false;
  bool m_bCheckForRemappedIds = false;
  // Expert testers who need to create a corrupt 3dm file
  // call IntentionallyWriteCorrupt3dmStartSectionForExpertTesting() before writing
  // the 3dm file.
  unsigned char m_IntentionallyWriteCorrupt3dmStartSection = 0;
  bool m_reservedB = false;
  unsigned int m_model_serial_number = 0;
  unsigned int m_reference_model_serial_number = 0;
  unsigned int m_instance_definition_model_serial_number = 0;
  unsigned int m_reserved1 = 0;
  MYON__UINT_PTR m_reserved2 = 0;

private:
  // ids of plug-ins that support saving older (V3) versions
  // of user data.  This information is filled in from the
  // list of plug-ins passed in whenteh settings are saved.
  MYON_SimpleArray<MYON_UUID> m_V3_plugin_id_list;

  struct MYON__3dmV1LayerIndex* m_V1_layer_list = nullptr;

private:
  // m_archive_text_style_table and m_archive_dim_style_table are private and not used by inline functions.
  // No DLL interface is required.

  mutable MYON_3dmAnnotationContext m_annotation_context;

#pragma MYON_PRAGMA_WARNING_PUSH
#pragma MYON_PRAGMA_WARNING_DISABLE_MSC( 4251 )
  // The m_archive_text_style_table[] array is used when reading archives.
  // It contains the text styles read from the archive
  MYON_SimpleArray< MYON_TextStyle* > m_archive_text_style_table;

  // The m_dim_style_index_text_style_index[] is used when reading archives.
  // MYON_2dex.i = text style archive index.
  // MYON_2dex.j = dimension style archive index.
  MYON_SimpleArray< MYON_2dex > m_text_style_to_dim_style_archive_index_map;

  // This m_archive_dim_style_table[] array is used when reading
  // and writing archives. This information is required when reading
  // and writing archives from previous versions.
  // - When writing, the dimstyles are copies of the model dimstyles
  //   and have model ids and indices.
  // - When reading, the dimstyles are copies of the archive dimstyles
  //   and have archive ids and indices.
  MYON_SimpleArray< MYON_DimStyle* > m_archive_dim_style_table;
  MYON_SimpleArray< MYON_DimStyle* > m_DELETE_ME_archive_dim_style_overrides;
  bool m_bLegacyOverrideDimStylesInArchive = false;

  const MYON_DimStyle* m_archive_current_dim_style = nullptr;

  // m_archive_dim_style_table_status values:
  // READING:
  // 0 = not started
  //   1 = BeginWrite3dmDimStyle() has been called, 
  //       m_archive_text_style_table[] is valid, 
  //       and Read3dmDimStyle() can be called.
  //   2 = All entries of m_archive_text_style_table[] have been read by Read3dmDimStyle().
  //   3 = EndRead3dmDimStyle() has been called.
  // WRITING:
  //   0 = not started
  //   1 = BeginWrite3dmDimStyle() has been called and Write3dmDimStyle() can be called.
  //   2 = Write3dmDimStyle() has saved at least one dimstyle
  //   3 = EndWrite3dmDimStyle() has been called.
  unsigned int m_archive_dim_style_table_status = 0;

  // index in m_archive_text_style_table[] where Read3dmDimStyle() should
  // begin searching for the next dimstyle to "read".
  unsigned int m_archive_dim_style_table_read_index = MYON_UNSET_UINT_INDEX;

#pragma MYON_PRAGMA_WARNING_POP

public:
  /*
  Description:
    When reading version 5 and earlier files that contain a text style
    table, this function can be used to get the archive text style from
    the archive text style index. This function is used when reading
    V5 and pre August 2016 V6 MYON_DimStyle information.
  */
  const MYON_TextStyle* ArchiveTextStyleFromArchiveTextStyleIndex(
    int archive_text_style_index
  ) const;

private:
  MYON_String m_archive_3dm_start_section_comment = MYON_String::EmptyString;
  class MYON_3dmProperties* m_archive_3dm_properties = nullptr;
  class MYON_3dmSettings* m_archive_3dm_settings = nullptr;

private:
  // prohibit default construction, copy construction, and operator=
  MYON_BinaryArchive() = delete;
  MYON_BinaryArchive( const MYON_BinaryArchive& ) = delete; // no implementation
  MYON_BinaryArchive& operator=( const MYON_BinaryArchive& ) = delete; // no implementation
};

class MYON_CLASS MYON_3dmGoo
{
  // used to store goo
public:
  MYON_3dmGoo();
  ~MYON_3dmGoo();
  MYON_3dmGoo( const MYON_3dmGoo& );
  MYON_3dmGoo& operator=( const MYON_3dmGoo& );

  void Dump(MYON_TextLog&) const;

  unsigned int m_typecode;
  int m_value;
  unsigned char* m_goo;
  MYON_3dmGoo* m_next_goo;
  MYON_3dmGoo* m_prev_goo;
};


class MYON_CLASS MYON_BinaryFile : public MYON_BinaryArchive
{
public:
  MYON_BinaryFile( 
    MYON::archive_mode archive_mode 
    );

  /*
  Description:
    Create an MYON_BinaryArchive that reads/writes from an ordinary file.
  Parameters:
    archive_mode - [in]
    fp - [in]
      If a file is being read, fp is the pointer returned 
      from MYON_FileStream::Open(...,"rb").
      If a file is being written, fp is the pointer returned 
      from MYON_FileStream::Open(...,"wb").
  */
  MYON_BinaryFile( 
    MYON::archive_mode archive_mode, 
    FILE* fp
    );

  /*
  Description:
    Create an MYON_BinaryArchive that reads/writes from an ordinary file.
  Parameters:
    archive_mode - [in]
    file_system_path - [in]
      path to file being read or written.
  */
  MYON_BinaryFile(
    MYON::archive_mode archive_mode, 
    const wchar_t* file_system_path
    );

  /*
  Description:
    Create an MYON_BinaryArchive that reads/writes from an ordinary file.
  Parameters:
    archive_mode - [in]
    file_system_path - [in]
      path to file being read or written.
  */
  MYON_BinaryFile(
    MYON::archive_mode archive_mode, 
    const char* file_system_path
    );

  ~MYON_BinaryFile();

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

public:

  //// fseek from end (since the file has an end)
  //bool SeekFromEnd( int ); 

  //////////
  // To use custom memory buffering instead of relying
  // on fread()/fwrite()'s build in buffering, call
  // EnableMemoryBuffer() with the buffer size immediately
  // after constructing the MYON_BinaryFile.  There appear
  // to be enough bugs in existing Windows NT/2000 NETWORK
  // I/O that using this hack will speed up I/O by factors
  // of 10 to 100.
  void EnableMemoryBuffer(
         int=16384 // capacity of memory buffer
         );

  /*
  Returns:
    True if a file stream is open (nullptr != m_fp).
  */
  bool FileIsOpen() const;

  void CloseFile();

private:
  // Implementation
  FILE* m_fp = nullptr;
  bool m_bCloseFileInDestructor = false;

  // if m_memory_buffer_capacity is zero, then Write() uses
  // fwrite() directly.  If m_memory_buffer_capacity is
  // greater than zero, then Write() buffers its results
  // into m_memory_buffer.  This is provided to work around
  // bugs in some networks that result in extremely slow
  // performance when seeking is used.
  size_t m_memory_buffer_capacity = 0;
  size_t m_memory_buffer_size = 0;
  size_t m_memory_buffer_ptr = 0;
  unsigned char* m_memory_buffer = nullptr;

private:
  // prohibit default construction, copy construction, and operator=
  MYON_BinaryFile() = delete;
  MYON_BinaryFile(const MYON_BinaryFile&) = delete;
  MYON_BinaryFile& operator=(const MYON_BinaryFile&) = delete;
};

class MYON_CLASS MYON_BinaryArchiveBuffer : public MYON_BinaryArchive
{
public:
  /*
  Description:
    Create an MYON_BinaryArchive that reads/writes from an MYON_Buffer.
  Parameters:
    mode - [in]
    buffer - [in]
  Remarks:
    If a non-null buffer is specified, then do not call SetBuffer()
  */
  MYON_BinaryArchiveBuffer( MYON::archive_mode, MYON_Buffer* buffer );

  virtual ~MYON_BinaryArchiveBuffer();

  /*
  Description:
    If the MYON_BinaryArchiveBuffer class is created with the constructor
    that has a single "mode" parameter, then use SetBuffer()
    to specify the buffer to read/write from before using
    the MYON_BinaryArchiveBuffer.
  Parameters:
    buffer - [in]
  Returns:
    True if the buffer is set.  Once the buffer is set it
    cannot be changed.
  */
  bool SetBuffer( MYON_Buffer* buffer );

  /*
  Returns:
    Buffer being read/written. 
  */
  MYON_Buffer* Buffer() const;

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
  // Buffer being read/written.
  MYON_Buffer* m_buffer;

private:
  // prohibit use - you should specify a buffer.
  MYON_BinaryArchiveBuffer( MYON::archive_mode );
private:
  // prohibit default construction, copy construction, and operator=
  MYON_BinaryArchiveBuffer( ); // no implementation
  MYON_BinaryArchiveBuffer( const MYON_BinaryArchiveBuffer& ); // no implementation
  MYON_BinaryArchiveBuffer& operator=( const MYON_BinaryArchiveBuffer& ); // no implementation
};


class MYON_CLASS MYON_Read3dmBufferArchive : public MYON_BinaryArchive
{
public:

  /*
  Description:
    Construct an MYON_BinaryArchive for reading information from a memory buffer.
  Parameters:
    sizeof_buffer - [in] size of buffer in bytes (>0)
    buffer - [in] memory buffer containing binary archive
    bCopyBuffer - [in]
      true - copy the input buffer.  
          Useful when the buffer may be destroyed while this class is still in use.
      false - Do not copy the input buffer.  
          In this case you are responsible for making certain the input buffer 
          is valid while this class is in use.
    archive_3dm_version  - [in] (1,2,3,4,5,50,60,70,...)
    archive_opennurbs_version - [in] 
  */
  MYON_Read3dmBufferArchive( 
    size_t sizeof_buffer, 
    const void* buffer,
    bool bCopyBuffer,
    int archive_3dm_version,
    unsigned int archive_opennurbs_version
    );

  ~MYON_Read3dmBufferArchive();

  /*
  Returns: 
     value of m_sizeof_buffer
  */
  size_t SizeOfBuffer() const;

  /*
  Returns: 
     value of m_buffer
  */
  const void* Buffer() const;

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
  void* m_p;
  const unsigned char* m_buffer;
  size_t m_sizeof_buffer;
  size_t m_buffer_position;
  MYON__INT_PTR m_reserved1;
  MYON__INT_PTR m_reserved2;
  MYON__INT_PTR m_reserved3;
  MYON__INT_PTR m_reserved4;

private:
  // prohibit use - no implementation
  MYON_Read3dmBufferArchive(); 
  MYON_Read3dmBufferArchive( const MYON_Read3dmBufferArchive& );
  MYON_Read3dmBufferArchive& operator=(const MYON_Read3dmBufferArchive&);
};

class MYON_CLASS MYON_Write3dmBufferArchive : public MYON_BinaryArchive
{
public:

  /*
  Description:
    Construct an MYON_BinaryArchive for writing information to a memory buffer.
  Parameters:
    initial_sizeof_buffer - [in] 
      initial size of buffer in bytes (>=0)
      If you are unable to estimate the size you will need, pass in zero.
    max_sizeof_buffer - [in] 
      maximum size of buffer in bytes (>=0)
      If max_sizeof_buffer > 0 and the amount of information saved 
      requires a buffer larger than this size, then writing fails. 
      If max_sizeof_buffer <= 0, then no buffer size limits are enforced.
    archive_3dm_version  - [in] (0, ,2,3,4,5,50,60,70,...)
      Pass 0 or MYON_BinaryArchive::CurrentArchiveVersion() to write the
      version of opennurbs archives used by lastest version of Rhino.
    archive_opennurbs_version - [in]
  */
  MYON_Write3dmBufferArchive( 
    size_t initial_sizeof_buffer, 
    size_t max_sizeof_buffer, 
    int archive_3dm_version,
    unsigned int archive_opennurbs_version
    );

  ~MYON_Write3dmBufferArchive();

  /*
  Returns: 
     Size of the archive in bytes.
  */
  size_t SizeOfArchive() const;

  /*
  Returns: 
     value of m_sizeof_buffer
  */
  size_t SizeOfBuffer() const;

  /*
  Returns: 
     value of m_buffer.
     SizeOfArchive() reports the number of bytes
     written to this buffer.
     SizeOfBuffer() reports the number of bytes
     allocated in this buffer.
     
  */
  const void* Buffer() const;

  /*
  Returns:
    The pointer to the buffer and sets all 
    members on this archive back to zero.
    The caller is responsible for calling onfree() on
    the pointer when finished with the buffer.
  */
  void* HarvestBuffer();

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
  void AllocBuffer(size_t);
  void* m_p;
  unsigned char* m_buffer;
  size_t m_sizeof_buffer;
  const size_t m_max_sizeof_buffer;
  size_t m_sizeof_archive;
  size_t m_buffer_position;
  MYON__INT_PTR m_reserved1;
  MYON__INT_PTR m_reserved2;
  MYON__INT_PTR m_reserved3;
  MYON__INT_PTR m_reserved4;

private:
  // prohibit use - no implementation
  MYON_Write3dmBufferArchive(); 
  MYON_Write3dmBufferArchive( const MYON_Write3dmBufferArchive& );
  MYON_Write3dmBufferArchive& operator=(const MYON_Write3dmBufferArchive&);
};

/*
Description:
  Create a simple archive that contains a single or multiple  geometric object(s).
Parameters:
  archive - [in] destination archive.
  version - [in] (0, 2, 3, 4,50,60,70,...) format version.archive version number.
      Version 2 format can be read by Rhino 2 and Rhino 3.  Version
      3 format can be read by Rhino 3.
      Pass 0 or MYON_BinaryArchive::CurrentArchiveVersion() to write
      the latest version of archives supported by Rhino.
  object - [in] object to be saved in the archive's object table.
      This is typically some type of MYON_Curve, MYON_Surface, MYON_Mesh,
      or MYON_Brep.
	object_list - [in] objects to be saved in the archive's object table.
      These are typically some type of MYON_Curve, MYON_Surface, MYON_Mesh,
      or MYON_Brep.
	object_list_count - [in] explicit count of number of objects in object_list.
Returns:
  @untitled table
  true     archive successfully written.
  false    archive not successfully written.
Example:

          const char* filename = "myfile.3dm";
          FILE* fp = MYON::OpenFile( filename, "wb" );
          MYON_BinaryFile file( fp, MYON::archive_mode::write3dm );
          bool ok = MYON_WriteArchive( archive, geometry );
          MYON::CloseFile( fp );

Remarks:
  For MYON_WriteOneObjectArchive the  object table in the archive will contain a single
  object.
*/
MYON_DECL
bool MYON_WriteOneObjectArchive( 
  MYON_BinaryArchive& archive,
  int version,
  const MYON_Object& object
  );

MYON_DECL
bool MYON_WriteOneObjectArchive( 
  const wchar_t* filename,
  const MYON_Object& object
  );

MYON_DECL
bool MYON_WriteMultipleObjectArchive( 
  MYON_BinaryArchive& archive,
  int version,
  const MYON_SimpleArray<const MYON_Object* >& object_list
  );

MYON_DECL
bool MYON_WriteMultipleObjectArchive( 
  MYON_BinaryArchive& archive,
  int version,
  size_t object_list_count,
  const MYON_Object* const* object_list
  );

bool MYON_WriteMultipleObjectArchive( 
  const wchar_t* filename,
  int version,
  size_t object_list_count,
  const MYON_Object* const* object_list
  );


/*
Opens a debug archive file 
	Uses directory set by MYON_SetDebugWriteObjectDirectory(const wchar_t* ).
	creates a file named "debug_file_nnnn.3dm"
Example:
	MYON_DebugWriteArchive debug;
	if(debug.m_Archive)
		 MYON_WriteArchive( *debug.m_Archive, geometry );

*/
class MYON_CLASS MYON_DebugWriteArchive
{
public:
  /*
    Creates a file in N_DebugWriteObjectDirectory() and allocates archive to write to
    that file.
  */
  MYON_DebugWriteArchive();
	~MYON_DebugWriteArchive();

  // check for nullptr before using
  // Destructor closes archive and deletes it.

  MYON_BinaryArchive* Archive() const;

  // Name of the archive file.
  // = .../debug_file_NNNNN.3dm where N = Number().
  const MYON_wString& FilePath() const;

  // the number of the archive or 0 
  unsigned int Number() const;

private:
  MYON_BinaryArchive* m_archive = nullptr; 
	FILE* m_fp = nullptr;
  unsigned int m_N = 0;
  MYON_wString m_file_path;

private:
  MYON_DebugWriteArchive(const MYON_DebugWriteArchive&) = delete;
  MYON_DebugWriteArchive& operator=(const MYON_DebugWriteArchive&) = delete;
};

#endif

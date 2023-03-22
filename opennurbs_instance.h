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

#if !defined(OPENNURBS_INSTANCE_INC_)
#define OPENNURBS_INSTANCE_INC_

class MYON_CLASS MYON_ReferencedComponentSettings
{
public:
  MYON_ReferencedComponentSettings() = default;
  ~MYON_ReferencedComponentSettings();
  MYON_ReferencedComponentSettings(const MYON_ReferencedComponentSettings& src);
  MYON_ReferencedComponentSettings& operator=(const MYON_ReferencedComponentSettings& src);

  bool Read(
    MYON_BinaryArchive& archive
  );

  bool Write(
    MYON_BinaryArchive& archive
  ) const;

  bool IsEmpty() const;
  bool IsNotEmpty() const;

  bool HasLayerInformation() const;
  bool HasLayerTableInformation() const;
  bool HasParentLayerInformation() const;

  /*
  Description:
    Update runtime layer color visibility, locked, ... settings in the
    layer table read from a reference file to the values to use in the
    runtime model.
    This is typically done right after the reference file layer table is
    read and before the layers are added to the runtime model.
  Parameters:
    source_archive_manifest - [in]
      manifest of archive being read (may partially read)
    model_manifest - [in]
      manifest of runtime model (may partially created)
    layer_count - [in]
      length of layers[] array;
    layers - [in/out]
      The input values should be the layer table read from the referenced file.
      The output values have color, visibility, locked, ... settings updated
      to the state they had the last time the model file (not the referenced file)
      was saved.
    linked_definition_parent_layer - [in/out]
      If linked_definition_parent_layer is not nullptr, its color, visibility, ...
      settings are updated to the state they had the last time the model file 
      (not the referenced file) was saved.
  Remarks:
    The layer idenitification information (name, index, id) are not changed by
    this function.
  */
  void AfterReferenceLayerTableRead(
    const class MYON_ComponentManifest& source_archive_manifest,
    const class MYON_ComponentManifest& model_manifest,
    const class MYON_ManifestMap& archive_to_model_map,
    MYON_Layer* linked_definition_parent_layer,
    unsigned int layer_count,
    MYON_Layer** layers
  );

  /*
  Description:
    Update the mapping from from reference file layer id to runtime model layer id.
    Typically this is done immediately after the reference file layers are added
    to the runtime model.
  Parameters:
    source_archive_manifest - [in]
      manifest of archive being read (may partially read)
    model_manifest - [in]
      manifest of runtime model (may partially created)
    archive_to_model_map - [in]
      Manifest map from reference file settings to runtime model settings.
      This map typically exists while the archive is being read and is
      destroyed after reading is complete. That's why the mapping has
      to be saved.
  */
  void AfterLayerTableAddedToModel(
    const class MYON_ComponentManifest& source_archive_manifest,
    const class MYON_ComponentManifest& model_manifest,
    const class MYON_ManifestMap& archive_to_model_map
  );

  /*
  Description:
    Save the current runtime layer color, visibility, ... states.
    Typically this is done immediately before a linked instance definition
    or worksession reference information is written.  Calling the Write()
    function destroys the information created by BeforeWrite() because
    it is generally out-of-date if modeling resumes after writing.
  Parameters:
    model_manifest - [in]
      manifest of runtime model
    destination_archive_manifest - [in]
      manifest of archive being written (may partially written)
    model_to_archive_map - [in]
      Manifest map from model to destination_archive_manifest.
    linked_definition_parent_layer - [in]
      nullptr or the parent layer
    context - [in]
      first parameter passed to ModelLayerFromIdFunc
    ModelLayerFromIdFunc - [in]
      Function to get model layers from id
  */
  void BeforeLinkedDefinitionWrite(
    const class MYON_ComponentManifest& model_manifest,
    const class MYON_ComponentManifest& destination_archive_manifest,
    const class MYON_ManifestMap& model_to_archive_map,
    const MYON_Layer* linked_definition_parent_layer,
    void* context,
    const MYON_Layer*(*ModelLayerFromIdFunc)(void* context, const MYON_UUID&)
  );

private:
  class MYON_ReferencedComponentSettingsImpl* Impl(
    bool bCreateIfNull
    );

  class MYON_ReferencedComponentSettingsImpl* m_impl = nullptr;
};

/*
Description:
  An MYON_InstanceDefinition defines the geometry used by 
  instance references.
See Also:
  MYON_InstanceRef
*/
class MYON_CLASS MYON_InstanceDefinition : public MYON_ModelComponent
{
  MYON_OBJECT_DECLARE(MYON_InstanceDefinition);

public:

  // IDEF_UPDATE_TYPE lists the possible relationships between
  // the instance definition geometry and the archive 
  // (m_source_archive) containing the original definition.
  enum class IDEF_UPDATE_TYPE : unsigned int
  {
    Unset = 0,
    Static = 1,
    LinkedAndEmbedded = 2,
    Linked = 3


    //static_def = 0,
    //embedded_def = 1,
    //  // As of 7 February, "static_def" and "embedded_def" 
    //  // and shall be treated the same. Using "static_def"
    //  // is preferred and "embedded_def" is obsolete.
    //  // The geometry for the instance definition
    //  // is saved in archives, is fixed and has no
    //  // connection to a source archive.
    //  // All source archive information should be
    //  // empty strings and m_source_archive_checksum
    //  // should be "zero".
    //linked_and_embedded_def = 2,
    //  // The geometry for the instance definition
    //  // is saved in archives.  Complete source
    //  // archive and checksum information will be 
    //  // present. The document setting 
    //  // MYON_3dmIOSettings.m_idef_link_update 
    //  // determines if, when and how the instance
    //  // definition geometry is updated by reading the
    //  // source archive.
    //linked_def = 3,   
    //  // The geometry for this instance definition
    //  // is not saved in the archive that contains
    //  // this instance definition. This instance 
    //  // definition geometry is imported from a
    //  // "source archive" The "source archive" file
    //  // name and checksum information are saved
    //  // in m_source_archive and m_source_archive_checksum.
    //  // If file named in m_source_archive is not available, 
    //  // then this instance definition is not valid and any
    //  // references to it are not valid.
  };

  // Converts and integer into an IDEF_UPDATE_TYPE enum.
  static MYON_InstanceDefinition::IDEF_UPDATE_TYPE InstanceDefinitionTypeFromUnsigned(
    unsigned int idef_type_as_unsigned
    );

  // Bits that identify subsets of the instance definition
  // fields. These bits are used to determine which fields to
  // set when an MYON_InstanceDefinition class is used to
  // modify an existing instance definition.
  enum
  {
    no_idef_settings            =    0,
    idef_name_setting           =    1,  // m_name
    idef_description_setting    =    2,  // m_description
    idef_url_setting            =    4,  // all m_url_* fields
    idef_units_setting          =    8,  // m_us and m_unit_scale
    idef_source_archive_setting = 0x10,  // all m_source_*, layer style, update depth fields
    idef_userdata_setting       = 0x20, 
    all_idef_settings           = 0xFFFFFFFF
  };

public:
  MYON_InstanceDefinition() MYON_NOEXCEPT;
  ~MYON_InstanceDefinition();
  MYON_InstanceDefinition(const MYON_InstanceDefinition&);
  MYON_InstanceDefinition& operator=(const MYON_InstanceDefinition&);

private:
  void Internal_Destroy();
  void Internal_Copy(const MYON_InstanceDefinition& src);

public:
  
  static const MYON_InstanceDefinition Unset;

  /*
  Parameters:
    model_component_reference - [in]
    none_return_value - [in]
      value to return if MYON_InstanceDefinition::Cast(model_component_ref.ModelComponent())
      is nullptr
  Returns:
    If MYON_InstanceDefinition::Cast(model_component_ref.ModelComponent()) is not nullptr,
    that pointer is returned.  Otherwise, none_return_value is returned. 
  */
  static const MYON_InstanceDefinition* FromModelComponentRef(
    const class MYON_ModelComponentReference& model_component_reference,
    const MYON_InstanceDefinition* none_return_value
    );


  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;

  // virtual MYON_Object::Dump override
  void Dump(
    MYON_TextLog& text_log
    ) const override;

public:
  bool Write(
    MYON_BinaryArchive& archive
    ) const override;

private:
  bool Internal_WriteV5(
    MYON_BinaryArchive& archive
    ) const;
  bool Internal_WriteV6(
    MYON_BinaryArchive& archive
    ) const;

public:
  bool Read(
    MYON_BinaryArchive& archive
    ) override;

private:
  bool Internal_ReadV5(
    MYON_BinaryArchive& archive
    );
  bool Internal_ReadV6(
    MYON_BinaryArchive& archive
    );

public:
  MYON::object_type ObjectType() const override;

  // virtual MYON_Object:: override
  unsigned int SizeOf() const override;

  const MYON_BoundingBox BoundingBox() const;
  
  void SetBoundingBox( MYON_BoundingBox bbox );
  
  void ClearBoundingBox();

  const MYON_wString Description() const;
  void SetDescription( const wchar_t* description );

  const MYON_wString URL() const;
  void SetURL( const wchar_t* url );

  const MYON_wString URL_Tag() const;
  void SetURL_Tag( const wchar_t* url_tag );

  /*
  Returns:
    A list of object ids in the instance geometry table sorted by id.
  */
  const MYON_SimpleArray<MYON_UUID>& InstanceGeometryIdList() const;

  /*
  Parameters:
    instance_geometry_id_list - [in]
       A list of object ids in the instance geometry table.
  */
  void SetInstanceGeometryIdList(
    const MYON_SimpleArray<MYON_UUID>& instance_geometry_id_list
  );

  /*
  Description:
    Remove all ids from the InstanceGeometryIdList().
  */
  void ClearInstanceGeometryIdList();

  /*
  Description:
    Remove id from the InstanceGeometryIdList().
  */
  bool RemoveInstanceGeometryId(
    MYON_UUID id
  );

  /*
  Description:
    Remove InstanceGeometryIdList()[id_index] from the InstanceGeometryIdList() array.
  */
  bool RemoveInstanceGeometryId(
    int id_index
  );

  /*
  Description:
    Add id to the InstanceGeometryIdList().
  Parameters:
    id - [in]
      non-nil id to add.
  Returns:
    True if id is not nil and was added to the InstanceGeometryIdList().
  */
  bool AddInstanceGeometryId(
    MYON_UUID id
  );

  /*
  Returns:
    True if id is in the InstanceGeometryIdList().
  */
  bool IsInstanceGeometryId(
    MYON_UUID id
  ) const;

private:
  int Internal_InstanceGeometryIdIndex(
    MYON_UUID id
  ) const;

public:
  /*
  Parameters:
    instance_definition_type - [in]
      MYON_InstanceDefinition::IDEF_UPDATE_TYPE::Unset - change the type to Unset 
        and remove all linked file information.
      MYON_InstanceDefinition::IDEF_UPDATE_TYPE::Static - change the type to Static 
        and remove all linked file information.
      MYON_InstanceDefinition::IDEF_UPDATE_TYPE::LinkedAndEmbedded - change 
        the type to from Linked to LinkedAndEmbedded.  If the current type
        is not Linked, then no changes are made.
      MYON_InstanceDefinition::IDEF_UPDATE_TYPE::Linked - change 
        the type to from LinkedAndEmbedded to Linked.  If the current type
        is not LinkedAndEmbedded, then no changes are made.
  */
  bool SetInstanceDefinitionType(
    const MYON_InstanceDefinition::IDEF_UPDATE_TYPE instance_definition_type
    );

  /*
  Parameters:
    linked_definition_type - [in]
      Either MYON_InstanceDefinition::IDEF_UPDATE_TYPE::LinkedAndEmbedded
      or MYON_InstanceDefinition::IDEF_UPDATE_TYPE::Linked.
    linked_file_reference - [in]
  */
  bool SetLinkedFileReference(
    MYON_InstanceDefinition::IDEF_UPDATE_TYPE linked_definition_type,
    MYON_FileReference linked_file_reference
    );

  bool SetLinkedFileReference(
    MYON_InstanceDefinition::IDEF_UPDATE_TYPE linked_definition_type,
    const wchar_t* linked_file_full_path
    );

  const MYON_FileReference LinkedFileReference() const;

  /*
    Destroy all linked file path information and convert the type to Static.
  */
  void ClearLinkedFileReference();

  void ClearLinkedFileContentHash();

  void ClearLinkedFileRelativePath();
  
  const MYON_wString& LinkedFilePath() const;
  
  const MYON_UnitSystem& UnitSystem() const;

public:
  /*
  Description:
    Sets m_us and m_unit_scale.
  */
  void SetUnitSystem( MYON::LengthUnitSystem us );
  void SetUnitSystem( const MYON_UnitSystem& us );

  /*
  Returns:
    True if this is a linked instance definition with
    layer settings information.
  */
  bool HasLinkedIdefReferenceComponentSettings() const;

  void ClearLinkedIdefReferenceComponentSettings();

  /*
  Parameters:
    bCreateIfNonePresent - [in]
      When bCreateIfNonePresent is true and the idef type is MYON_InstanceDefinition::IDEF_UPDATE_TYPE::Linked,
      then MYON_ReferencedComponentSettings will be created if none are present.
  Return:
    MYON_ReferencedComponentSettings pointer or nullptr.
  */
  const MYON_ReferencedComponentSettings* LinkedIdefReferenceComponentSettings() const;

  /*
  Parameters:
    bCreateIfNonePresent - [in]
      When bCreateIfNonePresent is true and the idef type is MYON_InstanceDefinition::IDEF_UPDATE_TYPE::Linked,
      then MYON_ReferencedComponentSettings will be created if none are present.
  Return:
    MYON_ReferencedComponentSettings pointer or nullptr.
  */
  MYON_ReferencedComponentSettings* LinkedIdefReferenceComponentSettings(
    bool bCreateIfNonePresent
  );

public:

  // OBSOLETE - change IdefUpdateType() to InstanceDefinitionType()
  MYON_InstanceDefinition::IDEF_UPDATE_TYPE IdefUpdateType() const;

  MYON_InstanceDefinition::IDEF_UPDATE_TYPE InstanceDefinitionType() const;

  /*
  Returns:
    true if InstanceDefinitionType() = MYON_InstanceDefinition::IDEF_UPDATE_TYPE::Linked or MYON_InstanceDefinition::IDEF_UPDATE_TYPE::LinkedAndEmbedded.
  */
  bool IsLinkedType() const;

  /*
  Description:
    This property applies when an instance definition is linked.
  Returns:
    true:
      When reading the file that defines the content of the linked instance definition,
      skip any linked instance definitions found in that file.
    false:
      When reading the file that defines the content of the linked instance definition,
      recursively load linked instance definitions found in that file.
  */
  bool SkipNestedLinkedDefinitions() const;

  void SetSkipNestedLinkedDefinitions(
    bool bSkipNestedLinkedDefinitions
  );

private:
  // list of object ids in the instance geometry table.
  MYON_SimpleArray<MYON_UUID> m_object_uuid;

private:
  MYON_wString m_description; 
  MYON_wString m_url;
  MYON_wString m_url_tag;     // UI link text for m_url

private:
  MYON_BoundingBox m_bbox = MYON_BoundingBox::EmptyBoundingBox;

private:
  MYON_UnitSystem  m_us = MYON_UnitSystem::None;
  
private:
  // Note: the embedded_def type is obsolete.
  //  To avoid having to deal with this obsolete type in
  //  your code, using MYON_InstanceDefintion::IdefUpdateType()
  //  to get this value.  The IdefUpdateType() function
  //  with convert the obsolete value to the correct
  //  value.
  MYON_InstanceDefinition::IDEF_UPDATE_TYPE m_idef_update_type = MYON_InstanceDefinition::IDEF_UPDATE_TYPE::Static; 

private:
  bool m_bSkipNestedLinkedDefinitions = false;

private:
  /////////////////////////////////////////////////////////////
  //
  // linked instance definition internals
  //
private:
  MYON_FileReference m_linked_file_reference;

  // For V5 3dm archive compatibility.
  // Set as needed by the Write() function for new idefs and saved if the idef is read from a V5 file.
private:
  mutable MYON_CheckSum m_linked_file_V5_checksum = MYON_CheckSum::UnsetCheckSum;
private:
  bool Internal_SetLinkedFileReference(
    MYON_InstanceDefinition::IDEF_UPDATE_TYPE linked_definition_type,
    const MYON_FileReference& linked_file_reference,
    MYON_CheckSum V5_checksum
    );

  // See comment for Internal_ReferencedComponentSettings() function.
private:
  mutable class MYON_ReferencedComponentSettings* m_linked_idef_component_settings = nullptr;

public:
  
  /// <summary>
  /// MYON_InstanceDefinition::LinkedComponentStates specifies how model components
  /// (layers, materials, dimension styles, ...) from linked instance definition files
  /// are appear in the active model.
  /// </summary>  
  enum class eLinkedComponentAppearance : unsigned char
  {
    ///<summary>
    /// This is the only valid layer style when the instance definition type is
    /// MYON_InstanceDefinition::IDEF_UPDATE_TYPE::Static or
    /// MYON_InstanceDefinition::IDEF_UPDATE_TYPE::LinkedAndEmbedded.
    /// This style is not valid when the instance definition type
    /// MYON_InstanceDefinition::IDEF_UPDATE_TYPE::Linked.
    ///</summary>
    Unset = 0,

    ///<summary>
    /// Model components (layers, materials, dimension styles, ...) from 
    /// linked instance definition files are embedded as ordinary components 
    /// in the active model.
    /// This layer style may be used when the instance definition type is
    /// MYON_InstanceDefinition::IDEF_UPDATE_TYPE::Linked.
    ///</summary>
    Active = 1,

    ///<summary>
    /// Layers from the linked instance definition are reference components in the model.
    /// This is the default layer style when the instance definition type is
    /// MYON_InstanceDefinition::IDEF_UPDATE_TYPE::Linked.
    /// This layer style may be used when the instance definition type is
    /// MYON_InstanceDefinition::IDEF_UPDATE_TYPE::Linked.
    ///</summary>
    Reference = 2
  };

  static MYON_InstanceDefinition::eLinkedComponentAppearance LinkedComponentAppearanceFromUnsigned(
    unsigned int linked_component_appearance_as_unsigned
    );

  MYON_InstanceDefinition::eLinkedComponentAppearance LinkedComponentAppearance() const;

  bool SetLinkedComponentAppearance(
    MYON_InstanceDefinition::eLinkedComponentAppearance linked_component_appearance
    );
  
private:
  MYON_InstanceDefinition::eLinkedComponentAppearance m_linked_component_appearance = MYON_InstanceDefinition::eLinkedComponentAppearance::Unset;
                               
public:

  /*
  Returns:
    A SHA-1 hash of these instance definitions properties:   

    InstanceGeometryIdList()
    BoundingBox()
    UnitSystem()
    InstanceDefinitionType()
    LinkedFileReference()
    LinkedComponentAppearance()
  */    
  const MYON_SHA1_Hash GeometryContentHash() const;

  /*
  Returns:
    A SHA-1 hash of these instance definitions properties
    Description()
    URL()
    URL_Tag()
    and all the properties that contribute to the GeometryContentHash().
  */    
  const MYON_SHA1_Hash ContentHash() const;

private:
  void Internal_AccumulateHash() const;

private:
  // Internal_AccumulateHash() uses lazy evaluation to set m_geometry_content_hash when needed.
  mutable MYON_SHA1_Hash m_geometry_content_hash = MYON_SHA1_Hash::ZeroDigest;

  // Internal_AccumulateHash() uses lazy evaluation to set m_content_hash when needed.
  mutable MYON_SHA1_Hash m_content_hash = MYON_SHA1_Hash::ZeroDigest;

private:
  // Increments content version number and sets hashes to MYON_SHA1_Hash::ZeroDigest.
  void Internal_ContentChanged();

private:
  unsigned char m_reserved2A = 0;
  unsigned char m_reserved2B = 0;
  unsigned char m_reserved2C = 0;

private:
  unsigned int m_reserved1 = 0;

private:
  MYON__UINT_PTR m_reserved_ptr = 0;
};

#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_InstanceDefinition*>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_ObjectArray<MYON_InstanceDefinition>;
#endif

/*
Description:
  An MYON_InstanceRef is a reference to an instance definition
  along with transformation to apply to the definition.
See Also:
  MYON_InstanceRef
*/
class MYON_CLASS MYON_InstanceRef : public MYON_Geometry
{
  MYON_OBJECT_DECLARE(MYON_InstanceRef);

public:
  MYON_InstanceRef() = default;
  ~MYON_InstanceRef() = default;
  MYON_InstanceRef(const MYON_InstanceRef&) = default;
  MYON_InstanceRef& operator=(const MYON_InstanceRef&) = default;

public:
  /////////////////////////////////////////////////////////////
  //
  // virtual MYON_Object overrides
  //
  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;
  bool Write(
         MYON_BinaryArchive& binary_archive
       ) const override;
  bool Read(
         MYON_BinaryArchive& binary_archive
       ) override;
  MYON::object_type ObjectType() const override;

  /////////////////////////////////////////////////////////////
  //
  // virtual MYON_Geometry overrides
  //
  int Dimension() const override;

  // virtual MYON_Geometry GetBBox override		
  bool GetBBox( double* boxmin, double* boxmax, bool bGrowBox = false ) const override;

  bool Transform( 
         const MYON_Xform& xform
         ) override;

  // virtual MYON_Geometry::IsDeformable() override
  bool IsDeformable() const override;

  // virtual MYON_Geometry::MakeDeformable() override
  bool MakeDeformable() override;

  /////////////////////////////////////////////////////////////
  //

  // Unique id of the instance definition (MYON_InstanceDefinition) 
  // in the instance definition table that defines the geometry
  // used by this reference.
  MYON_UUID m_instance_definition_uuid = MYON_nil_uuid;

  // Transformation for this reference.
  MYON_Xform m_xform = MYON_Xform::IdentityTransformation;

  // Bounding box for this reference.
  MYON_BoundingBox m_bbox;

#if 0
public:
  /*
  Remove all reference to the nested linked idef information.
  */
  void ClearReferenceToNestedLinkedIdef();

  /*
  Returns:
    true
      if input was valid and the reference to the nested linked idef was set.
    false 
      if reference to the nested linked idef was not set.
  */
  bool SetReferenceToNestedLinkedIdef(
    const MYON_UUID& parent_idef_uuid,
    const MYON_FileReference& parent_reference_file,
    const MYON_FileReference& nested_reference_file
  );

  /*
  Parameters:
    parent_idef_uuid - [in]
      The persistent id of the parent idef that contains the (possibly deeply nested)
      instance definion this reference refers to.
    parent_reference_file - [in]
      the file for the parent idef.
    nested_reference_file - [in]
      if the referenced idef is itself linked, nested_reference_file identifies 
      the file.

  Returns:
    True if this is a reference to a nested linked idef.
  */
  bool GetReferenceToNestedLinkedIdef(
    MYON_UUID& parent_idef_uuid,
    MYON_FileReference& parent_reference_file,
    MYON_FileReference& nested_reference_file
  ) const;

  /*
  Returns:
    True if this is a reference to a nested linked idef.
  */
  bool ContainsReferenceToNestedLinkedIdef() const;

private:
  /////////////////////////////////////////////////////////////
  //
  // Additional information used when this reference is to
  // an instance definition that is nested inside an ordinary
  // linked instance definition.
  //
  // For example, if 
  // idefA = linked instance definition referencing file A.
  // idefX = any type of instance definition found in idefA.
  // 
  // iref = model geometry reference to idefX.
  //
  // When A is not a 3dm file or the 3dm id of idefX is
  // in use in the current model, the id of idefX will change
  // every time A is read. This means saving the value of
  // iref.m_instance_definition_uuid is not sufficient to identify
  // idefX. In this case, the additional information
  //
  //  iref.m_bReferenceToNestedLinkedIdef = true
  //  iref.m_parent_idef_uuid = idefA.Id()
  //  iref.m_parent_reference_file = idefA.FileReference().
  //  iref.m_nested_reference_file = idefX.FileReference().
  // 
  // is used to identify idefX in a persistent way.
  //
  bool m_bReferenceToNestedLinkedIdef = false;
  MYON_UUID m_parent_idef_uuid = MYON_nil_uuid; // persistent id
  MYON_FileReference m_parent_reference_file = MYON_FileReference::Unset;
  MYON_FileReference m_nested_reference_file = MYON_FileReference::Unset;
#endif

public:
  // Tolerance to use for flagging instance xforms
  // as singular.
  // A valid MYON_InstanceRef.m_xform satisfies:
  //   true == (m_xform.Inverse()*m_xform).IsIdentity(MYON_InstanceRef::SingularTransformationTolerance)
  static const double SingularTransformationTolerance;
};

#endif

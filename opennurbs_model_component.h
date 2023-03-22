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

#if !defined(MYON_MODEL_COMPONENT_INC_)
#define MYON_MODEL_COMPONENT_INC_

/*
Description:
  The MYON_ModelComponent class is a base class for all components in a
  model and manages the index, id and other information common to all
  components in a model.
*/
class MYON_CLASS MYON_ModelComponent : public MYON_Object
{
  MYON_OBJECT_DECLARE(MYON_ModelComponent);

public:
  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;

  void Dump( 
    MYON_TextLog& 
    ) const override;

  unsigned int SizeOf() const override;


  MYON__UINT32 DataCRC(
    MYON__UINT32 current_remainder
    ) const override;

  MYON_UUID ModelObjectId() const override;

public:

#pragma region RH_C_SHARED_ENUM [MYON_ModelComponent::Type] [Rhino.DocObjects.ModelComponentType] [public:byte]
  ///<summary>
  ///The MYON_ModelComponent::Type enum has a value for each explicit component type
  ///and two special values, Unset and Mixed. Use an MYON_ModelComponentTypeIterator
  ///instance to iterate over the MYON_ModelComponent::Type values.
  ///</summary>
  enum class Type : unsigned char
  {
    ///<summary>No value set.</summary>
    Unset = 0,
    ///<summary>Embedded image (bitmap) components.</summary>
    Image = 1,
    ///<summary>Texture mapping.</summary>
    TextureMapping = 2,
    ///<summary>Material.</summary>
    Material = 3,
    ///<summary>Render material. Deprecated in favor of Material.</summary>
    RenderMaterial = Material,
    ///<summary>Line pattern (linetype).</summary>
    LinePattern = 4,
    ///<summary>Layer.</summary>
    Layer = 5,
    ///<summary>Group.</summary>
    Group = 6,
    ///<summary>Text style.</summary>
    TextStyle = 7,
    ///<summary>AnnotationDimension style.</summary>
    DimStyle = 8,
    ///<summary>Render light.</summary>
    RenderLight = 9,
    ///<summary>Hatch pattern.</summary>
    HatchPattern = 10,
    ///<summary>Instance definition.</summary>
    InstanceDefinition = 11,
    ///<summary>Model objects (points, curves, surfaces, ..., annotation, page details, ...), including objects referenced by instance definitions.</summary>
    ModelGeometry = 12,
    ///<summary>History record.</summary>
    HistoryRecord = 13,
    ///<summary>Render content.</summary>
    RenderContent = 14,
    ///<summary>Embedded file for render texture.</summary>
    EmbeddedFile = 15,
    ///<summary>Post Effect.</summary>
    PostEffect = 16,

    // If you add any more, add them here, above NumOf.

    ///<summary>Number of items above.</summary>
    NumOf,

    ///<summary>Multiple component types. Used when a component type filter must include all explicit component types.</summary>
    Mixed = 0xFE
  };
#pragma endregion

  static MYON_ModelComponent::Type ComponentTypeFromUnsigned(
    unsigned int component_type_as_unsigned
    );

  static const MYON_wString ComponentTypeToString(
    MYON_ModelComponent::Type
    );

  /*
  Parameters:
  component_type - [in]
  Returns:
    True if component_type is a valid enum value, 
    and not equal to MYON_ModelComponent::Type::Unset.
  */
  static bool ComponentTypeIsValid(
    MYON_ModelComponent::Type component_type
    );

  /*
  Parameters:
    component_type - [in]
  Returns:
    True if component_type is a valid enum value, 
    and not equal to MYON_ModelComponent::Type::Unset, 
    and not equal to MYON_ModelComponent::Type::Mixed.
  */
  static bool ComponentTypeIsValidAndNotMixed(
    MYON_ModelComponent::Type component_type
    );

  /*
  Parameters:
    component_type - [in]
  Returns:
    True if component's of the specified type require uniquen names.
  Remarks:
    In general, component names ignore case and parent when testing for
    equality. 
    However, there are exceptions. Use
    MYON_ModelComponent::UniqueNameIncludesParent() and
    MYON_ModelComponent::UniqueNameIgnoresCase() 
    to test for exceptions.
  */
  static bool UniqueNameRequired(
    MYON_ModelComponent::Type component_type
    );

  /*
  Parameters:
    component_type - [in]
  Returns:
    True if objects with the same name and different parents are considered
    to have a unique name. This typically means the user interface for the
    object names is some type of tree.
  Remarks:
    Currently, layers are the only object type where this property is true.
    This function should be called "NameIncludesParent" because it 
    also applies to components like materials and geometry objects
    that are not required to have a unique name.
  */
  static bool UniqueNameIncludesParent(
    MYON_ModelComponent::Type component_type
    );

  /*
  Parameters:
    component_type - [in]
  Returns:
    True if component name ignores case when testing for equality.
  Remarks:
    Currently all other component types except for groups ignore 
    case when testing for equality.
    This function should be called "NameIgnoresCase" because it 
    also applies to components like materials and geometry objects
    that are not required to have a unique name.
  */
  static bool UniqueNameIgnoresCase(
    MYON_ModelComponent::Type component_type
    );

  static bool IndexRequired(
    MYON_ModelComponent::Type component_type
    );

  /*
  Returns:
    True if a componenent name can begin with the first code point.
  Remarks:
    Component names cannot begin with a (, ), [, ], {, }, or space.
    These brackets can be the second or later code points.
    A space can be an interior code point.
  */
  static bool IsValidComponentNameFirstCodePoint(
    MYON__UINT32 unicode_code_point
    );

  /*
  Parameters:
    length - [in]
      number of wchar_t elements in candidate_component_name[] or -1
      if it is a null terminated string.
    candidate_component_name - [in]
      The first code point is decoded from this string
      and passed to MYON_ModelComponent::IsValidComponentNameFirstCodePoint().
  Returns:
    True if a componenent name can begin with the first code point.
  Remarks:
    Component names cannot begin with a (, ), [, ], {, }, or space.
    These brackets can be the second or later code points.
    A space can be an interior code point.
  */
  static bool IsValidComponentNameFirstCodePoint(
    int length,
    const wchar_t* candidate_component_name 
    );

  /*
  Parameters:
    candidate_component_name - [in]
      A null terminated string that is UTF-16 or UTF-32 encoded.
      When sizeof(wchar_t) >= 4, both UTF-16 surrogate pairs
      and UTF-32 code point values > 0xFFFF are valid and can
      appear in the same string.
  Returns:
    True if candidate_component_name is a valid component name.
  Remarks:
    Component names cannot begin with a (, ), [, ], {, }, or space.
    These brackets can be the second or later code points.
    A space can be an interior code point.
  */
  static bool IsValidComponentName(
    const wchar_t* candidate_component_name 
    );

  static bool IsValidComponentName(
    const MYON_wString& candidate_component_name
    );

  /*
  Parameters:
    length - [in]
      > 0.
      The expected number of elements before the null terminator in candidate_component_name[].
    candidate_component_name - [in]
      A null terminated string that is UTF-16 or UTF-32 encoded.
      When sizeof(wchar_t) >= 4, both UTF-16 surrogate pairs
      and UTF-32 code point values > 0xFFFF are valid and can
      appear in the same string.
      The capacity of candidate_component_name[] must be >= length+1.
      This validity test fails if candidate_component_name[length] is not zero
      or if a null terminator appears before candidate_component_name[length].
  Returns:
    True if candidate_component_name is a valid component name.
  Remarks:
    Component names cannot begin with a (, ), [, ], {, }, or space.
    These brackets can be the second or later code points.
    A space can be an interior code point.
  */

  static bool IsValidComponentName(
    size_t length,
    const wchar_t* candidate_component_name 
    );

  /*
  Parameters:
    model_component - [in]
    model_manfest - [in]
      current model contents
    valid_name - [in]
      Valid name for this component in a model with the specified manifest.
  Returns:
    true: model_component.Name() is valid
    false: model_component.Name() is not valid    
  */
  static bool IsValidComponentName(
    const class MYON_ComponentManifest&  model_manfest,
    const MYON_ModelComponent& model_component,
    bool bPermitReferencePrefix,
    MYON_wString& valid_name
  );

private:
  friend class CRhinoObject;

  MYON_ModelComponent(MYON__UINT64 runtime_serial_number) MYON_NOEXCEPT;

public:
  static const MYON_ModelComponent Unset;

  MYON_ModelComponent() MYON_NOEXCEPT;

  ~MYON_ModelComponent() = default;
  
  // Locked status of source attributes is not copied to permit copy and modify operations.
  // Runtime serial number and content version number are not copied.
  MYON_ModelComponent(
    const MYON_ModelComponent& source
    );

  /*
  Description:
    Sets and locks the component type attribute.
  Parameters:
    component_type - [in]
  */
  MYON_ModelComponent(
    MYON_ModelComponent::Type component_type
    ) MYON_NOEXCEPT;

  // Locked status of source attributes is not copied to permit copy and modify operations.
  MYON_ModelComponent(
    MYON_ModelComponent::Type component_type,
    const MYON_ModelComponent& source
    ) MYON_NOEXCEPT;
  
  /*
  Remarks:
    operator= will not change locked destination attributes.
    Locked status of source attributes is not copied to permit copy and modify operations.
  */
  MYON_ModelComponent& operator=(const MYON_ModelComponent& source);

  /*
  The MYON_ModelComponent::Attributes enum is used to set bits that identify
  model component attributes.
  */
  enum Attributes : unsigned int
  {
    NoAttributes = 0U,

    ComponentSerialNumberAttribute = 0x0001U,
    TypeAttribute = 0x0002U,
    ModelSerialNumberAttribute = 0x0004U,
    IdAttribute = 0x0008U,
    IndexAttribute = 0x0010U,
    NameAttribute = 0x0020U,
    ParentIdAttribute = 0x0040U,
    DeletedNameAttribute = 0x0080U,
    ComponentStatusAttribute = 0x0100U,

    // Default model attributes saved in binary archives.
    // Id, ParentId, Name, Index
    // Do not include SystemComponentAttribute in BinaryArchiveAttributes
    BinaryArchiveAttributes = 0x78U,

    // Do not include SystemComponentAttribute in AllAttributes
    AllAttributes = 0x1FFU,

    SystemComponentAttribute = 0x8000U
  };

  unsigned int CopyFrom(
    const MYON_ModelComponent& src,
    unsigned int attributes_filter
    );

  bool WriteModelComponentAttributes(
    class MYON_BinaryArchive& archive,
    unsigned int attributes_filter
    ) const;

  bool ReadModelComponentAttributes(
    class MYON_BinaryArchive& archive
    );
  
  ///////////////////////////////////////////////////////////////////////////////
  //
  // Model Serial Number interface
  //
  //  The ModelSerialNumber() identifies the model that manages the component.
  //  If the component is from some type of reference model, then one or both of
  //  the reference model serial number and InstanceDefinitionModelSerialNumber()
  //  identify the source model.
  //

  /*
  Description:
    Specify the model that is managing this component.
  Parameters:
    model_serial_number - [in]
      In Rhino, this is the document runtime serial number.
  Returns:
    true if the model_serial_number value was changed to component_model_serial_number or is already equal to component_model_serial_number.
    false if the model_serial_number value is locked and component_model_serial_number != locked value.
  */
  bool SetModelSerialNumber(
    unsigned int model_serial_number
    );

  /*
  Description:
    Specify the model that is managing this component.
  Parameters:
    model_serial_number - [in]
      In Rhino, this is the document runtime serial number.
    reference_model_serial_number - [in]
      In Rhino, this is the worksession reference model serial number.
      instance_definition_model_serial_number - [in]
      In Rhino, this is the linked instance definition model serial number.
  Returns:
    true if the model_serial_number value was changed to component_model_serial_number or is already equal to component_model_serial_number.
    false if the model_serial_number value is locked and component_model_serial_number != locked value.
  */
  bool SetModelSerialNumber(
    unsigned int model_serial_number,
    unsigned int reference_model_serial_number,
    unsigned int instance_definition_model_serial_number
    );

  /*
  Returns:
    A value identifying the model that manages this component.
  Remarks:
    If the component is being managed by a model, this value identifies the model.
    In Rhino, this value is the document runtime serial number.
    Typically this value is set and locked by the code that adds 
    a component to a model.
    This value is copied by the copy constructor and operator=. 
    This value is not saved in .3dm archives.
  */
  unsigned int ModelSerialNumber() const;

  /*
  Returns:
    When a component is in a model for reference, this value identifies the
    reference model.
  Remarks:
    Reference components are not saved in .3dm archives.
    Typically this value is set and locked by the code that adds 
    a component to a model.
    This value is copied by the copy constructor and operator=. 
    This value is not saved in .3dm archives.

    In Rhino, this value is a worksession model runtime serial number and 
    these values are used.
           0: not from a worksession reference model
           1: from an unidentified reference model
      2-1000: reserved for future use
       >1000: worksession reference model serial number
  */
  unsigned int ReferenceModelSerialNumber() const;

  /*
  Returns:
    When a component is in a model as part of the information required
    for a linked instance definition, this value identifies the the linked
    instance definition reference model.
  Remarks:
    Reference components are not saved in .3dm archives.
    Typically this value is set and locked by the code that adds 
    a component to a model.
    This value is copied by the copy constructor and operator=. 
    This value is not saved in .3dm archives.

    In Rhino, this value is a linked instance definition runtime serial number
    and  these values are used..
            0: Active model component.
       1-1000: reserved for future use
        >1000: linked instance definition serial number
  */
  unsigned int InstanceDefinitionModelSerialNumber() const;

  /*
  Description:
    Sets the model serial number, reference model serial number and instance
    definition model serial number values to 0 and flags these values
    as unset.
  */
  bool ClearModelSerialNumber();

  /*
  Description:
    Calling LockModelSerialNumber() will prohibit future changes to the 
    ModelSerialNumber(), ReferenceModelSerialNumber(), and
    InstanceDefinitionModelSerialNumber() attributes.
  */
  void LockModelSerialNumber();

  /*
  Returns:
    True if the ModelSerialNumber(), ReferenceModelSerialNumber(), and
    InstanceDefinitionModelSerialNumber() attributes are locked.
  */
  bool ModelSerialNumberIsLocked() const;

  /*
  Returns:
    True if the ModelSerialNumber(), ReferenceModelSerialNumber(), and
    InstanceDefinitionModelSerialNumber() attributes are set.
  */
  bool ModelSerialNumberIsSet() const;

  /*
  Returns 
    True if ReferenceModelSerialNumber() or InstanceDefinitionModelSerialNumber() are not zero.
  */
  bool IsReferenceComponent() const;

  ///////////////////////////////////////////////////////////////////////////////
  //
  // Type interface
  //

  /*
  Returns:
    Value of the Type attribute.
  Remarks:
    If the component is in a model, then the component_type is unique for all components
    of identical type in the model and is locked.
  */
  MYON_ModelComponent::Type ComponentType() const;

  /*
  Parameters:
    component_component_type - [in]
  Returns:
    true if the component_type value was changed to component_component_type or is already equal to component_component_type.
    false if the component_type value is locked and component_component_type != locked value.
  */
  bool SetComponentType(
    MYON_ModelComponent::Type component_component_type
    );

  /*
  Description:
    Set the model serial number value to the MYON_ModelComponent::Unset.Type()
    and change the state so TypeIsSet() returns false.
  */
  bool ClearComponentType();

  /*
  Description:
    Calling LockType() will prhibit future changes to the Type attribute.
  */
  void LockComponentType();

  /*
  Returns:
    True if the Type attribute is locked.
  */
  bool ComponentTypeIsLocked() const;

  /*
  Returns:
    True if the Type attribute is set.
  */
  bool ComponentTypeIsSet() const;

  ///////////////////////////////////////////////////////////////////////////////
  //
  // Id interface
  //
  //  The component id is intended to be unique in the context of a model.
  //

  /*
  Returns:
    Value of the Id attribute.
  Remarks:
    If the component is in a model, then the id is unique for all components in
    the model and is locked.
  */
  const MYON_UUID& Id() const;

  bool IdIsNil() const;
  bool IdIsNotNil() const;

  /*
  Parameters:
    component_id - [in]
  Returns:
    true if the id value was changed to component_id or is already equal to component_id.
    false if the id value is locked and component_id != locked value.
  */
  bool SetId(
    const MYON_UUID& component_id
    );

  /*
  Description:
    Sets the id to a new value created by MYON_CreateUuid().
  Returns:
    MYON_nil_uuid: Id is locked.
    Otherwise the value of the new id.
  */
  const MYON_UUID& SetId();

  /*
  Description:
    If the it is nil, call SetId().
  */
  const MYON_UUID& IfIdIsNilSetId();

  /*
  Description:
    Sets the id to a new value created by MYON_CreateUuid() and locks the 
    id attribute so it cannot be changed.
  Returns:
    MYON_nil_uuid: Id is locked.
    Otherwise the value of the new id.
  */  
  const MYON_UUID& SetAndLockId();
  

  /*
  Description:
    Set the component id value to the MYON_ModelComponent::Unset.ModelComponentId()
    and change the state so ModelComponentIdIsSet() returns false.
  */
  bool ClearId();

  /*
  Description:
    Calling LockModelComponentId() will prhibit future changes to the ModelComponentId attribute.
  */
  void LockId();

  /*
  Returns:
    True if the ModelComponentId attribute is locked.
  */
  bool IdIsLocked() const;

  /*
  Returns:
    True if the ModelComponentId attribute is set.
  */
  bool IdIsSet() const;


  ///////////////////////////////////////////////////////////////////////////////
  //
  // Index interface
  //
  //  When a component is managed by a model, the component index is identifies
  //  the component within the context of that model.
  //

  /*
  Returns:
    Value of the runtime model component index attribute.
  Remarks:
    If the component is in a model, then the index is unique for all components
    of identical type in the model and is locked.
    If the index has not been set, MYON_UNSET_INT_INDEX is returned.
    The Index() value can change when saved in an archive (.3dm file).
    Use the Id() when you need to reference model compoenents in an archive.
  */
  int Index() const;

  /*
  Returns:
    Value of the model component index attribute.
  Parameters:
    unset_index_value - [in]
      Value to return if the index has not been set.
      MYON_UNSET_INT_INDEX or indices of default components are often
      used for this parameter.
  Returns:
    IndexIsSet() ? Index() : unset_index_value;
  Remarks:
    If the component is in a model, then the index is unique for all components
    of identical type in the model and is locked.
  */
  int Index(
    int unset_index_value
    ) const;

  /*
  Parameters:
    component_index - [in]
  Returns:
    true if the index value was changed to component_index or is already equal to component_index.
    false if the index value is locked and component_index != locked value.
  */
  bool SetIndex(
    int component_index
    );

  /*
  Description:
    Set the component index value to the MYON_ModelComponent::Unset.ModelComponentIndex()
    and change the state so ModelComponentIndexIsSet() returns false.
  */
  bool ClearIndex();

  /*
  Description:
    Calling LockModelComponentIndex() will prhibit future changes to the ModelComponentIndex attribute.
  */
  void LockIndex();

  /*
  Returns:
    True if the ModelComponentIndex attribute is locked.
  */
  bool IndexIsLocked() const;

  /*
  Returns:
    True if the ModelComponentIndex attribute is set.
  */
  bool IndexIsSet() const;

  ///////////////////////////////////////////////////////////////////////////////
  //
  // Parent object interface
  //
  //
  const MYON_UUID& ParentId() const;

  bool ParentIdIsNil() const;
  bool ParentIdIsNotNil() const;

  bool SetParentId(
    const MYON_UUID& parent_id
    );
  bool ClearParentId();
  void LockParentId();
  bool ParentIdIsLocked() const;
  bool ParentIdIsSet() const;

  ///////////////////////////////////////////////////////////////////////////////
  //
  // Model component locked, hidden, deleted, selected, highlights, damaged, ... status interface
  //
  //
  MYON_ComponentStatus ModelComponentStatus() const;

  bool SetModelComponentStatus(
    MYON_ComponentStatus component_status
    );

  bool ClearModelComponentStatus();

  void LockModelComponentStatus();

  bool ModelComponentStatusIsLocked() const;

  bool ModelComponentStatusIsSet() const;

  /*
  Returns:
    The component's deleted state.
  Remarks:
    Deleted components can be undeleted.
  */
  bool IsDeleted() const;

  /*
  Parameters:
    bDeleted - [in]
    manifest - [in/out]
      optional manifest to update
  Returns:
    True if the component's deleted state was changed from
    !bDeleted to bDeleted and all other name and manifest information
    was updated as expected.
  */
  bool SetDeletedModelComponentState(
    bool bDeleted,
    class MYON_ComponentManifest* manifest
    );

  /*
  Returns:
    The component's user interface locked state.
  Remarks:
    This is a user interface locked state and does not effect
    changing the component values.
  */
  bool IsLocked() const;

  void SetLockedModelComponentState(
    bool bLocked 
    );

  /*
  Returns:
    The component's user interface hidden state.
  Remarks:
    This is a user interface hidden state and does not effect
    changing the component values.
  */
  bool IsHidden() const;

  void SetHiddenModelComponentState(
    bool bHidden 
    );


  ///////////////////////////////////////////////////////////////////////////////
  //
  // Name interface
  //
  //

  /*
  Returns:
    Value of the Name attribute.
  Remarks:
    If the component is in a model, then the name is unique among all components in
    the model. Names are formatted as reference : parent::leaf. 
    For example in "A.3dm : Z", "A.3dm" is the reference and "Z" is the leaf. 
    For a layer full path "X::Y::Z", "X::Y" is the parent and "Z" is the leaf.
    For most models, only the leaf is present in the name. 
    The reference portion appears when a model component originates in a reference file
    (a linked instance definition with reference component names or a worksession reference).
    Components with a tree hierarchy, like layers, can have a parent and leaf.
  */
  const MYON_wString Name() const;

  const MYON_wString DeletedName() const;

  /*
  Parameters:
    component_name - [out]
  Returns:
    A pointer to the string in component_name.
  */
  const wchar_t* GetName(
    MYON_wString& component_name
    ) const;
  
  /*
  Description:
    Get a platform independent hash of the name suitable for equality testing
    and binary searching.
    When testing for equality or doing binary searches, using MYON_NameHash values
    is faster than using the CompareName or MYON_wString::CompareAttributeName.
  */
  const MYON_NameHash& NameHash() const;
  const MYON_NameHash& DeletedNameHash() const;

  /*
  Parameters:
    new_name - [in]      
  Returns:
    If new_name is empty or a valid component name, then name hash this component would have if SetName(new_name) were called.
    If new_name is not valid, then MYON_NameHash::UnsetNameHash is returned.
  Remarks:
    This function trims leading and trailing white space, includes the parent id when 
    MYON_ModelComponent::UniqueNameIncludesParent(ComponentType()) is true,
    and uses MYON_ModelComponent::IsValidComponentName() to determine if a 
    non-empty name is valid.    
  */
  const MYON_NameHash NewNameHash(
    const wchar_t* new_name
    ) const;

  /*
  Description:
    Returns the string " : ".  This is the string Rhino uses
    to separate the reference file names from and model component names
    read from those files.

  Example:
      File A.3dm contains a layer "alpha", dimstyle "a1", an embedded block "A_blk",
      and a linked block "X" referencing B.3dm with reference component names.
      File B.3dm contains a layer "beta", dimstyle "b1", an embedded block "B_blk",
      and linked block "Y" referencing C.3dm.
      File C.3dm contains a layer gamma, dimstyle "c1", an embedded block "C_blk",
      and linked block "Z" referencing D.3dm.
      File D.3dm contains a layer "delta", dimstyle "d1", and an embedded block "D_blk".
      
      Reading file A.3dm will craete the following components:
      Layers:
        alpha
        X>B.3dm
          beta (full layer path "X>B.3dm::beta")
        Y>C.3dm
          gamma (full layer path "Y>C.3dm::gamma")
        Z>D.3dm
          delta (full layer path "Z>D.3dm::delta")
      Blocks:
        X
        A_blk
        Y
        B.3dm : B_blk
        Z
        C.3dm : C_blk
        D.3dm : D_blk
      Annotation styles
        a1
        B.3dm : b1
        C.3dm : c1
        D.3dm : d1

  See Also:
    MYON_ModelComponent::ReferencePrefixDelimiter = " : "
    MYON_ModelComponent::ReferencePrefixSeparator = ">"
    MYON_ModelComponent::NamePathSeparator = "::"
  */
  static const MYON_wString ReferencePrefixDelimiter;

  /*
  Description:
    Returns the string ">".  This is the string Rhino uses
    to separate the block definition name and linked file name
    in grandparent layers.

  Example:
      File A.3dm contains a layer "alpha", dimstyle "a1", an embedded block "A_blk",
      and a linked block "X" referencing B.3dm with reference component names.
      File B.3dm contains a layer "beta", dimstyle "b1", an embedded block "B_blk",
      and linked block "Y" referencing C.3dm.
      File C.3dm contains a layer gamma, dimstyle "c1", an embedded block "C_blk",
      and linked block "Z" referencing D.3dm.
      File D.3dm contains a layer "delta", dimstyle "d1", and an embedded block "D_blk".
      
      Reading file A.3dm will craete the following components:
      Layers:
        alpha
        X>B.3dm
          beta (full layer path "X>B.3dm::beta")
        Y>C.3dm
          gamma (full layer path "Y>C.3dm::gamma")
        Z>D.3dm
          delta (full layer path "Z>D.3dm::delta")
      Blocks:
        X
        A_blk
        Y
        B.3dm : B_blk
        Z
        C.3dm : C_blk
        D.3dm : D_blk
      Annotation styles
        a1
        B.3dm : b1
        C.3dm : c1
        D.3dm : d1

  See Also:
    MYON_ModelComponent::ReferencePrefixDelimiter = " : "
    MYON_ModelComponent::ReferencePrefixSeparator = ">"
    MYON_ModelComponent::NamePathSeparator = "::"
  */
  static const MYON_wString ReferencePrefixSeparator;


  /*
  Description:
    Returns the string "::".  This is the string Rhino uses in layer
    full path names to separate the names of individual layers.

  Example:
      If a model has layers
      A
        B
          C
      then the full path names are
      "A", "A::B", "A::B::C".

  See Also:
    MYON_ModelComponent::ReferencePrefixDelimiter = " : "
    MYON_ModelComponent::ReferencePrefixSeparator = ">"
    MYON_ModelComponent::NamePathSeparator = "::"
  */
  static const MYON_wString NamePathSeparator;

  ///*
  //Description:
  //Parameters:
  //  reference_prefix - [in]
  //Returns:
  //  The worksession component name reference prefix.
  //Example:
  //  "[ A.3dm ]" = MYON_ModelComponent::WorksessionReferencePrefix("A.3dm")
  //*/
  //static const MYON_wString WorksessionReferencePrefix(
  //  const wchar_t* reference_prefix,
  //  bool bAppendReferencePrefixDelimiter
  //);


  /*
  Description:
    Test a string to see if its beginning matches the 
    string returned by MYON_ModelComponent::NameReferenceDelimiter.
  Parameters:
    s - [in];
      string to test.
  Returns:
    null:
      The beginning of the string does not match MYON_ModelComponent::NameReferenceDelimiter.
    non-null:
      The beginning of the string matches MYON_ModelComponent::NameReferenceDelimiter.
      The returned pointer is the first character in s after the last character
      of the delimiter.  Put another way, if the beginning of s matches
      the string MYON_ModelComponent::NameReferenceDelimiter, then the returned pointer is
      s + MYON_ModelComponent::NameReferenceDelimiter.Length().
  */
  static const wchar_t* IsReferencePrefixDelimiter(
    const wchar_t* s
  );

  /*
  Description:
    Test a string to see if its beginning matches the 
    string returned by MYON_ModelComponent::NameReferenceSeparator.
  Parameters:
    s - [in];
      string to test.
  Returns:
    null:
      The beginning of the string does not match MYON_ModelComponent::NameReferenceSeparator.
    non-null:
      The beginning of the string matches MYON_ModelComponent::NameReferenceSeparator.
      The returned pointer is the first character in s after the last character
      of the delimiter.  Put another way, if the beginning of s matches
      the string MYON_ModelComponent::NameReferenceSeparator, then the returned pointer is
      s + MYON_ModelComponent::NameReferenceSeparator.Length().
  */
  static const wchar_t* IsReferencePrefixSeparator(
    const wchar_t* s
  );

  /*
  Description:
    Test a string to see if its beginning matches the 
    string returned by MYON_ModelComponent::NamePathSeparator.
  Parameters:
    s - [in];
      string to test.
  Returns:
    null:
      The beginning of the string does not match MYON_ModelComponent::NamePathSeparator.
    non-null:
      The beginning of the string matches MYON_ModelComponent::NamePathSeparator.
      The returned pointer is the first character in s after the last character
      of the delimiter.  Put another way, if the beginning of s matches
      the string MYON_ModelComponent::NamePathSeparator, then the returned pointer is
      s + MYON_ModelComponent::NamePathSeparator.Length().
  */
  static const wchar_t* IsNamePathSeparator(
    const wchar_t* s
  );

  /*
  Example:
    If a full layer name is "A.3dm : X::Y::Z", then
    reference_prefix = "A.3dm"
    name_parent = "X::Y"
    name_leaf = "Z"
  */
  static void SplitName(
    const wchar_t* name,
    MYON_wString& reference_prefix,
    MYON_wString& name_parent,
    MYON_wString& name_leaf
  );

  /*
  Parameters:
    name - [in]
  Example:
    If the name is "A.3dm : X", then "A.3dm" is returned.
  */
  static const MYON_wString NameReferencePrefix(
    const wchar_t* name
  );

  /*
  Parameters:
    name - [in]
    bIncludeReference - [in]
  Example:
    If the name is "A.3dm>B.3dm : X::Y::Z" and bIncludeReference is true, 
    then "A.3dm>B.3dm : X::Y" is returned.
    If the name is "A.3dm>B.3dm : X::Y::Z" and bIncludeReference is false, 
    then "X::Y" is returned.
  */
  static const MYON_wString NameParent(
    const wchar_t* name,
    bool bIncludeReference
  );

  /*
  Example:
    If the name is "A.3dm>B.3dm : X::Y::Z", 
    then name_leaf = "Z" is returned.
  */
  static const MYON_wString NameLeaf(
    const wchar_t* name
  );

  /*
  Parameters:
    name - [in]
  Example:
    If the name is "A.3dm : X", then "X" is returned.
  */
  static const MYON_wString RemoveReferencePrefix(
    const wchar_t* name
  );

  /*
  Description:
    Remove all occurrences of MYON::NameReferenceDelimiter() from name.
  */
  static const MYON_wString RemoveAllReferencePrefixDelimiters(
    const wchar_t* name
  );

  /*
  Description:
    Remove any trailing occurrence of MYON_ModelComponent::NameReferenceDelimiter from name.
  Example:
    "A.3dm" = MYON_ModelComponent::RemoveTrailingRemoveReferencePrefixDelimiter("A.3dm : ");
  */
  static const MYON_wString RemoveTrailingReferencePrefixDelimiter(
    const wchar_t* name
  );

  /*
  Description:
    Remove any trailing occurrence of MYON_ModelComponent::NameReferenceSeparator from name.
  */
  static const MYON_wString RemoveTrailingReferencePrefixSeparator(
    const wchar_t* name
  );

  /*
  Description:
    Remove any trailing occurrence of MYON_ModelComponent::NamePathSeparator from name.
  */
  static const MYON_wString RemoveTrailingNamePathSeparator(
    const wchar_t* name
  );


  
private:
  const MYON_NameHash& Internal_NameHash() const;

public:
  /*
  Description:
    Expert user function that gets a pointer to the name.
  Returns:
    A pointer to the string that contains the name. This pointer
    must be used immediately and then discarded before any code
    can possibly call SetName() or the MYON_ModelComponent destructor
    could be called.
  Remarks:
    About the only good use for this function is when a model compoenent is
    persistent and the name is needed for a formatted string.  For any
    other use, call the Name() function and store the result in an
    MYON_wString.  This function is dangerous because the returned pointer
    will be invalid if SetName() is called.
  */
  const wchar_t* NameAsPointer() const;

  /*
  Returns:
    true if the name is unset or is set to the empty string.
  */
  bool NameIsEmpty() const;

  /*
  Returns:
    true if the name is set and the value is not the empty string.
  */
  bool NameIsNotEmpty() const;

  /*
  Parameters:
    component_name - [in]
      Leading and trailing nonzero unicode code points with values <= MYON_wString::Space are ignored.
  Returns:
    true 
      if the name attribute was changed to component_name or is already equal to component_name.
    false 
      False is returned and no changes are made if 
      the name attribute is locked and component_name != locked value 
      or
      component_name is not empty and MYON_ModelComponent::IsValidComponentName(component_name) is false.
  Remarks:
    If component_name is nullptr or the empty string, the NameIsSet() state will still be true.
  */
  bool SetName(
    const wchar_t* component_name
    );

  /*
  Description:
    Change the name of a component and optionally update a manifest.
  Parameters:
    new_name - [in]
      New name for the component.
    manifest - [in]
      nullptr or a manifest with an item for the component.
  */
  bool ChangeName(
    const wchar_t* new_name,
    class MYON_ComponentManifest* manifest
  );

  /*
  Description:
    Set the component name value to the MYON_ModelComponent::Unset.Name()
    and change the state so NameIsSet() and DeletedNameIsSet() both return false.
  */
  bool ClearName();

  /*
  Description:
    Calling LockName() will prohibit future changes to the Name attribute.
  */
  void LockName();

  /*
  Returns:
    True if the Name attribute is locked.
  */
  bool NameIsLocked() const;

  /*
  Returns:
    True if the name attribute is set.
  Remarks:
    At most one of NameIsSet() and DeletedNameIsSet() is true.
    A name can be set to the empty string.
  */
  bool NameIsSet() const;

  /*
  Returns:
    True if the deleted name attribute is set.
  Remarks:
    At most one of NameIsSet() and DeletedNameIsSet() is true.
  */
  bool DeletedNameIsSet() const;

  /*
  Description:
    If NameIsSet() is true, then the name is deleted in a waty that can be undone by calling UndeleteName() and
    the DeletedNameIsSet() property is changed to true.

    If NameIsSet() is false or NameIsLocked() is true, then nothing is changed.

    Note that a name can be set to the empty string and there is a difference between
    a name being set to the empty string and a name being unset.
    If you want to make the name attribute unset, call ClearName().
  */
  bool DeleteName();

  bool UndeleteName();

  void LockAllSettingsExceptName();
  bool SetLocalizedSystemComponentName(
    const wchar_t* system_component_localized_name
    );

  /*
  Parameters:
    other_name -[in]
  Returns:
    MYON_wString::CompareAttributeName(this->Name(), other_name)
  Remarks:
    CompareName() is the correct tool to use when looking up objects by name.
  */
  int CompareName(
    const MYON_UUID& other_parent_id,
    const wchar_t* other_name
    ) const;
  int CompareName(
    const wchar_t* other_name
    ) const;

  /*
  Parameters:
    other_name -[in]
  Returns:
    int rc = MYON_wString::CompareAttributeName(this->Name(), other_name);
    return (0 != rc) ? rc : MYON_wString::CompareOrdinal(this->Name(), other_name, false);
  Remarks:
    CompareNameExact() is the correct tool to use when detecting changes in
    case are important.  Use CompareName() when searching for components by name. 
  */
  int CompareNameExact(
    const MYON_UUID& other_parent_id,
    const wchar_t* other_name
    ) const;
  int CompareNameExact(
    const wchar_t* other_name
    ) const;


  /*
  Parameters:
    a -[in]
    b -[in]
  Returns:
    MYON_wString::CompareAttributeName(a.Name(), b.Name() )
  Remarks:
    CompareName() is the correct tool to use when looking up objects by name.
  */
  static int CompareName(
    const MYON_ModelComponent& a,
    const MYON_ModelComponent& b
    );

  /*
  Parameters:
    a -[in]
    b -[in]
  Returns:
    int rc = MYON_wString::CompareAttributeName(a.Name(), b.Name());
    return (0 != rc) ? rc : MYON_wString::CompareOrdinal(a.Name(), b.Name(), false);
  Remarks:
    CompareNameExact() is the correct tool to use when detecting changes in
    case are important.  Use CompareName() when searching for components by name. 
  */
  static int CompareNameExact(
    const MYON_ModelComponent& a,
    const MYON_ModelComponent& b
    );

  static int CompareId(
    const MYON_ModelComponent& a,
    const MYON_ModelComponent& b
    );

  /*
  Description 
    Call MYON_ModelComponent::CompareName() and then MYON_ModelComponent::CompareId().
    When used for sorting, will be sorted by name and then by id.
  */
  static int CompareNameAndId(
    const MYON_ModelComponent& a,
    const MYON_ModelComponent& b
    );

  /*
  Description 
    Call MYON_ModelComponent::Id()and then MYON_ModelComponent::CompareName().
    When used for sorting, will be sorted by id and then by name.
  */
  static int CompareIdAndName(
    const MYON_ModelComponent& a,
    const MYON_ModelComponent& b
    );

  /*
  Description:
    Set a component's id, name and index to the manifest_item values.
  Parameters:
    manifest_item - [in]
    manifest_name - [in]
    bSetId - [in]
      Set the component id to manifest_item.m_manifest_id.
    bSetParentId - [in]
      Use manifest_item.NameHash().ParentId()
    bSetName - [in]
      Set the component name to manifest_name.
    bSetIndex - [in]
      Set the component index to manifest_item.m_manifest_index.
  */
  bool SetIdentification(
    const class MYON_ComponentManifestItem& manifest_item,
    const wchar_t* manifest_name,
    bool bSetId,
    bool bParentId,
    bool bSetName,
    bool bSetIndex
    );
  

  ///////////////////////////////////////////////////////////////////////////////
  //
  // General attributes interface
  //
  //

  /*
  Description:
    Writes the attributes identified by the component_filter parameter.
  Parameters:
    attributes_filter - [in]
      A bitfield that determines which attributes will be cleared.
  Returns:
    a bitfield indicating which attributes were cleared.
    (Locked attributes cannot be cleared.)
  */
  unsigned int ClearModelComponentAttributes(
    unsigned int attributes_filter
    );

  /*
  Returns:
    The runtime serial number.  
    This value is unique for every instance of an MYON_ModelComponent class.  
    This value is never saved in .3dm archives.
  */
  MYON__UINT64 RuntimeSerialNumber() const;

  /*
  Returns:
    The next MYON_ModelComponent instance will have runtime serial number
    >= MYON_ModelComponent::NextRuntimeSerialNumber();
  */
  static MYON__UINT64 NextRuntimeSerialNumber();

  /*
  Description:
    Whenever an attribute is changed, the content version number is incremented.
    The ContentVersionNumber() is commonly used by consumers of the model 
    component attributes to trigger updates when needed.
  Remarks:
    The ContentVersionNumber value is copied by the copy constructor and operator=.
    The value is not saved in .3dm archives.
  */
  MYON__UINT64 ContentVersionNumber() const;

  /*
  Returns:
    True if this model component is a system constant.  
  Remarks:
    An incomplete list of system constant model compoenents is below:

      MYON_ModelComponent::Unset

      MYON_InstanceDefinition::Empty

      MYON_Linetype::Unset
      MYON_Linetype::Continuous
      MYON_Linetype::ByLayer
      MYON_Linetype::ByParent

      MYON_Layer::Unset
      MYON_Layer::Default

      MYON_TextStyle::Unset
      MYON_TextStyle::Default
      MYON_TextStyle::ByLayer
      MYON_TextStyle::ByParent

      MYON_DimStyle::Unset
      MYON_DimStyle::Default
      MYON_DimStyle::DefaultInchDecimal
      MYON_DimStyle::DefaultInchFractional
      MYON_DimStyle::DefaultFootInchArchitectural
      MYON_DimStyle::DefaultMillimeterSmall
      MYON_DimStyle::DefaultMillimeterLarge
      MYON_DimStyle::DefaultMillimeterArchitectural
  */
  bool IsSystemComponent() const;

  bool EraseIdentification(
    bool bIgnoreLocks
    );

  bool SetAsSystemComponent();
  bool SetAsUnsetSystemComponent();

private:
  bool Internal_SetAsSystemComponent(
    bool bUnsetSystemComponent
    );

protected:
  /*
    Classes derived from MYON_ModelComponent should have private data members and
    call IncrementContentVersionNumber() when the data member value changes.
  */
  void IncrementContentVersionNumber() const;

private:
  const MYON__UINT64 m_runtime_serial_number;  
  mutable MYON__UINT64 m_content_version_number = 0;
  MYON__UINT32 m_model_serial_number = 0;
  MYON__UINT32 m_reference_model_serial_number = 0;
  MYON__UINT32 m_linked_idef_serial_number = 0;
  MYON_ModelComponent::Type m_component_type = MYON_ModelComponent::Type::Unset;
 
  MYON__UINT16 m_locked_status = 0;
  MYON__UINT16 m_set_status = 0;
  MYON_ComponentStatus m_component_status = MYON_ComponentStatus::NoneSet;
  MYON__UINT16 m_reserved2 = 0;

  // m_component_index is the index of the component in the model identified 
  // by m_model_serial_number.
  //
  // Some components use small negative indices (-1,-2,-3,...) to identify
  // default or constant components.  An incomplete list includes these:
  //   MYON_TextStyle::Default, (index = -1) 
  //   MYON_TextStyle::ByLayer,  (index = -2) 
  //   MYON_TextStyle::ByParent,  (index = -3) 
  //   MYON_Linetype::Continuous,  (index = -1) 
  //   MYON_Linetype::ByLayer,  (index = -2) 
  //   MYON_Linetype::ByParent,  (index = -3) 
  //
  // MYON_UNSET_INT_INDEX is a large negative number.
  MYON__INT32 m_component_index = MYON_UNSET_INT_INDEX;  
  MYON__UINT32 m_reserved3 = 0;

  MYON_UUID m_component_id = MYON_nil_uuid;
  MYON_UUID m_component_parent_id = MYON_nil_uuid;
  // lazy evaluation is used top initialize m_component_name_hash
  mutable MYON_NameHash m_component_name_hash;

  MYON_wString m_component_name;

public:
  // For internal use.  Never call this function.
  static unsigned int Internal_SystemComponentHelper();

private:
#pragma MYON_PRAGMA_WARNING_PUSH
#pragma MYON_PRAGMA_WARNING_DISABLE_MSC( 4251 )
  // C4251: ... needs to have dll-interface to be used by clients of class ...
  // This warning is not correct.
  // Internal_RuntimeSerialNumberGenerator is private and all code that manages it is explicitly implemented in the DLL.
  static std::atomic<MYON__UINT64> Internal_RuntimeSerialNumberGenerator;
#pragma MYON_PRAGMA_WARNING_POP
};


class MYON_CLASS MYON_ModelComponentContentMark
{
public:
  static const MYON_ModelComponentContentMark Unset;
public:
  MYON_ModelComponentContentMark() = default;
  ~MYON_ModelComponentContentMark() = default;
  MYON_ModelComponentContentMark(const MYON_ModelComponentContentMark&) = default;
  MYON_ModelComponentContentMark& operator=(const MYON_ModelComponentContentMark&) = default;

  /*
  Description:
    Save the current content state of model_component.
  Parameters:
    model_component - [in]
  */
  MYON_ModelComponentContentMark(
    const class MYON_ModelComponent& model_component
  );

  /*
  Description:
    Save the current content state of model_component.
  Parameters:
    model_component - [in]
  */
  MYON_ModelComponentContentMark(
    const class MYON_ModelComponent* model_component
  );

  /*
  Description:
    Save the current content state of model_component.
  Parameters:
    model_component - [in]
  */
  void Set(
    const class MYON_ModelComponent& model_component
  );

  /*
  Description:
    Save the current content state of model_component.
  Parameters:
    model_component - [in]
  */
  void Set(
    const class MYON_ModelComponent* model_component
  );
  
  /*
  Parameters:
    model_component - [in]
  Returns:
    true if the content of model_component is identical
    to the content state saved in this MYON_ModelComponentContentMark.
    false otherwise.
  */
  bool EqualContent(
    const class MYON_ModelComponent& model_component
  ) const;
    
  /*
  Parameters:
    model_component - [in]
  Returns:
    true if the content of model_component is identical
    to the content state saved in this MYON_ModelComponentContentMark.
    false otherwise.
  */
  bool EqualContent(
    const class MYON_ModelComponent* model_component
  ) const;
  
  /*
  Parameters:
    lhs - [in]
    rhs - [in]
  Returns:
    true if lhs and rhs are identical.
    false otherwise.
  */
  static bool EqualContent(
    const MYON_ModelComponentContentMark& lhs,
    const MYON_ModelComponentContentMark& rhs
  );

  /*
  Returns:
    Saved model component id.
  */
  MYON_UUID ComponentId() const;

  /*
  Returns:
    Saved model component runtime serial number.
  */
  MYON__UINT64 ComponentRuntimeSerialNumber() const;

  /*
  Returns:
    Saved model component content version number.
  */
  MYON__UINT64 ComponentContentVersionNumber() const;

  /*
  Returns:
    Saved model component type.
  */
  MYON_ModelComponent::Type ComponentType() const;

  /*
  Returns:
    True if a model component content state is saved in this MYON_ModelComponentContentMark.
    False otherwise.
  */
  bool IsSet() const;

  /*
  Returns:
    True if a model component content state is saved in this MYON_ModelComponentContentMark.
    False otherwise.
  */
  bool IsUnset() const;

private:
  MYON_UUID m_component_id = MYON_nil_uuid;
  MYON__UINT64 m_component_serial_number = 0;  
  MYON__UINT64 m_component_content_version_number = 0;
  MYON_ModelComponent::Type m_component_type = MYON_ModelComponent::Type::Unset;

private:
  unsigned char m_reserved1 = 0;
  unsigned char m_reserved2 = 0;
  unsigned char m_reserved3 = 0;
  unsigned int m_reserved4 = 0;
};

bool operator==(
  const MYON_ModelComponentContentMark& lhs,
  const MYON_ModelComponentContentMark& rhs
  );

bool operator!=(
  const MYON_ModelComponentContentMark& lhs,
  const MYON_ModelComponentContentMark& rhs
  );

class MYON_CLASS MYON_ModelComponentTypeIterator
{
public:
  // Default constructor creates MYON_ModelComponentTypeIterator::Empty
  MYON_ModelComponentTypeIterator() MYON_NOEXCEPT;

  ~MYON_ModelComponentTypeIterator() = default;
  MYON_ModelComponentTypeIterator(const MYON_ModelComponentTypeIterator&) = default;
  MYON_ModelComponentTypeIterator& operator=(const MYON_ModelComponentTypeIterator&) = default;

  /*
  Parameters:
    type_count - [in] number of types
    types - [in]
      list of types to iterate over
  */
  MYON_ModelComponentTypeIterator(
    size_t type_count,
    const MYON_ModelComponent::Type* types
    ) MYON_NOEXCEPT;
  
  // Contains no types
  static const MYON_ModelComponentTypeIterator Empty;

  // Contains all explicit component types (excludes Unset and Mixed).
  static const MYON_ModelComponentTypeIterator ExplicitComponentTypes;

  // Every type in MYON_ModelComponentTypeIterator::ExplicitComponents
  // except Image, RenderLight, ModelGeometry, and HistoryRecord
  // The types in MYON_ModelComponentTypeIterator::Tables
  // identify the most commonly used "table" component types.
  static const MYON_ModelComponentTypeIterator TableComponentTypes;

  // Number of types in the iterator
  unsigned int TypeCount() const;

  /*
  Description:
    Sets the iterator to point to the first available type. 
  Returns:
    The first available type.
  */
  MYON_ModelComponent::Type FirstType();

  /*
  Description:
    Sets the iterator to point to the last available type. 
  Returns:
    The last available type.
  */
  MYON_ModelComponent::Type LastType();

  /*
  Description:
    Increments the iterator and returns the new type.
  Returns:
    The new type.
    MYON_ModelComponent::Type::Unset if the iterator is incremented the last type.
  */
  MYON_ModelComponent::Type NextType();
  
  /*
  Description:
    Decrements the iterator and returns the new type.
  Returns:
    The new type
    MYON_ModelComponent::Type::Unset if the iterator is decrented the first type.
  */
  MYON_ModelComponent::Type PreviousType();

  /*
  Description:
    Decrements the iterator and returns the new type.
  Returns:
    The current type.
  */
  MYON_ModelComponent::Type CurrentType() const;

private:
  MYON__INT32 m_type_count = 0;

  // -1 iterated before first, m_type_count = iteratated past last
  MYON__INT32 m_current_index = MYON_UNSET_UINT_INDEX; 

  MYON_ModelComponent::Type m_types[32];
};

/// <summary>
/// MYON_ModelComponentReference is a persistent reference to a model component.
/// MYON_ModelComponentReference and MYON_ModelComponentWeakReference are based on
/// like std::shared_ptr and std::weak_ptr.
/// </summary>
class MYON_CLASS MYON_ModelComponentReference
{
public:
  static const MYON_ModelComponentReference Empty;
  
  // No = default to insure m_sp is completely managed in the openurbs DLL.
  MYON_ModelComponentReference() MYON_NOEXCEPT; // No = default to insure m_sp is completely managed in the openurbs DLL.
  ~MYON_ModelComponentReference(); // No = default to insure m_sp is completely managed in the openurbs DLL.
  MYON_ModelComponentReference(const MYON_ModelComponentReference&) MYON_NOEXCEPT; // No = default to insure m_sp is completely managed in the openurbs DLL.
  MYON_ModelComponentReference& operator=(const MYON_ModelComponentReference&); // No = default to insure m_sp is completely managed in the openurbs DLL.
  
#if defined(MYON_HAS_RVALUEREF)
  MYON_ModelComponentReference( MYON_ModelComponentReference&& ) MYON_NOEXCEPT;
  MYON_ModelComponentReference& operator=( MYON_ModelComponentReference&& );
#endif

  MYON_ModelComponentReference(
    const class MYON_ModelComponentWeakReference& weak_ref
  )  MYON_NOEXCEPT;

  MYON_ModelComponentReference& operator=( const MYON_ModelComponentWeakReference& );


  MYON_ModelComponentReference(
    std::shared_ptr<MYON_ModelComponent>& sp
    ) MYON_NOEXCEPT;

  MYON_ModelComponentReference& operator=(
    std::shared_ptr<MYON_ModelComponent>& sp
    );
  
  /*
  Parameters:
    constant_system_component - [in]
      A constant system component that exists for the duration of 
      the application.
  Remarks:
    See MYON_ModelComponent::CreateConstantSystemComponentReference()
    for more information about constant system components.

  */
  static MYON_ModelComponentReference CreateConstantSystemComponentReference(
    const class MYON_ModelComponent& constant_system_component
    ) MYON_NOEXCEPT;

  /*
  Parameters:
    model_component - [in]
    bManagedComponentReference - [in]
      true
        model_component will be deleted by the last MYON_ModelComponentReference
        and must have been created by call to operator new() with heap allocation.
      false 
        model_component must remain in scope until the last MYON_ModelComponentReference
        is destroyed.
  Remarks:
    The input pointer will be managed and deleted by the returned
    by MYON_ModelComponentReference.
  */
  static MYON_ModelComponentReference CreateForExperts(
    class MYON_ModelComponent* model_component,
    bool bManagedComponentReference
    ) MYON_NOEXCEPT;

  /*
  Return:
    A pointer to the managed model component or nullptr.
  See Also:
    MYON_ModelComponentRef::ExclusiveModelComponent();
  */
  const class MYON_ModelComponent* ModelComponent() const MYON_NOEXCEPT;

  /*
  Return:
    If this is the only reference to a component, the a pointer to
    that component is returned.
    Otherwise, nullptr is returned
  See Also:
    MYON_ModelGeometryComponent::Attributes()
    MYON_ModelGeometryComponent::Geometry()
    MYON_ModelGeometryComponent::ExclusiveAttributes()
    MYON_ModelGeometryComponent::ExclusiveGeometry();
    MYON_ModelComponentRef::ExclusiveModelComponent();
    MYONX_Model::ComponentFromRuntimeSerialNumber()
  Remarks:
    If .NET or other wrappers using "lazy garbage collection" memory management are in use,
    there may be stale references awaiting gargage collection and this function will return
    nullptr when you think it should not. 
    For this function to work reliably, the MYONX_Model and its components 
    and references should be in well constructed C++ code with carefully 
    crafted memory management.
  */
  class MYON_ModelComponent* ExclusiveModelComponent() const MYON_NOEXCEPT;

  MYON__UINT64 ModelComponentRuntimeSerialNumber() const MYON_NOEXCEPT;
  const MYON_UUID ModelComponentId() const MYON_NOEXCEPT;
  const MYON_NameHash ModelComponentNameHash() const MYON_NOEXCEPT;
  int ModelComponentIndex() const MYON_NOEXCEPT;

  unsigned int ReferenceCount() const MYON_NOEXCEPT;
  
  bool IsEmpty() const MYON_NOEXCEPT;

  void Dump(
    MYON_TextLog& text_log
    ) const;

private:
  friend class MYON_ModelComponentWeakReference;

#pragma MYON_PRAGMA_WARNING_PUSH
#pragma MYON_PRAGMA_WARNING_DISABLE_MSC( 4251 ) 
  // C4251: ... needs to have dll-interface to be used by clients of class ...
  // m_sp is private and all code that manages m_sp is explicitly implemented in the DLL.
  std::shared_ptr<MYON_ModelComponent> m_sp;
#pragma MYON_PRAGMA_WARNING_POP
};

#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_ClassArray<MYON_ModelComponentReference>;
#endif

/// <summary>
/// MYON_ModelComponentWeakReference is a weak shared reference to a model component.
/// MYON_ModelComponentReference and MYON_ModelComponentWeakReference are based on
/// like std::shared_ptr and std::weak_ptr.
/// </summary>
class MYON_CLASS MYON_ModelComponentWeakReference
{
public:
  static const MYON_ModelComponentWeakReference Empty;

public:
  // No = default to insure m_sp is completely managed in the openurbs DLL.
  MYON_ModelComponentWeakReference() MYON_NOEXCEPT; // No = default to insure m_sp is completely managed in the openurbs DLL.
  ~MYON_ModelComponentWeakReference(); // No = default to insure m_sp is completely managed in the openurbs DLL.
  MYON_ModelComponentWeakReference(const MYON_ModelComponentWeakReference&) MYON_NOEXCEPT; // No = default to insure m_sp is completely managed in the openurbs DLL.
  MYON_ModelComponentWeakReference& operator=(const MYON_ModelComponentWeakReference&); // No = default to insure m_sp is completely managed in the openurbs DLL.
  
#if defined(MYON_HAS_RVALUEREF)
  MYON_ModelComponentWeakReference( MYON_ModelComponentWeakReference&& ) MYON_NOEXCEPT;
  MYON_ModelComponentWeakReference& operator=( MYON_ModelComponentWeakReference&& );
#endif

  MYON_ModelComponentWeakReference(
    const class MYON_ModelComponentReference&
  )  MYON_NOEXCEPT;

  MYON_ModelComponentWeakReference& operator=( const MYON_ModelComponentReference& );


private:
  friend class MYON_ModelComponentReference;
#pragma MYON_PRAGMA_WARNING_PUSH
#pragma MYON_PRAGMA_WARNING_DISABLE_MSC( 4251 ) 
  // C4251: ... needs to have dll-interface to be used by clients of class ...
  // m_weak_sp is private and all code that manages m_weak_sp is explicitly implemented in the DLL.
  std::weak_ptr<MYON_ModelComponent> m_wp;
#pragma MYON_PRAGMA_WARNING_POP
};

#endif

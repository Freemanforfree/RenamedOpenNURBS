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

#if !defined(OPENNURBS_EXTENSIONS_INC_)
#define OPENNURBS_EXTENSIONS_INC_

/*
Description:
  The MYONX_ErrorCounter is useful for counting errors that occur in a section of code.
*/
class MYON_CLASS MYONX_ErrorCounter
{
public:
  MYONX_ErrorCounter() = default;
  ~MYONX_ErrorCounter() = default;
  MYONX_ErrorCounter(const MYONX_ErrorCounter&) = default;
  MYONX_ErrorCounter& operator=(const MYONX_ErrorCounter&) = default;

  const MYONX_ErrorCounter operator += (const MYONX_ErrorCounter& rhs);
  const MYONX_ErrorCounter operator + (const MYONX_ErrorCounter& rhs);

  static const MYONX_ErrorCounter Zero;

  /*
  Returns:
    Number of failures.
  */
  unsigned int FailureCount() const;

  /*
  Returns:
    Number of errors.
  */
  unsigned int ErrorCount() const;

  /*
  Returns:
    Number of warnings.
  */
  unsigned int WarningCount() const;

  /*
  Returns:
    Number of failures, errors, and warnings.
  */
  unsigned int TotalCount() const;

  /*
  Description:
    Adds one to the failure count.
  Returns:
    Number of failures including this one.
  */
  unsigned int IncrementFailureCount();

  /*
  Description:
    Adds one to the error count.
  Returns:
    Number of errors including this one.
  */
  unsigned int IncrementErrorCount();

  /*
  Description:
    Adds one to the warning count.
  Returns:
    Number of warnings including this one.
  */
  unsigned int IncrementWarningCount();

  /*
  Description:
    Saves the current value of MYON_GetErrorCount() 
    so future calls to MYON_ERROR can be counted.
  */
  void ClearLibraryErrors();

  /*
  Description:
    Adds the number of calls to MYON_ERROR since the last 
    call to ClearLibraryErrors(), AddLibraryErrors(),
    ClearLibraryErrorsAndWarnings, or AddLibraryErrorsAndWarnings().
  Returns:
    The number of errors added.
  */
  unsigned int AddLibraryErrors();

  /*
  Description:
    Saves the current value of MYON_GetWarningCount() 
    so future calls to MYON_WARNING can be counted.
  */
  void ClearLibraryWarnings();

  /*
  Description:
    Adds the number of calls to MYON_WARNING since the last 
    call to ClearLibraryWarnings(), AddLibraryWarnings(),
    ClearLibraryErrorsAndWarnings(), or AddLibraryErrorsAndWarnings().
  Returns:
    The number of warnings added.
  */
  unsigned int AddLibraryWarnings();

  /*
  Description:
    Calls ClearLibraryErrors() and ClearLibraryWarnings().
  */
  void ClearLibraryErrorsAndWarnings();

  /*
  Description:
    Calls AddLibraryErrors() and AddLibraryWarnings().
  Returns:
    The number of errors and warnings added.
  */
  unsigned int AddLibraryErrorsAndWarnings();

  void Dump(MYON_TextLog& text_log) const;

private:
  unsigned int m_failure_count = 0;
  unsigned int m_error_count = 0;
  unsigned int m_warning_count = 0;

  unsigned int m_state_bit_field = 0;
  unsigned int m_opennurbs_library_error_count = 0;
  unsigned int m_opennurbs_library_warning_count = 0;
};


/*
Description:
  Used to store user data information in an MYONX_Model.
*/
class MYON_CLASS MYONX_Model_UserData
{
public:
#if defined(OPENNURBS_EXPORTS) || defined(OPENNURBS_IMPORTS)
  // See comments at the top of opennurbs_extensions.cpp for details.

  // new/delete
  void* operator new(size_t);
  void  operator delete(void*);

  // array new/delete
  void* operator new[] (size_t);
  void  operator delete[] (void*);

  // in place new/delete
  void* operator new(size_t,void*);
  void  operator delete(void*,void*);
#endif

  MYONX_Model_UserData() = default;
  ~MYONX_Model_UserData() = default;
  MYONX_Model_UserData(const MYONX_Model_UserData&) = default;
  MYONX_Model_UserData& operator=(const MYONX_Model_UserData&) = default;

  void Dump( MYON_TextLog& ) const;

  MYON_UUID m_uuid = MYON_nil_uuid;
  MYON_3dmGoo m_goo;

public:
  // 3dm version = 1,2,3,4,5,50,60,...
  unsigned int m_usertable_3dm_version = 0;

  // opennurbs_version = old yyyymmddn value or
  // a value from MYON_VersionNumberConstruct().
  unsigned int m_usertable_opennurbs_version = 0;
};

#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYONX_Model_UserData*>;
#endif

/*
Description:
  Pedegodgical example of all the things in an OpenNURBS 3dm archive.
  The openNURBS examples use MYONX_Model to store the information
  read from 3dm archives.  Please study example_read.cpp for
  details.
*/
class MYON_CLASS MYONX_Model
{
#if defined(OPENNURBS_EXPORTS) || defined(OPENNURBS_IMPORTS)
  // See comments at the top of opennurbs_extensions.cpp for details.

public:

  // new/delete
  void* operator new(size_t);
  void  operator delete(void*);

  // array new/delete
  void* operator new[] (size_t);
  void  operator delete[] (void*);

  // in place new/delete
  void* operator new(size_t,void*);
  void  operator delete(void*,void*);
#endif

public:
  MYONX_Model();
  virtual ~MYONX_Model();

  void Reset();

private:
  // prohibit use of copy construction and operator=
  MYONX_Model(const MYONX_Model&) = delete;
  MYONX_Model& operator=(const MYONX_Model&) = delete;

public:
  /*
  Description:
    Reads an openNURBS archive and saves the information in this model
  Parameters:
    archive - [in]
      archive to read from
    table_filter - [in]
      If table_filter is zero, then everything in the archive is read.
      Otherwise the bits in table_filter identify what tables should 
      be read.  The bits are defined by the 
      MYON_BInaryArchive::table_type enum.
    model_object_type_filter - [in]
      If model_object_type_filter is not zero, then it is a bitfield filter 
      made by bitwise oring MYON::object_type values to select which types of 
      objects will be read from the model object table.
    error_log - [out] 
      any archive reading errors are logged here.
  Returns:
    true if archive is read with no error.  False if errors occur.
    Error details are logged in error_log.  If crc errors are in
    the archive, then MYONX_Model::m_crc_error_count is set to the 
    number of crc errors.
  Example:

            // for ASCII file names
            const char* sFileName = ....;
            FILE* fp = MYON::OpenFile( sFileName, "rb");

            // for UNICODE file names
            const wchar_t* wsFileName = ....;
            FILE* fp = MYON::OpenFile( wsFileName, L"rb");

            bool bModelRead = false;
            bool bModelIsValid = false;

            MYON_TextLog error_log;
            MYONX_Model model;

            if ( 0 != fp )
            {
              MYON_BinaryFile archive( MYON::archive_mode::read3dm, fp );
              bModelRead = model.Read( archive, error_log );
              MYON::CloseFile( fp );
            }

            if ( bModelRead )
            {
              bModelIsValid = model.Validate(error_log);
            }

  See Also:
    MYONX_Model::IsValid
    MYONX_Model::Write
    MYONX_Model::m_crc_error_count
  */
  bool Read( 
    MYON_BinaryArchive& archive,
    unsigned int table_filter,
    unsigned int model_object_type_filter,
    MYON_TextLog* error_log
    );

  bool Read( 
    const char* filename,
    unsigned int table_filter,
    unsigned int model_object_type_filter,
    MYON_TextLog* error_log
    );

  bool Read( 
    const wchar_t* filename,
    unsigned int table_filter,
    unsigned int model_object_type_filter,
    MYON_TextLog* error_log
    );

  bool Read( 
    MYON_BinaryArchive& archive,
    MYON_TextLog* error_log = nullptr
    );

  bool Read( 
    const char* filename,
    MYON_TextLog* error_log = nullptr
    );

  bool Read( 
    const wchar_t* filename,
    MYON_TextLog* error_log = nullptr
    );

  /*
  Description:
    Reads everything up to the object table.

  Parameters:
    archive - [in] 
      archive to read from
    bManageComponents - [in]
      true: 
        The MYONX_Model destructor will delete the model components
        created by this function.
      false: 
        The caller must delete the MYON_ModelComponent components after
        the MYONX_Model is destroyed.
    table_filter - [in]
      If table_filter is zero, then everything in the archive before
      the model object table is read.  Otherwise the bits in 
      table_filter identify what tables should be read.  The bits
      are defined by the MYON_BInaryArchive::table_type enum.
    error_log - [out] any archive reading errors are logged here.
     pass nullptr if you don't want to log errors

  Returns:
    If the input is valid and everything before the model object
    table is successfully read, then true is returned.  Otherwise
    false is returned.

  Example:

            // for ASCII file names
            const char* sFileName = ....;
            FILE* fp = MYON::OpenFile( sFileName, "rb");

            // for UNICODE file names
            const wchar_t* wsFileName = ....;
            FILE* fp = MYON::OpenFile( wsFileName, L"rb");

            bool bModelRead = false;
            bool bModelIsValid = false;

            MYON_TextLog error_log;

            if ( 0 != fp )
            {
              MYON_BinaryFile archive( MYON::archive_mode::read3dm, fp );
              MYONX_Model model;
              
              // Read settings, layer information, and other tables
              // with information that is referenced by model object
              // attributes.
              bModelRead = model.IncrementalReadBegin( archive, error_log );

              if ( bModelRead )
              {
                object_filter = MYON::mesh_object   // read meshes
                              | MYON::curve_object  //  and curves
                              ;
                for(;;)
                {
                  // read the next model object
                  MYON_ModelGeometryComponent* pModelObject = model.IncrementalReadModelObject(object_filter,0);
                  if ( 0 == pModelObject )
                    break;
                  
                  ... // work with this model object

                  // done with this object.
                  pModelObject = 0;
                  model.m_object_table.Remove();
                }                
              }
              MYON::CloseFile( fp );
            }
  See Also:
    MYONX_Model::IsValid
    MYONX_Model::Write
    MYONX_Model::m_crc_error_count
  */
  bool IncrementalReadBegin(
    MYON_BinaryArchive& archive,
    bool bManageComponents,
    unsigned int table_filter,
    MYON_TextLog* error_log
    );

  /*
  Description:
    Reads the next item in the model geometry table.

  Parameters:
    archive - [in]
    bManageModelGeometryComponent - [in]
      true: 
        The MYONX_Model destructor will delete the MYON_ModelGeometryComponent components
        created by this function.
      false: 
        The caller must delete the MYON_ModelGeometryComponent components after
        the MYONX_Model is destroyed.
    bManageGeometry - [in]
      true: 
        The MYON_ModelGeometryComponent destructor will delete the MYON_Geometry 
        classes created by this function.
      false: 
        The caller must delete the MYON_Geometry classes after
        the MYONX_Model and MYON_ModelGeometryComponent components are destroyed.
    bManageAttributes - [in]
      true: 
        The MYON_ModelGeometryComponent destructor will delete the MYON_3dmObjectAttributes
        classes created by this function.
      false: 
        The caller must delete the MYON_3dmObjectAttributes classes after
        the MYONX_Model and MYON_ModelGeometryComponent components are destroyed.
    model_object_type_filter - [in]
      If model_object_type_filter is not zero, then it is a bitfield filter 
      made by bitwise oring MYON::object_type values to select which types of
      objects will be read from the model object table.
    model_geometry_reference - [out]
      A reference to an MYON_ModelGeometryComponent.  This referenced MYON_ModelGeometryComponent
      component is also added to the MYONX_Model. 
      Call MYONX_Model.RemoveComponent() if you want to discard it before
      continuing.  
  Returns:
    True
      Successful.  If model_geometry_reference.IsEmpty() is true, 
      then no more geometry objects are available and you should call
      IncrementalReadFinish().
    False
      An error occurred and reading should terminate.
  Remarks:
    You must call IncrementalReadBegin() before making any calls to
    IncrementalReadModelObject().
  */
  bool IncrementalReadModelGeometry(
    MYON_BinaryArchive& archive,
    bool bManageModelGeometryComponent,
    bool bManageGeometry,
    bool bManageAttributes,
    unsigned int model_object_type_filter,
    MYON_ModelComponentReference& model_geometry_reference
    );

  /*
  Description:
    Reads everything up to the object table.

  Parameters:
    archive - [in] 
      archive to read from
    bManageComponents - [in]
      true: 
        The MYONX_Model destructor will delete the model components
        created by this function.
      false: 
        The caller must delete the MYON_ModelComponent components after
        the MYONX_Model is destroyed.
    table_filter - [in]
      If table_filter is zero, then everything in the archive before
      the model object table is read.  Otherwise the bits in 
      table_filter identify what tables should be read.  The bits
      are defined by the MYON_BInaryArchive::table_type enum.
    error_log - [out] any archive reading errors are logged here.
     pass nullptr if you don't want to log errors

  Returns:
    If the input is valid and everything before the model object
    table is successfully read, then true is returned.  Otherwise
    false is returned.

  See Also:
    MYONX_Model::IsValid
    MYONX_Model::Write
    MYONX_Model::m_crc_error_count
  */
  bool IncrementalReadFinish(
    MYON_BinaryArchive& archive,
    bool bManageComponents,
    unsigned int table_filter,
    MYON_TextLog* error_log
    );

  /*
  Description:
    Writes contents of this model to an openNURBS archive.

  Parameters:
    filename - [in]

    version - [in] 
      Version of the openNURBS archive to write.
        0 default value and suggested.
           When 0 is passed in, the value of MYON_BinaryArchive::CurrentArchiveVersion()
           is used.
        2, 3, 4, 50, 60, ...
          If you pass in a value < MYON_BinaryArchive::CurrentArchiveVersion(), then some
          information in current data structures will not be saved in the 3dm archive.
          Rhino 2.x can read version 2 files.
          Rhino 3.x can read version 2 and 3 files.
          Rhino 4.x can read version 2, 3, and 4 files.
          Rhino 5.x can read version 2, 3, 4, 5, and 50 files.
          Rhino 6.x can read version 2, 3, 4, 5, 50, and 60 files.

    error_log - [out]
      any archive writing errors are logged here.

  Returns:
    True if archive is written with no error. 
    False if errors occur.
    Error details are logged in error_log.
  */
  bool Write( 
    const char* filename,
    int version = 0,
    MYON_TextLog* error_log = nullptr
    ) const;

  /*
  Description:
    Writes contents of this model to an openNURBS archive.

  Parameters:
    filename - [in]

    version - [in] 
      Version of the openNURBS archive to write.
        0 default value and suggested.
           When 0 is passed in, the value of MYON_BinaryArchive::CurrentArchiveVersion()
           is used.
        2, 3, 4, 50, 60, ...
          If you pass in a value < MYON_BinaryArchive::CurrentArchiveVersion(), then some
          information in current data structures will not be saved in the 3dm archive.
          Rhino 2.x can read version 2 files.
          Rhino 3.x can read version 2 and 3 files.
          Rhino 4.x can read version 2, 3, and 4 files.
          Rhino 5.x can read version 2, 3, 4, 5, and 50 files.
          Rhino 6.x can read version 2, 3, 4, 5, 50, and 60 files.

    error_log - [out]
      any archive writing errors are logged here.

  Returns:
    True if archive is written with no error. 
    False if errors occur.
    Error details are logged in error_log.
  */
  bool Write( 
    const wchar_t* filename,
    int version = 0,
    MYON_TextLog* error_log = nullptr
    ) const;

  /*
  Description:
    Writes contents of this model to an openNURBS archive.

  Parameters:
    archive - [in]
      archive to write to
      You must call archive.SetArchiveFullPath(...) i order for file references to work correctly.

    version - [in] 
      Version of the openNURBS archive to write.
        0 default value and suggested.
           When 0 is passed in, the value of MYON_BinaryArchive::CurrentArchiveVersion()
           is used.
        2, 3, 4, 50, 60, ...
          If you pass in a value < MYON_BinaryArchive::CurrentArchiveVersion(), then some
          information in current data structures will not be saved in the 3dm archive.
          Rhino 2.x can read version 2 files.
          Rhino 3.x can read version 2 and 3 files.
          Rhino 4.x can read version 2, 3, and 4 files.
          Rhino 5.x can read version 2, 3, 4, 5, and 50 files.
          Rhino 6.x can read version 2, 3, 4, 5, 50, and 60 files.

    error_log - [out]
      any archive writing errors are logged here.

  Returns:
    True if archive is written with no error. 
    False if errors occur.
    Error details are logged in error_log.

  Example:

            model = ...;
            if ( model.IsValid( error_log ) )
            {
              const wchar_t* wsFileName = ....;
              FILE* fp = MYON::OpenFile( wsFileName, L"wb");

              bool ok = false;
              if ( 0 != fp )
              {
                const char* sStartSectionComment = "...";
                int version = 5; // 2, 3, 4 or 5 are valid
                MYON_BinaryFile archive( MYON::archive_mode::write3dm, fp );
                archive.SetArchiveFullPath(wsFileName);
                ok = model.write( archive, 
                                  version, 
                                  sStartSectionComment, 
                                  error_log );
                MYON::CloseFile( fp );
              }
           }

  */
  bool Write( 
    MYON_BinaryArchive& archive,
    int version = 0,
    MYON_TextLog* error_log = nullptr
    ) const;

  /////////////////////////////////////////////////////////////////////
  //
  // BEGIN model definitions
  //

  // 3dm archive start section information
  int m_3dm_file_version = 0;
  unsigned int m_3dm_opennurbs_version = 0;
  MYON__UINT64 m_3dm_file_byte_count = 0;

  MYON_String m_sStartSectionComments;

  // Properties include revision history, notes, information about
  // the application that created the file, and an optional preview image.
  MYON_3dmProperties m_properties;

  // Settings include tolerance, and unit system, and defaults used
  // for creating views and objects.
  MYON_3dmSettings m_settings;

  /*
  Description:
    A manifest of every model component in this MYONX_Model.
  Remarks:
    Use the manifest to find model objects from a name, id or index.

    The manifest Id, Name, and Index values are values used in
    the model. These are assigned when a component is added to the MYONX_Model.
    When possible the id and name are not changed.

    The manifest=model and original component values are different when:
    - The original component Id or Name was not set and a value was automatically
      assigned.
    - The original component Id or Name was not unique and was modified when the component
      was added to the model.
    - Generally the original component index differs from the manifest=model component
      index.
    
    The OriginalToModelMap() can be used to convert original component index 
    and id to the manifest=model index and id.
    
    The ModelToOriginalMap() can be used to manifest=model index and id to
    the original component index and id.
  */
  const MYON_ComponentManifest& Manifest() const;

  /*
  Returns:
    A map from original component index and id to manifest=model index and id.
  Remarks:
    MYON_ManifestMapItem Source = original component index and id.
    MYON_ManifestMapItem Destination = model-manifest index and id.
  */
  const MYON_ManifestMap& OriginalToModelMap() const;

  /*
  Returns:
    A map from manifest=model index and id to original component index and id.
  Remarks:
    MYON_ManifestMapItem Source = model-manifest index and id.
    MYON_ManifestMapItem Destination = original component index and id.
  */
  const MYON_ManifestMap& ModelToOriginalMap() const;

  /*
  Description:
    This number changes every time the content of the MYONX_Model is modified.
  */
  MYON__UINT64 ModelContentVersionNumber() const;

  /*
  Description:
    Add a copy of a model_component to this model.
  model_component - [in]
    A copy of model_component is added to this model.
    The index, id, and name of the copied component are
    set the the model values (Manifest() "Manifest" index, name, and id).
  
  bResolveIdAndNameConflicts - [in]
    If bResolveIdAndNameConflicts is false, then model_component.Id() must be non-nil 
    and not used in this model and model_component.Name() must be correctly set.
    If bResolveIdAndNameConflicts is true, then id and name will be modified
    as needed in the model and manifest.

  Returns:
    A reference to the added model component.
    If the reference is empty (MYON_ModelComponent::IsEmpty() is true)
    then the input was not valid.
  */
  MYON_ModelComponentReference AddModelComponent(
    const class MYON_ModelComponent& model_component,
    bool bResolveIdAndNameConflicts
    );

  MYON_ModelComponentReference AddModelComponent(
    const class MYON_ModelComponent& model_component
    );

  MYON_ModelComponentReference RemoveModelComponent(
    MYON_ModelComponent::Type component_type,
    MYON_UUID component_id
    );
  
  /*
  Description:
    Easy way to add a layer to the model.
  Returns:
    If layer_name is valid, the layer's index (>=0) is returned. Otherwise,
    MYON_UNSET_INT_INDEX is returned.
  */
  int AddLayer(
    const wchar_t* layer_name,
    MYON_Color layer_color
  );

  /*
  Description:
    Easy way to add a default layer to the model.
  Properties:
    layer_name - [in]
      can be nullptr or empty.
    layer_color - [in]
      can be MYON_Color::UnsetColor
  Returns:
    The default layer's index (>=0) is returned.
  */
  int AddDefaultLayer(
    const wchar_t* layer_name,
    MYON_Color layer_color
  );

  /*
  Description:
    Easy way to add a default dimension style to the model.
  Parameters:
    dimension_style_name - [in]
      can be nullptr or empty
    length_unit_system - [in]
      If MYON::LengthUnitSystem::Unset, then settings length unit system is used.
    tolerance - [in]
      If not > 0, then settings tolerance is used.
  Returns:
    The default dimension style's index (>=0) is returned.
  */
  int AddDefaultDimensionStyle(
    const wchar_t* dimension_style_name,
    MYON::LengthUnitSystem length_unit_system,
    double model_tolerance
  );

  /*
  Description:
    Easy way to add a render material to the model. Creates a PBR material.
  Returns:
    If mat_name is valid, the material's index is returned. Otherwise MYON_UNSET_INT_INDEX is returned.
  */
  int AddRenderMaterial(
    const wchar_t* mat_name
  );

  /*
  Description:
    Easy way to add a render environment to the model. Creates a basic environment.
  Returns:
    If env_name is valid, the environment's index is returned. Otherwise MYON_UNSET_INT_INDEX is returned.
  */
  int AddRenderEnvironment(
    const wchar_t* env_name
  );

  /*
  Description:
    Easy way to add a render texture to the model. Creates a bitmap texture. The texture's name is derived from
    the supplied file name. The file must exist locally or the function will fail.
  Returns:
    If the function succeeds, the texture's index is returned. Otherwise MYON_UNSET_INT_INDEX is returned.
  */
  int AddRenderTexture(
    const wchar_t* filename
  );

  /*
  Description:
    Add a managed model component (MYON_Layer, MYON_DimStyle, ...) to this model.

  managed_model_component - [in]
    managed_model_component must be created by operator new and on the heap.
    It will be deleted when the model and last MYON_ModelComponentReference are 
    destroyed.
  
  bResolveIdAndNameConflicts - [in]
    If bResolveIdAndNameConflicts is false, then model_component.Id() must be non-nil 
    and not used in this model and model_component.Name() must be correctly set.
    If bResolveIdAndNameConflicts is true, then id and name will be modified
    as needed in managed_model_component, the model, and the manifest.

  Returns:
    A reference to the added model component.
    If the reference is empty (MYON_ModelComponent::IsEmpty() is true)
    then the input was not valid.
  */
  MYON_ModelComponentReference AddManagedModelComponent(
    class MYON_ModelComponent* managed_model_component,
    bool bResolveIdAndNameConflicts
    );

  MYON_ModelComponentReference AddManagedModelComponent(
    class MYON_ModelComponent* managed_model_component
    );

  /*
  Description:
    Add a model component to this model and control how the model_component instance
    is managed.
  
  model_component - [in]
    An MYON_ModelComponent created on the heap by calling new X where X is
    derived from MYON_ModelComponent.

  bManagedComponent  - [in]
    If bManagedComponent is true, then ~MYONX_Model will delete the component.
    If bManagedComponent is false, then you are responsible for insuring
    the component exists past the destruction of this MYONX_Model.

  bResolveIdAndNameConflicts  - [in]
    If bResolveIdAndNameConflicts is false, then model_component.Id() must be non-nil 
    and not used in this model and model_component.Name() must be correctly set.
    If bResolveIdAndNameConflicts is true, then id and name will be modified
    as needed.
    
  bUpdateComponentIdentification - [in]
    The model_component Index(), Id(), and Name() values are set to match
    the ones used in the model (Manifest() "Manifest" values.)
    
  Returns:
    A reference to the added model component.
    If the reference is empty (MYON_ModelComponentReference::IsEmpty() is true),
    then the input was not valid and the model component was not added.
  */
  MYON_ModelComponentReference AddModelComponentForExperts(
    class MYON_ModelComponent* model_component,
    bool bManagedComponent,
    bool bResolveIdAndNameConflicts,
    bool bUpdateComponentIdentification
    );

  /*
  Description:
    Add an copy of the model_geometry and attributes to this model.

  Parameters:
    geometry_object - [in]
      geometry_object must point to a geometric object (curve, surface, brep, mesh, points, ...),
      a render light, an annotation object, or a detail object.
      A copy of geometry_object is added to and managed by this model.
    attributes - [in]
      nullptr if not available.
      A copy of attributes is added to and managed by this model.
  
  bResolveIdAndNameConflicts - [in]
    If bResolveIdAndNameConflicts is false, then attributes must be nullptr 
    or attributes->m_uid must be non-nil and not used in this model.
    If bResolveIdAndNameConflicts is true, then id will be modified
    as needed.

  Returns:
    A reference to the added model component.
    If the reference is empty (MYON_ModelComponent::IsEmpty() is true)
    then the input was not valid.
  */
  MYON_ModelComponentReference AddModelGeometryComponent(
    const class MYON_Object* geometry_object,
    const class MYON_3dmObjectAttributes* attributes,
    bool bResolveIdAndNameConflicts
    );

  MYON_ModelComponentReference AddModelGeometryComponent(
    const class MYON_Object* geometry_object,
    const class MYON_3dmObjectAttributes* attributes
    );

  /*
  Description:
    Add an copy of the model_geometry and attributes to this model.

  Parameters:
    managed_geometry_object - [in]
      managed_geometry_object must point to an instance geometric object (curve, surface, brep, mesh, points, ...),
      a render light, an annotation object, or a detail object created by operator new and on the heap.
      It will be deleted when the this MYONX_Model and the last MYON_ModelComponentReference are destroyed.

    managed_attributes - [in]
      managed_attributes should be nullptr or point to an instance created by operator new and on the heap.
      It will be deleted when the this MYONX_Model and the last MYON_ModelComponentReference are destroyed.  

  bResolveIdAndNameConflicts - [in]
    If bResolveIdAndNameConflicts is false, then managed_attributes must be nullptr 
    or managed_attributes->m_uuid must be non-nil and not used in this model.
    If bResolveIdAndNameConflicts is true, then id will be modified
    as needed.

  Returns:
    A reference to the added model component.
    If the reference is empty (MYON_ModelComponent::IsEmpty() is true)
    then the input was not valid.
  */
  MYON_ModelComponentReference AddManagedModelGeometryComponent(
    class MYON_Object* managed_geometry_object,
    class MYON_3dmObjectAttributes* managed_attributes,
    bool bResolveIdAndNameConflicts
    );

  MYON_ModelComponentReference AddManagedModelGeometryComponent(
    class MYON_Object* managed_geometry_object,
    class MYON_3dmObjectAttributes* managed_attributes
    );

  /*
  Description:
    Add geometry and attributes to this model and control how the instances are managed.

  Parameters:
    bManageGeometry - [in]
      If true, geometry_object should point to an instance created by operator new and on the heap.
      It will be deleted when the this MYONX_Model and the last MYON_ModelComponentReference are destroyed.
      If false, the expert caller is carefully managing the instance and memory to insure
      model_geometry is a valid instance while this MYONX_Model and any MYON_ModelComponentReference 
      are active.

    geometry_object - [in]
      geometry_object should point to a geometric object (curve, surface, brep, mesh, points, ...),
      a render light, an annotation object, or a detail object.

    bManageAttributes - [in]
      If true, attributes should be nullptr or point to an instance created by operator new and on the heap.
      It will be deleted when the this MYONX_Model and the last MYON_ModelComponentReference are destroyed.
      If false, the expert caller is carefully managing the instance and memory to insure
      attributes is a valid instance while this MYONX_Model and and MYON_ModelComponentReference 
      are active.

    attributes - [in]
      nullptr if not available.
  
  bResolveIdAndNameConflicts - [in]
    If bResolveIdAndNameConflicts is false, then attributes must be nullptr 
    or attributes->m_uid must be non-nil and not used in this model.
    If bResolveIdAndNameConflicts is true, then id will be modified
    as needed.

  Returns:
    A reference to the added model component.
    If the reference is empty (MYON_ModelComponent::IsEmpty() is true)
    then the input was not valid.
  */
  MYON_ModelComponentReference AddModelGeometryComponentForExperts(
    bool bManageGeometry,
    class MYON_Object* geometry_object,
    bool bManageAttributes,
    class MYON_3dmObjectAttributes* attributes,
    bool bResolveIdAndNameConflicts
    );

  unsigned int ComponentIndexLimit(
    MYON_ModelComponent::Type component_type
    ) const;

  /*
  Returns:
    Number of active and deleted components.
    Count does not include system components.
  */
  unsigned int ActiveAndDeletedComponentCount(
    MYON_ModelComponent::Type component_type
    ) const;

  /*
  Returns:
    Number of active components.
    Count does not include system components.
  */
  unsigned int ActiveComponentCount(
    MYON_ModelComponent::Type component_type
    ) const;

  /*
  Returns:
    Number of deleted components.
  */
  unsigned int DeletedComponentCount(
    MYON_ModelComponent::Type component_type
    ) const;

  MYON_ModelComponentReference ComponentFromIndex(
    MYON_ModelComponent::Type component_type,
    int component_model_index
    ) const;    

  MYON_ModelComponentReference ComponentFromUnsignedIndex(
    MYON_ModelComponent::Type component_type,
    unsigned int component_model_index
    ) const;    

  MYON_ModelComponentReference ComponentFromId(
    MYON_ModelComponent::Type component_type,
    MYON_UUID component_model_id
    ) const;  

  MYON_ModelComponentReference ComponentFromName(
    MYON_ModelComponent::Type component_type,
    MYON_UUID component_parent_id,
    const wchar_t* component_model_name
    ) const;

  MYON_ModelComponentReference ComponentFromNameHash(
    MYON_ModelComponent::Type component_type,
    const MYON_NameHash& component_model_name_hash
    ) const;

  /*
  Parameters:
    runtime_serial_number - [in]
      Value of MYON_ModelComponent::RuntimeSerialNumber() to search for.
  Returns:
    If there is a model component with the specified runtime serial number,
    then a reference to that component is returned.
    Otherwise, MYON_ModelComponentReference::Empty is returned.
  Remarks:
    MYONX_Model::ComponentFromRuntimeSerialNumber() used to get a reference rather than a copy of the model's 
    primary MYON_ModelComponentReference. This is the function that must be used if a caller is going to 
    use exclusive access functions like

      MYON_ModelComponent* MYON_ModelComponentReference::ExclusiveModelComponent()
      MYON_3dmObjectAttributes* MYON_ModelGeometryComponent::ExclusiveAttributes()
      MYON_Geometry* MYON_ModelGeometryComponent::ExclusiveGeometry()

    to modify content that is in the MYONX_Model. The exclusive access functions 
    will only return non-nullptr values when there are no external references to
    the model component.
  */
  const MYON_ModelComponentReference& ComponentFromRuntimeSerialNumber(
    MYON__UINT64 runtime_serial_number
  ) const;

  /*
  Description:
    Get an image from its model index.
  Parameters:
    image_model_index - [in]
  Returns:
    An MYON_ModelComponentReference to the image.
  Remarks:
    Model index and Manifest() manifest item index are the same.
  */
  MYON_ModelComponentReference ImageFromIndex(
    int image_model_index
    ) const;

  MYON_ModelComponentReference ImageFromId(
    MYON_UUID image_id
    ) const;

  MYON_ModelComponentReference ImageFromFileFullPath(
    const wchar_t* image_file_full_path_name
    ) const;

  MYON_ModelComponentReference ImageFromFileContent(
    const MYON_ContentHash& image_file_content_hash
    ) const;

  MYON_ModelComponentReference ImageFromFileReference(
    const MYON_FileReference& file_reference
    ) const;

  /*
  Description:
    Get a line pattern from its model index.
  Parameters:
    line_pattern_model_index - [in]
  Returns:
    An MYON_ModelComponentReference to the line pattern.
  Remarks:
    Model index and Manifest() manifest item index are the same.
  */
  MYON_ModelComponentReference LinePatternFromIndex(
    int line_pattern_model_index
    ) const;

  MYON_ModelComponentReference LinePatternFromId(
    MYON_UUID line_pattern_model_id
    ) const;

  MYON_ModelComponentReference LinePatternFromName(
    const wchar_t* line_pattern_name
    ) const;

  MYON_ModelComponentReference LinePatternFromNameHash(
    MYON_NameHash line_pattern_model_name_hash
    ) const;

  /*
  Description:
    Get linetype from object attributes.
  Parameters:
    attributes - [in] object attributes.
    line_pattern - [out] linetype
  */
  MYON_ModelComponentReference LinePatternFromAttributes( 
    const MYON_3dmObjectAttributes& attributes
    ) const;

  MYON_ModelComponentReference LinePatternFromLayerIndex( 
    int layer_index
    ) const;

  // These methods are deprecated due to the use of the phrase 'RenderMaterial' which now refers to
  // materials provided by the RDK. The materials used by _these_ methods are plain MYON_Materials.
  MYON_DEPRECATED MYON_ModelComponentReference RenderMaterialFromLayerIndex(int) const;
  MYON_DEPRECATED MYON_ModelComponentReference RenderMaterialFromAttributes(const MYON_3dmObjectAttributes&) const;
  MYON_DEPRECATED MYON_ModelComponentReference RenderMaterialFromIndex(int) const;
  MYON_DEPRECATED MYON_ModelComponentReference RenderMaterialFromId(MYON_UUID) const;

  /*
  Description:
    Get material from layer index.
  Parameters:
    layer_index - [in] layer index.
  */
  MYON_ModelComponentReference MaterialFromLayerIndex(
    int layer_index
    ) const;

  /*
  Description:
    Get material from object attributes.
  Parameters:
    attributes - [in] object attributes.
  */
  MYON_ModelComponentReference MaterialFromAttributes(
    const MYON_3dmObjectAttributes& attributes
    ) const;

  /*
  Description:
    Get material from index.
  Parameters:
    material_index - [in] index.
  */
  MYON_ModelComponentReference MaterialFromIndex(
    int material_index
    ) const;

  /*
  Description:
    Get material from id.
  Parameters:
    material_id - [in] id.
  */
  MYON_ModelComponentReference MaterialFromId(
    MYON_UUID material_id
    ) const;

  /*
  Description:
    Get a layer from its model index.
  Parameters:
    layer_model_index - [in]
  Returns:
    An MYON_ModelComponentReference to the layer.
  Remarks:
    Model index and Manifest() manifest item index are the same.
  */
  MYON_ModelComponentReference LayerFromIndex(
    int layer_model_index
    ) const;

  MYON_ModelComponentReference LayerFromId(
    MYON_UUID layer_model_id
    ) const;

  MYON_ModelComponentReference LayerFromName(
    MYON_UUID layer_parent_id,
    const wchar_t* layer_name
    ) const;

  MYON_ModelComponentReference LayerFromNameHash(
    const MYON_NameHash& layer_model_name_hash
    ) const;

  MYON_ModelComponentReference LayerFromAttributes( 
    const MYON_3dmObjectAttributes& attributes
    ) const;

  /*
  Description:
    Get a dimension style from its model index.
  Parameters:
    dimension_style_model_index - [in]
  Returns:
    An MYON_ModelComponentReference to the dimension style.
  Remarks:
    Model index and Manifest() manifest item index are the same.
  */
  MYON_ModelComponentReference DimensionStyleFromIndex(
    int dimension_style_index
    ) const;
  MYON_ModelComponentReference DimensionStyleFromId(
    MYON_UUID dimension_styleid
    ) const;
  MYON_ModelComponentReference DimensionStyleFromName(
    const wchar_t* dimension_style_name
    ) const;
  MYON_ModelComponentReference DimensionStyleFromNameHash(
    MYON_NameHash dimension_style_name_hash
    ) const;

  /*
  Returns:
    Id of the current dimension style or nil if the current style is 
    not set or not in this model.
  */
  MYON_UUID CurrentDimensionStyleId() const;

  /*
  Parameters:
    dimension_style_id - [in]
      Id of a dimension style in this model, a system dimension style, or MYON_nil_uuid.
  Returns:
    true if dimension_style_id is valid and is set.
  */
  bool SetCurrentDimensionStyleId(
    MYON_UUID dimension_style_id
  );

  /*
  Returns:
    Current dimension style
    = DimensionStyleFromId(CurrentDimensionStyleId())
  */
  MYON_ModelComponentReference CurrentDimensionStyle() const;

  /*
  Returns:
    A system dimension style that is the default for this model
    and is used when a referenced dimension style is missing from
    this model.
  */
  MYON_ModelComponentReference DefaultDimensionStyle() const;

  /*
  Parameters:
    font - [in]
    model_space_text_scale - [in]
      If model_space_text_scale > 0, then the DimScale() must be equal to model_space_text_scale.
    bIgnoreSystemDimStyles - [in]
  Returns:
    The first dimension style with the specified font.
  Remarks:
    dimension styles with a non-nil parent id are ignored.
  */
  MYON_ModelComponentReference FirstDimensionStyleFromFont(
    const MYON_Font* font,
    double model_space_text_scale,
    bool bIgnoreSystemDimStyles
    ) const;

  /*
  Parameters:
    managed_font_serial_number - [in]
    model_space_text_scale - [in]
      If model_space_text_scale > 0, then the DimScale() must be equal to model_space_text_scale.
    bIgnoreSystemDimStyles - [in]
  Returns:
    The first dimension style with the specified font.
  Remarks:
    dimension styles with a non-nil parent id are ignored.
  */
  MYON_ModelComponentReference FirstDimensionStyleFromManagedFontSerialNumber(
    unsigned int managed_font_serial_number,
    double model_space_text_scale,
    bool bIgnoreSystemDimStyles
    ) const;

  /*
  Description:
    Find or create a dimension style with the specified font characteristics.
  */
  MYON_ModelComponentReference DimensionStyleWithFontCharacteristics(
    const MYON_Font& font_characteristics,
    double model_space_text_scale
    );

  /*
  Description:
    Find a model geometry component from Id
  Parameters:
    model_geometry_component_id - [in]
  Returns:
    If there is a model geometry component with the id, it is returned.
    Otherwise, MYON_ModelComponentReference::Empty is returned.
  */
  MYON_ModelComponentReference ModelGeometryFromId(
    MYON_UUID model_geometry_component_id
    ) const;

  /*
  Description:
    Find a model geometry component from Id
  Parameters:
    model_geometry_component_id - [in]
  Returns:
    If there is a model geometry component with the id, it is returned.
    Otherwise, MYON_ModelGeometryComponent::Unset is returned.
  */
  const MYON_ModelGeometryComponent& ModelGeometryComponentFromId(
    MYON_UUID model_geometry_component_id
    ) const;

public:
  MYON_SimpleArray<MYONX_Model_UserData*> m_userdata_table;

private:
  MYON_ModelComponentReference m_default_render_material = MYON_ModelComponentReference::CreateConstantSystemComponentReference(MYON_Material::Default);
  MYON_ModelComponentReference m_default_line_pattern = MYON_ModelComponentReference::CreateConstantSystemComponentReference(MYON_Linetype::Continuous);
  MYON_ModelComponentReference m_default_layer = MYON_ModelComponentReference::CreateConstantSystemComponentReference(MYON_Layer::Default);
  MYON_ModelComponentReference m_default_text_style = MYON_ModelComponentReference::CreateConstantSystemComponentReference(MYON_TextStyle::Default);
  MYON_ModelComponentReference m_default_dimension_style = MYON_ModelComponentReference::CreateConstantSystemComponentReference(MYON_DimStyle::Default);

private:
  MYON_ModelComponentReference Internal_AddModelComponent(
    MYON_ModelComponent* model_component,
    MYON_UUID id,
    MYON_UUID name_parent_id,
    const MYON_wString& name,
    bool bManagedComponent,
    bool bUpdateComponentIdentification
    );

private: // Private extension.
  friend class MYONX_ModelPrivate;
  class MYONX_ModelPrivate* m_private;

private:
  void Internal_IncrementModelContentVersionNumber();

private:
  // A manifest of everything in the model. Use the manifest to find
  // objects from a name, id or index.
  MYON_ComponentManifest m_manifest;
  MYON_ManifestMap m_original_to_manifest_map;
  MYON_ManifestMap m_manifest_to_original_map;

private:
  friend class MYONX_ModelComponentIterator;
  class MYONX_ModelComponentReferenceLink* Internal_ModelComponentLinkFromSerialNumber( 
    MYON__UINT64 model_component_runtime_serial_number 
    ) const;
  class MYONX_ModelComponentReferenceLink* Internal_AddModelComponentReference(
    MYON_ModelComponentReference mcr
    );
  void Internal_RemoveModelComponentReferenceLink(
    class MYONX_ModelComponentReferenceLink* mcr_link
    );
  // A map used to lookup by serial number.
  MYON_SerialNumberMap m_mcr_sn_map;
  MYON_FixedSizePool m_mcr_link_fsp;

#pragma MYON_PRAGMA_WARNING_PUSH
#pragma MYON_PRAGMA_WARNING_DISABLE_MSC( 4251 ) 
  // C4251: "...needs to have dll-interface to be used by clients of class". This warning is not correct.
  // m_mcr_lists is private and all code that manages it is explicitly implemented in the DLL.
  class MYONX_ModelComponentList
  {
  public:
    MYON_ModelComponent::Type m_component_type = MYON_ModelComponent::Type::Unset;
    unsigned int m_count = 0;
    class MYONX_ModelComponentReferenceLink* m_first_mcr_link = nullptr;
    class MYONX_ModelComponentReferenceLink* m_last_mcr_link = nullptr;
  };

  //-------------------------------------------------------------------------------------------
  // The following two lines are deprecated due to limiting the number of types to 16.
  // They are no longer used. Instead, a new list inside the extension is used. This is private
  // and is only used internally to MYONX_Model but it can't be removed without breaking the SDK.
  //-------------------------------------------------------------------------------------------
  enum : unsigned int { MYONX_MCR_LIST_COUNT = 16 };        // DEPRECATED -- not used.
  MYONX_ModelComponentList m_mcr_lists[MYONX_MCR_LIST_COUNT]; // DEPRECATED -- not used.
  //-------------------------------------------------------------------------------------------

  const MYONX_ModelComponentList& Internal_ComponentListConst(MYON_ModelComponent::Type component_type) const;
  MYONX_ModelComponentList& Internal_ComponentList(MYON_ModelComponent::Type component_type);
#pragma MYON_PRAGMA_WARNING_POP

public:
  bool ValdateComponentIdAndName(
    MYON_ModelComponent::Type component_type,
    const MYON_UUID& candidate_id,
    const MYON_UUID& component_parent_id,
    const wchar_t* candidate_name,
    bool bResolveIdConflict,
    bool bResolveNameConflict,
    MYON_UUID& model_id,
    MYON_wString& model_name
    ) const;

  //
  // END model definitions
  //
  /////////////////////////////////////////////////////////////////////

public:
  /*
  Returns:
    Bounding box of every object in m_object_table[].
  */
  MYON_BoundingBox ModelGeometryBoundingBox() const;

  /*
  Returns:
    Bounding box of every render light in m_light_table[].
  */
  MYON_BoundingBox RenderLightBoundingBox() const;

private:
  void Internal_ComponentTypeBoundingBox(
    const MYON_ModelComponent::Type component_type,
    MYON_BoundingBox& bbox
    ) const;

public:
  /*
  Description:
    Get wireframe drawing color from object attributes.
  Parameters:
    attributes - [in] object attributes.
  Returns:
    Wireframe drawing color.
  */
  MYON_Color WireframeColorFromAttributes(
    const MYON_3dmObjectAttributes& attributes
    ) const;

  /* 
  Description:
    See if the instance reference iref refers to an instance
    definition.
  Parameters:
    iref - [in]
    idef_uuid - [in] id of idef we are looking for
  Returns:
    @untitled table
     0         iref does not use idef
     1         iref directly references idef
    >1         iref has a nested reference to idef (nesting depth returned)
    -1         iref.m_instance_definition_uuid is not valid
    -2         invalid idef found
  */
  int UsesIDef( 
    const MYON_InstanceRef& iref,
    MYON_UUID idef_uuid
    ) const;

  /////////////////////////////////////////////////////////////////////
  //
  // BEGIN model document level user string tools
  //

  /*
  Description:
    Attach a user string to the document.
  Parameters:
    key - [in] id used to retrieve this string.
    string_value - [in] 
      If nullptr, the string with this id will be removed.
  Returns:
    True if successful.
  */
  bool SetDocumentUserString( 
    const wchar_t* key, 
    const wchar_t* string_value 
    );

  /*
  Description:
    Get user string from the document.
  Parameters:
    key - [in] id used to retrieve the string.
    string_value - [out]
  Returns:
    True if a string with id was found.
  */
  bool GetDocumentUserString( 
    const wchar_t* key, 
    MYON_wString& string_value 
    ) const;

  /*
  Description:
    Get a list of all user strings in the document.
  Parameters:
    user_strings - [out]
      user strings are appended to this list.
  Returns:
    Number of elements appended to the user_strings list.
  */
  int GetDocumentUserStrings( MYON_ClassArray<MYON_UserString>& user_strings ) const;

  //
  // END model document level user string tools
  //
  /////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////
  //
  // BEGIN model text dump tools
  //

  // text dump of entire model
  void Dump( MYON_TextLog& ) const;
  
  // text dump of model properties and settings
  void DumpSummary( MYON_TextLog& ) const;

  // text dump of user data table
  void DumpUserDataTable( MYON_TextLog& ) const;

  void DumpComponentList( 
    MYON_ModelComponent::Type component_type,
    MYON_TextLog& text_log
    ) const;

  /*
  Returns:
    A text dump of all component lists.
  */
  void DumpComponentLists(
    MYON_TextLog& text_log
  ) const;

  /*
  Returns:
    A SHA-1 hash of the model's content. If two models have identical content,
    then the have equal ContentHash() values.
  */
  MYON_SHA1_Hash ContentHash() const;

private:
  void Internal_DumpSummary(
    MYON_TextLog& dump, 
    bool bInvariantContentOnly
  ) const;

  //
  // END model text dump tools
  //
  /////////////////////////////////////////////////////////////////////

public:
  /////////////////////////////////////////////////////////////////////
  //
  // BEGIN Rendering information
  //

  MYON_DEPRECATED_MSG("This function is deprecated.")
  static bool IsRDKDocumentInformation(const MYONX_Model_UserData& docud);

  MYON_DEPRECATED_MSG("This function is deprecated.")
  static bool GetRDKDocumentInformation(const MYONX_Model_UserData& docud, MYON_wString& rdk_xml_document_data);

  MYON_DEPRECATED_MSG("This function is deprecated as it did not return the buffer sizes, making it useless")
  static bool GetRDKEmbeddedFiles(const MYONX_Model_UserData& docud, MYON_ClassArray<MYON_wString>& paths, MYON_SimpleArray<unsigned char*>& embedded_files_as_buffers);

  MYON_DEPRECATED_MSG("This function is deprecated in favor of MYON_EmbeddedFile.")
  static bool GetRDKEmbeddedFiles(const MYONX_Model_UserData& docud, MYON_ClassArray<MYON_wString>& paths, MYON_SimpleArray<unsigned char*>& embedded_files_as_buffers, MYON_SimpleArray<size_t>& buffer_sizes);

  MYON_DEPRECATED_MSG("This function is deprecated in favor of MYON_EmbeddedFile.")
  static bool GetRDKEmbeddedFilePaths(const MYONX_Model_UserData& docud, MYON_ClassArray<MYON_wString>& paths);

  MYON_DEPRECATED_MSG("This function is deprecated in favor of MYON_EmbeddedFile.")
  static bool GetRDKEmbeddedFile(const MYONX_Model_UserData& docud, const wchar_t* path, MYON_SimpleArray<unsigned char>& bytes);

  MYON_DEPRECATED_MSG("This function is deprecated.")
  static bool IsRDKObjectInformation(const MYON_UserData& objectud);

  MYON_DEPRECATED_MSG("This function is deprecated. Please use GetDecalIterator(), AddDecal() and GetDecal() instead.")
  static bool GetRDKObjectInformation(const MYON_Object& object, MYON_wString& xml);

  //
  // END Rendering information
  //
  /////////////////////////////////////////////////////////////////////

private:
  mutable MYON_BoundingBox m_model_geometry_bbox = MYON_BoundingBox::UnsetBoundingBox;
  mutable MYON_BoundingBox m_render_light_bbox = MYON_BoundingBox::UnsetBoundingBox;
  class MYON_DocumentUserStringList* m_model_user_string_list = nullptr;
};

class MYON_CLASS MYONX_ModelComponentIterator
{
public:
  MYONX_ModelComponentIterator() = default;
  ~MYONX_ModelComponentIterator() = default;
  MYONX_ModelComponentIterator(const MYONX_ModelComponentIterator&) = default;
  MYONX_ModelComponentIterator& operator=(const MYONX_ModelComponentIterator&) = default;

  MYONX_ModelComponentIterator(
    const MYONX_Model& model,
    MYON_ModelComponent::Type component_type
    );

  const MYONX_Model* Model() const;

  MYON_ModelComponentReference FirstComponentReference();
  MYON_ModelComponentReference LastComponentReference();
  MYON_ModelComponentReference CurrentComponentReference() const;
  MYON_ModelComponentReference NextComponentReference();
  MYON_ModelComponentReference PreviousComponentReference();

  MYON_ModelComponentWeakReference FirstComponentWeakReference();
  MYON_ModelComponentWeakReference LastComponentWeakReference();
  MYON_ModelComponentWeakReference NextComponentWeakReference();
  MYON_ModelComponentWeakReference PreviousComponentWeakReference();
  MYON_ModelComponentWeakReference CurrentComponentWeakReference() const;

  // Use these with caution unless it is clear you are the only thread
  // with references to the model and the iterator.
  const MYON_ModelComponent* FirstComponent();
  const MYON_ModelComponent* LastComponent();
  const MYON_ModelComponent* CurrentComponent() const;
  const MYON_ModelComponent* NextComponent();
  const MYON_ModelComponent* PreviousComponent();

  /*
  Returns:
    Number of active components in the current model.
  Remarks:
    If the model is modified during iteration, this value will changes.
  */
  unsigned int ActiveComponentCount() const;
  
private:
  const class MYONX_Model::MYONX_ModelComponentList* Internal_List() const;
  void Internal_SetLink(const class MYONX_ModelComponentReferenceLink* link) const;
  void Internal_SetLink(MYON__UINT64 model_component_sn) const;

  MYON_ModelComponent::Type m_component_type = MYON_ModelComponent::Type::Unset;
  const class MYONX_Model* m_model = nullptr;
  mutable MYON__UINT64 m_model_content_version = 0;
  mutable const class MYONX_Model::MYONX_ModelComponentList* m_list = nullptr;
  mutable const class MYONX_ModelComponentReferenceLink* m_link = nullptr;
  mutable MYON__UINT64 m_current_component_sn = 0;
  mutable MYON__UINT64 m_next_component_sn = 0;
  mutable MYON__UINT64 m_prev_component_sn = 0;

  // The current component is a weak ref so that a stand alone iterator cannot
  // keep the current element alive since iterations often involve deletion.
  // The iterators next/prev will still work as expected when the current element
  // is deleted. In particular, an iterator can be used to efficiently delete
  // portions of a model and have the deletion occur when many people
  // expect it to occur and not at a later time. This makes debugging 
  // invalid deletions much easier.
  mutable MYON_ModelComponentWeakReference m_current_component_weak_ref;
};

class MYON_CLASS MYONX_ModelTest
{
public:
  MYONX_ModelTest() = default;
  ~MYONX_ModelTest() = default;
  MYONX_ModelTest(const MYONX_ModelTest&) = default;
  MYONX_ModelTest& operator=(const MYONX_ModelTest&) = default;

  static const MYONX_ModelTest Unset;

public:

#pragma region // XXRH_C_SHARED_ENUM // [MYONX_ModelTest::Type] [Rhino.Geometry.Something.Type] [nested:byte]
  /// <summary>
  /// MYONX_ModelTest::Type identifies the type of file reading test to perform.
  /// </summary>
  enum class Type : unsigned char
  {
    Unset = 0,

    ///<summary>
    /// Read the source 3dm file.
    ///</summary>
    Read = 1,

    ///<summary>
    /// Read the source 3dm file and write one or two temporary 3dm files. The original
    /// source file is not modified. If the 3dm version of the source file
    /// is < MYON_BinaryArchive::CurrentArchiveVersion(), then two temporary 3dm
    /// files are written, the first with 3dm version = MYON_BinaryArchive::CurrentArchiveVersion()-10
    /// and the second with 3dm version = MYON_BinaryArchive::CurrentArchiveVersion().
    /// For example, if Rhino 6 is the current version of Rhino and a file written 
    /// by Rhino 5 is read, then both a temporary Rhino 5 and a temporary Rhino 6 3dm
    /// file are written.
    ///</summary>
    ReadWrite = 2,

    ///<summary>
    /// Perform the ReadWrite test and read the temporary files.
    ///</summary>
    ReadWriteRead = 3,

    ///<summary>
    /// Perform the ReadWriteRead test. If one of the temporary files has the same 3dm version
    /// as the original source file, verify that the MYONX_Models created by reading the original
    /// 3dm file and the temporary 3dm file with the same version have identical values
    /// of MYONX_Model::ContentHash().
    ///</summary>
    ReadWriteReadCompare = 4
  };
#pragma endregion

  static const char* TestTypeToString(MYONX_ModelTest::Type test_type);
  static const wchar_t* TestTypeToWideString(MYONX_ModelTest::Type test_type);

#pragma region // XXRH_C_SHARED_ENUM // [MYONX_ModelTest::Result] [Rhino.Geometry.Something.Result] [nested:byte]
  /// <summary>
  /// MYONX_ModelTest::Result reports the result of a test.
  /// </summary>
  enum class Result : unsigned char
  {
    ///<summary>
    /// Test result is not set.
    ///</summary>
    Unset = 0,

    ///<summary>
    /// Test failed to complete.
    ///</summary>
    Fail = 1,

    ///<summary>
    /// Test was performed and completed, but at least one MYON_ERROR occurred.
    ///</summary>
    Errors = 2,

    ///<summary>
    /// Test was performed and completed, but at least one MYON_WARNING occurred.
    ///</summary>
    Warnings = 3,
 

    ///<summary>
    /// Test was performed and passed.
    ///</summary>
    Pass = 4,

    ///<summary>
    /// Test was not performed because the input did not satisfy prerequisites or an
    /// earlier test failed.
    /// For example, if a MYONX_ModelReadTest::TestType::ReadWriteReadCompare 
    /// test is requested and the source file is a Rhino 1 file, the compare
    /// test is skipped.
    /// For example, if a MYONX_ModelReadTest::TestType::ReadWriteRead 
    /// test is requested and the Write test fails, the second Read test is skipped.
    ///</summary>
    Skip = 5,
  };
#pragma endregion

  static const char* ResultToString(MYONX_ModelTest::Result result);
  static const wchar_t* ResultToWideString(MYONX_ModelTest::Result result);

  static MYONX_ModelTest::Result WorstResult(
    MYONX_ModelTest::Result a,
    MYONX_ModelTest::Result b
  );

  /*
  Parameters:
    error_count - [in]
    no_errors_result - [in]
      result to return when 0 = error_count.TotalCount().
  */
  static MYONX_ModelTest::Result ResultFromErrorCounter(
    MYONX_ErrorCounter error_count,
    MYONX_ModelTest::Result no_errors_result
    );

  /*
  Description:
    Test MYONX_Model::Read() and MYONX_Model::Write().
  Parameters:
    file_path - [in]
      file path
    test_type - [in]
      test to perform.
    bKeepModels - [in]
      If true, then the MYONX_Models created by reading 3dm archives are saved
      so the can be examined after the tests complete.
    text_log_file_path - [in]
      If not empty, the string to use for file_path in the output text_log.
      This is used to create logs on different computers that can be compared.
    text_log - [in]
      If text_log is not nullptr, then a summary of the test is sent to text_log.
  Returns:
    True if every test passed with no warnings or errors.
    False if a test failed or warnings or errors occurred.
  */
  bool ReadTest(
    const char* file_path,
    MYONX_ModelTest::Type test_type,
    bool bKeepModels,
    const char* text_log_file_path,
    MYON_TextLog* text_log
  );

  /*
  Description:
    MYONX_Model::ReadTest() can be used to test reading a specific file.
  Parameters:
    file_path - [in]
      file path
    test_type - [in]
      test to perform.
    bKeepModels - [in]
      If true, then the MYONX_Models created by reading 3dm archives are saved
      so the can be examined after the tests complete.
    text_log_file_path - [in]
      If not empty, the string to use for file_path in the output text_log.
      This is used to create logs on different computers that can be compared.
    text_log - [in]
      If text_log is not nullptr, then a summary of the test is sent to text_log.
  Returns:
    True if every test passed with no warnings or errors.
    False if a test failed or warnings or errors occurred.
  */
  bool ReadTest(
    const wchar_t* file_path,
    MYONX_ModelTest::Type test_type,
    bool bKeepModels,
    const wchar_t* text_log_file_path,
    MYON_TextLog* text_log
  );

  /*
  Description:
    MYONX_Model::ReadTest() can be used to test reading a specific file.
  Parameters:
    fp - [in]
      fp pointer to a file opened with MYON_FileStream::Opent(...,"rb");
    test_type - [in]
      test to perform.
    bKeepModels - [in]
      If true, then the MYONX_Models created by reading 3dm archives are saved
      so the can be examined after the tests complete.
    text_log_file_path - [in]
      If not empty, the string to use for file_path in the output text_log.
      This is used to create logs on different computers that can be compared.
    text_log - [in]
      If text_log is not nullptr, then a summary of the test is sent to text_log.
  Returns:
    True if every test passed with no warnings or errors.
    False if a test failed or warnings or errors occurred.
  */
  bool ReadTest(
    FILE* fp,
    MYONX_ModelTest::Type test_type,
    bool bKeepModels,
    const wchar_t* text_log_file_path,
    MYON_TextLog* text_log
  );


  /*
  Description:
    MYONX_Model::Test() can be used to test reading a specific file.
  Parameters:
    archive - [in]      
    test_type - [in]
      test to perform.
    bKeepModels - [in]
      If true, then the MYONX_Models created by reading 3dm archives are saved
      so the can be examined after the tests complete.
    text_log_file_path - [in]
      If not empty, the string to use for file_path in the output text_log.
      This is used to create logs on different computers that can be compared.
    text_log - [in]
      If text_log is not nullptr, then a summary of the test is sent to text_log.
  Returns:
    True if every test passed with no warnings or errors.
    False if a test failed or warnings or errors occurred.
  */
  bool ReadTest(
    MYON_BinaryArchive& archive,
    MYONX_ModelTest::Type test_type,
    bool bKeepModels,
    const wchar_t* text_log_file_path,
    MYON_TextLog* text_log
  );

  /*
  Description:
    Prints test results.
  */
  void Dump(MYON_TextLog& text_log) const;


  /*
  Description:
    Prints the model context to text_log.
  */
  static bool DumpModel(const MYONX_Model* model, MYON_TextLog& text_log);

  /*
  Description:
    Prints the source model context to text file next to the source file
    with the file _ONX_ModelText_original_<PLATFORM>.txt appended to the 
    source file name.
  Remark:
    Call after test is completed.
  */
  bool DumpSourceModel() const;

  /*
  Description:
    Prints the source model context to text_log.
  Remark:
    Call after test is completed.
  */
  bool DumpSourceModel(const wchar_t* text_file_full_path) const;

  /*
  Description:
    Prints the source model context to text_log.
  Remark:
    Call after test is completed.
  */
  bool DumpSourceModel(MYON_TextLog& text_log) const;

  /*
  Description:
    Prints the model obtained from the last read in the read-write-read test to
    with the file _ONX_ModelText_copy_<PLATFORM>.txt appended to the 
    original source file name.
  Remark:
    Call after test is completed.
  */
  bool DumpReadWriteReadModel() const;

  /*
  Description:
    Prints the model obtained from the last read in the read-write-read test to
    with the file _ONX_ModelText_copy_<PLATFORM>.txt appended to a text file
    with the specified name.
  Remark:
    Call after test is completed.
  */
  bool DumpReadWriteReadModel(const wchar_t* text_file_full_path) const;

  /*
  Description:
    Prints the model obtained from the last read in the read-write-read test to
    with the file _ONX_ModelText_copy_<PLATFORM>.txt appended to the text_log.
  Remark:
    Call after test is completed.
  */
  bool DumpReadWriteReadModel(MYON_TextLog& text_log) const;

private:
  void Internal_BeginTest();

  void Internal_EndCurrentTest();

  void Internal_BeginNextTest(
    MYONX_ModelTest::Type test_type
  );


  void Internal_ReadTest(
    MYON_BinaryArchive& archive,
    MYONX_ModelTest::Type test_type,
    bool bKeepModels,
    const wchar_t* text_log_file_path,
    MYON_TextLog* text_log
  );

  bool Internal_TallyTestResults();

public:

  // Test that was performed.
  MYONX_ModelTest::Type TestType() const;

  /*
  Returns:
    The name of the source 3dm file.
  */
  const MYON_wString Source3dmFilePath() const;

  /*
  Returns:
    The string used in the output log to identify the source 3dm file.
  */
  const MYON_wString TextLogSource3dmFilePath() const;

  /*
  Returns:
    Version of the 3dm fie, 1,2,3,4,5,50,60,...
  */
  unsigned int Source3dmFileVersion() const;
  
  /*
  Returns:
    Worst result for any test that was attempted.
  */
  MYONX_ModelTest::Result TestResult() const;
 
  /*
  Parameters:
    test_type - [in]
  Returns:
    Result of the test identified by the test_type parameter.
  */
  MYONX_ModelTest::Result TestResult(
    MYONX_ModelTest::Type test_type
  );

  static bool SkipCompare(
  unsigned int source_3dm_file_version
  );

  /*
  Returns:
    Total number of failures, errors, and warnings for all tests that
    were performed.
  */
  MYONX_ErrorCounter ErrorCounter() const;
 
  /*
  Returns:
    Total number of failures, errors, and warnings for all tests that
    were performed.
  */
  MYONX_ErrorCounter ErrorCounter(
   MYONX_ModelTest::Type test_type
   ) const;

  const MYON_SHA1_Hash SourceModelHash();
  const MYON_SHA1_Hash ReadWriteReadModelHash();

  /*
  Returns:
    nullptr if the test was run with bKeepModels=false or the 
    source archive could not be read.
    Otherwise, a pointer to the source model.
  */
  std::shared_ptr<MYONX_Model> SourceModel() const;

  /*
  Returns:
    nullptr if the read write read test was not performed or was run with bKeepModels=false.
    Otherwise, a pointer to the result of the read write read test.
  */
  std::shared_ptr<MYONX_Model> ReadWriteReadModel() const;


private:
  MYONX_ModelTest::Type m_test_type = MYONX_ModelTest::Type::Unset;

  MYON_wString m_source_3dm_file_path;

  // if set, used when printing the name of m_source_3dm_file_path in the text
  // log so results from different computers can be compared.
  MYON_wString m_text_log_3dm_file_path;

  unsigned int m_model_3dm_file_version[3] = {0};

  unsigned int m_current_test_index = 0;

  MYONX_ModelTest::Result m_test_result = MYONX_ModelTest::Result::Unset;
  MYONX_ModelTest::Result m_test_results[7] = {};

  MYONX_ErrorCounter m_error_count;
  MYONX_ErrorCounter m_error_counts[7];

#pragma MYON_PRAGMA_WARNING_PUSH
#pragma MYON_PRAGMA_WARNING_DISABLE_MSC( 4251 ) 
  // C4251: ... : class 'std::shared_ptr<MYON_MeshThicknessAnalysisImpl>' 
  //        needs to have dll-interface to be used by clients ...
  // m_model[] is private and all code that manages m_sp is explicitly implemented in the DLL.

  // m_model[0] = model from source file
  // m_model[1] = model[0] -> write to current 3dm version -> read into model[1]
  // m_model[2] = model[0] -> write to prev 3dm version -> read into model[2]
  std::shared_ptr<MYONX_Model> m_model[3];
#pragma MYON_PRAGMA_WARNING_POP

  // m_model_hash[i] = m_model[0].Hash()
  MYON_SHA1_Hash m_model_hash[3];
};


#endif

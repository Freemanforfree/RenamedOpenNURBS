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

#if !defined(MYON_MODEL_GEOMETRY_INC_)
#define MYON_MODEL_GEOMETRY_INC_

/*
Description:
  Used to store geometry table object definition and attributes in an MYONX_Model.
*/
class MYON_CLASS MYON_ModelGeometryComponent : public MYON_ModelComponent
{
  MYON_OBJECT_DECLARE(MYON_ModelGeometryComponent);

public:
  static const MYON_ModelGeometryComponent Unset;

  static const MYON_ModelGeometryComponent* FromModelComponentRef(
    const class MYON_ModelComponentReference& model_component_reference,
    const MYON_ModelGeometryComponent* none_return_value
    );

  bool UpdateReferencedComponents(
    const class MYON_ComponentManifest& source_manifest,
    const class MYON_ComponentManifest& destination_manifest,
    const class MYON_ManifestMap& manifest_map
    ) override;
  
  bool IsEmpty() const;

  bool IsInstanceDefinitionGeometry() const;

private:

public:
  MYON_ModelGeometryComponent()  MYON_NOEXCEPT;

  MYON_ModelGeometryComponent(
    MYON_ModelComponent::Type type
  ) MYON_NOEXCEPT;

  ~MYON_ModelGeometryComponent();
  MYON_ModelGeometryComponent(const MYON_ModelGeometryComponent&);
  MYON_ModelGeometryComponent& operator=(const MYON_ModelGeometryComponent&);

#if defined(MYON_HAS_RVALUEREF)
  // rvalue copy constructor
  MYON_ModelGeometryComponent( MYON_ModelGeometryComponent&& ) MYON_NOEXCEPT;
  // rvalue assignment operator
  MYON_ModelGeometryComponent& operator=( MYON_ModelGeometryComponent&& );
#endif

  void Dump(
    MYON_TextLog& text_log
    ) const override;

  /*
  Parameters:
    geometry - [in]
      MYON_Curve, MYON_Surface, MYON_Brep, MYON_Mesh, MYON_Light, annotation, detail, ...
      A new copy of the geometry is managed by the MYON_ModelGeometryComponent class.
    attributes - [in]
      nullptr if not available.
      A new copy of the attributes is managed by the MYON_ModelGeometryComponent class.
    model_geometry_component - [in]
      If not nullptr, this class is set. Otherwise operator new allocates
      an MYON_ModelGeometryComponent class.
  Remarks:
    The returned MYON_ModelGeometryComponent manages geometry and attributes and will
    eventually delete them.
  */
  static MYON_ModelGeometryComponent* Create(
    const class MYON_Object& model_geometry,
    const class MYON_3dmObjectAttributes* attributes,
    MYON_ModelGeometryComponent* model_geometry_component
    );

  /*
  Parameters:
    geometry_object - [in]
      MYON_Curve, MYON_Surface, MYON_Brep, MYON_Mesh, MYON_Light, annotation, detail, ...
      geometry_object was created on the heap using operator new and 
      the MYON_ModelGeometryComponent destructor will delete geometry_object.
    attributes - [in]
      attributes is nullptr or was created on the heap using operator new
      and the MYON_ModelGeometryComponent destructor will delete attributes.
    model_geometry - [in]
      If not nullptr, this class is set. Otherwise operator new allocates
      an MYON_ModelGeometryComponent class.
  Remarks:
    The returned MYON_ModelGeometryComponent manages geometry_object and attributes and will
    eventually delete them.
  */
  static MYON_ModelGeometryComponent* CreateManaged(
    class MYON_Object* geometry_object,
    class MYON_3dmObjectAttributes* attributes,
    MYON_ModelGeometryComponent* model_geometry_component
    );

  /*
  Parameters:
    bManageGeometry - [in]
      If true, geometry_object was created on the heap using operator new and 
      the MYON_ModelGeometryComponent destructor will delete geometry_object. Otherwise
      the expert caller is carefully managing the geometry_object instance and memory.
    geometry_object - [in]
      MYON_Curve, MYON_Surface, MYON_Brep, MYON_Mesh, MYON_Light, annotation, detail, ...
    bManageAttributes - [in]
      If true, attributes is nullptr or was created on the heap using operator new
      and the MYON_ModelGeometryComponent destructor will delete attributes. Otherwise
      the expert caller is carefully managing the attributes instance and memory.
    attributes - [in]
      nullptr if not available
    model_geometry_component - [in]
      If not nullptr, this class is set. Otherwise operator new allocates
      an MYON_ModelGeometryComponent class.
  */
  static MYON_ModelGeometryComponent* CreateForExperts(
    bool bManageGeometry,
    class MYON_Object* geometry_object,
    bool bManageAttributes,
    class MYON_3dmObjectAttributes* attributes,
    MYON_ModelGeometryComponent* model_geometry_component
    );

  /*
  Description:
    Get a pointer to geometry. The returned pointer may be shared
    and should not be used to modify the geometry.
  Parameters:
    no_geometry_return_value - [in]
      This value is returned if no geometric object has been set.
      A good choices for this parameter's value depends on the context.
      Common options are nullptr.
  Returns:
    The curve, surface, annotation, detail, light, ... geometry, 
    or no_geometry_return_value if the geometry has not been set.    
    If the geometry is a light, then ComponentType() will return MYON_ModelComponent::Type::RenderLight.    
    If the geometry is set and something besides light, then ComponentType() 
    will return MYON_ModelComponent::Type::ModelGeometry.
    Otherwise, ComponentType() will return MYON_ModelComponent::Type::ModelGeometry::Unset.
  See Also:
    MYON_ModelGeometryComponent::Attributes()
    MYON_ModelGeometryComponent::Geometry()
    MYON_ModelGeometryComponent::ExclusiveAttributes()
    MYON_ModelGeometryComponent::ExclusiveGeometry();
    MYON_ModelComponentRef::ExclusiveModelComponent();
    MYONX_Model::ComponentFromRuntimeSerialNumber()
  */
  const class MYON_Geometry* Geometry(
    const class MYON_Geometry* no_geometry_return_value
    ) const;

  /*
  Description:
    Get a pointer to geometry that can be used to modify the geometry.
    The returned pointer is not shared at the time it is returned
    and will not be shared until a copy of this MYON_ModelGeometryComponent
    is created.
  Returns:
    If this MYON_ModelGeometryComponent is the only reference to the geometry, 
    then a pointer to the geometry is returned.
    Otherwise, nullptr is returned.
  See Also:
    MYON_ModelGeometryComponent::Attributes()
    MYON_ModelGeometryComponent::Geometry()
    MYON_ModelGeometryComponent::ExclusiveAttributes()
    MYON_ModelGeometryComponent::ExclusiveGeometry();
    MYON_ModelComponentRef::ExclusiveModelComponent();
    MYONX_Model::ComponentFromRuntimeSerialNumber()
  */
  class MYON_Geometry* ExclusiveGeometry() const;

  /*
  Description:
    Get a pointer to attributes. The returned pointer may be shared
    and should not be used to modify the attributes.
  Parameters:
    no_attributes_return_value - [in]
      This value is returned if no attributes have been set.
      A good choices for this parameter's value depends on the context.
      Common options are nullptr, &MYON_3dmObjectAttributes::Unset,
      &MYON_3dmObjectAttributes::Default, or the model's current default attributes. 
  Returns:
    The layer, rendering and other attributes for this element, 
    or no_attributes_return_value if the attributes have not been set.
  See Also:
    MYON_ModelGeometryComponent::Attributes()
    MYON_ModelGeometryComponent::Geometry()
    MYON_ModelGeometryComponent::ExclusiveAttributes()
    MYON_ModelGeometryComponent::ExclusiveGeometry();
    MYON_ModelComponentRef::ExclusiveModelComponent();
    MYONX_Model::ComponentFromRuntimeSerialNumber()
  */
  const MYON_3dmObjectAttributes* Attributes(
    const MYON_3dmObjectAttributes* no_attributes_return_value
    ) const;

  /*
  Description:
    Get a pointer to attributes that can be used to modify the attributes.
    The returned pointer is not shared at the time it is returned
    and will not be shared until a copy of this MYON_ModelGeometryComponent
    is created.
  Returns:
    If this MYON_ModelGeometryComponent is the only reference to the attributes, 
    then a pointer to the attributes is returned.
    Otherwise, nullptr is returned.
  See Also:
    MYON_ModelGeometryComponent::Attributes()
    MYON_ModelGeometryComponent::Geometry()
    MYON_ModelGeometryComponent::ExclusiveAttributes()
    MYON_ModelGeometryComponent::ExclusiveGeometry();
    MYON_ModelComponentRef::ExclusiveModelComponent();
    MYONX_Model::ComponentFromRuntimeSerialNumber()
  */
  class MYON_3dmObjectAttributes* ExclusiveAttributes() const;

private:

#pragma MYON_PRAGMA_WARNING_PUSH
#pragma MYON_PRAGMA_WARNING_DISABLE_MSC( 4251 ) 
  // C4251: ... needs to have dll-interface to be used by clients of class ...
  // m_geometry_sp is private and all code that manages m_sp is explicitly implemented in the DLL.
  // m_attributes_sp is private and all code that manages m_sp is explicitly implemented in the DLL.
private:
  std::shared_ptr<MYON_Geometry> m_geometry_sp;
private:
  std::shared_ptr<MYON_3dmObjectAttributes> m_attributes_sp;
#pragma MYON_PRAGMA_WARNING_POP
};

#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_ModelGeometryComponent*>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<const MYON_ModelGeometryComponent*>;
#endif

#endif

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

#if !defined(OPENNURBS_LAYER_INC_)
#define OPENNURBS_LAYER_INC_

class MYON_CLASS MYON_Layer : public MYON_ModelComponent
{
  MYON_OBJECT_DECLARE(MYON_Layer);

public:

  MYON_Layer()  MYON_NOEXCEPT;
  ~MYON_Layer();
  MYON_Layer(const MYON_Layer&);
  MYON_Layer& operator=(const MYON_Layer&);

  static const MYON_Layer Unset;   // index = MYON_UNSET_INT_INDEX, id = nil
  static const MYON_Layer Default; // index = -1, id set, unique and persistent

  /*
  Parameters:
    model_component_reference - [in]
    none_return_value - [in]
      value to return if MYON_Layer::Cast(model_component_ref.ModelComponent())
      is nullptr
  Returns:
    If MYON_Layer::Cast(model_component_ref.ModelComponent()) is not nullptr,
    that pointer is returned.  Otherwise, none_return_value is returned. 
  */
  static const MYON_Layer* FromModelComponentRef(
    const class MYON_ModelComponentReference& model_component_reference,
    const MYON_Layer* none_return_value
    );

  bool UpdateReferencedComponents(
    const class MYON_ComponentManifest& source_manifest,
    const class MYON_ComponentManifest& destination_manifest,
    const class MYON_ManifestMap& manifest_map
    ) override;

  //////////////////////////////////////////////////////////////////////
  //
  // MYON_Object overrides
  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;

  void Dump( MYON_TextLog& ) const override; // for debugging

  bool Write(
         MYON_BinaryArchive&  // serialize definition to binary archive
       ) const override;

  bool Read(
         MYON_BinaryArchive&  // restore definition from binary archive
       ) override;

  MYON::object_type ObjectType() const override;

  //////////////////////////////////////////////////////////////////////
  //
  // Interface

  // The PER_VIEWPORT_SETTINGS enum defines
  // the bits used to set masks in functions used
  // to specify and query per viewport layer settings.
  enum PER_VIEWPORT_SETTINGS : unsigned int
  {
    per_viewport_none              =  0,

    per_viewport_id               =  1,
    per_viewport_color            =  2,
    per_viewport_plot_color       =  4,
    per_viewport_plot_weight      =  8,
    per_viewport_visible          = 16,
    per_viewport_persistent_visibility = 32,

    per_viewport_all_settings     = 0xFFFFFFFF
    // (Developers: these values are used in file IO and must not be changed.)
  };

 /*
  Parameters:
    viewport_id - [in]
      If viewport_id is not nil, then checks for per viewport
      settings for that specific viewport.
      If viewport_id is nil, then checks for per viewport settings
      in any viewport.
    settings_mask - [in]
      settings_mask is a bitfield that specifies which settings
      to check for.  The bits are defined in the
      MYON_Layer::PER_VIEWPORT_PROPERTIES enum.  If you want to 
      determine if the layer has any per viewport settings,
      then pass 0xFFFFFFFF.
  Returns:
    True if the layer has per viewport override for the specified
    settings.
  */
  bool HasPerViewportSettings(
    MYON_UUID viewport_id,
    unsigned int settings_mask
    ) const;

  /*
  Parameters:
    viewport_id - [in]
      If viewport_id is not nil, then checks for setting for
      that specific viewport.
      If viewport_id is nil, then checks for any viewport settings.
  Returns:
    True if the layer has per viewport settings.
  */
  bool HasPerViewportSettings(
    const MYON_UUID& viewport_id
    ) const;


  /*
  Description:
    Copies all per viewport settings for the source_viewport_id
  Parameters:
    source_viewport_id - [in]
      viewport id to copy all per viewport settings from
    destination_viewport_id - [in]
      viewport od to copy all per viewport settings to
  Returns:
    True if the settings could be copied, False if no per-viewport
    settings exist for the source viewport id
  */
  bool CopyPerViewportSettings( 
    MYON_UUID source_viewport_id,
    MYON_UUID destination_viewport_id
    );


  /*
  Description:
    Copies specified per viewport settings from a source layer to this
    layer.
  Parameters:
    source_layer - [in]
      layer to copy settings from
    viewport_id - [in]
      viewport id to copy all per viewport settings from.
      If viewport_id is nil, then the per viewport settings
      for all viewports will be copied.
    settings_mask - [in]
      bits indicate which settings to copy
      Use the MYON_Layer PER_VIEWPORT_SETTINGS enum to
      set the bits.
  Returns:
    True if the settings were copied, False if no per-viewport
    settings exist for the specified viewport_id.
  */
  bool CopyPerViewportSettings( 
    const MYON_Layer& source_layer,
    MYON_UUID viewport_id,
    unsigned int settings_mask
    );

  /*
  Description:
    Delete per viewport layer settings.
  Parameters:
    viewport_id - [in]
      If viewport_id is not nil, then the settings for that
      viewport are deleted.  If viewport_id is nil, then all
      per viewport settings are deleted.
  */
  void DeletePerViewportSettings( 
    const MYON_UUID& viewport_id 
    ) const;

  /*
  Description:
    Cull unused per viewport layer settings.
  Parameters:
    viewport_id_count - [in]
    viewport_id_list - [in]
      Settings for any viewports NOT in the viewport_id_list[]
      are culled.
  */
  void CullPerViewportSettings( 
    int viewport_id_count, 
    const MYON_UUID* viewport_id_list
    );

  /*
  Description:
    The PerViewportSettingsCRC() can be used to determine
    when layers have different per viewport settings.
  */
  MYON__UINT32 PerViewportSettingsCRC() const;

  /*
  Description:
    Set the color used by objects on this layer that do
    not have a per object color set
  Parameters:
    layer_color - [in]
      Passing MYON_UNSET_COLOR will clear the settings.
    viewport_id - [in]
      If viewport_id is not nil, then the setting applies only
      to the viewport with the specified id.
  */
	void SetColor( MYON_Color layer_color ); // layer display color

  /*
  Description:
    Set the color used by objects on this layer that do
    not have a per object color set
  Parameters:
    viewport_id - [in]
      If viewport_id is not nil, then the setting applies only
      to the viewport with the specified id.
    layer_color - [in]
      Passing MYON_UNSET_COLOR will clear the settings.
  */
  void SetPerViewportColor( MYON_UUID viewport_id, MYON_Color layer_color );

  // /* use MYON_Layer::SetPerViewportColor */
  //MYON_DEPRECATED void SetColor( MYON_Color, const MYON_UUID& );

  /*
  Parameters:
    viewport_id - [in]
      If viewport_id is not nil, then the setting to use
      for a specific viewport is returned.
  Returns:
    The color used by objects on this layer that do
    not have a per object color set.
  */
	MYON_Color Color() const;

  /*
  Parameters:
    viewport_id - [in]
      If viewport_id is not nil, then the setting to use
      for a specific viewport is returned.
  Returns:
    The color used by objects in the specified viewport and
    on this layer that do not have a per object color set.
  */
  MYON_Color PerViewportColor( MYON_UUID viewport_id ) const;

  /*
  Description:
    Remove any per viewport layer color setting so the
    layer's overall setting will be used for all viewports.
  Parameters:
    viewport_id - [in]
      If viewport_id is not nil, then the setting for this
      viewport will be deleted.  If viewport_id is nil,
      the all per viewport layer color settings will be removed.
  */
  void DeletePerViewportColor( const MYON_UUID& viewport_id );

  /*
  Description:
    Set the plotting color used by objects on this layer that do
    not have a per object plotting color set
  Parameters:
    plot_color - [in]
      Passing MYON_UNSET_COLOR will clear the settings.
    viewport_id - [in]
      If viewport_id is not nil, then the setting applies only
      to the viewport with the specified id.
  */
	void SetPlotColor( MYON_Color plot_color ); // plotting color

  void SetPerViewportPlotColor( MYON_UUID viewport_id, MYON_Color plot_color );

  /*
  Returns:
    The plotting color used by objects on this layer that do
    not have a per object color set.
  */
	MYON_Color PlotColor() const;

  /*
  Parameters:
    viewport_id - [in]
      If viewport_id is not nil, then the setting to use
      for a specific viewport is returned.
  Returns:
    The plotting color used by objects on this layer that do
    not have a per object color set.
  */
	MYON_Color PerViewportPlotColor( MYON_UUID viewport_id ) const;

  /*
  Description:
    Remove any per viewport plot color setting so the
    layer's overall setting will be used for all viewports.
  Parameters:
    viewport_id - [in]
      If viewport_id is not nil, then the setting for this
      viewport will be deleted.  If viewport_id is nil,
      the all per viewport plot color settings will be removed.
  */
  void DeletePerViewportPlotColor( const MYON_UUID& viewport_id );

  /*
  Description:
    Set the index of the linetype used by objects on this layer that do
    not have a per object lintypes
  Parameters:
    linetype_index - [in]
      Passing -1 will clear the setting.
  */
	bool SetLinetypeIndex( int linetype_index );

  /*
  Returns:
    The index of the linetype used by objects on this layer that do
    not have a per object linetype set.
  */
	int LinetypeIndex() const;

  /*
  Returns:
    Returns true if objects on layer are visible.
  Remarks:
    Does not inspect per viewport settings. It is better to use the
    overload that take an MYON_3dmView for visibility testing
  See Also:
    MYON_Layer::SetVisible
  */
  bool IsVisible() const;

  /*
  Returns:
    Returns true if objects on layer are visible for a given view.
  See Also:
    MYON_Layer::SetVisible
  */
  bool IsVisible(const class MYON_3dmView* view) const;

  /*
  Description:
    Controls layer visibility
  Parameters:
    bVisible - [in] true to make layer visible, 
                    false to make layer invisible
    viewport_id - [in]
      If viewport_id is not nil, then the setting applies only
      to the viewport with the specified id.
  See Also:
    MYON_Layer::IsVisible
  */
  void SetVisible( bool bVisible );

  /*
  Description:
    The persistent visibility setting is used for layers whose
    visibility can be changed by a "parent" object. A common case
    is when a layer is a child layer (MYON_Layer.m_parent_id is
    not nil). In this case, when a parent layer is turned off,
    then child layers are also turned off. The persistent
    visibility setting determines what happens when the parent
    is turned on again.
  Returns:
    true: 
      If this layer's visibility is controlled by a parent object
      and the parent is turned on (after being off), then this
      layer will also be turned on.
    false:
      If this layer's visibility is controlled by a parent object
      and the parent layer is turned on (after being off), then
      this layer will continue to be off.
  Remarks:
    When the persistent visibility is not explicitly set, this
    function returns the current value of IsVisible().
  See Also:
    MYON_Layer::SetPersistentVisibility
    MYON_Layer::UnsetPersistentVisibility
  */
  bool PersistentVisibility() const;

  /*
  Description:
    Set the persistent visibility setting for this layer.
  Parameters:
    bPersistentVisibility - [in]
      persistent visibility setting for this layer.
  Remarks:
    See MYON_Layer::PersistentVisibility for a detailed description
    of persistent visibility.
  See Also:
    MYON_Layer::PersistentVisibility
    MYON_Layer::UnsetPersistentVisibility
  */
  void SetPersistentVisibility( bool bPersistentVisibility );

  /*
  Description:
    Remove any explicit persistent visibility setting from this
    layer. When persistent visibility is not explicitly set,
    the value of MYON_Layer::IsVisible() is used.
  Remarks:
    See MYON_Layer::PersistentVisibility for a detailed description
    of persistent visibility.
  See Also:
    MYON_Layer::PersistentVisibility
    MYON_Layer::SetPersistentVisibility
  */
  void UnsetPersistentVisibility();
  

  /*
  Returns:
    Returns true if objects on layer are visible in model viewports.
  See Also:
    MYON_Layer::IsVisible()
    MYON_Layer::PerViewportIsVisible()
*/
  bool ModelIsVisible() const;

  /*
  Description:
    Controls layer visibility in  in model viewports.
  Parameters:
    bVisible - [in] true to make layer visible,
                    false to make layer invisible
  See Also:
    MYON_Layer::SetVisible()
    MYON_Layer::SetPerViewportVisible()
  */
  void SetModelVisible(bool bVisible);

  /*
  Returns:
    true:
      If this layer's visibility in model viewports is
      controlled by a parent object and the parent is turned on
      (after being off), then this layer will also be turned on
      in the specified viewport.
    false:
      If this layer's visibility in model viewports is
      controlled by a parent object and the parent layer is
      turned on (after being off), then this layer will continue
      to be off in the specified viewport.
  Remarks:
    See MYON_Layer::SetPersistentVisibility
    for a description of persistent visibility.
  See Also:
    MYON_Layer::SetModelPersistentVisibility
  */
  bool ModelPersistentVisibility() const;

  /*
  Description:
    This function allows model viewports setting the
    child visibility property.
  Parameters
    bPersistentVisibility - [in]
  Remarks:
    See MYON_Layer::SetPersistentVisibility
    for a description of the child visibility property.
  See Also:
    MYON_Layer::SetPersistentVisibility
  */
  void SetModelPersistentVisibility(bool bPersistentVisibility);

  void UnsetModelPersistentVisibility();

  /*
  Description:
    Remove any model viewport visibility setting so the
    layer's overall setting will be used for all viewports.
  */
  void DeleteModelVisible();

  /*
  Parameters:
    viewport_id - [in]
      If viewport_id is not nil, then the visibility setting
      for that viewport is returned.  

      If viewport_id is nil, the MYON_Layer::IsVisible() is returned.
  Returns:
    Returns true if objects on layer are visible.
  */
	bool PerViewportIsVisible( MYON_UUID viewport_id ) const;

  /*
  Description:
    Controls layer visibility in specific viewports.
  Parameters:
    viewport_id - [in]
      If viewport_id is not nil, then the setting applies only
      to the viewport with the specified id.  If viewport_id
      is nil, then the setting applies to all viewports with
      per viewport layer settings.
    bVisible - [in] true to make layer visible, 
                    false to make layer invisible
  See Also:
    MYON_Layer::PerViewportIsVisible()
  */
  void SetPerViewportVisible( 
    MYON_UUID viewport_id, 
    bool bVisible 
  );

  // /* use MYON_Layer::SetPerViewportVisible */
  // MYON_DEPRECATED void SetVisible( bool, const MYON_UUID& );

  /*
  Parameters:
    viewport_id - [in]
      id of a viewport.  If viewport_id is nil, then 
      MYON_Layer::PersistentVisibility() is returned.
  Returns:
    true: 
      If this layer's visibility in the specified viewport is 
      controlled by a parent object and the parent is turned on
      (after being off), then this layer will also be turned on
      in the specified viewport.
    false:
      If this layer's visibility in the specified viewport is
      controlled by a parent object and the parent layer is 
      turned on (after being off), then this layer will continue
      to be off in the specified viewport.
  Remarks:
    See MYON_Layer::SetPersistentVisibility
    for a description of persistent visibility.
  See Also:
    MYON_Layer::SetPerViewportPersistentVisibility
  */
  bool PerViewportPersistentVisibility( MYON_UUID viewport_id ) const;

  /*
  Description:
    This function allows per viewport setting the
    child visibility property.
  Parameters
    viewport_id - [in]
    bPersistentVisibility - [in]
  Remarks:
    See MYON_Layer::SetPersistentVisibility
    for a description of the child visibility property.
  See Also:
    MYON_Layer::SetPersistentVisibility
  */
  void SetPerViewportPersistentVisibility( MYON_UUID viewport_id, bool bPersistentVisibility );

  void UnsetPerViewportPersistentVisibility( MYON_UUID viewport_id );    

  /*
  Description:
    Remove any per viewport visibility setting so the
    layer's overall setting will be used for all viewports.
  Parameters:
    viewport_id - [in]
      If viewport_id is not nil, then the setting for this
      viewport will be deleted.  If viewport_id is nil,
      the all per viewport visibility settings will be removed.
  */
  void DeletePerViewportVisible( const MYON_UUID& viewport_id );

  /*
  Description:
    Get a list of the viewport ids of viewports that 
    that have per viewport visibility settings that
    override the default layer visibility setting 
    MYON_Layer::m_bVisible.
  Parameters:
    viewport_id_list - [out]
      List of viewport id's that have a per viewport visibility
      setting.  If the returned list is empty, then there
      are no per viewport visibility settings.
  Returns:
    Number of ids added to the list.
  */
  void GetPerViewportVisibilityViewportIds(
    MYON_SimpleArray<MYON_UUID>& viewport_id_list
    ) const;

  /*
  Description:
    Controls layer locked
  Parameters:
    bLocked - [in] True to lock layer
                   False to unlock layer
  See Also:
    MYON_Layer::IsLocked
  */
  void SetLocked( bool bLocked );

  /*
  Description:
    The persistent locking setting is used for layers that can
    be locked by a "parent" object. A common case is when a layer
    is a child layer (MYON_Layer.m_parent_id is not nil). In this 
    case, when a parent layer is locked, then child layers are 
    also locked. The persistent locking setting determines what
    happens when the parent is unlocked again.
  Returns:
    true: 
      If this layer's locking is controlled by a parent object
      and the parent is unlocked (after being locked), then this
      layer will also be unlocked.
    false:
      If this layer's locking is controlled by a parent object
      and the parent layer is unlocked (after being locked), then
      this layer will continue to be locked.
  Remarks:
    When the persistent locking is not explicitly set, this
    function returns the current value of IsLocked().
  See Also:
    MYON_Layer::SetPersistentLocking
    MYON_Layer::UnsetPersistentLocking
  */
  bool PersistentLocking() const;

  /*
  Description:
    Set the persistent locking setting for this layer.
  Parameters:
    bPersistentLocking - [in]
      persistent locking for this layer.
  Remarks:
    See MYON_Layer::PersistentLocking for a detailed description of
    persistent locking.
  See Also:
    MYON_Layer::PersistentLocking
    MYON_Layer::UnsetPersistentLocking
  */
  void SetPersistentLocking(bool bPersistentLocking);

  /*
  Description:
    Remove any explicitly persistent locking settings from this
    layer.
  Remarks:
    See MYON_Layer::PersistentLocking for a detailed description of
    persistent locking.
  See Also:
    MYON_Layer::PersistentLocking
    MYON_Layer::SetPersistentLocking
  */
  void UnsetPersistentLocking();

  /*
  Returns:
    Value of (IsVisible() && !IsLocked()).
  */
  bool IsVisibleAndNotLocked() const;

  /*
  Returns:
    Value of (IsVisible() && IsLocked()).
  */
  bool IsVisibleAndLocked() const;

  //////////
  // Index of render material for objects on this layer that have
  // MaterialSource() == MYON::material_from_layer.
  // A material index of -1 indicates no material has been assigned
  // and the material created by the default MYON_Material constructor
  // should be used.
  bool SetRenderMaterialIndex( int ); // index of layer's rendering material
  int RenderMaterialIndex() const;

  bool SetIgesLevel( int ); // IGES level for this layer
  int IgesLevel() const;

  /*
  Description:
    Get the weight (thickness) of the plotting pen.
  Returns:
    Thickness of the plotting pen in millimeters.
    A thickness of  0.0 indicates the "default" pen weight should be used.
    A thickness of -1.0 indicates the layer should not be printed.
  */
  double PlotWeight() const;

  double PerViewportPlotWeight( MYON_UUID viewport_id ) const;

  // /* use MYON_Layer::PerViewportPlotWeight */
  // MYON_DEPRECATED double PlotWeight( const MYON_UUID& ) const;

  /*
  Description:
    Set the weight of the plotting pen.
  Parameters:
    plot_weight_mm - [in] Set the thickness of the plotting pen in millimeters.
       0.0 means use the default pen width which is a Rhino app setting.
      -1.0 means layer does not print (still displays on the screen)
  */
  void SetPlotWeight(double plot_weight_mm);

  /*
  Description:
    Set the weight of the plotting pen.
  Parameters:
    plot_weight_mm - [in] Set the thickness of the plotting pen in millimeters.
       0.0 means use the default pen width which is a Rhino app setting.
      -1.0 means layer does not print (still displays on the screen)
  */
  void SetPerViewportPlotWeight(MYON_UUID viewport_id, double plot_weight_mm);

  /*
  Description:
    Remove any per viewport plot weight setting so the
    layer's overall setting will be used for all viewports.
  Parameters:
    viewport_id - [in]
      If viewport_id is not nil, then the setting for this
      viewport will be deleted.  If viewport_id is nil,
      the all per viewport plot weight settings will be removed.
  */
  void DeletePerViewportPlotWeight( const MYON_UUID& viewport_id );

  /*
  Description:
    Use UpdateViewportIds() to change viewport ids in situations
    like merging when a viewport id conflict requires the viewport
    ids in a file to be changed.
  Returns:
    Number of viewport ids that were updated.
  */
  int UpdateViewportIds( const MYON_UuidPairList& viewport_id_map );

public:

  // Layers are origanized in a hierarchical
  // structure (like file folders).
  // If a layer is in a parent layer,
  // then m_parent_layer_id is the id of
  // the parent layer.
  MYON_UUID ParentLayerId() const;

  void SetParentLayerId(
    MYON_UUID parent_layer_id
    );

  int m_iges_level = -1;        // IGES level number if this layer was made during IGES import
  
  // Rendering material:
  //   If you want something simple and fast, set 
  //   m_material_index to the index of your rendering material 
  //   and ignore m_rendering_attributes.
  //   If you are developing a fancy plug-in renderer, and a user is
  //   assigning one of your fabulous rendering materials to this
  //   layer, then add rendering material information to the 
  //   m_rendering_attributes.m_materials[] array. 
  //
  // Developers:
  //   As soon as m_rendering_attributes.m_materials[] is not empty,
  //   rendering material queries slow down.  Do not populate
  //   m_rendering_attributes.m_materials[] when setting
  //   m_material_index will take care of your needs.
  int m_material_index = -1;
  MYON_RenderingAttributes m_rendering_attributes;

  int m_linetype_index = -1;    // index of linetype

  // Layer display attributes.
  //   If m_display_material_id is nil, then m_color is the layer color
  //   and defaults are used for all other display attributes.
  //   If m_display_material_id is not nil, then some complicated
  //   scheme is used to decide what objects on this layer look like.
  //   In all cases, m_color is a good choice if you don't want to
  //   deal with m_display_material_id.  In Rhino, m_display_material_id
  //   is used to identify a registry entry that contains user specific
  //   display preferences.
  MYON_Color m_color = MYON_Color::Black;
  MYON_UUID m_display_material_id = MYON_nil_uuid;

  // Layer printing (plotting) attributes.
  MYON_Color m_plot_color = MYON_Color::UnsetColor;   // printing color
                           // MYON_UNSET_COLOR means use layer color
  double m_plot_weight_mm = 0.0; // printing pen thickness in mm
                           //  0.0 means use the default width (a Rhino app setting)
                           // -1.0 means layer does not print (still visible on screen)

  bool m_bExpanded = true; // If true, when the layer table is displayed in
                    // a tree control then the list of child layers is
                    // shown in the control.

  // Set objects on layer to participate in clipping for all clipping plane objects.
  // This is the default behavior.
  void SetClipParticipationForAll();

  // Set objects on layer to be immune from the clipping for all clipping planes.
  void SetClipParticipationForNone();

  // Set objects on layer to only be clipped by a specific set of clipping planes.
  void SetClipParticipationList(const MYON_UUID* clippingPlaneIds, int count);

  // Get details on how the objects on layer will interact with clipping planes
  // Parameters:
  //   forAllClippingPlanes [out] - if true, the objects interact with all
  //     clipping planes
  //   forNoClippingPlanes [out] - if true, the objects are immune from all
  //     clipping planes
  //   specificClipplaneList [out] - if the objects interact with only a
  //     specific set of clipping planes, this list will have the uuids of
  //     those clipping plane objects
  void GetClipParticipation(
    bool& forAllClippingPlanes,
    bool& forNoClippingPlanes,
    MYON_UuidList& specificClipplaneList) const;

#pragma region Section Attributes
  // Sections are the product of intersecting a plane with an object.
  // For surface type geometry (MYON_Brep, MYON_Extrusion, MYON_SubD, MYON_Mesh)
  // this intersection can result in curves as well as hatches for the
  // closed curves generated

  // When to fill/hatch the sections for an object can depend on the type of
  // object being sectioned. See MYON_SectionFillDecision for the choices of
  // when to generate hatches.
  MYON::SectionFillRule SectionFillRule() const;
  void SetSectionFillRule(MYON::SectionFillRule rule);

  // Hatch pattern index for hatch to use when drawing a closed section
  // Default is MYON_UNSET_INT_INDEX which means don't draw a hatch
  int SectionHatchIndex() const;
  void SetSectionHatchIndex(int index);

  // Scale applied to the hatch pattern for a section
  double SectionHatchScale() const;
  void SetSectionHatchScale(double scale);

  // Rotation angle in radians applied to hatch pattern for a section.
  double SectionHatchRotation() const;
  void SetSectionHatchRotation(double rotation);
#pragma endregion

private:
  // The following information may not be accurate and is subject
  // to change at any time.
  //
  // m_extension_bits & 0x01: 
  //   The value of ( m_extension_bits & 0x01) is used to speed
  //   common per viewport visibility and color queries.
  //     0x00 = there may be per viewport settings on this layer.
  //     0x01 = there are no per viewport settings on this layer.
  //
  // m_extension_bits & 0x06:
  //   The value of ( m_extension_bits & 0x06) is the persistent
  //   visibility setting for this layer.
  //     0x00 = no persistent visibility setting
  //     0x02 = persistent visibility = true
  //     0x04 = persistent visibility = false
  //     0x06 = invalid value - treated as 0x00
  //
  // m_extension_bits & 0x18:
  //   The value of ( m_extension_bits & 0x18) is the persistent
  //   locking setting for this layer.
  //     0x00 = no persistent locking setting
  //     0x08 = persistent locking = true
  //     0x10 = persistent locking = false
  //     0x18 = invalid value - treated as 0x00
  MYON__UINT8 m_extension_bits = 0;
  MYON__UINT16 m_reserved = 0;

private:
  mutable class MYON_LayerPrivate* m_private = nullptr;
};

#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_Layer*>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_ObjectArray<MYON_Layer>;
#endif

#endif


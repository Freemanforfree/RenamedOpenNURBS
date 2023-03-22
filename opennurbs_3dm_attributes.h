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

#if !defined(OPENNURBS_3DM_ATTRIBUTES_INC_)
#define OPENNURBS_3DM_ATTRIBUTES_INC_

/*
Description: 
  Top level OpenNURBS objects have geometry and attributes.  The
  geometry is stored in some class derived from MYON_Geometry and 
  the attributes are stored in an MYON_3dmObjectAttributes class.
  Examples of attributes are object name, object id, display 
  attributes, group membership, layer membership, and so on.
*/

class MYON_CLASS MYON_3dmObjectAttributes : public MYON_Object
{
  MYON_OBJECT_DECLARE(MYON_3dmObjectAttributes);

public:
  static const MYON_3dmObjectAttributes Unset;
  static const MYON_3dmObjectAttributes DefaultAttributes;

public:
  // MYON_Object virtual interface.  See MYON_Object for details.
  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;
  void Dump( MYON_TextLog& ) const override;
  unsigned int SizeOf() const override;
  bool Write(MYON_BinaryArchive&) const override;
  bool Read(MYON_BinaryArchive&) override;

  /*
  Returns:
    True if successful.
    (xform is invertable or didn't need to be).
  */
  MYON_DEPRECATED_MSG("Prefer the version that takes a const MYON_Geometry* - for object frame support.") bool Transform( const MYON_Xform& xform );
  bool Transform(const MYON_Geometry* pOriginalGeometry, const MYON_Xform& xform);

  // attributes of geometry and dimension table objects
public:
  MYON_3dmObjectAttributes();
  ~MYON_3dmObjectAttributes();
  MYON_3dmObjectAttributes(const MYON_3dmObjectAttributes&);
  MYON_3dmObjectAttributes& operator=(const MYON_3dmObjectAttributes&);

  bool operator==(const MYON_3dmObjectAttributes&) const;
  bool operator!=(const MYON_3dmObjectAttributes&) const;

  // Initializes all attributes to the default values.
  void Default();


  bool UpdateReferencedComponents(
    const class MYON_ComponentManifest& source_manifest,
    const class MYON_ComponentManifest& destination_manifest,
    const class MYON_ManifestMap& manifest_map
    ) override;

  // Interface ////////////////////////////////////////////////////////

  // An OpenNURBS object must be in one of three modes: normal, locked
  // or hidden.  If an object is in normal mode, then the object's layer
  // controls visibility and selectability.  If an object is locked, then
  // the object's layer controls visibility by the object cannot be selected.
  // If the object is hidden, it is not visible and it cannot be selected.
  MYON::object_mode Mode() const;
  void SetMode( MYON::object_mode ); // See Mode().

  /*
  Description:
    Use this query to determine if an object is part of an 
    instance definition.
  Returns:
    True if the object is part of an instance definition.
  */
  bool IsInstanceDefinitionObject() const;

  /*
  Returns:
    Returns true if object is visible.
  See Also:
    MYON_3dmObjectAttributes::SetVisible
  */
  bool IsVisible() const;

  /*
  Description:
    Controls object visibility
  Parameters:
    bVisible - [in] true to make object visible, 
                    false to make object invisible
  See Also:
    MYON_3dmObjectAttributes::IsVisible
  */
  void SetVisible( bool bVisible );

  // The Linetype used to display an OpenNURBS object is specified in one of two ways.
  // If LinetypeSource() is MYON::linetype_from_layer, then the object's layer 
  // MYON_Layer::Linetype() is used.
  // If LinetypeSource() is MYON::linetype_from_object, then value of m_linetype is used.
  MYON::object_linetype_source LinetypeSource() const;
  void SetLinetypeSource( MYON::object_linetype_source ); // See LinetypeSource().

  // The color used to display an OpenNURBS object is specified in one of three ways.
  // If ColorSource() is MYON::color_from_layer, then the object's layer 
  // MYON_Layer::Color() is used.
  // If ColorSource() is MYON::color_from_object, then value of m_color is used.
  // If ColorSource() is MYON::color_from_material, then the diffuse color of the object's
  // render material is used.  See MYON_3dmObjectAttributes::MaterialSource() to
  // determine where to get the definition of the object's render material.
  MYON::object_color_source ColorSource() const;
  void SetColorSource( MYON::object_color_source ); // See ColorSource().

  // The color used to plot an OpenNURBS object on paper is specified 
  // in one of three ways.
  // If PlotColorSource() is MYON::plot_color_from_layer, then the object's layer 
  // MYON_Layer::PlotColor() is used.
  // If PlotColorSource() is MYON::plot_color_from_object, then value of PlotColor() is used.
  MYON::plot_color_source PlotColorSource() const;
  void SetPlotColorSource( MYON::plot_color_source ); // See PlotColorSource().

  MYON::plot_weight_source PlotWeightSource() const;
  void SetPlotWeightSource( MYON::plot_weight_source );

  /*
  Description:
    If "this" has attributes (color, plot weight, ...) with 
    "by parent" sources, then the values of those attributes 
    on parent_attributes are copied.
  Parameters:
    parent_attributes - [in]
    parent_layer - [in]
    control_limits - [in]
      The bits in control_limits determine which attributes may
      may be copied.
                1: visibility
                2: color
                4: render material
                8: plot color
             0x10: plot weight
             0x20: linetype
             0x40: display order
             0x80: clip participation
  Returns:
     The bits in the returned integer indicate which attributes were
     actually modified.

                1: visibility
                2: color
                4: render material
                8: plot color
             0x10: plot weight
             0x20: linetype
             0x40: display order
             0x80: clip participation
            0x100: section style
  */
  unsigned int ApplyParentalControl(
         const MYON_3dmObjectAttributes& parent_attributes,
         const MYON_Layer& parent_layer,
         unsigned int control_limits = 0xFFFFFFFF
         );

  unsigned int ApplyParentalControl(
    const MYON_3dmObjectAttributes& parent_attributes,
    const MYON_Layer& parent_layer,
    const MYON_UUID& viewport_id,
    unsigned int control_limits = 0xFFFFFFFF
  );

  // Every OpenNURBS object has a UUID (universally unique identifier). When
  // an OpenNURBS object is added to a model, the value is checked.  If the
  // value is MYON_nil_uuid, a new UUID is created.  If the value is not
  // MYON_nil_uuid but it is already used by another object in the model, a new
  // UUID is created.  If the value is not MYON_nil_uuid and it is not used by 
  // another object in the model, then that value persists. When an object
  // is updated, by a move for example, the value of m_uuid persists.
  MYON_UUID m_uuid = MYON_nil_uuid;

  // The m_name member is public to avoid breaking the SDK.
  // Use SetName() and Name() for proper validation.
  // OpenNURBS object have optional text names.  More than one object in
  // a model can have the same name and some objects may have no name.
  // MYON_ModelComponent::IsValidComponentName(m_name) should be true.
  MYON_wString m_name;

  bool SetName(
    const wchar_t* name,
    bool bFixInvalidName
  );

  const MYON_wString Name() const;

  // OpenNURBS objects may have an URL.  There are no restrictions on what
  // value this URL may have.  As an example, if the object came from a
  // commercial part library, the URL might point to the definition of that
  // part.
  MYON_wString m_url;

  // Layer definitions in an OpenNURBS model are stored in a layer table.
  // The layer table is conceptually an array of MYON_Layer classes.  Every
  // OpenNURBS object in a model is on some layer.  The object's layer
  // is specified by zero based indices into the MYON_Layer array.
  int m_layer_index = 0;

  // Linetype definitions in an OpenNURBS model are stored in a linetype table.
  // The linetype table is conceptually an array of MYON_Linetype classes.  Every
  // OpenNURBS object in a model references some linetype.  The object's linetype
  // is specified by zero based indices into the MYON_Linetype array.
  // index 0 is reserved for continuous linetype (no pattern)
  int m_linetype_index = -1;

  // Rendering material:
  //   If you want something simple and fast, set 
  //   m_material_index to the index of the rendering material 
  //   and ignore m_rendering_attributes.
  //   If you are developing a high quality plug-in renderer, 
  //   and a user is assigning one of your fabulous rendering 
  //   materials to this object, then add rendering material 
  //   information to the  m_rendering_attributes.m_materials[] 
  //   array. 
  //
  // Developers:
  //   As soon as m_rendering_attributes.m_materials[] is not empty,
  //   rendering material queries slow down.  Do not populate
  //   m_rendering_attributes.m_materials[] when setting 
  //   m_material_index will take care of your needs.
  int m_material_index = -1;
  MYON_ObjectRenderingAttributes m_rendering_attributes;

  //////////////////////////////////////////////////////////////////
  //
  // BEGIN: Per object mesh parameter support
  //

  /*
  Parameters:
    mp - [in]
      per object mesh parameters
  Returns:
    True if successful.
  */
  bool SetCustomRenderMeshParameters(const class MYON_MeshParameters& mp);

  /*
  Parameters:
    bEnable - [in]
      true to enable use of the per object mesh parameters.
      false to disable use of the per object mesh parameters.
  Returns:
    False if the object doe not have per object mesh parameters
    and bEnable was true.  Use SetMeshParameters() to set
    per object mesh parameters.
  Remarks:
    Sets the value of MYON_MeshParameters::m_bCustomSettingsDisabled 
    to !bEnable
  */
  bool EnableCustomRenderMeshParameters(bool bEnable);

  /*
  Returns:
    Null or a pointer to fragile mesh parameters.
    If a non-null pointer is returned, copy it and use the copy.
    * DO NOT SAVE THIS POINTER FOR LATER USE. A call to 
      DeleteMeshParameters() will delete the class.
    * DO NOT const_cast the returned pointer and change its
      settings.  You must use either SetMeshParameters()
      or EnableMeshParameters() to change settings.
  Remarks:
    If the value of MYON_MeshParameters::m_bCustomSettingsDisabled is
    true, then do no use these parameters to make a render mesh.
  */
  const MYON_MeshParameters* CustomRenderMeshParameters() const;

  /*
  Description:
    Deletes any per object mesh parameters.
  */
  void DeleteCustomRenderMeshParameters();

  //
  // END: Per object mesh parameter support
  //
  //////////////////////////////////////////////////////////////////


  /*
  Description:
    Determine if the simple material should come from
    the object or from it's layer.
    High quality rendering plug-ins should use m_rendering_attributes.
  Returns:
    Where to get material information if you do are too lazy
    to look in m_rendering_attributes.m_materials[].
  */
  MYON::object_material_source MaterialSource() const;

  /*
  Description:
    Specifies if the simple material should be the one
    indicated by the material index or the one indicated
    by the object's layer.
  Parameters:
    ms - [in]
  */
  void SetMaterialSource( MYON::object_material_source ms );

  // If MYON::color_from_object == ColorSource(), then m_color is the object's
  // display color.
  MYON_Color      m_color;

  // If MYON::plot_color_from_object == PlotColorSource(), then m_color is the object's
  // display color.
  MYON_Color      m_plot_color;

  // Display order used to force objects to be drawn on top or behind each other
  // 0  = draw object in standard depth buffered order
  // <0 = draw object behind "normal" draw order objects
  // >0 = draw object on top of "normal" draw order objects
  // Larger number draws on top of smaller number.
  int m_display_order = 0;

  // Plot weight in millimeters.
  //   =0.0 means use the default width
  //   <0.0 means don't plot (visible for screen display, but does not show on plot)
  double m_plot_weight_mm = 0;

  // Used to indicate an object has a decoration (like an arrowhead on a curve)
  MYON::object_decoration  m_object_decoration = MYON::no_object_decoration;

  // When a surface object is displayed in wireframe, m_wire_density controls
  // how many isoparametric wires are used.
  //
  //   @table
  //   value    number of isoparametric wires
  //   -1       boundary wires
  //    0       boundary and knot wires 
  //    1       boundary and knot wires and, if there are no
  //            interior knots, a single interior wire.
  //   N>=2     boundary and knot wires and (N-1) interior wires
  int m_wire_density = 1;


  // If m_viewport_id is MYON_nil_uuid, the object is active in all viewports.
  // If m_viewport_id is not MYON_nil_uuid, then this object is only active in a
  // specific view. This field is primarily used to assign page space objects
  // to a specific page, but it can also be used to restrict model space to a
  // specific view.
  MYON_UUID m_viewport_id = MYON_nil_uuid;

  // Starting with V4, objects can be in either model space
  // or page space.  If an object is in page space, then
  // m_viewport_id is not nil and identifies the page it 
  // is on.
  MYON::active_space m_space = MYON::model_space;

  // Set the object to participate in clipping for all clipping plane objects.
  // This is the default behavior.
  void SetClipParticipationForAll();

  // Set the object to be immune from the clipping for all clipping planes.
  void SetClipParticipationForNone();

  // Set the object to only be clipped by a specific set of clipping planes.
  void SetClipParticipationList(const MYON_UUID* clippingPlaneIds, int count);

  // Get details on how the object will interact with clipping planes
  // Parameters:
  //   forAllClippingPlanes [out] - if true, this object interacts with all
  //     clipping planes
  //   forNoClippingPlanes [out] - if true, this object is immune from all
  //     clipping planes
  //   specificClipplaneList [out] - if the object interacts with only a
  //     specific set of clipping planes, this list will have the uuids of
  //     those clipping plane objects
  void GetClipParticipation(
    bool& forAllClippingPlanes,
    bool& forNoClippingPlanes,
    MYON_UuidList& specificClipplaneList) const;

  // Source for clip participation details
  MYON::ClipParticipationSource ClipParticipationSource() const;
  void SetClipParticipationSource(MYON::ClipParticipationSource source);

#pragma region Section Attributes
  // Sections are the product of intersecting a plane with an object.
  // For surface type geometry (MYON_Brep, MYON_Extrusion, MYON_SubD, MYON_Mesh)
  // this intersection can result in curves as well as hatches for the
  // closed curves generated

  // When to fill/hatch the sections for an object can depend on the type of
  // object being sectioned. See MYON_SectionFillRule for the choices of
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

  // Source for all section related attributes
  MYON::SectionAttributesSource SectionAttributesSource() const;
  void SetSectionAttributesSource(MYON::SectionAttributesSource source);
#pragma endregion

  // Per object linetype scale
  double LinetypePatternScale() const;
  void SetLinetypePatternScale(double scale);

  /*
  Description:
    Attributes can have optional custom linetypes associated with them. When a
    custom linetype is attached to an attribute, this linetype is used for an
    attribute instead of the linetype referenced by the linetype index. This
    function adds a custom linetype for this attribute.
  */
  void SetCustomLinetype(const MYON_Linetype& linetype);

  /*
  Description:
    Attributes can have optional custom linetypes associated with them. This
    function returns the custom linetype if one exists. If a custom linetype is
    not attached to this attribute, then an empty shared pointer is returned
  */
  const MYON_Linetype* CustomLinetype() const;

  /*
  Description:
    Remove any custom linetype associated with this attribute
  */
  void RemoveCustomLinetype();

#pragma region Hatch Specific Attributes
  MYON_Color HatchBackgroundFillColor() const;
  void SetHatchBackgrounFillColor(const MYON_Color& color);
  bool HatchBoundaryVisible() const;
  void SetHatchBoundaryVisible(bool on);
#pragma endregion


  MYON_Plane ObjectFrame(const MYON_COMPONENT_INDEX& ci) const;
  void SetObjectFrame(const MYON_COMPONENT_INDEX& ci, const MYON_Xform& wcs_to_ocs);
  void SetObjectFrame(const MYON_COMPONENT_INDEX& ci, const MYON_Plane& plane);

private:
  bool m_bVisible = true;
  // (m_mode % 16) = MYON::object_mode values
  // (m_mode / 16) = MYON::display_mode values
  unsigned char m_mode = MYON::normal_object;
  unsigned char m_color_source = MYON::color_from_layer;
  unsigned char m_plot_color_source = MYON::plot_color_from_layer;
  unsigned char m_plot_weight_source = MYON::plot_weight_from_layer;
  unsigned char m_material_source = MYON::material_from_layer;
  unsigned char m_linetype_source = MYON::linetype_from_layer;
  
  unsigned char m_reserved_0 = 0;

  MYON_Plane m_object_frame = MYON_Plane::UnsetPlane;
  
  MYON_SimpleArray<int> m_group; // array of zero based group indices

private:
  void CopyHelper(const MYON_3dmObjectAttributes& src);
  mutable class MYON_3dmObjectAttributesPrivate* m_private = nullptr;

public:
  // group interface

  // returns number of groups object belongs to
  int GroupCount() const;

  // Returns and array an array of GroupCount() zero based 
  // group indices.  If GroupCount() is zero, then GroupList()
  // returns nullptr.
  const int* GroupList() const;

  // Returns GroupCount() and puts a list of zero based group indices 
  // into the array.
  int GetGroupList(MYON_SimpleArray<int>&) const;

  // Returns the index of the last group in the group list
  // or -1 if the object is not in any groups
  int TopGroup() const;

  // Returns true if object is in group with the specified index
  bool IsInGroup(
    int // zero based group index
    ) const;

  // Returns true if the object is in any of the groups in the list
  bool IsInGroups(
    int,       // group_list_count
    const int* // group_list[] array
    ) const;

  // Returns true if object is in any of the groups in the list
  bool IsInGroups(
    const MYON_SimpleArray<int>& // group_list[] array
    ) const;

  // Adds object to the group with specified index by appending index to
  // group list (If the object is already in group, nothing is changed.)
  void AddToGroup(
    int // zero based group index
    );

  // Removes object from the group with specified index.  If the 
  // object is not in the group, nothing is changed.
  void RemoveFromGroup(
    int // zero based group index
    );

  // Removes the object from the last group in the group list.
  void RemoveFromTopGroup();

  // Removes object from all groups.
  void RemoveFromAllGroups();

  // Decals.

  /*
  Description:
    Get an array of decals that are stored on this attributes object.
    Do not store or delete pointers from the array.
  */
  const MYON_SimpleArray<MYON_Decal*>& GetDecalArray(void) const;

  /*
  Description:
    Add a new decal to this attributes object. The returned pointer points to an object
    that is owned by the attributes. Do not store or delete it.
  */
  MYON_Decal* AddDecal(void);

  /*
  Description:
    Delete a decal from this attributes object. Returns true if successful, else false.
  */
  bool DeleteDecal(MYON_Decal& decal);

  /*
  Description:
    Delete all decals from this attributes object.
  */
  void DeleteAllDecals(void);

  // Mesh Modifiers.

  /*
  Description:
    Get the mesh modifiers that are stored on this attributes object.
  */
  class MYON_MeshModifiers& MeshModifiers(void) const;

  // Display material references.

  /*
  Description:
    Searches for a matching display material.  For a given
    viewport id, there is at most one display material.
    For a given display material id, there can be multiple
    viewports.  If there is a display reference in the
    list with a nil viewport id, then the display material
    will be used in all viewports that are not explicitly
    referenced in other MYON_DisplayMaterialRefs.

  Parameters:
    search_material - [in] 
    found_material - [out]
    
    If FindDisplayMaterialRef(), the input value of search_material
    is never changed.  If FindDisplayMaterialRef() returns true, 
    the chart shows the output value of display_material.  When
    there are multiple possibilities for a match, the matches
    at the top of the chart have higher priority.

    search_material  found_material
    input value      output value

    (nil,nil)        (nil,did) if (nil,did) is in the list.
    (nil,did)        (vid,did) if (vid,did) is in the list.
    (nil,did)        (nil,did) if (nil,did) is in the list.
    (vid,nil)        (vid,did) if (vid,did) is in the list
    (vid,nil)        (vid,did) if (nil,did) is in the list
    (vid,did)        (vid,did) if (vid,did) is in the list.

  Example:
    MYON_UUID display_material_id = MYON_nil_uuid;
    MYON_Viewport vp = ...;
    MYON_DisplayMaterialRef search_dm;
    search_dm.m_viewport_id = vp.ViewportId();
    MYON_DisplayMaterialRef found_dm;
    if ( attributes.FindDisplayMaterial(search_dm, &found_dm) )
    {
      display_material_id = found_dm.m_display_material_id;
    }

  Returns:
    True if a matching display material is found.
  See Also:
    MYON_3dmObjectAttributes::AddDisplayMaterialRef
    MYON_3dmObjectAttributes::RemoveDisplayMaterialRef
  */
  bool FindDisplayMaterialRef(
      const MYON_DisplayMaterialRef& search_material,
      MYON_DisplayMaterialRef* found_material = nullptr
    ) const;

  /*
  Description:
    Quick way to see if a viewport has a special material.
  Parameters:
    viewport_id - [in]
    display_material_id - [out]
  Returns:
    True if a material_id is assigned.
  */
  bool FindDisplayMaterialId( 
        const MYON_UUID& viewport_id, 
        MYON_UUID* display_material_id = nullptr
        ) const;
     
  /*
  Description:
    Add a display material reference to the attributes.  If
    there is an existing entry with a matching viewport id,
    the existing entry is replaced.
  Parameters:
    display_material - [in]
  Returns:
    True if input is valid (material id != nil)
  See Also:
    MYON_3dmObjectAttributes::FindDisplayMaterialRef
    MYON_3dmObjectAttributes::RemoveDisplayMaterialRef
  */
  bool AddDisplayMaterialRef(
    MYON_DisplayMaterialRef display_material
    );

  /*
  Description:
    Remove a display material reference from the list.
  Parameters:
    viewport_id - [in] Any display material references
      with this viewport id will be removed.  If nil,
      then viewport_id is ignored.
    display_material_id - [in]
      Any display material references that match the
      viewport_id and have this display_material_id
      will be removed.  If nil, then display_material_id
      is ignored.
  Returns:
    True if a display material reference was removed.
  See Also:
    MYON_3dmObjectAttributes::FindDisplayMaterialRef
    MYON_3dmObjectAttributes::AddDisplayMaterialRef
  */
  bool RemoveDisplayMaterialRef(
    MYON_UUID viewport_id,
    MYON_UUID display_material_id = MYON_nil_uuid
    );

  /*
  Description:
    Remove the entire display material reference list.
  */
  void RemoveAllDisplayMaterialRefs();

  /*
  Returns:
    Number of display material references.
  */
  int DisplayMaterialRefCount() const;

  MYON_SimpleArray<MYON_DisplayMaterialRef> m_dmref;

private:
  bool Internal_WriteV5( MYON_BinaryArchive& archive ) const;
  bool Internal_ReadV5( MYON_BinaryArchive& archive );
};

#endif

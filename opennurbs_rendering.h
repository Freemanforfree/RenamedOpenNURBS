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

#if !defined(OPENNURBS_RENDERING_INC_)
#define OPENNURBS_RENDERING_INC_


class MYON_CLASS MYON_RenderingAttributes
{
public:
  MYON_RenderingAttributes();
  void Default();
  int Compare( const MYON_RenderingAttributes& other ) const;
  bool Write( MYON_BinaryArchive& archive ) const;
  bool Read( MYON_BinaryArchive& archive );

  bool IsValid( MYON_TextLog* text_log ) const;


  const MYON_MaterialRef* MaterialRef( const MYON_UUID& plugin_id ) const;

  MYON_ClassArray<MYON_MaterialRef> m_materials;
};

class MYON_CLASS MYON_ObjectRenderingAttributes : public MYON_RenderingAttributes
{
public:
  MYON_ObjectRenderingAttributes();
  void Default();
  int Compare( const MYON_ObjectRenderingAttributes& other ) const;
  bool Write( MYON_BinaryArchive& archive ) const;
  bool Read( MYON_BinaryArchive& archive );

  bool IsValid( MYON_TextLog* text_log ) const;

  /*
  Description:
    Update mapping channel transformations.
  Parameters:
    xform - [in]
      Transformation applied to parent object.
  Returns:
    True is successful.  False if there are mapping channels
    and xform cannot be inverted.
  */
  bool Transform( const MYON_Xform& xform );

  /*
  Parameters:
    plugin_id - [in]
  Returns:
    A pointer to the plug-in's mapping reference, if there
    is one. Otherwise nullptr is returned.
  */
  const MYON_MappingRef* MappingRef( 
    const MYON_UUID& plugin_id 
    ) const;

  /*
  Parameters:
    plugin_id - [in]
  Returns:
    If a mapping ref exists, it is returned.  Otherwise
    one is added.
  */
  MYON_MappingRef* AddMappingRef( 
    const MYON_UUID& plugin_id 
    );

  /*
  Parameters:
    plugin_id - [in]
  Returns:
    If a mapping ref exists, it is returned.  Otherwise
    one is added.
  */
  bool DeleteMappingRef( 
    const MYON_UUID& plugin_id 
    );


  /*
  Parameters:
    plugin_id - [in]
    mapping_channel_id - [in]
    mapping_id - [in]
      MYON_TextureMapping id
  Returns:
    A pointer to the plug-in's mapping channel, if there
    is one. Otherwise nullptr is returned.
  */
  const MYON_MappingChannel* MappingChannel( 
    const MYON_UUID& plugin_id, 
    int mapping_channel_id
    ) const;

  const MYON_MappingChannel* MappingChannel( 
    const MYON_UUID& plugin_id, 
    const MYON_UUID& mapping_id
    ) const;

  /*
  Returns:
    The mapping channel id to use when calling MappingChannel to retrieve the OCS mapping if there is one.
    See CRhinoTextureMapping::OcsMappingTransformForObject for an easy helper function to get the transform per object
  */
  static int OCSMappingChannelId(void);

  /*
  Parameters:
    plugin_id - [in]
    mapping_channel_id - [in]
    mapping_id - [in]
      MYON_TextureMapping id
  Returns:
    True if the mapping channel was added or a pefect
    match already existed.  False if a mapping channel 
    with a different mapping_id already exists for this
    plug-in and channel.
  */
  bool AddMappingChannel(
    const MYON_UUID& plugin_id, 
    int mapping_channel_id,
    const MYON_UUID& mapping_id
    );

  /*
  Parameters:
    plugin_id - [in]
    mapping_channel_id - [in]
    mapping_id - [in]
      MYON_TextureMapping id
  Returns:
    True if a matching mapping channel was deleted.
  */
  bool DeleteMappingChannel(
    const MYON_UUID& plugin_id, 
    int mapping_channel_id
    );

  bool DeleteMappingChannel(
    const MYON_UUID& plugin_id, 
    const MYON_UUID& mapping_id
    );

  /*
  Parameters:
    plugin_id - [in]
    old_mapping_channel_id - [in]
    new_mapping_channel_id - [in]
  Returns:
    True if a matching mapping channel was found and changed.
  */
  bool ChangeMappingChannel(
    const MYON_UUID& plugin_id, 
    int old_mapping_channel_id,
    int new_mapping_channel_id
    );

  // Use AddMappingRef() or AddMappingChannel() if you 
  // want to add an element to this array.
  //
  // Every mapping ref in this array must have
  // a distinct value of MYON_MappingRef.m_plugin_id.
  MYON_ClassArray<MYON_MappingRef> m_mappings;

  /*
  Parameters:
    bEnable - [in]
      false - (default)
       Do not generate bitmap textures that 
       approximate procedural textures.
      true - 
       generate bitmap textures that approximate
       procedural textures and use these for
       quick previews.
  Returns:
    True if advancded texture preview is enabled.
  */
  void EnableAdvancedTexturePreview(bool b);

  /*
  Returns:
    True if advancded texture preview is enabled.
  */
  bool AdvancedTexturePreview() const;

  bool m_bCastsShadows;    // default is true
  bool m_bReceivesShadows; // default is true

private:
  // m_bits encodes 8 true/false settings
  unsigned char m_bits; // (m_bits & 1) == AdvancedTexturePreview();

  unsigned char m_reserved1;
};

#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_ClassArray<MYON_RenderingAttributes>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_ClassArray<MYON_ObjectRenderingAttributes>;
#endif


#endif


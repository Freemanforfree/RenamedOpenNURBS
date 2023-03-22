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

#if !defined(OPENNURBS_MAPPING_CHANNEL_INC_)
#define OPENNURBS_MAPPING_CHANNEL_INC_

///////////////////////////////////////////////////////////////////////////////
//
// Class MYON_MappingChannel
// 
// Description:
//   MYON_3dmObjectAttributes uses MYON_MappingChannel to record 
//   which texture mapping function to use when applying a texture
//   with a matching mapping channel id.
//   When an object is rendered, if the material has textures and
//   MYON_Texture::m_mapping_channel_id = MYON_MappingChannel::m_mapping_channel_id,
//   then the mapping with id m_mapping_id is used to map the texture.
//   Otherwise, the mesh m_T[] texture coordinates are used to
//   apply the texture.
//
class MYON_CLASS MYON_MappingChannel
{
public:
  MYON_MappingChannel();
  void Default();
  int Compare( const MYON_MappingChannel& other ) const;
  bool Write( MYON_BinaryArchive& archive ) const;
  bool Read( MYON_BinaryArchive& archive );

  MYON_UUID m_mapping_id;  // Identifies an MYON_TextureMapping

  // RUNTIME textrure mapping table index.
  // If -1, it needs to be set.  This value is not saved int files.
  int m_mapping_index;

  // MYON_Texture's with a matching m_mapping_channel_id value
  // use the mapping identified by m_mapping_id.  This id
  // must be > 0 and <= 2147483647 (0x7FFFFFFF)
  int m_mapping_channel_id;

  // The default value of m_object_xform is the identity.
  // When an object that uses this mapping is transformed
  // by "T", m_object_xform is updated using the formula
  // m_object_xform = T*m_object_xform.  If texture coordinates
  // are lost and need to be recalculated and m_object_xform
  // is not the identity, then m_object_xform should be passed
  // to MYON_TextureMapping::Evaluate() as the mesh_xform parameter.
  // When validating mapping coordinates, m_object_xform itself 
  // be passed to HasMatchingTextureCoordinates() as the
  // object_transform parameter.
  MYON_Xform m_object_xform;
};

#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_MappingChannel>;
#endif

///////////////////////////////////////////////////////////////////////////////
//
// Class MYON_MaterialRef
// 
// Description:
//   MYON_3dmObjectAttributes uses MYON_MaterialRef to record which
//   rendering material and mappings a rendering plug-in wants to 
//   use.  This allows different rendering plug-ins to have different 
//   materials on the same object.  The values of
//   MYON_3dmObjectAttributes.m_material_index and 
//   MYON_3dmObjectAttributes.m_matrial_source reflect the settings
//   of the renderer that is currently active.
//

class MYON_CLASS MYON_MappingRef
{
public:
  MYON_MappingRef();
  void Default();
  int Compare( const MYON_MappingRef& other ) const;
  bool Write( MYON_BinaryArchive& archive ) const;
  bool Read( MYON_BinaryArchive& archive );

  bool IsValid( MYON_TextLog* text_log ) const;


  bool Transform( const MYON_Xform& xform );

  MYON_UUID m_plugin_id;   // Identifies a rendering plugin

  /*
  Parameters:
    mapping_channel_id - [in]
    mapping_id - [in]
      MYON_TextureMapping id
  Returns:
    A pointer to the plug-in's mapping channel, if there
    is one. Otherwise nullptr is returned.
  */
  const MYON_MappingChannel* MappingChannel( 
    int mapping_channel_id
    ) const;

  const MYON_MappingChannel* MappingChannel( 
    const MYON_UUID& mapping_id
    ) const;


  /*
  Parameters:
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
    int mapping_channel_id,
    const MYON_UUID& mapping_id
    );

  /*
  Parameters:
    mapping_channel_id - [in]
    mapping_id - [in]
      MYON_TextureMapping id
  Returns:
    True if a matching mapping channel was deleted.
  */
  bool DeleteMappingChannel(
    int mapping_channel_id
    );

  bool DeleteMappingChannel(
    const MYON_UUID& mapping_id
    );

  /*
  Parameters:
    old_mapping_channel_id - [in]
    new_mapping_channel_id - [in]
  Returns:
    True if a matching mapping channel was found and changed.
  */
  bool ChangeMappingChannel(
    int old_mapping_channel_id,
    int new_mapping_channel_id
    );

  // Use AddMappingChannel() if you  want to add an 
  // element to this array.
  //
  // Every mapping channel in this array must have
  // a distinct value of MYON_MappingChannel.m_mapping_channel_id
  MYON_SimpleArray<MYON_MappingChannel> m_mapping_channels;
};

class MYON_CLASS MYON_MaterialRef
{
public:
  // If m_material_id = MYON_MaterialRef::material_from_layer,
  // then the object's layer determine the material.
  // See MYON::material_from_layer.
  //static const MYON_UUID material_from_layer; // TOD0 - remove this

  // If m_material_id = MYON_MaterialRef::material_from_layer,
  // then the object's parent determine the material.
  // See MYON::material_from_parent.
  //static const MYON_UUID material_from_parent; // TODO - remove this

  MYON_MaterialRef();
  void Default();
  int Compare( const MYON_MaterialRef& other ) const;
  bool Write( MYON_BinaryArchive& archive ) const;
  bool Read( MYON_BinaryArchive& archive );

  MYON_UUID m_plugin_id;   // Identifies a rendering plugin

  MYON_UUID m_material_id; // Identifies an MYON_Material
  
  // If nil, then m_material_id is used for front and back faces
  MYON_UUID m_material_backface_id; // Identifies an MYON_Material

  MYON::object_material_source MaterialSource() const;
  void SetMaterialSource(
    MYON::object_material_source
    );
  unsigned char m_material_source; // MYON::object_material_source values
  unsigned char m_reserved1;
  unsigned char m_reserved2;
  unsigned char m_reserved3;

  // RUNTIME material table index for m_material_id.
  // This value is not saved in files.  If -1, then it
  // needs to be set.
  int m_material_index;

  // RUNTIME material table index for m_material_id.
  // This value is not saved in files.  If -1, then it
  // needs to be set.
  int m_material_backface_index;
};

#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_ClassArray<MYON_MaterialRef>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_ClassArray<MYON_MappingRef>;
#endif

#endif



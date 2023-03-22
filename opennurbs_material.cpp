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

#include "opennurbs.h"

#if !defined(MYON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// MYON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, MYON_COMPILING_OPENNURBS is defined
// and the opennurbs .h files alter what is declared and how it is declared.
#error MYON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif


////////////////////////////////////////////////////////////////
//   Class MYON_Material
////////////////////////////////////////////////////////////////

MYON_OBJECT_IMPLEMENT(MYON_Material,MYON_ModelComponent,"60B5DBBC-E660-11d3-BFE4-0010830122F0");


const MYON_Material* MYON_Material::FromModelComponentRef(
  const class MYON_ModelComponentReference& model_component_reference,
  const MYON_Material* none_return_value
  )
{
  const MYON_Material* p = MYON_Material::Cast(model_component_reference.ModelComponent());
  return (nullptr != p) ? p : none_return_value;
}

// Default constructor
MYON_Material::MYON_Material() MYON_NOEXCEPT
  : MYON_ModelComponent(MYON_ModelComponent::Type::RenderMaterial)
{}

MYON_Material::MYON_Material( const MYON_Material& src)
  : MYON_ModelComponent(MYON_ModelComponent::Type::RenderMaterial,src)
{
  Internal_CopyFrom(src);
}

void MYON_Material::Internal_CopyFrom(
  const MYON_Material& src
  )
{
#define MYON_COPY_SRC(m) m=src.m
  MYON_COPY_SRC(m_rdk_material_instance_id);
  MYON_COPY_SRC(m_ambient);
  MYON_COPY_SRC(m_diffuse);
  MYON_COPY_SRC(m_emission);
  MYON_COPY_SRC(m_specular);
  MYON_COPY_SRC(m_reflection);
  MYON_COPY_SRC(m_transparent);
  MYON_COPY_SRC(m_bShareable);
  MYON_COPY_SRC(m_bDisableLighting);
  MYON_COPY_SRC(m_bUseDiffuseTextureAlphaForObjectTransparencyTexture);
  MYON_COPY_SRC(m_bFresnelReflections);
  MYON_COPY_SRC(m_reflectivity);
  MYON_COPY_SRC(m_shine);
  MYON_COPY_SRC(m_transparency);
  MYON_COPY_SRC(m_reflection_glossiness);
  MYON_COPY_SRC(m_refraction_glossiness);
  MYON_COPY_SRC(m_index_of_refraction);
  MYON_COPY_SRC(m_fresnel_index_of_refraction);
  MYON_COPY_SRC(m_textures);
  MYON_COPY_SRC(m_material_channel);
  MYON_COPY_SRC(m_plugin_id);
#undef MYON_COPY_SRC
}

bool MYON_Material::IsValid( MYON_TextLog* text_log ) const
{
  return true;
}

void
MYON_Material::Dump( MYON_TextLog& dump ) const
{
  MYON_ModelComponent::Dump(dump);

  dump.Print("ambient rgb = "); dump.PrintRGB( m_ambient ); dump.Print("\n");
  dump.Print("diffuse rgb = "); dump.PrintRGB( m_diffuse ); dump.Print("\n");
  dump.Print("emmisive rgb = "); dump.PrintRGB( m_emission ); dump.Print("\n");
  dump.Print("specular rgb = "); dump.PrintRGB( m_specular ); dump.Print("\n");
  dump.Print("reflection rgb = "); dump.PrintRGB( m_reflection ); dump.Print("\n");
  dump.Print("transparent rgb = "); dump.PrintRGB( m_transparent ); dump.Print("\n");
  dump.Print("shine = %g%%\n",100.0*m_shine/MYON_Material::MaxShine );
  dump.Print("transparency = %g%%\n",100.0*m_transparency);
  dump.Print("reflectivity = %g%%\n",100.0*m_reflectivity);
  dump.Print("index of refraction = %g\n",m_index_of_refraction);

  dump.Print("plug-in id = "); dump.Print(m_plugin_id); dump.Print("\n");
  int i;
  for( i = 0; i < m_textures.Count(); i++ )
  {
    dump.Print("texture[%d]:\n",i);
    dump.PushIndent();
    m_textures[i].Dump(dump);
    dump.PopIndent();
  }
}


MYON_UUID MYON_Material::MaterialPlugInId() const
{
  return m_plugin_id;
}

void MYON_Material::SetMaterialPlugInId(
  MYON_UUID plugin_id
  )
{
  if (m_plugin_id != plugin_id)
  {
    m_plugin_id = plugin_id;
    IncrementContentVersionNumber();
  }
}

bool MYON_Material::Write( MYON_BinaryArchive& archive ) const
{
  if (archive.Archive3dmVersion() < 60)
    return Internal_WriteV5(archive);

  const int major_version = 1;
  const int minor_version = 0;
  if (!archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,major_version,minor_version))
    return false;

  bool rc = false;
  for (;;)
  {
    const unsigned int attributes_filter
      = MYON_ModelComponent::Attributes::IndexAttribute
      | MYON_ModelComponent::Attributes::IdAttribute
      | MYON_ModelComponent::Attributes::NameAttribute;
    if ( !WriteModelComponentAttributes(archive,attributes_filter)) break;

    if ( !archive.WriteUuid(m_plugin_id) ) break;

    if ( !archive.WriteColor( m_ambient ) ) break;
    if ( !archive.WriteColor( m_diffuse ) ) break;
    if ( !archive.WriteColor( m_emission ) ) break;
    if ( !archive.WriteColor( m_specular ) ) break;
    if ( !archive.WriteColor( m_reflection ) ) break;
    if ( !archive.WriteColor( m_transparent ) ) break;

    if ( !archive.WriteDouble( m_index_of_refraction ) ) break;
    if ( !archive.WriteDouble( m_reflectivity ) ) break;
    if ( !archive.WriteDouble( m_shine ) ) break;
    if (!archive.WriteDouble(m_transparency)) break;

    // array of textures written in a way that user data persists
    {
      const int textures_major_version = 1;
      const int textures_minor_version = 0;
      if (!archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK, textures_major_version, textures_minor_version))
        break;
      bool textures_chunk_rc = false;
      for (;;)
      {
        const unsigned int count = m_textures.Count();
        if (!archive.WriteInt(count))
          break;
        unsigned int i;
        for (i = 0; i < count; i++)
        {
          if (!archive.WriteObject(&m_textures[i]))
            break;
        }
        if (i < count)
          break;
        textures_chunk_rc = true;
        break;
      }
      if (!archive.EndWrite3dmChunk())
        textures_chunk_rc = false;
      if ( !textures_chunk_rc )
        break;
    }

    if ( !archive.WriteArray(m_material_channel) )
      break;
    if ( !archive.WriteBool(m_bShareable) )
      break;
    if ( !archive.WriteBool(m_bDisableLighting))
      break;
    if ( !archive.WriteBool(m_bFresnelReflections))
      break;
    if ( !archive.WriteDouble(m_reflection_glossiness))
      break;
    if ( !archive.WriteDouble(m_refraction_glossiness))
      break;
    if ( !archive.WriteDouble(m_fresnel_index_of_refraction))
      break;
    if ( !archive.WriteUuid(m_rdk_material_instance_id))
      break;
    if ( !archive.WriteBool(m_bUseDiffuseTextureAlphaForObjectTransparencyTexture))
      break;

    rc = true;
    break;
  }

  if (!archive.EndWrite3dmChunk())
    rc = false;

  return rc;
}


bool MYON_Material::Read( MYON_BinaryArchive& archive )
{
  *this = MYON_Material::Unset;

  if (archive.Archive3dmVersion() < 60)
    return Internal_ReadV5(archive);
  if (archive.ArchiveOpenNURBSVersion() <  2348833910)
    return Internal_ReadV5(archive);

  bool rc = false;
  int major_version = 0;
  int minor_version = 0;
  if (!archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_version,&minor_version))
    return false;
  for (;;)
  {
    if (1 != major_version)
      break;

    if ( !ReadModelComponentAttributes(archive)) break;

    if ( !archive.ReadUuid(m_plugin_id) ) break;

    if ( !archive.ReadColor( m_ambient ) ) break;
    if ( !archive.ReadColor( m_diffuse ) ) break;
    if ( !archive.ReadColor( m_emission ) ) break;
    if ( !archive.ReadColor( m_specular ) ) break;
    if ( !archive.ReadColor( m_reflection ) ) break;
    if ( !archive.ReadColor( m_transparent ) ) break;

    if ( !archive.ReadDouble( &m_index_of_refraction ) ) break;
    if ( !archive.ReadDouble( &m_reflectivity ) ) break;
    if ( !archive.ReadDouble( &m_shine ) ) break;
    if (!archive.ReadDouble(&m_transparency)) break;

    // array of textures read in a way that user data persists
    {
      int textures_major_version = 0;
      int textures_minor_version = 0;
      if (!archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK, &textures_major_version, &textures_minor_version))
        break;
      bool textures_chunk_rc = false;
      for (;;)
      {
        if (1 != textures_major_version)
          break;

        unsigned int count = 0;
        if ( !archive.ReadInt(&count) )
          break;
        m_textures.SetCount(0);
        m_textures.Reserve(count);
        unsigned int i;
        for (i = 0; i < count; i++)
        {
          if (!archive.ReadObject(m_textures.AppendNew()))
            break;
        }
        if (i < count)
          break;
        textures_chunk_rc = true;
        break;
      }
      if (!archive.EndRead3dmChunk())
        textures_chunk_rc = false;
      if (!textures_chunk_rc)
        break;
    }

    if ( !archive.ReadArray(m_material_channel) )
      break;
    if ( !archive.ReadBool(&m_bShareable) )
      break;
    if ( !archive.ReadBool(&m_bDisableLighting))
      break;
    if ( !archive.ReadBool(&m_bFresnelReflections))
      break;
    if ( !archive.ReadDouble(&m_reflection_glossiness))
      break;
    if ( !archive.ReadDouble(&m_refraction_glossiness))
      break;
    if ( !archive.ReadDouble(&m_fresnel_index_of_refraction))
      break;
    if ( !archive.ReadUuid(m_rdk_material_instance_id))
      break;
    if ( !archive.ReadBool(&m_bUseDiffuseTextureAlphaForObjectTransparencyTexture))
      break;

    rc = true;
    break;
  }
  if (!archive.EndRead3dmChunk())
    rc = false;
  return rc;
}

bool MYON_Material::Internal_WriteV5( MYON_BinaryArchive& file ) const
{
  bool rc = false;
  if ( file.Archive3dmVersion() <= 3 )
  {
    // V2 or V3 file format
    rc = Internal_WriteV3(file);
  }
  else
  {
    // V4 file format

    // The chunk version 2.0 prevents old V3 IO code from attempting
    // to read this material
    rc = file.Write3dmChunkVersion(2,0); // never change the 2,0


    // version 1.2 field (20061113*)
    // version 1.3 fields (20100917*)
    // version 1.4 fields 6.0.2013-11-6
    // version 1.5 fields 6.0.2014-05-16 (RDK material instance id)
    // version 1.6 fields 6.0.2014-07-11 (m_bUseDiffuseTextureAlphaForObjectTransparencyTexture)
    const int minor_version = (file.Archive3dmVersion() >= 60) ? 6 : 3;
    if (rc) rc = file.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,minor_version);
    if (rc)
    {
      for(;;)
      {
        if ( rc ) rc = file.WriteUuid(Id());
        if ( rc ) rc = file.Write3dmReferencedComponentIndex(*this);
        if ( rc ) rc = file.WriteString(Name());

        if ( rc ) rc = file.WriteUuid(m_plugin_id);

        if ( rc ) rc = file.WriteColor( m_ambient );
        if ( rc ) rc = file.WriteColor( m_diffuse );
        if ( rc ) rc = file.WriteColor( m_emission );
        if ( rc ) rc = file.WriteColor( m_specular );
        if ( rc ) rc = file.WriteColor( m_reflection );
        if ( rc ) rc = file.WriteColor( m_transparent );

        if ( rc ) rc = file.WriteDouble( m_index_of_refraction );
        if ( rc ) rc = file.WriteDouble( m_reflectivity );
        if ( rc ) rc = file.WriteDouble( m_shine );
        if ( rc ) rc = file.WriteDouble( m_transparency );

        if ( !rc )
          break;

        // array of textures written in a way that user data persists
        rc = file.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,0);
        if (rc)
        {
          int i, count = m_textures.Count();
          rc = file.WriteInt(count);
          for ( i = 0; i < count && rc; i++ )
          {
            rc = file.WriteObject(&m_textures[i]);
          }
          if ( !file.EndWrite3dmChunk() )
            rc = false;
        }

        //version 1.1 field
        MYON_wString obsolete_flamingo_library = MYON_wString::EmptyString;
        if (rc) rc = file.WriteString(obsolete_flamingo_library);

        // version 1.2 field (20061113)
        if (rc) rc = file.WriteArray(m_material_channel);

        // version 1.3 fields (20100917*)
        rc = file.WriteBool(m_bShareable);
        if (!rc) break;
        rc = file.WriteBool(m_bDisableLighting);
        if (!rc) break;

        if ( minor_version >= 4 )
        {
          rc = file.WriteBool(m_bFresnelReflections);
          if (!rc) break;
          rc = file.WriteDouble(m_reflection_glossiness);
          if (!rc) break;
          rc = file.WriteDouble(m_refraction_glossiness);
          if (!rc) break;
          rc = file.WriteDouble(m_fresnel_index_of_refraction);
          if (!rc) break;
          if (minor_version >= 5)
          {
            rc = file.WriteUuid(m_rdk_material_instance_id);
            if (!rc) break;
          }
          if (minor_version >= 6)
          {
            rc = file.WriteBool(m_bUseDiffuseTextureAlphaForObjectTransparencyTexture);
            if (!rc) break;
          }
        }

        break;
      }
      if (!file.EndWrite3dmChunk() )
        rc = false;
    }
  }

  if (rc && file.Archive3dmVersion() < 60)
  {
    if (RdkMaterialInstanceIdIsNotNil())
    {
      // For V5 files and earlier, we need to save
      // the RDK material instance id as user data.
      // Because MYON_RdkMaterialInstanceIdObsoleteUserData.DeleteAfterWrite()
      // returns true, the user data we are attaching here will be deleted
      // after it is written.
      MYON_RdkMaterialInstanceIdObsoleteUserData* ud = new MYON_RdkMaterialInstanceIdObsoleteUserData();
      ud->m_rdk_material_instance_id = RdkMaterialInstanceId();
      const_cast<MYON_Material*>(this)->AttachUserData(ud);
    }
  }

  return rc;
}

bool MYON_Material::Internal_ReadV5( MYON_BinaryArchive& file )
{
  int major_version = 0;
  int minor_version = 0;
  bool rc = file.Read3dmChunkVersion(&major_version,&minor_version);
  if (rc)
  {
    if ( 1 == major_version )
    {
      rc = Internal_ReadV3(file,minor_version);
    }
    else if ( 2 == major_version )
    {
      // fancy V4 material
      // V4 file format

      // The chunk version 2.0 prevents old V3 IO code from attempting
      // to read this material
      rc = file.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_version,&minor_version);
      if (rc)
      {
        for(;;)
        {
          MYON_UUID material_id = Id();
          if ( rc ) rc = file.ReadUuid(material_id);
          if (rc) SetId(material_id);

          int material_index = Index();
          if ( rc ) rc = file.ReadInt(&material_index);
          if (rc) SetIndex(material_index);

          MYON_wString material_name;
          if ( rc ) rc = file.ReadString(material_name);
          if (rc) SetName(material_name);

          if ( rc ) rc = file.ReadUuid(m_plugin_id);

          if ( rc ) rc = file.ReadColor( m_ambient );
          if ( rc ) rc = file.ReadColor( m_diffuse );
          if ( rc ) rc = file.ReadColor( m_emission );
          if ( rc ) rc = file.ReadColor( m_specular );
          if ( rc ) rc = file.ReadColor( m_reflection );
          if ( rc ) rc = file.ReadColor( m_transparent );

          if ( rc
               && file.ArchiveOpenNURBSVersion() < 200912010
               && 128 == m_transparent.Red()
               && 128 == m_transparent.Green()
               && 128 == m_transparent.Blue()
               )
          {
            // Prior to 1 Dec 2009 the MYON_Material::Defaults() set
            // m_transparent to 128,128,128.  This was the wrong
            // value for the default.  This "hack" is here to
            // make it appear that the default was always white.
            m_transparent = m_diffuse;
          }

          if ( rc ) rc = file.ReadDouble( &m_index_of_refraction );
          if ( rc ) rc = file.ReadDouble( &m_reflectivity );
          if ( rc ) rc = file.ReadDouble( &m_shine );
          if ( rc ) rc = file.ReadDouble( &m_transparency );

          if ( !rc )
            break;

          // array of textures read in a way that user data persists
          int texmajver = 0;
          int texminver = 0;
          rc = file.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&texmajver,&texminver);
          if (rc)
          {
            if ( 1 == texmajver )
            {
              int i, count = 0;
              rc = file.ReadInt(&count);
              if (rc) m_textures.Reserve(count);
              for ( i = 0; i < count && rc; i++ )
              {
                int trc = file.ReadObject(m_textures.AppendNew());
                if (trc <= 0)
                {
                  // http://mcneel.myjetbrains.com/youtrack/issue/RH-30204
                  // Part of fixing crash reading corrupt file sem2observe copy.3dm
                  rc = false;
                  m_textures.Remove();
                }
                else if ( trc > 1 )
                  m_textures.Remove();
              }
            }
            if ( !file.EndRead3dmChunk() )
              rc = false;
          }

          if ( rc && minor_version >= 1 )
          {
            MYON_wString obsolete_flamingo_library;
            rc = file.ReadString(obsolete_flamingo_library);
            if ( !rc ) break;

            if ( minor_version >= 2 )
            {
              // version 1.2 field (20061113)
              rc = file.ReadArray(m_material_channel);
              if (!rc) break;

              if ( minor_version >= 3 )
              {
                // version 1.3 fields (20100917*)
                rc = file.ReadBool(&m_bShareable);
                if (!rc) break;
                rc = file.ReadBool(&m_bDisableLighting);
                if (!rc) break;
                if ( minor_version >= 4 )
                {
                  rc = file.ReadBool(&m_bFresnelReflections);
                  if (!rc) break;
                  rc = file.ReadDouble(&m_reflection_glossiness);
                  if (!rc) break;
                  rc = file.ReadDouble(&m_refraction_glossiness);
                  if (!rc) break;
                  rc = file.ReadDouble(&m_fresnel_index_of_refraction);
                  if (!rc) break;

                  if (minor_version >= 5)
                  {
                    // version 1.5 field 6.0.2014-05-16
                    rc = file.ReadUuid(m_rdk_material_instance_id);
                    if (!rc) break;
                  }

                  if (minor_version >= 6)
                  {
                    // version 1.6 field 6.0.2014-07-11
                    rc = file.ReadBool(&m_bUseDiffuseTextureAlphaForObjectTransparencyTexture);
                    if (!rc) break;
                  }
                }
              }
            }

          }

          break;
        }
        if (!file.EndRead3dmChunk() )
          rc = false;
      }
    }
  }
  return rc;
}


bool MYON_Material::Internal_WriteV3( MYON_BinaryArchive& file ) const
{
  int i;
  // V2 and V3 file format

  bool rc = file.Write3dmChunkVersion(1,1);
  if ( rc ) rc = file.WriteColor( m_ambient );
  if ( rc ) rc = file.WriteColor( m_diffuse );
  if ( rc ) rc = file.WriteColor( m_emission );
  if ( rc ) rc = file.WriteColor( m_specular );
  if ( rc ) rc = file.WriteDouble( Shine() );
  if (rc) rc = file.WriteDouble(m_transparency);

  if (rc) rc = file.WriteChar((unsigned char)1); // OBSOLETE // m_casts_shadows
  if (rc) rc = file.WriteChar((unsigned char)1); // OBSOLETE // m_shows_shadows

  if (rc) rc = file.WriteChar((unsigned char)0); // OBSOLETE // m_wire_mode
  if (rc) rc = file.WriteChar((unsigned char)2); // OBSOLETE // m_wire_density

  if (rc) rc = file.WriteColor(MYON_Color(0, 0, 0)); // OBSOLETE // m_wire_color

  if (rc)
  {
    // OBSOLETE - line style info never used
    short s = 0;
    if (rc) rc = file.WriteShort(s);
    if (rc) rc = file.WriteShort(s);
    if (rc) rc = file.WriteDouble(0.0);
    if (rc) rc = file.WriteDouble(1.0);
  }

  MYON_wString filename;
  int j = 0;
  i = FindTexture(nullptr, MYON_Texture::TYPE::bitmap_texture);
  if (i >= 0)
  {
    const MYON_Texture& tmap = m_textures[i];
    const MYON_wString tmap_filename = tmap.m_image_file_reference.FullPath();
    if (tmap_filename.Length() > 0)
    {
      filename = tmap_filename;
      j = (MYON_Texture::MODE::decal_texture == tmap.m_mode) ? 2 : 1;
    }
  }
  // OBSOLETE // if ( rc ) rc = file.WriteString( TextureBitmapFileName() );
  // OBSOLETE // i = TextureMode();
  // OBSOLETE // if ( rc ) rc = file.WriteInt( i );
  // OBSOLETE // if ( rc ) rc = file.WriteInt( m_texture_bitmap_index );
  if (rc) rc = file.WriteString(filename);
  if (rc) rc = file.WriteInt(j);
  if (rc) rc = file.WriteInt(0);

  filename.Destroy();
  j = 0;
  double bump_scale = 1.0;
  i = FindTexture(nullptr, MYON_Texture::TYPE::bump_texture);
  if (i >= 0)
  {
    const MYON_Texture& tmap = m_textures[i];
    const MYON_wString tmap_filename = tmap.m_image_file_reference.FullPath();
    if (tmap_filename.Length() > 0)
    {
      filename = tmap_filename;
      j = (MYON_Texture::MODE::decal_texture == tmap.m_mode) ? 2 : 1;
      bump_scale = tmap.m_bump_scale[1];
    }
  }
  // OBSOLETE //if ( rc ) rc = file.WriteString( BumpBitmapFileName() );
  // OBSOLETE //i = BumpMode();
  // OBSOLETE //if ( rc ) rc = file.WriteInt( i );
  // OBSOLETE //if ( rc ) rc = file.WriteInt( m_bump_bitmap_index );
  // OBSOLETE //if ( rc ) rc = file.WriteDouble( m_bump_scale );
  if (rc) rc = file.WriteString(filename);
  if (rc) rc = file.WriteInt(j);
  if (rc) rc = file.WriteInt(0);
  if (rc) rc = file.WriteDouble(bump_scale);

  filename.Destroy();
  j = 0;
  i = FindTexture(nullptr, MYON_Texture::TYPE::emap_texture);
  if (i >= 0)
  {
    const MYON_Texture& tmap = m_textures[i];
    const MYON_wString tmap_filename = tmap.m_image_file_reference.FullPath();
    if (tmap_filename.Length() > 0)
    {
      filename = tmap_filename;
      j = (MYON_Texture::MODE::decal_texture == tmap.m_mode) ? 2 : 1;
    }
  }
  // OBSOLETE //if ( rc ) rc = file.WriteString( EmapBitmapFileName() );
  // OBSOLETE //i = EmapMode();
  // OBSOLETE //if ( rc ) rc = file.WriteInt( i );
  // OBSOLETE //if ( rc ) rc = file.WriteInt( m_emap_bitmap_index );
  if (rc) rc = file.WriteString(filename);
  if (rc) rc = file.WriteInt(j);
  if (rc) rc = file.WriteInt(0);

  if (rc) rc = file.Write3dmReferencedComponentIndex(*this);

  if (rc) rc = file.WriteUuid(m_plugin_id);

  const MYON_wString obsolete_flamingo_library = MYON_wString::EmptyString;
  if (rc) rc = file.WriteString(obsolete_flamingo_library);
  if (rc) rc = file.WriteString(Name());


  // 1.1 fields
  if (rc) rc = file.WriteUuid(Id());
  if (rc) rc = file.WriteColor(m_reflection);
  if (rc) rc = file.WriteColor(m_transparent);
  if (rc) rc = file.WriteDouble(m_index_of_refraction);

  return rc;
}



bool MYON_Material::Internal_ReadV3( MYON_BinaryArchive& file, int minor_version )
{
  double shine = 0.0, transparency = 0.0;
  int i, j;
  bool rc = true;
  {
    // common to all version 1.x formats
    if ( rc ) rc = file.ReadColor( m_ambient );
    if ( rc ) rc = file.ReadColor( m_diffuse );
    if ( rc ) rc = file.ReadColor( m_emission );
    if ( rc ) rc = file.ReadColor( m_specular );
    if ( rc ) rc = file.ReadDouble( &shine );
    if ( rc ) SetShine(shine);
    if ( rc ) rc = file.ReadDouble( &transparency );
    if ( rc ) SetTransparency(transparency);

    unsigned char obsolete_uc;
    if ( rc ) rc = file.ReadChar( &obsolete_uc ); // m_casts_shadows
    if ( rc ) rc = file.ReadChar( &obsolete_uc ); // m_shows_shadows

    if ( rc ) rc = file.ReadChar( &obsolete_uc ); // m_wire_mode
    if ( rc ) rc = file.ReadChar( &obsolete_uc ); // m_wire_density

    MYON_Color obsolete_color;
    if ( rc ) rc = file.ReadColor( obsolete_color ); // m_wire_color

    if (rc)
    {
      // OBSOLETE if ( rc ) rc = file.ReadLineStyle( m_wire_style );
      short s;
      double x;
      if (rc) rc = file.ReadShort(&s);
      if (rc) rc = file.ReadShort(&s);
      if (rc) rc = file.ReadDouble(&x);
      if (rc) rc = file.ReadDouble(&x);
    }

    MYON_wString str;

    if ( rc ) rc = file.ReadString( str ); //sTextureBitmapFileName
    i = 0;
    j = 0;
    if ( rc ) rc = file.ReadInt( &i );
    // OBSOLETE // if ( rc ) SetTextureMode( MYON::TextureMode(i) );
    if ( rc ) rc = file.ReadInt( &j );//&m_texture_bitmap_index

    if ( rc && !str.IsEmpty() )
    {
      MYON_Texture& texture = m_textures[AddTexture(static_cast< const wchar_t* >(str),MYON_Texture::TYPE::bitmap_texture)];
      if ( 2 == i )
      {
        texture.m_mode = MYON_Texture::MODE::decal_texture;
      }
      else
      {
        texture.m_mode = MYON_Texture::MODE::modulate_texture;
      }
    }

    if ( rc ) rc = file.ReadString( str ); // sBumpBitmapFileName
    if ( rc ) rc = file.ReadInt( &i );
   // OBSOLETE // if ( rc ) SetBumpMode( MYON::TextureMode(i) );
    if ( rc ) rc = file.ReadInt( &j );//&m_bump_bitmap_index );
    double bump_scale = 0.0;
    if ( rc ) rc = file.ReadDouble( &bump_scale );

    if ( rc && !str.IsEmpty() )
    {
      MYON_Texture& texture = m_textures[AddTexture(static_cast< const wchar_t* >(str),MYON_Texture::TYPE::bump_texture)];
      if ( 2 == i )
      {
        texture.m_mode = MYON_Texture::MODE::decal_texture;
      }
      else
      {
        texture.m_mode = MYON_Texture::MODE::modulate_texture;
      }
      texture.m_bump_scale.Set(0.0,bump_scale);
    }

    if ( rc ) rc = file.ReadString( str ); // sEmapBitmapFileName
    if ( rc ) rc = file.ReadInt( &i );
    // OBSOLETE // if ( rc ) SetEmapMode( MYON::TextureMode(i) );
    if ( rc ) rc = file.ReadInt( &j ); //&m_emap_bitmap_index;

    if ( rc && !str.IsEmpty() )
    {
      MYON_Texture& texture = m_textures[AddTexture(static_cast< const wchar_t* >(str),MYON_Texture::TYPE::emap_texture)];
      if ( 2 == i )
      {
        texture.m_mode = MYON_Texture::MODE::decal_texture;
      }
      else
      {
        texture.m_mode = MYON_Texture::MODE::modulate_texture;
      }
    }

    int material_index = Index();
    if ( rc ) rc = file.ReadInt( &material_index );
    if (rc) SetIndex(material_index);

    if ( rc ) rc = file.ReadUuid( m_plugin_id );

    MYON_wString obsolete_flamingo_library;
    if ( rc ) rc = file.ReadString( obsolete_flamingo_library );

    MYON_wString material_name;
    if ( rc ) rc = file.ReadString( material_name );
    if (rc) SetName(material_name);

    if ( minor_version >= 1 )
    {
      // 1.1 fields
      MYON_UUID material_id = Id();
      if (rc) rc = file.ReadUuid( material_id );
      SetId(material_id);
      if (rc) rc = file.ReadColor( m_reflection);
      if (rc) rc = file.ReadColor( m_transparent);
      if (rc) rc = file.ReadDouble( &m_index_of_refraction );
    }
    else
    {
      // old material needs a valid non-nil id.
      SetId();
    }

  }

  return rc;
}

MYON::object_type MYON_Material::ObjectType() const
{
   return MYON::material_object;
}

int MYON_Material::FindTexture( const wchar_t* filename,
                              MYON_Texture::TYPE type,
                              int i0
                              ) const
{
  int i, count = m_textures.Count();
  for (i = ((i0 < 0) ? 0 : (i0+1)); i < count; i++ )
  {
    if (    type != m_textures[i].m_type
         && type != MYON_Texture::TYPE::no_texture_type )
    {
      continue;
    }
    const MYON_wString texture_file_name = m_textures[i].m_image_file_reference.FullPath();
    if ( filename && texture_file_name.ComparePath(filename) )
    {
      continue;
    }
    return i;
  }
  return -1;
}

double MYON_Material::FresnelReflectionCoefficient(
  double fresnel_index_of_refraction,
  const double N[3],
  const double R[3]
  )
{
  double x,y,z,c,g;

  for(;;)
  {
    x = N[0]-R[0];
    y = N[1]-R[1];
    z = N[2]-R[2];
    c = MYON_Length3d(x,y,z);
    if ( !(c > MYON_DBL_MIN) )
      break;

    c = (N[0]*x + N[1]*y + N[2]*z)/c; // c = N o (N - R) / |N - R|

    g = fresnel_index_of_refraction*fresnel_index_of_refraction + c*c - 1.0;
    g = (g > 0.0) ? sqrt(g) : 0.0;

    // unsafe (potiential divide by zero, overflow, ...) and inefficient
    // return ( ((g-c)*(g-c))/(2*(g+c)*(g+c)) ) * (1.0f + ( ((c*(g+c)-1.0)*(c*(g+c)-1.0))/((c*(g+c)+1.0)*(c*(g+c)+1.0)) ) );

    x = g+c;
    if ( !(x != 0.0) )
      break; // x is NAN or zero

    z = (g-c)/x; // z = (g-c)/(g+c)
    if ( fabs(z) <= 1.0e-154 )
      return 0.0; // z*z below will be zero or denormalized

    x *= c; // x = c*(g+c)

    y = x + 1.0; // y = c*(g+c) + 1.0
    if ( !(y != 0.0) )
      break; // y is NAN or zero

    y = (x - 1.0)/y; // y = (c*(g+c) - 1.0)/(c*(g+c) + 1.0)

    x =  0.5*z*z*(1.0 + y*y);
    if ( !(x == x) )
      break; // x is NAN

    if ( !MYON_IS_FINITE(x) )
      break; // x is infinity

    return x; // x is
  }

  return 1.0; // error occured
}

double  MYON_Material::FresnelReflectionCoefficient(
  MYON_3dVector N,
  MYON_3dVector R
  ) const
{
  return m_bFresnelReflections
    ? MYON_Material::FresnelReflectionCoefficient(m_fresnel_index_of_refraction,&N.x,&R.x)
    : 1.0;
}

int MYON_Material::FindTexture( MYON_UUID texture_id ) const
{
  int i, count = m_textures.Count();
  for (i = 0; i < count; i++ )
  {
    if ( !MYON_UuidCompare(&texture_id,&m_textures[i].m_texture_id) )
      return i;
  }
  return -1;
}

int MYON_Material::DeleteTexture(const wchar_t* filename,MYON_Texture::TYPE type )
{
  int deleted_count = 0;
  int i;

  if ( !filename && type == MYON_Texture::TYPE::no_texture_type )
  {
    deleted_count = m_textures.Count();
    m_textures.Destroy();
  }
  else
  {
    for ( i = m_textures.Count()-1; i >= 0; i--)
    {
      if ( type != MYON_Texture::TYPE::no_texture_type && type != m_textures[i].m_type )
        continue;
      if ( filename && m_textures[i].m_image_file_reference.FullPath().ComparePath(filename) )
        continue;
      m_textures.Remove(i);
      deleted_count++;
    }
  }
  return deleted_count;
}

int MYON_Material::AddTexture( const MYON_Texture& tx )
{
  // has to copy user data
  int i = FindTexture( static_cast< const wchar_t* >(tx.m_image_file_reference.FullPath()), tx.m_type );
  if ( i < 0 )
  {
    i = m_textures.Count();
    m_textures.Append(tx);
  }
  else
  {
    m_textures[i] = tx;
  }
  if ( MYON_UuidIsNil(m_textures[i].m_texture_id) )
  {
    MYON_CreateUuid(m_textures[i].m_texture_id);
  }

  return i;
}


int MYON_Material::AddTexture(const wchar_t* filename,MYON_Texture::TYPE type)
{
  int ti = FindTexture(nullptr,type);
  if ( ti < 0 )
  {
    ti = m_textures.Count();
    m_textures.AppendNew();
  }
  if (ti >= 0 )
  {
    m_textures[ti].m_image_file_reference.SetFullPath(filename,false);
    m_textures[ti].m_type = type;
    m_textures[ti].m_mode = MYON_Texture::MODE::modulate_texture;
    m_textures[ti].m_magfilter = MYON_Texture::FILTER::linear_filter;
    MYON_CreateUuid(m_textures[ti].m_texture_id);
  }
  return ti;
}

// Shine values are in range 0.0 to MYON_Material::GetMaxShine()
double MYON_Material::Shine() const
{
  return m_shine;
}

void MYON_Material::SetShine( double shine )
{
  if (shine == shine) // NANs fail this test
  {
    if (shine < 0.0)
      m_shine = 0.0;
    else if (shine > MYON_Material::MaxShine)
      m_shine = MYON_Material::MaxShine;
    else
      m_shine = (float)shine;
  }
}

  // Transparency values are in range 0.0 = opaque to 1.0 = transparent
double MYON_Material::Transparency( ) const
{
  return  m_transparency;
}

void MYON_Material::SetTransparency( double transparency )
{
  if ( transparency < 0.0 )
    m_transparency = 0.0;
  else if ( transparency > 1.0)
    m_transparency = 1.0;
  else
    m_transparency = transparency;
}

  // Transparency values are in range 0.0 = opaque to 1.0 = transparent
double MYON_Material::Reflectivity( ) const
{
  return  m_reflectivity;
}

void MYON_Material::SetReflectivity( double reflectivity )
{
  if ( reflectivity < 0.0 )
    m_reflectivity = 0.0;
  else if ( reflectivity > 1.0)
    m_reflectivity = 1.0;
  else
    m_reflectivity = reflectivity;
}

const MYON_UUID MYON_Material::MaterialChannelIdFromIndex(
  int material_channel_index
) const
{
  for (;;)
  {
    if (material_channel_index <= 0)
      break;
    const int count = m_material_channel.Count();
    if (count <= 0)
      break;
    const MYON_UuidIndex* a = m_material_channel.Array();
    for ( const MYON_UuidIndex* a1 = a + count; a < a1; ++a )
    {
      if (material_channel_index == a->m_i)
        return a->m_id;
    }
    break;
  }

  return MYON_nil_uuid;
}

int MYON_Material::MaterialChannelIndexFromId(
  MYON_UUID material_channel_id
) const
{
  for (;;)
  {
    if (MYON_nil_uuid == material_channel_id)
      break;
    const unsigned count = m_material_channel.UnsignedCount();
    if (0 == count)
      break;
    const MYON_UuidIndex* a = m_material_channel.Array();
    for ( const MYON_UuidIndex* a1 = a + count; a < a1; ++a)
    {
      if (material_channel_id == a->m_id)
        return a->m_i;
    }
    break;
  }

  return 0;
}

int  MYON_Material::MaterialChannelIndexFromId(
  MYON_UUID material_channel_id,
  bool bAddIdIfNotPresent
)
{
  for (;;)
  {
    if (MYON_nil_uuid == material_channel_id)
      break;
    int unused_index = 0;

    const int count = m_material_channel.Count();
    if (count > 0)
    {
      const MYON_UuidIndex* a = m_material_channel.Array();
      for (const MYON_UuidIndex* a1 = a + count; a < a1; ++a)
      {
        if (material_channel_id == a->m_id)
          return a->m_i;
        if (a->m_i > unused_index)
          unused_index = a->m_i;
      }
    }

    if (false == bAddIdIfNotPresent)
      break;
    if (count >= MYON_Material::MaximumMaterialChannelIndex)
      break; // some rogue actor filled the m_material_channel[] array.

    ++unused_index;
    if ( unused_index <= 0 || unused_index > MYON_Material::MaximumMaterialChannelIndex)
    {
      // int overflow or too big for a material channel index
      for (unused_index = 1; unused_index <= count + 1; ++unused_index)
      {
        if (MYON_nil_uuid == MaterialChannelIdFromIndex(unused_index))
          break;
      }
    }
    const MYON_UuidIndex ui(material_channel_id, unused_index);
    m_material_channel.Append(ui);
    return ui.m_i;
  }

  return 0;
}



bool operator==( const MYON_Material& a, const MYON_Material& b )
{
  return (0 == MYON_Material::Compare(a,b));
}

bool operator!=( const MYON_Material& a, const MYON_Material& b )
{
  return (0 != MYON_Material::Compare(a,b));
}

static int CompareNans(double a, double b)
{
  if (a == a)
  {
    if (b == b)
    {
      return ( ( a < b ) ? -1 : ((a > b) ? 1 : 0 ) );
    }
    // a is not a NAN, b is a NAN
    return -1; // a < b - NAN's are last
  }
  else if (b == b)
  {
    // a is a NAN, b is not a NAN
    return -1; // b < a - NAN's are last
  }
  return 0; // a and b are both NaNs
}

static int CompareDouble( double a, double b )
{
  return ( ( a < b ) ? -1 : ((a > b) ? 1 : (a==b ? 0 : CompareNans(a,b)) ) );
}

static int CompareXform( const MYON_Xform& a, const MYON_Xform& b )
{
  int i,j;
  const double* da = &a.m_xform[0][0];
  const double* db = &b.m_xform[0][0];
  i = 16;
  j = 0;
  while ( i-- && !j)
  {
    j = CompareDouble(*da++,*db++);
  }

  return j;
}

int MYON_Texture::Compare(const MYON_Texture& a, const MYON_Texture& b)
{
  int rc = MYON_UuidCompare(&a.m_texture_id, &b.m_texture_id);
  while (!rc)
  {
    if (a.m_mapping_channel_id < b.m_mapping_channel_id)
      rc = -1;
    else if (a.m_mapping_channel_id > b.m_mapping_channel_id)
      rc = 1;
    if (rc) break;

    rc = a.m_image_file_reference.FullPath().ComparePath(static_cast< const wchar_t* >(b.m_image_file_reference.FullPath()));
    if (rc) break;

    rc = ((int)(a.m_bOn ? 1 : 0)) - ((int)(b.m_bOn ? 1 : 0));
    if (rc) break;

    rc = ((int)a.m_type) - ((int)b.m_type);
    if (rc) break;

    rc = ((int)a.m_mode) - ((int)b.m_mode);
    if (rc) break;

    rc = ((int)a.m_minfilter) - ((int)b.m_minfilter);
    if (rc) break;

    rc = ((int)a.m_magfilter) - ((int)b.m_magfilter);
    if (rc) break;

    rc = ((int)a.m_wrapu) - ((int)b.m_wrapu);
    if (rc) break;

    rc = ((int)a.m_wrapv) - ((int)b.m_wrapv);
    if (rc) break;

    rc = ((int)a.m_wrapw) - ((int)b.m_wrapw);
    if (rc) break;

    rc = CompareXform(a.m_uvw, b.m_uvw);
    if (rc) break;

    rc = a.m_border_color.Compare(b.m_border_color);
    if (rc) break;

    rc = a.m_transparent_color.Compare(b.m_transparent_color);
    if (rc) break;

    rc = MYON_Interval::Compare(a.m_bump_scale, b.m_bump_scale);
    if (rc) break;

    rc = CompareDouble(a.m_blend_constant_A, b.m_blend_constant_A);
    if (rc) break;

    rc = CompareDouble(a.m_blend_A0, b.m_blend_A0);
    if (rc) break;
    rc = CompareDouble(a.m_blend_A1, b.m_blend_A1);
    if (rc) break;
    rc = CompareDouble(a.m_blend_A2, b.m_blend_A2);
    if (rc) break;
    rc = CompareDouble(a.m_blend_A3, b.m_blend_A3);
    if (rc) break;

    rc = CompareDouble(a.m_blend_RGB0, b.m_blend_RGB0);
    if (rc) break;
    rc = CompareDouble(a.m_blend_RGB1, b.m_blend_RGB1);
    if (rc) break;
    rc = CompareDouble(a.m_blend_RGB2, b.m_blend_RGB2);
    if (rc) break;
    rc = CompareDouble(a.m_blend_RGB3, b.m_blend_RGB3);
    if (rc) break;

    rc = ((int)(a.m_bTreatAsLinear ? 1 : 0)) - ((int)(b.m_bTreatAsLinear ? 1 : 0));
    if (rc) break;

    break;
  }

  return rc;
}

int MYON_Texture::CompareAppearance(const MYON_Texture& a, const MYON_Texture& b)
{
  int rc = 0;
  while (!rc)
  {
    if (a.m_mapping_channel_id < b.m_mapping_channel_id)
      rc = -1;
    else if (a.m_mapping_channel_id > b.m_mapping_channel_id)
      rc = 1;
    if (rc) break;

    rc = a.m_image_file_reference.FullPath().ComparePath(static_cast< const wchar_t* >(b.m_image_file_reference.FullPath()));
    if (rc) break;

    rc = ((int)(a.m_bOn ? 1 : 0)) - ((int)(b.m_bOn ? 1 : 0));
    if (rc) break;

    rc = ((int)a.m_type) - ((int)b.m_type);
    if (rc) break;

    rc = ((int)a.m_mode) - ((int)b.m_mode);
    if (rc) break;

    rc = ((int)a.m_minfilter) - ((int)b.m_minfilter);
    if (rc) break;

    rc = ((int)a.m_magfilter) - ((int)b.m_magfilter);
    if (rc) break;

    rc = ((int)a.m_wrapu) - ((int)b.m_wrapu);
    if (rc) break;

    rc = ((int)a.m_wrapv) - ((int)b.m_wrapv);
    if (rc) break;

    rc = ((int)a.m_wrapw) - ((int)b.m_wrapw);
    if (rc) break;

    rc = CompareXform(a.m_uvw, b.m_uvw);
    if (rc) break;

    rc = a.m_border_color.Compare(b.m_border_color);
    if (rc) break;

    rc = a.m_transparent_color.Compare(b.m_transparent_color);
    if (rc) break;

    rc = MYON_Interval::Compare(a.m_bump_scale, b.m_bump_scale);
    if (rc) break;

    rc = CompareDouble(a.m_blend_constant_A, b.m_blend_constant_A);
    if (rc) break;

    rc = CompareDouble(a.m_blend_A0, b.m_blend_A0);
    if (rc) break;
    rc = CompareDouble(a.m_blend_A1, b.m_blend_A1);
    if (rc) break;
    rc = CompareDouble(a.m_blend_A2, b.m_blend_A2);
    if (rc) break;
    rc = CompareDouble(a.m_blend_A3, b.m_blend_A3);
    if (rc) break;

    rc = CompareDouble(a.m_blend_RGB0, b.m_blend_RGB0);
    if (rc) break;
    rc = CompareDouble(a.m_blend_RGB1, b.m_blend_RGB1);
    if (rc) break;
    rc = CompareDouble(a.m_blend_RGB2, b.m_blend_RGB2);
    if (rc) break;
    rc = CompareDouble(a.m_blend_RGB3, b.m_blend_RGB3);
    if (rc) break;

    rc = ((int)(a.m_bTreatAsLinear ? 1 : 0)) - ((int)(b.m_bTreatAsLinear ? 1 : 0));
    if (rc) break;

    break;
  }

  return rc;
}

int MYON_Material::Compare( const MYON_Material& a, const MYON_Material& b )
{
  int rc = CompareNameAndIds(a,b);
  if (0 == rc)
    rc = CompareAppearance(a,b);
  return rc;
}

int MYON_Material::CompareNameAndIds( const MYON_Material& a, const MYON_Material& b )
{
  // do NOT test index or id

  const MYON_UUID aid = a.Id();
  const MYON_UUID bid = b.Id();
  int rc = MYON_UuidCompare( &aid, &bid );
  if (rc) return rc;

  rc = a.Name().CompareOrdinal( static_cast< const wchar_t* >(b.Name()), false );
  if (rc) return rc;

  rc = MYON_UuidCompare(&a.m_rdk_material_instance_id, &b.m_rdk_material_instance_id);
  if (rc) return rc;

  const int tcount = a.m_textures.Count();
  rc = tcount - b.m_textures.Count();
  for (int i = 0; i < tcount && 0 == rc; i++)
  {
    rc = MYON_UuidCompare(a.m_textures[i].m_texture_id, b.m_textures[i].m_texture_id);
  }

  return rc;
}


int MYON_Material::CompareColorAttributes( const MYON_Material& a, const MYON_Material& b )
{
  const auto a_pbr = a.PhysicallyBased();
  const auto b_pbr = b.PhysicallyBased();
  if (a_pbr && !b_pbr)
    return -1;
  if (!a_pbr && b_pbr)
    return 1;

  if (a_pbr && b_pbr)
  {
    int rc = a_pbr->BaseColor().Compare(a_pbr->BaseColor());
    if (rc) return rc;

    rc = ((int)a_pbr->BRDF()) - ((int)b_pbr->BRDF());
    if (rc) return rc;

    rc = CompareDouble(a_pbr->Subsurface(), b_pbr->Subsurface());
    if (0 != rc) return rc;

    rc = a_pbr->SubsurfaceScatteringColor().Compare(b_pbr->SubsurfaceScatteringColor());
    if (0 != rc) return rc;

    rc = CompareDouble(a_pbr->SubsurfaceScatteringRadius(), b_pbr->SubsurfaceScatteringRadius());
    if (0 != rc) return rc;
    
    rc = CompareDouble(a_pbr->Metallic(), b_pbr->Metallic());
    if (0 != rc) return rc;

    rc = CompareDouble(a_pbr->Specular(), b_pbr->Specular());
    if (0 != rc) return rc;

    rc = CompareDouble(a_pbr->SpecularTint(), b_pbr->SpecularTint());
    if (0 != rc) return rc;

    rc = CompareDouble(a_pbr->Roughness(), b_pbr->Roughness());
    if (0 != rc) return rc;

    rc = CompareDouble(a_pbr->Sheen(), b_pbr->Sheen());
    if (0 != rc) return rc;

    rc = CompareDouble(a_pbr->SheenTint(), b_pbr->SheenTint());
    if (0 != rc) return rc;

    rc = CompareDouble(a_pbr->Anisotropic(), b_pbr->Anisotropic());
    if (0 != rc) return rc;

    rc = CompareDouble(a_pbr->AnisotropicRotation(), b_pbr->AnisotropicRotation());
    if (0 != rc) return rc;

    rc = CompareDouble(a_pbr->Clearcoat(), b_pbr->Clearcoat());
    if (0 != rc) return rc;

    rc = CompareDouble(a_pbr->ClearcoatRoughness(), b_pbr->ClearcoatRoughness());
    if (0 != rc) return rc;

    rc = CompareDouble(a_pbr->Opacity(), b_pbr->Opacity());
    if (0 != rc) return rc;

    rc = CompareDouble(a_pbr->OpacityIOR(), b_pbr->OpacityIOR());
    if (0 != rc) return rc;

    rc = CompareDouble(a_pbr->OpacityRoughness(), b_pbr->OpacityRoughness());
    if (0 != rc) return rc;

    rc = CompareDouble(a_pbr->Alpha(), b_pbr->Alpha());
    if (0 != rc) return rc;

    rc = a_pbr->Emission().Compare(b_pbr->Emission());
    return rc;
  }

  int rc = a.m_ambient.Compare(b.m_ambient);
  if (rc) return rc;

  rc = a.m_diffuse.Compare( b.m_diffuse );
  if (rc) return rc;

  rc = a.m_emission.Compare( b.m_emission );
  if (rc) return rc;

  rc = a.m_specular.Compare( b.m_specular );
  if (rc) return rc;

  rc = a.m_reflection.Compare( b.m_reflection );
  if (rc) return rc;

  rc = a.m_transparent.Compare( b.m_transparent );
  if (rc) return rc;

  rc = CompareDouble(a.m_transparency,b.m_transparency);
  if (rc) return rc;

  rc = ((int)a.m_bDisableLighting) - ((int)b.m_bDisableLighting);

  return rc;
}


int MYON_Material::CompareReflectionAttributes(const MYON_Material& a, const MYON_Material& b)
{
  const auto a_pbr = a.PhysicallyBased();
	const auto b_pbr = b.PhysicallyBased();
	if (a_pbr && !b_pbr)
		return -1;
	if (!a_pbr && b_pbr)
		return 1;

	if (a_pbr && b_pbr)
	{
    int rc = CompareDouble(a_pbr->Metallic(), b_pbr->Metallic());
    if (0 != rc) return rc;

    rc = CompareDouble(a_pbr->Specular(), b_pbr->Specular());
    if (0 != rc) return rc;

    rc = CompareDouble(a_pbr->SpecularTint(), b_pbr->SpecularTint());
    if (0 != rc) return rc;

    rc = CompareDouble(a_pbr->Roughness(), b_pbr->Roughness());
    if (0 != rc) return rc;

    rc = CompareDouble(a_pbr->Anisotropic(), b_pbr->Anisotropic());
    if (0 != rc) return rc;

    rc = CompareDouble(a_pbr->AnisotropicRotation(), b_pbr->AnisotropicRotation());
    if (0 != rc) return rc;

    rc = CompareDouble(a_pbr->Clearcoat(), b_pbr->Clearcoat());
    if (0 != rc) return rc;

    rc = CompareDouble(a_pbr->ClearcoatRoughness(), b_pbr->ClearcoatRoughness());
    
    return rc;
	}

	int rc = a.m_reflection.Compare(b.m_reflection);
	if (0 != rc) return rc;

	rc = CompareDouble(a.m_index_of_refraction, b.m_index_of_refraction);
	if (0 != rc) return rc;

	rc = CompareDouble(a.m_reflectivity, b.m_reflectivity);
	if (0 != rc) return rc;

	rc = CompareDouble(a.m_shine, b.m_shine);
	if (0 != rc) return rc;

	rc = (a.m_bFresnelReflections ? 1 : 0) - (b.m_bFresnelReflections ? 1 : 0);
	if (0 != rc) return rc;
	if (a.m_bFresnelReflections)
	{
		rc = CompareDouble(a.m_fresnel_index_of_refraction, b.m_fresnel_index_of_refraction);
		if (0 != rc) return rc;
	}

	rc = CompareDouble(a.m_reflection_glossiness, b.m_reflection_glossiness);
	if (0 != rc) return rc;

	rc = CompareDouble(a.m_refraction_glossiness, b.m_refraction_glossiness);

	return rc;
}

int MYON_Material::CompareTextureAttributes(const MYON_Material& a, const MYON_Material& b)
{
  // do NOT test index or id

  const int tcount = a.m_textures.Count();
  int rc = tcount - b.m_textures.Count();
  for (int i = 0; i < tcount && 0 == rc; i++)
  {
    rc = MYON_Texture::Compare(a.m_textures[i], b.m_textures[i]);
  }
  if (0 == rc)
    rc = ((int)a.m_bUseDiffuseTextureAlphaForObjectTransparencyTexture) - ((int)b.m_bUseDiffuseTextureAlphaForObjectTransparencyTexture);


  return rc;
}

int MYON_Material::CompareTextureAttributesAppearance(const MYON_Material& a, const MYON_Material& b)
{
  // do NOT test index or id

  const int tcount = a.m_textures.Count();
  int rc = tcount - b.m_textures.Count();
  for (int i = 0; i < tcount && 0 == rc; i++)
  {
    rc = MYON_Texture::CompareAppearance(a.m_textures[i], b.m_textures[i]);
  }
  if (0 == rc)
    rc = ((int)a.m_bUseDiffuseTextureAlphaForObjectTransparencyTexture) - ((int)b.m_bUseDiffuseTextureAlphaForObjectTransparencyTexture);


  return rc;
}

int MYON_Material::CompareAppearance( const MYON_Material& a, const MYON_Material& b )
{
  int rc = CompareColorAttributes(a,b);
  if ( 0 == rc )
    rc = CompareReflectionAttributes(a,b);
  if ( 0 == rc )
    rc = CompareTextureAttributesAppearance(a,b);

  // This was added because the plugin uuid for a material gets changed to
  // universal uuid by RhRdkCreateImportedMaterial.  In the case of import_vrml
  // that happens just before it's added to the material table.  That breaks the
  // comparison unless you know that and change your plugin id before checking
  // to see if the material exists.  This is a change that Dale L did for me to work
  // around that.
  if (0 == rc)
  {
    static const MYON_UUID uuidUniversal =
    {
      // {99999999-9999-9999-9999-999999999999}
      0x99999999, 0x9999, 0x9999, { 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99 }
    };
    if (
      0 != MYON_UuidCompare(&uuidUniversal, &a.m_plugin_id)
      &&
      0 != MYON_UuidCompare(&uuidUniversal, &b.m_plugin_id)
      )
    {
      rc = MYON_UuidCompare(&a.m_plugin_id, &b.m_plugin_id);
    }
  }

  return rc;
}




MYON_UUID MYON_Material::RdkMaterialInstanceId() const
{
  return m_rdk_material_instance_id;
}

/*
Description:
Set this material's RDK material id.
Parameters:
rdk_material_id - [in]
RDK material id value.
Remarks:
The RDK material id identifies a material definition managed by
the RDK (rendering development kit).  Multiple materials in
a Rhino or opennurbs model can reference the same RDK material.
*/
void MYON_Material::SetRdkMaterialInstanceId(
  MYON_UUID rdk_material_instance_id
  )
{
  m_rdk_material_instance_id = rdk_material_instance_id;
}

bool MYON_Material::RdkMaterialInstanceIdIsNotNil() const
{
  return (!(MYON_nil_uuid == m_rdk_material_instance_id));
}

bool MYON_Material::RdkMaterialInstanceIdIsNil() const
{
  return (MYON_nil_uuid == m_rdk_material_instance_id);
}

// NO MYON_OBJECT_IMPLEMENT(MYON_RdkMaterialInstanceIdObsoleteUserData, MYON_ObsoleteUserData, ...)
// because MYON_RdkMaterialInstanceIdObsoleteUserData is derived from MYON_ObsoleteUserData.

// MYON_RdkUserData class id value
//  = MYON_RdkMaterialInstanceIdObsoleteUserData::ClassId()->Uuid()
//  = AFA82772-1525-43dd-A63C-C84AC5806911
// From MYON_RdkUserData MYON_OBJECT_IMPLEMENT macro
//   file: src4\opennurbs\opennurbs_xml.cpp
//
// {AFA82772-1525-43dd-A63C-C84AC5806911}
const MYON_UUID MYON_RdkMaterialInstanceIdObsoleteUserData::m_archive_class_id_ctor =
{ 0xAFA82772, 0x1525, 0x43dd, { 0xA6, 0x3C, 0xC8, 0x4A, 0xC5, 0x80, 0x69, 0x11 } };

// MYON_RdkUserData::m_userdata_uuid value
//  = MYON_RdkUserData::Uuid()
//  = MYON_RdkUserData::m_Uuid
//  = B63ED079-CF67-416c-800D-22023AE1BE21
// From
//   file: src4\opennurbs\opennurbs_xml.cpp
//
// {B63ED079-CF67-416c-800D-22023AE1BE21}
const MYON_UUID MYON_RdkMaterialInstanceIdObsoleteUserData::m_archive_userdata_uuid_ctor =
{ 0xb63ed079, 0xcf67, 0x416c, { 0x80, 0xd, 0x22, 0x2, 0x3a, 0xe1, 0xbe, 0x21 } };

// MYON_RdkUserData::m_application_uuid value
//  = CRhRdkRhinoPlugIn::RhinoPlugInUuid()
//  = CRhRdkRhinoPlugIn::m_uuidRhinoPlugIn
//  = 16592D58-4A2F-401D-BF5E-3B87741C1B1B
// From
//  file: src4/rhino4/Plug-ins/RDK/RDK/RhRcmRhinoPlugIn.cpp
//
// {16592D58-4A2F-401D-BF5E-3B87741C1B1B}
const MYON_UUID MYON_RdkMaterialInstanceIdObsoleteUserData::m_archive_application_uuid_ctor =
{ 0x16592D58, 0x4A2F, 0x401D, { 0xBF, 0x5E, 0x3B, 0x87, 0x74, 0x1C, 0x1B, 0x1B } };

const unsigned int MYON_RdkMaterialInstanceIdObsoleteUserData::m_userdata_copycount_ctor = 1;

const MYON_Xform MYON_RdkMaterialInstanceIdObsoleteUserData::m_userdata_xform_ctor(MYON_Xform::IdentityTransformation);

MYON_RdkMaterialInstanceIdObsoleteUserData::MYON_RdkMaterialInstanceIdObsoleteUserData()
: m_rdk_material_instance_id(MYON_nil_uuid)
{
  ///////////////////////////////////////////////////////////
  // MYON_UserData field initialization:

  // The values of m_userdata_uuid and m_application_uuid match those used by MYON_RdkUserData.
  m_userdata_uuid = MYON_RdkMaterialInstanceIdObsoleteUserData::m_archive_userdata_uuid_ctor;
  m_application_uuid = MYON_RdkMaterialInstanceIdObsoleteUserData::m_archive_application_uuid_ctor;

  // Values for the 3dm archive.
  m_userdata_copycount = MYON_RdkMaterialInstanceIdObsoleteUserData::m_userdata_copycount_ctor;
  m_userdata_xform = MYON_RdkMaterialInstanceIdObsoleteUserData::m_userdata_xform_ctor;

  ///////////////////////////////////////////////////////////
  // MYON_ObsoleteUserData field initialization:
  m_archive_class_uuid = MYON_RdkMaterialInstanceIdObsoleteUserData::m_archive_class_id_ctor;
}

MYON_RdkMaterialInstanceIdObsoleteUserData::~MYON_RdkMaterialInstanceIdObsoleteUserData()
{}


MYON_RdkMaterialInstanceIdObsoleteUserData::MYON_RdkMaterialInstanceIdObsoleteUserData(const MYON_RdkMaterialInstanceIdObsoleteUserData& src)
: MYON_ObsoleteUserData(src)
, m_rdk_material_instance_id(src.m_rdk_material_instance_id)
{}

MYON_RdkMaterialInstanceIdObsoleteUserData& MYON_RdkMaterialInstanceIdObsoleteUserData::operator=(const MYON_RdkMaterialInstanceIdObsoleteUserData& src)
{
  if (this != &src)
  {
    MYON_ObsoleteUserData::operator=(src);
    m_rdk_material_instance_id = src.m_rdk_material_instance_id;
  }
  return *this;
}

bool MYON_RdkMaterialInstanceIdObsoleteUserData::GetDescription(MYON_wString& description)
{
  description = "RDK material instance id as user data for writing pre V6 file formats.";
  return true;
}

bool MYON_RdkMaterialInstanceIdObsoleteUserData::WriteToArchive(
  const class MYON_BinaryArchive& archive,
  const class MYON_Object* parent_object
  ) const
{
  return (MYON_UuidIsNotNil(m_rdk_material_instance_id) && archive.Archive3dmVersion() <= 50);
}

bool MYON_RdkMaterialInstanceIdObsoleteUserData::DeleteAfterWrite(
  const MYON_BinaryArchive& archive,
  const MYON_Object* parent_object
  ) const
{
  // Returning true will cause the MYON_BinaryArchive::WriteObject()
  // plumbing to delete this user data.
  return true;
}

bool MYON_RdkMaterialInstanceIdObsoleteUserData::DeleteAfterRead(
  const MYON_BinaryArchive& archive,
  MYON_Object* parent_object
  ) const
{
  // move the RDK material id value from the V5 user data to
  // the V6 MYON_Material.m_rdk_material_instance_id field.
  MYON_Material* mat = MYON_Material::Cast(parent_object);
  if (mat && mat->RdkMaterialInstanceIdIsNil())
    mat->SetRdkMaterialInstanceId(m_rdk_material_instance_id);

  // Returning true will cause the MYON_BinaryArchive::ReadObject()
  // plumbing to delete this user data.
  return true;
}

bool MYON_RdkMaterialInstanceIdObsoleteUserData::IsRdkMaterialInstanceIdUserData(
  MYON_UUID class_id,
  MYON_UUID userdata_id,
  MYON_UUID app_id,
  MYON_Object* object
  )
{
  return (
    nullptr != MYON_Material::Cast(object)
    && class_id == MYON_RdkMaterialInstanceIdObsoleteUserData::m_archive_class_id_ctor
    && userdata_id == MYON_RdkMaterialInstanceIdObsoleteUserData::m_archive_userdata_uuid_ctor
    && app_id == MYON_RdkMaterialInstanceIdObsoleteUserData::m_archive_application_uuid_ctor
    );
}

static const char* ParsePastWhiteSpace(const char* s)
{
  for (char c = s ? *s++ : 0; 0 != c; c = *s++)
  {
    if (32 == c)
      continue;
    if (c >= 9 && c <= 13)
      continue;
    break;
  }
  return s;
}

static char ToUpper(char c)
{
  if (c >= 'a' && c <= 'z')
    c -= 0x20;
  return c;
}

static const char* ParsePast(const char* token, const char* s)
{
  if (0 == token || token[0] <= 32)
    return 0;

  s = ParsePastWhiteSpace(s);
  if (0 == s || s[0] <= 32)
    return 0;

  for (/*empty init*/; ToUpper(*s) == ToUpper(*token); s++, token++)
  {
    if (0 == *s)
      return s;
  }
  return 0;
}

bool MYON_RdkMaterialInstanceIdObsoleteUserData::Read(
  MYON_BinaryArchive& archive
  )
{
  m_rdk_material_instance_id = MYON_nil_uuid;

  bool rc = false;
  for (;;)
  {
    int version = 0;
    if (!archive.ReadInt(&version))
      break;
    if (2 != version)
      break;

    int s_length = 0;
    if (!archive.ReadInt(&s_length))
      break;
    if (s_length < 0 || s_length > 1024)
      break;
    if (0 == s_length)
    {
      rc = true;
      break;
    }

    MYON_String str((char)0, (int)s_length);
    if (str.Length() < s_length)
      break;
    const char* s = str.Array();
    if (0 == s)
      break;

    if (!archive.ReadByte(s_length, (void*)s))
      break;

    // parse the xml the CRhRdkUserData::Write() function
    // put into V5 files and V6 files written before May 16, 2014.

    s = ParsePast("<", s);
    s = ParsePast("xml", s);
    s = ParsePast(">", s);

    s = ParsePast("<", s);
    s = ParsePast("render-content-manager-data", s);
    s = ParsePast(">", s);

    s = ParsePast("<", s);
    s = ParsePast("material", s);
    s = ParsePast("instance-id", s);
    s = ParsePast("=", s);

    s = ParsePast("\"", s);
    s = ParsePastWhiteSpace(s);
    s = MYON_ParseUuidString(s, &m_rdk_material_instance_id);
    s = ParsePast("\"", s);

    rc = (0 != s);
    break;
  }

  return rc;
}

bool MYON_RdkMaterialInstanceIdObsoleteUserData::Write(
  MYON_BinaryArchive& archive
  ) const
{
  // write xml the CRhRdkUserData::Write() function
  // put into V5 files and V6 files written before May 16, 2014.

  char s__rdk_material_instance_id[37];
  MYON_String s =
    "<xml>\n"
    "<render-content-manager-data>\n"
    "<material instance-id=\"";
   s += MYON_UuidToString(m_rdk_material_instance_id, s__rdk_material_instance_id);
   s +=
    "\" name=\"\" />\n"
    "</render-content-manager-data>\n"
    "</xml>";
   const int s_length = (int)s.Length();
   const char* s_array = s.Array();
   bool rc
     =  archive.WriteInt(2) // "2" was a version number used in V5 files
     && archive.WriteInt(s_length)
     && archive.WriteByte(s_length, s_array);
   return rc;
}


MYON_Color MYON_Material::Ambient() const
{
  return m_ambient & 0x00FFFFFF;
}

MYON_Color MYON_Material::Diffuse( ) const
{
  return m_diffuse & 0x00FFFFFF;
}

MYON_Color MYON_Material::Emission( ) const
{
  return m_emission & 0x00FFFFFF;
}

MYON_Color MYON_Material::Specular() const
{
  return m_specular & 0x00FFFFFF;
}

void MYON_Material::SetAmbient( MYON_Color  c )
{
  if (m_ambient != c)
  {
    m_ambient = c;
    IncrementContentVersionNumber();
  }
}

void MYON_Material::SetDiffuse(  MYON_Color c )
{
  if (m_diffuse != c)
  {
    m_diffuse = c;
    IncrementContentVersionNumber();
  }
}

void MYON_Material::SetEmission( MYON_Color c )
{
  if (m_emission != c)
  {
    m_emission = c;
    IncrementContentVersionNumber();
  }
}

void MYON_Material::SetSpecular( MYON_Color c )
{
  if (m_specular != c)
  {
    m_specular = c;
    IncrementContentVersionNumber();
  }
}

bool MYON_Material::Shareable() const
{
  return m_bShareable;
}

void MYON_Material::SetShareable(
  bool bShareable
  )
{
  if (bShareable != m_bShareable)
  {
    m_bShareable = bShareable?true:false;
    IncrementContentVersionNumber();
  }
}


bool MYON_Material::DisableLighting() const
{
  return m_bDisableLighting;
}

void MYON_Material::SetDisableLighting(
  bool bDisableLighting
  )
{
  if (bDisableLighting != m_bDisableLighting)
  {
    m_bDisableLighting = bDisableLighting?true:false;
    IncrementContentVersionNumber();
  }
}

//Very simple preview color function that better supports metals and dialectrics.
MYON_Color MYON_Material::PreviewColor(void) const
{
	if (Transparency() > 0.5)
	{
		return m_transparent;
	}

	if (!FresnelReflections())
	{
		if (Reflectivity() > 0.5)
		{
			return m_reflection;
		}
	}

	return Diffuse( );
}

bool MYON_Material::UseDiffuseTextureAlphaForObjectTransparencyTexture() const
{
  return m_bUseDiffuseTextureAlphaForObjectTransparencyTexture;
}

void MYON_Material::SetUseDiffuseTextureAlphaForObjectTransparencyTexture(
  bool bUseDiffuseTextureAlphaForObjectTransparencyTexture
  )
{
  if (bUseDiffuseTextureAlphaForObjectTransparencyTexture != m_bUseDiffuseTextureAlphaForObjectTransparencyTexture)
  {
    m_bUseDiffuseTextureAlphaForObjectTransparencyTexture = bUseDiffuseTextureAlphaForObjectTransparencyTexture?true:false;
    IncrementContentVersionNumber();
  }
}

bool MYON_Material::FresnelReflections() const
{
  return m_bFresnelReflections;
}

void MYON_Material::SetFresnelReflections(
  bool bFresnelReflections
  )
{
  if (bFresnelReflections != m_bFresnelReflections)
  {
    m_bFresnelReflections = bFresnelReflections?true:false;
    IncrementContentVersionNumber();
  }
}

////////////////////////////////////////////////////////////////
//   Class MYON_Texture
////////////////////////////////////////////////////////////////

MYON_OBJECT_IMPLEMENT(MYON_Texture,MYON_Object,"D6FF106D-329B-4f29-97E2-FD282A618020");

bool MYON_Texture::IsValid( MYON_TextLog* text_log ) const
{
  if ( MYON_Texture::TYPE::no_texture_type == m_type )
  {
    if ( text_log )
    {
      text_log->Print("MYON_Texture m_type has invalid value.\n");
    }
    return false;
  }

  // TODO ...

  return true;
}

// overrides virtual MYON_Object::Dump
void MYON_Texture::Dump( MYON_TextLog& ) const
{

}

// overrides virtual MYON_Object::SizeOf
unsigned int MYON_Texture::SizeOf() const
{
  unsigned int sz = MYON_Object::SizeOf();
  sz += sizeof(*this) - sizeof(MYON_Object);
  return sz;
}

// overrides virtual MYON_Object::Write
bool MYON_Texture::Write(
  MYON_BinaryArchive& binary_archive
  ) const
{
  const int major_version = 1;
  const int minor_version
    = binary_archive.Archive3dmVersion() >= 60
    ? (binary_archive.Archive3dmVersion() >= 70 ? 2 : 1)
    : 0;

  bool rc = binary_archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,major_version,minor_version);
  if (rc)
  {

    for(;;)
    {
      // 1.0 values
      rc = binary_archive.WriteUuid(m_texture_id);
      if (!rc) break;
      rc = binary_archive.WriteInt(m_mapping_channel_id);
      if (!rc) break;
      rc =
        m_image_file_reference.FullPath().IsNotEmpty()
        ? binary_archive.WriteString(m_image_file_reference.FullPath())
        : binary_archive.WriteString(m_image_file_reference.RelativePath());
      if (!rc) break;
      rc = binary_archive.WriteBool(m_bOn);
      if (!rc) break;
      rc = binary_archive.WriteInt(static_cast<unsigned int>(m_type));
      if (!rc) break;
      rc = binary_archive.WriteInt(static_cast<unsigned int>(m_mode));
      if (!rc) break;
      rc = binary_archive.WriteInt(static_cast<unsigned int>(m_minfilter));
      if (!rc) break;
      rc = binary_archive.WriteInt(static_cast<unsigned int>(m_magfilter));
      if (!rc) break;
      rc = binary_archive.WriteInt(static_cast<unsigned int>(m_wrapu));
      if (!rc) break;
      rc = binary_archive.WriteInt(static_cast<unsigned int>(m_wrapv));
      if (!rc) break;
      rc = binary_archive.WriteInt(static_cast<unsigned int>(m_wrapw));
      if (!rc) break;
      rc = binary_archive.WriteXform(m_uvw);
      if (!rc) break;
      rc = binary_archive.WriteColor(m_border_color);
      if (!rc) break;
      rc = binary_archive.WriteColor(m_transparent_color);
      if (!rc) break;
      rc = binary_archive.WriteUuid(m_transparency_texture_id);
      if (!rc) break;
      rc = binary_archive.WriteInterval(m_bump_scale);
      if (!rc) break;
      rc = binary_archive.WriteDouble(m_blend_constant_A);
      if (!rc) break;
      rc = binary_archive.WriteDouble(m_blend_A0);
      if (!rc) break;
      rc = binary_archive.WriteDouble(m_blend_A1);
      if (!rc) break;
      rc = binary_archive.WriteDouble(m_blend_A2);
      if (!rc) break;
      rc = binary_archive.WriteDouble(m_blend_A3);
      if (!rc) break;
      rc = binary_archive.WriteColor(m_blend_constant_RGB);
      if (!rc) break;
      rc = binary_archive.WriteDouble(m_blend_RGB0);
      if (!rc) break;
      rc = binary_archive.WriteDouble(m_blend_RGB1);
      if (!rc) break;
      rc = binary_archive.WriteDouble(m_blend_RGB2);
      if (!rc) break;
      rc = binary_archive.WriteDouble(m_blend_RGB3);
      if (!rc) break;
      rc = binary_archive.WriteInt(m_blend_order);
      if (!rc) break;

      if ( minor_version <= 0 )
        break;

      // version 1.1 added m_image_file_reference
      rc = m_image_file_reference.Write(true,binary_archive);
      if (!rc) break;

      if ( minor_version <= 1 )
        break;
      
      // version 1.2 added m_bTreatAsLinear
      rc = binary_archive.WriteBool(m_bTreatAsLinear);
      if (!rc) break;

      break;
    }


    if ( !binary_archive.EndWrite3dmChunk() )
      rc = false;
  }
  return rc;
}

static MYON_Texture::MAPPING_CHANNEL Internal_BuiltInMappingChannelFromUnsigned(
  unsigned int built_in_mapping_channel_as_unsigned,
  bool bEnableErrorMessage
  )
{
  switch (built_in_mapping_channel_as_unsigned)
  {
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::MAPPING_CHANNEL::tc_channel);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::MAPPING_CHANNEL::default_channel);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::MAPPING_CHANNEL::screen_based_channel);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::MAPPING_CHANNEL::wcs_channel);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::MAPPING_CHANNEL::wcs_box_channel);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::MAPPING_CHANNEL::environment_map_box_channel);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::MAPPING_CHANNEL::environment_map_light_probe_channel);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::MAPPING_CHANNEL::environment_map_spherical_channel);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::MAPPING_CHANNEL::environment_map_cube_map_channel);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::MAPPING_CHANNEL::environment_map_vcross_cube_map_channel);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::MAPPING_CHANNEL::environment_map_hcross_cube_map_channel);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::MAPPING_CHANNEL::environment_map_hemispherical_channel);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::MAPPING_CHANNEL::environment_map_emap_channel);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::MAPPING_CHANNEL::srfp_channel);
  //MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::MAPPING_CHANNEL::emap_channel);
  }

  if (bEnableErrorMessage)
  {
    MYON_ERROR("Invalid built_in_mapping_channel_as_unsigned value.");
  }
  return MYON_Texture::MAPPING_CHANNEL::tc_channel;
}

MYON_Texture::MAPPING_CHANNEL MYON_Texture::BuiltInMappingChannelFromUnsigned(
  unsigned int built_in_mapping_channel_as_unsigned
  )
{
  return Internal_BuiltInMappingChannelFromUnsigned(
    built_in_mapping_channel_as_unsigned,
    true
    );
}


const MYON_SHA1_Hash MYON_Texture::ContentHash() const
{
  MYON_SHA1 sha1;
  sha1.AccumulateUnsigned32(m_mapping_channel_id);
  sha1.AccumulateSubHash(m_image_file_reference.FullPathHash());
  sha1.AccumulateBool(m_bOn);
  sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_type));
  sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_mode));
  sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_minfilter));
  sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_magfilter));
  sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_wrapu));
  sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_wrapv));
  sha1.AccumulateUnsigned32(static_cast<unsigned int>(m_wrapw));
  sha1.AccumulateTransformation(m_uvw);
  sha1.AccumulateUnsigned32(m_border_color);
  sha1.AccumulateUnsigned32(m_transparent_color);
  sha1.AccumulateId(m_transparency_texture_id);
  sha1.AccumulateDouble(m_bump_scale.m_t[0]);
  sha1.AccumulateDouble(m_bump_scale.m_t[1]);
  sha1.AccumulateDouble(m_blend_constant_A);
  sha1.AccumulateDouble(m_blend_A0);
  sha1.AccumulateDouble(m_blend_A1);
  sha1.AccumulateDouble(m_blend_A2);
  sha1.AccumulateDouble(m_blend_A3);
  sha1.AccumulateDouble(m_blend_constant_RGB);
  sha1.AccumulateDouble(m_blend_RGB0);
  sha1.AccumulateDouble(m_blend_RGB1);
  sha1.AccumulateDouble(m_blend_RGB2);
  sha1.AccumulateDouble(m_blend_RGB3);
  sha1.AccumulateInteger32(m_blend_order);
  sha1.AccumulateBool(m_bTreatAsLinear);
  return sha1.Hash();
}


bool MYON_Texture::IsBuiltInMappingChannel(
  unsigned int mapping_channel_id
  )
{
  MYON_Texture::MAPPING_CHANNEL mc = Internal_BuiltInMappingChannelFromUnsigned(mapping_channel_id,false);
  return (static_cast<unsigned int>(mc) == mapping_channel_id);
}

void MYON_Texture::SetBuiltInMappingChannel(
  MYON_Texture::MAPPING_CHANNEL built_in_mapping_channel_as_unsigned
  )
{
  SetMappingChannel( static_cast<unsigned int>(built_in_mapping_channel_as_unsigned) );
}

void MYON_Texture::SetMappingChannel(
  unsigned int mapping_channel_id
  )
{
  m_mapping_channel_id = mapping_channel_id;
}

MYON_Texture::TYPE MYON_Texture::TypeFromUnsigned(unsigned int type_as_unsigned)
{
  switch (type_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::TYPE::no_texture_type);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::TYPE::diffuse_texture);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::TYPE::bump_texture);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::TYPE::emap_texture);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::TYPE::opacity_texture);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::TYPE::pbr_subsurface_texture);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::TYPE::pbr_subsurface_scattering_texture);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::TYPE::pbr_subsurface_scattering_radius_texture);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::TYPE::pbr_metallic_texture);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::TYPE::pbr_specular_texture);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::TYPE::pbr_specular_tint_texture);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::TYPE::pbr_roughness_texture);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::TYPE::pbr_anisotropic_texture);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::TYPE::pbr_anisotropic_rotation_texture);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::TYPE::pbr_sheen_texture);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::TYPE::pbr_sheen_tint_texture);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::TYPE::pbr_clearcoat_texture);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::TYPE::pbr_clearcoat_roughness_texture);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::TYPE::pbr_clearcoat_bump_texture);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::TYPE::pbr_opacity_ior_texture);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::TYPE::pbr_opacity_roughness_texture);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::TYPE::pbr_emission_texture);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::TYPE::pbr_ambient_occlusion_texture);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::TYPE::pbr_displacement_texture);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::TYPE::pbr_alpha_texture);
  }

  MYON_ERROR("Invalid type_as_unsigned value.");
  return MYON_Texture::TYPE::no_texture_type;
}

MYON_Texture::MODE MYON_Texture::ModeFromUnsigned( unsigned int mode_as_unsigned )
{
  switch(mode_as_unsigned)
  {
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::MODE::no_texture_mode);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::MODE::modulate_texture);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::MODE::decal_texture);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::MODE::blend_texture);
  }

  MYON_ERROR("Invalid mode_as_unsigned value.");
  return MYON_Texture::MODE::no_texture_mode;
}

MYON_Texture::FILTER MYON_Texture::FilterFromUnsigned( unsigned int filter_as_unsigned )
{
  switch(filter_as_unsigned)
  {
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::FILTER::nearest_filter);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::FILTER::linear_filter);
  }

  MYON_ERROR("Invalid filter_as_unsigned value.");
  return MYON_Texture::FILTER::linear_filter;
}

MYON_Texture::WRAP MYON_Texture::WrapFromUnsigned( unsigned int wrap_as_unsigned )
{

  switch(wrap_as_unsigned)
  {
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::WRAP::repeat_wrap);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Texture::WRAP::clamp_wrap);
  }

  MYON_ERROR("Invalid wrap_as_unsigned value.");
  return MYON_Texture::WRAP::repeat_wrap;
}

bool MYON_Texture::Read(
  MYON_BinaryArchive& binary_archive
  )
{
  *this = MYON_Texture::Default;

  int major_version = 0;
  int minor_version = 0;
  bool rc = binary_archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_version,&minor_version);
  if (rc)
  {

    if ( 1 != major_version )
    {
      rc = false;
    }
    else
    {
      unsigned int i;
      for(;;)
      {
        // 1.0 values
        rc = binary_archive.ReadUuid( m_texture_id );
        if (!rc) break;

        rc = binary_archive.ReadInt( &m_mapping_channel_id );
        if (!rc) break;

        MYON_wString filename;
        rc = binary_archive.ReadString(filename);
        if (!rc) break;
        if (MYON_FileSystemPath::IsRelativePath(filename))
          m_image_file_reference.SetRelativePath(filename);
        else
          m_image_file_reference.SetFullPath(filename,false);

        rc = binary_archive.ReadBool(&m_bOn);
        if (!rc) break;

        rc = binary_archive.ReadInt(&i);
        if (!rc) break;
        m_type = MYON_Texture::TypeFromUnsigned(i);

        rc = binary_archive.ReadInt(&i);
        if (!rc) break;
        m_mode = MYON_Texture::ModeFromUnsigned(i);

        rc = binary_archive.ReadInt(&i);
        if (!rc) break;
        m_minfilter = MYON_Texture::FilterFromUnsigned(i);

        rc = binary_archive.ReadInt(&i);
        if (!rc) break;
        m_magfilter = MYON_Texture::FilterFromUnsigned(i);

        rc = binary_archive.ReadInt(&i);
        if (!rc) break;
        m_wrapu = MYON_Texture::WrapFromUnsigned(i);

        rc = binary_archive.ReadInt(&i);
        if (!rc) break;
        m_wrapv = MYON_Texture::WrapFromUnsigned(i);

        rc = binary_archive.ReadInt(&i);
        if (!rc) break;
        m_wrapw = MYON_Texture::WrapFromUnsigned(i);

        rc = binary_archive.ReadXform(m_uvw);
        if (!rc) break;

        rc = binary_archive.ReadColor(m_border_color);
        if (!rc) break;

        rc = binary_archive.ReadColor(m_transparent_color);
        if (!rc) break;

        rc = binary_archive.ReadUuid(m_transparency_texture_id);
        if (!rc) break;

        rc = binary_archive.ReadInterval(m_bump_scale);
        if (!rc) break;

        rc = binary_archive.ReadDouble(&m_blend_constant_A);
        if (!rc) break;
        rc = binary_archive.ReadDouble(&m_blend_A0);
        if (!rc) break;
        rc = binary_archive.ReadDouble(&m_blend_A1);
        if (!rc) break;
        rc = binary_archive.ReadDouble(&m_blend_A2);
        if (!rc) break;
        rc = binary_archive.ReadDouble(&m_blend_A3);
        if (!rc) break;
        rc = binary_archive.ReadColor(m_blend_constant_RGB);
        if (!rc) break;
        rc = binary_archive.ReadDouble(&m_blend_RGB0);
        if (!rc) break;
        rc = binary_archive.ReadDouble(&m_blend_RGB1);
        if (!rc) break;
        rc = binary_archive.ReadDouble(&m_blend_RGB2);
        if (!rc) break;
        rc = binary_archive.ReadDouble(&m_blend_RGB3);
        if (!rc) break;

        rc = binary_archive.ReadInt(&m_blend_order);
        if (!rc) break;

        if ( minor_version <= 0 )
          break;

        rc = m_image_file_reference.Read(binary_archive);
        if (!rc) break;

        if (minor_version <= 1)
          break;

        rc = binary_archive.ReadBool(&m_bTreatAsLinear);
        if (!rc) break;

        break;
      }
    }

    if ( !binary_archive.EndRead3dmChunk() )
      rc = false;
  }
  return rc;
}


MYON_OBJECT_IMPLEMENT(MYON_TextureMapping,MYON_ModelComponent,"32EC997A-C3BF-4ae5-AB19-FD572B8AD554");


const MYON_TextureMapping* MYON_TextureMapping::FromModelComponentRef(
  const class MYON_ModelComponentReference& model_component_reference,
  const MYON_TextureMapping* none_return_value
  )
{
  const MYON_TextureMapping* p = MYON_TextureMapping::Cast(model_component_reference.ModelComponent());
  return (nullptr != p) ? p : none_return_value;
}

MYON_TextureMapping::TYPE MYON_TextureMapping::TypeFromUnsigned(
  unsigned int type_as_unsigned
  )
{
  switch (type_as_unsigned)
  {
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_TextureMapping::TYPE::no_mapping);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_TextureMapping::TYPE::srfp_mapping);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_TextureMapping::TYPE::plane_mapping);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_TextureMapping::TYPE::cylinder_mapping);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_TextureMapping::TYPE::sphere_mapping);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_TextureMapping::TYPE::box_mapping);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_TextureMapping::TYPE::mesh_mapping_primitive);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_TextureMapping::TYPE::srf_mapping_primitive);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_TextureMapping::TYPE::brep_mapping_primitive);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_TextureMapping::TYPE::ocs_mapping);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_TextureMapping::TYPE::false_colors);
  }

  MYON_ERROR("Invalid type_as_unsigned value.");
  return MYON_TextureMapping::TYPE::no_mapping;
}

const MYON_wString MYON_TextureMapping::TypeToString(MYON_TextureMapping::TYPE texture_mapping_type)
{
  switch (texture_mapping_type)
  {
  MYON_ENUM_TO_WIDE_STRING_CASE(MYON_TextureMapping::TYPE::no_mapping);
  MYON_ENUM_TO_WIDE_STRING_CASE(MYON_TextureMapping::TYPE::srfp_mapping);
  MYON_ENUM_TO_WIDE_STRING_CASE(MYON_TextureMapping::TYPE::plane_mapping);
  MYON_ENUM_TO_WIDE_STRING_CASE(MYON_TextureMapping::TYPE::cylinder_mapping);
  MYON_ENUM_TO_WIDE_STRING_CASE(MYON_TextureMapping::TYPE::sphere_mapping);
  MYON_ENUM_TO_WIDE_STRING_CASE(MYON_TextureMapping::TYPE::box_mapping);
  MYON_ENUM_TO_WIDE_STRING_CASE(MYON_TextureMapping::TYPE::mesh_mapping_primitive);
  MYON_ENUM_TO_WIDE_STRING_CASE(MYON_TextureMapping::TYPE::srf_mapping_primitive);
  MYON_ENUM_TO_WIDE_STRING_CASE(MYON_TextureMapping::TYPE::brep_mapping_primitive);
  MYON_ENUM_TO_WIDE_STRING_CASE(MYON_TextureMapping::TYPE::ocs_mapping);
  MYON_ENUM_TO_WIDE_STRING_CASE(MYON_TextureMapping::TYPE::false_colors);
  }

  MYON_ERROR("Invalid texture_mapping_type value.");
  return MYON_wString::EmptyString;
}


MYON_TextureMapping::PROJECTION MYON_TextureMapping::ProjectionFromUnsigned(
  unsigned int projection_as_unsigned
  )
{
  switch (projection_as_unsigned)
  {
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_TextureMapping::PROJECTION::no_projection);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_TextureMapping::PROJECTION::clspt_projection);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_TextureMapping::PROJECTION::ray_projection);
  }

  MYON_ERROR("Invalid projection_as_unsigned value.");
  return MYON_TextureMapping::PROJECTION::no_projection;
}

const MYON_wString MYON_TextureMapping::ProjectionToString(MYON_TextureMapping::PROJECTION texture_mapping_projection)
{
  switch (texture_mapping_projection)
  {
  MYON_ENUM_TO_WIDE_STRING_CASE(MYON_TextureMapping::PROJECTION::no_projection);
  MYON_ENUM_TO_WIDE_STRING_CASE(MYON_TextureMapping::PROJECTION::clspt_projection);
  MYON_ENUM_TO_WIDE_STRING_CASE(MYON_TextureMapping::PROJECTION::ray_projection);
  }

  MYON_ERROR("Invalid texture_mapping_projection value.");
  return MYON_wString::EmptyString;
}

MYON_TextureMapping::TEXTURE_SPACE MYON_TextureMapping::TextureSpaceFromUnsigned(
  unsigned int texture_space_as_unsigned
  )
{
  switch (texture_space_as_unsigned)
  {
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_TextureMapping::TEXTURE_SPACE::single);
  MYON_ENUM_FROM_UNSIGNED_CASE(MYON_TextureMapping::TEXTURE_SPACE::divided);
  }

  MYON_ERROR("Invalid texture_space_as_unsigned value.");
  return MYON_TextureMapping::TEXTURE_SPACE::single;
}

const MYON_wString MYON_TextureMapping::SpaceToString(MYON_TextureMapping::TEXTURE_SPACE texture_mapping_space)
{
  switch (texture_mapping_space)
  {
  MYON_ENUM_TO_WIDE_STRING_CASE(MYON_TextureMapping::TEXTURE_SPACE::single);
  MYON_ENUM_TO_WIDE_STRING_CASE(MYON_TextureMapping::TEXTURE_SPACE::divided);
  }

  MYON_ERROR("Invalid texture_mapping_space value.");
  return MYON_wString::EmptyString;
}

MYON_TextureMapping::MYON_TextureMapping() MYON_NOEXCEPT
  : MYON_ModelComponent(MYON_ModelComponent::Type::TextureMapping)
{}

MYON_TextureMapping::~MYON_TextureMapping()
{
  Internal_Destroy();
}

MYON_TextureMapping::MYON_TextureMapping( const MYON_TextureMapping& src)
  : MYON_ModelComponent(MYON_ModelComponent::Type::TextureMapping,src)
{
  Internal_CopyFrom(src);
}

MYON_TextureMapping& MYON_TextureMapping::operator=(const MYON_TextureMapping& src)
{
  if ( this != &src )
  {
    Internal_Destroy();
    MYON_ModelComponent::operator=(src);
    Internal_CopyFrom(src);
  }
  return *this;
}

void MYON_TextureMapping::Internal_Destroy()
{
  PurgeUserData();

  m_mapping_primitive.reset();
}

void MYON_TextureMapping::Internal_CopyFrom(const MYON_TextureMapping& src)
{
  m_type				= src.m_type;
  m_projection			= src.m_projection;
  m_bCapped				= src.m_bCapped;
  m_texture_space		= src.m_texture_space;
  m_Pxyz				= src.m_Pxyz;
  m_Nxyz				= src.m_Nxyz;
  m_uvw					= src.m_uvw;
  m_mapping_primitive	= src.m_mapping_primitive;
}

bool MYON_TextureMapping::IsValid( MYON_TextLog* text_log ) const
{
  if ( m_type != MYON_TextureMapping::TypeFromUnsigned( static_cast<unsigned int>(m_type) ) )
  {
    if ( text_log )
    {
      text_log->Print("MYON_TextureMapping m_type = %d is not a valid value.\n",m_type);
    }
    return false;
  }

  if ( m_projection != MYON_TextureMapping::ProjectionFromUnsigned( static_cast<unsigned int>(m_projection) ) )
  {
    if ( text_log )
    {
      text_log->Print("MYON_TextureMapping m_projection = %d is not a valid value.\n",m_projection);
    }
    return false;
  }

  if (m_texture_space != MYON_TextureMapping::TextureSpaceFromUnsigned( static_cast<unsigned int>(m_texture_space)))
  {
	  if (text_log)
	  {
		  text_log->Print("MYON_TextureMapping m_texture_space = %d is not a valid value.\n",m_texture_space);
	  }
	  return false;
  }

  return true;
}

void MYON_TextureMapping::Dump( MYON_TextLog& text_log ) const
{
  MYON_ModelComponent::Dump(text_log);

  text_log.PushIndent();

  const MYON_wString type(MYON_TextureMapping::TypeToString(m_type));
  text_log.Print("m_type = %ls\n",static_cast<const wchar_t*>(type));

  const MYON_wString projection(MYON_TextureMapping::ProjectionToString(m_projection));
  text_log.Print("m_projection = %ls\n",static_cast<const wchar_t*>(projection));

  const MYON_wString texture_space(MYON_TextureMapping::SpaceToString(m_texture_space));
  text_log.Print("m_texture_space = %ls\n",static_cast<const wchar_t*>(texture_space));

  text_log.Print("m_Pxyz =\n");
  text_log.PushIndent();
  text_log.Print(m_Pxyz);
  text_log.PopIndent();

  text_log.Print("m_Nxyz =\n");
  text_log.PushIndent();
  text_log.Print(m_Nxyz);
  text_log.PopIndent();

  text_log.Print("m_uvw =\n");
  text_log.PushIndent();
  text_log.Print(m_uvw);
  text_log.PopIndent();

  text_log.PopIndent();
}

unsigned int MYON_TextureMapping::SizeOf() const
{
  unsigned int sz = MYON_Object::SizeOf();
  sz = sizeof(*this) - sizeof(MYON_Object);
  return sz;
}


MYON_Xform MYON_Texture::GetPictureShrinkSurfaceTransformation(
  const class MYON_Brep* original,
  const class MYON_Brep* shrunk,
  const MYON_Xform* error_return
)
{
  const class MYON_Surface* original_srf
    = (nullptr != original && 1 == original->m_F.Count())
    ? original->m_F[0].SurfaceOf()
    : nullptr;
  const class MYON_Surface* shrunk_srf
    = (nullptr != shrunk && 1 == shrunk->m_F.Count())
    ? shrunk->m_F[0].SurfaceOf()
    : nullptr;
  return MYON_Texture::GetPictureShrinkSurfaceTransformation(
    original_srf,
    shrunk_srf,
    error_return
  );
}

MYON_Xform MYON_Texture::GetPictureShrinkSurfaceTransformation(
  const class MYON_Surface* original,
  const class MYON_Surface* shrunk,
  const MYON_Xform* error_return
)
{
  MYON_Interval original_udomain;
  MYON_Interval original_vdomain;
  MYON_Interval shrunk_udomain;
  MYON_Interval shrunk_vdomain;
  if (nullptr != original)
  {
    original_udomain = original->Domain(0);
    original_vdomain = original->Domain(1);
  }
  if (nullptr != shrunk)
  {
    shrunk_udomain = shrunk->Domain(0);
    shrunk_vdomain = shrunk->Domain(1);
  }
  return MYON_Texture::GetPictureShrinkSurfaceTransformation(
    original_udomain, original_vdomain,
    shrunk_udomain, shrunk_vdomain,
    error_return
  );
}

MYON_Xform MYON_Texture::GetPictureShrinkSurfaceTransformation(
  const class MYON_Interval& original_udomain,
  const class MYON_Interval& original_vdomain,
  const class MYON_Interval& shrunk_udomain,
  const class MYON_Interval& shrunk_vdomain,
  const MYON_Xform* error_return
)
{
  if (nullptr == error_return)
    error_return = &MYON_Xform::Nan;

  if (false == original_udomain.IsIncreasing())
    return *error_return;
  if (false == original_vdomain.IsIncreasing())
    return *error_return;

  if (false == shrunk_udomain.IsIncreasing())
    return *error_return;
  if (false == shrunk_vdomain.IsIncreasing())
    return *error_return;

  if (false == original_udomain.Includes(shrunk_udomain, false))
    return *error_return;
  if (false == original_vdomain.Includes(shrunk_vdomain, false))
    return *error_return;
  if (false == original_udomain.Includes(shrunk_udomain, true) && false == original_vdomain.Includes(shrunk_vdomain, true))
    return *error_return;

  const MYON_3dPoint p0(original_udomain.NormalizedParameterAt(shrunk_udomain[0]), original_vdomain.NormalizedParameterAt(shrunk_vdomain[0]),0.0);
  const MYON_3dPoint p1(original_udomain.NormalizedParameterAt(shrunk_udomain[1]), original_vdomain.NormalizedParameterAt(shrunk_vdomain[1]),0.0);
  if (!(0.0 <= p0.x && p0.x < p1.x && p1.x <= 1.0))
    return *error_return;
  if (!(0.0 <= p0.y && p0.y < p1.y && p1.y <= 1.0))
    return *error_return;

  const double sx = shrunk_udomain.Length() / original_udomain.Length();
  if (!(sx > 0.0 && sx <= 1.0))
    return *error_return;
  const double sy = shrunk_vdomain.Length() / original_vdomain.Length();
  if (!(sx > 0.0 && sx <= 1.0))
    return *error_return;

  // The new brep has a smaller surface.
  // Adust the texture transform to use the proper subset of the old picture image texture.
  MYON_Xform x
    = MYON_Xform::TranslationTransformation(p0 - MYON_3dPoint::Origin)
    * MYON_Xform::ScaleTransformation(MYON_3dPoint::Origin, sx, sy, 1.0);

  return x;
}

bool MYON_TextureMapping::ReverseTextureCoordinate( int dir )
{
  bool rc = false;
  if ( 0 <= dir && dir <= 3 )
  {
    MYON_Xform x(MYON_Xform::IdentityTransformation);
    x.m_xform[dir][dir] = -1.0;
    x.m_xform[dir][3] = 1.0;
    m_uvw = x*m_uvw;
    rc = true;
  }
  return rc;
}

bool MYON_TextureMapping::SwapTextureCoordinate( int i, int j )
{
  bool rc = false;
  if (i!=j && 0 <= i && i <= 3 && 0 <= j && j <= 3)
  {
    MYON_Xform x(MYON_Xform::IdentityTransformation);
    x.m_xform[i][i] = x.m_xform[j][j] = 0.0;
    x.m_xform[i][j] = x.m_xform[j][i] = 1.0;
    m_uvw = x*m_uvw;
    rc = true;
  }
  return rc;
}

bool MYON_TextureMapping::TileTextureCoordinate( int dir, double count, double offset )
{
  bool rc = false;
  if ( 0 <= dir && dir <= 3 && 0.0 != count && MYON_IsValid(count) && MYON_IsValid(offset) )
  {
    MYON_Xform x(MYON_Xform::IdentityTransformation);
    x.m_xform[dir][dir] = count;
    x.m_xform[dir][3] = offset;
    m_uvw = x*m_uvw;
    rc = true;
  }
  return rc;
}


bool MYON_Texture::ReverseTextureCoordinate( int dir )
{
  bool rc = false;
  if ( 0 <= dir && dir <= 3 )
  {
    MYON_Xform x(MYON_Xform::IdentityTransformation);
    x.m_xform[dir][dir] = -1.0;
    x.m_xform[dir][3] = 1.0;
    m_uvw = x*m_uvw;
    rc = true;
  }
  return rc;
}

bool MYON_Texture::SwapTextureCoordinate( int i, int j )
{
  bool rc = false;
  if (i!=j && 0 <= i && i <= 3 && 0 <= j && j <= 3)
  {
    MYON_Xform x(MYON_Xform::IdentityTransformation);
    x.m_xform[i][i] = x.m_xform[j][j] = 0.0;
    x.m_xform[i][j] = x.m_xform[j][i] = 1.0;
    m_uvw = x*m_uvw;
    rc = true;
  }
  return rc;
}

bool MYON_Texture::TileTextureCoordinate( int dir, double count, double offset )
{
  bool rc = false;
  if ( 0 <= dir && dir <= 3 && 0.0 != count && MYON_IsValid(count) && MYON_IsValid(offset) )
  {
    MYON_Xform x(MYON_Xform::IdentityTransformation);
    x.m_xform[dir][dir] = count;
    x.m_xform[dir][3] = offset;
    m_uvw = x*m_uvw;
    rc = true;
  }
  return rc;
}

bool MYON_Texture::IsTiled( int dir, double* count, double* offset ) const
{
  if ( count )
    *count = 1.0;
  if ( offset )
    *offset = 0.0;

  if ( 0 <= dir && dir <= 3 )
  {
    int row0=-1, row, col;
    for ( row = 0; row < 3; row++ )
    {
      for ( col = 0; col < 3; col++ )
      {
        if ( col != dir && 0.0 != m_uvw.m_xform[row][col] )
          break;
      }
      if ( 3 == col )
      {
        if ( -1 == row0 )
        {
          row0 = row;
        }
        else
          return false;
      }
    }
    if ( row0 >= 0 )
    {
      if (count)
        *count = m_uvw.m_xform[row0][dir];
      if ( offset )
        *offset = m_uvw.m_xform[row0][3];
      return true;
    }
  }

  return false;
}

static const double on__overflow_tol = 1.0e100;

static
int BestHitHelper(double t0, double t1)
{
  return ((t0 < 0.0 && t1 > t0) || (0.0 <= t1 && t1 < t0)) ? 1 : 0;
}

static
int IntersectBoxRayHelper(const MYON_3dPoint& rst, const MYON_3dVector& n, int dir, double* s)
{
  /*
  returns:
    0 = ray parallel to sides
    1 = ray hit left side (x=-1)
    2 = ray hit right side (x=+1)
    3 = ray hit back side (y=-1)
    4 = ray hit front side (y=+1)
    5 = ray hit bottom side (z=-1)
    6 = ray hit top side (z=+1)
  */
  double nx = (&n.x)[dir];
  MYON_3dPoint Q;
  double t,t0,t1;

  // protect against overflow
  t = fabs(nx)*on__overflow_tol;
  t0 = (-1.0 - (&rst.x)[dir]);
  t1 = ( 1.0 - (&rst.x)[dir]);
  if ( fabs(t0) >= t || fabs(t1) >= t )
  {
    *s = MYON_UNSET_VALUE;
    return 0;
  }

  t0 /= nx;
  Q = rst + t0*n;
  if ( dir )
  {
    t = Q.x;
    Q.x = Q[dir];
    Q[dir] = t;
  }
  if ( fabs(Q.x+1.0) > MYON_SQRT_EPSILON
        || Q.y < -(1.0+MYON_SQRT_EPSILON) || Q.y > (1.0+MYON_SQRT_EPSILON)
        || Q.z < -(1.0+MYON_SQRT_EPSILON) || Q.z > (1.0+MYON_SQRT_EPSILON)
        )
  {
    // The ray's intersection with the plane missed the
    // (-1,+1)x(-1,+1) square that is the side of the box.
    t0 = MYON_UNSET_VALUE;
  }

  t1 /= nx;
  Q = rst + t1*n;
  if ( dir )
  {
    t = Q.x;
    Q.x = Q[dir];
    Q[dir] = t;
  }
  if ( fabs(Q.x-1.0) > MYON_SQRT_EPSILON
        || Q.y < -(1.0+MYON_SQRT_EPSILON) || Q.y > (1.0+MYON_SQRT_EPSILON)
        || Q.z < -(1.0+MYON_SQRT_EPSILON) || Q.z > (1.0+MYON_SQRT_EPSILON)
        )
  {
    // The ray's intersection with the plane missed the
    // (-1,+1)x(-1,+1) square that is the side of the box.
    t1 = MYON_UNSET_VALUE;
    if ( MYON_UNSET_VALUE == t0 )
    {
      *s = MYON_UNSET_VALUE;
      return 0;
    }
  }

  int rc;
  if ( MYON_UNSET_VALUE == t0 || 1 == BestHitHelper(t0,t1) )
  {
    rc = 2 + 2*dir;
    *s = t1;
  }
  else
  {
    rc = 1 + 2*dir;
    *s = t0;
  }
  return rc;
}


int MYON_TextureMapping::EvaluatePlaneMapping(
  const MYON_3dPoint& P,
  const MYON_3dVector& N,
  MYON_3dPoint* T
  ) const
{
  // The matrix m_Pxyz transforms the world  coordinate
  // "mapping rectangle" into the rectangle
  //   -1 <= r <= 1, -1 <= s <= 1, and  (-1 <= t <= 1)

  MYON_3dPoint rst(m_Pxyz*P);

  if ( MYON_TextureMapping::PROJECTION::ray_projection == m_projection )
  {
    MYON_3dVector n(m_Nxyz*N);
    if ( fabs(rst.z) < fabs(n.z)*on__overflow_tol )
    {
      double t = -rst.z/n.z;
      rst.x = rst.x + t*n.x;
      rst.y = rst.y + t*n.y;
    }
  }

  // convert -1 <= r <= 1, -1 <= s <= 1
  // to normalized texture coordinate
	rst.x = 0.5*rst.x + 0.5;
	rst.y = 0.5*rst.y + 0.5;

  // Apply texture coordinate transformation
  *T = m_uvw*rst;

  //See docs - if m_bCapped is false, then planar is truely flat.
  if (!m_bCapped)
	  T->z = 0.0;

  return 1;
}

int MYON_TextureMapping::EvaluateSphereMapping(
											  const MYON_3dPoint& P,
											  const MYON_3dVector& N,
											  MYON_3dPoint* T
											  ) const
{
  // The matrix m_Pxyz transforms the world coordinate
  // "mapping sphere" into the sphere centered at
  // rst = (0,0,0) with radius 1.0.

  MYON_3dPoint rst(m_Pxyz*P);
	const double r = ((const MYON_3dVector*)(&rst.x))->Length();
	double t0, t1;

	if ( MYON_TextureMapping::PROJECTION::ray_projection == m_projection )
	{
		MYON_3dVector n(m_Nxyz*N);
		// Shoot a ray from P in the direction N and see if it
		// hits the sphere.
		int rc = MYON_SolveQuadraticEquation( (n.x*n.x+n.y*n.y+n.z*n.z),
			2.0*(rst.x*n.x+rst.y*n.y+rst.z*n.z),
			(rst.x*rst.x+rst.y*rst.y+rst.z*rst.z) - 1.0,
			&t0, &t1 );
		if (rc >= 0 )
		{
			if ( 2 != rc && 1 == BestHitHelper(t0,t1) )
			{
				t0 = t1;
			}
			rst = rst + t0*n;
		}
	}

	// convert sphere 3d location to longitude, latitude, radius
	double longitude = (0.0 != rst.y || 0.0 != rst.x)
		? atan2(rst.y,rst.x)
		: 0.0;
	double latitude = (0.0 != rst.z)
		? atan2(rst.z,((const MYON_2dVector*)(&rst.x))->Length())
		: 0.0;
	if ( latitude > MYON_PI )
		latitude -= 2.0*MYON_PI;

  // convert longitude to normalized texture coordinate
	rst.x = 0.5*longitude/MYON_PI;
	if ( rst.x < -MYON_EPSILON )
		rst.x += 1.0;
	else if (rst.x < 0.0)
		rst.x = 0.0;
	else if (rst.x > 1.0)
		rst.x = 1.0;

  // convert longitude to normalized texture coordinate
	rst.y = latitude/MYON_PI + 0.5;
  if ( rst.y <= 0.0 )
    rst.y = 0.0;
	else if ( rst.y > 1.0 )
		  rst.y = 1.0;

  // radius is already normalized
	rst.z = r;

  // apply texture coordinate transformation
	*T = m_uvw*rst;

  return 1;
}

int MYON_TextureMapping::EvaluateCylinderMapping(
												const MYON_3dPoint& P,
												const MYON_3dVector& N,
												MYON_3dPoint* T
												) const
{
  // The matrix m_Pxyz transforms the world coordinate
  // "mapping cylinder" into the cylinder centered at
  // rst = (0,0,0) with radius 1.0.  The axis runs
  // from rst = (0,0,-1) to rst = (0,0,+1).

	MYON_3dPoint rst(m_Pxyz*P);

	MYON_3dPoint Q;
	const double r = ((const MYON_2dVector*)(&rst.x))->Length();
	double t, t0, t1;
	int side0, side1;
	PROJECTION mapping_proj = m_projection;

	side0 = 0;
	if ( MYON_TextureMapping::PROJECTION::ray_projection == mapping_proj )
	{
		MYON_3dVector n(m_Nxyz*N);
		t = 0.0;

		if ( m_bCapped )
		{
			// shoot at caps
			//  The < t check prevents overflow when the
			//  ray is nearly parallel to the cap.
			t = fabs(n.z)*on__overflow_tol;
			if ( fabs(1.0+rst.z) < t && fabs(1.0-rst.z) < t )
			{
				side0 = 2;
				side1 = 3;

				t0 = (-1.0 - rst.z)/n.z;
				Q = rst + t0*n;
				if ( fabs(1.0+Q.z) > MYON_SQRT_EPSILON
					|| (Q.x*Q.x + Q.y*Q.y) > 1.0 + 2.0*MYON_SQRT_EPSILON + MYON_EPSILON )
				{
          // The ray's intersection with the bottom plane missed the
          // radius 1 disk that is the bottom of the cylinder.
					side0 = 0;
				}

				t1 = ( 1.0 - rst.z)/n.z;
				Q = rst + t1*n;
				if ( fabs(1.0-Q.z) > MYON_SQRT_EPSILON
					|| (Q.x*Q.x + Q.y*Q.y) > 1.0 + 2.0*MYON_SQRT_EPSILON + MYON_EPSILON )
				{
          // The ray's intersection with the top plane missed the
          // radius 1 disk that is the top of the cylinder.
					side1 = 0;
				}
				if ( 0 == side0 || 1 == BestHitHelper(t0,t1) )
				{
					side0 = side1;
					t = t1;
				}
				else
				{
					t = t0;
				}
			}
		}

		// shoot ray at the cylinder wall
		int rc = MYON_SolveQuadraticEquation( (n.x*n.x+n.y*n.y),
			2.0*(rst.x*n.x+rst.y*n.y),
			(rst.x*rst.x+rst.y*rst.y) - 1.0,
			&t0, &t1 );
		if (rc >= 0 )
		{
			if ( 2 != rc  && 1 == BestHitHelper(t0,t1) )
			{
				t0 = t1;
			}
			if ( 0 == side0 )
			{
				// Either the caps are missing or the ray missed the caps.
        // The best hit is the cylinder wall.
				side0 = 1;
				rst = rst + t0*n;
			}
			else if ( 1 != BestHitHelper(t0,t) )
			{
				// The cylinder is capped and the ray hit the cap,
        // hit the infinite cylinder wall, and the wall
        // hit is "first".  If the ray hits the finite
        // cylinder wall, the I will use the wall hit.
				t1 = rst.z + t0*n.z;
				if ( t1 >= -(1.0+MYON_SQRT_EPSILON) && t1 <= 1.0+MYON_SQRT_EPSILON )
				{
					// use the hit on the cylinder wall
					side0 = 1;
					rst.x = rst.x + t0*n.x;
					rst.y = rst.y + t0*n.y;
          rst.x = t1;
				}
			}
		}

		if ( side0 > 1 )
		{
			// best hit is on a cap
			rst = rst + t*n;
		}
	}

	if ( m_bCapped && 0 == side0 )
	{
    if ( fabs(rst.z) > 1.0+MYON_SQRT_EPSILON )
    {
      if ( fabs(rst.z) > r )
      {
        side0 = (rst.z < 0.0) ? 2 : 3;
      }
    }
    else if ( r <= 1.001 )
    {
      // The point is inside the capped cylinder.
      // Use normal to dermine which surface to use
      // for closest point test.
		  MYON_3dVector n(m_Nxyz*N);
      if (  ( fabs(n.z) > fabs(n.x) && fabs(n.z) > fabs(n.y) ) )
      {
        side0 = (n.z < 0.0) ? 2 : 3;
      }
    }
	}

	if ( 2 == side0 || 3 == side0 )
	{
    // The cylinder is capped and P maps to
    // the top (1 == side0) or bottom (2 == side0)

    if ( 2 == side0 )
    {
      // This is the same convention as box mapping.
      // Put another way, if you change the mapping
      // between box and cylinder, you get the same
      // picture on the top and bottom.
      rst.x = -rst.x;
    }

		if ( MYON_TextureMapping::TEXTURE_SPACE::divided == m_texture_space )
		{
		  if ( r >= 1.0-MYON_SQRT_EPSILON )
		  {
			  rst.x /= (r+MYON_SQRT_EPSILON);
			  rst.y /= (r+MYON_SQRT_EPSILON);
		  }
    }
    else if ( r > 1.0 )
	  {
		  rst.x /= r;
		  rst.y /= r;
	  }


    // convert to normalized texture coordinates
		rst.x = 0.5*rst.x + 0.5;
    if ( rst.x < 0.0) rst.x = 0.0; else if (rst.x > 1.0) rst.x = 1.0;
		rst.y = 0.5*rst.y + 0.5;
    if ( rst.y < 0.0) rst.y = 0.0; else if (rst.y > 1.0) rst.y = 1.0;

		if ( MYON_TextureMapping::TEXTURE_SPACE::divided == m_texture_space )
		{
      // bottom uses 4/6 <= x <= 5/6 region of the texture map.
      // top uses 5/6 <= x <= 1 region of the texture map.
			rst.x = (2.0 + side0 + rst.x)/6.0;
		}
	}
	else
	{
    // P maps to side of the cylinder.
    //
    // convert longitude to normalized texture coordinate
		t = (0.0 != rst.y || 0.0 != rst.x) ? atan2(rst.y,rst.x) : 0.0;
		rst.x = 0.5*t/MYON_PI;
		if ( rst.x < -MYON_EPSILON )
			rst.x += 1.0;
		else if (rst.x < 0.0 )
			rst.x = 0.0;
		else if (rst.x > 1.0 )
			rst.x = 1.0;

    if ( MYON_TextureMapping::TEXTURE_SPACE::divided == m_texture_space )
    {
      // side uses 0 <= x <= 2/3 region of the texture map
      rst.x *= 2.0;
			rst.x /= 3.0;
    }

    // convert height to normalized texture coordinate
  	rst.y = 0.5*rst.z + 0.5;
    if ( m_bCapped )
    {
      // clamp height
      if ( rst.y < 0.0 ) rst.y = 0.0; else if ( rst.y > 1.0 ) rst.y = 1.0;
    }
    side0 = 1;
	}
	rst.z = r;

	*T = m_uvw*rst;

  return side0;
}

int MYON_TextureMapping::EvaluateBoxMapping(
										   const MYON_3dPoint& P,
										   const MYON_3dVector& N,
										   MYON_3dPoint* T
										   ) const
{
  // The matrix m_Pxyz transforms the world coordinate
  // "mapping cylinder" into the cylinder centered at
  // rst = (0,0,0) with radius 1.0.  The axis runs
  // from rst = (0,0,-1) to rst = (0,0,+1).

  MYON_3dPoint rst(m_Pxyz*P);

	MYON_3dVector n(m_Nxyz*N);
  n.Unitize();

	int side0, side1;
	double t0, t1;

	side0 = 0;
	t0 = 0.0;

  // side flag
  //  1 =  left side (x=-1)
  //  2 =  right side (x=+1)
  //  3 =  back side (y=-1)
  //  4 =  front side (y=+1)
  //  5 =  bottom side (z=-1)
  //  6 =  top side (z=+1)

  if ( MYON_TextureMapping::PROJECTION::ray_projection == m_projection )
	{

		if ( m_bCapped )
		{
			// intersect ray with top and bottom
			side0 = IntersectBoxRayHelper(rst,n,2,&t0);
		}
		// intersect ray with front and back
		side1 = IntersectBoxRayHelper(rst,n,0,&t1);
		if ( 0 == side0 || 1 == BestHitHelper(t0,t1) )
		{
			side0 = side1;
			t0 = t1;
		}
		// intersect ray with left and right
		side1 = IntersectBoxRayHelper(rst,n,1,&t1);
		if ( 0 == side0 || 1 == BestHitHelper(t0,t1) )
		{
			side0 = side1;
			t0 = t1;
		}
		if ( 0 != side0 )
		{
			// ray hit the box
			rst = rst + t0*n;
		}
	}

  if ( 0 == side0 )
  {
    // set side0 = side closest to the point
    side1 = (fabs(rst.x) >= fabs(rst.y)) ? 0 : 1;
    if ( m_bCapped && fabs(rst.z) > fabs(((double*)&rst.x)[side1]) )
      side1 = 2;
    t1 = (&rst.x)[side1];
    if ( t1 < 0.0 )
    {
      side0 = 2*side1 + 1;
    }
    else
    {
      side0 = 2*side1 + 2;
    }

    //if ( fabs(t1) <= 1.0+MYON_SQRT_EPSILON )...
    //// The point is inside the box.  If the normal
    //// is not zero, then use it to choose the side
    //// used for the closest point projection.

    side1 = ( fabs(n.x) >= fabs(n.y) ) ? 0 : 1;
    if ( m_bCapped && fabs(n.z) > fabs((&n.x)[side1]))
    {
      side1 = 2;
    }
    t1 = n[side1];
    if ( 0.0 != t1 )
    {
      if ( t1 < 0.0 )
        side0 = 2*side1 + 1;
      else if ( t1 > 0.0 )
        side0 = 2*side1 + 2;
    }
  }

	double shift = 0.0;

  // side flag
  //  1 =  left side (x=-1)
  //  2 =  right side (x=+1)
  //  3 =  back side (y=-1)
  //  4 =  front side (y=+1)
  //  5 =  bottom side (z=-1)
  //  6 =  top side (z=+1)

	switch(side0)
	{
	case 1: // x = -1
		rst.x = -rst.y;
		rst.y =  rst.z;
		shift =  3.0;
		break;
	case 2: // x = +1
		rst.x =  rst.y;
		rst.y =  rst.z;
		shift =  1.0;
		break;
	case 3: // y = -1
		rst.y =  rst.z;
		shift =  0.0;
		break;
	case 4: // y = +1
		rst.x = -rst.x;
		rst.y =  rst.z;
		shift =  2.0;
		break;
	case 5: // z = -1
		rst.x = -rst.x;
		shift =  4.0;
		break;
	case 6: // z = +1
		shift =  5.0;
		break;
	}

  // normalize texture coordinates
  rst.x = 0.5*rst.x + 0.5;
  rst.y = 0.5*rst.y + 0.5;
	rst.z = 0.0;

	if( MYON_TextureMapping::TEXTURE_SPACE::divided == m_texture_space)
	{
    rst.x = (shift + rst.x)/(m_bCapped ? 6.0 : 4.0);
	}

	*T = m_uvw*rst;

  return side0;
}


static bool ProjectToFaceSpace(const MYON_3dPoint & ptV0, const MYON_3dPoint & ptV1, const MYON_3dPoint & ptV2, const MYON_3dPoint & ptP, double & xOut, double & yOut)
{
	const double m00 = (ptV1 - ptV0).LengthSquared();
	const double m01 = MYON_DotProduct(ptV1 - ptV0, ptV2 - ptV0);
	const double d0 = MYON_DotProduct(ptP - ptV0, ptV1 - ptV0);
	const double m10 = m01;
	const double m11 = (ptV2 - ptV0).LengthSquared();
	const double d1 = MYON_DotProduct(ptP - ptV0, ptV2 - ptV0);
	double pivot_ratio = 0.0;

	if (2 == MYON_Solve2x2(m00, m01, m10, m11, d0, d1, &xOut, &yOut, &pivot_ratio))
	{
		if (pivot_ratio < 0.01)
		{
			const MYON_3dPoint ptProjection = ptV0 + xOut * (ptV1 - ptV0) + yOut * (ptV2 - ptV0);

			MYON_3dVector vtOffsetDir = ptP - ptProjection;

			const double offsetLength = vtOffsetDir.LengthAndUnitize();
			if (offsetLength < 1e-12)
				return true;

			MYON_3dVector vtPlaneNormal = MYON_CrossProduct(ptV2 - ptV0, ptV2 - ptV1);

			if (false == vtPlaneNormal.Unitize())
				return false;

			const double dOffsetDirDotPlaneNormal = MYON_DotProduct(vtPlaneNormal, vtOffsetDir);

			if (-0.99 < dOffsetDirDotPlaneNormal &&
				dOffsetDirDotPlaneNormal < 0.99)
				return false;
		}

		return true;

	}

	return false;
}


int MYON_TextureMapping::Evaluate(
        const MYON_3dPoint& P,
        const MYON_3dVector& N,
        MYON_3dPoint* T,
        const MYON_Xform& P_xform,
        const MYON_Xform& N_xform
        ) const
{
  int rc;
  MYON_3dPoint Q = P_xform*P;
  if ( MYON_TextureMapping::PROJECTION::ray_projection == m_projection )
  {
    // need a transformed normal
    MYON_3dVector V = N_xform*N;
    V.Unitize();
    rc = Evaluate(Q,V,T);
  }
  else
  {
    // normal is ignored
    rc = Evaluate(Q,N,T);
  }
  return rc;
}

int MYON_TextureMapping::Evaluate(
        const MYON_3dPoint& P,
        const MYON_3dVector& N,
        MYON_3dPoint* T
        ) const
{
  int rc;

	switch(m_type)
	{
	case MYON_TextureMapping::TYPE::srfp_mapping:
		*T = m_uvw * P; // Do NOT apply m_Pxyz here.
    rc = 1;
		break;
	case MYON_TextureMapping::TYPE::sphere_mapping:
		rc = EvaluateSphereMapping(P,N,T);
		break;
	case MYON_TextureMapping::TYPE::cylinder_mapping:
		rc = EvaluateCylinderMapping(P,N,T);
		break;
	case MYON_TextureMapping::TYPE::box_mapping:
		rc = EvaluateBoxMapping(P,N,T);
		break;

	case MYON_TextureMapping::TYPE::mesh_mapping_primitive:
    rc = 0;
		break;

	case MYON_TextureMapping::TYPE::srf_mapping_primitive:
    rc = 0;
		break;

	case MYON_TextureMapping::TYPE::brep_mapping_primitive:
    rc = 0;
		break;

	default:
		rc = EvaluatePlaneMapping(P,N,T);
		break;
	}
  return rc;
}

const MYON_Object* MYON_TextureMapping::CustomMappingPrimitive(void) const
{
	return m_mapping_primitive.get();
}

const std::shared_ptr<const MYON_Object>& MYON_TextureMapping::SharedCustomMappingPrimitive(void) const
{
  return m_mapping_primitive;
}

//Returns a valid mesh if the custom mapping primitive is a mesh.  Otherwise nullptr.
//Implementation is return MYON_Mesh::Cast(CustomMappingPrimitive());
const MYON_Mesh* MYON_TextureMapping::CustomMappingMeshPrimitive(void) const
{
	return MYON_Mesh::Cast(CustomMappingPrimitive());
}

//Returns a valid brep if the custom mapping primitive is a brep.  Otherwise nullptr.
//Implementation is return MYON_Brep::Cast(CustomMappingPrimitive());
const MYON_Brep* MYON_TextureMapping::CustomMappingBrepPrimitive(void) const
{
	return MYON_Brep::Cast(CustomMappingPrimitive());
}
//Returns a valid surface if the custom mapping primitive is a surface.  Otherwise nullptr.
//Implementation is return MYON_Surface::Cast(CustomMappingPrimitive());
const MYON_Surface* MYON_TextureMapping::CustomMappingSurfacePrimitive(void) const
{
	return MYON_Surface::Cast(CustomMappingPrimitive());
}

class MappingCRCCache : public MYON_UserData
{
  MYON_OBJECT_DECLARE(MappingCRCCache);
public:
  MappingCRCCache(int mapping_crc)
  {
    m_application_uuid = MYON_opennurbs_id;
    m_userdata_uuid = MYON_CLASS_ID(MappingCRCCache);
    m_userdata_copycount = 1;
    m_mapping_crc = mapping_crc;
  }

  MappingCRCCache()
  {
    m_application_uuid = MYON_opennurbs_id;
    m_userdata_uuid = MYON_CLASS_ID(MappingCRCCache);
    m_userdata_copycount = 1;
    m_mapping_crc = -1;
  }

  MappingCRCCache(const MappingCRCCache& src) : MYON_UserData(src)
  {
    m_userdata_copycount = src.m_userdata_copycount;
    m_mapping_crc = src.m_mapping_crc;
  }

  static const int current_version;

  auto& operator=(const MappingCRCCache& src)
  {
    MYON_UserData::operator = (src);
    return *this;
  }

#if defined(MYON_COMPILER_CLANG)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif
  virtual bool GetDescription(MYON_wString& description)
  {
    description = L"MappingCRCCache";
    return true;
  }

  virtual bool Archive() const { return true; }

  virtual bool Write(MYON_BinaryArchive& binary_archive) const
  {
    if (binary_archive.WriteInt(current_version))
    {
      if (binary_archive.WriteInt(m_mapping_crc))
      {
        return true;
      }
    }
    
    return false;
  }

  virtual bool Read(MYON_BinaryArchive& binary_archive)
  {
    int nVersion = 1;
    if (binary_archive.ReadInt(&nVersion) && current_version == nVersion)
    {
      if (binary_archive.ReadInt(&m_mapping_crc))
      {
        return true;
      }
    }

    return false;
  }
#if defined(MYON_COMPILER_CLANG)
#pragma clang diagnostic pop
#endif

public:
  int m_mapping_crc = -1;
};

MYON_OBJECT_IMPLEMENT(MappingCRCCache, MYON_UserData, "5A4971F3-AA73-493C-A385-2F7EB4288989");
const int MappingCRCCache::current_version = 1;



static MYON__UINT32 MappingCRC(const MYON_Object* pMappingPrimitive)
{
  const MYON__UINT32 crc32_in = 0x12341234;

  if (const auto* p = MYON_Mesh::Cast(pMappingPrimitive))
  {
    MYON__UINT32 crc32 =  p->DataCRC(crc32_in);

    if (p->HasTextureCoordinates())
    {
      // 25 August 2010 Dale Lear
      //   Including m_T[] in crc32 per Jussi and Andy email discussion.
      //   This is probably correct because users will expect the
      //   "picture" on the mesh to be applied to the target in
      //   a visual way.
      const MYON_2fPoint* tex = p->m_T.Array();
      crc32 = MYON_CRC32(crc32, p->m_T.Count() * sizeof(tex[0]), tex);
    }
    // January 7th 2021 Jussi
    //   Added crc of origin to change the mapping crc to tricker re-mapping
    //   of custom mesh mapped objects. This is to make the improvements to
    //   CMeshClosestPointMapper::MatchFaceTC effective on existing models.
    crc32 = MYON_CRC32(crc32, sizeof(MYON_3dPoint), &MYON_3dPoint::UnsetPoint);

    return crc32;
  }

  if (const auto* p = MYON_Brep::Cast(pMappingPrimitive))
  {
    return p->DataCRC(crc32_in);
    // 25 August 2010 Dale Lear
    //   Should brep's render meshes be included in the crc?
    //   The texture that is being mapped is actually
    //   being applied to the brep by the render mesh's
    //   m_T[] values and some users will want to see
    //   the "picture" on the brep mapped to the
    //   "picture" on the
    //   target.
  }

  if (const auto* p = MYON_Surface::Cast(pMappingPrimitive))
  {
    return p->DataCRC(crc32_in);
  }

  return crc32_in;
}

void MYON_TextureMapping::SetCustomMappingPrimitive(MYON_Object* pMappingObject)
{
  if (nullptr != pMappingObject)
  {
    const auto mapping_crc = ::MappingCRC(pMappingObject);

    if (auto* pUD = static_cast<MappingCRCCache*>(pMappingObject->GetUserData(MYON_CLASS_ID(MappingCRCCache))))
    {
      pUD->m_mapping_crc = mapping_crc;
    }

    pMappingObject->AttachUserData(new MappingCRCCache(mapping_crc));
  }

  m_mapping_primitive.reset(pMappingObject);
}

MYON__UINT32 MYON_TextureMapping::MappingCRC() const
{
  // include any member that can change values returned by Evaluate
  MYON__UINT32 crc32 = 0x12345678;
  crc32 = MYON_CRC32(crc32,sizeof(m_type),&m_type);
  if ( MYON_TextureMapping::TYPE::srfp_mapping != m_type )
  {
    // As of 21 June 2006 m_Pxyz cannot effect MYON_TextureMapping::TYPE::srfp_mapping,
    // so it shouldn't be included in the CRC for srfp_mappings.
    crc32 = MYON_CRC32(crc32,sizeof(m_projection),    &m_projection);
    crc32 = MYON_CRC32(crc32,sizeof(m_texture_space), &m_texture_space);
    crc32 = MYON_CRC32(crc32,sizeof(m_bCapped),		    &m_bCapped);
    crc32 = MYON_CRC32(crc32,sizeof(m_Pxyz),          &m_Pxyz);
    // do not include m_Nxyz here - it won't help and may hurt

    if (m_mapping_primitive)
    {
      const auto* pUD = static_cast<MappingCRCCache*>(m_mapping_primitive->GetUserData(MYON_CLASS_ID(MappingCRCCache)));
      if (pUD)
      {
        crc32 = MYON_CRC32(crc32, sizeof(MappingCRCCache::m_mapping_crc), &pUD->m_mapping_crc);
      }
      else
      {
        //Existing files.
        const auto mapping_crc = ::MappingCRC(m_mapping_primitive.get());
        const_cast<MYON_Object*>(m_mapping_primitive.get())->AttachUserData(new MappingCRCCache(mapping_crc));
        crc32 = MYON_CRC32(crc32, sizeof(mapping_crc), &mapping_crc);
      }
    }
  }
  else
  {
    // Jussi, Aug 18 2022, RH-69752: Surface mapping uvw transform has changed.
    // Changing crc makes sure existing models will show the correct mapping.
    crc32++;
  }

  crc32 = MYON_CRC32(crc32,sizeof(m_uvw), &m_uvw);
  return crc32;
}

bool MYON_TextureMapping::RequiresVertexNormals() const
{
  if ( MYON_TextureMapping::TYPE::srfp_mapping == m_type )
    return false;

	if(m_projection == MYON_TextureMapping::PROJECTION::ray_projection)
    return true;

  if(m_type == MYON_TextureMapping::TYPE::box_mapping)
    return true;
	if(m_type == MYON_TextureMapping::TYPE::cylinder_mapping && m_bCapped)
    return true;

	return false;
}

bool MYON_TextureMapping::IsPeriodic(void) const
{
	return (m_type == MYON_TextureMapping::TYPE::sphere_mapping || m_type == MYON_TextureMapping::TYPE::cylinder_mapping);
}

bool MYON_TextureMapping::HasMatchingTextureCoordinates(
       const MYON_Mesh& mesh,
       const MYON_Xform* mesh_xform
       ) const
{
  bool rc = (mesh.HasTextureCoordinates())
          ? HasMatchingTextureCoordinates(mesh.m_Ttag,mesh_xform)
          : false;

  return rc;
}

bool MYON_TextureMapping::HasMatchingCachedTextureCoordinates(
  const MYON_Mesh& mesh,
  const MYON_Xform* mesh_xform
) const
{
  bool rc = false;

  for (int i = 0; i < mesh.m_TC.Count(); i++)
  {
    if (mesh.VertexCount() == mesh.m_TC[i].m_T.Count() && HasMatchingTextureCoordinates(mesh.m_TC[i].m_tag, mesh_xform))
    {
      rc = true;
      break;
    }
  }

  return rc;
}

bool MYON_TextureMapping::HasMatchingTextureCoordinates(
       const MYON_MappingTag& tag,
       const MYON_Xform* mesh_xform
       ) const
{
  bool rc = false;

  // DO NOT COMPARE m_mapping_id's in this function.
  // This function returns true if the tc values
  // calculated by the mapping will be the same
  // as the mapping that was used to set the tag.
  if ( tag.m_mapping_crc == MappingCRC() )
  {
    rc = true;

    // zero transformations indicate the mapping
    // values are independent of the 3d location
    // of the mesh.  The MYON_TextureMapping::TYPE::srfp_mapping != m_type
    // check is used because these mappings are
    // always independent of 3d location but
    // the transformations are often set.
    if ( MYON_TextureMapping::TYPE::srfp_mapping != m_type
         && mesh_xform
         && mesh_xform->IsValid()
         && !mesh_xform->IsZero()
         && !tag.m_mesh_xform.IsZero()
       )
    {
      // compare xforms - these can have a bit of slop
      const double* a = &mesh_xform->m_xform[0][0];
      const double* b = &tag.m_mesh_xform.m_xform[0][0];
      for ( int i = 16; i--; /*empty*/ )
      {
        if ( fabs(*a++ - *b++) > MYON_SQRT_EPSILON )
        {
          rc = false;
          break;
        }
      }
    }
  }

  return rc;
}

static
bool GetSPTCHelper(
  const MYON_Mesh& mesh,
  const MYON_TextureMapping& mapping,
  float* tc,
  int tc_stride
  )
{
  const int vcnt = mesh.m_V.Count();
  if ( vcnt <= 0 )
    return false;
  if ( !mesh.HasSurfaceParameters() )
    return false;
  const MYON_2dPoint* S = mesh.m_S.Array();
  if ( !S )
    return false;

  int i;
  double u, v, a, b;

  // srf_udom and srf_vdom record the range
  // of parameters saved in the m_S[] array.
  MYON_Interval srf_udom = mesh.m_srf_domain[0];
  MYON_Interval srf_vdom = mesh.m_srf_domain[1];
  if ( !srf_udom.IsIncreasing() || !srf_vdom.IsIncreasing() )
  {
    // Attempt to calculate it from m_S[].
    srf_udom.m_t[0] = srf_udom.m_t[1] = S[0].x;
    srf_vdom.m_t[0] = srf_vdom.m_t[1] = S[0].y;
    for ( i = 1; i < vcnt; i++ )
    {
      u = S[i].x;
      if      (u < srf_udom.m_t[0]) srf_udom.m_t[0] = u;
      else if (u > srf_udom.m_t[1]) srf_udom.m_t[1] = u;
      v = S[i].y;
      if      (v < srf_vdom.m_t[0]) srf_vdom.m_t[0] = v;
      else if (v > srf_vdom.m_t[1]) srf_vdom.m_t[1] = v;
    }
    if (    !srf_udom.IsIncreasing()
         || !srf_vdom.IsIncreasing() )
    {
      return false;
    }
  }

  bool bHaveUVWXform =   mapping.m_uvw.IsValid()
                     && !mapping.m_uvw.IsIdentity()
                     && !mapping.m_uvw.IsZero();

  if ( mesh.HasPackedTextureRegion() )
  {
    // Packed textures are not compatible with the use
    // of m_uvw.  m_uvw is ignored in this block
    // of code on purpose.  //SEE BELOW
    const MYON_Interval tex_udom = mesh.m_packed_tex_domain[0];
    const MYON_Interval tex_vdom = mesh.m_packed_tex_domain[1];
    for ( i = 0; i < vcnt; i++)
    {
	    u = S[i].x;
	    v = S[i].y;

	    // (u, v) = known surface parameter
	    if ( mesh.m_packed_tex_rotate )
	    {
        // verify this by checking with mesher
	       a = 1.0 - srf_vdom.NormalizedParameterAt( v );
	       b = srf_udom.NormalizedParameterAt( u );
	    }
	    else
	    {
	      a = srf_udom.NormalizedParameterAt( u );
	      b = srf_vdom.NormalizedParameterAt( v );
	    }

      // When textures are packed, tex_udom and tex_vdom
      // are subintervals of (0,1).
	    u = tex_udom.ParameterAt(a);
	    v = tex_vdom.ParameterAt(b);

	    // Jussi, Aug 18 2022, RH-69752: Apply uvw transform in texture space
	    if (bHaveUVWXform)
	    {
	      const MYON_2dPoint si = mapping.m_uvw * MYON_2dPoint(u, v);
	      u = si.x;
	      v = si.y;
	    }

	    tc[0] = (float)u;
	    tc[1] = (float)v;
      tc += tc_stride;
    }
  }
  else if ( bHaveUVWXform )
  {
    const MYON_Xform xform(mapping.m_uvw);
    MYON_3dPoint P;
    for ( i = 0; i < vcnt; i++)
    {
	    // normalize surface parameter
      P.x = srf_udom.NormalizedParameterAt( S[i].x );
      P.y = srf_vdom.NormalizedParameterAt( S[i].y );
      P.z = 0.0;

      // apply m_uvw transformation
      P = xform*P;

      tc[0] = (float)P.x;
      tc[1] = (float)P.y;
      tc += tc_stride;
    }
  }
  else
  {
    // m_srf_tex_rotate is ignored on purpose.
    // It only applies if the texture is packed.
    for ( i = 0; i < vcnt; i++)
    {
	    // tc = normalized surface parameter
      a = srf_udom.NormalizedParameterAt( S[i].x );
      b = srf_vdom.NormalizedParameterAt( S[i].y );

	    tc[0] = (float)a;
	    tc[1] = (float)b;
      tc += tc_stride;
    }
  }

  return true;
}

#if !defined(OPENNURBS_PLUS)
class MYON__MTCBDATA
{
  // DO NOT PUT THIS CLASS IN A HEADER FILE
  // IT IS A PRIVATE HELPER CLASS.
public:
  const MYON_Mesh* m_pMesh;
  MYON_3dPoint m_pt;
  MYON_RTreeSphere m_sphere;
  int m_face_index;
  double m_t[4];
  double m_dist;
  bool m_bRestart;
};

bool MYON_CALLBACK_CDECL MTCB(void* a_context, MYON__INT_PTR a_id)
{
  MYON__MTCBDATA* pData = (MYON__MTCBDATA*)a_context;
  if (nullptr == pData)
    return true;
  pData->m_bRestart = false;
  const int fi = (int)a_id;
  if (fi < 0 || fi >= pData->m_pMesh->m_F.Count())
    return true;
  const MYON_MeshFace& face = pData->m_pMesh->m_F[fi];
  const int fcc = face.IsTriangle() ? 3 : 4;
  for (int fci = 0; fci < fcc; fci++)
  {
    // Distance to vertex at corner fci
    const MYON_3dPoint ptV = pData->m_pMesh->Vertex(face.vi[fci]);
    const double distV = ptV.DistanceTo(pData->m_pt);
    if (distV < pData->m_dist)
    {
      pData->m_dist = distV;
      pData->m_face_index = fi;
      pData->m_t[0] = pData->m_t[1] = pData->m_t[2] = pData->m_t[3] = 0.0;
      pData->m_t[fci] = 1.0;
    }

    // Distance to edge between corners fci-1 and fci
    const int fcip = (fci + fcc - 1) % fcc;
    const MYON_3dPoint ptPV = pData->m_pMesh->Vertex(face.vi[fcip]);
    MYON_Line edge(ptPV, ptV);
    double t = 0.0;
    if (edge.ClosestPointTo(pData->m_pt, &t))
    {
      if (t >= 0.0 && t <= 1.0)
      {
        const MYON_3dPoint ptE = edge.PointAt(t);
        const double distE = ptE.DistanceTo(pData->m_pt);
        if (distE < pData->m_dist)
        {
          pData->m_dist = distE;
          pData->m_face_index = fi;
          pData->m_t[0] = pData->m_t[1] = pData->m_t[2] = pData->m_t[3] = 0.0;
          pData->m_t[fci] = t;
          pData->m_t[fcip] = 1.0 - t;
        }
      }
    }
  }

  int tris = 1;
  MYON_3dex fcis[2] = { {0, 1, 2}, MYON_3dex::Unset };
  //int fcis[2][3] = {{0, 1, 2}, {MYON_UNSET_INT_INDEX, MYON_UNSET_INT_INDEX, MYON_UNSET_INT_INDEX}};
  if (face.IsQuad())
  {
    fcis[1] = { 0, 3, 2 };
    tris = 2;
  }

  for (int ti = 0; ti < tris; ti++)
  {
    const int vis[3] = { face.vi[fcis[ti].i] , face.vi[fcis[ti].j], face.vi[fcis[ti].k] };
    const MYON_3dPoint pts[3] = { pData->m_pMesh->Vertex(vis[0]), pData->m_pMesh->Vertex(vis[1]), pData->m_pMesh->Vertex(vis[2]) };
    double t0 = 0.0, t1 = 0.0;
    if (ProjectToFaceSpace(pts[0], pts[1], pts[2], pData->m_pt, t0, t1))
    {
      if (0 <= t0 && 0 <= t1 && t0 + t1 <= 1.0)
      {
        const MYON_3dPoint ptF = pts[0] + t0 * (pts[1] - pts[0]) + t1 * (pts[2] - pts[0]);
        const double distF = ptF.DistanceTo(pData->m_pt);
        if (distF < pData->m_dist)
        {
          pData->m_dist = distF;
          pData->m_face_index = fi;
          pData->m_t[0] = pData->m_t[1] = pData->m_t[2] = pData->m_t[3] = 0.0;
          pData->m_t[fcis[ti].i] = 1.0 - t0 - t1;
          pData->m_t[fcis[ti].j] = t0;
          pData->m_t[fcis[ti].k] = t1;
        }
      }
    }
  }

  if (pData->m_dist < pData->m_sphere.m_radius)
  {
    pData->m_sphere.m_radius = pData->m_dist;
    pData->m_bRestart = true;
    return false;
  }

  return true;
}
#endif

bool MYON_TextureMapping::GetTextureCoordinates(
          const MYON_Mesh& mesh,
          MYON_SimpleArray<MYON_3fPoint>& T,
          const MYON_Xform* mesh_xform,
          bool bLazy,
          MYON_SimpleArray<int>* Tside
          ) const
{
  if ( Tside )
    Tside->SetCount(0);

  int i;
  const int vcnt = mesh.m_V.Count();
  if ( vcnt <= 0 )
    return false;

  if ( bLazy )
  {
    int tci, tccount = mesh.m_TC.Count();
    for ( tci = 0; tci < tccount; tci++ )
    {
      if ( vcnt == mesh.m_TC[tci].m_T.Count() )
      {
        if ( HasMatchingTextureCoordinates(mesh.m_TC[tci].m_tag,mesh_xform) )
        {
          T = mesh.m_TC[tci].m_T;
          return true;
        }
      }
    }

    if ( HasMatchingTextureCoordinates(mesh,mesh_xform ) )
    {
      T.Reserve(vcnt);
      T.SetCount(vcnt);
      const MYON_2fPoint* f = mesh.m_T.Array();
      MYON_3fPoint* d = T.Array();
      for ( i = vcnt; i--; f++, d++ )
      {
        d->x = f->x;
        d->y = f->y;
        d->z = 0.0f;
      }
      return true;
    }
  }

	bool rc = false;

  if ( MYON_TextureMapping::TYPE::srfp_mapping == m_type )
  {
    // uv textures from surface parameterization
    T.Reserve(vcnt);
    T.SetCount(vcnt);
    T.Zero();
    rc = GetSPTCHelper(mesh,*this,&T[0].x,3);
	}
  else
  {
    MYON_3dPoint  P, tc;
		MYON_3dVector N(0.0,0.0,0.0);

		const MYON_3fPoint*  mesh_V = mesh.m_V.Array();
		const MYON_3fVector* mesh_N = mesh.HasVertexNormals()
                              ? mesh.m_N.Array()
                              : 0;

    T.Reserve(vcnt);
    T.SetCount(vcnt);

    int* Tsd = 0;
    if ( Tside )
    {
      Tside->Reserve(vcnt);
      Tside->SetCount(vcnt);
      Tsd = Tside->Array();
      memset(Tsd,0,vcnt*sizeof(Tsd[0]));
    }

    MYON_Xform P_xform(MYON_Xform::IdentityTransformation), N_xform(MYON_Xform::IdentityTransformation);
    const double* PT = 0;
    const double* NT = 0;
    if ( mesh_xform )
    {
      if ( mesh_xform->IsZero() || mesh_xform->IsIdentity() )
      {
        // ignore transformation
        mesh_xform = 0;
      }
      else if ( 0.0 != mesh_xform->GetMappingXforms(P_xform,N_xform) )
      {
        PT = &P_xform[0][0];
        NT = &N_xform[0][0];
      }
      else
      {
        mesh_xform = 0;
      }
    }

    const float* f;
    double w;
    int sd;

		if (MYON_TextureMapping::PROJECTION::clspt_projection == m_projection
      && (MYON_TextureMapping::TYPE::mesh_mapping_primitive == m_type || MYON_TextureMapping::TYPE::brep_mapping_primitive == m_type)
      && nullptr != m_mapping_primitive)
		{
      rc = false;
      if (MYON_TextureMapping::TYPE::mesh_mapping_primitive == m_type)
      {
        const MYON_Xform matP = nullptr != PT ? m_Pxyz * MYON_Xform(PT) : m_Pxyz;
        const MYON_Xform matN = nullptr != NT ? m_Nxyz * MYON_Xform(NT) : m_Nxyz;

        const MYON_Mesh* pMesh = CustomMappingMeshPrimitive();
        if (nullptr != pMesh)
        {
          MYON_RTree faceTree;
          if (faceTree.CreateMeshFaceTree(pMesh))
          {
            for (int vi = 0; vi < mesh.VertexCount(); vi++)
            {
              T[vi] = MYON_3fPoint::Origin;

              const MYON_3dPoint vtx(mesh.m_V[vi]);
              const MYON_3dPoint ptV = matP * vtx;
              const MYON_3fVector vtVN = mesh.HasVertexNormals() ? MYON_3fVector(matN * MYON_3dVector(mesh.m_N[vi])) : MYON_3fVector::ZeroVector;

              MYON__MTCBDATA data;
              data.m_sphere.m_point[0] = ptV.x;
              data.m_sphere.m_point[1] = ptV.y;
              data.m_sphere.m_point[2] = ptV.z;
              data.m_sphere.m_radius = pMesh->BoundingBox().FarPoint(ptV).DistanceTo(ptV);
              data.m_pMesh = pMesh;
              data.m_pt = ptV;
              data.m_dist = DBL_MAX;
              do
              {
                faceTree.Search(&data.m_sphere, MTCB, (void*)&data);
              } while (data.m_bRestart);
              if (0 <= data.m_face_index && data.m_face_index < pMesh->m_F.Count())
              {
                const MYON_MeshFace& face = pMesh->m_F[data.m_face_index];
                const MYON_3dPoint ptUV =
                  data.m_t[0] * MYON_3dPoint(pMesh->m_T[face.vi[0]]) +
                  data.m_t[1] * MYON_3dPoint(pMesh->m_T[face.vi[1]]) +
                  data.m_t[2] * MYON_3dPoint(pMesh->m_T[face.vi[2]]) +
                  data.m_t[3] * MYON_3dPoint(pMesh->m_T[face.vi[3]]);
                const MYON_3dPoint vtc = m_uvw * ptUV;
                T[vi].Set((float)vtc.x, (float)vtc.y, 0.0f);
              }
            }
          }
        }
      }
		}
		else if ( mesh_N &&
          (   MYON_TextureMapping::PROJECTION::ray_projection == m_projection
           || MYON_TextureMapping::TYPE::box_mapping == m_type
           || MYON_TextureMapping::TYPE::cylinder_mapping == m_type
           || MYON_TextureMapping::TYPE::mesh_mapping_primitive == m_type
		   )
        )
  	{
			// calculation uses mesh vertex normal
      if ( PT && NT )
      {
        // need to transform vertex and normal
        // before calculating texture coordinates
			  for (i = 0; i < vcnt; i++)
			  {
          f = &mesh_V[i].x;
				  w = PT[12]*f[0] + PT[13]*f[1] + PT[14]*f[2] + PT[15];
          w = (0.0 != w) ? 1.0/w : 1.0;
				  P.x = w*(PT[0]*f[0] + PT[1]*f[1] + PT[ 2]*f[2] + PT[ 3]);
				  P.y = w*(PT[4]*f[0] + PT[5]*f[1] + PT[ 6]*f[2] + PT[ 7]);
				  P.z = w*(PT[8]*f[0] + PT[9]*f[1] + PT[10]*f[2] + PT[11]);

          f = &mesh_N[i].x;
          N.x = PT[0]*f[0] + PT[1]*f[1] + PT[ 2]*f[2];
				  N.y = PT[4]*f[0] + PT[5]*f[1] + PT[ 6]*f[2];
				  N.z = PT[8]*f[0] + PT[9]*f[1] + PT[10]*f[2];
          N.Unitize();
				  sd = Evaluate(P,N,&tc);
				  T[i] = tc;
          if ( Tsd ) Tsd[i] = sd;
			  }
      }
			else
      {
        // mesh vertex and normal are ok
			  for (i = 0; i < vcnt; i++)
			  {
				  P = mesh_V[i];
				  N = mesh_N[i];
				  sd = Evaluate(P,N,&tc);
				  T[i] = tc;
          if ( Tsd ) Tsd[i] = sd;
			  }
      }
		}
		else if ( PT )
    {
      // normal is not used
      // mesh vertex needs to be transformed
      for ( i = 0; i < vcnt; i++ )
      {
        f = &mesh_V[i].x;
			  w = PT[12]*f[0] + PT[13]*f[1] + PT[14]*f[2] + PT[15];
        w = (0.0 != w) ? 1.0/w : 1.0;
			  P.x = w*(PT[0]*f[0] + PT[1]*f[1] + PT[ 2]*f[2] + PT[ 3]);
			  P.y = w*(PT[4]*f[0] + PT[5]*f[1] + PT[ 6]*f[2] + PT[ 7]);
			  P.z = w*(PT[8]*f[0] + PT[9]*f[1] + PT[10]*f[2] + PT[11]);
        sd = Evaluate(P,N,&tc);
			  T[i] = tc;
        if ( Tsd )
          Tsd[i] = sd;
		  }
    }
    else
		{
			// normal is not used and mesh vertex is ok
			for ( i = 0; i < vcnt; i++ )
			{
				P = mesh_V[i];
				sd = Evaluate(P,N,&tc);
				T[i] = tc;
				if ( Tsd )
					Tsd[i] = sd;
			}
    }
    rc = true;
	}

	return rc;
}

static
void ThreeToTwoHelper(
      const MYON_SimpleArray<MYON_3fPoint>& T3,
      MYON_SimpleArray<MYON_2fPoint>& T2
      )
{
  int i = T3.Count();
  const MYON_3fPoint* t3 = T3.Array();

  T2.Reserve(i);
  T2.SetCount(i);
  MYON_2fPoint* t2 = T2.Array();
  while(i--)
  {
    t2->x = t3->x;
    t2->y = t3->y;
    t2++;
    t3++;
  }
}

bool MYON_TextureMapping::GetTextureCoordinates(
            const MYON_Mesh& mesh,
            MYON_SimpleArray<MYON_2fPoint>& T,
            const MYON_Xform* mesh_xform,
            bool bLazy,
            MYON_SimpleArray<int>* Tside
            ) const
{
  bool rc = false;
  if ( Tside )
    Tside->SetCount(0);
  if ( bLazy )
  {
    if ( HasMatchingTextureCoordinates(mesh,mesh_xform ) )
    {
      if ( T.Array() != mesh.m_T.Array() )
      {
        // different arrays - copy
        T = mesh.m_T;
      }
      return true;
    }
    else
    {
      int vcnt = mesh.m_V.Count();
      int tci, tccount = mesh.m_TC.Count();
      for ( tci = 0; tci < tccount; tci++ )
      {
        if ( vcnt == mesh.m_TC[tci].m_T.Count() )
        {
          if ( HasMatchingTextureCoordinates(mesh.m_TC[tci].m_tag,mesh_xform) )
          {
            // copy T3d[] results to T[]
            ThreeToTwoHelper(mesh.m_TC[tci].m_T,T);
            return true;
          }
        }
      }
    }
  }

  if ( MYON_TextureMapping::TYPE::srfp_mapping == m_type )
  {
    if (mesh.HasSurfaceParameters())
    {
      // uv textures from surface parameterization
      T.Reserve(mesh.m_V.Count());
      T.SetCount(mesh.m_V.Count());
      T.Zero();
      rc = GetSPTCHelper(mesh, *this, &T[0].x, 2);
    }
    else
    {
      //In this case, we're just going to leave the TC array in place
      rc = false;
    }
  }
  else
  {
    T.SetCount(0);
	  MYON_SimpleArray<MYON_3fPoint> T3;
    if ( GetTextureCoordinates(mesh, T3, mesh_xform, false, Tside ) )
    {
      // copy T3d[] results to T[]
      ThreeToTwoHelper(T3,T);
      rc = true;
	  }
  }
	return rc;
}


//bool MYON_Mesh::GetSurfaceParameterTextureXform(
//          class MYON_Xform& StoT
//          ) const
//
//{
//  bool rc = false;
//  StoT.Identity();
//
//  // Gets default mesh mapping
//  const MYON_Interval surface_u_domain(m_srf_domain[0]);
//  const MYON_Interval surface_v_domain(m_srf_domain[1]);
//  const MYON_Interval texture_u_domain(m_tex_domain[0]);
//  const MYON_Interval texture_v_domain(m_tex_domain[1]);
//  bool bRotateTexture = m_srf_tex_rotate;
//  if (   surface_u_domain.IsInterval()
//      && surface_v_domain.IsInterval()
//      && texture_u_domain.IsInterval()
//      && texture_v_domain.IsInterval()
//      )
//  {
//    double du = 1.0/surface_u_domain.Length();
//    double dv = 1.0/surface_v_domain.Length();
//    MYON_Xform x1(1.0), x2(1.0), x3(1.0);
//    x1.m_xform[0][0] = du; x1.m_xform[0][3] = -surface_u_domain[0]*du;
//    x1.m_xform[1][1] = dv; x1.m_xform[1][3] = -surface_v_domain[0]*dv;
//    if ( bRotateTexture )
//    {
//      x2.m_xform[0][0] =  0.0; x2.m_xform[0][1] = -1.0; x2.m_xform[0][3] = 1.0;
//      x2.m_xform[1][0] =  1.0; x2.m_xform[1][1] =  0.0;
//    }
//    x3.m_xform[0][0] = texture_u_domain.Length(); x3.m_xform[0][3] = texture_u_domain[0];
//    x3.m_xform[1][1] = texture_v_domain.Length(); x3.m_xform[1][3] = texture_v_domain[0];
//
//    // transforms surface(u,v) to texture(u,v)
//    StoT = x3*x2*x1;
//
//    rc = true;
//  }
//  return rc;
//}

class MYON__CMeshFaceTC
{
  // DO NOT PUT THIS CLASS IN A HEADER FILE
  // IT IS A PRIVATE HELPER CLASS.
public:
  int   fi;
  int   quad[4];
  float Tx[4];
  bool  bSetT[4];
};

class MYON__CChangeTextureCoordinateHelper
{
  // DO NOT PUT THIS CLASS IN A HEADER FILE
  // IT IS A PRIVATE HELPER CLASS.
public:
  MYON__CChangeTextureCoordinateHelper( MYON_Mesh& mesh, int newvcnt, float*& mesh_T );
  ~MYON__CChangeTextureCoordinateHelper();

  int DupVertex(int vi);
  void ChangeTextureCoordinate(int* Fvi, int fvi, float x, float y, float* mesh_T, int mesh_T_stride );

  int m_tci;

  MYON_Mesh& m_mesh;
  MYON_3dPointArray* m_mesh_dV = nullptr;
  bool m_bHasVertexNormals;
  bool m_bHasVertexTextures;
  bool m_bHasVertexColors;
  bool m_bHasSurfaceParameters;
  bool m_bHasPrincipalCurvatures;
  bool m_bHasHiddenVertices;

  bool m_bHasCachedTextures;
  MYON_SimpleArray< MYON_TextureCoordinates* > m_TC;

  // m_vuse[] is an array of length = original number of
  // vertices in m_mesh and m_vuse[vi] = number of faces
  // that reference vertex vi. If this vertex needs to be
  // split, vuse[vi] is decremented.  The ultimate goal
  // is to split a few times as needed so we don't
  // bloat the mesh with repeated calls to changing
  // texture maps. m_vuse[] is set the first time
  // DupVertex() is called.
  int m_vuse_count;
  MYON_SimpleArray< unsigned int > m_vuse;
private:
  // no implementation
  MYON__CChangeTextureCoordinateHelper(const MYON__CChangeTextureCoordinateHelper&);
  MYON__CChangeTextureCoordinateHelper& operator=(const MYON__CChangeTextureCoordinateHelper&);
};

void MYON__CChangeTextureCoordinateHelper::ChangeTextureCoordinate(int* Fvi, int fvi, float x, float y,
                                                             float* mesh_T, int mesh_T_stride )
{
  int oldvi = Fvi[fvi];
  float* T = mesh_T+(oldvi*mesh_T_stride);
  if ( x != T[0] || (y != MYON_UNSET_FLOAT && y != T[1]) )
  {
    int newvi = DupVertex(oldvi);
    T = mesh_T + (newvi*mesh_T_stride);
    T[0] = x;
    if ( y != MYON_UNSET_FLOAT )
     T[1] = y;

    if ( 2 == fvi && oldvi == Fvi[3] )
    {
      Fvi[2] = newvi;
      Fvi[3] = newvi;
    }
    else
    {
      Fvi[fvi] = newvi;
    }
  }
}


MYON__CChangeTextureCoordinateHelper::MYON__CChangeTextureCoordinateHelper(
    MYON_Mesh& mesh,
    int newvcnt,
    float*& mesh_T )
: m_mesh(mesh)
, m_mesh_dV(0)
, m_vuse_count(0)
{
  // adding vertices invalidates this cached information.
  m_mesh.DestroyTopology();
  m_mesh.DestroyPartition();
  m_mesh.DestroyTree();

  m_tci = -1;

  const int vcnt = m_mesh.m_V.Count();

  // It is critical to reserve enough room in the arrays
  // before duplication starts.  Otherwise, during duplication,
  // a dyanamic array can be reallocated, which will make
  // saved array base pointers will be invalid, and you crash
  // the next time they are used.

  m_mesh.m_V.Reserve(vcnt+newvcnt);

  if ( m_mesh.HasDoublePrecisionVertices() )
  {
    m_mesh_dV = &m_mesh.m_dV;
    m_mesh_dV->Reserve(vcnt+newvcnt);
  }
  else
  {
    m_mesh.DestroyDoublePrecisionVertices();
  }

  m_bHasVertexNormals = m_mesh.HasVertexNormals();
  if ( m_bHasVertexNormals )
    m_mesh.m_N.Reserve(vcnt+newvcnt);

  m_bHasVertexTextures = m_mesh.HasTextureCoordinates();
  if ( m_bHasVertexTextures )
  {
    float* p = (float*)m_mesh.m_T.Array();
    m_mesh.m_T.Reserve(vcnt+newvcnt);
    if ( p == mesh_T )
      mesh_T = (float*)m_mesh.m_T.Array();
  }

  m_bHasVertexColors = m_mesh.HasVertexColors();
  if ( m_bHasVertexColors )
    m_mesh.m_C.Reserve(vcnt+newvcnt);

  m_bHasSurfaceParameters = m_mesh.HasSurfaceParameters();
  if ( m_bHasSurfaceParameters )
    m_mesh.m_S.Reserve(vcnt+newvcnt);

  m_bHasPrincipalCurvatures = m_mesh.HasPrincipalCurvatures();
  if ( m_bHasPrincipalCurvatures )
    m_mesh.m_K.Reserve(vcnt+newvcnt);

  m_bHasHiddenVertices = (0 != m_mesh.HiddenVertexArray());
  if ( m_bHasHiddenVertices )
    m_mesh.m_H.Reserve(vcnt+newvcnt);

  // Set m_TC[] to be the subset of m_mesh.m_TC[] that is
  // valid for duplication.
  m_bHasCachedTextures = false;
  int tci, tccount = m_mesh.m_TC.Count();
  m_TC.Reserve(tccount);
  for ( tci = 0; tci < tccount; tci++ )
  {
    MYON_TextureCoordinates& tc = m_mesh.m_TC[tci];
    if ( vcnt == tc.m_T.Count() )
    {
      m_bHasCachedTextures = true;
      float* p = (float*)tc.m_T.Array();
      tc.m_T.Reserve(vcnt+newvcnt);
      if ( p == mesh_T )
        mesh_T = (float*)tc.m_T.Array();
      m_TC.Append( &tc );
    }
  }
}


MYON__CChangeTextureCoordinateHelper::~MYON__CChangeTextureCoordinateHelper()
{
  if ( nullptr != m_mesh_dV )
  {
    m_mesh_dV = 0;
  }
}

int MYON__CChangeTextureCoordinateHelper::DupVertex(int vi)
{
  if ( 0 == m_vuse_count )
  {
    // m_vuse[] is an array of length = original number of
    // vertices in m_mesh and m_vuse[vi] = number of faces
    // that reference vertex vi. If this vertex needs to be
    // split, vuse[vi] is decremented.  The ultimate goal
    // is to split a few times as needed so we don't
    // bloat the mesh with repeated calls to changing
    // texture maps. m_vuse[] is set the first time
    // DupVertex() is called.
    m_vuse_count = m_mesh.m_V.Count();
    m_vuse.Reserve(m_vuse_count);
    m_vuse.SetCount(m_vuse_count);
    m_vuse.Zero();
    for ( int fi = 0; fi < m_mesh.m_F.Count(); fi++ )
    {
      const int* Fvi = m_mesh.m_F[fi].vi;
      int i = Fvi[0];
      if ( i >= 0 && i < m_vuse_count )
        m_vuse[i]++;
      i = Fvi[1];
      if ( i >= 0 && i < m_vuse_count )
        m_vuse[i]++;
      i = Fvi[2];
      if ( i >= 0 && i < m_vuse_count )
        m_vuse[i]++;
      i = Fvi[3];
      if ( Fvi[2] != i && i >= 0 && i < m_vuse_count )
        m_vuse[i]++;
    }
  }

  if ( vi >= 0 && vi < m_vuse_count )
  {
    if ( m_vuse[vi] <= 1 )
      return vi; // only one face uses this vertex - no need to dup the vertex

    // otherwise we will duplicate this vertex, reducing its use count by 1.
    m_vuse[vi]--;
  }


  m_mesh.m_V.AppendNew();
  *m_mesh.m_V.Last() = m_mesh.m_V[vi];
  if ( 0 != m_mesh_dV )
  {
    m_mesh_dV->AppendNew();
    *(m_mesh_dV->Last()) = m_mesh_dV->operator[](vi);
  }
  if ( m_bHasVertexTextures )
  {
    m_mesh.m_T.AppendNew();
    *m_mesh.m_T.Last() = m_mesh.m_T[vi];
  }
  if ( m_bHasVertexNormals )
  {
    m_mesh.m_N.AppendNew();
    *m_mesh.m_N.Last() = m_mesh.m_N[vi];
  }
  if ( m_bHasVertexColors )
  {
    m_mesh.m_C.AppendNew();
    *m_mesh.m_C.Last() = m_mesh.m_C[vi];
  }
  if ( m_bHasSurfaceParameters )
  {
    m_mesh.m_S.AppendNew();
    *m_mesh.m_S.Last() = m_mesh.m_S[vi];
  }
  if ( m_bHasPrincipalCurvatures )
  {
    m_mesh.m_K.AppendNew();
    *m_mesh.m_K.Last() = m_mesh.m_K[vi];
  }
  if ( m_bHasHiddenVertices )
  {
    m_mesh.m_H.AppendNew();
    if ( 0 != (*m_mesh.m_H.Last() = m_mesh.m_H[vi]) )
      m_mesh.m_hidden_count++;
  }

  if ( m_bHasCachedTextures )
  {
    // Note:  This m_TC[] is the subset of m_mesh.m_TC[]
    //        that need to be duped.  The constructor
    //        insures that m_TC[i] is not nullptr and
    //        has the right count and capacity.
    //
    //        DO NOT REFERENCE m_mesh.m_TC[] in this block.
    int tccount = m_TC.Count();
    for ( int i = 0; i < tccount; i++ )
    {
      MYON_SimpleArray<MYON_3fPoint>& T = m_TC[i]->m_T;
      T.AppendNew();
      *T.Last() = T[vi];
    }
  }

  return m_mesh.m_V.Count()-1;
}


static
float PoleFix( float t0,  float t1 )
{
  float t = ( MYON_UNSET_FLOAT == t0 )
          ? t1
          : ((MYON_UNSET_FLOAT == t1 ) ? t0 : (0.5f*(t0+t1)));
  return t;
}

static
int IntersectBoxSideRayHelper(int side, const MYON_3dPoint& rst, const MYON_3dVector& n, double* s)
{
  /*
  returns:
    0 = ray parallel to sides
    1 = ray hit left side (x=-1)
    2 = ray hit right side (x=+1)
    3 = ray hit back side (y=-1)
    4 = ray hit front side (y=+1)
    5 = ray hit bottom side (z=-1)
    6 = ray hit top side (z=+1)
  */
  double nx;
  MYON_3dPoint Q;
  double t,t0,t1;
  int dir;


  switch(side)
  {
  case 1: // =  left side (x=-1)
    t1 = -1.0;
    dir = 0;
    break;
  case 2: //   right side (x=+1)
    t1 = 1.0;
    dir = 0;
    break;
  case 3: //   back side (y=-1)
    t1 = -1.0;
    dir = 1;
    break;
  case 4: //   front side (y=+1)
    t1 = 1.0;
    dir = 1;
    break;
  case 5: //   bottom side (z=-1)
    t1 = -1.0;
    dir = 2;
    break;
  case 6: //   top side (z=+1)
    t1 = 1.0;
    dir = 2;
    break;
  default:
    *s = MYON_UNSET_VALUE;
    return 0;
    break;
  }

  // protect against overflow
  nx = (&n.x)[dir];
  t0 = (t1 - (&rst.x)[dir]);
  if ( fabs(t0) >= fabs(nx)*on__overflow_tol )
  {
    *s = MYON_UNSET_VALUE;
    return 0;
  }

  t0 /= nx;
  Q = rst + t0*n;
  if ( dir )
  {
    t = Q.x;
    Q.x = Q[dir];
    Q[dir] = t;
  }
  if ( fabs(Q.x-t1) > MYON_SQRT_EPSILON || fabs(Q.y) > 1.0e8 || fabs(Q.z) > 1.0e8 )
  {
    *s = MYON_UNSET_VALUE;
    return 0;
  }


  *s = t0;
  return side;
}

static
bool EvBoxSideTextureCoordinateHelper2(
                       int side,
                       const MYON_TextureMapping& box_mapping,
										   const MYON_3dPoint& P,
										   const MYON_3dVector& N,
										   MYON_3dPoint* T
										   )
{
  // side flag
  //  1 =  left side (x=-1)
  //  2 =  right side (x=+1)
  //  3 =  back side (y=-1)
  //  4 =  front side (y=+1)
  //  5 =  bottom side (z=-1)
  //  6 =  top side (z=+1)
  // The matrix m_Pxyz transforms the world coordinate
  // "mapping cylinder" into the cylinder centered at
  // rst = (0,0,0) with radius 1.0.  The axis runs
  // from rst = (0,0,-1) to rst = (0,0,+1).

  MYON_3dPoint rst(box_mapping.m_Pxyz*P);

	MYON_3dVector n(box_mapping.m_Nxyz*N);
  n.Unitize();

  // side flag
  //  1 =  left side (x=-1)
  //  2 =  right side (x=+1)
  //  3 =  back side (y=-1)
  //  4 =  front side (y=+1)
  //  5 =  bottom side (z=-1)
  //  6 =  top side (z=+1)

  if ( MYON_TextureMapping::PROJECTION::ray_projection == box_mapping.m_projection )
	{
    double s;
    if ( side == IntersectBoxSideRayHelper(side, rst, n, &s) )
    {
		  // ray hit the box side
		  rst = rst + s*n;
    }
	}

	double shift = 0.0;

  // side flag
  //  1 =  left side (x=-1)
  //  2 =  right side (x=+1)
  //  3 =  back side (y=-1)
  //  4 =  front side (y=+1)
  //  5 =  bottom side (z=-1)
  //  6 =  top side (z=+1)

	switch(side)
	{
	case 1: // x = -1
		rst.x = -rst.y;
		rst.y =  rst.z;
		shift =  3.0;
		break;
	case 2: // x = +1
		rst.x =  rst.y;
		rst.y =  rst.z;
		shift =  1.0;
		break;
	case 3: // y = -1
		rst.y =  rst.z;
		shift =  0.0;
		break;
	case 4: // y = +1
		rst.x = -rst.x;
		rst.y =  rst.z;
		shift =  2.0;
		break;
	case 5: // z = -1
		rst.x = -rst.x;
		shift =  4.0;
		break;
	case 6: // z = +1
		shift =  5.0;
		break;
  default:
    return 0;
    break;
	}

  // normalize texture coordinates
  rst.x = 0.5*rst.x + 0.5;
  rst.y = 0.5*rst.y + 0.5;
	rst.z = 0.0;

	if( MYON_TextureMapping::TEXTURE_SPACE::divided == box_mapping.m_texture_space)
	{
    rst.x = (shift + rst.x)/(box_mapping.m_bCapped ? 6.0 : 4.0);
	}

	*T = box_mapping.m_uvw*rst;

  return true;
}

static
bool EvBoxSideTextureCoordinateHelper1(
          const MYON_Mesh& mesh,
          const MYON_Xform* mesh_xform,
          int vi,
          int side,
          const MYON_TextureMapping& box_mapping,
          float* Tx,
          float* Ty
          )
{
	bool rc = false;
  MYON_3dPoint  P, tc;
	MYON_3dVector N(0.0,0.0,0.0);

	const MYON_3fPoint*  mesh_V = mesh.m_V.Array();
	const MYON_3fVector* mesh_N = mesh.HasVertexNormals()
                            ? mesh.m_N.Array()
                            : 0;

  MYON_Xform P_xform(MYON_Xform::IdentityTransformation), N_xform(MYON_Xform::IdentityTransformation);
  const double* PT = 0;
  const double* NT = 0;
  if ( mesh_xform )
  {
    if ( mesh_xform->IsZero() || mesh_xform->IsIdentity() )
    {
      // ignore transformation
      mesh_xform = 0;
    }
    else if ( 0.0 != mesh_xform->GetMappingXforms(P_xform,N_xform) )
    {
      PT = &P_xform[0][0];
      NT = &N_xform[0][0];
    }
    else
    {
      mesh_xform = 0;
    }
  }

  const float* f;
  double w;

  if ( mesh_N && MYON_TextureMapping::PROJECTION::ray_projection == box_mapping.m_projection )
	{
		// calculation uses mesh vertex normal
    if ( PT && NT )
    {
      // need to transform vertex and normal
      // before calculating texture coordinates
      f = &mesh_V[vi].x;
		  w = PT[12]*f[0] + PT[13]*f[1] + PT[14]*f[2] + PT[15];
      w = (0.0 != w) ? 1.0/w : 1.0;
		  P.x = w*(PT[0]*f[0] + PT[1]*f[1] + PT[ 2]*f[2] + PT[ 3]);
		  P.y = w*(PT[4]*f[0] + PT[5]*f[1] + PT[ 6]*f[2] + PT[ 7]);
		  P.z = w*(PT[8]*f[0] + PT[9]*f[1] + PT[10]*f[2] + PT[11]);

      f = &mesh_N[vi].x;
      N.x = PT[0]*f[0] + PT[1]*f[1] + PT[ 2]*f[2];
		  N.y = PT[4]*f[0] + PT[5]*f[1] + PT[ 6]*f[2];
		  N.z = PT[8]*f[0] + PT[9]*f[1] + PT[10]*f[2];
      N.Unitize();
    }
    else
    {
      // mesh vertex and normal are ok
		  P = mesh_V[vi];
		  N = mesh_N[vi];
    }
	}
	else if ( PT )
  {
    // normal is not used
    // mesh vertex needs to be transformed
    f = &mesh_V[vi].x;
	  w = PT[12]*f[0] + PT[13]*f[1] + PT[14]*f[2] + PT[15];
    w = (0.0 != w) ? 1.0/w : 1.0;
	  P.x = w*(PT[0]*f[0] + PT[1]*f[1] + PT[ 2]*f[2] + PT[ 3]);
	  P.y = w*(PT[4]*f[0] + PT[5]*f[1] + PT[ 6]*f[2] + PT[ 7]);
	  P.z = w*(PT[8]*f[0] + PT[9]*f[1] + PT[10]*f[2] + PT[11]);
  }
  else
  {
    // normal is not used and mesh vertex is ok
    P = mesh_V[vi];
  }


  rc = EvBoxSideTextureCoordinateHelper2(side,box_mapping,P,N,&tc);
  if (rc)
  {
    rc = tc.IsValid();
    if (rc)
    {
      *Tx = (float)tc.x;
      *Ty = (float)tc.y;
    }
  }
	return rc;
}


class MYON__CNewMeshFace
{
public:
  int fi;
  int newvcnt;
  bool bNewV[4];
  MYON_2fPoint tc[4];
};

static
float TcDistanceHelper(const MYON_2fPoint& tc)
{
  float dx = (tc.x > 0.5f) ? (1.0f-tc.x) : tc.x;
  if ( dx < 0.0f)
    return 0.0f;
  float dy = (tc.y > 0.5f) ? (1.0f-tc.y) : tc.y;
  if ( dy < 0.0f)
    return 0.0f;
  return (dx < dy) ? dx : dy;
}

static
void AdjustSingleBoxTextureCoordinatesHelper(
          MYON_Mesh& mesh,
          const MYON_Xform* mesh_xform,
          float* mesh_T,
          int    mesh_T_stride,
          const int* Tsd,
          const MYON_TextureMapping& box_mapping
          )
{
  const int vcnt = mesh.m_V.Count();
  const int fcnt = mesh.m_F.Count();
  if ( vcnt < 3 || fcnt < 1 || vcnt != mesh.m_T.Count() || !Tsd )
    return;
  const MYON_MeshFace* mesh_F = mesh.m_F.Array();
  const int* Fvi;
  int j, k, fi, sd[4], fvicnt, side, newvcnt=0;
  MYON__CNewMeshFace mf;
  MYON_2fPoint tc;
  MYON_SimpleArray<MYON__CNewMeshFace> mflist(512);
  float d;
  for ( fi = 0; fi < fcnt; fi++ )
  {
    Fvi = mesh_F[fi].vi;
    sd[0] = Tsd[Fvi[0]];
    sd[1] = Tsd[Fvi[1]];
    sd[2] = Tsd[Fvi[2]];
    sd[3] = Tsd[Fvi[3]];
    if ( sd[0] == sd[1] && sd[0] == sd[2] && sd[0] == sd[3] )
    {
      // all texture coords are on same side of box
      continue;
    }
    fvicnt = (Fvi[2] != Fvi[3]) ? 4 : 3;

    memset(&mf,0,sizeof(mf));
    mf.tc[0] = mesh_T + (Fvi[0]*mesh_T_stride);
    mf.tc[1] = mesh_T + (Fvi[1]*mesh_T_stride);
    mf.tc[2] = mesh_T + (Fvi[2]*mesh_T_stride);
    mf.tc[3] = mesh_T + (Fvi[3]*mesh_T_stride);

    // find the side we will use for this face
    side = sd[0];
    d = TcDistanceHelper(mf.tc[0]);
    for ( j = 1; j < fvicnt; j++ )
    {
      float d1 = TcDistanceHelper(mf.tc[j]);
      if (d1 > d)
      {
        side = sd[j];
        d = d1;
      }
    }

    // Jussi, 5th September 2011:
    // This 'continue' only works for faces having one or more of its tc's in (0,1)x(0,1).
    // I have commented it out as a fix to RR 90329.
    //if ( d <= 0.0f )
    //  continue;

    for ( j = 0; j < fvicnt; j++ )
    {
      if ( sd[j] != side )
      {
        // calculate new tc for this side
        if ( EvBoxSideTextureCoordinateHelper1(
          mesh,
          mesh_xform,
          Fvi[j],
          side,
          box_mapping,
          &tc.x,&tc.y) )
        {
          if ( tc.x != mf.tc[j].x || tc.y != mf.tc[j].y )
          {
            mf.tc[j] = tc;
            mf.bNewV[j] = true;
            mf.newvcnt++;
          }
        }
        else
          break;
      }
    }
    if ( j >= fvicnt && mf.newvcnt > 0 )
    {
      mf.fi = fi;
      newvcnt += mf.newvcnt;
      mflist.Append(mf);
    }
  }

  if ( newvcnt <= 0 )
    return;

  MYON__CChangeTextureCoordinateHelper helper(mesh,vcnt+newvcnt,mesh_T);

  const int mflist_count = mflist.Count();

  for ( k = 0; k < mflist_count; k++ )
  {
    mf = mflist[k];
    int* fvi = mesh.m_F[mf.fi].vi;
    fvicnt = (fvi[2]!=fvi[3]) ? 4 : 3;
    for ( j = 0; j < fvicnt; j++ )
    {
      if ( mf.bNewV[j] )
      {
        helper.ChangeTextureCoordinate(fvi,j,mf.tc[j].x,mf.tc[j].y,mesh_T,mesh_T_stride);
      }
    }
  }
}

static
void AdjustMeshPeriodicTextureCoordinatesHelper(
          MYON_Mesh& mesh,
          const MYON_Xform* mesh_xform,
          float* mesh_T,
          int    mesh_T_stride,
          const int* Tsd,
          double two_pi_tc,
          const MYON_TextureMapping& mapping
          )
{
  // This helper adjusts texture coordinates on faces that
  // span the seam on mapping spheres and cylinders and
  // resolves the mulitiple valued problem that
  // exists at the poles of sphere mappings.

  const int vcnt = mesh.m_V.Count();
  const int fcnt = mesh.m_F.Count();
  if ( vcnt < 3 || fcnt < 1 || vcnt != mesh.m_T.Count() )
    return;

  // see if any texture coordinate adjustment is necessary
  const MYON_TextureMapping::TYPE mapping_type = mapping.m_type;
  const bool bSphereCheck = ( MYON_TextureMapping::TYPE::sphere_mapping == mapping_type );
  const bool bCylinderCheck = (Tsd && MYON_TextureMapping::TYPE::cylinder_mapping == mapping_type);
  const bool bBoxCheck = (Tsd && MYON_TextureMapping::TYPE::box_mapping == mapping_type);

  if ( bBoxCheck && MYON_TextureMapping::TEXTURE_SPACE::single == mapping.m_texture_space )
  {
    AdjustSingleBoxTextureCoordinatesHelper( mesh, mesh_xform, mesh_T, mesh_T_stride, Tsd, mapping );
    return;
  }

  MYON_Workspace ws;
  int* quad = ws.GetIntMemory(vcnt); // ~ws will free quad memory
  float* Tx = (float*)ws.GetMemory(vcnt*sizeof(Tx[0]));
  float t;
  int vi, ti, q=0;
  int ftc_count = 0;

  const float ang0 = (float)(0.25*two_pi_tc);
  const float ang1 = (float)(0.75*two_pi_tc);


  for ( vi = ti = 0; vi < vcnt; vi++, ti += mesh_T_stride )
  {
    quad[vi] = 0;
    Tx[vi] = mesh_T[ti];
    if ( bCylinderCheck )
    {
      if ( 1 != Tsd[vi] )
        continue;
    }
    else if ( bBoxCheck )
    {
      if ( 1 != Tsd[vi] && 3 != Tsd[vi] )
        continue;
    }
    else if ( bSphereCheck )
    {
      t = mesh_T[ti+1]; // t = "v" texture coordinate
      if ( t < 0.001f )
      {
        quad[vi] = 8; q |= 8; // south pole point
        ftc_count++;
        continue;
      }
      if ( t > 0.999f )
      {
        quad[vi] = 8; q |= 8; // north pole point
        ftc_count++;
        continue;
      }
    }

    t = Tx[vi]; // t = "u" texture coordinate
    if ( t < ang0 )
    {
      quad[vi] = 1; q |= 1; // longitude < pi/2
      ftc_count++;
    }
    else if ( t > ang1 )
    {
      quad[vi] = 4; q |= 4; // longitude > 3pi/2
      ftc_count++;
    }
  }

  if ( 0 == q || 1 == q || 4 == q )
  {
    // nothing needs to be adjusted
    return;
  }

  // 4*ftc_count = (over) estimate of the number of faces that
  // will be changed.
  MYON_SimpleArray<MYON__CMeshFaceTC> ftc_list(ftc_count*4 + 128);
  ftc_count = 0;
  const MYON_MeshFace* F = mesh.m_F.Array();
  const int* Fvi;
  int fi;
  MYON__CMeshFaceTC ftc;
  memset(&ftc,0,sizeof(ftc));
  float t0, t1;

  for ( fi = 0; fi < fcnt; fi++ )
  {
    Fvi = F[fi].vi;

    ftc.quad[0] = quad[Fvi[0]];
    ftc.quad[1] = quad[Fvi[1]];
    ftc.quad[2] = quad[Fvi[2]];
    ftc.quad[3] = quad[Fvi[3]];

    q = (ftc.quad[0] | ftc.quad[1] | ftc.quad[2] | ftc.quad[3]);
    if ( 0 == q || 1 == q || 4 == q )
    {
      // no adjustments need to be made
      continue;
    }

    // ftc.fi will be set to fi if a texture coordinate needs to be adjusted
    ftc.fi = -1;

    ftc.Tx[0] = Tx[Fvi[0]];
    ftc.Tx[1] = Tx[Fvi[1]];
    ftc.Tx[2] = Tx[Fvi[2]];
    ftc.Tx[3] = Tx[Fvi[3]];

    if ( 0 != (8&q) )
    {
      // see if check for north/south sphere mapping poles and fix them
      if ( 8 == ftc.quad[0] )
      {
        t0 = (8 == ftc.quad[3]) ? MYON_UNSET_FLOAT : ftc.Tx[3];
        t1 = (8 == ftc.quad[1]) ? MYON_UNSET_FLOAT : ftc.Tx[1];
        if ( MYON_UNSET_FLOAT != t0 || MYON_UNSET_FLOAT != t1 )
        {
          ftc.Tx[0] = PoleFix(t0,t1);
          ftc.quad[0] = ((ftc.Tx[0] < ang0) ? 1 : ((ftc.Tx[0] > ang1) ? 4 : 0));
          q |= ftc.quad[0];
          ftc.fi = fi;
        }
      }
      if ( 8 == ftc.quad[1] )
      {
        t0 = (8 == ftc.quad[0]) ? MYON_UNSET_FLOAT : ftc.Tx[0];
        t1 = (8 == ftc.quad[2]) ? MYON_UNSET_FLOAT : ftc.Tx[2];
        if ( MYON_UNSET_FLOAT != t0 || MYON_UNSET_FLOAT != t1 )
        {
          ftc.Tx[1] = PoleFix(t0,t1);
          ftc.quad[1] = ((ftc.Tx[1] < ang0) ? 1 : ((ftc.Tx[1] > ang1) ? 4 : 0));
          q |= ftc.quad[1];
          ftc.fi = fi;
        }
      }
      if ( 8 == ftc.quad[2] )
      {
        int k = (Fvi[2] == Fvi[3]) ? 0 : 3;
        t0 = (8 == ftc.quad[1]) ? MYON_UNSET_FLOAT : ftc.Tx[1];
        t1 = (8 == ftc.quad[k]) ? MYON_UNSET_FLOAT : ftc.Tx[k];
        if ( MYON_UNSET_FLOAT != t0 || MYON_UNSET_FLOAT != t1 )
        {
          ftc.Tx[2] = PoleFix(t0,t1);
          ftc.quad[2] = ((ftc.Tx[2] < ang0) ? 1 : ((ftc.Tx[2] > ang1) ? 4 : 0));
          if ( !k )
          {
            ftc.Tx[3] = ftc.Tx[2];
            ftc.quad[3] = ftc.quad[2];
          }
          q |= ftc.quad[2];
          ftc.fi = fi;
        }
      }
      if ( 8 == ftc.quad[3] && Fvi[2] != Fvi[3] )
      {
        t0 = (8 == ftc.quad[2]) ? MYON_UNSET_FLOAT : ftc.Tx[2];
        t1 = (8 == ftc.quad[0]) ? MYON_UNSET_FLOAT : ftc.Tx[0];
        if ( MYON_UNSET_FLOAT != t0 || MYON_UNSET_FLOAT != t1 )
        {
          ftc.Tx[3] = PoleFix(t0,t1);
          ftc.quad[3] = ((ftc.Tx[3] < ang0) ? 1 : ((ftc.Tx[3] > ang1) ? 4 : 0));
          q |= ftc.quad[3];
          ftc.fi = fi;
        }
      }
    }

    if ( 5 == (5&q) )
    {
      // The face has corners on both sides of the seam
      if ( two_pi_tc == 1.0 )
      {
        if ( 1 == ftc.quad[0] ) {ftc.Tx[0] += 1.0f; ftc.fi = fi;}
        if ( 1 == ftc.quad[1] ) {ftc.Tx[1] += 1.0f; ftc.fi = fi;}
        if ( 1 == ftc.quad[2] ) {ftc.Tx[2] += 1.0f; ftc.fi = fi;}
        if ( 1 == ftc.quad[3] ) {ftc.Tx[3] += 1.0f; ftc.fi = fi;}
      }
      else
      {
        // With divided textures, wrapping the texture coordinate
        // does not work because it wraps into a region of the
        // texture not use by this "side".  In this case, the
        // only thing to do is to pick the best end of the texture
        // map and clamp the tcs that hang over.  If the mesh
        // has edges near the texture seam, the picture will
        // still look ok.
        float f0=0.0f, f1=0.0f, twopitc = (float)two_pi_tc;
        //int f0cnt=0, f1cnt=0;
        if ( 1 == ftc.quad[0] ) f0 += ftc.Tx[0]; else if ( 4 == ftc.quad[0] ) f1 += twopitc-ftc.Tx[0];
        if ( 1 == ftc.quad[1] ) f0 += ftc.Tx[1]; else if ( 4 == ftc.quad[1] ) f1 += twopitc-ftc.Tx[1];
        if ( 1 == ftc.quad[2] ) f0 += ftc.Tx[2]; else if ( 4 == ftc.quad[2] ) f1 += twopitc-ftc.Tx[2];
        if (Fvi[2] != Fvi[3])
        {
          if ( 1 == ftc.quad[3] ) f0 += ftc.Tx[3]; else if ( 4 == ftc.quad[3] ) f1 += twopitc-ftc.Tx[3];
        }
        if (f0 >= f1 )
        {
          // "most" of the face is on the left side of the texture
          // If a vertex is on the right side, clamp its tc to 0.
          if ( 4 == ftc.quad[0] ) {ftc.Tx[0] = 0.0f; ftc.fi = fi;}
          if ( 4 == ftc.quad[1] ) {ftc.Tx[1] = 0.0f; ftc.fi = fi;}
          if ( 4 == ftc.quad[2] ) {ftc.Tx[2] = 0.0f; ftc.fi = fi;}
          if ( 4 == ftc.quad[3] ) {ftc.Tx[3] = 0.0f; ftc.fi = fi;}
        }
        else
        {
          // "most" of the face is on the right side of the texture
          // If a vertex is on the left side, clamp its tc to two_pi_tc.
          if ( 1 == ftc.quad[0] ) {ftc.Tx[0] = twopitc; ftc.fi = fi;}
          if ( 1 == ftc.quad[1] ) {ftc.Tx[1] = twopitc; ftc.fi = fi;}
          if ( 1 == ftc.quad[2] ) {ftc.Tx[2] = twopitc; ftc.fi = fi;}
          if ( 1 == ftc.quad[3] ) {ftc.Tx[3] = twopitc; ftc.fi = fi;}
        }
      }
    }

    if ( ftc.fi >= 0 )
    {
      // face will require special handling
      ftc_list.Append(ftc);
    }
  }

  ftc_count = ftc_list.Count();
  if ( ftc_count <= 0 )
    return;

  // Count the number of new vertices that will be added.
  int ftci;
  int newvcnt = 0;
  for ( ftci = 0; ftci < ftc_count; ftci++ )
  {
    MYON__CMeshFaceTC& ftc_local = ftc_list[ftci];
    Fvi = F[ftc_local.fi].vi;
    if ( ftc_local.Tx[0] != Tx[Fvi[0]] )
    {
      ftc_local.bSetT[0] = true;
      newvcnt++;
    }
    if ( ftc_local.Tx[1] != Tx[Fvi[1]] )
    {
      ftc_local.bSetT[1] = true;
      newvcnt++;
    }
    if ( ftc_local.Tx[2] != Tx[Fvi[2]] )
    {
      ftc_local.bSetT[2] = true;
      newvcnt++;
    }
    if ( Fvi[2] != Fvi[3] )
    {
      if ( ftc_local.Tx[3] != Tx[Fvi[3]] )
      {
        ftc_local.bSetT[3] = true;
        newvcnt++;
      }
    }
  }

  if ( newvcnt <= 0 )
    return;


  F = 0; // Setting them to nullptr makes sure anybody who
         // tries to use them below will crash.

  // reserve room for new vertex information
  MYON__CChangeTextureCoordinateHelper helper(mesh,newvcnt,mesh_T);

  // add vertices and update mesh faces
  for ( ftci = 0; ftci < ftc_count; ftci++ )
  {
    const MYON__CMeshFaceTC& ftc_local = ftc_list[ftci];
    int* meshFvi = mesh.m_F[ftc_local.fi].vi;

    if ( ftc_local.bSetT[0] )
    {
      helper.ChangeTextureCoordinate(meshFvi,0,ftc_local.Tx[0],MYON_UNSET_FLOAT,mesh_T,mesh_T_stride);
    }
    if ( ftc_local.bSetT[1] )
    {
      helper.ChangeTextureCoordinate(meshFvi,1,ftc_local.Tx[1],MYON_UNSET_FLOAT,mesh_T,mesh_T_stride);
    }
    if ( ftc_local.bSetT[2] )
    {
      helper.ChangeTextureCoordinate(meshFvi,2,ftc_local.Tx[2],MYON_UNSET_FLOAT,mesh_T,mesh_T_stride);
    }
    if ( ftc_local.bSetT[3] )
    {
      helper.ChangeTextureCoordinate(meshFvi,3,ftc_local.Tx[3],MYON_UNSET_FLOAT,mesh_T,mesh_T_stride);
    }
  }
}

static
bool SeamCheckHelper( const MYON_TextureMapping& mp,
                      double& two_pi_tc,
                      MYON_SimpleArray<int>& Tside,
                      MYON_SimpleArray<int>*& Tsd )
{
  bool bSeamCheck = false;
  switch(mp.m_type)
  {
    case MYON_TextureMapping::TYPE::box_mapping:
      if ( MYON_TextureMapping::TEXTURE_SPACE::divided == mp.m_texture_space )
      {
        if ( mp.m_bCapped )
          two_pi_tc = 2.0/3.0;
        Tsd = &Tside;
        bSeamCheck = true;
      }
      else if ( MYON_TextureMapping::TEXTURE_SPACE::single == mp.m_texture_space )
      {
        Tsd = &Tside;
        bSeamCheck = true;
      }
      break;

    case MYON_TextureMapping::TYPE::cylinder_mapping:
      if ( MYON_TextureMapping::TEXTURE_SPACE::divided == mp.m_texture_space )
      {
        two_pi_tc = 2.0/3.0;
        Tsd = &Tside;
      }
      bSeamCheck = true;
      break;

    case MYON_TextureMapping::TYPE::sphere_mapping:
      bSeamCheck = true;
      break;

    default:
      // intentionally skip other enum values
      break;
  }

  return bSeamCheck;
}

//If there are unused vertices, this may not work correctly - but it is very fast.
static inline bool HasSharedVertices(const MYON_Mesh& mesh)
{
  return mesh.m_V.Count() < ((mesh.TriangleCount() * 3) + (mesh.QuadCount() * 4));
}

const MYON_TextureCoordinates* MYON_Mesh::SetCachedTextureCoordinates(
  const class MYON_TextureMapping& mapping,
  const class MYON_Xform* mesh_xform,
  bool bLazy
)
{
  return SetCachedTextureCoordinatesEx(mapping, mesh_xform, bLazy, true);
}

const MYON_TextureCoordinates* MYON_Mesh::SetCachedTextureCoordinatesEx(
        const class MYON_TextureMapping& mapping,
        const class MYON_Xform* mesh_xform,
        bool bLazy,
        bool bSeamCheck
        )
{
  if ( mapping.RequiresVertexNormals() && !HasVertexNormals() )
    ComputeVertexNormals();

  MYON_TextureMapping mp = mapping;
  double two_pi_tc = 1.0;
  MYON_SimpleArray<int> Tside;
  MYON_SimpleArray<int>* Tsd = 0;

  if (bSeamCheck)
  {
    bSeamCheck = SeamCheckHelper(mp, two_pi_tc, Tside, Tsd) && HasSharedVertices(*this);
    if (bSeamCheck)
      mp.m_uvw = MYON_Xform::IdentityTransformation;
  }

  MYON_TextureCoordinates* TC = 0;
  {
    for ( int i = 0; i < m_TC.Count(); i++ )
    {
      if ( m_TC[i].m_tag.m_mapping_id == mapping.Id() )
      {
        TC = &m_TC[i];
        break;
      }
    }
  }
  if ( bLazy && TC && mapping.HasMatchingTextureCoordinates( TC->m_tag, mesh_xform ) )
    return TC;

  if ( !TC )
  {
    m_TC.AppendNew();
    TC = m_TC.Last();
  }

  // Use mp instead of mapping to call GetTextureCoordinates()
  // because m_uvw must be the identity if we have seams.
  if ( !mp.GetTextureCoordinates( *this,TC->m_T,mesh_xform,false,Tsd) )
  {
    int tci = (int)(TC - m_TC.Array());
    m_TC.Remove(tci);
    return 0;
  }

  TC->m_tag.Set(mapping);
  if (    mesh_xform && mesh_xform->IsValid()
       && !mesh_xform->IsIdentity()
       && !mesh_xform->IsZero()
     )
  {
    TC->m_tag.m_mesh_xform = *mesh_xform;
  }

  TC->m_dim = 2;

  if ( bSeamCheck &&  m_F.Count() > 0 && TC->m_T.Count() == m_V.Count() )
  {
    float* mesh_T = (float*)TC->m_T.Array();
    int mesh_T_stride = sizeof(TC->m_T[0])/sizeof(mesh_T[0]);
    if ( Tsd && Tside.Count() != m_V.Count() )
      Tsd = 0;
    AdjustMeshPeriodicTextureCoordinatesHelper( *this, mesh_xform, mesh_T, mesh_T_stride, Tsd ? Tside.Array() : 0, two_pi_tc, mp );
    mesh_T = 0; // when the array is grown, the pointer may become invalid
    if ( !mapping.m_uvw.IsIdentity() && !mapping.m_uvw.IsZero() )
    {
      // Apply the uvw transformation that is on mapping
      // to the texture coordinates.
      MYON_3dPoint T;
      int vi, vcnt = TC->m_T.Count();
      MYON_3fPoint* meshT = TC->m_T.Array();
      for ( vi = 0; vi < vcnt; vi++ )
      {
        T = meshT[vi];
        T = mapping.m_uvw*T;
        meshT[vi] = T;
      }
    }
  }

  return TC;
}

bool MYON_Mesh::SetTextureCoordinates(
  const class MYON_TextureMapping& mapping,
  const class MYON_Xform* mesh_xform,
  bool bLazy
)
{
  return SetTextureCoordinatesEx(mapping, mesh_xform, bLazy, true);
}

bool MYON_Mesh::SetTextureCoordinatesEx(
                  const class MYON_TextureMapping& mapping,
                  const class MYON_Xform* mesh_xform,
                  bool bLazy,
                  bool bSeamCheck
                  )
{
  if ( mapping.RequiresVertexNormals() && !HasVertexNormals() )
    ComputeVertexNormals();

  InvalidateTextureCoordinateBoundingBox();

  MYON_SimpleArray<int> Tside;
  MYON_SimpleArray<int>* Tsd = 0;
  MYON_TextureMapping mp = mapping;

  double two_pi_tc = 1.0;

  if (bSeamCheck)
  {
    bSeamCheck = SeamCheckHelper(mp, two_pi_tc, Tside, Tsd) && HasSharedVertices(*this);
    if (bSeamCheck)
      mp.m_uvw = MYON_Xform::IdentityTransformation;
  }

  // Use mp instead of mapping to call GetTextureCoordinates()
  // because m_uvw must be the identity if we have seams.
  bool rc = mp.GetTextureCoordinates(*this,m_T,mesh_xform,bLazy,Tsd);

  if (Tsd != nullptr && Tsd->Count() == 0)
  {
    // Tsd array is needed for seam check but it was not filled by GetTextureCoordinates.
    // This happened because matching texture coordinates were found. These coordinates
    // were already once seam checked. So seam check can be skipped now.
    bSeamCheck = false;
  }

  if (rc)
  {
    // update the texture coordinate tag
    m_Ttag.Set(mapping);
    if (    mesh_xform
         && mesh_xform->IsValid()
         && !mesh_xform->IsIdentity()
         && !mesh_xform->IsZero()
       )
    {
      m_Ttag.m_mesh_xform  = *mesh_xform;
    }
  }

  if ( rc && bSeamCheck && HasTextureCoordinates() && m_F.Count() > 0 )
  {
    float* mesh_T = (float*)m_T.Array();
    int mesh_T_stride = sizeof(m_T[0])/sizeof(mesh_T[0]);
    if ( Tsd && Tside.Count() != m_V.Count() )
      Tsd = 0;
    AdjustMeshPeriodicTextureCoordinatesHelper( *this, mesh_xform, mesh_T, mesh_T_stride, Tsd ? Tside.Array() : 0, two_pi_tc, mp );
    mesh_T = 0; // when the array is grown, the pointer may become invalid
    if ( !mapping.m_uvw.IsIdentity() && !mapping.m_uvw.IsZero() )
    {
      // Apply the uvw transformation that is on mapping
      // to the texture coordinates.
      MYON_2fPoint* meshT = m_T.Array();
      MYON_3dPoint T;
      int vi, vcnt = m_T.Count();
      for ( vi = 0; vi < vcnt; vi++ )
      {
        T.x = meshT[vi].x;
        T.y = meshT[vi].y;
        T.z = 0.0;
        T = mapping.m_uvw*T;
        meshT[vi].x = (float)T.x;
        meshT[vi].y = (float)T.y;
      }
    }
  }

  return rc;
}

void MYON_Mesh::InvalidateCachedTextureCoordinates(bool bOnlyInvalidateCachedSurfaceParameterMapping)
{
  if (bOnlyInvalidateCachedSurfaceParameterMapping)
  {
    for (int i = m_TC.Count() - 1; i >= 0; i--)
    {
      if (m_TC[i].m_tag.m_mapping_type == MYON_TextureMapping::TYPE::srfp_mapping)
      {
        m_TC.Remove(i);
      }
    }
  }
  else
  {
    m_TC.Destroy();
  }
}

MYON_MappingChannel::MYON_MappingChannel()
{
  Default();
}

void MYON_MappingChannel::Default()
{
  memset(this,0,sizeof(*this));
  m_mapping_channel_id = 1;
  m_mapping_index = -1;
  m_object_xform = MYON_Xform::IdentityTransformation;
}

int MYON_MappingChannel::Compare( const MYON_MappingChannel& other ) const
{
  int rc = m_mapping_channel_id - other.m_mapping_channel_id;
  if (!rc)
    rc = MYON_UuidCompare(m_mapping_id,other.m_mapping_id);
  if (!rc)
    rc = m_object_xform.Compare(other.m_object_xform);
  return rc;
}

bool MYON_MappingChannel::Write( MYON_BinaryArchive& archive ) const
{
  bool rc = archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,1);
  if (rc)
  {
    rc = archive.WriteInt(m_mapping_channel_id);
    if (rc) rc = archive.WriteUuid(m_mapping_id);

    // 1.1 field added 6 June 2006
    if (rc) rc = archive.WriteXform(m_object_xform);

    if ( !archive.EndWrite3dmChunk() )
      rc = false;
  }
  return rc;
}

bool MYON_MappingChannel::Read( MYON_BinaryArchive& archive )
{
  Default();
  int major_version = 0;
  int minor_version = 0;
  bool rc = archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_version,&minor_version);
  if (rc)
  {
    rc = (1 == major_version);
    if (rc) rc = archive.ReadInt(&m_mapping_channel_id);
    if (rc) rc = archive.ReadUuid(m_mapping_id);

    if ( rc && minor_version >= 1 )
    {
      // 1.1 field added 6 June 2006
      if (rc) rc = archive.ReadXform(m_object_xform);
      if (rc
          && archive.ArchiveOpenNURBSVersion() < 200610030
          && m_object_xform.IsZero()
          )
      {
        // Between versions 200606060 and 200610030,
        // there was a bug that created some mapping
        // channels with zero transformations.  This
        // if clause finds those and sets them to the
        // identity.
        m_object_xform = MYON_Xform::IdentityTransformation;
      }
    }

    if ( !archive.EndRead3dmChunk() )
      rc = false;
  }
  return rc;
}


MYON_MaterialRef::MYON_MaterialRef()
{
  Default();
}

MYON_MappingRef::MYON_MappingRef()
{
  Default();
}

void MYON_MaterialRef::Default()
{
  memset(this,0,sizeof(*this));
  // runtme index value of -1 means not set
  m_material_index          = -1;
  m_material_backface_index = -1;
  m_material_source = MYON::material_from_layer;
}

void MYON_MappingRef::Default()
{
  m_plugin_id = MYON_nil_uuid;
  m_mapping_channels.Destroy();
}

int MYON_MaterialRef::Compare( const MYON_MaterialRef& other ) const
{
  int rc = MYON_UuidCompare(m_plugin_id,other.m_plugin_id);
  if (rc)
    rc = ((int)m_material_source) - ((int)other.m_material_source);
  if (!rc)
    rc = MYON_UuidCompare(m_material_id,other.m_material_id);
  if (!rc)
    rc = MYON_UuidCompare(m_material_backface_id,other.m_material_backface_id);
  return rc;
}

int MYON_MappingRef::Compare( const MYON_MappingRef& other ) const
{
  int rc = MYON_UuidCompare(m_plugin_id,other.m_plugin_id);
  if ( !rc)
  {
    const int count = m_mapping_channels.Count();
    rc = count - other.m_mapping_channels.Count();
    if (!rc)
    {
      for ( int i = 0; i < count && !rc; i++ )
      {
        rc = m_mapping_channels[i].Compare(other.m_mapping_channels[i]);
      }
    }
  }
  return rc;
}


bool MYON_MaterialRef::Write( MYON_BinaryArchive& archive ) const
{
  bool rc = archive.BeginWrite3dmChunk( TCODE_ANONYMOUS_CHUNK, 1, 1 );
  if (rc)
  {
    if (rc) rc = archive.WriteUuid( m_plugin_id );
    if (rc) rc = archive.WriteUuid( m_material_id );

    // 23 May 2006 Dale lear
    //   m_mapping_channels[] was removed from MYON_MaterialRef.
    //   To keep from breaking the file format, I need to
    //   write a zero as the array length.
    //
    //if (rc) rc = archive.WriteArray( m_mapping_channels );
    if (rc) rc = archive.WriteInt(0);

    // 23 May 2006 added
    if (rc) rc = archive.WriteUuid( m_material_backface_id );
    if (rc) rc = archive.WriteInt( m_material_source );


    if ( !archive.EndWrite3dmChunk() )
      rc = false;
  }
  return rc;
}

bool MYON_MaterialRef::Read( MYON_BinaryArchive& archive )
{
  Default();
  int major_version = 0;
  int minor_version = 0;
  bool rc = archive.BeginRead3dmChunk( TCODE_ANONYMOUS_CHUNK, &major_version, &minor_version );
  if (rc)
  {
    rc = (1 == major_version);

    if (rc) rc = archive.ReadUuid( m_plugin_id );
    if (rc) rc = archive.ReadUuid( m_material_id );

    // 23 May 2006 Dale lear
    //   m_mapping_channels[] was removed from MYON_MaterialRef.
    //   To keep from breaking the file format, I need to
    //   write a zero as the array length.
    MYON_SimpleArray<MYON_MappingChannel> obsolete_mapping_channels;
    if (rc) rc = archive.ReadArray( obsolete_mapping_channels );

    if ( minor_version >= 1 )
    {
      if (rc) rc = archive.ReadUuid( m_material_backface_id );
      int i = m_material_source;
      if (rc) rc = archive.ReadInt( &i );
      if (rc) m_material_source = (unsigned char)MYON::ObjectMaterialSource(i);
    }

    if ( !archive.EndRead3dmChunk() )
      rc = false;
  }
  return rc;
}

void MYON_MaterialRef::SetMaterialSource(
  MYON::object_material_source material_source
  )
{
  m_material_source = (unsigned char)material_source;
}

MYON::object_material_source MYON_MaterialRef::MaterialSource() const
{
  return MYON::ObjectMaterialSource(m_material_source);
}

bool MYON_MappingRef::Write( MYON_BinaryArchive& archive ) const
{
  bool rc = archive.BeginWrite3dmChunk( TCODE_ANONYMOUS_CHUNK, 1, 0 );
  if (rc)
  {
    if (rc) rc = archive.WriteUuid( m_plugin_id );
    if (rc) rc = archive.WriteArray( m_mapping_channels );

    if ( !archive.EndWrite3dmChunk() )
      rc = false;
  }
  return rc;
}

bool MYON_MappingRef::Read( MYON_BinaryArchive& archive )
{
  Default();
  int major_version = 0;
  int minor_version = 0;
  bool rc = archive.BeginRead3dmChunk( TCODE_ANONYMOUS_CHUNK, &major_version, &minor_version );
  if (rc)
  {
    rc = (1 == major_version);

    if (rc) rc = archive.ReadUuid( m_plugin_id );
    if (rc) rc = archive.ReadArray( m_mapping_channels );

    if ( !archive.EndRead3dmChunk() )
      rc = false;
  }
  return rc;
}

bool MYON_MappingRef::Transform( const MYON_Xform& xform )
{
  int count = m_mapping_channels.Count();
  if ( count > 0 )
  {
    for ( MYON_MappingChannel* mapping_channel = m_mapping_channels.Array();
          count--;
          mapping_channel++ )
    {
      mapping_channel->m_object_xform = xform*mapping_channel->m_object_xform;
    }
  }
  return true;
}

MYON_ObjectRenderingAttributes::MYON_ObjectRenderingAttributes()
{
  Default();
}

MYON_RenderingAttributes::MYON_RenderingAttributes()
{
  Default();
}

void MYON_ObjectRenderingAttributes::Default()
{
  MYON_RenderingAttributes::Default();
  m_mappings.Destroy();
  m_bCastsShadows = true;
  m_bReceivesShadows = true;
  m_bits = 0;
  m_reserved1 = 0;
}

void MYON_RenderingAttributes::Default()
{
  m_materials.Destroy();
}

void MYON_ObjectRenderingAttributes::EnableAdvancedTexturePreview(bool b)
{
  if ( b )
    m_bits |= 1;    // set bit 1
  else
    m_bits &= 0xFE; // clear bit 1
}

bool MYON_ObjectRenderingAttributes::AdvancedTexturePreview() const
{
  return (0 != (1 & m_bits)) ? true : false;
}

bool MYON_RenderingAttributes::IsValid( MYON_TextLog* text_log ) const
{
  // plug-in uuids must be unique
  int count;
  if( (count = m_materials.Count()) > 1 )
  {
    const MYON_MaterialRef* mr = m_materials.Array();
    MYON_UUID plugin_id;
    int i, j;
    for ( i = 0; i < count-1; i++ )
    {
      plugin_id = mr[i].m_plugin_id;
      for ( j = i+1; j < count; j++ )
      {
        if ( !MYON_UuidCompare(&plugin_id,&mr[j].m_plugin_id ) )
        {
          if( text_log )
          {
            text_log->Print("MYON_RenderingAttributes error: m_materials[%d] and m_materials[%d] have the same plug-in id.\n",i,j);
          }
          return false;
        }
      }
    }
  }
  return true;
}

bool MYON_ObjectRenderingAttributes::IsValid( MYON_TextLog* text_log ) const
{
  if ( !MYON_RenderingAttributes::IsValid(text_log) )
    return false;

  // plug-in uuids must be unique
  int count;
  if( (count = m_mappings.Count()) > 1 )
  {
    const MYON_MappingRef* mr = m_mappings.Array();
    MYON_UUID plugin_id;
    int i, j;
    for ( i = 0; i < count-1; i++ )
    {
      plugin_id = mr[i].m_plugin_id;
      for ( j = i+1; j < count; j++ )
      {
        if ( !MYON_UuidCompare(&plugin_id,&mr[j].m_plugin_id ) )
        {
          if( text_log )
          {
            text_log->Print("MYON_ObjectRenderingAttributes error: m_mappings[%d] and m_mappings[%d] have the same plug-in id.\n",i,j);
          }
          return false;
        }
      }
    }
  }

  return true;
}

int MYON_RenderingAttributes::Compare( const MYON_RenderingAttributes& other ) const
{
  const int count = m_materials.Count();
  int rc = count - other.m_materials.Count();
  if (!rc)
  {
    int i;
    for ( i = 0; i < count && !rc; i++ )
    {
      rc = m_materials[i].Compare(other.m_materials[i]);
    }
  }
  return rc;
}

const MYON_MaterialRef* MYON_RenderingAttributes::MaterialRef( const MYON_UUID& plugin_id ) const
{
  int count;
  if ( (count = m_materials.Count()) > 0 )
  {
    for ( const MYON_MaterialRef* mr = m_materials.Array(); count--; mr++ )
    {
      if ( plugin_id == mr->m_plugin_id )
        return mr;
    }
  }
  return 0;
}

int MYON_ObjectRenderingAttributes::Compare( const MYON_ObjectRenderingAttributes& other ) const
{
  int rc = MYON_RenderingAttributes::Compare(other);
  if (!rc)
  {
    int i;
    const int count = m_mappings.Count();
    rc = other.m_mappings.Count() - count;
    for ( i = 0; i < count && !rc; i++ )
    {
      rc = m_mappings[i].Compare(other.m_mappings[i]);
    }
    if ( !rc )
    {
      rc = ((int)(m_bCastsShadows?1:0)) - ((int)(other.m_bCastsShadows?1:0));
      if ( !rc )
      {
        rc = ((int)(m_bReceivesShadows?1:0)) - ((int)(other.m_bReceivesShadows?1:0));
      }
	  if ( !rc )
	  {
	    rc = ((int)(AdvancedTexturePreview()?1:0)) - ((int)(other.AdvancedTexturePreview()?1:0));
	  }
    }
  }
  return rc;
}

bool MYON_ObjectRenderingAttributes::Transform( const MYON_Xform& xform )
{
  int i;
  if ( (i = m_mappings.Count()) > 0 )
  {
    for( MYON_MappingRef* mr = m_mappings.Array(); i--; mr++ )
      mr->Transform(xform);
  }
  return true;
}

const MYON_MappingRef* MYON_ObjectRenderingAttributes::MappingRef(
  const MYON_UUID& plugin_id ) const
{
  int count;
  if ( (count = m_mappings.Count()) > 0 )
  {
    for ( const MYON_MappingRef* mr = m_mappings.Array(); count--; mr++ )
    {
      if ( plugin_id == mr->m_plugin_id )
        return mr;
    }
  }

  //ALB 2013.12.03
  //Fixes http://mcneel.myjetbrains.com/youtrack/issue/RH-5730
  //I'm sick of this bug being considered irrelavent, and since I've decided to go out of my way to
  //Sort out as many mapping problems as I can, I'm fixing this one like this.
  if (m_mappings.Count() > 0)
  {
	  return &m_mappings[0];
  }

  return 0;
}

MYON_MappingRef* MYON_ObjectRenderingAttributes::AddMappingRef(
  const MYON_UUID& plugin_id
  )
{
  MYON_MappingRef* mr = 0;
  int count;
  if ( (count = m_mappings.Count()) > 0 )
  {
    for ( mr = const_cast<MYON_MappingRef*>(m_mappings.Array()); count--; mr++ )
    {
      if ( plugin_id == mr->m_plugin_id )
        break;
    }
  }

  if ( !mr )
  {
    mr = &m_mappings.AppendNew();
    mr->m_plugin_id = plugin_id;
  }

  return mr;
}

bool MYON_ObjectRenderingAttributes::DeleteMappingRef(
  const MYON_UUID& plugin_id
  )
{
  const MYON_MappingRef* mr = MappingRef(plugin_id);
  if ( mr )
    m_mappings.Remove( (int)(mr - m_mappings.Array()) ); // safe ptr to in conversion
  return (0 != mr);
}

//static
int MYON_ObjectRenderingAttributes::OCSMappingChannelId(void)
{
  return 100000;
}

const MYON_MappingChannel* MYON_ObjectRenderingAttributes::MappingChannel(
  const MYON_UUID& plugin_id,
  const MYON_UUID& mapping_id
  ) const
{
  const MYON_MappingRef* mr = MappingRef(plugin_id);
  if ( mr )
  {
    int count;
    if ( (count = mr->m_mapping_channels.Count()) > 0 )
    {
      for ( const MYON_MappingChannel* mc = mr->m_mapping_channels.Array(); count--; mc++ )
      {
        if ( mapping_id == mc->m_mapping_id )
          return mc;
      }
    }
  }
  return 0;
}

const MYON_MappingChannel* MYON_ObjectRenderingAttributes::MappingChannel(
  const MYON_UUID& plugin_id,
  int mapping_channel_id
  ) const
{
  const MYON_MappingRef* mr = MappingRef(plugin_id);
  if ( mr )
  {
    int count;
    if ( (count = mr->m_mapping_channels.Count()) > 0 )
    {
      for ( const MYON_MappingChannel* mc = mr->m_mapping_channels.Array(); count--; mc++ )
      {
        if ( mapping_channel_id == mc->m_mapping_channel_id )
          return mc;
      }
    }
  }
  return 0;
}



bool MYON_ObjectRenderingAttributes::AddMappingChannel(
        const MYON_UUID& plugin_id,
        int mapping_channel_id,
        const MYON_UUID& mapping_id
        )
{
  MYON_MappingRef* mr = const_cast<MYON_MappingRef*>(MappingRef(plugin_id));
  if ( !mr )
  {
    mr = &m_mappings.AppendNew();
    mr->m_plugin_id = plugin_id;
    MYON_MappingChannel& mc = mr->m_mapping_channels.AppendNew();
    mc.m_mapping_channel_id = mapping_channel_id;
    mc.m_mapping_id = mapping_id;
    mc.m_mapping_index = -1; // 27th October 2011 John Croudy - constructor is not called by AppendNew().
    mc.m_object_xform = MYON_Xform::IdentityTransformation;
    return true;
  }

  return mr->AddMappingChannel(mapping_channel_id,mapping_id);
}

bool MYON_ObjectRenderingAttributes::DeleteMappingChannel(
  const MYON_UUID& plugin_id,
  int mapping_channel_id
  )
{
  MYON_MappingRef* mr = const_cast<MYON_MappingRef*>(MappingRef(plugin_id));
  return mr ? mr->DeleteMappingChannel(mapping_channel_id) : false;
}

bool MYON_ObjectRenderingAttributes::DeleteMappingChannel(
  const MYON_UUID& plugin_id,
  const MYON_UUID& mapping_id
  )
{
  MYON_MappingRef* mr = const_cast<MYON_MappingRef*>(MappingRef(plugin_id));
  return mr ? mr->DeleteMappingChannel(mapping_id) : false;
}

bool MYON_ObjectRenderingAttributes::ChangeMappingChannel(
  const MYON_UUID& plugin_id,
  int old_mapping_channel_id,
  int new_mapping_channel_id
  )
{
  MYON_MappingRef* mr = const_cast<MYON_MappingRef*>(MappingRef(plugin_id));
  return mr ? mr->ChangeMappingChannel(old_mapping_channel_id,new_mapping_channel_id) : false;
}

const MYON_MappingChannel* MYON_MappingRef::MappingChannel(
  const MYON_UUID& mapping_id
  ) const
{
  int count;
  if ( (count = m_mapping_channels.Count()) > 0 )
  {
    for ( const MYON_MappingChannel* mc = m_mapping_channels.Array(); count--; mc++ )
    {
      if ( mapping_id == mc->m_mapping_id )
        return mc;
    }
  }
  return 0;
}

const MYON_MappingChannel* MYON_MappingRef::MappingChannel(
  int mapping_channel_id
  ) const
{
  int count;
  if ( (count = m_mapping_channels.Count()) > 0 )
  {
    for ( const MYON_MappingChannel* mc = m_mapping_channels.Array(); count--; mc++ )
    {
      if ( mapping_channel_id == mc->m_mapping_channel_id )
        return mc;
    }
  }
  return 0;
}



bool MYON_MappingRef::AddMappingChannel(
        int mapping_channel_id,
        const MYON_UUID& mapping_id
        )
{
  int i;
  if ( (i = m_mapping_channels.Count()) > 0 )
  {
    for ( const MYON_MappingChannel* mc = m_mapping_channels.Array(); i--; mc++ )
    {
      if ( mapping_channel_id == mc->m_mapping_channel_id )
      {
        // a matching mapping channel id exists
        // return true if mapping_id matches
        return ( 0 == MYON_UuidCompare(&mapping_id,&mc->m_mapping_id) );
      }
    }
  }

  MYON_MappingChannel& mc   = m_mapping_channels.AppendNew();
  mc.m_mapping_channel_id = mapping_channel_id;
  mc.m_mapping_id         = mapping_id;
  mc.m_mapping_index      = -1; // 27th October 2011 John Croudy - constructor is not called by AppendNew().
  mc.m_object_xform = MYON_Xform::IdentityTransformation;

  return true;
}

bool MYON_MappingRef::DeleteMappingChannel(int mapping_channel_id)
{
  const MYON_MappingChannel* mc = MappingChannel(mapping_channel_id);
  if ( mc )
  {
    m_mapping_channels.Remove((int)(mc - m_mapping_channels.Array()));
  }
  return ( 0 != mc);
}

bool MYON_MappingRef::DeleteMappingChannel(const MYON_UUID& mapping_id)
{
  const MYON_MappingChannel* mc = MappingChannel(mapping_id);
  if ( mc )
  {
    m_mapping_channels.Remove((int)(mc - m_mapping_channels.Array()));
  }
  return ( 0 != mc);
}

bool MYON_MappingRef::ChangeMappingChannel(
  int old_mapping_channel_id,
  int new_mapping_channel_id
  )
{
  MYON_MappingChannel* mc = const_cast<MYON_MappingChannel*>(MappingChannel(old_mapping_channel_id));
  if ( mc )
  {
    mc->m_mapping_channel_id = new_mapping_channel_id;
  }
  return ( 0 != mc );
}

bool MYON_RenderingAttributes::Write( MYON_BinaryArchive& archive ) const
{
  bool rc = archive.BeginWrite3dmChunk( TCODE_ANONYMOUS_CHUNK, 1, 0 );
  if ( !rc )
    return false;
  for(;;)
  {
    rc = archive.WriteArray(m_materials);
    if ( !rc ) break;

    break;
  }
  if ( !archive.EndWrite3dmChunk() )
    rc = false;
  return rc;
}

bool MYON_RenderingAttributes::Read( MYON_BinaryArchive& archive )
{
  Default();
  int major_version = 0;
  int minor_version = 0;
  bool rc = archive.BeginRead3dmChunk( TCODE_ANONYMOUS_CHUNK, &major_version, &minor_version );
  if (!rc)
    return false;
  for(;;)
  {
    rc = ( 1 == major_version );
    if (!rc) break;
    rc = archive.ReadArray(m_materials);
    if (!rc) break;

    break;
  }
  if ( !archive.EndRead3dmChunk() )
    rc = false;
  return rc;
}

bool MYON_ObjectRenderingAttributes::Write( MYON_BinaryArchive& archive ) const
{
  bool rc = archive.BeginWrite3dmChunk( TCODE_ANONYMOUS_CHUNK, 1, 3 );
  if ( !rc )
    return false;
  for(;;)
  {
    // DO NOT CALL MYON_RenderingAttributes::Write
    rc = archive.WriteArray(m_materials);
    if ( !rc ) break;
    rc = archive.WriteArray(m_mappings);
    if ( !rc ) break;

    // version 1.2 fields added 20061129
    rc = archive.WriteBool(m_bCastsShadows);
    if ( !rc ) break;
    rc = archive.WriteBool(m_bReceivesShadows);
    if ( !rc ) break;

    // version 1.3 fields added 20101019
    bool b = AdvancedTexturePreview();
    rc = archive.WriteBool(b);
    if ( !rc ) break;

    break;
  }
  if ( !archive.EndWrite3dmChunk() )
    rc = false;
  return rc;
}

bool MYON_ObjectRenderingAttributes::Read( MYON_BinaryArchive& archive )
{
  Default();
  int major_version = 0;
  int minor_version = 0;
  bool rc = archive.BeginRead3dmChunk( TCODE_ANONYMOUS_CHUNK, &major_version, &minor_version );
  if (!rc)
    return false;
  for(;;)
  {
    rc = ( 1 == major_version && minor_version >= 1 );
    if (!rc) break;

    // DO NOT CALL MYON_RenderingAttributes::Read
    if (rc) rc = archive.ReadArray(m_materials);
    if (!rc) break;
    if (rc) rc = archive.ReadArray(m_mappings);
    if (!rc) break;

    if ( minor_version <= 1 )
      break;

    // version 1.2 fields added 20061129
    rc = archive.ReadBool(&m_bCastsShadows);
    if ( !rc ) break;
    rc = archive.ReadBool(&m_bReceivesShadows);
    if ( !rc ) break;

    if ( minor_version <= 2 )
      break;

    // version 1.3 fields added 20101019
    bool b = AdvancedTexturePreview();
    rc = archive.ReadBool(&b);
    if ( !rc ) break;
    // Jussi 20120430: We don't want to enable advanced texture preview by default. It will be
    //                 turned on when needed (depending on active render plug-in etc).
    //EnableAdvancedTexturePreview(b);

    break;
  }
  if ( !archive.EndRead3dmChunk() )
    rc = false;

  return rc;
}


bool MYON_TextureMapping::SetSurfaceParameterMapping(void)
{
  // The nullptr check is wierd.
  // Speculation: A reference was null and somebody added this
  // as a hack to prevent a crash.
  if ( false == MYON_IsNullPtr(this) )
  {
    *this = MYON_TextureMapping::SurfaceParameterTextureMapping;
    SetId(); // new id
  }
	return true;
}


bool MYON_TextureMapping::SetPlaneMapping(
          const MYON_Plane& plane,
          const MYON_Interval& dx,
          const MYON_Interval& dy,
          const MYON_Interval& dz
          )
{
  *this = MYON_TextureMapping::Unset;

  // Don't call plane.IsValid(), because the plane
  // equation does not matter and many developers
  // forget to set it correctly.
  if ( !plane.origin.IsValid() )
    return false;
  if ( !MYON_IsRightHandFrame( plane.xaxis, plane.yaxis, plane.zaxis ) )
    return false;
  if ( !dx.IsValid() || !dy.IsValid() || !dz.IsValid() )
    return false;

  MYON_3dPoint C = plane.PointAt(dx.Mid(),dy.Mid(),dz.Mid());
  C.x = (0.0 == C.x) ? 0.0 : -C.x;
  C.y = (0.0 == C.y) ? 0.0 : -C.y;
  C.z = (0.0 == C.z) ? 0.0 : -C.z;
  MYON_3dVector xaxis = plane.xaxis;
  MYON_3dVector yaxis = plane.yaxis;
  MYON_3dVector zaxis = plane.zaxis;

  // Any "cleanup" needs to be done here
  // to xaxis, yaxis, zaxis.

  double sx,sy,sz;
  if ( 0.0 == (sx = dx.Length())) sx = 2.0;
  if ( 0.0 == (sy = dy.Length())) sy = 2.0;
  if ( 0.0 == (sz = dz.Length())) sz = 2.0;

  // The plane mapping matrix m_Pxyz transforms the
  // world coordinate rectangle to a (-1<=r<=1,
  // on plane to a
  // 1 X 1 square in the xy plane centered at the
  // origin.

  // m_Pxyz = surface point transformation
  MYON_3dVector X = (2.0/sx)*xaxis;
  MYON_3dVector Y = (2.0/sy)*yaxis;
  MYON_3dVector Z = (2.0/sz)*zaxis;

  m_Pxyz.m_xform[0][0] = X.x;
  m_Pxyz.m_xform[0][1] = X.y;
  m_Pxyz.m_xform[0][2] = X.z;
  m_Pxyz.m_xform[0][3] = (X.x*C.x + X.y*C.y + X.z*C.z);

  m_Pxyz.m_xform[1][0] = Y.x;
  m_Pxyz.m_xform[1][1] = Y.y;
  m_Pxyz.m_xform[1][2] = Y.z;
  m_Pxyz.m_xform[1][3] = (Y.x*C.x + Y.y*C.y + Y.z*C.z);

  m_Pxyz.m_xform[2][0] = Z.x;
  m_Pxyz.m_xform[2][1] = Z.y;
  m_Pxyz.m_xform[2][2] = Z.z;
  m_Pxyz.m_xform[2][3] = (Z.x*C.x + Z.y*C.y + Z.z*C.z);

  m_Pxyz.m_xform[3][0] = 0.0;
  m_Pxyz.m_xform[3][1] = 0.0;
  m_Pxyz.m_xform[3][2] = 0.0;
  m_Pxyz.m_xform[3][3] = 1.0;

  // m_Nxyz = surface normal transformation
  //        = inverse transpose of upper 3x3 of m_Pxyz
  X = (0.5*sx)*xaxis;
  Y = (0.5*sy)*yaxis;
  Z = (0.5*sz)*zaxis;
  m_Nxyz.m_xform[0][0] = X.x;
  m_Nxyz.m_xform[0][1] = X.y;
  m_Nxyz.m_xform[0][2] = X.z;
  m_Nxyz.m_xform[0][3] = 0.0;

  m_Nxyz.m_xform[1][0] = Y.x;
  m_Nxyz.m_xform[1][1] = Y.y;
  m_Nxyz.m_xform[1][2] = Y.z;
  m_Nxyz.m_xform[1][3] = 0.0;

  m_Nxyz.m_xform[2][0] = Z.x;
  m_Nxyz.m_xform[2][1] = Z.y;
  m_Nxyz.m_xform[2][2] = Z.z;
  m_Nxyz.m_xform[2][3] = 0.0;

  m_Nxyz.m_xform[3][0] = 0.0;
  m_Nxyz.m_xform[3][1] = 0.0;
  m_Nxyz.m_xform[3][2] = 0.0;
  m_Nxyz.m_xform[3][3] = 1.0;

  m_type = MYON_TextureMapping::TYPE::plane_mapping;
  SetId();

#if defined(MYON_DEBUG)
  {
    MYON_Plane p;
    p.xaxis = (2.0/sx)*plane.xaxis;
    p.yaxis = (2.0/sy)*plane.yaxis;
    p.zaxis = (2.0/sz)*plane.zaxis;
    p.origin.Set(-C.x,-C.y,-C.z);
    p.UpdateEquation();
    MYON_Xform P_dbg, N_dbg;
    P_dbg.Rotation(p,MYON_xy_plane);
    P_dbg.GetSurfaceNormalXform(N_dbg);

    for ( int i = 0; i < 4; i++ )
    {
      for ( int j = 0; j < 4; j++ )
      {
        if ( fabs(m_Pxyz[i][j] - P_dbg[i][j]) >= MYON_SQRT_EPSILON*(fabs(m_Pxyz[i][j])+128.0) )
        {
          MYON_ERROR("m_Pxyz is nor right\n");
          break;
        }
        if ( fabs(m_Nxyz[i][j] - N_dbg[i][j]) >= MYON_SQRT_EPSILON*(fabs(m_Nxyz[i][j])+128.0) )
        {
          MYON_ERROR("m_Nxyz is nor right\n");
          break;
        }
      }
    }
  }
#endif
	return true;
}

bool MYON_TextureMapping::SetOcsMapping(
  const MYON_Plane& plane
)
{
  const MYON_Interval interval(0.0, 1.0);
  const auto rc = SetPlaneMapping(plane, interval, interval, interval);
  m_type = MYON_TextureMapping::TYPE::plane_mapping;
  return rc;
}

bool MYON_TextureMapping::SetBoxMapping(const MYON_Plane& plane,
                                      MYON_Interval dx,
                                      MYON_Interval dy,
                                      MYON_Interval dz,
                                      bool bCapped
                                      )
{
  bool rc = SetPlaneMapping(plane,dx,dy,dz);
  if (rc)
  {
    m_bCapped = bCapped;
    m_type = MYON_TextureMapping::TYPE::box_mapping;
  }
  return rc;
}

bool MYON_TextureMapping::SetCylinderMapping(const MYON_Cylinder& cylinder, bool bIsCapped)
{
  MYON_Interval dr, dh;
  if ( !MYON_IsValid(cylinder.circle.radius ) )
    return false;
  double r = cylinder.circle.radius;
  if ( 0.0 == r )
    r = 1.0;
  dr.Set(-r,r);
  dh.Set(cylinder.height[0],cylinder.height[1]);
  if ( dh[0] == dh[1] )
  {
    if ( MYON_UNSET_VALUE == dh[0] )
    {
      dh.Set(-1.0,1.0);
    }
    else
    {
      dh.m_t[0] -= 1.0;
      dh.m_t[0] += 1.0;
    }
  }
  if ( !dh.IsValid() )
    return false;

  bool rc = SetBoxMapping(cylinder.circle.plane,dr,dr,dh,bIsCapped);
  if (rc)
  {
	  m_type = MYON_TextureMapping::TYPE::cylinder_mapping;
  }

	return rc;
}

bool MYON_TextureMapping::SetSphereMapping(const MYON_Sphere& sphere)
{
  MYON_Interval dr(-sphere.radius,sphere.radius);
  bool rc = SetBoxMapping(sphere.plane,dr,dr,dr,false);
  if (rc)
  {
	  m_type = MYON_TextureMapping::TYPE::sphere_mapping;
  }
	return rc;
}



bool MYON_TextureMapping::GetMappingPlane(MYON_Plane& plane,
                                        MYON_Interval& dx,
                                        MYON_Interval& dy,
                                        MYON_Interval& dz
                                        ) const
{
  MYON_Xform xform(m_Pxyz);

  MYON_3dVector S(((MYON_3dVector*)&xform.m_xform[0])->Length(),
                ((MYON_3dVector*)&xform.m_xform[1])->Length(),
                ((MYON_3dVector*)&xform.m_xform[2])->Length());

  if ( 0.0 == S.x )
    return false;
  S.x = 1.0/S.x;
  if ( 0.0 == S.y )
    return false;
  S.y = 1.0/S.y;
  if ( 0.0 == S.z )
    return false;
  S.z = 1.0/S.z;

  xform.m_xform[0][0] *= S.x; xform.m_xform[0][1] *= S.x; xform.m_xform[0][2] *= S.x;
  xform.m_xform[0][3] *= S.x;

  xform.m_xform[1][0] *= S.y; xform.m_xform[1][1] *= S.y; xform.m_xform[1][2] *= S.y;
  xform.m_xform[1][3] *= S.y;

  xform.m_xform[2][0] *= S.z; xform.m_xform[2][1] *= S.z; xform.m_xform[2][2] *= S.z;
  xform.m_xform[2][3] *= S.z;

  xform.m_xform[3][0] = 0.0;
  xform.m_xform[3][1] = 0.0;
  xform.m_xform[3][2] = 0.0;
  xform.m_xform[3][3] = 1.0;

  MYON_Xform inv(xform);
  if ( !inv.Invert() )
    return false;

  plane.origin.Set(inv.m_xform[0][3],inv.m_xform[1][3],inv.m_xform[2][3]);
  xform.m_xform[0][3] = 0.0;
  xform.m_xform[1][3] = 0.0;
  xform.m_xform[2][3] = 0.0;
  plane.xaxis = &xform.m_xform[0][0];
  plane.yaxis = &xform.m_xform[1][0];
  plane.zaxis = &xform.m_xform[2][0];

	plane.UpdateEquation();

  dx.Set(-S.x,S.x);
  dy.Set(-S.y,S.y);
  dz.Set(-S.z,S.z);

  return plane.IsValid();
}

bool MYON_TextureMapping::GetMappingBox(MYON_Plane& plane,
                                      MYON_Interval& dx,
                                      MYON_Interval& dy,
                                      MYON_Interval& dz) const
{
	return GetMappingPlane(plane, dx, dy, dz);
}

bool MYON_TextureMapping::GetMappingCylinder(MYON_Cylinder& cylinder) const
{
  MYON_Interval dx, dy, dz;
  MYON_Plane plane;
  bool rc = GetMappingPlane(cylinder.circle.plane, dx, dy, dz);
  if (rc)
  {
    double r0 = 0.5*dx.Length();
    double r1 = 0.5*dy.Length();
    cylinder.circle.radius = (r0 == r1) ? r0 : 0.5*(r0+r1);
    cylinder.height[0] = dz[0];
    cylinder.height[1] = dz[1];
  }

  return rc && cylinder.IsValid();
}

bool MYON_TextureMapping::GetMappingSphere(MYON_Sphere& sphere) const
{
  MYON_Interval dx, dy, dz;
  bool rc = GetMappingPlane(sphere.plane, dx, dy, dz);
  if (rc)
  {
    double r0 = 0.5*dx.Length();
    double r1 = 0.5*dy.Length();
    double r2 = 0.5*dz.Length();
    sphere.radius = (r0 == r1 && r0 == r2) ? r0 : (r0+r1+r2)/3.0;
  }
  return rc && sphere.IsValid();
}





///////////////////////////////////////////////////////////////////////////////
//
// Class MYON_PBRMaterial
//

#define SUPPORT_PBR_USERDATA_SERIALIZATION

class MYON_PhysicallyBasedMaterialUserData : public MYON_UserData
{
private:
  MYON_OBJECT_DECLARE(MYON_PhysicallyBasedMaterialUserData);

#if defined(MYON_COMPILER_CLANG)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif
  bool GetDescription(MYON_wString & description)
  {
    description = L"MYON_PhysicallyBasedMaterialUserData";
    return true;
  }
#if defined(MYON_COMPILER_CLANG)
#pragma clang diagnostic pop
#endif
  
public:
  MYON_PhysicallyBasedMaterialUserData()
  {
    m_userdata_uuid = MYON_CLASS_ID(MYON_PhysicallyBasedMaterialUserData);
    m_application_uuid = MYON_opennurbs6_id;
    m_userdata_copycount = 1;
  }

  ~MYON_PhysicallyBasedMaterialUserData() {}


  MYON_PhysicallyBasedMaterialUserData(const MYON_PhysicallyBasedMaterialUserData& src)
    : MYON_UserData(src)
  {
    m_userdata_copycount = src.m_userdata_copycount;
    m_parameters = src.m_parameters;
  }

  MYON_PhysicallyBasedMaterialUserData& operator=(const MYON_PhysicallyBasedMaterialUserData& src)
  {
    if (this != &src)
    {
      MYON_UserData::operator = (src);
      m_parameters = src.m_parameters;
    }
    return *this;
  }

private:
  // MYON_Object overrides
#if defined SUPPORT_PBR_USERDATA_SERIALIZATION
  bool Write(MYON_BinaryArchive& archive) const override
  {
    MYON_ASSERT(IsValid());

    const int chunk_version = 2;
    if (false == archive.BeginWrite3dmAnonymousChunk(chunk_version))
      return false;

    bool rc = m_parameters.Write(archive, chunk_version);

    if (!archive.EndWrite3dmChunk())
    {
      rc = false;
    }

    return rc;
  }

  bool Read(MYON_BinaryArchive& archive) override
  {
    int chunk_version = 0;
    if (false == archive.BeginRead3dmAnonymousChunk(&chunk_version))
      return false;

    bool rc = false;

    if (chunk_version == 1 || chunk_version == 2)
    {
      rc = m_parameters.Read(archive, chunk_version);
    }

    if (!archive.EndRead3dmChunk())
    {
      rc = false;
    }

    MYON_ASSERT(IsValid());
    return rc;
  }

  bool Archive() const override
  {
    return true;
  }
#else
  bool Archive() const override
  {
    return false;
  }
#endif

  bool IsValid(class MYON_TextLog* text_log = nullptr) const override
  {
    if (!m_parameters.IsValid(text_log))
      return false;

    return MYON_UserData::IsValid(text_log);
  }

public:
  struct Parameters
  {
#if defined SUPPORT_PBR_USERDATA_SERIALIZATION
    bool Write(MYON_BinaryArchive& binary_archive, int version) const
    {
      if (!binary_archive.WriteColor(base_color)) return false;
      if (!binary_archive.WriteInt((int)brdf)) return false;
      if (!binary_archive.WriteDouble(subsurface)) return false;
      if (!binary_archive.WriteColor(subsurface_scattering_color)) return false;
      if (!binary_archive.WriteDouble(subsurface_scattering_radius)) return false;
      if (!binary_archive.WriteDouble(metallic)) return false;
      if (!binary_archive.WriteDouble(specular)) return false;
      if (!binary_archive.WriteDouble(specular_tint)) return false;
      if (!binary_archive.WriteDouble(roughness)) return false;
      if (!binary_archive.WriteDouble(anisotropic)) return false;
      if (!binary_archive.WriteDouble(anisotropic_rotation)) return false;
      if (!binary_archive.WriteDouble(sheen)) return false;
      if (!binary_archive.WriteDouble(sheen_tint)) return false;
      if (!binary_archive.WriteDouble(clearcoat)) return false;
      if (!binary_archive.WriteDouble(clearcoat_roughness)) return false;
      if (!binary_archive.WriteDouble(opacity_IOR)) return false;
      if (!binary_archive.WriteDouble(opacity)) return false;
      if (!binary_archive.WriteDouble(opacity_roughness)) return false;
      if (!binary_archive.WriteColor(emission)) return false;

      if (version >= 2)
      {
        if (!binary_archive.WriteDouble(alpha)) return false;
      }
      
      return true;
    }

    bool Read(MYON_BinaryArchive& binary_archive, int version)
    {
      if (!binary_archive.ReadColor(base_color)) return false;                          
      if (!binary_archive.ReadInt((int*)&brdf)) return false;
      if (!binary_archive.ReadDouble(&subsurface)) return false;
      if (!binary_archive.ReadColor(subsurface_scattering_color)) return false;
      if (!binary_archive.ReadDouble(&subsurface_scattering_radius)) return false;
      if (!binary_archive.ReadDouble(&metallic)) return false;
      if (!binary_archive.ReadDouble(&specular)) return false;
      if (!binary_archive.ReadDouble(&specular_tint)) return false;
      if (!binary_archive.ReadDouble(&roughness)) return false;
      if (!binary_archive.ReadDouble(&anisotropic)) return false;
      if (!binary_archive.ReadDouble(&anisotropic_rotation)) return false;
      if (!binary_archive.ReadDouble(&sheen)) return false;
      if (!binary_archive.ReadDouble(&sheen_tint)) return false;
      if (!binary_archive.ReadDouble(&clearcoat)) return false;
      if (!binary_archive.ReadDouble(&clearcoat_roughness)) return false;
      if (!binary_archive.ReadDouble(&opacity_IOR)) return false;
      if (!binary_archive.ReadDouble(&opacity)) return false;
      if (!binary_archive.ReadDouble(&opacity_roughness)) return false;
      if (!binary_archive.ReadColor(emission)) return false;

      if (version >= 2)
      {
        if (!binary_archive.ReadDouble(&alpha)) return false;
      }
      
      return true;
    }
#endif

    bool IsValid(class MYON_TextLog* text_log = nullptr) const
    {
      //!base_color.IsValid() means that PBR is not supported.
      //if (!base_color.IsValid()) return false;
      if (MYON_UNSET_VALUE == subsurface) return false;
      if (!subsurface_scattering_color.IsValid()) return false;
      if (MYON_IS_UNSET_DOUBLE(subsurface_scattering_radius)) return false;
      if (MYON_IS_UNSET_DOUBLE(metallic)) return false;
      if (MYON_IS_UNSET_DOUBLE(specular)) return false;
      if (MYON_IS_UNSET_DOUBLE(specular_tint)) return false;
      if (MYON_IS_UNSET_DOUBLE(roughness)) return false;
      if (MYON_IS_UNSET_DOUBLE(anisotropic)) return false;
      if (MYON_IS_UNSET_DOUBLE(anisotropic_rotation)) return false;
      if (MYON_IS_UNSET_DOUBLE(sheen)) return false;
      if (MYON_IS_UNSET_DOUBLE(sheen_tint)) return false;
      if (MYON_IS_UNSET_DOUBLE(clearcoat)) return false;
      if (MYON_IS_UNSET_DOUBLE(clearcoat_roughness)) return false;
      if (MYON_IS_UNSET_DOUBLE(opacity_IOR)) return false;
      if (MYON_IS_UNSET_DOUBLE(opacity)) return false;
      if (MYON_IS_UNSET_DOUBLE(opacity_roughness)) return false;
      if (!emission.IsValid()) return false;
      if (MYON_IS_UNSET_DOUBLE(alpha)) return false;

      return true;
    }

    MYON_4fColor base_color = MYON_4fColor::Unset;
    double subsurface = 0.0;
    MYON_4fColor subsurface_scattering_color = MYON_Color::White;
    double subsurface_scattering_radius = 0.0;
    double metallic = 0.0;
    double specular = 0.5;
    double specular_tint = 0.0;
    double roughness = 1.0;
    double anisotropic = 0.0;
    double anisotropic_rotation = 0.0;
    double sheen = 0.0;
    double sheen_tint = 0.0;
    double clearcoat = 0.0;
    double clearcoat_roughness = 0.0;
    double opacity_IOR = 1.52;
    double opacity = 1.0;
    double opacity_roughness = 0.0;
    MYON_4fColor emission = MYON_Color::Black;
    MYON_PhysicallyBasedMaterial::BRDFs brdf = MYON_PhysicallyBasedMaterial::BRDFs::GGX;
    double alpha = 1.0;
  } m_parameters;
};

MYON_UUID MYON_Material::PhysicallyBasedUserdataId(void)
{
  // {5694E1AC-40E6-44F4-9CA9-3B6D0E8C4440}
  static const MYON_UUID id = { 0x5694e1ac, 0x40e6, 0x44f4,{ 0x9c, 0xa9, 0x3b, 0x6d, 0xe, 0x8c, 0x44, 0x40 } };

  return id;
}

MYON_OBJECT_IMPLEMENT(MYON_PhysicallyBasedMaterialUserData, MYON_UserData, "5694E1AC-40E6-44F4-9CA9-3B6D0E8C4440");

class MYON_PhysicallyBasedMaterial::Impl
{
public:
  Impl(MYON_Material& mat)
    : material(&mat),
      m_pDummy(nullptr)
  {
  }

  ~Impl()
  {
    delete m_pDummy;
  }

  bool UserDataExists(void) const
  {
    const auto pUD = material->GetUserData(MYON_CLASS_ID(MYON_PhysicallyBasedMaterialUserData));
    return nullptr != pUD;
  }

  void RemoveUserData(void) const
  {
    auto pUD = material->GetUserData(MYON_CLASS_ID(MYON_PhysicallyBasedMaterialUserData));

    if (material->DetachUserData(pUD))
    {
      delete pUD;
    }
  }

  const MYON_PhysicallyBasedMaterialUserData& UserData() const
  {
    const auto pUD = material->GetUserData(MYON_CLASS_ID(MYON_PhysicallyBasedMaterialUserData));
    if (pUD)
    {
      return static_cast<const MYON_PhysicallyBasedMaterialUserData&>(*pUD);
    }

    if (m_pDummy)
    {
      return *m_pDummy;
    }

    //This is the const version
    m_pDummy = new MYON_PhysicallyBasedMaterialUserData();
    return *m_pDummy;
  }


  MYON_PhysicallyBasedMaterialUserData& UserData()
  {
    auto pUD = material->GetUserData(MYON_CLASS_ID(MYON_PhysicallyBasedMaterialUserData));
    if (pUD)
    {
      return static_cast<MYON_PhysicallyBasedMaterialUserData&>(*pUD);
    }

    MYON_PhysicallyBasedMaterialUserData* p = nullptr;

    if (nullptr == m_pDummy)
    {
      p = new MYON_PhysicallyBasedMaterialUserData();
    }
    else
    {
      p = m_pDummy;
      m_pDummy = nullptr;
    }

    material->AttachUserData(p);

    return *p;
  }

  MYON_Material* material;

  mutable MYON_PhysicallyBasedMaterialUserData* m_pDummy;
};





MYON_4fColor MYON_PhysicallyBasedMaterial::BaseColor(void) const
{
  return Implementation().UserData().m_parameters.base_color;
}

void MYON_PhysicallyBasedMaterial::SetBaseColor(const MYON_4fColor& c)
{
  Implementation().UserData().m_parameters.base_color = c;
}

MYON_PhysicallyBasedMaterial::BRDFs MYON_PhysicallyBasedMaterial::BRDF(void) const
{
  return Implementation().UserData().m_parameters.brdf;
}

void MYON_PhysicallyBasedMaterial::SetBRDF(const BRDFs& b)
{
  Implementation().UserData().m_parameters.brdf = b;
}

double MYON_PhysicallyBasedMaterial::Subsurface(void) const
{
  return Implementation().UserData().m_parameters.subsurface;
}

void MYON_PhysicallyBasedMaterial::SetSubsurface(double d)
{
  Implementation().UserData().m_parameters.subsurface = d;
}

MYON_4fColor MYON_PhysicallyBasedMaterial::SubsurfaceScatteringColor(void) const
{
  return Implementation().UserData().m_parameters.subsurface_scattering_color;
}

void MYON_PhysicallyBasedMaterial::SetSubsurfaceScatteringColor(const MYON_4fColor& c)
{
  Implementation().UserData().m_parameters.subsurface_scattering_color = c;
}

double MYON_PhysicallyBasedMaterial::SubsurfaceScatteringRadius(void) const
{
  return Implementation().UserData().m_parameters.subsurface_scattering_radius;
}

void MYON_PhysicallyBasedMaterial::SetSubsurfaceScatteringRadius(double d)
{
  Implementation().UserData().m_parameters.subsurface_scattering_radius = d;
}

double MYON_PhysicallyBasedMaterial::Metallic(void) const
{
  return Implementation().UserData().m_parameters.metallic;
}

void MYON_PhysicallyBasedMaterial::SetMetallic(double d)
{
  Implementation().UserData().m_parameters.metallic = d;
}

double MYON_PhysicallyBasedMaterial::Specular(void) const
{
  return Implementation().UserData().m_parameters.specular;
}

double MYON_PhysicallyBasedMaterial::ReflectiveIOR(void) const
{
  const double d2 = Specular() * 0.08;
  return sqrt(d2);
}

void MYON_PhysicallyBasedMaterial::SetReflectiveIOR(double ior)
{
  const double d = (ior - 1.0) / (ior + 1.0);
  SetSpecular(d*d / 0.08);
}

void MYON_PhysicallyBasedMaterial::SetSpecular(double d)
{
  Implementation().UserData().m_parameters.specular = d;
}

double MYON_PhysicallyBasedMaterial::SpecularTint(void) const
{
  return Implementation().UserData().m_parameters.specular_tint;
}

void MYON_PhysicallyBasedMaterial::SetSpecularTint(double d)
{
  Implementation().UserData().m_parameters.specular_tint = d;
}

double MYON_PhysicallyBasedMaterial::Roughness(void) const
{
  return Implementation().UserData().m_parameters.roughness;
}

void MYON_PhysicallyBasedMaterial::SetRoughness(double d)
{
  Implementation().UserData().m_parameters.roughness = d;
}

double MYON_PhysicallyBasedMaterial::Anisotropic(void) const
{
  return Implementation().UserData().m_parameters.anisotropic;
}

void MYON_PhysicallyBasedMaterial::SetAnisotropic(double d)
{
  Implementation().UserData().m_parameters.anisotropic = d;
}

double MYON_PhysicallyBasedMaterial::AnisotropicRotation(void) const
{
  return Implementation().UserData().m_parameters.anisotropic_rotation;
}

void MYON_PhysicallyBasedMaterial::SetAnisotropicRotation(double d)
{
  Implementation().UserData().m_parameters.anisotropic_rotation = d;
}

double MYON_PhysicallyBasedMaterial::Sheen(void) const
{
  return Implementation().UserData().m_parameters.sheen;
}

void MYON_PhysicallyBasedMaterial::SetSheen(double d)
{
  Implementation().UserData().m_parameters.sheen = d;
}

double MYON_PhysicallyBasedMaterial::SheenTint(void) const
{
  return Implementation().UserData().m_parameters.sheen_tint;
}

void MYON_PhysicallyBasedMaterial::SetSheenTint(double d)
{
  Implementation().UserData().m_parameters.sheen_tint = d;
}

double MYON_PhysicallyBasedMaterial::Clearcoat(void) const
{
  return Implementation().UserData().m_parameters.clearcoat;
}

void MYON_PhysicallyBasedMaterial::SetClearcoat(double d)
{
  Implementation().UserData().m_parameters.clearcoat = d;
}

double MYON_PhysicallyBasedMaterial::ClearcoatRoughness(void) const
{
  return Implementation().UserData().m_parameters.clearcoat_roughness;
}

void MYON_PhysicallyBasedMaterial::SetClearcoatRoughness(double d)
{
  Implementation().UserData().m_parameters.clearcoat_roughness = d;
}

double MYON_PhysicallyBasedMaterial::OpacityIOR(void) const
{
  return Implementation().UserData().m_parameters.opacity_IOR;
}

void MYON_PhysicallyBasedMaterial::SetOpacityIOR(double d)
{
  Implementation().UserData().m_parameters.opacity_IOR = d;
}

double MYON_PhysicallyBasedMaterial::Opacity(void) const
{
  return Implementation().UserData().m_parameters.opacity;
}

void MYON_PhysicallyBasedMaterial::SetOpacity(double d)
{
  Implementation().UserData().m_parameters.opacity = d;
}

double MYON_PhysicallyBasedMaterial::OpacityRoughness(void) const
{
  return Implementation().UserData().m_parameters.opacity_roughness;
}

void MYON_PhysicallyBasedMaterial::SetOpacityRoughness(double d)
{
  Implementation().UserData().m_parameters.opacity_roughness = d;
}

bool MYON_PhysicallyBasedMaterial::IsValid(class MYON_TextLog* text_log) const
{
  return Implementation().UserData().m_parameters.IsValid(text_log);
}

MYON_4fColor MYON_PhysicallyBasedMaterial::Emission(void) const
{
  return Implementation().UserData().m_parameters.emission;
}

void MYON_PhysicallyBasedMaterial::SetEmission(MYON_4fColor d)
{
  Implementation().UserData().m_parameters.emission = d;
}

double MYON_PhysicallyBasedMaterial::Alpha(void) const
{
  return Implementation().UserData().m_parameters.alpha;
}

void MYON_PhysicallyBasedMaterial::SetAlpha(double d)
{
  Implementation().UserData().m_parameters.alpha = d;
}

MYON_PhysicallyBasedMaterial::MYON_PhysicallyBasedMaterial(const MYON_Material& src)
{
  //Placement new - create the impl in the stack allocated space.
  new (&_impl) Impl(const_cast<MYON_Material&>(src));
}

MYON_PhysicallyBasedMaterial::~MYON_PhysicallyBasedMaterial()
{
  //delete _pImpl;
  //Call the dtor, don't release the memory
  Implementation().~Impl();
}


MYON_PhysicallyBasedMaterial::MYON_PhysicallyBasedMaterial(const MYON_PhysicallyBasedMaterial& src)
{
  new (&_impl) Impl(*src.Implementation().material);
}

bool MYON_Material::IsPhysicallyBased(void) const
{
  //Optimized version
  const auto pUD = static_cast<MYON_PhysicallyBasedMaterialUserData*>(GetUserData(MYON_CLASS_ID(MYON_PhysicallyBasedMaterialUserData)));
  if (nullptr == pUD)
    return false;

  //https://mcneel.myjetbrains.com/youtrack/issue/RH-68577
  return pUD->m_parameters.base_color.IsValid();
  
  //return nullptr != PhysicallyBased();
}

std::shared_ptr<MYON_PhysicallyBasedMaterial> MYON_Material::PhysicallyBased(void)
{
  auto ptr = std::make_shared<MYON_PhysicallyBasedMaterial>(*this);

  if (nullptr != ptr && MYON_PhysicallyBasedMaterial_Supported(*ptr))
  {
    return ptr;
  }

  return nullptr;
}

const std::shared_ptr <MYON_PhysicallyBasedMaterial> MYON_Material::PhysicallyBased(void) const
{
  auto ptr = std::make_shared<MYON_PhysicallyBasedMaterial>(*this);

  if (nullptr != ptr && MYON_PhysicallyBasedMaterial_Supported(*ptr))
  {
    return ptr;
  }

  return nullptr;
}

void MYON_Material::ToPhysicallyBased(void)
{
  if (IsPhysicallyBased())
    return;

  //This should always be valid.
  auto pbr = std::make_shared<MYON_PhysicallyBasedMaterial>(*this);
  MYON_ASSERT(pbr != nullptr);

  const bool bMetal = m_transparency < 0.01 && !m_bFresnelReflections && m_reflectivity > 0.99;
  const bool bGlass = m_transparency > 0.99;

  pbr->SetBaseColor(bMetal ? m_reflection : bGlass ? m_transparent : m_diffuse);
  pbr->SetMetallic(bMetal ? 1.0 : 0.0);
  pbr->SetRoughness(bMetal ? m_reflection_glossiness : 1.0 - m_reflectivity);
  pbr->SetOpacity(1.0 - m_transparency);
  pbr->SetOpacityIOR(m_index_of_refraction);

  MYON_ASSERT(IsPhysicallyBased());
}


const MYON_PhysicallyBasedMaterial::Impl& MYON_PhysicallyBasedMaterial::Implementation(void) const
{
  return *reinterpret_cast<const MYON_PhysicallyBasedMaterial::Impl*>(_impl);
}

MYON_PhysicallyBasedMaterial::Impl& MYON_PhysicallyBasedMaterial::Implementation(void)
{
  return *reinterpret_cast<MYON_PhysicallyBasedMaterial::Impl*>(_impl);
}


bool MYON_PhysicallyBasedMaterial_Supported(const MYON_PhysicallyBasedMaterial& material)
{
  if (!material.Implementation().UserDataExists())
    return false;

  return material.BaseColor().IsValid();
}

bool MYON_PhysicallyBasedMaterial::UseBaseColorTextureAlphaForObjectAlphaTransparencyTexture() const
{
  MYON_Material& mat = *Implementation().material;
  return mat.UseDiffuseTextureAlphaForObjectTransparencyTexture();
}

void MYON_PhysicallyBasedMaterial::SetUseBaseColorTextureAlphaForObjectAlphaTransparencyTexture(bool b)
{
  MYON_Material& mat = *Implementation().material;
  mat.SetUseDiffuseTextureAlphaForObjectTransparencyTexture(b);
}

void MYON_PhysicallyBasedMaterial::ToLegacy(void)
{
  MYON_Material& mat = *Implementation().material;
  Implementation().~Impl();
  new (&_impl) Impl(const_cast<MYON_Material&>(mat));

  Implementation().RemoveUserData();
}

void MYON_PhysicallyBasedMaterial::SynchronizeLegacyMaterial(void)
{
  auto& mat = *Implementation().material;

  const bool bIsMetal = Metallic() > 0.5;

  mat.SetDiffuse(bIsMetal ? MYON_Color::Black : (MYON_Color)BaseColor());

  const double reflectivity = bIsMetal ? 1.0 : 1.0 - Roughness();

  mat.SetFresnelReflections(!bIsMetal);

  mat.SetReflectivity(reflectivity);
  mat.SetTransparency(1.0 - Opacity());
  mat.SetAmbient(MYON_Color::Black);

  //Gloss
  mat.SetShine(MYON_Material::MaxShine * reflectivity);

  if (bIsMetal)
  {
    mat.SetSpecular(BaseColor());
    mat.m_reflection = BaseColor();
  }
  else
  {
    const int bf = (int)(255.0 * reflectivity);
    mat.SetSpecular(MYON_Color(bf, bf, bf));
    mat.m_reflection = MYON_Color::White;
  }

  mat.m_reflection_glossiness = Roughness();
  mat.m_refraction_glossiness = OpacityRoughness();

  mat.SetEmission(Emission());

  mat.m_index_of_refraction = OpacityIOR();

  //No need to do the textures, because the ones that are supported are in the right channels already.
}

int MYON_PhysicallyBasedMaterial::FindTexture(const wchar_t* filename, MYON_Texture::TYPE type, int i0) const
{
  return Material().FindTexture(filename, type, i0);
}

int MYON_PhysicallyBasedMaterial::AddTexture(const MYON_Texture& tx)
{
  return Material().AddTexture(tx);
}

int MYON_PhysicallyBasedMaterial::AddTexture(const wchar_t* filename, MYON_Texture::TYPE type)
{
  return Material().AddTexture(filename, type);
}

int MYON_PhysicallyBasedMaterial::DeleteTexture(const wchar_t* filename, MYON_Texture::TYPE type)
{
  return Material().DeleteTexture(filename, type);
}

MYON_Material& MYON_PhysicallyBasedMaterial::Material(void)
{
  return *Implementation().material;
}

const MYON_Material& MYON_PhysicallyBasedMaterial::Material(void) const
{
  return *Implementation().material;
}

MYON_wString MYON_PhysicallyBasedMaterial::ParametersNames::BaseColor(void) { return L"pbr-base-color"; }
MYON_wString MYON_PhysicallyBasedMaterial::ParametersNames::BRDF(void) { return L"pbr-brdf"; }
MYON_wString MYON_PhysicallyBasedMaterial::ParametersNames::Subsurface(void) { return L"pbr-subsurface"; }
MYON_wString MYON_PhysicallyBasedMaterial::ParametersNames::SubsurfaceScatteringColor(void) { return L"pbr-subsurface-scattering-color"; }
MYON_wString MYON_PhysicallyBasedMaterial::ParametersNames::SubsurfaceScatteringRadius(void) { return L"pbr-subsurface-scattering-radius"; }
MYON_wString MYON_PhysicallyBasedMaterial::ParametersNames::Specular(void) { return L"pbr-specular"; }
MYON_wString MYON_PhysicallyBasedMaterial::ParametersNames::SpecularTint(void) { return L"pbr-specular-tint"; }
MYON_wString MYON_PhysicallyBasedMaterial::ParametersNames::Metallic(void) { return L"pbr-metallic"; }
MYON_wString MYON_PhysicallyBasedMaterial::ParametersNames::Roughness(void) { return L"pbr-roughness"; }
MYON_wString MYON_PhysicallyBasedMaterial::ParametersNames::Anisotropic(void) { return L"pbr-anisotropic"; }
MYON_wString MYON_PhysicallyBasedMaterial::ParametersNames::AnisotropicRotation(void) { return L"pbr-anisotropic-rotation"; }
MYON_wString MYON_PhysicallyBasedMaterial::ParametersNames::Sheen(void) { return L"pbr-sheen"; }
MYON_wString MYON_PhysicallyBasedMaterial::ParametersNames::SheenTint(void) { return L"pbr-sheen-tint"; }
MYON_wString MYON_PhysicallyBasedMaterial::ParametersNames::Clearcoat(void) { return L"pbr-clearcoat"; }
MYON_wString MYON_PhysicallyBasedMaterial::ParametersNames::ClearcoatRoughness(void) { return L"pbr-clearcoat-roughness"; }
MYON_wString MYON_PhysicallyBasedMaterial::ParametersNames::ClearcoatBump(void) { return L"pbr-clearcoat-bump"; }
MYON_wString MYON_PhysicallyBasedMaterial::ParametersNames::OpacityIor(void) { return L"pbr-opacity-ior"; }
MYON_wString MYON_PhysicallyBasedMaterial::ParametersNames::Opacity(void) { return L"pbr-opacity"; }
MYON_wString MYON_PhysicallyBasedMaterial::ParametersNames::OpacityRoughness(void) { return L"pbr-opacity-roughness"; }
MYON_wString MYON_PhysicallyBasedMaterial::ParametersNames::Emission(void) { return L"pbr-emission"; }

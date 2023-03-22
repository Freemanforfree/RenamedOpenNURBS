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
#include "opennurbs_internal_defines.h"

#if !defined(MYON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// MYON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, MYON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error MYON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

class MYON_3dmObjectAttributesPrivate
{
public:
  MYON_3dmObjectAttributesPrivate() = delete;
  MYON_3dmObjectAttributesPrivate(const MYON_3dmObjectAttributes* attr);
  ~MYON_3dmObjectAttributesPrivate() = default;
  MYON_3dmObjectAttributesPrivate& operator=(const MYON_3dmObjectAttributesPrivate&) = default;

  bool operator==(const MYON_3dmObjectAttributesPrivate&) const;
  bool operator!=(const MYON_3dmObjectAttributesPrivate&) const;

  MYON::ClipParticipationSource m_clip_participation_source = MYON::ClipParticipationSource::FromLayer;
  MYON_UuidList m_clipplane_list;
  bool m_clipping_proof = false;

  MYON::SectionAttributesSource m_section_attributes_source = MYON::SectionAttributesSource::FromLayer;
  MYON::SectionFillRule m_section_fill_rule = MYON::SectionFillRule::ClosedCurves;
  int m_section_hatch_index = MYON_UNSET_INT_INDEX; // MYON_HatchPattern::Unset.Index();
  double m_section_hatch_scale = 1.0;
  double m_section_hatch_rotation = 0.0;
  double m_linetype_scale = 1.0;
  MYON_Color m_hatch_background_fill;
  bool m_hatch_boundary_visible = false;

  std::shared_ptr<MYON_Linetype> m_custom_linetype;

  MYON_DecalCollection m_decals;
  MYON_MeshModifiers m_mesh_modifiers;
};

MYON_3dmObjectAttributesPrivate::MYON_3dmObjectAttributesPrivate(const MYON_3dmObjectAttributes* attr)
  :
  m_decals(const_cast<MYON_3dmObjectAttributes*>(attr))
{
  m_hatch_background_fill = MYON_Color::UnsetColor;
}

bool MYON_3dmObjectAttributesPrivate::operator==(const MYON_3dmObjectAttributesPrivate& other) const
{
  if (m_clip_participation_source != other.m_clip_participation_source)
    return false;

  if (m_clipplane_list != other.m_clipplane_list)
    return false;

  if (m_clipping_proof != other.m_clipping_proof)
    return false;

  if (m_section_attributes_source != other.m_section_attributes_source)
    return false;

  if (m_section_fill_rule != other.m_section_fill_rule)
    return false;

  if (m_section_hatch_index != other.m_section_hatch_index)
    return false;

  if (m_section_hatch_scale != other.m_section_hatch_scale)
    return false;

  if (m_section_hatch_rotation != other.m_section_hatch_rotation)
    return false;

  if (m_linetype_scale != other.m_linetype_scale)
    return false;

  if (m_hatch_background_fill != other.m_hatch_background_fill)
    return false;

  if (m_hatch_boundary_visible != other.m_hatch_boundary_visible)
    return false;

  const MYON_Linetype* customThis = m_custom_linetype.get();
  const MYON_Linetype* customOther = other.m_custom_linetype.get();
  if (customThis != customOther)
    return false;

  return true;
}

bool MYON_3dmObjectAttributesPrivate::operator!=(const MYON_3dmObjectAttributesPrivate& other) const
{
  return !MYON_3dmObjectAttributesPrivate::operator==(other);
}

static const MYON_3dmObjectAttributesPrivate DefaultAttributesPrivate(nullptr);

MYON_OBJECT_IMPLEMENT( MYON_3dmObjectAttributes, MYON_Object, "A828C015-09F5-477c-8665-F0482F5D6996" );

MYON_3dmObjectAttributes::MYON_3dmObjectAttributes()
  : m_color(0,0,0)
  , m_plot_color(0,0,0)
{
  // No longer need to call Default. All members are initialized in class declaration
  // Default();
}

MYON_3dmObjectAttributes::~MYON_3dmObjectAttributes()
{
  if (m_private)
    delete m_private;
}

void MYON_3dmObjectAttributes::CopyHelper(const MYON_3dmObjectAttributes& src)
{
  // private helper for the copy constructor and operator=.
  m_uuid = src.m_uuid;
  m_name = src.m_name;
  m_url  = src.m_url;
  m_layer_index = src.m_layer_index;
  m_linetype_index = src.m_linetype_index;
  m_material_index = src.m_material_index;
  m_rendering_attributes = src.m_rendering_attributes;
  m_color = src.m_color;
  m_plot_color = src.m_plot_color;
  m_display_order = src.m_display_order;
  m_plot_weight_mm = src.m_plot_weight_mm;
  m_object_decoration = src.m_object_decoration;
  m_wire_density = src.m_wire_density;
  m_viewport_id = src.m_viewport_id;
  m_space = src.m_space;
  m_bVisible = src.m_bVisible;
  m_mode = src.m_mode;
  m_color_source = src.m_color_source;
  m_plot_color_source = src.m_plot_color_source;
  m_plot_weight_source = src.m_plot_weight_source;
  m_material_source = src.m_material_source;
  m_linetype_source = src.m_linetype_source;
  m_reserved_0 = src.m_reserved_0;
  m_object_frame = src.m_object_frame;
  m_group = src.m_group;
  if (m_private)
    delete m_private;
  m_private = nullptr;
  if (src.m_private)
  {
    m_private = new MYON_3dmObjectAttributesPrivate(this);
    *m_private = *src.m_private;
  }
  m_dmref = src.m_dmref;
}

MYON_3dmObjectAttributes::MYON_3dmObjectAttributes(const MYON_3dmObjectAttributes& src) 
  : MYON_Object(src)
  , m_color(0, 0, 0)
  , m_plot_color(0, 0, 0)
{
  CopyHelper(src);
}

bool MYON_3dmObjectAttributes::operator==(const MYON_3dmObjectAttributes& other) const
{
  if ( MYON_UuidCompare( m_uuid, other.m_uuid ) )
    return false;
  if ( m_name.CompareOrdinal(other.m_name,false) )
    return false;
  if ( m_url.CompareOrdinal(other.m_url,false) )
    return false;
  if ( m_layer_index != other.m_layer_index )
    return false;
  if ( m_material_index != other.m_material_index )
    return false;
  if ( m_linetype_index != other.m_linetype_index )
    return false;
  if ( m_color != other.m_color )
    return false;
  if ( m_plot_color != other.m_plot_color )
    return false;
  if ( m_display_order != other.m_display_order )
    return false;
  if ( m_object_decoration != other.m_object_decoration )
    return false;
  if ( m_wire_density != other.m_wire_density )
    return false;
  if ( m_mode != other.m_mode )
    return false;
  if ( m_color_source != other.m_color_source )
    return false;
  if ( m_linetype_source != other.m_linetype_source )
    return false;
  if ( m_plot_color_source != other.m_plot_color_source )
    return false;
  if ( m_material_source != other.m_material_source )
    return false;
  if ( m_plot_weight_mm != other.m_plot_weight_mm )
    return false;
  if ( m_plot_weight_source != other.m_plot_weight_source )
    return false;

  int count = m_group.Count();
  if ( count != other.m_group.Count() )
    return false;
  if ( count > 0 )
  {
    const int* a = m_group.Array();
    const int* b = other.m_group.Array();
    if ( memcmp( a, b, count*sizeof(*a) ) )
      return false;
  }

  if ( m_bVisible != other.m_bVisible )
    return false;

  if ( m_rendering_attributes.Compare(other.m_rendering_attributes) )
    return false;

  if ( m_space != other.m_space)
    return false;

  if ( m_viewport_id != other.m_viewport_id )
    return false;

  if ( m_dmref != other.m_dmref )
    return false;

  if (m_object_frame != other.m_object_frame)
    return false;

  if (m_private && other.m_private)
  {
    if (*m_private != *other.m_private)
      return false;
  }
  else
  {
    if (m_private && *m_private != DefaultAttributesPrivate)
      return false;
    if (other.m_private && *other.m_private != DefaultAttributesPrivate)
      return false;
  }

  return true;
}

bool MYON_3dmObjectAttributes::operator!=(const MYON_3dmObjectAttributes& other) const
{
  return !MYON_3dmObjectAttributes::operator==(other);
}


MYON_3dmObjectAttributes& MYON_3dmObjectAttributes::operator=(const MYON_3dmObjectAttributes& src )
{
  if ( this != &src ) 
  {
    MYON_Object::operator=(src);
    CopyHelper(src);
  }
  return *this;
}

bool MYON_3dmObjectAttributes::UpdateReferencedComponents(
  const class MYON_ComponentManifest& source_manifest,
  const class MYON_ComponentManifest& destination_manifest,
  const class MYON_ManifestMap& manifest_map
  )
{
  bool rc = true;

  // Update layer reference
  if (m_layer_index >= 0)
  {
    int destination_layer_index = MYON_UNSET_INT_INDEX;
    if (manifest_map.GetAndValidateDestinationIndex(MYON_ModelComponent::Type::Layer, m_layer_index, destination_manifest, &destination_layer_index))
    {
      m_layer_index = destination_layer_index;
    }
    else
    {
      MYON_ERROR("Unable to update layer reference.");
      rc = false;
      m_layer_index = MYON_3dmObjectAttributes::DefaultAttributes.m_layer_index;
    }
  }

  // Update render material reference
  if (m_material_index >= 0)
  {
    int destination_material_index = MYON_UNSET_INT_INDEX;
    if (manifest_map.GetAndValidateDestinationIndex(MYON_ModelComponent::Type::RenderMaterial, m_material_index, destination_manifest, &destination_material_index))
    {
      m_material_index = destination_material_index;
    }
    else
    {
      MYON_ERROR("Unable to update render material reference.");
      rc = false;
      m_material_index = MYON_3dmObjectAttributes::DefaultAttributes.m_material_index;
    }
  }

  // Update line pattern reference
  if (m_linetype_index >= 0)
  {
    int destination_line_pattern_index = MYON_UNSET_INT_INDEX;
    if (manifest_map.GetAndValidateDestinationIndex(MYON_ModelComponent::Type::LinePattern, m_linetype_index, destination_manifest, &destination_line_pattern_index))
    {
      m_linetype_index = destination_line_pattern_index;
    }
    else
    {
      MYON_ERROR("Unable to update line pattern reference.");
      rc = false;
      m_linetype_index = MYON_3dmObjectAttributes::DefaultAttributes.m_linetype_index;
    }
  }

  // Update group references
  unsigned int group_count = 0;
  for (unsigned int i = 0; i < m_group.UnsignedCount(); i++)
  {
    int group_index = m_group[i];
    int destination_group_index_index = MYON_UNSET_INT_INDEX;
    if (!manifest_map.GetAndValidateDestinationIndex(MYON_ModelComponent::Type::Group, group_index, destination_manifest, &destination_group_index_index))
    {
      MYON_ERROR("Unable to update group reference.");
      rc = false;
      continue;
    }
    if (destination_group_index_index < 0)
    {
      MYON_ERROR("Unable to update group reference.");
      rc = false;
      continue;
    }
    m_group[group_count] = destination_group_index_index;
    group_count++;
  }
  m_group.SetCount(group_count);

  return rc;
}

void MYON_3dmObjectAttributes::Default()
{
  MYON_3dmObjectAttributes default_attrs;
  *this = default_attrs;
  // the following is no longer necessary now that initial values are defined in
  // class declaration
  /*
  PurgeUserData();
  m_uuid = MYON_nil_uuid;
  m_name.Destroy();
  m_url.Destroy();
  m_layer_index = 0;
  m_linetype_index = -1; // continuous
  m_material_index = -1; // white diffuse
  m_rendering_attributes.Default();
  m_color = MYON_Color(0,0,0);
  m_plot_color = MYON_Color(0,0,0); // Do not change to MYON_UNSET_COLOR
  m_display_order = 0;
  m_plot_weight_mm = 0.0;
  m_object_decoration = MYON::no_object_decoration;
  m_wire_density = 1;
  m_mode = MYON::normal_object;
  m_bVisible = true;
  m_color_source       = MYON::color_from_layer;
  m_material_source    = MYON::material_from_layer;
  m_linetype_source    = MYON::linetype_from_layer;
  m_plot_color_source  = MYON::plot_color_from_layer;
  m_plot_weight_source = MYON::plot_weight_from_layer;
  m_group.Destroy();
  m_space = MYON::model_space;
  m_viewport_id = MYON_nil_uuid;
  m_dmref.Destroy();

  if (m_private)
    delete m_private;
  m_private = nullptr;
  */
}


// {9BBB37E9-2131-4fb8-B9C6-5524859B98B8}
const MYON_UUID MYON_ObsoletePageSpaceObjectId =
{ 0x9bbb37e9, 0x2131, 0x4fb8, { 0xb9, 0xc6, 0x55, 0x24, 0x85, 0x9b, 0x98, 0xb8 } };

enum MYON_3dmObjectAttributesTypeCodes : unsigned char
{
  // TODO: add older itemids to this enum in order to make the read/write
  //       code easier for a developer to parse
  ObsoleteLineCapStyleSource = 23,
  ObsoleteLineCapStyle = 24,
  ObsoleteLineJoinStyleSource = 25,
  ObsoleteLineJoinStyle = 26,
  ClipParticipationSource = 27,
  SelectiveClippingData = 28,
  // 18 Oct 2021 S. Baer
  // file version 2.4: add section hatch attributes
  SectionAttributesSource = 29,
  SectionHatchIndex = 30,
  SectionHatchScale = 31,
  SectionHatchRotation = 32,
  // 5 Nov 2021 S. Baer
  // file version 2.5: add linetype scale
  LinetypeScale = 33,
  // 10 Jan 2022 S. Baer
  // file version 2.6: add hatch background fill, hatch boundary visible
  HatchBackgroundFill = 34,
  HatchBoundaryVisible = 35,
  // file version 2.7 was skipped
  // 26 Jan 2022 Andy Le Bihan
  // file version 2.8: object frame
  ObjectFrame = 36,
  // 15 Jun 2022 S. Baer
  // file version 2.9: SectionFillRule
  SectionFillRule = 37,
  // 30 Nov 2022 S. Baer
  // file version 2.10: custom linetype
  CustomLinetype = 38,

  // add items here
  LastAttributeTypeCode = 38
};

bool MYON_3dmObjectAttributes::Internal_ReadV5( MYON_BinaryArchive& file )
{
  unsigned char itemid = 0xFF;
  int major_version = 0;
  int minor_version = 0;
  bool rc = file.Read3dmChunkVersion(&major_version,&minor_version);
  if ( rc && 2 != major_version )
    rc = false;

  while(rc)
  {
    if (!rc) break;
    rc = file.ReadUuid(m_uuid);
    if (!rc) break;
    //rc = file.ReadInt(&m_layer_index);
    rc = file.Read3dmReferencedComponentIndex(MYON_ModelComponent::Type::Layer, &m_layer_index);
    if (!rc) break;

    // read non-default settings - skip everything else
    rc = file.ReadChar(&itemid);
    if (!rc) break;
    if ( 0 == itemid )
      break;
    
    if ( 1 == itemid )
    {
      MYON_wString name;
      rc = file.ReadString(name);
      if (!rc) break;
      SetName(name,true);

      rc = file.ReadChar(&itemid);
      if ( !rc || 0 == itemid ) break;
    }
    if ( 2 == itemid )
    {
      rc = file.ReadString(m_url);
      if (!rc) break;
      rc = file.ReadChar(&itemid);
      if ( !rc || 0 == itemid ) break;
    }
    if ( 3 == itemid )
    {
      //rc = file.ReadInt(&m_linetype_index);
      rc = file.Read3dmReferencedComponentIndex(MYON_ModelComponent::Type::LinePattern, &m_linetype_index);
      if (!rc) break;
      rc = file.ReadChar(&itemid);
      if ( !rc || 0 == itemid ) break;
    }
    if ( 4 == itemid )
    {
      //rc = file.ReadInt(&m_material_index);
      rc = file.Read3dmReferencedComponentIndex(MYON_ModelComponent::Type::RenderMaterial, &m_material_index);
      if (!rc) break;
      rc = file.ReadChar(&itemid);
      if ( !rc || 0 == itemid ) break;
    }
    if ( 5 == itemid )
    {
      rc = m_rendering_attributes.Read(file);
      if (!rc) break;
      rc = file.ReadChar(&itemid);
      if ( !rc || 0 == itemid ) break;
    }
    if ( 6 == itemid )
    {
      rc = file.ReadColor(m_color);
      if (!rc) break;
      rc = file.ReadChar(&itemid);
      if ( !rc || 0 == itemid ) break;
    }
    if ( 7 == itemid )
    {
      rc = file.ReadColor(m_plot_color);
      if (!rc) break;
      rc = file.ReadChar(&itemid);
      if ( !rc || 0 == itemid ) break;
    }
    if ( 8 == itemid )
    {
      rc = file.ReadDouble(&m_plot_weight_mm);
      if (!rc) break;
      rc = file.ReadChar(&itemid);
      if ( !rc || 0 == itemid ) break;
    }
    if ( 9 == itemid )
    {
      unsigned char c = 0;
      rc = file.ReadChar(&c);
      if (!rc) break;
      m_object_decoration = MYON::ObjectDecoration(c);
      rc = file.ReadChar(&itemid);
      if ( !rc || 0 == itemid ) break;
    }
    if ( 10 == itemid )
    {
      rc = file.ReadInt(&m_wire_density);
      if (!rc) break;
      rc = file.ReadChar(&itemid);
      if ( !rc || 0 == itemid ) break;
    }
    if ( 11 == itemid )
    {
      rc = file.ReadBool(&m_bVisible);
      if (!rc) break;
      rc = file.ReadChar(&itemid);
      if ( !rc || 0 == itemid ) break;
    }
    if ( 12 == itemid )
    {
      rc = file.ReadChar(&m_mode);
      if (!rc) break;
      rc = file.ReadChar(&itemid);
      if ( !rc || 0 == itemid ) break;
    }
    if ( 13 == itemid )
    {
      rc = file.ReadChar(&m_color_source);
      if (!rc) break;
      rc = file.ReadChar(&itemid);
      if ( !rc || 0 == itemid ) break;
    }
    if ( 14 == itemid )
    {
      rc = file.ReadChar(&m_plot_color_source);
      if (!rc) break;
      rc = file.ReadChar(&itemid);
      if ( !rc || 0 == itemid ) break;
    }
    if ( 15 == itemid )
    {
      rc = file.ReadChar(&m_plot_weight_source);
      if (!rc) break;
      rc = file.ReadChar(&itemid);
      if ( !rc || 0 == itemid ) break;
    }
    if ( 16 == itemid )
    {
      rc = file.ReadChar(&m_material_source);
      if (!rc) break;
      rc = file.ReadChar(&itemid);
      if ( !rc || 0 == itemid ) break;
    }
    if ( 17 == itemid )
    {
      rc = file.ReadChar(&m_linetype_source);
      if (!rc) break;
      rc = file.ReadChar(&itemid);
      if ( !rc || 0 == itemid ) break;
    }
    if ( 18 == itemid )
    {
      rc = file.Read3dmReferencedComponentIndexArray(MYON_ModelComponent::Type::Group, m_group);
      if (!rc) break;
      rc = file.ReadChar(&itemid);
      if ( !rc || 0 == itemid ) break;
    }
    if ( 19 == itemid )
    {
      unsigned char c = 0;
      rc = file.ReadChar(&c);
      if (!rc) break;
      m_space = MYON::ActiveSpace(c);
      rc = file.ReadChar(&itemid);
      if ( !rc || 0 == itemid ) break;
    }
    if ( 20 == itemid )
    {
      rc = file.ReadUuid(m_viewport_id);
      if (!rc) break;
      rc = file.ReadChar(&itemid);
      if ( !rc || 0 == itemid ) break;
    }
    if ( 21 == itemid )
    {
      rc = file.ReadArray(m_dmref);
      if (!rc) break;
      rc = file.ReadChar(&itemid);
      if ( !rc || 0 == itemid ) break;
    }

    // items 1 - 21 are in chunk version 2.0 files
    if ( minor_version <= 0 )
      break;

    // 28 Nov. 2009 - S. Baer
    // Added m_display_order to version 2.1 files
    if ( 22 == itemid )
    {
      rc = file.ReadInt(&m_display_order);
      if (!rc) break;
      rc = file.ReadChar(&itemid);
      if ( !rc || 0 == itemid ) break;
    }

    if ( minor_version <= 1 )
      break;

    // 12 Aug 2021 S. Baer
    // Items 23, 24, 25, 26 were in a version of Rhino 8 WIP for about 24 hours.
    // They were most likely never used by anyone, but just in case let's test,
    // read, and throw away.
    if (MYON_3dmObjectAttributesTypeCodes::ObsoleteLineCapStyleSource == itemid) //23
    {
      unsigned char source = 0;
      rc = file.ReadChar(&source);
      if (!rc) break;
      //SetLineCapStyleSource(MYON::ObjectLineCapStyleSourceFromUnsigned(source));
      rc = file.ReadChar(&itemid);
      if (!rc || 0 == itemid) break;
    }

    if (MYON_3dmObjectAttributesTypeCodes::ObsoleteLineCapStyle == itemid) //24
    {
      unsigned char style = 0;
      rc = file.ReadChar(&style);
      if (!rc) break;
      //SetLineCapStyle(MYON::LineCapStyleFromUnsigned(style));
      rc = file.ReadChar(&itemid);
      if (!rc || 0 == itemid) break;
    }

    if (MYON_3dmObjectAttributesTypeCodes::ObsoleteLineJoinStyleSource == itemid) //25
    {
      unsigned char source = 0;
      rc = file.ReadChar(&source);
      if (!rc) break;
      //SetLineJoinStyleSource(MYON::ObjectLineJoinStyleSourceFromUnsigned(source));
      rc = file.ReadChar(&itemid);
      if (!rc || 0 == itemid) break;
    }

    if (MYON_3dmObjectAttributesTypeCodes::ObsoleteLineJoinStyle == itemid) //26
    {
      unsigned char style = 0;
      rc = file.ReadChar(&style);
      if (!rc) break;
      //SetLineJoinStyle(MYON::LineJoinStyleFromUnsigned(style));
      rc = file.ReadChar(&itemid);
      if (!rc || 0 == itemid) break;
    }

    if (minor_version <= 2)
      break;

    if (MYON_3dmObjectAttributesTypeCodes::ClipParticipationSource == itemid) //27
    {
      unsigned char clip_participation_source = 0;
      rc = file.ReadChar(&clip_participation_source);
      if (!rc) break;
      SetClipParticipationSource(MYON::ClipParticipationSourceFromUnsigned(clip_participation_source));
      rc = file.ReadChar(&itemid);
      if (!rc || 0 == itemid) break;
    }

    if (MYON_3dmObjectAttributesTypeCodes::SelectiveClippingData == itemid) //28
    {
      bool clipping_proof = false;
      rc = file.ReadBool(&clipping_proof);
      if (!rc) break;
      MYON_UuidList selectiveClipping;
      rc = selectiveClipping.Read(file);
      if (!rc) break;
      if (clipping_proof)
        SetClipParticipationForNone();
      else
      {
        if (selectiveClipping.Count() > 0)
        {
          SetClipParticipationList(selectiveClipping.Array(), selectiveClipping.Count());
        }
      }
      rc = file.ReadChar(&itemid);
      if (!rc || 0 == itemid) break;
    }

    if (minor_version <= 3)
      break;

    if (MYON_3dmObjectAttributesTypeCodes::SectionAttributesSource == itemid) //29
    {
      unsigned char section_attr_source = 0;
      rc = file.ReadChar(&section_attr_source);
      if (!rc) break;
      SetSectionAttributesSource(MYON::SectionAttributesSourceFromUnsigned(section_attr_source));
      rc = file.ReadChar(&itemid);
      if (!rc || 0 == itemid) break;
    }
    if (MYON_3dmObjectAttributesTypeCodes::SectionHatchIndex == itemid) //30
    {
      int hatch_index = 0;
      rc = file.Read3dmReferencedComponentIndex(MYON_ModelComponent::Type::HatchPattern, &hatch_index);
      if (!rc) break;
      SetSectionHatchIndex(hatch_index);
      rc = file.ReadChar(&itemid);
      if (!rc || 0 == itemid) break;
    }
    if (MYON_3dmObjectAttributesTypeCodes::SectionHatchScale == itemid) //31
    {
      double hatch_scale = 1;
      rc = file.ReadDouble(&hatch_scale);
      if (!rc) break;
      SetSectionHatchScale(hatch_scale);
      rc = file.ReadChar(&itemid);
      if (!rc || 0 == itemid) break;
    }
    if (MYON_3dmObjectAttributesTypeCodes::SectionHatchRotation == itemid) //32
    {
      double hatch_rotation = 0;
      rc = file.ReadDouble(&hatch_rotation);
      if (!rc) break;
      SetSectionHatchRotation(hatch_rotation);
      rc = file.ReadChar(&itemid);
      if (!rc || 0 == itemid) break;
    }

    if (minor_version <= 4)
      break;

    if (MYON_3dmObjectAttributesTypeCodes::LinetypeScale == itemid) //33
    {
      double scale = 1.0;
      rc = file.ReadDouble(&scale);
      if (!rc) break;
      SetLinetypePatternScale(scale);
      rc = file.ReadChar(&itemid);
      if (!rc || 0 == itemid) break;
    }

    if (minor_version <= 5)
      break;

    if (MYON_3dmObjectAttributesTypeCodes::HatchBackgroundFill == itemid) // 34
    {
      MYON_Color color = MYON_Color::UnsetColor;
      rc = file.ReadColor(color);
      if (!rc) break;
      SetHatchBackgrounFillColor(color);
      rc = file.ReadChar(&itemid);
      if (!rc || 0 == itemid) break;
    }

    if (MYON_3dmObjectAttributesTypeCodes::HatchBoundaryVisible == itemid) // 35
    {
      bool visible = false;
      rc = file.ReadBool(&visible);
      if (!rc) break;
      SetHatchBoundaryVisible(visible);
      rc = file.ReadChar(&itemid);
      if (!rc || 0 == itemid) break;
    }

    if (minor_version <= 6)
      break;

    // nothing added in 2.7
    if (minor_version <= 7)
      break;

    if (MYON_3dmObjectAttributesTypeCodes::ObjectFrame == itemid) // 36
    {
      MYON_Xform object_frame = MYON_Xform::Nan;
      rc = file.ReadXform(object_frame);
      if (!rc) break;

      m_object_frame = MYON_Plane::World_xy;
      m_object_frame.Transform(object_frame);

      rc = file.ReadChar(&itemid);
      if (!rc || 0 == itemid) break;
    }

    if (minor_version <= 8)
      break;

    if (MYON_3dmObjectAttributesTypeCodes::SectionFillRule == itemid) // 37
    {
      unsigned char c = 0;
      rc = file.ReadChar(&c);
      if (!rc) break;

      SetSectionFillRule(MYON::SectionFillRuleFromUnsigned(c));

      rc = file.ReadChar(&itemid);
      if (!rc || 0 == itemid) break;
    }

    if (minor_version <= 9)
      break;

    if (MYON_3dmObjectAttributesTypeCodes::CustomLinetype == itemid) // 38
    {
      MYON_Linetype lt;
      rc = lt.Read(file);
      if (!rc) break;

      SetCustomLinetype(lt);

      rc = file.ReadChar(&itemid);
      if (!rc || 0 == itemid) break;
    }

    if (minor_version <= 10)
      break;

    // Add new item reading above and increment the LastAttributeTypeCode value
    // in the enum. Be sure to test reading of old and new files by old and new
    // code, before checking in your changes.
    //
    if ( itemid > MYON_3dmObjectAttributesTypeCodes::LastAttributeTypeCode )
    {
      // we are reading file written with code newer
      // than this code (minor_version > 2)
      itemid = 0;
    }

    break;
  }

  if ( rc && 0 != itemid )
  {
    MYON_ERROR("Bug in MYON_3dmObjectAttributes::ReadV5Helper or WriteV5Helper");
  }

  return rc;
}

bool MYON_3dmObjectAttributes::Read( MYON_BinaryArchive& file )
{
  Default();
  if (    file.Archive3dmVersion() >= 5 
       && file.ArchiveOpenNURBSVersion() >= 200712190 )
  {
    return Internal_ReadV5(file);
  }
  int i;
  int major_version = 0;
  int minor_version = 0;
  bool rc = file.Read3dmChunkVersion(&major_version,&minor_version);
  if ( rc && major_version == 1 ) 
  {
    if (rc) rc = file.ReadUuid(m_uuid);
    if (rc) rc = file.Read3dmReferencedComponentIndex(MYON_ModelComponent::Type::Layer,&m_layer_index);
    if (rc) rc = file.Read3dmReferencedComponentIndex(MYON_ModelComponent::Type::RenderMaterial,&m_material_index);
    if (rc) rc = file.ReadColor(m_color);
    
    while(rc)
    {
      // OBSOLETE if (rc) rc = file.ReadLineStyle(m_line_style); // 23 March 2005 Dale Lear
      // replaced with
      short s = 0;
      double x;
      rc = file.ReadShort(&s); 
      if (!rc) break;
      if ( file.Archive3dmVersion() < 4 || file.ArchiveOpenNURBSVersion() < 200503170 )
      {
        // ignore unused linestyle info in old files
        // This bit keeps the curve arrowheads from V3 showing up
        // in V4.
        m_object_decoration = MYON::ObjectDecoration( (s & MYON::both_arrowhead) );
      }
      rc = file.ReadShort(&s);
      if (!rc) break;
      rc = file.ReadDouble(&x);
      if (!rc) break;
      rc = file.ReadDouble(&x);
      break;
    }

    if (rc) rc = file.ReadInt(&m_wire_density);
    if (rc) rc = file.ReadChar(&m_mode);

    if (rc) rc = file.ReadChar(&m_color_source);
    if (rc) m_color_source = (unsigned char)MYON::ObjectColorSource(m_color_source);

    if (rc) rc = file.ReadChar(&m_linetype_source);
    if (rc) m_linetype_source = (unsigned char)MYON::ObjectLinetypeSource(m_linetype_source);

    if (rc) rc = file.ReadChar(&m_material_source);
    if (rc) m_material_source = (unsigned char)MYON::ObjectMaterialSource(m_material_source);

    MYON_wString name;
    if (rc) rc = file.ReadString(name);
    if (rc)
      SetName(name, true);

    if (rc) rc = file.ReadString(m_url);

    m_bVisible = (Mode() != MYON::hidden_object);
    if ( rc && minor_version >= 1 ) 
    {
      rc = file.Read3dmReferencedComponentIndexArray(MYON_ModelComponent::Type::Group, m_group);
      if ( rc && minor_version >= 2 )
      {
        rc = file.ReadBool(&m_bVisible);

        if ( rc && minor_version >= 3 )
        {
          rc = file.ReadArray(m_dmref);     

          if (rc && minor_version >= 4 )
          {
            // 23 March 2005 Dale Lear
            //    Added m_plot_color_source and m_plot_color
            i = 0;
            if (rc) rc = file.ReadInt(&i);
            if (rc) m_object_decoration = MYON::ObjectDecoration(i);
            if (rc) rc = file.ReadChar(&m_plot_color_source);
            if (rc) m_plot_color_source = (unsigned char)MYON::PlotColorSource(m_plot_color_source);
            if (rc) rc = file.ReadColor( m_plot_color );
            if (rc) rc = file.ReadChar(&m_plot_weight_source);
            if (rc) m_plot_weight_source = (unsigned char)MYON::PlotWeightSource(m_plot_weight_source);
            if (rc) rc = file.ReadDouble(&m_plot_weight_mm);


            if (rc && minor_version >= 5 )
            {
              // version 1.5 fields 11 April 2005
              if (rc) rc = file.Read3dmReferencedComponentIndex(MYON_ModelComponent::Type::LinePattern,&m_linetype_index);

              // version 1.6 fields 2 September 2005
              if (rc && minor_version >= 6 )
              {
                unsigned char uc = 0;
                rc = file.ReadChar(&uc);
                if (rc)
                {
                  m_space = (1 == uc) ? MYON::page_space : MYON::model_space;
                  m_dmref.Empty();
                  int i_local, count=0;
                  rc = file.ReadInt(&count);
                  if (rc && count > 0)
                  {
                    m_dmref.SetCapacity(count);
                    for ( i_local = 0; i_local < count && rc; i_local++)
                    {
                      MYON_DisplayMaterialRef& dmr = m_dmref.AppendNew();
                      rc = file.ReadUuid(dmr.m_viewport_id);
                      if (rc) rc = file.ReadUuid(dmr.m_display_material_id);
                      if ( rc )
                      {
                        // Assigning an object to a page started out as
                        // using dmrs.  The way the runtime info is saved
                        // has changed, but, at this point, I can't change
                        // the way the information is saved in the file and
                        // it doesn't matter.
                        if ( 0 == MYON_UuidCompare(&MYON_ObsoletePageSpaceObjectId,&dmr.m_display_material_id) )
                        {
                          m_viewport_id = dmr.m_viewport_id;
                          m_dmref.Remove();
                        }
                      }
                    }
                    if ( 0 == m_dmref.Count() )
                      m_dmref.Destroy();
                  }
                }

                if ( rc && minor_version >= 7 )
                {
                  // version 1.7 fields 6 June 2006
                  if (rc) rc = m_rendering_attributes.Read(file);
                }
              }
            }
          }
        }
      }
    }
  }
  else 
  {
    rc = false;
  }
  return rc;
}

bool MYON_3dmObjectAttributes::Internal_WriteV5( MYON_BinaryArchive& file ) const
{
  unsigned char c;
  // 29 Nov. 2009 S. Baer
  // Chunk version updated to 2.1 in order to support m_display_order
  // 12 Aug 2021 S. Baer
  // The next minor chunk version should be 3. There was a 24 hour period
  // where files could contain some extra information in a 2.2 chunk
  // 29 Sept 2021 S. Baer
  // Chunk version updated to 2.3 to support ClipParticipationSource and
  // clip participation list
  // 18 Oct 2021 S. Baer
  // Chunk version = 2.4 to support section hatch attributes
  // 5 Nov 2021 S. Baer
  // Chunk version = 2.5 to support linetype scale
  // 10 Jan 2022 S. Baer
  // Chunk version = 2.6 to support hatch background fill and hatch boundary visibility
  // 26 Jan 2022 Andy Le Bihan
  // Chunk version 2.7 was skipped
  // Chunk version = 2.8 to support object frame.
  // 15 Jun 2022 S. Baer
  // Chunk version = 2.9 to support SectionFillRule
  // 30 Nov 2022 S. Baer
  // Chunk version = 2.10 to support 
  bool rc = file.Write3dmChunkVersion(2,10);
  while(rc)
  {
    if (!rc) break;
    rc = file.WriteUuid(m_uuid);
    if (!rc) break;
    rc = file.Write3dmReferencedComponentIndex( MYON_ModelComponent::Type::Layer, m_layer_index);
    if (!rc) break;

    // write non-default settings - skip everything else
    if ( !m_name.IsEmpty() )
    {
      c = 1;
      rc = file.WriteChar(c);
      if (!rc) break;
      rc = file.WriteString(m_name);
      if (!rc) break;
    }
    if ( !m_url.IsEmpty() )
    {
      c = 2;
      rc = file.WriteChar(c);
      if (!rc) break;
      rc = file.WriteString(m_url);
      if (!rc) break;
    }
    if ( -1 != m_linetype_index )
    {
      c = 3;
      rc = file.WriteChar(c);
      if (!rc) break;
      rc = file.Write3dmReferencedComponentIndex( MYON_ModelComponent::Type::LinePattern, m_linetype_index);
      if (!rc) break;
    }
    if ( -1 != m_material_index && MaterialSource() == MYON::material_from_object)
    {
      c = 4;
      rc = file.WriteChar(c);
      if (!rc) break;
      rc = file.Write3dmReferencedComponentIndex( MYON_ModelComponent::Type::RenderMaterial, m_material_index);
      if (!rc) break;
    }
    if (    m_rendering_attributes.m_mappings.Count() > 0
         || m_rendering_attributes.m_materials.Count() > 0
         || true != m_rendering_attributes.m_bCastsShadows
         || true != m_rendering_attributes.m_bReceivesShadows
         || false != m_rendering_attributes.AdvancedTexturePreview()
         )
    {
      c = 5;
      rc = file.WriteChar(c);
      if (!rc) break;
      rc = m_rendering_attributes.Write(file);
      if (!rc) break;
    }
    if ( 0 != m_color )
    {
      c = 6;
      rc = file.WriteChar(c);
      if (!rc) break;
      rc = file.WriteColor(m_color);
      if (!rc) break;
    }
    if ( 0 != m_plot_color )
    {
      c = 7;
      rc = file.WriteChar(c);
      if (!rc) break;
      rc = file.WriteColor(m_plot_color);
      if (!rc) break;
    }
    if ( 0.0 != m_plot_weight_mm )
    {
      c = 8;
      rc = file.WriteChar(c);
      if (!rc) break;
      rc = file.WriteDouble(m_plot_weight_mm);
      if (!rc) break;
    }
    if ( MYON::no_object_decoration != m_object_decoration )
    {
      c = 9;
      rc = file.WriteChar(c);
      if (!rc) break;
      c = (unsigned char)m_object_decoration;
      rc = file.WriteChar(c);
      if (!rc) break;
    }
    if ( 1 != m_wire_density )
    {
      c = 10;
      rc = file.WriteChar(c);
      if (!rc) break;
      rc = file.WriteInt(m_wire_density);
      if (!rc) break;
    }
    if ( true != m_bVisible )
    {
      c = 11;
      rc = file.WriteChar(c);
      if (!rc) break;
      rc = file.WriteBool(m_bVisible);
      if (!rc) break;
    }
    if ( MYON::normal_object != m_mode )
    {
      c = 12;
      rc = file.WriteChar(c);
      if (!rc) break;
      rc = file.WriteChar(m_mode);
      if (!rc) break;
    }
    if ( MYON::color_from_layer != m_color_source )
    {
      c = 13;
      rc = file.WriteChar(c);
      if (!rc) break;
      rc = file.WriteChar(m_color_source);
      if (!rc) break;
    }
    if ( MYON::plot_color_from_layer != m_plot_color_source )
    {
      c = 14;
      rc = file.WriteChar(c);
      if (!rc) break;
      rc = file.WriteChar(m_plot_color_source);
      if (!rc) break;
    }
    if ( MYON::plot_weight_from_layer != m_plot_weight_source )
    {
      c = 15;
      rc = file.WriteChar(c);
      if (!rc) break;
      rc = file.WriteChar(m_plot_weight_source);
      if (!rc) break;
    }
    if ( MYON::material_from_layer != m_material_source )
    {
      c = 16;
      rc = file.WriteChar(c);
      if (!rc) break;
      rc = file.WriteChar(m_material_source);
      if (!rc) break;
    }
    if ( MYON::linetype_from_layer != m_linetype_source )
    {
      c = 17;
      rc = file.WriteChar(c);
      if (!rc) break;
      rc = file.WriteChar(m_linetype_source);
      if (!rc) break;
    }
    if ( m_group.Count() > 0 )
    {
      c = 18;
      rc = file.WriteChar(c);
      if (!rc) break;
      int count = m_group.Count();
      rc = file.WriteInt(count);
      for ( int i = 0; i < count && rc; i++)
        rc = file.Write3dmReferencedComponentIndex(MYON_ModelComponent::Type::Group, m_group[i]);
      if (!rc) break;
    }
    if ( MYON::model_space != m_space )
    {
      c = 19;
      rc = file.WriteChar(c);
      if (!rc) break;
      c = (unsigned char)m_space;
      rc = file.WriteChar(c);
      if (!rc) break;
    }
    if ( !MYON_UuidIsNil(m_viewport_id) )
    {
      c = 20;
      rc = file.WriteChar(c);
      if (!rc) break;
      rc = file.WriteUuid(m_viewport_id);
      if (!rc) break;
    }
    if ( m_dmref.Count() > 0 )
    {
      c = 21;
      rc = file.WriteChar(c);
      if (!rc) break;
      rc = file.WriteArray(m_dmref);
      if (!rc) break;
    }

    // 29 Nov. 2009 - S. Baer
    // Only write m_display_order if it's value!=0
    // m_display_order is written to version 2.1 files
    if ( 0 != m_display_order )
    {
      c = 22;
      rc = file.WriteChar(c);
      if (!rc) break;
      rc = file.WriteInt(m_display_order);
      if (!rc) break;
    }

    // 12 Aug 2021 S. Baer
    // Items 23, 24, 25, 26 were in a version of Rhino 8 WIP for about 24 hours.
    // They were most likely never used by anyone

    // New attribute non-default values are all held in the m_private variable.
    // If this variable is nullptr, then all of the following properties will
    // be default which means there is no need to check their state
    if (m_private)
    {
      if (ClipParticipationSource() != MYON::ClipParticipationSource::FromLayer)
      {
        c = MYON_3dmObjectAttributesTypeCodes::ClipParticipationSource; // 27
        rc = file.WriteChar(c);
        if (!rc) break;
        rc = file.WriteChar((unsigned char)ClipParticipationSource());
        if (!rc) break;
      }

      {
        bool forAllClippingPlanes = true;
        bool forNoClippingPlanes = false;
        MYON_UuidList selectiveClipping;
        GetClipParticipation(forAllClippingPlanes, forNoClippingPlanes, selectiveClipping);
        if (!forAllClippingPlanes)
        {
          c = MYON_3dmObjectAttributesTypeCodes::SelectiveClippingData; // 28
          rc = file.WriteChar(c);
          if (!rc) break;
          rc = file.WriteBool(forNoClippingPlanes);
          if (!rc) break;
          rc = selectiveClipping.Write(file);
          if (!rc) break;
        }
      }

      if (SectionAttributesSource() != MYON::SectionAttributesSource::FromLayer)
      {
        c = MYON_3dmObjectAttributesTypeCodes::SectionAttributesSource; // 29
        rc = file.WriteChar(c);
        if (!rc) break;
        rc = file.WriteChar((unsigned char)SectionAttributesSource());
        if (!rc) break;
      }

      if (SectionHatchIndex() != MYON_UNSET_INT_INDEX)
      {
        c = MYON_3dmObjectAttributesTypeCodes::SectionHatchIndex; // 30
        rc = file.WriteChar(c);
        if (!rc) break;
        rc = file.Write3dmReferencedComponentIndex(MYON_ModelComponent::Type::HatchPattern, SectionHatchIndex());
        if (!rc) break;
      }

      if (SectionHatchScale() != 1.0)
      {
        c = MYON_3dmObjectAttributesTypeCodes::SectionHatchScale; // 31
        rc = file.WriteChar(c);
        if (!rc) break;
        rc = file.WriteDouble(SectionHatchScale());
        if (!rc) break;
      }

      if (SectionHatchRotation() != 0.0)
      {
        c = MYON_3dmObjectAttributesTypeCodes::SectionHatchRotation; // 32
        rc = file.WriteChar(c);
        if (!rc) break;
        rc = file.WriteDouble(SectionHatchRotation());
        if (!rc) break;
      }

      if (fabs(1.0 - LinetypePatternScale()) > MYON_EPSILON)
      {
        c = MYON_3dmObjectAttributesTypeCodes::LinetypeScale; // 33
        rc = file.WriteChar(c);
        if (!rc) break;
        rc = file.WriteDouble(LinetypePatternScale());
        if (!rc) break;
      }

      if (HatchBackgroundFillColor() != MYON_Color::UnsetColor)
      {
        c = MYON_3dmObjectAttributesTypeCodes::HatchBackgroundFill; // 34
        rc = file.WriteChar(c);
        if (!rc) break;
        rc = file.WriteColor(HatchBackgroundFillColor());
        if (!rc) break;
      }

      if (HatchBoundaryVisible() != false)
      {
        c = MYON_3dmObjectAttributesTypeCodes::HatchBoundaryVisible; // 35
        rc = file.WriteChar(c);
        if (!rc) break;
        rc = file.WriteBool(HatchBoundaryVisible());
        if (!rc) break;
      }

      // Have the decal collection update the user data if anything has changed.
      const unsigned int archive_3dm_version = file.Archive3dmVersion();
      m_private->m_decals.UpdateUserData(archive_3dm_version);
    }

    // 15 Jun 2022 S. Baer
    // Do not write items if they are their default value.
    if (m_object_frame != MYON_Plane::UnsetPlane)
    {
      // 26 Jan 2022 Andy Le Bihan
      // Always write the object frame - it will always be initialized.
      c = MYON_3dmObjectAttributesTypeCodes::ObjectFrame;
      rc = file.WriteChar(c);
      if (!rc) break;

      MYON_Xform xform;
      xform.ChangeBasis(m_object_frame, MYON_Plane::World_xy);
      rc = file.WriteXform(xform);
      if (!rc) break;
    }

    // 15 Jun 2022 S. Baer
    // Write section fill rule
    if (SectionFillRule() != MYON::SectionFillRule::ClosedCurves)
    {
      c = MYON_3dmObjectAttributesTypeCodes::SectionFillRule; // 37
      rc = file.WriteChar(c);
      if (!rc) break;
      rc = file.WriteChar((unsigned char)SectionFillRule());
      if (!rc) break;
    }

    // 30 Nov 2022 S. Baer
    // Write custom linetype
    {
      const MYON_Linetype* linetype = CustomLinetype();
      if (linetype)
      {
        c = MYON_3dmObjectAttributesTypeCodes::CustomLinetype; // 38
        rc = file.WriteChar(c);
        if (!rc) break;
        rc = linetype->Write(file);
        if (!rc) break;
      }
    }

    // 0 indicates end of attributes - this should be the last item written
    c = 0;
    rc = file.WriteChar(c);
    break;
  }
  return rc;
}

bool MYON_3dmObjectAttributes::Write( MYON_BinaryArchive& file ) const
{
  if ( file.Archive3dmVersion() >= 5 )
  {
    // added at opennurbs version 200712190
    return Internal_WriteV5(file);
  }

  bool rc = file.Write3dmChunkVersion(1,8);
  // version 1.0 fields
  if (rc) rc = file.WriteUuid(m_uuid);
  if (rc) rc = file.Write3dmReferencedComponentIndex( MYON_ModelComponent::Type::Layer, m_layer_index);
  if (rc) rc = file.Write3dmReferencedComponentIndex( MYON_ModelComponent::Type::RenderMaterial, m_material_index);
  if (rc) rc = file.WriteColor(m_color);

  if (rc)
  {
    // OBSOLETE if (rc) rc = file.WriteLineStyle(m_line_style); // 23 March 2005 Dale Lear
    short s;
    s = (short)m_object_decoration;
    if (rc) rc = file.WriteShort(s);
    s = 0;
    if (rc) rc = file.WriteShort(s);
    if (rc) rc = file.WriteDouble(0.0);
    if (rc) rc = file.WriteDouble(1.0);
  }

  if (rc) rc = file.WriteInt(m_wire_density);
  if (rc) rc = file.WriteChar(m_mode);
  if (rc) rc = file.WriteChar(m_color_source);
  if (rc) rc = file.WriteChar(m_linetype_source);
  if (rc) rc = file.WriteChar(m_material_source);
  if (rc) rc = file.WriteString(m_name);
  if (rc) rc = file.WriteString(m_url);

  // version 1.1 fields
  int count = m_group.Count();
  rc = file.WriteInt(count);
  for ( int i = 0; i < count && rc; i++)
    rc = file.Write3dmReferencedComponentIndex(MYON_ModelComponent::Type::Group, m_group[i]);

  // version 1.2 fields
  if (rc) rc = file.WriteBool(m_bVisible);

  // version 1.3 fields
  if (rc) rc = file.WriteArray(m_dmref);

  // version 1.4 fields - 23 March 2005 Dale Lear
  if (rc) rc = file.WriteInt(m_object_decoration);
  if (rc) rc = file.WriteChar(m_plot_color_source);
  if (rc) rc = file.WriteColor(m_plot_color);
  if (rc) rc = file.WriteChar(m_plot_weight_source);
  if (rc) rc = file.WriteDouble(m_plot_weight_mm);

  // version 1.5 fields 11 April 2005
  if (rc) rc = file.Write3dmReferencedComponentIndex( MYON_ModelComponent::Type::LinePattern, m_linetype_index );

  // version 1.6 fields 2 September 2005
  if (rc)
  {
    unsigned char uc = 0;
    switch(m_space)
    {
    case MYON::no_space:    uc = 0; break;
    case MYON::model_space: uc = 0; break;
    case MYON::page_space:  uc = 1; break;
    case MYON::uveditor_space:
    case MYON::blockeditor_space:
        {
            uc = 2;
            MYON_ASSERT(false);
            break;
        }
    }
    rc = file.WriteChar(uc);
  }
  if (rc)
  {
    // 22 Sep 2006 - the way MYON_3dmObjectAttiributes indicates
    // that an object is put on a particular page view changed
    // from being saved in the m_dmref[] list to using the
    // m_space and m_viewport_id settings.  But the file format
    // cannot change at this point.  So, the bAddPagespaceDMR
    // is here to save the page info in the old dmr format.
    int count_local = m_dmref.Count();
    if ( count_local < 0 )
      count_local = 0;
    bool bAddPagespaceDMR = ( MYON::page_space == m_space && !MYON_UuidIsNil(m_viewport_id) );
    rc = file.WriteInt( bAddPagespaceDMR ? (count_local+1) : count_local );
    if ( rc && bAddPagespaceDMR )
    {
      rc = file.WriteUuid(m_viewport_id);
      if (rc) rc = file.WriteUuid(MYON_ObsoletePageSpaceObjectId);
    }
    int i;
    for ( i = 0; i < count_local && rc; i++ )
    {
      const MYON_DisplayMaterialRef& dmr = m_dmref[i];
      rc = file.WriteUuid(dmr.m_viewport_id);
      if (rc) rc = file.WriteUuid(dmr.m_display_material_id);
    }
  }

  // version 1.7 fields 6 June 2006
  if (rc) rc = m_rendering_attributes.Write(file);

  return rc;
}


bool MYON_3dmObjectAttributes::Transform( const MYON_Xform& xform )
{
  // Please discuss any changes with Dale Lear.
  MYON_Object::TransformUserData(xform);

  //https://mcneel.myjetbrains.com/youtrack/issue/RH-20531
  //m_object_frame.Transform(xform);

  return m_rendering_attributes.Transform(xform);
}

bool MYON_3dmObjectAttributes::Transform(const MYON_Geometry* pOriginalGeometry, const MYON_Xform& xform)
{
  // Please discuss any changes with Dale Lear.
  MYON_Object::TransformUserData(xform);

  //https://mcneel.myjetbrains.com/youtrack/issue/RH-20531

  //This assumes that the incoming geometry has already been transformed, so if this is initializing
  //the object frame, it doesn't need to be transformed.
  if (m_object_frame.IsValid())
  {
    m_object_frame.Transform(xform);
  }
  /*else if (pOriginalGeometry)
  {
    MYON_Plane plane = MYON_Plane::World_xy;
    plane.SetOrigin(pOriginalGeometry->BoundingBox().Center());

    m_object_frame.Transform(xform);
  }*/

  return m_rendering_attributes.Transform(xform);
}

bool MYON_3dmObjectAttributes::IsValid( MYON_TextLog* text_log ) const
{
  if ( MYON_UuidIsNil(m_uuid) )
  {
    if ( text_log )
    {
      text_log->Print("Object id is nil - this is not valid.\n");
    }
    return false;
  }

  if ( !m_rendering_attributes.IsValid(text_log) )
  {
    if ( text_log )
    {
      text_log->Print("Object rendering attributes are not valid.\n");
    }
    return false;
  }

  return true;
}

unsigned int MYON_3dmObjectAttributes::SizeOf() const
{
  unsigned int sz = sizeof(*this) - sizeof(MYON_Object) 
                  + m_name.Length()*sizeof(wchar_t)
                  + m_url.Length()*sizeof(wchar_t)
                  + m_group.SizeOfArray()
                  + MYON_Object::SizeOf();
  return sz;
}

// 12 Aug 2021 S. Baer
// When adding new fields written to 3dm files, always add information to this
// Dump function. Dump is used by the opennurbs file testing framework to
// perform comparisons and is useful for manual comparison in when tests fail.
void MYON_3dmObjectAttributes::Dump( MYON_TextLog& dump ) const
{
  const wchar_t* wsName = static_cast< const wchar_t* >(m_name);
  if ( !wsName )
    wsName = L"";
  dump.Print("object name = \"%ls\"\n",wsName);

  dump.Print("object uuid = ");
  dump.Print(m_uuid);
  dump.Print("\n");

  const char* sMode = "unknown";
  switch( Mode() )
  {
  case MYON::normal_object:
    sMode = "normal";
    break;
  case MYON::hidden_object:
    sMode = "hidden";
    break;
  case MYON::locked_object:
    sMode = "locked";
    break;
  default:
    sMode = "unknown";
    break;
  }
  dump.Print("object mode = %s\n",sMode); // sSMode is const char*

  dump.Print("object layer index = %d\n",m_layer_index);

  const MYON::object_material_source mat_source = MaterialSource();

  if (MYON::object_material_source::material_from_object == mat_source || false == dump.IsTextHash())
  {
    // Depending on when a 3dm file was written, m_material_index may get set to an implicit -1 
    // during writing as part of an old attempt to reduce the size of attributes when saved in .3dm files.
    // When mat_source is not (MYON::object_material_source::material_from_object. This causes the
    // 3dm content to vary in a way that content hashing must ignore. The sample file
    // C:\dev\github\mcneel\rhino\src4\opennurbs\example_files\V5\v5_teacup.3dm is an example.
    // It's old enough that it contians material index values >= 0 that are not saved
    // by SaveAs V5 writing code since circa 2010 or earlier.
    dump.Print("object material index = %d\n", m_material_index);
  }

  const char* sMaterialSource = "unknown";
  switch(MaterialSource()) {
  case MYON::object_material_source::material_from_layer: sMaterialSource = "layer material"; break;
  case MYON::object_material_source::material_from_object: sMaterialSource = "object material"; break;
  case MYON::object_material_source::material_from_parent: sMaterialSource = "parent material"; break;
  }
  dump.Print("material source = %s\n",sMaterialSource); // sMaterialSource is const char*
  const int group_count = GroupCount();
  if ( group_count > 0 ) {
    const int* group = GroupList();
    dump.Print("groups: ");
    int i;
    for ( i = 0; i < group_count; i++ ) {
      if ( i )
        dump.Print(",%d",group[i]);
      else
        dump.Print("%d",group[i]);
    }
    dump.Print("\n");
  }

  const MYON_Linetype* linetype = CustomLinetype();
  if (nullptr == linetype)
  {
    dump.Print("no custom linetype\n");
  }
  else
  {
    dump.Print("contains custom linetype\n");
  }
}

MYON::object_mode MYON_3dmObjectAttributes::Mode() const
{
  return MYON::ObjectMode( m_mode%16 );
}

void MYON_3dmObjectAttributes::SetMode( MYON::object_mode m )
{
  int om = MYON::ObjectMode(m);
  if ( om >= 16 || om < 0 )
    om = 0;
  m_mode = (unsigned char)om;

  // temporary
  m_bVisible = (om != MYON::hidden_object);
}

bool MYON_3dmObjectAttributes::IsInstanceDefinitionObject() const
{
  return (MYON::idef_object == Mode());
}

bool MYON_3dmObjectAttributes::IsVisible() const
{
  return m_bVisible;
}

void MYON_3dmObjectAttributes::SetVisible( bool bVisible )
{
  if ( m_bVisible != (bVisible?true:false) )
  {
    m_bVisible = (bVisible?true:false);

    // temporary
    if ( Mode() != MYON::idef_object )
      SetMode( m_bVisible ? MYON::normal_object : MYON::hidden_object );
  }
}

bool MYON_3dmObjectAttributes::SetName(
  const wchar_t* candidate_name,
  bool bFixInvalidName

)
{
  MYON_wString name(candidate_name);
  name.TrimLeftAndRight();
  bool rc = name.IsEmpty() || MYON_ModelComponent::IsValidComponentName(name);
  if (false == rc && bFixInvalidName)
  {
    // Because MYON_3dmObjectAttributes.m_name is public, it has been set to strings
    // that are not valid names. Some of these strings begin with a bracket that
    // is permitted to appear later in the name. Prepending a carrot makes the names
    // unique while not turning them into something that the Rhino or python parsers
    // keey off of in some other way.
    // way to make these names
    MYON_wString prefixed_name = '^';
    prefixed_name += name;
    rc = MYON_ModelComponent::IsValidComponentName(prefixed_name);
    if (rc)
      name = prefixed_name;
  }
  m_name = rc ? name : MYON_wString::EmptyString;
  return rc;
}

const MYON_wString MYON_3dmObjectAttributes::Name() const
{
  return m_name;
}


unsigned int MYON_3dmObjectAttributes::ApplyParentalControl(
        const MYON_3dmObjectAttributes& parents_attributes,
        const MYON_Layer& parent_layer,
        unsigned int control_limits
        )
{
  unsigned int rc = ApplyParentalControl(parents_attributes, parent_layer, MYON_nil_uuid, control_limits);
  return rc;
}

unsigned int MYON_3dmObjectAttributes::ApplyParentalControl(
  const MYON_3dmObjectAttributes& parents_attributes,
  const MYON_Layer& parent_layer,
  const MYON_UUID& viewport_id,
  unsigned int control_limits
)
{
  unsigned int rc = 0;

  if (m_bVisible && !parents_attributes.m_bVisible)
  {
    if (0 != (0x01 & control_limits))
    {
      rc |= 0x01;
      m_bVisible = false;
    }
  }

  if (MYON::color_from_parent == m_color_source)
  {
    if (0 != (0x02 & control_limits))
    {
      rc |= 0x02;
      m_color_source = parents_attributes.m_color_source;
      m_color = parents_attributes.m_color;
      // 2010 March 10 Dale Lear
      //   Changing the layer index is wrong!
      //   Color by parent means COLOR and not LAYER
      // WRONG! // m_layer_index = parents_attributes.m_layer_index;
      if (MYON::color_from_layer == m_color_source && parent_layer.Index() >= 0)
      {
        // this object will use the parent layer's color
        m_color_source = MYON::color_from_object;
        m_color = parent_layer.PerViewportColor(viewport_id);
      }
    }
  }

  if (MYON::material_from_parent == m_material_source)
  {
    if (0 != (0x04 & control_limits))
    {
      rc |= 0x04;
      m_material_source = parents_attributes.m_material_source;
      m_material_index = parents_attributes.m_material_index;
      // 2010 March 10 Dale Lear
      //   Changing the layer index is wrong!
      //   Material by parent means MATERIAL and not LAYER
      // WRONG! // m_layer_index = parents_attributes.m_layer_index;
      if (MYON::material_from_layer == m_material_source && parent_layer.Index() >= 0)
      {
        // this object will use the parent layer's material
        m_material_source = MYON::material_from_object;
        m_material_index = parent_layer.m_material_index;
      }
    }
  }

  if (MYON::plot_color_from_parent == m_plot_color_source)
  {
    if (0 != (0x08 & control_limits))
    {
      rc |= 0x08;
      m_plot_color_source = parents_attributes.m_plot_color_source;
      m_plot_color = parents_attributes.m_plot_color;
      if (MYON::plot_color_from_layer == m_plot_color_source && parent_layer.Index() >= 0)
      {
        // this object will use the parent layer's plot color
        m_plot_color_source = MYON::plot_color_from_object;
        m_plot_color = parent_layer.PerViewportPlotColor(viewport_id);
      }
    }
  }

  if (MYON::plot_weight_from_parent == m_plot_weight_source)
  {
    if (0 != (0x10 & control_limits))
    {
      rc |= 0x10;
      m_plot_weight_source = parents_attributes.m_plot_weight_source;
      m_plot_weight_mm = parents_attributes.m_plot_weight_mm;
      if (MYON::plot_weight_from_layer == m_plot_weight_source && parent_layer.Index() >= 0)
      {
        // this object will use the parent layer's plot weight
        m_plot_weight_source = MYON::plot_weight_from_object;
        m_plot_weight_mm = parent_layer.PerViewportPlotWeight(viewport_id);
      }
    }
  }

  if (MYON::linetype_from_parent == m_linetype_source)
  {
    if (0 != (0x20 & control_limits))
    {
      rc |= 0x20;
      m_linetype_source = parents_attributes.m_linetype_source;
      m_linetype_index = parents_attributes.m_linetype_index;
      if (MYON::linetype_from_layer == m_linetype_source && parent_layer.Index() >= 0)
      {
        // this object will use the parent layer's line type
        m_linetype_source = MYON::linetype_from_object;
        m_linetype_index = parent_layer.m_linetype_index;
      }
    }
  }

  if (0 != (0x40 & control_limits))
  {
    rc |= 0x40;
    m_display_order = parents_attributes.m_display_order;
  }

  if (MYON::ClipParticipationSource::FromParent == ClipParticipationSource())
  {
    if (0 != (0x80 & control_limits))
    {
      rc |= 0x80;
      SetClipParticipationSource(parents_attributes.ClipParticipationSource());
      if (MYON::ClipParticipationSource::FromLayer == ClipParticipationSource() && parent_layer.Index() >= 0)
      {
        SetClipParticipationSource(MYON::ClipParticipationSource::FromObject);
        bool forAll = false;
        bool forNone = false;
        MYON_UuidList list;
        parent_layer.GetClipParticipation(forAll, forNone, list);
        if (forAll)
          SetClipParticipationForAll();
        else if (forNone)
          SetClipParticipationForNone();
        else if (list.Count() > 0)
          SetClipParticipationList(list.Array(), list.Count());
      }
      if (MYON::ClipParticipationSource::FromObject == ClipParticipationSource())
      {
        bool forAll = false;
        bool forNone = false;
        MYON_UuidList list;
        parents_attributes.GetClipParticipation(forAll, forNone, list);
        if (forAll)
          SetClipParticipationForAll();
        else if (forNone)
          SetClipParticipationForNone();
        else if (list.Count() > 0)
          SetClipParticipationList(list.Array(), list.Count());
      }
    }
  }

  if (MYON::SectionAttributesSource::FromParent == SectionAttributesSource())
  {
    if (0 != (0x100 & control_limits))
    {
      rc |= 0x100;
      SetSectionAttributesSource(parents_attributes.SectionAttributesSource());
      if (MYON::SectionAttributesSource::FromLayer == SectionAttributesSource() && parent_layer.Index() >= 0)
      {
        SetSectionAttributesSource(MYON::SectionAttributesSource::FromLayer);
        SetSectionFillRule(parent_layer.SectionFillRule());
        SetSectionHatchIndex(parent_layer.SectionHatchIndex());
        SetSectionHatchRotation(parent_layer.SectionHatchRotation());
        SetSectionHatchScale(parent_layer.SectionHatchScale());
      }
      else
      {
        SetSectionFillRule(parents_attributes.SectionFillRule());
        SetSectionHatchIndex(parents_attributes.SectionHatchIndex());
        SetSectionHatchRotation(parents_attributes.SectionHatchRotation());
        SetSectionHatchScale(parents_attributes.SectionHatchScale());
      }
    }
  }

  return rc;
}


MYON::object_color_source MYON_3dmObjectAttributes::ColorSource() const
{
  return MYON::ObjectColorSource(m_color_source);
}

void MYON_3dmObjectAttributes::SetColorSource( MYON::object_color_source c )
{
  m_color_source = (unsigned char)MYON::ObjectColorSource(c);
}

MYON::object_linetype_source MYON_3dmObjectAttributes::LinetypeSource() const
{
  return MYON::ObjectLinetypeSource(m_linetype_source);
}

void MYON_3dmObjectAttributes::SetLinetypeSource( MYON::object_linetype_source c )
{
  m_linetype_source = (unsigned char)MYON::ObjectLinetypeSource(c);
}

MYON::object_material_source MYON_3dmObjectAttributes::MaterialSource() const
{
  return MYON::ObjectMaterialSource(m_material_source);
}

void MYON_3dmObjectAttributes::SetMaterialSource( MYON::object_material_source c )
{
  m_material_source = (unsigned char)MYON::ObjectMaterialSource(c);
}


MYON::plot_color_source MYON_3dmObjectAttributes::PlotColorSource() const
{
  return MYON::PlotColorSource(m_plot_color_source);
}

void MYON_3dmObjectAttributes::SetPlotColorSource( MYON::plot_color_source pcs )
{
  m_plot_color_source = (unsigned char)MYON::PlotColorSource(pcs);
}

MYON::plot_weight_source MYON_3dmObjectAttributes::PlotWeightSource() const
{
  return MYON::PlotWeightSource(m_plot_weight_source);
}

void MYON_3dmObjectAttributes::SetPlotWeightSource( MYON::plot_weight_source pws )
{
  m_plot_weight_source = (unsigned char)MYON::PlotColorSource(pws);
}


int MYON_3dmObjectAttributes::GroupCount() const
{
  return m_group.Count();
}

//////////
// returns an int array of GroupCount() zero based group indices
const int* MYON_3dmObjectAttributes::GroupList() const
{
  return (m_group.Count()>0) ? m_group.Array() : 0;
}

int MYON_3dmObjectAttributes::GetGroupList(MYON_SimpleArray<int>& group_list) const
{
  group_list = m_group;
  return group_list.Count();
}

//////////
// returns true if object is in group with specified index
bool MYON_3dmObjectAttributes::IsInGroup(
  int group_index // zero based group index
  ) const
{
  bool rc = false;
  const int count = m_group.Count();
  int i;
  for ( i = 0; i < count; i++ ) {
    if (m_group[i] == group_index) {
      rc = true;
      break;
    }
  }
  return rc;
}

bool MYON_3dmObjectAttributes::IsInGroups( int group_count, const int* group_list ) const
{
  // returns true if object is in any of the groups in the list
  bool rc = false;
  if ( group_count > 0 && group_list ) {
    const int obj_group_count  = GroupCount();
    const int* obj_group_list = GroupList();
    // in practice these arrays will be very short and this search will be fast
    int i, j;
    for ( i = 0; i < obj_group_count; i++ ) for ( j = 0; j < group_count; j++ ) {
      if ( obj_group_list[i] == group_list[j] )
        return true;
    }
  }
  return rc;
}

bool MYON_3dmObjectAttributes::IsInGroups( const MYON_SimpleArray<int>& group_list ) const
{
  return IsInGroups( group_list.Count(), group_list.Array() );
}


//////////
// Adds object to group with specified index (If object is already in
// group, nothing is changed.)
void MYON_3dmObjectAttributes::AddToGroup(
  int group_index // zero based group index
  )
{
  if ( group_index >= 0 ) {
    if ( !IsInGroup(group_index) )
      m_group.Append(group_index);
  }
}

//////////
// returns the index of the last group in the group list
// or -1 if the object is not in any groups
int MYON_3dmObjectAttributes::TopGroup() const
{
  const int* top_group = m_group.Last();
  return top_group ? *top_group : -1;
}

//////////
// removes the object from the last group in the group list
void MYON_3dmObjectAttributes::RemoveFromTopGroup()
{
  int c = m_group.Count();
  if ( c > 0 ) {
    c--;
    m_group.SetCount(c);
  }
}

//////////
// Removes object from group with specified index.  If object is not
// in the group, nothing is changed.
void MYON_3dmObjectAttributes::RemoveFromGroup(
  int group_index // zero based group index
  )
{
  int i;
  const int count = m_group.Count();
  for ( i = 0; i < count; i++ ) {
    if (m_group[i] == group_index) {
      m_group.Remove(i);
      break;
    }
  }
}

//////////
// Removes object from all groups.
void MYON_3dmObjectAttributes::RemoveFromAllGroups()
{
  m_group.Destroy();
}


bool MYON_3dmObjectAttributes::FindDisplayMaterialId( 
      const MYON_UUID& viewport_id, 
      MYON_UUID* display_material_id
      ) const
{
  bool rc = false;
  if ( m_dmref.Count() > 0 )
  {
    MYON_DisplayMaterialRef search_material, found_material;
    search_material.m_viewport_id = viewport_id;
    if ( 0 != (rc = FindDisplayMaterialRef(search_material,&found_material)) )
    {
      if ( display_material_id )
        *display_material_id = found_material.m_display_material_id;
    }
  }
  return rc;
}


bool MYON_3dmObjectAttributes::FindDisplayMaterialRef(
  const MYON_DisplayMaterialRef& search_material,
  MYON_DisplayMaterialRef* found_material
  ) const
{
  int i = m_dmref.Count();
  if ( i > 0 )
  {
    int j = -1;
    if ( search_material.m_viewport_id != MYON_nil_uuid )
    {
      if ( search_material.m_display_material_id != MYON_nil_uuid )
      {
        while(i--)
        {
          if ( (m_dmref[i].m_display_material_id == search_material.m_display_material_id) &&
               (m_dmref[i].m_viewport_id == search_material.m_viewport_id) )
          {
            if(found_material)
              *found_material = m_dmref[i];
            return true;
          }
        }
      }
      else
      {
        while(i--)
        {
          const MYON_UUID& vid = m_dmref[i].m_viewport_id;
          if ( vid == search_material.m_viewport_id )
          {
            if(found_material)
              *found_material = m_dmref[i];
            return true;
          }
          if ( vid == MYON_nil_uuid )
          {
            j = i;
          }
        }
        if ( j >= 0 )
        {
          if(found_material)
            *found_material = m_dmref[j];
          return true;
        }
      }
    }
    else
    {
      if ( search_material.m_display_material_id != MYON_nil_uuid )
      {
        while(i--)
        {
          if ( m_dmref[i].m_display_material_id == search_material.m_display_material_id )
          {
            if ( m_dmref[i].m_viewport_id == MYON_nil_uuid )
            {
              if(found_material)
                *found_material = m_dmref[i];
              return true;
            }
            if ( j < 0 )
              j = i;
          }
        }
        if ( j >= 0 )
        {
          if(found_material)
            *found_material = m_dmref[j];
          return true;
        }
      }
      else
      {
        while(i--)
        {
          if ( m_dmref[i].m_viewport_id == MYON_nil_uuid )
          {
            if(found_material)
              *found_material = m_dmref[i];
            return true;
          }
        }
      }
    }
  }
  return false;
}


bool MYON_3dmObjectAttributes::AddDisplayMaterialRef(
  MYON_DisplayMaterialRef display_material
  )
{
  bool rc = false;
  if ( !(display_material.m_display_material_id == MYON_nil_uuid) )
  {
    int i = m_dmref.Count();
    while(i--)
    {
      if ( m_dmref[i].m_viewport_id == display_material.m_viewport_id )
      {
         m_dmref[i] = display_material;
         return true;
      }
    }
    m_dmref.Append(display_material);
  }
  return rc;
}

void MYON_3dmObjectAttributes::RemoveAllDisplayMaterialRefs()
{
  m_dmref.Destroy();
}

bool MYON_3dmObjectAttributes::RemoveDisplayMaterialRef(
  MYON_UUID viewport_id,
  MYON_UUID display_material_id
  )
{
  bool rc = false;
  int i = m_dmref.Count();
  if ( i > 0 )
  {
    const bool bCheckViewportId = !MYON_UuidIsNil(viewport_id);
    const bool bCheckMaterialId = !MYON_UuidIsNil(display_material_id);
    if ( bCheckViewportId || bCheckMaterialId )
    {
      while(i--)
      {
        if ( bCheckViewportId && m_dmref[i].m_viewport_id != viewport_id )
          continue;
        if ( bCheckMaterialId && m_dmref[i].m_display_material_id != display_material_id )
          continue;

        // remove this item
        rc = true;
        m_dmref.Remove(i);
      }
    }
    else
    {
      // 20 Sep 2006 Dale Lear - this was added so we can
      // remove all entries with non-nil viewport and nil
      // uuid.
      while(i--)
      {
        if (   !MYON_UuidIsNil(m_dmref[i].m_viewport_id)
             && MYON_UuidIsNil(m_dmref[i].m_display_material_id)
           )
        {
          // remove this item
          rc = true;
          m_dmref.Remove(i);
        }
      }
    }
  }
  return rc;
}

int MYON_3dmObjectAttributes::DisplayMaterialRefCount() const
{
  return m_dmref.Count();
}


MYON::ClipParticipationSource MYON_3dmObjectAttributes::ClipParticipationSource() const
{
  return m_private ? m_private->m_clip_participation_source : DefaultAttributesPrivate.m_clip_participation_source;
}
void MYON_3dmObjectAttributes::SetClipParticipationSource(MYON::ClipParticipationSource source)
{
  if (ClipParticipationSource() == source)
    return;

  if (nullptr == m_private)
    m_private = new MYON_3dmObjectAttributesPrivate(this);
  m_private->m_clip_participation_source = source;
}

void MYON_3dmObjectAttributes::SetClipParticipationForAll()
{
  // default is true for all clipping planes. If our private pointer hasn't
  // been created, there is no need to do anything
  if (nullptr == m_private)
    return;

  m_private->m_clipplane_list.Empty();
  m_private->m_clipping_proof = false;
}
void MYON_3dmObjectAttributes::SetClipParticipationForNone()
{
  if (nullptr == m_private)
    m_private = new MYON_3dmObjectAttributesPrivate(this);
  m_private->m_clipplane_list.Empty();
  m_private->m_clipping_proof = true;
}
void MYON_3dmObjectAttributes::SetClipParticipationList(const MYON_UUID* clippingPlaneIds, int count)
{
  if (nullptr == clippingPlaneIds || count < 1)
    SetClipParticipationForAll();

  if (nullptr == m_private)
    m_private = new MYON_3dmObjectAttributesPrivate(this);
  m_private->m_clipplane_list.Empty();
  for (int i = 0; i < count; i++)
    m_private->m_clipplane_list.AddUuid(clippingPlaneIds[i], true);

  m_private->m_clipping_proof = false;
}
void MYON_3dmObjectAttributes::GetClipParticipation(
  bool& forAllClippingPlanes,
  bool& forNoClippingPlanes,
  MYON_UuidList& specificClipplaneList) const
{
  if (nullptr == m_private)
  {
    forAllClippingPlanes = true;
    forNoClippingPlanes = false;
    specificClipplaneList.Empty();
    return;
  }

  specificClipplaneList = m_private->m_clipplane_list;
  if (specificClipplaneList.Count() > 0)
  {
    forAllClippingPlanes = false;
    forNoClippingPlanes = false;
  }
  else
  {
    forNoClippingPlanes = m_private->m_clipping_proof;
    forAllClippingPlanes = !forNoClippingPlanes;
  }
}

MYON::SectionFillRule MYON_3dmObjectAttributes::SectionFillRule() const
{
  return m_private ? m_private->m_section_fill_rule : DefaultAttributesPrivate.m_section_fill_rule;
}
void MYON_3dmObjectAttributes::SetSectionFillRule(MYON::SectionFillRule rule)
{
  if (SectionFillRule() == rule)
    return;

  if (nullptr == m_private)
    m_private = new MYON_3dmObjectAttributesPrivate(this);
  m_private->m_section_fill_rule = rule;
}

int MYON_3dmObjectAttributes::SectionHatchIndex() const
{
  return m_private ? m_private->m_section_hatch_index : DefaultAttributesPrivate.m_section_hatch_index;
}

void MYON_3dmObjectAttributes::SetSectionHatchIndex(int index)
{
  if (SectionHatchIndex() == index)
    return;

  if (nullptr == m_private)
    m_private = new MYON_3dmObjectAttributesPrivate(this);
  m_private->m_section_hatch_index = index;
}

double MYON_3dmObjectAttributes::SectionHatchScale() const
{
  return m_private ? m_private->m_section_hatch_scale : DefaultAttributesPrivate.m_section_hatch_scale;
}
void MYON_3dmObjectAttributes::SetSectionHatchScale(double scale)
{
  if (SectionHatchScale() == scale)
    return;

  if (nullptr == m_private)
    m_private = new MYON_3dmObjectAttributesPrivate(this);
  m_private->m_section_hatch_scale = scale;
}

double MYON_3dmObjectAttributes::SectionHatchRotation() const
{
  return m_private ? m_private->m_section_hatch_rotation : DefaultAttributesPrivate.m_section_hatch_rotation;
}
void MYON_3dmObjectAttributes::SetSectionHatchRotation(double rotation)
{
  if (SectionHatchRotation() == rotation)
    return;

  if (nullptr == m_private)
    m_private = new MYON_3dmObjectAttributesPrivate(this);
  m_private->m_section_hatch_rotation = rotation;
}

MYON::SectionAttributesSource MYON_3dmObjectAttributes::SectionAttributesSource() const
{
  return m_private ? m_private->m_section_attributes_source : DefaultAttributesPrivate.m_section_attributes_source;
}
void MYON_3dmObjectAttributes::SetSectionAttributesSource(MYON::SectionAttributesSource source)
{
  if (SectionAttributesSource() == source)
    return;

  if (nullptr == m_private)
    m_private = new MYON_3dmObjectAttributesPrivate(this);
  m_private->m_section_attributes_source = source;
}

double MYON_3dmObjectAttributes::LinetypePatternScale() const
{
  return m_private ? m_private->m_linetype_scale : 1.0;
}
void MYON_3dmObjectAttributes::SetLinetypePatternScale(double scale)
{
  if (scale < MYON_EPSILON)
    return;

  if (fabs(LinetypePatternScale() - scale) < MYON_EPSILON)
    return;

  if (nullptr == m_private)
    m_private = new MYON_3dmObjectAttributesPrivate(this);
  m_private->m_linetype_scale = scale;
}

void MYON_3dmObjectAttributes::SetCustomLinetype(const MYON_Linetype& linetype)
{
  if (nullptr == m_private)
    m_private = new MYON_3dmObjectAttributesPrivate(this);

  m_private->m_custom_linetype.reset(new MYON_Linetype(linetype));
}
const MYON_Linetype* MYON_3dmObjectAttributes::CustomLinetype() const
{
  const MYON_Linetype* rc = nullptr;
  if (m_private)
    rc = m_private->m_custom_linetype.get();
  return rc;
}
void MYON_3dmObjectAttributes::RemoveCustomLinetype()
{
  if (m_private)
    m_private->m_custom_linetype.reset();
}


MYON_Color MYON_3dmObjectAttributes::HatchBackgroundFillColor() const
{
  return m_private ? m_private->m_hatch_background_fill : MYON_Color::UnsetColor;
}
void MYON_3dmObjectAttributes::SetHatchBackgrounFillColor(const MYON_Color& color)
{
  MYON_Color c = color;
  if (c.Alpha() == 255)
    c = MYON_Color::UnsetColor;
  if (HatchBackgroundFillColor() == c)
    return;

  if (nullptr == m_private)
    m_private = new MYON_3dmObjectAttributesPrivate(this);
  m_private->m_hatch_background_fill = c;
}
bool MYON_3dmObjectAttributes::HatchBoundaryVisible() const
{
  return m_private ? m_private->m_hatch_boundary_visible : DefaultAttributesPrivate.m_hatch_boundary_visible;
}
void MYON_3dmObjectAttributes::SetHatchBoundaryVisible(bool on)
{
  if (HatchBoundaryVisible() == on)
    return;

  if (nullptr == m_private)
    m_private = new MYON_3dmObjectAttributesPrivate(this);
  m_private->m_hatch_boundary_visible = on;
}


//https://mcneel.myjetbrains.com/youtrack/issue/RH-20531
MYON_Plane MYON_3dmObjectAttributes::ObjectFrame(const MYON_COMPONENT_INDEX& ci) const
{
  return m_object_frame;
}

//https://mcneel.myjetbrains.com/youtrack/issue/RH-20531
void MYON_3dmObjectAttributes::SetObjectFrame(const MYON_COMPONENT_INDEX&, const MYON_Xform& wcs_to_ocs)
{
  m_object_frame = MYON_Plane::World_xy;
  m_object_frame.Transform(wcs_to_ocs);
}

void MYON_3dmObjectAttributes::SetObjectFrame(const MYON_COMPONENT_INDEX& ci, const MYON_Plane& plane)
{
  m_object_frame = plane;
}

MYON_MeshModifiers& MYON_3dmObjectAttributes::MeshModifiers(void) const
{
  if (nullptr == m_private)
    m_private = new MYON_3dmObjectAttributesPrivate(this);

  return m_private->m_mesh_modifiers;
}

const MYON_SimpleArray<MYON_Decal*>& MYON_3dmObjectAttributes::GetDecalArray(void) const
{
  if (nullptr == m_private)
    m_private = new MYON_3dmObjectAttributesPrivate(this);

  return m_private->m_decals.GetDecalArray();
}

MYON_Decal* MYON_3dmObjectAttributes::AddDecal(void)
{
  if (nullptr == m_private)
    m_private = new MYON_3dmObjectAttributesPrivate(this);

  return m_private->m_decals.AddDecal();
}

bool MYON_3dmObjectAttributes::DeleteDecal(MYON_Decal& decal)
{
  if (nullptr == m_private)
    m_private = new MYON_3dmObjectAttributesPrivate(this);

  return m_private->m_decals.DeleteDecal(decal);
}

void MYON_3dmObjectAttributes::DeleteAllDecals(void)
{
  //if (nullptr == m_private)
  //  m_private = new MYON_3dmObjectAttributesPrivate(this);

  // Delete all the user data? [JOHN-DECAL-FIX]
  // TODO:
//  return m_private->m_decals.DeleteAllDecals();
}

// {1403A7E4-E7AD-4a01-A2AA-41DAE6BE7ECB}
const MYON_UUID MYON_DisplayMaterialRef::m_invisible_in_detail_id = 
{ 0x1403a7e4, 0xe7ad, 0x4a01, { 0xa2, 0xaa, 0x41, 0xda, 0xe6, 0xbe, 0x7e, 0xcb } };

MYON_DisplayMaterialRef::MYON_DisplayMaterialRef()
{
  m_viewport_id = MYON_nil_uuid;
  m_display_material_id = MYON_nil_uuid;
}

bool MYON_DisplayMaterialRef::operator==(const MYON_DisplayMaterialRef& other) const
{
  return (Compare(other)==0); 
}

bool MYON_DisplayMaterialRef::operator!=(const MYON_DisplayMaterialRef& other) const
{
  return (Compare(other)!=0); 
}

int MYON_DisplayMaterialRef::Compare(const MYON_DisplayMaterialRef& other) const
{
  int i = MYON_UuidCompare(m_viewport_id,other.m_viewport_id);
  if (0==i)
    i = MYON_UuidCompare(m_display_material_id,other.m_display_material_id);
  return i;
}

bool MYON_DisplayMaterialRef::operator<(const MYON_DisplayMaterialRef& other) const
{
  return (Compare(other)<0); 
}

bool MYON_DisplayMaterialRef::operator<=(const MYON_DisplayMaterialRef& other) const
{
  return (Compare(other)<=0); 
}

bool MYON_DisplayMaterialRef::operator>(const MYON_DisplayMaterialRef& other) const
{
  return (Compare(other)>0); 
}

bool MYON_DisplayMaterialRef::operator>=(const MYON_DisplayMaterialRef& other) const
{
  return (Compare(other)>=0); 
}

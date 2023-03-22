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

class MYON_LayerPrivate
{
public:
  MYON_LayerPrivate() = default;
  ~MYON_LayerPrivate() = default;

  bool operator==(const MYON_LayerPrivate&) const;
  bool operator!=(const MYON_LayerPrivate&) const;

  MYON_UuidList m_clipplane_list;
  bool m_clipping_proof = false;

  MYON::SectionFillRule m_section_fill_rule = MYON::SectionFillRule::ClosedCurves;
  int m_section_hatch_index = MYON_UNSET_INT_INDEX; // MYON_HatchPattern::Unset.Index();
  double m_section_hatch_scale = 1.0;
  double m_section_hatch_rotation = 0.0;

  std::shared_ptr<MYON_Linetype> m_custom_linetype;
};

static const MYON_LayerPrivate DefaultLayerPrivate;


bool MYON_LayerPrivate::operator==(const MYON_LayerPrivate& other) const
{
  if (this == &other)
    return true;

  if (m_clipplane_list != other.m_clipplane_list)
    return false;

  if (m_clipping_proof != other.m_clipping_proof)
    return false;

  if (m_section_fill_rule != other.m_section_fill_rule)
    return false;

  if (m_section_hatch_index != other.m_section_hatch_index)
    return false;

  if (m_section_hatch_scale != other.m_section_hatch_scale)
    return false;

  if (m_section_hatch_rotation != other.m_section_hatch_rotation)
    return false;

  const MYON_Linetype* customThis = m_custom_linetype.get();
  const MYON_Linetype* customOther = other.m_custom_linetype.get();
  if (customThis != customOther)
    return false;

  return true;
}

bool MYON_LayerPrivate::operator!=(const MYON_LayerPrivate& other) const
{
  return !MYON_LayerPrivate::operator==(other);
}


MYON_OBJECT_IMPLEMENT(MYON_Layer,MYON_ModelComponent,"95809813-E985-11d3-BFE5-0010830122F0");

const MYON_Layer* MYON_Layer::FromModelComponentRef(
  const class MYON_ModelComponentReference& model_component_reference,
  const MYON_Layer* none_return_value
  )
{
  const MYON_Layer* p = MYON_Layer::Cast(model_component_reference.ModelComponent());
  return (nullptr != p) ? p : none_return_value;
}

bool MYON_Layer::UpdateReferencedComponents(
  const class MYON_ComponentManifest& source_manifest,
  const class MYON_ComponentManifest& destination_manifest,
  const class MYON_ManifestMap& manifest_map
  )
{
  bool rc = true;

  // Update render material reference
  int material_index = RenderMaterialIndex();
  if (material_index >= 0)
  {
    int destination_material_index = MYON_UNSET_INT_INDEX;
    if (manifest_map.GetAndValidateDestinationIndex(
      MYON_ModelComponent::Type::RenderMaterial,
      material_index,
      destination_manifest,
      &destination_material_index))
    {
      material_index = destination_material_index;
    }
    else
    {
      MYON_ERROR("Unable to update render material reference.");
      rc = false;
      material_index = MYON_Layer::Default.RenderMaterialIndex();
    }
    SetRenderMaterialIndex(material_index);
  }

  // Update line pattern reference
  int line_pattern_index = LinetypeIndex();
  if (line_pattern_index >= 0)
  {
    int destination_line_pattern_index = MYON_UNSET_INT_INDEX;
    if (manifest_map.GetAndValidateDestinationIndex(
      MYON_ModelComponent::Type::LinePattern,
      line_pattern_index,
      destination_manifest,
      &destination_line_pattern_index))
    {
      line_pattern_index = destination_line_pattern_index;
    }
    else
    {
      MYON_ERROR("Unable to update line pattern reference.");
      rc = false;
      line_pattern_index = MYON_Layer::Default.LinetypeIndex();
    }
    SetLinetypeIndex(line_pattern_index);
  }

  // Update parent layer reference
  MYON_UUID parent_layer_id = ParentLayerId();
  if (MYON_nil_uuid != parent_layer_id)
  {
    const MYON_UUID manifest_item_id = destination_manifest.ItemFromId(
      MYON_ModelComponent::Type::Layer,
      parent_layer_id
      ).Id();
    if ( MYON_nil_uuid == manifest_item_id )
    {
      MYON_ERROR("Unable to update parent layer id reference.");
      rc = false;
      parent_layer_id = MYON_Layer::Default.ParentLayerId();
    }
    else
    {
      parent_layer_id = manifest_item_id;
    }
    SetParentLayerId(parent_layer_id);
  }

  return rc;
}

#define MYON_BOZO_VACCINE_3E4904E6E9304fbcAA42EBD407AEFE3B
#define MYON_BOZO_VACCINE_BFB63C094BC7472789BB7CC754118200

MYON_Layer::MYON_Layer() MYON_NOEXCEPT
  : MYON_ModelComponent(MYON_ModelComponent::Type::Layer)
{}

MYON_Layer::~MYON_Layer()
{
  if (m_private)
    delete m_private;
}

MYON_Layer::MYON_Layer( const MYON_Layer& src)
  : MYON_ModelComponent(MYON_ModelComponent::Type::Layer,src)
  , m_iges_level(src.m_iges_level)
  , m_material_index(src.m_material_index)
  , m_rendering_attributes(src.m_rendering_attributes)
  , m_linetype_index(src.m_linetype_index)
  , m_color(src.m_color)
  , m_display_material_id(src.m_display_material_id)
  , m_plot_color(src.m_plot_color)
  , m_plot_weight_mm(src.m_plot_weight_mm)
  , m_bExpanded(src.m_bExpanded)
  , m_extension_bits(src.m_extension_bits)
{
  if (src.m_private)
  {
    m_private = new MYON_LayerPrivate();
    *m_private = *src.m_private;
  }
}

MYON_Layer& MYON_Layer::operator=(const MYON_Layer& src)
{
  if (this != &src)
  {
    MYON_ModelComponent::operator=(src);
    m_iges_level = src.m_iges_level;
    m_material_index = src.m_material_index;
    m_rendering_attributes = src.m_rendering_attributes;
    m_linetype_index = src.m_linetype_index;
    m_color = src.m_color;
    m_display_material_id = src.m_display_material_id;
    m_plot_color = src.m_plot_color;
    m_plot_weight_mm = src.m_plot_weight_mm;
    m_bExpanded = src.m_bExpanded;
    m_extension_bits = src.m_extension_bits;

    if (m_private)
      delete m_private;
    m_private = nullptr;
    if (src.m_private)
    {
      m_private = new MYON_LayerPrivate();
      *m_private = *src.m_private;
    }
  }
  return *this;
}


static void SetExtensionBit( unsigned char* layer_m_extension_bits, unsigned char mask )
{
  *layer_m_extension_bits |= mask;
}

static void ClearExtensionBit(  unsigned char* layer_m_extension_bits, unsigned char mask )
{
  unsigned char notmask = ~mask;
  *layer_m_extension_bits &= notmask;
}

static bool ExtensionBit( unsigned char layer_m_extension_bits, unsigned char mask )
{
  return (0 != (layer_m_extension_bits & mask));
}

bool MYON_Layer::IsValid( MYON_TextLog* text_log ) const
{
  if ( NameIsEmpty() )
  {
    if ( text_log )
    {
      text_log->Print("Layer name is empty.\n");
    }
    return false;
  }
  return true;
}

// 12 Aug 2021 S. Baer
// When adding new fields written to 3dm files, always add information to this
// Dump function. Dump is used by the opennurbs file testing framework to
// perform comparisons and is useful for manual comparison in when tests fail.
void MYON_Layer::Dump( MYON_TextLog& dump ) const
{
  MYON_ModelComponent::Dump(dump);
  dump.Print("display = %s\n",IsVisible()?"visible":"hidden");
  dump.Print("picking = %s\n",IsLocked()?"locked":"unlocked");
  dump.Print("display color rgb = "); dump.PrintRGB(m_color); dump.Print("\n");
  dump.Print("plot color rgb = "); dump.PrintRGB(m_plot_color); dump.Print("\n");
  dump.Print("default material index = %d\n",m_material_index);

  {
    bool clipAll = true;
    bool clipNone = false;
    MYON_UuidList cliplist;
    GetClipParticipation(clipAll, clipNone, cliplist);
    if (clipAll)
    {
      dump.Print("participates with all clipping planes\n");
    }
    else if (clipNone)
    {
      dump.Print("participates with no clipping planes\n");
    }
    else
    {
      dump.Print("participates with specific clipping planes\n");
    }
  }

  int index = SectionHatchIndex();
  if (MYON_UNSET_INT_INDEX == index)
  {
    dump.Print("no section hatch\n");
  }
  else
  {
    dump.Print("section hatch index = %d\n", index);
  }
}

// 28 Sept. 2021
// This enum is patterned off of MYON_3dmObjectAttributeTypeCode
// Layers will grow with properties at the same rate as MYON_3dmObjectAttributes
// Attribute I/O changed years ago to only write non-default values into
// 3dm files so we don't end up stuffing repeated data into a 3dm that can be
// inferred when the data matches default values.
enum MYON_LayerTypeCodes : unsigned char
{
  SelectiveClippingData = 28,
  // 18 Oct 2021 S. Baer
  // chunk version 1.11: add section hatch attributes
  SectionHatchIndex = 29,
  SectionHatchScale = 30,
  SectionHatchRotation = 31,
  // 14 June 2022 S. Baer
  // chunk version 1.12: add section fill rule
  SectionFillRule = 32,
  // 30 Nov 2022 S. Baer
  // chunk version 1.13: add custom linetype
  CustomLinetype = 33,

  LastLayerTypeCode = 33
};

bool MYON_Layer::Write(
       MYON_BinaryArchive& file // serialize definition to binary archive
     ) const
{
  int i;

  bool rc = file.Write3dmChunkVersion(1,13);
  while(rc)
  {
    // Save the visibility state this layer has when its parent
    // is visible ignoring current parent visibility value.
    bool bVisible = PersistentVisibility();

    // Save the locked state this layer has when its parent
    // is unlocked ignoring current parenting locked setting.
    const bool bLocked = PersistentLocking();

    // Save OBSOLETE mode value so we don't break file format
    if ( bVisible )
      i = 0; // "normal" layer mode
    else if ( bLocked )
      i = 2; // "locked" layer mode
    else
      i = 1; // "hidden" layer mode

    rc = file.WriteInt( i );
    if (!rc) break;

    rc = file.Write3dmReferencedComponentIndex( *this );
    if (!rc) break;

    rc = file.WriteInt( m_iges_level );
    if (!rc) break;

    rc = file.Write3dmReferencedComponentIndex( MYON_ModelComponent::Type::RenderMaterial, m_material_index );
    if (!rc) break;

    // Starting with version 200312110, this value is zero.  For files written
    // with earlier versions, the number was a "model index" value that was
    // set to something >= 1, but never used.  We have to continue to 
    // read/write an integer here so that old/new versions of opennurbs can
    // read files written by new/old versions.
    i = 0;
    rc = file.WriteInt( i );
    if (!rc) break;

    rc = file.WriteColor( m_color );
    if (!rc) break;
    
    {
      // OBSOLETE LINE STYLE if ( rc ) rc = file.WriteLineStyle( LineStyle() );
      // Starting with version 200503170, this section is "officially" not used.
      // Prior to that, it was old MYON_LineStyle information that has
      // never been used.
      short s = 0;
      if (rc) rc = file.WriteShort(s);    // default pattern
      if (rc) rc = file.WriteShort(s);    // default pattern index
      if (rc) rc = file.WriteDouble(0.0); // default thickness
      if (rc) rc = file.WriteDouble(1.0); // default scale
    }
    if (!rc) break;

    if (
      file.Active3dmTable() == MYON_3dmArchiveTableType::layer_table
      && file.Archive3dmVersion() <= 4
      && NameIsNotEmpty()
      && ParentIdIsNotNil()
      )
    {
      MYON_wString name = Name();
      // In V4 there are no parent layers and all V4 layer names must be unique.
      // Since layers can be written in any order, we cannot know if there will
      // eventually be a parent layer using the same name as this child layer.
      // So, child layer names get a hash appended to insure they are unique.
      MYON_UUID parent_layer_id = ParentId();
      MYON__UINT16 hash = MYON_CRC16(0, sizeof(parent_layer_id), &parent_layer_id);
      MYON_RandomNumberGenerator rng;
      rng.Seed(hash);
      for (int attempt_count = 0; attempt_count < 100; attempt_count++)
      {
        while ( 0 == hash )
          hash = (MYON__UINT16)(rng.RandomNumber() % 0xFFFFU);
        MYON_wString sublayer_name;
        sublayer_name.Format(L"%ls (%04x)", static_cast<const wchar_t*>(name),hash);
        
        // Use MYON_nil_uuid ast the parent id because we need a name that is uniques
        // as a "top level" layer name for V4 files.
        const MYON_NameHash sublayer_name_hash = MYON_NameHash::Create(MYON_nil_uuid,sublayer_name);

        if ( file.Manifest().ItemFromNameHash(ComponentType(), sublayer_name_hash).IsUnset() )
        {
          // have a unique name
          name = sublayer_name;
          const_cast< MYON_ComponentManifest& >(file.Manifest()).ChangeComponentNameHash(Id(), sublayer_name_hash);
          break;
        }
        hash = (MYON__UINT16)(rng.RandomNumber() % 0xFFFFU);
      }
      rc = file.WriteString( name );
    }
    else
    {
      rc = file.WriteModelComponentName(*this);
    }

    if (!rc) break;

    // 1.1 fields
    rc = file.WriteBool(bVisible);
    if (!rc) break;

    // 1.2 field
    rc = file.Write3dmReferencedComponentIndex( MYON_ModelComponent::Type::LinePattern, m_linetype_index );
    if (!rc) break;

    // 1.3 field - 23 March 2005 Dale Lear
    rc = file.WriteColor( m_plot_color);
    if (!rc) break;
    rc = file.WriteDouble( m_plot_weight_mm);
    if (!rc) break;

    // 1.4 field - 3 May 2005 Dale Lear 
    //           - locked and visible are independent settings
    rc = file.WriteBool( bLocked );
    if (!rc) break;

    // 1.5 field
    rc = file.WriteUuid( Id() );
    if (!rc) break;

    // 1.6 field
    MYON_UUID parent_layer_id = ParentLayerId();
    rc = file.WriteUuid( parent_layer_id );
    if (!rc) break;

    // 1.6 field
    rc = file.WriteBool( m_bExpanded );
    if (!rc) break;

    // 1.7 field - added 6 June 2006
    rc = m_rendering_attributes.Write(file);
    if (!rc) break;

    // 1.8 field - added 19 Sep 2006
    rc = file.WriteUuid(m_display_material_id);
    if (!rc) break;

    // 1.10 field - added 29 Sep 2021
    // using MYON_3dmObjectAttributes inspired technique for writing only
    // non-default values to 3dm files
    unsigned char c = 0;

    // selective clipping (1.10)
    {
      bool forAllClippingPlanes = true;
      bool forNoClippingPlanes = false;
      MYON_UuidList selectiveList;
      GetClipParticipation(forAllClippingPlanes, forNoClippingPlanes, selectiveList);
      // only write selective clipping data if it is not default
      if (false == forAllClippingPlanes || true == forNoClippingPlanes || selectiveList.Count() > 0)
      {
        c = MYON_LayerTypeCodes::SelectiveClippingData;
        rc = file.WriteChar(c);
        if (!rc) break;
        rc = file.WriteBool(forNoClippingPlanes);
        if (!rc) break;
        rc = selectiveList.Write(file);
        if (!rc) break;
      }
    }

    // section hatch (1.11)
    {
      if (SectionHatchIndex() != MYON_UNSET_INT_INDEX)
      {
        c = MYON_LayerTypeCodes::SectionHatchIndex;
        rc = file.WriteChar(c);
        if (!rc) break;
        rc = file.Write3dmReferencedComponentIndex(MYON_ModelComponent::Type::HatchPattern, SectionHatchIndex());
        if (!rc) break;
      }
      if (SectionHatchScale() != 1.0)
      {
        c = MYON_LayerTypeCodes::SectionHatchScale;
        rc = file.WriteChar(c);
        if (!rc) break;
        rc = file.WriteDouble(SectionHatchScale());
        if (!rc) break;
      }
      if (SectionHatchRotation() != 0.0)
      {
        c = MYON_LayerTypeCodes::SectionHatchRotation;
        rc = file.WriteChar(c);
        if (!rc) break;
        rc = file.WriteDouble(SectionHatchRotation());
        if (!rc) break;
      }
    }

    // section fill (1.12)
    if (SectionFillRule() != MYON::SectionFillRule::ClosedCurves)
    {
      c = MYON_LayerTypeCodes::SectionFillRule; //32
      rc = file.WriteChar(c);
      if (!rc) break;
      rc = file.WriteChar((unsigned char)SectionFillRule());
      if (!rc) break;
    }

    // custom linetype (1.13)
    // 17 Feb 2023 S. Baer
    // Custom linetypes were an experiment that ended up making the UI more complicated
    // than necessary. Do not write any custom linetype data into the 3dm file.
    //{
    //  const MYON_Linetype* linetype = CustomLinetype();
    //  if (linetype)
    //  {
    //    c = MYON_LayerTypeCodes::CustomLinetype; // 33
    //    rc = file.WriteChar(c);
    //    if (!rc) break;
    //    rc = linetype->Write(file);
    //    if (!rc) break;
    //  }
    //}

    // 0 indicates end of new non-default attributes
    c = 0;
    rc = file.WriteChar(c);

    break;
  }

  return rc;
}

bool MYON_Layer::Read(
       MYON_BinaryArchive& file // restore definition from binary archive
     )
{
  int obsolete_value1 = 0; // see MYON_Layer::Write
  int major_version=0;
  int minor_version=0;
  int mode = MYON::normal_layer;
  *this = MYON_Layer::Unset;
  bool rc = file.Read3dmChunkVersion(&major_version,&minor_version);
  if ( rc && major_version == 1 )
  {
    // common to all 1.x formats
    if ( rc ) rc = file.ReadInt( &mode );
    if ( rc ) 
    {
      switch(mode)
      {
      case 0: // OBSOLETE MYON::normal_layer
        break;
      case 1: // OBSOLETE MYON::hidden_layer
        SetHiddenModelComponentState(true);
        break;
      case 2: // OBSOLETE MYON::locked_layer
        SetLockedModelComponentState( true );
        break;
      default:
        break;
      }
    }
    int layer_index = Index();
    if (rc)
    {
      // this is the archive layer index - it will probably change when the
      // layer is added to the model. Since the layer has not been added to
      // the model, there is not way to automatically update it at this time.
      rc = file.ReadInt(&layer_index);
    }
    if (rc)
      SetIndex(layer_index);

    if ( rc ) rc = file.ReadInt( &m_iges_level );

    // render material index
    int render_material_index = MYON_UNSET_INT_INDEX;
    if (rc) 
      rc = file.Read3dmReferencedComponentIndex(MYON_ModelComponent::Type::RenderMaterial,&render_material_index);
    if (rc && MYON_UNSET_INT_INDEX != render_material_index )
      SetRenderMaterialIndex(render_material_index);

    if ( rc ) rc = file.ReadInt( &obsolete_value1 );
    if ( rc ) rc = file.ReadColor( m_color );

    // 25 Aug 2021 S. Baer (RH-65410)
    // Pre-V7 files ignored alpha on layer colors and in some cases
    // alpha was being set to completely transparent. In this case,
    // make the color opaque. Even V7 files with completely transparent
    // color is strange, but it can be intentionally set for some reason
    if (rc && m_color.Alpha() == 255 && m_color != MYON_Color::UnsetColor && file.Archive3dmVersion() < 70)
    {
      m_color.SetAlpha(0);
    }

    {
      // OBSOLETE line style was never used - read and discard the next 20 bytes
      short s;
      double x;
      if (rc) file.ReadShort(&s);
      if (rc) file.ReadShort(&s);
      if (rc) file.ReadDouble(&x);
      if (rc) file.ReadDouble(&x);
    }

    MYON_wString layer_name;
    if ( rc ) rc = file.ReadString( layer_name );
    if (rc)
      SetName(layer_name);

    if ( rc && minor_version >= 1 )
    {
      bool bVisible = true;
      rc = file.ReadBool(&bVisible);
      if ( rc && false == bVisible)
        SetHiddenModelComponentState(true);
      bVisible = (false == ModelComponentStatus().IsHidden());

      if ( rc && minor_version >= 2 )
      {
        // line pattern index
        int line_pattern_index = MYON_UNSET_INT_INDEX;
        rc = file.Read3dmReferencedComponentIndex( MYON_ModelComponent::Type::LinePattern, &line_pattern_index );
        if (rc && MYON_UNSET_INT_INDEX != line_pattern_index)
          SetLinetypeIndex(line_pattern_index);

        if (rc && minor_version >= 3 )
        {
          // 23 March 2005 Dale Lear
          rc = file.ReadColor( m_plot_color);
          if (rc) rc = file.ReadDouble( &m_plot_weight_mm);

          if (rc && minor_version >= 4 )
          {
            bool bLocked = false;
            rc = file.ReadBool(&bLocked);
            if (rc && bLocked )
              SetLockedModelComponentState(bLocked);
            bLocked = ModelComponentStatus().IsLocked();

            if (rc && minor_version >= 5 )
            {
              MYON_UUID layer_id = MYON_nil_uuid;
              rc = file.ReadUuid(layer_id);
              if (rc)
                SetId(layer_id);
              if ( rc
                   && minor_version >= 6
                   && file.ArchiveOpenNURBSVersion() > 200505110
                 )
              {
                // Some files saved with opennurbs version 200505110 
                // do not contain correctly written m_parent_layer_id
                // and m_bExpanded values.
                // It is ok to default these values.
                MYON_UUID parent_layer_id = MYON_nil_uuid;
                rc = file.ReadUuid(parent_layer_id);
                if (rc)
                {
                  SetParentId(parent_layer_id);
                  if ( MYON_UuidIsNotNil(parent_layer_id) )
                  {
                    //SetParentId(parent_layer_id);
                    if ( ModelComponentStatus().IsHidden() )
                      SetPersistentVisibility(false);
                    if ( ModelComponentStatus().IsLocked())
                      SetPersistentLocking(true);
                  }
                  rc = file.ReadBool(&m_bExpanded);
                }
              }

              if ( rc && minor_version >= 7 )
              {
                // 1.7 field - added 6 June 2006
                rc = m_rendering_attributes.Read(file);

                if ( rc && minor_version >= 8 )
                {
                  // 1.8 field - added 19 Sep 2006
                  rc = file.ReadUuid(m_display_material_id);

                  if (rc && minor_version == 9)
                  {
                    // 1.9 field - added 11 Aug 2021 and removed on 12 Aug 2021
                    // Note above that we are looking specifically at minor_version of 9
                    // read two chars and throw them away.
                    // minor_version >= 10 should skip this entirely
                    unsigned char style = 0;
                    rc = file.ReadChar(&style);
                    if (rc)
                    {
                      style = 0;
                      rc = file.ReadChar(&style);
                    }
                  }

                  if (rc && minor_version >= 10)
                  {
                    unsigned char itemid = 0xFF;
                    while (rc)
                    {
                      rc = file.ReadChar(&itemid);
                      if (!rc) break;
                      if (0 == itemid)
                        break;

                      if (MYON_LayerTypeCodes::SelectiveClippingData == itemid) //28
                      {
                        bool noClippingPlanes = false;
                        MYON_UuidList selectiveList;
                        rc = file.ReadBool(&noClippingPlanes);
                        if (!rc) break;
                        rc = selectiveList.Read(file);
                        if (!rc) break;
                        if (noClippingPlanes)
                          SetClipParticipationForNone();
                        else if (selectiveList.Count() > 0)
                          SetClipParticipationList(selectiveList.Array(), selectiveList.Count());
                        else
                          SetClipParticipationForAll();

                        rc = file.ReadChar(&itemid);
                        if (!rc || 0 == itemid) break;
                      }

                      if (minor_version <= 10)
                        break;

                      if (MYON_LayerTypeCodes::SectionHatchIndex == itemid) // 30
                      {
                        int pattern = 0;
                        rc = file.Read3dmReferencedComponentIndex(MYON_ModelComponent::Type::HatchPattern, &pattern);
                        if (!rc) break;
                        SetSectionHatchIndex(pattern);
                        rc = file.ReadChar(&itemid);
                        if (!rc || 0 == itemid) break;
                      }

                      if (MYON_LayerTypeCodes::SectionHatchScale == itemid) // 31
                      {
                        double scale = 1;
                        rc = file.ReadDouble(&scale);
                        if (!rc) break;
                        SetSectionHatchScale(scale);
                        rc = file.ReadChar(&itemid);
                        if (!rc || 0 == itemid) break;
                      }

                      if (MYON_LayerTypeCodes::SectionHatchRotation == itemid) // 32
                      {
                        double rotation = 0;
                        rc = file.ReadDouble(&rotation);
                        if (!rc) break;
                        SetSectionHatchRotation(rotation);
                        rc = file.ReadChar(&itemid);
                        if (!rc || 0 == itemid) break;
                      }

                      if (minor_version <= 11)
                        break;

                      if (MYON_LayerTypeCodes::SectionFillRule == itemid)
                      {
                        unsigned char c = 0;
                        rc = file.ReadChar(&c);
                        if (!rc) break;
                        SetSectionFillRule(MYON::SectionFillRuleFromUnsigned(c));
                        rc = file.ReadChar(&itemid);
                        if (!rc || 0 == itemid) break;
                      }

                      if (minor_version <= 12)
                        break;

                      if (MYON_LayerTypeCodes::CustomLinetype == itemid)
                      {
                        MYON_Linetype lt;
                        rc = lt.Read(file);
                        if (!rc) break;
                        // 17 Feb 2023 S. Baer
                        // Custom linetype per layer was a short lived experiment
                        // We still need to read a linetype out of the few 3dm files
                        // that have them, but we no longer set any sort of custom linetype
                        //SetCustomLinetype(lt);
                        rc = file.ReadChar(&itemid);
                        if (!rc || 0 == itemid) break;
                      }

                      // break if minor_version<=13. If itemid is non-zero and
                      // minor_version is not > 13, then we know we have an I/O
                      // reading bug that needs to be tracked down
                      if (minor_version <= 13)
                        break;

                      // Add new item reading above and increment the LastLayerTypeCode value
                      // in the enum. Be sure to test reading of old and new files by old and new
                      // code, before checking in your changes.
                      if (itemid > MYON_LayerTypeCodes::LastLayerTypeCode)
                      {
                        // we are reading file written with code newer than this code
                        itemid = 0;
                      }

                      break;
                    }

                    if (rc && 0 != itemid)
                    {
                      MYON_ERROR("Bug in MYON_Layer::Read or Write");
                    }

                  }
                }
              }
            }
          }
        }
      }
    }

    if ( IdIsNil()  )
    {
      // old files didn't have layer ids and we need unique ones.
      SetId();
    }
  }
  else
  {
    MYON_ERROR("MYON_Layer::Read() encountered a layer written by future code.");
    rc = false;
  }

  return rc;
}

MYON::object_type MYON_Layer::ObjectType() const
{
  return MYON::layer_object;
}

//////////////////////////////////////////////////////////////////////
//
// Interface


void MYON_Layer::SetColor( MYON_Color c)
{
  m_color = c;
}

void MYON_Layer::SetPlotColor( MYON_Color c)
{
  m_plot_color = c;
}

MYON_Color MYON_Layer::Color() const
{
  return m_color;
}

MYON_Color MYON_Layer::PlotColor() const
{
  return ((m_plot_color == MYON_UNSET_COLOR) ? m_color : m_plot_color);
}

bool MYON_Layer::SetLinetypeIndex( int index)
{
  if( m_linetype_index != index )
  {
    IncrementContentVersionNumber();
    m_linetype_index = index;
    return true;
  }
  return false;
}

int MYON_Layer::LinetypeIndex() const
{
  return m_linetype_index;
}


MYON_UUID MYON_Layer::ParentLayerId() const
{
  return ParentId();
}

void MYON_Layer::SetParentLayerId(
  MYON_UUID parent_layer_id
  )
{
  SetParentId(parent_layer_id);
}



bool MYON_Layer::IsVisible() const
{
  return IsHidden() ? false : true;
}

bool MYON_Layer::IsVisible(const MYON_3dmView* view) const
{
  if (nullptr == view)
    return IsVisible();

  if (view->m_view_type == MYON::view_type::model_view_type)
    return ModelIsVisible();

  return PerViewportIsVisible(view->m_vp.ViewportId());
}

void MYON_Layer::SetVisible( bool bVisible )
{
  SetHiddenModelComponentState( bVisible ? false : true );
  bVisible = (false == IsHidden());
  if ( ParentIdIsNil() )
    UnsetPersistentVisibility();
  else if ( bVisible )
  {
    // When a parent layer is turned off, the m_bVisible value for
    // every child layer layer is set to false. When a parent layer
    // is turned on and the visible child setting is true, the
    // child's m_bVisible value is set to true.
    //
    // This call ensures that if, at some point in the future, the
    // parent layer is turned off and then turned back on, this
    // layer will get turned back on as well.
    SetPersistentVisibility(true);
  }
}

void MYON_Layer::SetLocked( bool bLocked )
{
  SetLockedModelComponentState(bLocked);
  bLocked = IsLocked();

  if ( ParentIdIsNil() )
    UnsetPersistentLocking();
  else if ( !bLocked )
  {
    // When a parent layer is locked off, the m_bLocked value for
    // every child layer layer is set to true. When a parent layer
    // is unlocked on and the locked child setting is false, the
    // child's m_bLocked value is set to false.
    //
    // This call ensures that if, at some point in the future, the
    // parent layer is locked and then unlocked, this layer will 
    // get unlocked on as well.
    SetPersistentLocking(false);
  }
}

bool MYON_Layer::IsVisibleAndNotLocked() const
{
  return (false == IsHidden() && false == IsLocked());
}

bool MYON_Layer::IsVisibleAndLocked() const
{
  return (false == IsHidden() && IsLocked());
}

bool MYON_Layer::SetRenderMaterialIndex( int i )
{
  if ( i != m_material_index )
    IncrementContentVersionNumber();
  m_material_index = i;
  return true;
}

int MYON_Layer::RenderMaterialIndex() const
{
  return m_material_index;
}

bool MYON_Layer::SetIgesLevel( int level )
{
  m_iges_level = level;
  return true;
}

int MYON_Layer::IgesLevel() const
{
  return m_iges_level;
}

double MYON_Layer::PlotWeight() const
{
  return m_plot_weight_mm;
}

void MYON_Layer::SetPlotWeight(double plot_weight_mm)
{
  m_plot_weight_mm = (MYON_IsValid(plot_weight_mm) && (plot_weight_mm>0.0 || -1.0==plot_weight_mm) ) 
                   ? plot_weight_mm 
                   : 0.0;
}


////////////////////////////////////////////////////////////////
//
// BEGIN MYON__LayerPerViewSettings class
//

class /*NEVER EXPORT THIS CLASS DEFINITION*/ MYON__LayerPerViewSettings
{
#if !defined(MYON_BOZO_VACCINE_3E4904E6E9304fbcAA42EBD407AEFE3B)
#error Never copy this class definition or put this definition in a header file!
#endif
public:
  MYON__LayerPerViewSettings();
  void SetDefaultValues();
  bool Write( const MYON_Layer& layer, MYON_BinaryArchive& binary_archive ) const;
  bool Read( const MYON_Layer& layer, MYON_BinaryArchive& binary_archive);

  MYON__UINT32 DataCRC(MYON__UINT32 current_remainder) const;

  MYON_UUID m_viewport_id;   // id of the viewport with custom layer settings
                           // if this id is nil, then the rest of the settings
                           // in this class are meaningless.
  MYON_Color m_color;        // MYON_UNSET_COLOR means use MYON_Layer::m_color
  MYON_Color m_plot_color;   // MYON_UNSET_COLOR means use MYON_Layer::m_plot_color
  double m_plot_weight_mm; // MYON_UNSET_VALUE means use MYON_Layer::m_plot_weight_mm

  unsigned char m_visible; // 0 means use MYON_Layer::m_bVisible
                           // 1 = visible in viewport
                           // 2 = off in viewport
  unsigned char m_persistent_visibility; // 0 = unset, 1 = visible, 2 = not visible

  static
  int Compare(
      const MYON__LayerPerViewSettings* a,
      const MYON__LayerPerViewSettings* b 
      );

  static
  int CompareViewportId(
      const MYON__LayerPerViewSettings* a, 
      const MYON__LayerPerViewSettings* b
      );

  /*
  Returns:
    A bitfield that sets the bits if a layer setting is
    per viewport for the specified for the viewport. 
    The MYON_Layer::PER_VIEWPORT_SETTINGS enum values 
    which bits correspond to which settings. 
  Remarks:
    If m_viewport_id is nil, this function returns 0. 
  */
  unsigned int SettingsMask() const;

  /*
  Description:
    Copy specified settings from src to this class.
  Parameters:
    src - [in]
      settings to copy
    settings_mask - [in]
      a bitfield that specifies which settings to copy.  The bits
      are defined in the MYON_Layer::PER_VIEWPORT_SETTINGS enum.
  */
  void CopySettings( 
      const MYON__LayerPerViewSettings* src, 
      unsigned int settings_mask 
      );
};

MYON__UINT32 MYON__LayerPerViewSettings::DataCRC(MYON__UINT32 current_remainder) const
{
  const unsigned int settings_mask = SettingsMask();
  if ( 0 != settings_mask )
  {
    if ( 0 != (settings_mask & MYON_Layer::per_viewport_id) )
      current_remainder = MYON_CRC32(current_remainder,sizeof(m_viewport_id),&m_viewport_id);
    if ( 0 != (settings_mask & MYON_Layer::per_viewport_color) )
      current_remainder = MYON_CRC32(current_remainder,sizeof(m_color),&m_color);
    if ( 0 != (settings_mask & MYON_Layer::per_viewport_plot_color) )
      current_remainder = MYON_CRC32(current_remainder,sizeof(m_plot_color),&m_plot_color);
    if ( 0 != (settings_mask & MYON_Layer::per_viewport_plot_weight) )
      current_remainder = MYON_CRC32(current_remainder,sizeof(m_plot_weight_mm),&m_plot_weight_mm);
    if ( 0 != (settings_mask & MYON_Layer::per_viewport_visible) )
      current_remainder = MYON_CRC32(current_remainder,sizeof(m_visible),&m_visible);
    if ( 0 != (settings_mask & MYON_Layer::per_viewport_persistent_visibility) )
      current_remainder = MYON_CRC32(current_remainder,sizeof(m_persistent_visibility),&m_persistent_visibility);
  }
  return current_remainder;
}

void MYON__LayerPerViewSettings::CopySettings( const MYON__LayerPerViewSettings* src, unsigned int settings_mask )
{
  if ( 0 != src && this != src && 0 != settings_mask )
  {
    if ( 0 != (settings_mask & MYON_Layer::per_viewport_id) )
      m_viewport_id = src->m_viewport_id;
    if ( 0 != (settings_mask & MYON_Layer::per_viewport_color) )
      m_color = src->m_color;
    if ( 0 != (settings_mask & MYON_Layer::per_viewport_plot_color) )
      m_plot_color = src->m_plot_color;
    if ( 0 != (settings_mask & MYON_Layer::per_viewport_plot_weight) )
      m_plot_weight_mm = src->m_plot_weight_mm;
    if ( 0 != (settings_mask & MYON_Layer::per_viewport_visible) )
      m_visible = src->m_visible;
    if ( 0 != (settings_mask & MYON_Layer::per_viewport_persistent_visibility) )
      m_persistent_visibility = src->m_persistent_visibility;
  }
}

int MYON__LayerPerViewSettings::Compare( const MYON__LayerPerViewSettings* a, const MYON__LayerPerViewSettings* b )
{
  int rc = MYON_UuidCompare(a->m_viewport_id,b->m_viewport_id);
  if ( 0 == rc )
  {
    unsigned int abits = a->SettingsMask();
    unsigned int bbits = b->SettingsMask();
    rc = ((int)abits) - ((int)bbits);
    if ( 0 == rc )
    {
      if ( 0 != (MYON_Layer::per_viewport_visible & abits) )
      {
        rc = ((int)a->m_visible) - ((int)b->m_visible);
      }
      if ( 0 == rc && 0 != (MYON_Layer::per_viewport_persistent_visibility & abits) )
      {
        rc = ((int)a->m_persistent_visibility) - ((int)b->m_persistent_visibility);
      }
      if ( 0 == rc && 0 != (MYON_Layer::per_viewport_color & abits) )
      {
        rc = ((int)a->m_color) - ((int)b->m_color);
      }
      if ( 0 == rc && 0 != (MYON_Layer::per_viewport_plot_color & abits) )
      {
        rc = ((int)a->m_plot_color) - ((int)b->m_plot_color);
      }
      if ( 0 == rc && 0 != (MYON_Layer::per_viewport_plot_weight & abits) )
      {
        if ( a->m_plot_weight_mm < b->m_plot_weight_mm )
          rc = -1;
        else if ( a->m_plot_weight_mm > b->m_plot_weight_mm )
          rc = 1;
      }
    }
  }
  return rc;
}

int MYON__LayerPerViewSettings::CompareViewportId( const MYON__LayerPerViewSettings* a, const MYON__LayerPerViewSettings* b )
{
  return MYON_UuidCompare(a->m_viewport_id,b->m_viewport_id);
}

unsigned int MYON__LayerPerViewSettings::SettingsMask() const
{
  // It is critical that this function returns
  // zero when m_viewport_id = nil and returns
  // zero when no layer properties are overridden
  // for the specified viewport.
  unsigned int bits = 0;
  if ( !MYON_UuidIsNil(m_viewport_id) )
  {
    if ( MYON_UNSET_COLOR != m_color )
      bits |= MYON_Layer::per_viewport_color;
    if ( MYON_UNSET_COLOR != m_plot_color )
      bits |= MYON_Layer::per_viewport_plot_color;
    if ( (m_plot_weight_mm >= 0.0 || -1.0 == m_plot_weight_mm) && MYON_IsValid(m_plot_weight_mm) )
      bits |= MYON_Layer::per_viewport_plot_weight;
    if ( 1 == m_visible || 2 == m_visible )
      bits |= MYON_Layer::per_viewport_visible;
    if ( 1 == m_persistent_visibility || 2 == m_persistent_visibility )
      bits |= MYON_Layer::per_viewport_persistent_visibility;
    // It is critical that bit "1" is set only if
    // some layer property is overridden.  That's 
    // why the 0 != bits test is here.
    if ( 0 != bits )
      bits |= MYON_Layer::per_viewport_id;
  }

  return bits;
}

MYON__LayerPerViewSettings::MYON__LayerPerViewSettings()
{
  SetDefaultValues();
}

void MYON__LayerPerViewSettings::SetDefaultValues()
{
  memset(this,0,sizeof(*this));
  m_color = MYON_UNSET_COLOR;
  m_plot_color = MYON_UNSET_COLOR;
  m_plot_weight_mm = MYON_UNSET_VALUE;
}

bool MYON__LayerPerViewSettings::Write(const MYON_Layer& layer, MYON_BinaryArchive& binary_archive) const
{
  if ( !binary_archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,2) )
    return false;

  bool rcc = false;
  for(;;)
  {
    // This complicated "bits" stuff is to minimize number of bytes
    // written in the file.  Even though long term storage space is 
    // nearly free, we have lots of customers who complain about 
    // large file size and so ...
    unsigned int bits = SettingsMask();
    if ( !binary_archive.WriteInt(1,&bits) )
      break;
    
    if ( 0 == bits )
    {
      rcc = true;
      break; // all settings are defaults or viewport_id is nil
    }

    if ( !binary_archive.WriteUuid(m_viewport_id) )
      break;

    if ( 0 != (MYON_Layer::per_viewport_color & bits) )
    {
      if  ( !binary_archive.WriteColor(m_color) )
        break;
    }

    if ( 0 != (MYON_Layer::per_viewport_plot_color & bits) )
    {
      if ( !binary_archive.WriteColor(m_plot_color) )
        break;
    }

    if ( 0 != (MYON_Layer::per_viewport_plot_weight & bits) )
    {
      if ( !binary_archive.WriteDouble(m_plot_weight_mm) )
        break;
    }

    if ( 0 != (MYON_Layer::per_viewport_visible & bits) )
    {
      if ( !binary_archive.WriteChar(m_visible) )
        break;
      // version 1.1 addition
      if ( !binary_archive.WriteChar(m_visible) ) // (makes old a file old rhinos can read)
        break;
    }

    // 1.2 addition
    if ( 0 != (MYON_Layer::per_viewport_persistent_visibility & bits) )
    {
      if ( !binary_archive.WriteChar(m_persistent_visibility) )
        break;
    }

    rcc = true;
    break;
  }

  if ( !binary_archive.EndWrite3dmChunk() )
    rcc = false;

  return rcc;
}

bool MYON__LayerPerViewSettings::Read(const MYON_Layer& layer, MYON_BinaryArchive& binary_archive)
{
  SetDefaultValues();

  int major_version = 0;
  int minor_version = 0;
  if ( !binary_archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_version,&minor_version) )
    return false;

  bool rc = false;
  for(;;)
  {
    if (1 != major_version)
      break;

    // This complicated "bits" stuff is to minimize number of bytes
    // written in the file.  Even though long term storage space is 
    // nearly free, we have lots of customers who complain about 
    // large file size and so ...
    unsigned int bits = 0;
    if ( !binary_archive.ReadInt(1,&bits) )
      break;
    if ( 0 == bits )
    {
      rc = true;
      break;
    }

    if ( !binary_archive.ReadUuid(m_viewport_id) )
      break;

    if ( 0 != (MYON_Layer::per_viewport_color & bits) )
    {
      if ( !binary_archive.ReadColor(m_color) )
        break;
    }

    if ( 0 != (MYON_Layer::per_viewport_plot_color & bits) )
    {
      if ( !binary_archive.ReadColor(m_plot_color) )
        break;
    }

    if ( 0 != (MYON_Layer::per_viewport_plot_weight & bits) )
    {
      if ( !binary_archive.ReadDouble(&m_plot_weight_mm) )
        break;
    }

    if ( 0 != (MYON_Layer::per_viewport_visible & bits) )
    {
      if ( !binary_archive.ReadChar(&m_visible) )
        break;
      if ( minor_version >= 1 )
      {
        // for reading older Rhino files
        // Yes, writing m_visible and reading m_persistent_visibility is done on purpose.
        if ( !binary_archive.ReadChar(&m_persistent_visibility) )
          break;
      }
    }

    if ( minor_version >= 2 )
    {
      if ( 0 != (MYON_Layer::per_viewport_persistent_visibility & bits) )
      {
        if ( !binary_archive.ReadChar(&m_persistent_visibility) )
          break;
      }
    }

    if ( layer.ParentIdIsNil() )
      m_persistent_visibility = 0;
    rc = true;
    break;
  }

  if ( !binary_archive.EndRead3dmChunk() )
    rc = false;

  return rc;
}


//
//
// END MYON__LayerPerViewSettings class
//
////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
//
// BEGIN MYON__LayerExtensions user data class
//

class /*NEVER EXPORT THIS CLASS DEFINITION*/ MYON__LayerExtensions : public MYON_UserData
{
#if !defined(MYON_BOZO_VACCINE_3E4904E6E9304fbcAA42EBD407AEFE3B)
#error Never copy this class definition or put this definition in a header file!
#endif
  MYON_OBJECT_DECLARE(MYON__LayerExtensions);

public:
  MYON__LayerExtensions();
  ~MYON__LayerExtensions();
  // default copy constructor and operator= work fine.

public:
  // virtual MYON_Object override
  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;
  // virtual MYON_Object override
  unsigned int SizeOf() const override;
  // virtual MYON_Object override
  MYON__UINT32 DataCRC(MYON__UINT32 current_remainder) const override;
  // virtual MYON_Object override
  bool Write(MYON_BinaryArchive& binary_archive) const override;
  // virtual MYON_Object override
  bool Read(MYON_BinaryArchive& binary_archive) override;
  // virtual MYON_UserData override
  bool Archive() const override;
  // virtual MYON_UserData override
  bool GetDescription( MYON_wString& description ) override;

public:
  bool IsEmpty() const;

  static
  MYON__LayerPerViewSettings* ViewportSettings(
      const MYON_Layer& layer, const unsigned char* layer_m_extension_bits, 
      MYON_UUID viewport_id, 
      bool bCreate
      );

  static
  void DeleteViewportSettings(
      const MYON_Layer& layer, const unsigned char* layer_m_extension_bits, 
      const MYON__LayerPerViewSettings* vp_settings_to_delete
      );

  static
  MYON__LayerExtensions* LayerExtensions(
      const MYON_Layer& layer, const unsigned char* layer_m_extension_bits,
      bool bCreate
      );

  // per viewport overrides of color, linetype, plot color, plot weight, and visibility
  MYON_SimpleArray<MYON__LayerPerViewSettings> m_vp_settings;
};

#undef MYON_BOZO_VACCINE_3E4904E6E9304fbcAA42EBD407AEFE3B

MYON_OBJECT_IMPLEMENT(MYON__LayerExtensions,MYON_UserData,"3E4904E6-E930-4fbc-AA42-EBD407AEFE3B");

MYON__LayerExtensions* MYON__LayerExtensions::LayerExtensions(const MYON_Layer& layer, const unsigned char* layer_m_extension_bits, bool bCreate)
{
  MYON__LayerExtensions* ud = MYON__LayerExtensions::Cast(layer.GetUserData(MYON_CLASS_ID(MYON__LayerExtensions)));

  if ( 0 == ud )
  {
    if ( bCreate )
    {
      ud = new MYON__LayerExtensions();
      const_cast<MYON_Layer&>(layer).AttachUserData(ud);
      // Clear 0x01 bit of MYON_Layer::m_extension_bits so 
      // MYON_Layer visibility and color queries will check
      // for MYON__LayerExtensions userdata.
      ClearExtensionBit( const_cast<unsigned char*>(layer_m_extension_bits), 0x01 );
    }
    else
    {
      // Set 0x01 bit of MYON_Layer::m_extension_bits so 
      // MYON_Layer visibility and color queries will not
      // perform the expensive check for MYON__LayerExtensions 
      // userdata. This speeds up visibility and color queries 
      // that occur millions of times when complicated models
      // are rendered.
      SetExtensionBit( const_cast<unsigned char*>(layer_m_extension_bits), 0x01 );
    }
  }
  else
  {
    // Clear 0x01 bit of MYON_Layer::m_extension_bits so 
    // MYON_Layer visibility and color queries will check
    // for MYON__LayerExtensions userdata.
    ClearExtensionBit( const_cast<unsigned char*>(layer_m_extension_bits), 0x01 );
  }

  return ud;
}

MYON__LayerExtensions::MYON__LayerExtensions()
{
  m_userdata_uuid = MYON_CLASS_ID(MYON__LayerExtensions);
  m_application_uuid = MYON_opennurbs5_id;
  m_userdata_copycount = 1;
}

MYON__LayerExtensions::~MYON__LayerExtensions()
{
}

// virtual MYON_Object override
bool MYON__LayerExtensions::IsValid( MYON_TextLog* text_log ) const
{
  return true;
}

// virtual MYON_Object override
unsigned int MYON__LayerExtensions::SizeOf() const
{
  size_t sz = sizeof(*this) - sizeof(MYON_UserData);
  sz += m_vp_settings.SizeOfArray();
  return (unsigned int)sz;
}

// virtual MYON_Object override
MYON__UINT32 MYON__LayerExtensions::DataCRC(MYON__UINT32 current_remainder) const
{
  MYON__UINT32 crc = 0;
  crc = m_vp_settings.DataCRC(crc);
  return crc;
}

// virtual MYON_Object override
bool MYON__LayerExtensions::Write(MYON_BinaryArchive& binary_archive) const
{
  bool rc = binary_archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,0);
  if ( !rc )
    return false;

  for(;;)
  {
    const MYON_Layer* layer = MYON_Layer::Cast( Owner() );
    if ( 0 == layer )
      break;
    int count = m_vp_settings.Count();
    rc = binary_archive.WriteInt(count);
    if ( !rc ) break;
    for ( int i = 0; i < count && rc; i++ )
    {
      rc = m_vp_settings[i].Write( *layer, binary_archive );
    }
    if (!rc) break;

    break;
  }

  if ( !binary_archive.EndWrite3dmChunk() )
    rc = false;

  return rc;
}

// virtual MYON_Object override
bool MYON__LayerExtensions::Read(MYON_BinaryArchive& binary_archive)
{
  m_vp_settings.SetCount(0);

  int major_version = 0;
  int minor_version = 0;
  bool rc = binary_archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK,&major_version,&minor_version);
  if ( !rc )
    return false;

  for(;;)
  {
    const MYON_Layer* layer = MYON_Layer::Cast( Owner() );
    rc = ( 0 != layer );
    if (!rc) break;

    rc = (1 == major_version);
    if (!rc) break;

    int count = 0;
    rc = binary_archive.ReadInt(&count);
    if ( !rc ) break;
    m_vp_settings.Reserve(count);
    for ( int i = 0; i < count; i++ )
    {
      rc = m_vp_settings.AppendNew().Read(*layer,binary_archive);
      if (!rc) 
      {
        m_vp_settings.Remove();
        break;
      }
      if ( 0 == m_vp_settings.Last()->SettingsMask() )
        m_vp_settings.Remove();
    }

    // to make MYON_Layer::PerViewportSettingsCRC() return
    // equal values for equal settings, it is critical
    // that m_vp_settings[] be sorted.
    m_vp_settings.QuickSort(MYON__LayerPerViewSettings::Compare);

    if (!rc) break;

    break;
  }

  if ( !binary_archive.EndRead3dmChunk() )
    rc = false;

  return rc;
}

// virtual MYON_UserData override
bool MYON__LayerExtensions::Archive() const
{
  return !IsEmpty();
}

// virtual MYON_UserData override
bool MYON__LayerExtensions::GetDescription( MYON_wString& description )
{
  description = L"Layer Extensions";
  return true;
}

MYON__LayerPerViewSettings* MYON__LayerExtensions::ViewportSettings( 
  const MYON_Layer& layer,
  const unsigned char* layer_m_extension_bits,
  MYON_UUID viewport_id,
  bool bCreate
  )
{
  if ( !MYON_UuidIsNil(viewport_id) )
  {
    MYON__LayerExtensions* ud = MYON__LayerExtensions::LayerExtensions(layer,layer_m_extension_bits,bCreate);
    if ( ud )
    {
      int i;
      const int vp_settings_count = ud->m_vp_settings.Count();
      MYON__LayerPerViewSettings* vp_settings = ud->m_vp_settings.Array();
      for ( i = 0; i < vp_settings_count; i++ )
      {
        if ( 0 == memcmp(&viewport_id,&vp_settings[i].m_viewport_id,sizeof(MYON_UUID)) )
          return (vp_settings+i);
      }
      if ( bCreate )
      {
        MYON__LayerPerViewSettings& new_vp_settings = ud->m_vp_settings.AppendNew();
        vp_settings = ud->m_vp_settings.Array(); // appending can grow the array
        new_vp_settings.SetDefaultValues();
        new_vp_settings.m_viewport_id = viewport_id;

        // to make MYON_Layer::PerViewportSettingsCRC() return
        // equal values for equal settings, it is critical
        // that m_vp_settings[] be sorted.
        ud->m_vp_settings.QuickSort(MYON__LayerPerViewSettings::Compare);

        for ( i = 0; i <= vp_settings_count; i++ ) // "i <= ..." is correct because of the .AppendNew()
        {
          if ( 0 == memcmp(&viewport_id,&vp_settings[i].m_viewport_id,sizeof(MYON_UUID)) )
            return (vp_settings+i);
        }
      }
    }
  }
  return 0;
}

void MYON__LayerExtensions::DeleteViewportSettings( 
  const MYON_Layer& layer, 
  const unsigned char* layer_m_extension_bits,
  const MYON__LayerPerViewSettings* vp_settings_to_delete
  )
{
  MYON__LayerExtensions* ud = MYON__LayerExtensions::LayerExtensions(layer,layer_m_extension_bits,false);
  if ( ud )
  {
    if ( 0 == vp_settings_to_delete )
    {
      delete ud;
      // Set bit 0x01 of MYON_Layer::m_extension_bits to prevent
      // MYON_Layer visibilty and color queries from wasting
      // time looking for userdata.
      SetExtensionBit( const_cast<unsigned char*>(layer_m_extension_bits), 0x01 );
    }
    else
    {
      const size_t vp_settings_count = ud->m_vp_settings.Count();
      if ( vp_settings_count > 0 )
      {
        const MYON__LayerPerViewSettings* vp_settings0 = ud->m_vp_settings.Array();
        if ( vp_settings0 <= vp_settings_to_delete )
        {
          int i = (int)(vp_settings_to_delete-vp_settings0);
          ud->m_vp_settings.Remove(i);
        }
      }
      if ( ud->IsEmpty() )
      {
        delete ud;
        // Set bit 0x01 of MYON_Layer::m_extension_bits to prevent
        // MYON_Layer visibilty and color queries from wasting
        // time looking for userdata.
        SetExtensionBit( const_cast<unsigned char*>(layer_m_extension_bits), 0x01 );
      }
    }
  }
}

bool MYON__LayerExtensions::IsEmpty() const
{
  const int count = m_vp_settings.Count();

  for ( int i = 0; i < count; i++ )
    if ( 0 != m_vp_settings[i].SettingsMask() )
      return false;

  return true; // nothing of value in this user data
}

//
// END MYON__LayerExtensions user data class
//
////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////
//
// BEGIN MYON_Layer per viewport interface functions
//

void MYON_Layer::SetPerViewportColor( MYON_UUID viewport_id, MYON_Color layer_color )
{
  if ( MYON_UuidIsNil(viewport_id) )
  {
    DeletePerViewportColor(viewport_id);
    if ( MYON_Color::UnsetColor != layer_color )
      m_color = layer_color;
  }
  else
  {
    bool bSet = ( layer_color != MYON_UNSET_COLOR );
    MYON__LayerPerViewSettings* vp_settings = MYON__LayerExtensions::ViewportSettings( *this, &m_extension_bits, viewport_id, bSet );
    if ( vp_settings )
    {
      vp_settings->m_color = layer_color;
      if ( !bSet && 0 == vp_settings->SettingsMask() )
        MYON__LayerExtensions::DeleteViewportSettings(*this, &m_extension_bits, vp_settings);
    }
  }
}

//void MYON_Layer::SetColor( MYON_Color layer_color, const MYON_UUID& viewport_id )
//{
//  SetPerViewportColor( viewport_id, layer_color );
//}

MYON_Color MYON_Layer::PerViewportColor( MYON_UUID viewport_id ) const
{
  if ( !ExtensionBit(m_extension_bits,0x01) )
  {
    const MYON__LayerPerViewSettings* vp_settings = MYON__LayerExtensions::ViewportSettings( *this, &m_extension_bits, viewport_id, false );
    if ( 0 != vp_settings && MYON_UNSET_COLOR != vp_settings->m_color )
      return vp_settings->m_color;
  }

  return m_color;
}

//MYON_Color MYON_Layer::Color( const MYON_UUID& viewport_id ) const
//{
//  return PerViewportColor( viewport_id );
//}

void MYON_Layer::SetPerViewportPlotColor( MYON_UUID viewport_id, MYON_Color plot_color )
{
  if ( MYON_UuidIsNil(viewport_id) )
  {
    DeletePerViewportPlotColor(viewport_id);
    SetPlotColor(plot_color);
  }
  else
  {
    bool bSet = ( plot_color != MYON_UNSET_COLOR );
    MYON__LayerPerViewSettings* vp_settings = MYON__LayerExtensions::ViewportSettings( *this, &m_extension_bits, viewport_id, bSet );
    if ( vp_settings )
    {
      vp_settings->m_plot_color = plot_color;
      if ( !bSet && 0 == vp_settings->SettingsMask() )
        MYON__LayerExtensions::DeleteViewportSettings(*this, &m_extension_bits,vp_settings);
    }
  }
}

//void MYON_Layer::SetPlotColor( MYON_Color plot_color, const MYON_UUID& viewport_id )
//{
//  return SetPerViewportPlotColor( viewport_id, plot_color );
//}

MYON_Color MYON_Layer::PerViewportPlotColor( MYON_UUID viewport_id ) const
{
  if ( !ExtensionBit(m_extension_bits,0x01) )
  {
    const MYON__LayerPerViewSettings* vp_settings = MYON__LayerExtensions::ViewportSettings( *this, &m_extension_bits, viewport_id, false );
    if ( 0 != vp_settings && vp_settings->m_plot_color != MYON_UNSET_COLOR )
      return vp_settings->m_plot_color;
  }

  // no per viewport settings
  // 2-Nov-2009 Dale Fugier, modified to call default PlotColor()
  return PlotColor();
}

void MYON_Layer::SetPerViewportPlotWeight( MYON_UUID viewport_id, double plot_weight_mm )
{
  if ( MYON_UuidIsNil(viewport_id) )
  {
    DeletePerViewportPlotWeight(viewport_id);
    SetPlotWeight(plot_weight_mm); // this call handles invalid plot weights
  }
  else
  {
    bool bSet = ( MYON_IsValid(plot_weight_mm) && (plot_weight_mm>=0.0 || -1.0==plot_weight_mm) );
    MYON__LayerPerViewSettings* vp_settings = MYON__LayerExtensions::ViewportSettings( *this, &m_extension_bits, viewport_id, bSet );
    if ( vp_settings )
    {
      vp_settings->m_plot_weight_mm = (bSet) ? plot_weight_mm : MYON_UNSET_VALUE;
      if ( !bSet && 0 == vp_settings->SettingsMask() )
        MYON__LayerExtensions::DeleteViewportSettings(*this, &m_extension_bits, vp_settings);
    }
  }
}

double MYON_Layer::PerViewportPlotWeight( MYON_UUID viewport_id ) const
{
  if ( !ExtensionBit(m_extension_bits,0x01) )
  {
    const MYON__LayerPerViewSettings* vp_settings = MYON__LayerExtensions::ViewportSettings( *this, &m_extension_bits, viewport_id, false );
    if ( 0 != vp_settings && (vp_settings->m_plot_weight_mm >= 0.0 || -1.0 == vp_settings->m_plot_weight_mm) )
      return vp_settings->m_plot_weight_mm;
  }
  return PlotWeight();
}

// {5CCA6037-AFC7-4204-9548-EC32CD7177D6}
static const MYON_UUID MYON_model_viewport_id = { 0x5cca6037, 0xafc7, 0x4204, { 0x95, 0x48, 0xec, 0x32, 0xcd, 0x71, 0x77, 0xd6 } };

bool MYON_Layer::ModelIsVisible() const
{
  return PerViewportIsVisible(MYON_model_viewport_id);
}

void MYON_Layer::SetModelVisible(bool bVisible)
{
  SetPerViewportVisible(MYON_model_viewport_id, bVisible);
}

bool MYON_Layer::ModelPersistentVisibility() const
{
  return PerViewportPersistentVisibility(MYON_model_viewport_id);
}

void MYON_Layer::SetModelPersistentVisibility(bool bPersistentVisibility)
{
  SetPerViewportPersistentVisibility(MYON_model_viewport_id, bPersistentVisibility);
}

void MYON_Layer::UnsetModelPersistentVisibility()
{
  UnsetPerViewportPersistentVisibility(MYON_model_viewport_id);
}

void MYON_Layer::DeleteModelVisible()
{
  DeletePerViewportVisible(MYON_model_viewport_id);
}

bool MYON_Layer::PerViewportIsVisible( MYON_UUID viewport_id ) const
{
  if ( false == ExtensionBit(m_extension_bits,0x01) && MYON_nil_uuid != viewport_id )
  {
    const MYON__LayerPerViewSettings* vp_settings = MYON__LayerExtensions::ViewportSettings( *this, &m_extension_bits, viewport_id, false );
    if (vp_settings)
    {
      if ( 1 == vp_settings->m_visible )
        return true;  // per viewport MYON setting overrides layer setting
      if ( 2 == vp_settings->m_visible )
        return false; // per viewport OFF setting overrides layer setting
    }
  }

  return IsVisible(); // use layer setting
}

void MYON_Layer::SetPerViewportVisible( MYON_UUID viewport_id, bool bVisible )
{
  if ( MYON_UuidIsNil(viewport_id) )
  {
    // remove per view visible settings
    DeletePerViewportVisible(viewport_id);

    // set general visibility setting
    SetVisible(bVisible);
  }
  else 
  {
    MYON__LayerPerViewSettings* vp_settings = MYON__LayerExtensions::ViewportSettings( *this, &m_extension_bits, viewport_id, true );
    if (vp_settings)
    {
      vp_settings->m_visible = (bVisible)
        ? 1  // layer is on in this viewport
        : 2; // layer is off in this viewport
      if ( ParentIdIsNil() )
        vp_settings->m_persistent_visibility = 0;
      else if ( bVisible )
        vp_settings->m_persistent_visibility = 1;
    }
  }
}

bool MYON_Layer::PerViewportPersistentVisibility( MYON_UUID viewport_id ) const
{
  // added to fix bug 82587
  if ( !ExtensionBit(m_extension_bits,0x01) && MYON_UuidIsNotNil(viewport_id) )
  {
    MYON__LayerPerViewSettings* vp_settings = MYON__LayerExtensions::ViewportSettings( *this, &m_extension_bits, viewport_id, false );
    if ( 0 != vp_settings )
    {
      if ( 1 == vp_settings->m_visible )
        return true;
      if ( ParentIdIsNotNil() )
      {
        if ( 1 == vp_settings->m_persistent_visibility )
          return true;
        if ( 2 == vp_settings->m_persistent_visibility )
          return false;
      }
      if ( 2 == vp_settings->m_visible )
        return false;
    }
  }

  return PersistentVisibility();
}

void MYON_Layer::SetPerViewportPersistentVisibility( MYON_UUID viewport_id, bool bVisibleChild )
{
  // added to fix bug 82587
  if ( MYON_UuidIsNotNil(viewport_id) )
  {
    bool bCreate = false; // This "false" is correct because the per viewport visibility
                          // setting needs to be in existance for this call to make any
                          // sense in the first place.
    MYON__LayerPerViewSettings* vp_settings = MYON__LayerExtensions::ViewportSettings( *this, &m_extension_bits, viewport_id, bCreate );
    if (vp_settings )
      vp_settings->m_persistent_visibility = bVisibleChild ? 1 : 2;
  }
}

void MYON_Layer::UnsetPerViewportPersistentVisibility( MYON_UUID viewport_id )
{
  // added to fix bug 82587
  if ( MYON_UuidIsNil(viewport_id) )
  {
    MYON__LayerExtensions* ud = MYON__LayerExtensions::LayerExtensions( *this, &m_extension_bits, false );
    if ( 0 != ud )
    {
      for ( int i = 0; i < ud->m_vp_settings.Count(); i++ )
      {
        ud->m_vp_settings[i].m_persistent_visibility = 0;
      }
    }
  }
  else
  {
    bool bCreate = false; // This "false" is correct because the per viewport visibility
                          // setting needs to be in existance for this call to make any
                          // sense in the first place.
    MYON__LayerPerViewSettings* vp_settings = MYON__LayerExtensions::ViewportSettings( *this, &m_extension_bits, viewport_id, bCreate );
    if (vp_settings )
      vp_settings->m_persistent_visibility = 0;
  }
}

void MYON_Layer::DeletePerViewportColor( const MYON_UUID& viewport_id )
{
  if ( MYON_UuidIsNil(viewport_id) )
  {
    MYON__LayerExtensions* ud = MYON__LayerExtensions::LayerExtensions(*this,&m_extension_bits,false);
    if ( 0 != ud )
    {
      for ( int i = ud->m_vp_settings.Count(); i--; /*empty iterator*/ )
      {
        ud->m_vp_settings[i].m_color = MYON_Color::UnsetColor;
        if ( 0 == ud->m_vp_settings[i].SettingsMask() )
          ud->m_vp_settings.Remove(i);
      }
      if ( ud->IsEmpty() )
      {
        MYON__LayerExtensions::DeleteViewportSettings( *this, &m_extension_bits, 0 );
        ud = 0;
      }
    }
  }
  else
  {
    MYON__LayerPerViewSettings* vp_settings = MYON__LayerExtensions::ViewportSettings( *this, &m_extension_bits, viewport_id, false );
    if (vp_settings) 
    {
      vp_settings->m_color = MYON_Color::UnsetColor;
      if ( 0 == vp_settings->SettingsMask() )
        MYON__LayerExtensions::DeleteViewportSettings(*this,&m_extension_bits,vp_settings);
    }
  }
}

void MYON_Layer::DeletePerViewportPlotColor( const MYON_UUID& viewport_id )
{
  if ( MYON_UuidIsNil(viewport_id) )
  {
    MYON__LayerExtensions* ud = MYON__LayerExtensions::LayerExtensions(*this,&m_extension_bits,false);
    if ( 0 != ud )
    {
      for ( int i = ud->m_vp_settings.Count(); i--; /*empty iterator*/ )
      {
        ud->m_vp_settings[i].m_plot_color = MYON_UNSET_COLOR;
        if ( 0 == ud->m_vp_settings[i].SettingsMask() )
          ud->m_vp_settings.Remove(i);
      }
      if ( ud->IsEmpty() )
      {
        MYON__LayerExtensions::DeleteViewportSettings( *this, &m_extension_bits, 0 );
        ud = 0;
      }
    }
  }
  else
  {
    MYON__LayerPerViewSettings* vp_settings = MYON__LayerExtensions::ViewportSettings( *this, &m_extension_bits, viewport_id, false );
    if (vp_settings) 
    {
      vp_settings->m_plot_color = MYON_UNSET_COLOR;
      if ( 0 == vp_settings->SettingsMask() )
        MYON__LayerExtensions::DeleteViewportSettings(*this,&m_extension_bits,vp_settings);
    }
  }
}

int MYON_Layer::UpdateViewportIds( const MYON_UuidPairList& viewport_id_map )
{
  if ( viewport_id_map.Count() <= 0 )
    return 0;
  MYON__LayerExtensions* ud = MYON__LayerExtensions::LayerExtensions(*this,&m_extension_bits,false);
  if ( 0 == ud )
    return 0;
  int rc = 0;
  MYON_UUID new_id;
  for ( int i = 0; i < ud->m_vp_settings.Count(); i++ )
  {
    MYON__LayerPerViewSettings& s = ud->m_vp_settings[i];
    if ( viewport_id_map.FindId1(s.m_viewport_id,&new_id) && s.m_viewport_id != new_id )
    {
      s.m_viewport_id = new_id;
      rc++;
    }
  }
  return rc;
}

void MYON_Layer::DeletePerViewportPlotWeight( const MYON_UUID& viewport_id )
{
  if ( MYON_UuidIsNil(viewport_id) )
  {
    MYON__LayerExtensions* ud = MYON__LayerExtensions::LayerExtensions(*this,&m_extension_bits,false);
    if ( 0 != ud )
    {
      for ( int i = ud->m_vp_settings.Count(); i--; /*empty iterator*/ )
      {
        ud->m_vp_settings[i].m_plot_weight_mm = MYON_UNSET_VALUE;
        if ( 0 == ud->m_vp_settings[i].SettingsMask() )
          ud->m_vp_settings.Remove(i);
      }
      if ( ud->IsEmpty() )
      {
        MYON__LayerExtensions::DeleteViewportSettings( *this, &m_extension_bits, 0 );
        ud = 0;
      }
    }
  }
  else
  {
    MYON__LayerPerViewSettings* vp_settings = MYON__LayerExtensions::ViewportSettings( *this, &m_extension_bits, viewport_id, false );
    if (vp_settings) 
    {
      vp_settings->m_plot_weight_mm = MYON_UNSET_VALUE;
      if ( 0 == vp_settings->SettingsMask() )
        MYON__LayerExtensions::DeleteViewportSettings(*this,&m_extension_bits,vp_settings);
    }
  }
}

void MYON_Layer::DeletePerViewportVisible( const MYON_UUID& viewport_id )
{
  if ( MYON_UuidIsNil(viewport_id) )
  {
    MYON__LayerExtensions* ud = MYON__LayerExtensions::LayerExtensions(*this,&m_extension_bits,false);
    if ( 0 != ud )
    {
      for ( int i = ud->m_vp_settings.Count(); i--; /*empty iterator*/ )
      {
        ud->m_vp_settings[i].m_visible = 0;
        ud->m_vp_settings[i].m_persistent_visibility = 0;
        if ( 0 == ud->m_vp_settings[i].SettingsMask() )
          ud->m_vp_settings.Remove(i);
      }
      if ( ud->IsEmpty() )
      {
        MYON__LayerExtensions::DeleteViewportSettings( *this, &m_extension_bits, 0 );
        ud = 0;
      }
    }
  }
  else
  {
    MYON__LayerPerViewSettings* vp_settings = MYON__LayerExtensions::ViewportSettings( *this, &m_extension_bits, viewport_id, false );
    if (vp_settings) 
    {
      vp_settings->m_visible = 0;
      vp_settings->m_persistent_visibility = 0;
      if ( 0 == vp_settings->SettingsMask() )
        MYON__LayerExtensions::DeleteViewportSettings(*this,&m_extension_bits,vp_settings);
    }
  }
}

void MYON_Layer::GetPerViewportVisibilityViewportIds(
    MYON_SimpleArray<MYON_UUID>& viewport_id_list
    ) const
{
  viewport_id_list.SetCount(0);
  const MYON__LayerExtensions* ud = MYON__LayerExtensions::LayerExtensions(*this,&m_extension_bits,false);
  if ( 0 != ud )
  {
    const int count = ud->m_vp_settings.Count();
    if ( count > 0 )
    {
      viewport_id_list.Reserve(count);
      for( int i = 0; i < count; i++ )
      {
        const MYON__LayerPerViewSettings& s = ud->m_vp_settings[i];
        if (    0 != ( MYON_Layer::per_viewport_visible & s.SettingsMask() ) 
             || 0 != ( MYON_Layer::per_viewport_persistent_visibility & s.SettingsMask() ) 
           )
        {
          viewport_id_list.Append(s.m_viewport_id);
        }
      }
    }
  }
}

bool MYON_Layer::HasPerViewportSettings( const MYON_UUID& viewport_id ) const
{
  return HasPerViewportSettings( viewport_id, 0xFFFFFFFF );
}

bool MYON_Layer::HasPerViewportSettings(
    MYON_UUID viewport_id,
    unsigned int settings_mask
    ) const
{

  if ( 0 != settings_mask )
  {
    if ( MYON_UuidIsNil(viewport_id) )
    {
      const MYON__LayerExtensions* ud = MYON__LayerExtensions::LayerExtensions(*this,&m_extension_bits,false);
      if ( 0 != ud )
      {
        const int count = ud->m_vp_settings.Count();
        for ( int i = 0; i < count; i++ )
        {
          const MYON__LayerPerViewSettings& s = ud->m_vp_settings[i];
          if ( 0 != (settings_mask & s.SettingsMask()) )
            return true;
        }
      }
    }
    else
    {
      const MYON__LayerPerViewSettings* pvs = MYON__LayerExtensions::ViewportSettings( *this, &m_extension_bits, viewport_id, false );
      if ( 0 != pvs && 0 != (settings_mask & pvs->SettingsMask() ) )
        return true;
    }
  }

  return false;
}

bool MYON_Layer::CopyPerViewportSettings(MYON_UUID source_viewport_id, MYON_UUID destination_viewport_id)
{
  bool rc = false;
  if (    MYON_UuidIsNotNil(source_viewport_id) 
       && MYON_UuidIsNotNil(destination_viewport_id) 
       && 0 != MYON_UuidCompare(source_viewport_id, destination_viewport_id)
     )
  {
    const MYON__LayerPerViewSettings* src = MYON__LayerExtensions::ViewportSettings( *this, &m_extension_bits, source_viewport_id, false );
    if( 0 != src )
    {
      // Make a local copy of the source settings because
      // the pointer to the source settings may be invalid
      // after adding storage for the destination settings.
      const MYON__LayerPerViewSettings local_src(*src);
      src = 0; // never use this pointer again in this function.
      MYON__LayerPerViewSettings* dst = MYON__LayerExtensions::ViewportSettings( *this, &m_extension_bits, destination_viewport_id, true);
      if( 0 != dst )
      {
        *dst = local_src;
        dst->m_viewport_id = destination_viewport_id;
        rc = true;
      }
    }
  }
  return rc;
}

bool MYON_Layer::CopyPerViewportSettings( 
    const MYON_Layer& source_layer,
    MYON_UUID viewport_id,
    unsigned int settings_mask
    )
{
  bool rc = false;
  if ( 0 != settings_mask && this != &source_layer )
  {
    if ( MYON_UuidIsNil(viewport_id) )
    {
      // copy per viwport settings for every viewport
      const MYON__LayerExtensions* soruce_layer_ud = MYON__LayerExtensions::LayerExtensions(source_layer,&source_layer.m_extension_bits,false);
      if ( 0 != soruce_layer_ud )
      {
        const int count = soruce_layer_ud->m_vp_settings.Count();
        for ( int i = 0; i < count; i++ )
        {
          const MYON__LayerPerViewSettings& src = soruce_layer_ud->m_vp_settings[i];
          MYON__LayerPerViewSettings* dst = MYON__LayerExtensions::ViewportSettings( *this, &m_extension_bits, src.m_viewport_id, true);
          if ( 0 != dst )
          {
            dst->CopySettings(&src,settings_mask);
            rc = true;
          }
        }
      }
    }
    else
    {
      // copy per viwport settings for a specified viewport.
      const MYON__LayerPerViewSettings* src = MYON__LayerExtensions::ViewportSettings( source_layer, &source_layer.m_extension_bits, viewport_id, false);
      if ( 0 != src )
      {
        MYON__LayerPerViewSettings* dst = MYON__LayerExtensions::ViewportSettings( *this, &m_extension_bits, viewport_id, true);
        if ( 0 != dst )
        {
          dst->CopySettings(src,settings_mask);
          rc = true;
        }
      }
    }
  }
  return rc;
}


void MYON_Layer::DeletePerViewportSettings( const MYON_UUID& viewport_id ) const
{
  if ( MYON_UuidIsNil(viewport_id) )
  {
    MYON__LayerExtensions::DeleteViewportSettings(*this,&m_extension_bits,0);
  }
  else
  {
    MYON__LayerPerViewSettings* vp_settings = MYON__LayerExtensions::ViewportSettings( *this, &m_extension_bits, viewport_id, false );
    if ( vp_settings )
      MYON__LayerExtensions::DeleteViewportSettings(*this,&m_extension_bits,vp_settings);
  }
}


void MYON_Layer::CullPerViewportSettings( int viewport_id_count, const MYON_UUID* viewport_id_list )
{
  MYON__LayerExtensions* ud = MYON__LayerExtensions::LayerExtensions(*this,&m_extension_bits,false);
  if ( 0 != ud )
  {
    if ( viewport_id_count <= 0 )
    {
      // delete all per viewport settings
      MYON__LayerExtensions::DeleteViewportSettings( *this, &m_extension_bits, 0 );
      ud = 0;
    }
    else if ( viewport_id_count > 0 && 0 != viewport_id_list )
    {
      int i, j;
      for ( i = ud->m_vp_settings.Count(); i--; /*empty iterator*/ )
      {
        const MYON_UUID vp_id = ud->m_vp_settings[i].m_viewport_id;
        for ( j = 0; j < viewport_id_count; j++ )
        {
          if ( 0 == memcmp(&viewport_id_list[i],&vp_id,sizeof(vp_id)) )
            break;
        }
        if ( j >= viewport_id_count )
        {
          // ud->m_vp_settings[i].m_viewport_id is NOT in viewport_id_list[]
          ud->m_vp_settings.Remove(i);
        }
      }
      if ( ud->IsEmpty() )
      {
        // nothing useful in ud
        MYON__LayerExtensions::DeleteViewportSettings( *this, &m_extension_bits, 0 );
        ud = 0;
      }
    }
  }
}

MYON__UINT32 MYON_Layer::PerViewportSettingsCRC() const
{
  MYON__UINT32 crc = 0;
  if ( !ExtensionBit(m_extension_bits,0x01) )
  {
    MYON__LayerExtensions* ud = MYON__LayerExtensions::LayerExtensions(*this,&m_extension_bits,false);
    if ( 0 != ud )
    {
      for ( int i = 0; i < ud->m_vp_settings.Count(); i++ )
        crc = ud->m_vp_settings[i].DataCRC(crc);
    }
  }
  return crc;
}

//
// END MYON_Layer per viewport interface functions
//
////////////////////////////////////////////////////////////////


bool MYON_Layer::PersistentVisibility() const
{
  if ( !IsVisible() && ParentIdIsNotNil() )
  {
    switch ( 0x06 & m_extension_bits )
    {
    case 0x02:
      return true;
    case 0x04:
      return false;
    }
  }

  return IsVisible();
}

void MYON_Layer::SetPersistentVisibility(bool bVisibleChild)
{
  const unsigned char and_mask = 0xF9;
  const unsigned char or_bit = ParentIdIsNotNil() 
                             ? (bVisibleChild ? 0x02 : 0x04)
                             : 0x00;
  m_extension_bits &= and_mask;
  m_extension_bits |= or_bit;
}

void MYON_Layer::UnsetPersistentVisibility()
{
  const unsigned char and_mask = 0xF9;
  m_extension_bits &= and_mask;
}

bool MYON_Layer::PersistentLocking() const
{
  if ( IsLocked() && ParentIdIsNotNil() )
  {
    switch ( 0x18 & m_extension_bits )
    {
    case 0x08:
      return true;
    case 0x10:
      return false;
    }
  }

  return IsLocked();
}

void MYON_Layer::SetPersistentLocking(bool bLockedChild)
{
  const unsigned char and_mask = 0xE7;
  const unsigned char or_bit = ParentIdIsNotNil()
                             ? (bLockedChild ? 0x08 : 0x10)
                             : 0x00;
  m_extension_bits &= and_mask;
  m_extension_bits |= or_bit;
}

void MYON_Layer::UnsetPersistentLocking()
{
  // a set bit means the child will be unlocked when the parent is unlocked
  const unsigned char and_mask = 0xE7;
  m_extension_bits &= and_mask;
}

void MYON_Layer::SetClipParticipationForAll()
{
  // default is true for all clipping planes. If our private pointer hasn't
  // been created, there is no need to do anything
  if (nullptr == m_private)
    return;

  m_private->m_clipplane_list.Empty();
  m_private->m_clipping_proof = false;
}
void MYON_Layer::SetClipParticipationForNone()
{
  if (nullptr == m_private)
    m_private = new MYON_LayerPrivate();
  m_private->m_clipplane_list.Empty();
  m_private->m_clipping_proof = true;
}
void MYON_Layer::SetClipParticipationList(const MYON_UUID* clippingPlaneIds, int count)
{
  if (nullptr == clippingPlaneIds || count < 1)
    SetClipParticipationForAll();

  if (nullptr == m_private)
    m_private = new MYON_LayerPrivate();
  m_private->m_clipplane_list.Empty();
  for (int i = 0; i < count; i++)
    m_private->m_clipplane_list.AddUuid(clippingPlaneIds[i], true);

  m_private->m_clipping_proof = false;
}
void MYON_Layer::GetClipParticipation(
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

MYON::SectionFillRule MYON_Layer::SectionFillRule() const
{
  return m_private ? m_private->m_section_fill_rule : DefaultLayerPrivate.m_section_fill_rule;
}
void MYON_Layer::SetSectionFillRule(MYON::SectionFillRule rule)
{
  if (SectionFillRule() == rule)
    return;
  if (nullptr == m_private)
    m_private = new MYON_LayerPrivate();
  m_private->m_section_fill_rule = rule;
}

int MYON_Layer::SectionHatchIndex() const
{
  return m_private ? m_private->m_section_hatch_index : DefaultLayerPrivate.m_section_hatch_index;
}
void MYON_Layer::SetSectionHatchIndex(int index)
{
  if (SectionHatchIndex() == index)
    return;
  if (nullptr == m_private)
    m_private = new MYON_LayerPrivate();
  m_private->m_section_hatch_index = index;
}

double MYON_Layer::SectionHatchScale() const
{
  return m_private ? m_private->m_section_hatch_scale : DefaultLayerPrivate.m_section_hatch_scale;
}
void MYON_Layer::SetSectionHatchScale(double scale)
{
  if (SectionHatchScale() == scale)
    return;
  if (nullptr == m_private)
    m_private = new MYON_LayerPrivate();
  m_private->m_section_hatch_scale = scale;
}

double MYON_Layer::SectionHatchRotation() const
{
  return m_private ? m_private->m_section_hatch_rotation : DefaultLayerPrivate.m_section_hatch_rotation;
}
void MYON_Layer::SetSectionHatchRotation(double rotation)
{
  if (SectionHatchRotation() == rotation)
    return;
  if (nullptr == m_private)
    m_private = new MYON_LayerPrivate();
  m_private->m_section_hatch_rotation = rotation;
}


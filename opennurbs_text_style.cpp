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

MYON_OBJECT_IMPLEMENT( MYON_TextStyle, MYON_ModelComponent, "4F0F51FB-35D0-4865-9998-6D2C6A99721D" );

const MYON_TextStyle* MYON_TextStyle::FromModelComponentRef(
  const class MYON_ModelComponentReference& model_component_reference,
  const MYON_TextStyle* none_return_value
  )
{
  const MYON_TextStyle* p = MYON_TextStyle::Cast(model_component_reference.ModelComponent());
  return (nullptr != p) ? p : none_return_value;
}

MYON_TextStyle::MYON_TextStyle()
  : MYON_ModelComponent(MYON_ModelComponent::Type::TextStyle)
{}

MYON_TextStyle::MYON_TextStyle(const MYON_TextStyle& src)
  : MYON_ModelComponent(MYON_ModelComponent::Type::TextStyle,src)
{
  Internal_Copy(src);
}

MYON_TextStyle& MYON_TextStyle::operator=(const MYON_TextStyle& src)
{
  if (this != &src)
  {
    m_is_locked_bits = 0;
    MYON_ModelComponent::operator=(src);
    Internal_Copy(src);
  }
  return *this;
}

void MYON_TextStyle::Internal_Copy(
  const MYON_TextStyle& src
  )
{
  m_managed_font = src.m_managed_font;
  m_font_description = src.m_font_description;
  m_is_set_bits = src.m_is_set_bits;
  // DO NOT COPY m_is_locked_bits.
}

// On September 24, 2015 the "V3" MYON_Font was split into
//   MYON_TextStyle (a document object) and
//   MYON_Font (a current runtime resource)
//   Versions of opennurbs before MYON_TextStyle::binary_archive_opennurbs_version
//   serialized MYON_TextStyle information in the V5 MYON_Font format.
const unsigned int MYON_TextStyle::binary_archive_opennurbs_version = MYON_VersionNumberConstruct(6, 0, 2015, 9, 23, 0);

bool MYON_TextStyle::Write(
  MYON_BinaryArchive& file // serialize definition to binary archive
  ) const
{
  const MYON_Font* font 
    = (nullptr == m_managed_font || 0 == m_managed_font->RuntimeSerialNumber())
    ? &MYON_Font::Default
    : m_managed_font;

  if ( file.Archive3dmVersion() < 60
    || file.ArchiveOpenNURBSVersion() < MYON_TextStyle::binary_archive_opennurbs_version
    )
    return font->WriteV5(Index(),Id(),file);

  if (!file.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK,1,1))
    return false;
  
  bool rc = false;
  for (;;)
  {
    if (!file.WriteModelComponentAttributes(*this,MYON_ModelComponent::Attributes::BinaryArchiveAttributes))
      break;

    // We only need to save the m_font_description when we are using an
    // alternate font or writing a V5 Mac file.
    const bool bWriteFont = (nullptr != m_managed_font);
    MYON_wString postscript_name;
    MYON_wString font_description;
    if (nullptr != m_managed_font)
    {
      postscript_name = m_managed_font->PostScriptName();
      if (postscript_name.IsEmpty())
        postscript_name = m_managed_font->Description(MYON_Font::NameLocale::LocalizedFirst, MYON_wString::HyphenMinus, 0, false);
      font_description = postscript_name;
    }
    else
    {
      font_description = m_font_description;
    }

    if (MYON::RuntimeEnvironment::Apple == file.ArchiveRuntimeEnvironment() && m_apple_font_name.IsNotEmpty() && postscript_name.IsEmpty() )
    {
      font_description = m_apple_font_name;
    }

    const bool bHaveFontDescription = font_description.IsNotEmpty();

    if (!file.WriteBool(bHaveFontDescription))
      break;
    
    if (bHaveFontDescription)
    {
      if (!file.WriteString(font_description))
        break;
    }

    if (!file.WriteBool(bWriteFont))
      break;

    if (bWriteFont)
    {
      if (!m_managed_font->Write(file))
        break;
    }

    // version 1.1 added id and name
    if (!file.WriteUuid(Id()))
      break;
    if (!file.WriteString(Name()))
      break;

    rc = true;
    break;
  }
  if (!file.EndWrite3dmChunk() )
    rc = false;
  return rc;
}

static void SetNameFromFontDescription(
  const MYON_ComponentManifest& manifest,
  MYON_TextStyle& text_style
  )
{ 
  // Dale Lear RH-63824 May 3, 2021
  // It is critical that bIncludeNotOnDevice be set to false.
  // Otherwise missing fonts will have a description beginning with "[Not on device]"
  // and square brackets are not permitted in names.
  // This code is inventing a Rhino 6/7 dimstyle name from a V4 text style.
  // The text style names were unreliable in V4 and we've used the font
  // description as a proxy for years now.
  const bool bIncludeNotOnDevice = false;

  const MYON_wString font_description(text_style.Font().Description(MYON_Font::NameLocale::LocalizedFirst, MYON_wString::HyphenMinus, MYON_wString::Space, true, bIncludeNotOnDevice));

  MYON_wString text_style_name = manifest.UnusedName(
    text_style.ComponentType(),
    MYON_nil_uuid,
    static_cast<const wchar_t*>(font_description),
    nullptr,
    nullptr,
    0,
    nullptr);
  text_style.SetName(text_style_name);
}

bool MYON_TextStyle::Read(
  MYON_BinaryArchive& file // restore definition from binary archive
  )
{
  ClearModelComponentAttributes(MYON_ModelComponent::Attributes::AllAttributes);
  m_managed_font = nullptr;
  m_font_description = MYON_wString::EmptyString;

  MYON__UINT32 typecode = 0;
  MYON__INT64 big_value = 0;
  if (file.Archive3dmVersion() < 60
    || file.ArchiveOpenNURBSVersion() < MYON_TextStyle::binary_archive_opennurbs_version
    // Sep 23, 215 had several in-house WIP versions and this makes the few files created
    // on that day read correctly
    || (file.ArchiveOpenNURBSVersion() < MYON_VersionNumberConstruct(6, 0, 2015, 9, 24, 0) 
        && file.PeekAt3dmBigChunkType(&typecode,&big_value) 
        && TCODE_ANONYMOUS_CHUNK != typecode
       )
    )
  {
    MYON_Font font;
    int V5_font_index = -1;
    MYON_UUID V5_font_id = MYON_nil_uuid;
    bool rc = font.ReadV5(file,&V5_font_index,&V5_font_id);
    SetFont(font.ManagedFont());
    SetIndex(V5_font_index);
    if ( MYON_nil_uuid == V5_font_id )
      SetId();
    else
      SetId(V5_font_id);

    // Use V5 font face name to set text style name.
    if (nullptr != m_managed_font)
      SetNameFromFontDescription(file.Manifest(),*this);
    return rc;
  }

  int major_version = 0;
  int minor_version = 0;
  if (!file.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK, &major_version, &minor_version))
    return false;
  
  bool rc = false;
  for (;;)
  {
    unsigned int model_component_attributes_filter = 0;
    if (!file.ReadModelComponentAttributes(*this,&model_component_attributes_filter) )
      break;

    // We only need to save the m_font_description when we are using an
    // alternate font.
    bool bHaveFontDescription = false;
    if (!file.ReadBool(&bHaveFontDescription))
      break;
    
    MYON_wString font_description;
    MYON_wString apple_font_name;

    if (bHaveFontDescription)
    {
      if (!file.ReadString(font_description))
        break;
      font_description.TrimLeftAndRight();
      bHaveFontDescription = font_description.IsNotEmpty();
    }

    if (MYON::RuntimeEnvironment::Apple == file.ArchiveRuntimeEnvironment()
      && font_description.IsNotEmpty()
      && false == MYON_Font::IsNotAppleFontName(font_description) )
    {
      apple_font_name = font_description;
    }

    bool bReadFont = false;
    if (!file.ReadBool(&bReadFont))
      break;

    if (bReadFont)
    {
      MYON_Font font_characteristics;
      if (!font_characteristics.Read(file))
        break;
      if (apple_font_name.IsEmpty())
        apple_font_name = font_characteristics.PostScriptName();
      SetFontFromDescription(
         static_cast<const wchar_t*>(font_description),
         static_cast<const wchar_t*>(apple_font_name),
         &font_characteristics
       );
    }
    else
    {
      SetFontFromDescription(
        static_cast<const wchar_t*>(font_description),
        static_cast<const wchar_t*>(apple_font_name),
        nullptr
      );
    }

    if (minor_version < 1)
    {
      // id and name were not saved in version 1.0 
      // They are required now and are created when an old file is read.
      SetId();
      if (nullptr != m_managed_font)
        SetNameFromFontDescription(file.Manifest(),*this);
      rc = true;
      break;
    }

    // version 1.1 added id and name
    MYON_UUID id;
    if (!file.ReadUuid(id))
      break;
    SetId(id);

    MYON_wString name;
    if (!file.ReadString(name))
      break;
    SetName(name);
    
    rc = true;
    break;
  }
  if (!file.EndRead3dmChunk() )
    rc = false;
  return rc;
}

void MYON_TextStyle::ClearFont()
{
  if (false == FontIsLocked())
  {
    m_managed_font = nullptr;
    m_font_description = MYON_wString::EmptyString;
    m_apple_font_name = MYON_wString::EmptyString;
    m_is_set_bits &= ~MYON_TextStyle::font_bit;
    IncrementContentVersionNumber();
  }
}

bool MYON_TextStyle::FontIsSet() const
{
  return (0 != (m_is_set_bits & MYON_TextStyle::font_bit));
}

void MYON_TextStyle::LockFont()
{
  m_is_locked_bits |= MYON_TextStyle::font_bit;
}

bool MYON_TextStyle::FontIsLocked() const
{
  return (0 != (m_is_locked_bits & MYON_TextStyle::font_bit));
}


void MYON_TextStyle::SetFont(
  const MYON_Font& font_characteristics
  )
{
  return SetFont(&font_characteristics);
}

void MYON_TextStyle::SetFont(
  const MYON_Font* font_characteristics
  )
{
  if (false == FontIsLocked())
  {
    m_font_description = MYON_wString::EmptyString;
    m_apple_font_name = MYON_wString::EmptyString;
    
    m_managed_font
      = (nullptr == font_characteristics)
      ? nullptr
      : font_characteristics->ManagedFont();
    
    
    if (nullptr == m_managed_font)
    {
      m_managed_font = &MYON_Font::Default;
    }
    
    if (nullptr != font_characteristics)
    {
      m_font_description = font_characteristics->Description();
      m_apple_font_name = font_characteristics->PostScriptName();
    }
    
    if (m_font_description.IsEmpty())
      m_font_description = m_managed_font->Description();
    
    if (m_apple_font_name.IsEmpty())
      m_apple_font_name = m_managed_font->PostScriptName();

    m_is_set_bits |= MYON_TextStyle::font_bit;
    IncrementContentVersionNumber();
  }
}

void MYON_TextStyle::SetFontFromDescription(
  const wchar_t* font_description,
  const wchar_t* apple_font_name,
  const MYON_Font* alternate_font
  )
{
  MYON_wString local_font_description(font_description);
  local_font_description.TrimLeftAndRight();
  MYON_wString local_apple_font_name(apple_font_name);
  local_apple_font_name.TrimLeftAndRight();
  if (local_font_description.IsEmpty())
  {
    SetFont(alternate_font);
  }
  else if (false == FontIsLocked())
  {
    MYON_Font font_characteristics;
    font_characteristics.SetFromFontDescription(
      static_cast<const wchar_t*>(local_font_description),
      static_cast<const wchar_t*>(local_apple_font_name)
      );
    const MYON_Font* managed_font = font_characteristics.ManagedFont();
    if (nullptr == managed_font)
    {
      SetFont(alternate_font);
    }
    else
    {
      SetFont(managed_font);
      m_font_description = local_font_description;
      m_apple_font_name = local_apple_font_name;
    }
    unsigned char font_is_set_bit = 1;
    m_is_set_bits &= font_is_set_bit;
    IncrementContentVersionNumber();
  }
}

bool MYON_TextStyle::EqualTextStyleFontAndName(
  const MYON_TextStyle& a,
  const MYON_TextStyle& b
  )
{
  return (
    a.Font().RuntimeSerialNumber() == b.Font().RuntimeSerialNumber()
    && a.m_font_description == b.m_font_description
    && MYON_ModelComponent::CompareNameExact(a, b)
    );
}

const wchar_t* MYON_TextStyle::GetNameAndFontDescription(
  const wchar_t* sSeparator,
  MYON_wString& name_and_font_description
  ) const
{
  GetName(name_and_font_description);
  if ( FontIsSet() )
  {
    MYON_wString font_description = FontDescription();
    if (font_description != name_and_font_description)
    {
      if (name_and_font_description.IsNotEmpty() && font_description.IsNotEmpty())
        name_and_font_description += sSeparator;
      name_and_font_description += font_description;
      if (name_and_font_description.IsEmpty() || font_description.IsEmpty())
        name_and_font_description.TrimLeftAndRight();
    }
  }
  return static_cast<const wchar_t*>(name_and_font_description);
}

const MYON_Font& MYON_TextStyle::Font() const
{
  return (nullptr != m_managed_font && m_managed_font->IsManagedFont()) ? *m_managed_font : MYON_Font::Default;
}

const MYON_wString& MYON_TextStyle::FontDescription() const
{
  return m_font_description;
}

const wchar_t* MYON_TextStyle::FontDescriptionAsPointer() const
{
  return static_cast<const wchar_t*>(m_font_description);
}


const MYON_wString& MYON_TextStyle::AppleFontName() const
{
  return m_apple_font_name;
}

const wchar_t* MYON_TextStyle::AppleFontNameAsPointer() const
{
  return static_cast<const wchar_t*>(m_apple_font_name);
}


bool MYON_TextStyle::IsValid(MYON_TextLog* text_log) const
{
  return (
    FontIsSet()
    && MYON_ModelComponent::Type::TextStyle == ComponentType()
    && MYON_ModelComponent::IsValid(text_log)
    && nullptr != m_managed_font 
    && m_managed_font->IsManagedFont() 
    && m_managed_font->IsValid()
    );
}

void MYON_TextStyle::Dump(MYON_TextLog& dump) const
{
  dump.Print("Text Style\n");
  dump.PushIndent();

  MYON_ModelComponent::Dump(dump);

  dump.Print(L"Font = \"%ls\"\n", static_cast<const wchar_t*>(m_font_description));

  dump.PopIndent();
}


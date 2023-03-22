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

bool MYON_NameHash::IsValidAndNotEmpty() const
{
  return (
    (m_flags & MYON_NameHash::flags_length_mask) > 0 
    && MYON_SHA1_Hash::EmptyContentHash != m_sha1_hash
    );
}

bool MYON_NameHash::IsEmptyNameHash() const
{
  return (MYON_SHA1_Hash::EmptyContentHash == m_sha1_hash);
}

bool MYON_NameHash::IsInvalidNameHash() const
{
  return (MYON_SHA1_Hash::ZeroDigest == m_sha1_hash);
}

bool MYON_NameHash::IgnoreCase() const
{
  return (0 != m_flags && MYON_NameHash::flags_file_path != m_flags && 0 == (m_flags & MYON_NameHash::flags_case_sensitive));
}

MYON__UINT32 MYON_NameHash::MappedNameCodePointCount() const
{
  return 
    (m_flags != MYON_NameHash::flags_file_path)
    ? (m_flags & MYON_NameHash::flags_length_mask)
    : 0U;
}

MYON_UUID MYON_NameHash::ParentId() const
{
  return m_parent_id;
}

// SHA-1 hash of mapped name
MYON_SHA1_Hash MYON_NameHash::MappedNameSha1Hash() const
{
  return m_sha1_hash;
}

// 32 bit hash of mapped name
MYON__UINT32 MYON_NameHash::DataCRC(MYON__UINT32 current_remainder) const
{
  current_remainder = IdCRC(current_remainder);
  current_remainder= NameCRC(current_remainder);
  return current_remainder;
}

MYON__UINT32 MYON_NameHash::IdCRC(MYON__UINT32 current_remainder) const
{
  return MYON_CRC32(current_remainder,sizeof(m_parent_id),&m_parent_id);
}

MYON__UINT32 MYON_NameHash::NameCRC(MYON__UINT32 current_remainder) const
{
  current_remainder = MYON_CRC32(current_remainder,sizeof(m_flags),&m_flags);
  return MYON_CRC32(current_remainder,sizeof(m_sha1_hash),&m_sha1_hash);
}

int MYON_NameHash::CompareNameSHA1(
  const MYON_NameHash& a,
  const MYON_NameHash& b
  )
{
  if ( a.m_flags < b.m_flags )
    return -1;
  if ( a.m_flags > b.m_flags )
    return 1;
  return MYON_SHA1_Hash::Compare(a.m_sha1_hash,b.m_sha1_hash);
}

int MYON_NameHash::CompareNameSHA1Ptr(
  const MYON_NameHash* a,
  const MYON_NameHash* b
  )
{
  if (nullptr == a)
    a = &MYON_NameHash::EmptyNameHash;
  if (nullptr == b)
    b = &MYON_NameHash::EmptyNameHash;
  return MYON_NameHash::CompareNameSHA1(*a,*b);
}

int MYON_NameHash::CompareParentId(
  const MYON_NameHash& a,
  const MYON_NameHash& b
  )
{
  return MYON_UuidCompare(a.m_parent_id,b.m_parent_id);
}

int MYON_NameHash::CompareParentIdPtr(
  const MYON_NameHash* a,
  const MYON_NameHash* b
  )
{
  if (nullptr == a)
    a = &MYON_NameHash::EmptyNameHash;
  if (nullptr == b)
    b = &MYON_NameHash::EmptyNameHash;
  return MYON_NameHash::CompareParentId(*a,*b);
}

int MYON_NameHash::Compare(
  const MYON_NameHash& a,
  const MYON_NameHash& b
  )
{
  if ( a.m_flags < b.m_flags )
    return -1;
  if ( a.m_flags > b.m_flags )
    return 1;
  int rc = MYON_UuidCompare(a.m_parent_id,b.m_parent_id);
  if ( 0 == rc )
    rc = MYON_SHA1_Hash::Compare(a.m_sha1_hash,b.m_sha1_hash);
  return rc;
}

int MYON_NameHash::ComparePtr(
  const MYON_NameHash* a,
  const MYON_NameHash* b
  )
{
  if (nullptr == a)
    a = &MYON_NameHash::EmptyNameHash;
  if (nullptr == b)
    b = &MYON_NameHash::EmptyNameHash;
  return MYON_NameHash::Compare(*a,*b);
}

bool operator==(
  const MYON_NameHash& a,
  const MYON_NameHash& b
  )
{
  return (MYON_NameHash::Compare(a,b) == 0);
}

bool operator!=(
  const MYON_NameHash& a,
  const MYON_NameHash& b
  )
{
  return (MYON_NameHash::Compare(a,b) != 0);
}

bool operator<(
  const MYON_NameHash& a,
  const MYON_NameHash& b
  )
{
  return (MYON_NameHash::Compare(a,b) < 0);
}

bool operator<=(
  const MYON_NameHash& a,
  const MYON_NameHash& b
  )
{
  return (MYON_NameHash::Compare(a,b) <= 0);
}

bool operator>(
  const MYON_NameHash& a,
  const MYON_NameHash& b
  )
{
  return (MYON_NameHash::Compare(a,b) > 0);
}

bool operator>=(
  const MYON_NameHash& a,
  const MYON_NameHash& b
  )
{
  return (MYON_NameHash::Compare(a,b) >= 0);
}

MYON_NameHash MYON_NameHash::Create(
  const MYON_wString& name
  )
{
  return MYON_NameHash::Create(MYON_nil_uuid, name);
}

MYON_NameHash MYON_NameHash::Create(
  const MYON_wString& name,
  bool bIgnoreCase
  )
{
  return MYON_NameHash::Create(MYON_nil_uuid, name, bIgnoreCase);
}

MYON_NameHash MYON_NameHash::Create(
  const wchar_t* name
  )
{
  return MYON_NameHash::Create(MYON_nil_uuid, name);
}

MYON_NameHash MYON_NameHash::Create(
  const wchar_t* name,
  bool bIgnoreCase
  )
{
  return MYON_NameHash::Create(MYON_nil_uuid, name, bIgnoreCase);
}

MYON_NameHash MYON_NameHash::Create(
  size_t length,
  const wchar_t* name
  )
{
  return MYON_NameHash::Create(MYON_nil_uuid, length, name);
}

MYON_NameHash MYON_NameHash::Create(
  size_t length,
  const wchar_t* name,
  bool bIgnoreCase
  )
{
  return MYON_NameHash::Create(MYON_nil_uuid, length, name, bIgnoreCase);
}

MYON_NameHash MYON_NameHash::Create(
  const MYON_String& name
  )
{
  return MYON_NameHash::Create(MYON_nil_uuid, name);
}

MYON_NameHash MYON_NameHash::Create(
  const MYON_String& name,
  bool bIgnoreCase
  )
{
  return MYON_NameHash::Create(MYON_nil_uuid, name, bIgnoreCase);
}

MYON_NameHash MYON_NameHash::Create(
  const char* name
  )
{
  return MYON_NameHash::Create(MYON_nil_uuid, name);
}

MYON_NameHash MYON_NameHash::Create(
  const char* name,
  bool bIgnoreCase
  )
{
  return MYON_NameHash::Create(MYON_nil_uuid, name, bIgnoreCase);
}

MYON_NameHash MYON_NameHash::Create(
  size_t length,
  const char* name
  )
{
  return MYON_NameHash::Create(MYON_nil_uuid, length, name);
}

MYON_NameHash MYON_NameHash::Create(
  size_t length,
  const char* name,
  bool bIgnoreCase
  )
{
  return MYON_NameHash::Create(MYON_nil_uuid, length, name, bIgnoreCase);
}

MYON_NameHash MYON_NameHash::Create(
  const MYON_UUID& name_parent_id,
  const MYON_wString& name
  )
{
  return MYON_NameHash::Create(name_parent_id, name.UnsignedLength(), name.Array());
}

MYON_NameHash MYON_NameHash::Create(
  const MYON_UUID& name_parent_id,
  const MYON_wString& name,
  bool bIgnoreCase
  )
{
  return MYON_NameHash::Create(name_parent_id, name.UnsignedLength(), name.Array(), bIgnoreCase);
}

MYON_NameHash MYON_NameHash::Create(
  const MYON_UUID& name_parent_id,
  const wchar_t* name
  )
{
  return MYON_NameHash::Create(name_parent_id, MYON_wString::Length(name), name);
}

MYON_NameHash MYON_NameHash::Create(
  const MYON_UUID& name_parent_id,
  const wchar_t* name,
  bool bIgnoreCase
  )
{
  return MYON_NameHash::Create(name_parent_id, MYON_wString::Length(name), name, bIgnoreCase);
}

bool MYON_NameHash::IsFilePathHash() const
{
  return (m_flags == MYON_NameHash::flags_file_path && m_sha1_hash != MYON_SHA1_Hash::ZeroDigest);
}

MYON_NameHash MYON_NameHash::CreateFilePathHash(
  const class MYON_FileReference& file_reference
  )
{
  MYON_NameHash hash;
  if (file_reference.IsSet())
  {
    hash.m_flags = MYON_NameHash::flags_file_path;
    hash.m_sha1_hash = file_reference.FullPathHash();
  }
  else
  {
    hash.m_sha1_hash = MYON_SHA1_Hash::EmptyContentHash;
  }

  return hash;
}

MYON_NameHash MYON_NameHash::CreateFilePathHash(
  const wchar_t* file_path
  )
{
  MYON_FileReference file_reference;
  file_reference.SetFullPath(file_path, false);
  return MYON_NameHash::CreateFilePathHash(file_reference);
}

MYON_NameHash MYON_NameHash::CreateFilePathHash(
  const char* file_path
  )
{
  MYON_FileReference file_reference;
  file_reference.SetFullPath(file_path, false);
  return MYON_NameHash::CreateFilePathHash(file_reference);
}


void MYON_SHA1::AccumulateString(
  const char* sUTF8,
  int element_count,
  MYON_StringMapOrdinalType mapping
)
{
  const MYON::endian context_endian = MYON::Endian();
  MYON__UINT32 sUTF32[64];
  const size_t sUTF32_capacity = sizeof(sUTF32)/sizeof(sUTF32[0]);
  unsigned int UTF32_length = 0;
  bool bEmpty = true;

  const int char_count
    = (nullptr != sUTF8 && element_count >= 0)
    ? element_count
    : MYON_StringLengthUTF8(sUTF8);

  if ( char_count <= 0 )
    return;

  size_t length = (size_t)char_count;

  for (;;)
  {
    const char* next = nullptr;
    int UTF32_count = MYON_ConvertUTF8ToUTF32(
      false,
      sUTF8,
      (int)length,
      sUTF32,
      sUTF32_capacity,
      nullptr,
      0xFFFFFFFFU,
      0xFFFD,
      &next
      );

    size_t parsed_length
      = (nullptr != next && next > sUTF8)
      ? (next - sUTF8)
      : 0;

    if (parsed_length <= 0 || parsed_length > length )
      break;

    if (UTF32_count > 0)
    {
      UTF32_length += ((unsigned int)UTF32_count);

      for (int i = 0; i < UTF32_count; i++)
      {
        if ( MYON_StringMapOrdinalType::Identity != mapping )
          sUTF32[i] = MYON_UnicodeMapCodePointOrdinal(mapping, sUTF32[i]);
        if ( 0 != sUTF32[i] )
          bEmpty = false;
      }

      const size_t sz = ((size_t)UTF32_count) * sizeof(sUTF32[0]);

      if (MYON::endian::big_endian == context_endian)
      {
        // swap bytes so the SHA-1 value is independent of endian byte order
		    unsigned char* b = (unsigned char*)sUTF32;
        unsigned char* b1 = b + sz;
		    unsigned char  c;
		    while(b < b1) 
        {
			    c = b[0]; b[0] = b[3]; b[3] = c;
			    c = b[1]; b[1] = b[2]; b[2] = c;
			    b += sizeof(sUTF32[0]);
		    }
      }

      this->AccumulateBytes(sUTF32,sz);
    }

    sUTF8 += parsed_length;
    length -= parsed_length;
    if (0 == length)
      break;
  }
}

void MYON_SHA1::AccumulateString(
  const class MYON_String& str,
  MYON_StringMapOrdinalType mapping
)
{
  // Do not permit corrupt strings to crash this code.
  str.IsValid(false);

  AccumulateString( str.Array(), str.Length(), mapping);
}

void MYON_SHA1::AccumulateString(
  const class MYON_String& str
)
{
  AccumulateString(str, MYON_StringMapOrdinalType::Identity);
}

const MYON_SHA1_Hash MYON_StringContentHash(
  const char* sUTF8,
  int element_count,
  MYON_StringMapOrdinalType mapping
)
{
  MYON_SHA1 sha1;
  sha1.AccumulateString(sUTF8, element_count, mapping);
  return sha1.Hash();
}

void MYON_SHA1::AccumulateString(
  const wchar_t* sWideString,
  int element_count,
  MYON_StringMapOrdinalType mapping
)
{
  const MYON::endian context_endian = MYON::Endian();
  MYON__UINT32 sUTF32[64];
  const size_t sUTF32_capacity = sizeof(sUTF32)/sizeof(sUTF32[0]);
  unsigned int UTF32_length = 0;
  bool bEmpty = true;

  const int wchar_count
    = (nullptr != sWideString && element_count >= 0)
    ? element_count
    : MYON_StringLengthWideChar(sWideString);

  if ( wchar_count <= 0 )
    return;

  size_t length = (size_t)wchar_count;

  for (;;)
  {
    const wchar_t* next = nullptr;
    int UTF32_count = MYON_ConvertWideCharToUTF32(
      false,
      sWideString,
      (int)length,
      sUTF32,
      sUTF32_capacity,
      nullptr,
      0xFFFFFFFFU,
      0xFFFD,
      &next
      );

    size_t parsed_length
      = (nullptr != next && next > sWideString)
      ? (next - sWideString)
      : 0;

    if (parsed_length <= 0 || parsed_length > length )
      break;

    if (UTF32_count > 0)
    {
      UTF32_length += ((unsigned int)UTF32_count);

      for (int i = 0; i < UTF32_count; i++)
      {
        if ( MYON_StringMapOrdinalType::Identity != mapping )
          sUTF32[i] = MYON_UnicodeMapCodePointOrdinal(mapping, sUTF32[i]);
        if ( 0 != sUTF32[i] )
          bEmpty = false;
      }

      const size_t sz = ((size_t)UTF32_count) * sizeof(sUTF32[0]);

      if (MYON::endian::big_endian == context_endian)
      {
        // swap bytes so the SHA-1 value is independent of endian byte order
		    unsigned char* b = (unsigned char*)sUTF32;
        unsigned char* b1 = b + sz;
		    unsigned char  c;
		    while(b < b1) 
        {
			    c = b[0]; b[0] = b[3]; b[3] = c;
			    c = b[1]; b[1] = b[2]; b[2] = c;
			    b += sizeof(sUTF32[0]);
		    }
      }

      this->AccumulateBytes(sUTF32,sz);
    }

    sWideString += parsed_length;
    length -= parsed_length;
    if (0 == length)
      break;
  }
}

void MYON_SHA1::AccumulateString(
  const class MYON_wString& str,
  MYON_StringMapOrdinalType mapping
)
{
  // Do not permit corrupt strings to crash this code.
  str.IsValid(false);

  AccumulateString( str.Array(), str.Length(), mapping);
}

void MYON_SHA1::AccumulateString(
  const class MYON_wString& str
)
{
  AccumulateString(str, MYON_StringMapOrdinalType::Identity);
}

const MYON_SHA1_Hash MYON_StringContentHash(
  const wchar_t* sWideString,
  int element_count,
  MYON_StringMapOrdinalType mapping
)
{
  MYON_SHA1 sha1;
  sha1.AccumulateString(sWideString, element_count, mapping);
  return sha1.Hash();
}


const MYON_SHA1_Hash MYON_String::ContentHash(
  MYON_StringMapOrdinalType mapping
) const
{
  const char* sUTF8 = this->Array();
  int length = this->Length();
  return MYON_StringContentHash(sUTF8, length, mapping);
}

const MYON_SHA1_Hash MYON_wString::ContentHash(
  MYON_StringMapOrdinalType mapping
) const
{
  const wchar_t* sWideString = this->Array();
  int length = this->Length();
  return MYON_StringContentHash(sWideString, length, mapping);
}


MYON_NameHash MYON_NameHash::Create(
  const MYON_UUID& name_parent_id,
  size_t length,
  const wchar_t* name
)
{
  const bool bIgnoreCase = true;
  return MYON_NameHash::Create(name_parent_id, length, name, bIgnoreCase);
}

MYON_NameHash MYON_NameHash::Create(
  const MYON_UUID& name_parent_id,
  size_t length, 
  const wchar_t* name,
  bool bIgnoreCase
  )
{
  MYON_NameHash hash;

  if (0 == length || nullptr == name)
  {
    // Do not return MYON_NameHash::EmptyNameHash because
    // this code is used to initialize that static member.
    hash.m_sha1_hash = MYON_SHA1_Hash::EmptyContentHash;
    return hash;
  }
  
  if (false == MYON_ModelComponent::IsValidComponentName(name))
    return MYON_NameHash::UnsetNameHash;

  const MYON::endian context_endian = MYON::Endian();
  MYON__UINT32 sUTF32[64];
  const size_t sUTF32_capacity = sizeof(sUTF32)/sizeof(sUTF32[0]);
  unsigned int UTF32_length = 0;
  bool bEmpty = true;
  MYON_SHA1 sha1;

  for (;;)
  {
    const wchar_t* next = nullptr;
    int UTF32_count = MYON_ConvertWideCharToUTF32(
      false,
      name,
      (int)length,
      sUTF32,
      sUTF32_capacity,
      nullptr,
      0xFFFFFFFFU,
      0xFFFD,
      &next
      );

    size_t parsed_length
      = (nullptr != next && next > name)
      ? (next - name)
      : 0;

    if (parsed_length <= 0 || parsed_length > length )
      break;

    if (UTF32_count > 0)
    {
      UTF32_length += ((unsigned int)UTF32_count);

      if (bEmpty)
      {
        for (int i = 0; i < UTF32_count; i++)
        {
          if (0 != sUTF32[i])
          {
            bEmpty = false;
            break;
          }
        }
      }

      if (bIgnoreCase)
      {
        for (int i = 0; i < UTF32_count; i++)
        {
          if (MYON_IsUnicodeSpaceCodePoint(sUTF32[i]))
            sUTF32[i] = MYON_UnicodeCodePoint::MYON_Space;
          else
            sUTF32[i] = MYON_UnicodeMapCodePointOrdinal(MYON_StringMapOrdinalType::MinimumOrdinal, sUTF32[i]);
        }
      }
      else
      {
        for (int i = 0; i < UTF32_count; i++)
        {
          if (MYON_IsUnicodeSpaceCodePoint(sUTF32[i]))
            sUTF32[i] = MYON_UnicodeCodePoint::MYON_Space;
        }
      }

      const size_t sz = ((size_t)UTF32_count) * sizeof(sUTF32[0]);

      if (MYON::endian::big_endian == context_endian)
      {
        // swap bytes so the SHA-1 value is independent of endian byte order
		    unsigned char* b = (unsigned char*)sUTF32;
        unsigned char* b1 = b + sz;
		    unsigned char  c;
		    while(b < b1) 
        {
			    c = b[0]; b[0] = b[3]; b[3] = c;
			    c = b[1]; b[1] = b[2]; b[2] = c;
			    b += sizeof(sUTF32[0]);
		    }
      }

      sha1.AccumulateBytes(sUTF32,sz);
    }

    name += parsed_length;
    length -= parsed_length;
    if (0 == length)
      break;
  }

  if ( bEmpty )
    return MYON_NameHash::EmptyNameHash;

  hash.m_parent_id = name_parent_id;
  hash.m_flags = (UTF32_length & MYON_NameHash::flags_length_mask);
  if (false == bIgnoreCase)
    hash.m_flags |= MYON_NameHash::flags_case_sensitive;
  hash.m_sha1_hash = sha1.Hash();

  return hash;
}

MYON_NameHash MYON_NameHash::Create(
  const MYON_UUID& name_parent_id,
  const MYON_String& name
  )
{
  return MYON_NameHash::Create(name_parent_id,(size_t)name.Length(), name.Array());
}


MYON_NameHash MYON_NameHash::Create(
  const MYON_UUID& name_parent_id,
  const MYON_String& name,
  bool bIgnoreCase
  )
{
  return MYON_NameHash::Create(name_parent_id,(size_t)name.Length(), name.Array(), bIgnoreCase);
}

MYON_NameHash MYON_NameHash::Create(
  const MYON_UUID& name_parent_id,
  const char* name
  )
{
  return MYON_NameHash::Create(name_parent_id,MYON_String::Length(name), name);
}

MYON_NameHash MYON_NameHash::Create(
  const MYON_UUID& name_parent_id,
  const char* name,
  bool bIgnoreCase
  )
{
  return MYON_NameHash::Create(name_parent_id,MYON_String::Length(name), name, bIgnoreCase);
}


MYON_NameHash MYON_NameHash::Create(
  const MYON_UUID& name_parent_id,
  size_t length,
  const char* name
)
{
  const bool bIgnoreCase = true;
  return MYON_NameHash::Create( name_parent_id, length, name, bIgnoreCase);
}

MYON_NameHash MYON_NameHash::Create(
  const MYON_UUID& name_parent_id,
  size_t length, 
  const char* name,
  bool bIgnoreCase
  )
{
  const MYON::endian context_endian = MYON::Endian();

  MYON_NameHash hash;
  if (length <= 0 || nullptr == name)
  {
    // Do not return MYON_NameHash::EmptyNameHash because
    // this code is used to initialize that static member.
    hash.m_sha1_hash = MYON_SHA1_Hash::EmptyContentHash;
    return hash;
  }

  MYON__UINT32 sUTF32[64];
  const size_t sUTF32_capacity = sizeof(sUTF32)/sizeof(sUTF32[0]);
  unsigned int UTF32_length = 0;
  bool bEmpty = true;
  MYON_SHA1 sha1;

  for (;;)
  {
    const char* next = nullptr;
    int UTF32_count = MYON_ConvertUTF8ToUTF32(
      false,
      name,
      (int)length,
      sUTF32,
      sUTF32_capacity,
      nullptr,
      0xFFFFFFFFU,
      0xFFFD,
      &next
      );

    size_t parsed_length
      = (nullptr != next && next > name)
      ? (next - name)
      : 0;

    if (parsed_length <= 0 || parsed_length > length )
      break;

    if (UTF32_count > 0)
    {
      UTF32_length += ((unsigned int)UTF32_count);

      if (bEmpty)
      {
        for (int i = 0; i < UTF32_count; i++)
        {
          if (0 != sUTF32[i])
          {
            bEmpty = false;
            break;
          }
        }
      }

      if (bIgnoreCase)
      {
        for (int i = 0; i < UTF32_count; i++)
        {
          sUTF32[i] = MYON_UnicodeMapCodePointOrdinal(MYON_StringMapOrdinalType::MinimumOrdinal, sUTF32[i]);
        }
      }

      if (MYON::endian::big_endian == context_endian)
      {
        // swap bytes so the SHA-1 value is independent of endian byte order
		    unsigned char* b = (unsigned char*)sUTF32;
        unsigned char* b1 = b + UTF32_length*sizeof(sUTF32[0]);
		    unsigned char  c;
		    while(b < b1) 
        {
			    c = b[0]; b[0] = b[3]; b[3] = c;
			    c = b[1]; b[1] = b[2]; b[2] = c;
			    b += sizeof(sUTF32[0]);
		    }
      }

      sha1.AccumulateBytes(sUTF32,UTF32_length*sizeof(sUTF32[0]));
    }

    name += parsed_length;
    length -= parsed_length;
    if (0 == length)
      break;
  }

  if ( bEmpty )
    return MYON_NameHash::EmptyNameHash;

  hash.m_parent_id = name_parent_id;
  hash.m_flags = (UTF32_length & MYON_NameHash::flags_length_mask);
  if (false == bIgnoreCase)
    hash.m_flags |= MYON_NameHash::flags_case_sensitive;
  hash.m_sha1_hash = sha1.Hash();

  return hash;
}

MYON_NameHash MYON_NameHash::CreateIdAndEmptyName(
  const MYON_UUID& parent_id
  )
{
  MYON_NameHash hash;
  hash.m_sha1_hash = MYON_SHA1_Hash::EmptyContentHash;
  hash.m_parent_id = parent_id;
  return hash;
}

MYON_NameHash MYON_NameHash::CreateIdAndUnsetName(
  const MYON_UUID& parent_id
  )
{
  MYON_NameHash hash;
  hash.m_sha1_hash = MYON_SHA1_Hash::ZeroDigest;
  hash.m_parent_id = parent_id;
  return hash;
}

MYON_NameHash MYON_NameHash::Internal_DotNetInterfaceSet(
  const MYON_UUID& parent_id,
  const MYON_SHA1_Hash& sha1_hash,
  const MYON__UINT32 flags
)
{
  // The parent_id, sha1_hash and flags values must be exact 
  // copies of values saved from a valid instance of 
  // a MYON_NameHash instantiation.
  // Any other use will likely result in the creation of an invalid MYON_NameHash class.
  MYON_NameHash hash;
  hash.m_flags = flags;
  hash.m_sha1_hash = sha1_hash;
  hash.m_parent_id = parent_id;
  return hash;
}

MYON__UINT32 MYON_NameHash::Internal_DotNetInterfaceGetFlags() const
{
  return m_flags;
}

class MYON_ComponentManifestItem_PRIVATE : public MYON_ComponentManifestItem
{
public:
  MYON_ComponentManifestItem_PRIVATE() = default;
  ~MYON_ComponentManifestItem_PRIVATE() = default;
  MYON_ComponentManifestItem_PRIVATE(const MYON_ComponentManifestItem_PRIVATE&) = default;
  MYON_ComponentManifestItem_PRIVATE& operator=(const MYON_ComponentManifestItem_PRIVATE&) = default;

  MYON_ComponentManifestItem_PRIVATE(
    const MYON_ComponentManifestItem& item
    )
    : MYON_ComponentManifestItem(item)
  {}

public:
  unsigned int m_manifest_table_sn = 0;
  unsigned int m_manifest_impl_sn = 0;
  MYON_ComponentManifestItem_PRIVATE* m_next = nullptr;
  MYON_ComponentManifestItem_PRIVATE* m_prev = nullptr;
};

class MYON_ComponentManifestHash32TableItem : public MYON_Hash32TableItem
{
public:
  const MYON_ComponentManifestItem_PRIVATE* m_manifest_item = nullptr;
};

class MYON_ManifestHash32TableItemFixedSizePool : private MYON_FixedSizePool
{
public:
  MYON_ManifestHash32TableItemFixedSizePool() = default;
  ~MYON_ManifestHash32TableItemFixedSizePool() = default;
private:
  MYON_ManifestHash32TableItemFixedSizePool(const MYON_ManifestHash32TableItemFixedSizePool&) = delete;
  MYON_ManifestHash32TableItemFixedSizePool& operator=(const MYON_ManifestHash32TableItemFixedSizePool&) = delete;

public:
  class MYON_ComponentManifestHash32TableItem* AllocateHashTableItem(const MYON_ComponentManifestItem_PRIVATE* manifest_item)
  {
    if (0 == this->SizeofElement())
      Create(sizeof(MYON_ComponentManifestHash32TableItem),0,0);
    // Must use AllocateElement() instead of AllocateDirtyElement() because the actual item 
    // may be derived from MYON_ComponentManifestHash32TableItem.
    MYON_ComponentManifestHash32TableItem* hash_table_item = (MYON_ComponentManifestHash32TableItem*)AllocateElement();
    hash_table_item->m_manifest_item = manifest_item;
    return hash_table_item;
  }

  void ReturnHashTableItem(
  class MYON_ComponentManifestHash32TableItem* hash_table_item
    )
  {
    if (nullptr != hash_table_item)
    {
      hash_table_item->m_manifest_item = nullptr;
      ReturnElement(hash_table_item);
    }
  }
};

class MYON_ComponentNameHash32Table : private MYON_Hash32Table
{
public:
  MYON_ComponentNameHash32Table(
    MYON_ManifestHash32TableItemFixedSizePool& fsp
    )
    : m_fsp(fsp)
  {}

  ~MYON_ComponentNameHash32Table() = default;

private:
  MYON_ComponentNameHash32Table() = delete;
  MYON_ComponentNameHash32Table(const MYON_ComponentNameHash32Table&) = delete;
  MYON_ComponentNameHash32Table& operator=(const MYON_ComponentNameHash32Table&) = delete;

private:
  MYON_ManifestHash32TableItemFixedSizePool& m_fsp;

  static MYON__UINT32 Hash32(
    MYON_ModelComponent::Type component_type,
    const MYON_NameHash& name_hash
    )
  {
    // Using the static_cast<unsigned int>(component_type) as the seed 
    // is done so objects of different types with the same name will
    // generally have a different 32-bit hash.
    return name_hash.DataCRC(static_cast<unsigned int>(component_type));
  }

  static MYON__UINT32 Hash32(
    const MYON_ComponentManifestItem& manifest_item
    )
  {
    return Hash32(manifest_item.ComponentType(),manifest_item.NameHash());
  }

public:
  const MYON_ComponentManifestHash32TableItem* AddManifestItem(
    const class MYON_ComponentManifestItem_PRIVATE* manifest_item
    )
  {
    if (nullptr != manifest_item && manifest_item->NameHash().IsValidAndNotEmpty() )
    {
      MYON_ComponentManifestHash32TableItem* hash_table_item = m_fsp.AllocateHashTableItem(manifest_item);
      MYON__UINT32 hash32 = MYON_ComponentNameHash32Table::Hash32(*manifest_item);
      if ( AddItem( hash32, hash_table_item ) )
        return hash_table_item;
      m_fsp.ReturnHashTableItem(hash_table_item);
    }
    return nullptr;
  }

  bool RemoveManifestItem(
    MYON__UINT32 hash32,
    const class MYON_ComponentManifestItem_PRIVATE* manifest_item
    )
  {
    if ( nullptr != manifest_item )
    {
      for (const MYON_Hash32TableItem* hash_table_item = FirstItemWithHash(hash32); nullptr != hash_table_item; hash_table_item = NextItemWithHash(hash_table_item))
      {
        if (manifest_item == ((const MYON_ComponentManifestHash32TableItem*)hash_table_item)->m_manifest_item)
        {
          if (RemoveItem(const_cast<MYON_Hash32TableItem*>(hash_table_item)))
          {
            m_fsp.ReturnHashTableItem((MYON_ComponentManifestHash32TableItem*)hash_table_item);
            return true;
          }
        }
      }
    }
    return false;
  }

  bool RemoveManifestItem(
    const class MYON_ComponentManifestItem_PRIVATE* manifest_item
    )
  {
    if ( nullptr != manifest_item && manifest_item->NameHash().IsValidAndNotEmpty() )
      return RemoveManifestItem(Hash32(*manifest_item),manifest_item);
    return false;
  }

  const MYON_ComponentManifestHash32TableItem* FirstManifestItemWithName(
    MYON_ModelComponent::Type component_type,
    const MYON_NameHash& name_hash
    ) const
  {
    if (false == name_hash.IsValidAndNotEmpty())
      return nullptr;
    const MYON__UINT32 hash32 = MYON_ComponentNameHash32Table::Hash32(component_type,name_hash);
    for (const MYON_Hash32TableItem* hash_table_item = FirstItemWithHash(hash32); nullptr != hash_table_item; hash_table_item = NextItemWithHash(hash_table_item))
    {
      const MYON_ComponentManifestItem_PRIVATE* manifest_item = static_cast<const MYON_ComponentManifestHash32TableItem*>(hash_table_item)->m_manifest_item;
      if (component_type == manifest_item->ComponentType() && name_hash == manifest_item->NameHash() )
        return (const MYON_ComponentManifestHash32TableItem*)hash_table_item;
    }
    return nullptr;
  }

  bool NameInUse(
    MYON_ModelComponent::Type component_type,
    const MYON_NameHash& name_hash,
    bool bIgnoreParentId
    ) const
  {
    // Do not test "system" component names.
    // They can generally be reused by model components.
    if (false == name_hash.IsValidAndNotEmpty())
      return false;

    if (false == bIgnoreParentId || false == MYON_ModelComponent::UniqueNameIncludesParent(component_type))
      return (nullptr != FirstManifestItemWithName(component_type,name_hash));

    // slow search when parent id must be ignored (layer names for example)
    for (const MYON_Hash32TableItem* hash_table_item = FirstTableItem(); nullptr != hash_table_item; hash_table_item = NextTableItem(hash_table_item))
    {
      const MYON_ComponentManifestItem_PRIVATE* manifest_item = static_cast<const MYON_ComponentManifestHash32TableItem*>(hash_table_item)->m_manifest_item;
      if (component_type == manifest_item->ComponentType() && MYON_NameHash::CompareNameSHA1(name_hash,manifest_item->NameHash()) )
        return true;
    }
    return false;
  }

  const MYON_ComponentManifestHash32TableItem* NextManifestItemWithName(
    const MYON_ComponentManifestHash32TableItem* current_manifest_item
    ) const
  {
    if ( nullptr == current_manifest_item || nullptr == current_manifest_item->m_manifest_item )
      return nullptr;
    const MYON_NameHash name_hash = current_manifest_item->m_manifest_item->NameHash();
    if ( false == name_hash.IsValidAndNotEmpty() )
      return nullptr;
    const MYON_ModelComponent::Type component_type = current_manifest_item->m_manifest_item->ComponentType();
    for (const MYON_Hash32TableItem* hash_table_item = NextItemWithHash(current_manifest_item); nullptr != hash_table_item; hash_table_item = NextItemWithHash(hash_table_item))
    {
      if ( component_type == ((const MYON_ComponentManifestHash32TableItem*)hash_table_item)->m_manifest_item->ComponentType()
        && name_hash == ((const MYON_ComponentManifestHash32TableItem*)hash_table_item)->m_manifest_item->NameHash() 
        )
        return (const MYON_ComponentManifestHash32TableItem*)hash_table_item;
    }
    return nullptr;
  }
};


class MYON_ComponentIdHash32Table : private MYON_Hash32Table
{
public:
  MYON_ComponentIdHash32Table(
    MYON_ManifestHash32TableItemFixedSizePool& fsp
    )
    : m_fsp(fsp)
  {}

  ~MYON_ComponentIdHash32Table() = default;

private:
  MYON_ComponentIdHash32Table() = delete;
  MYON_ComponentIdHash32Table(const MYON_ComponentIdHash32Table&) = delete;
  MYON_ComponentIdHash32Table& operator=(const MYON_ComponentIdHash32Table&) = delete;

private:
  static MYON_UUID IdFromManifestItem(
    const class MYON_ComponentManifestItem* manifest_item
    ) 
  {
    return (nullptr != manifest_item) ? manifest_item->Id() : MYON_nil_uuid;
  }

private:
  static MYON__UINT32 IdHash32(
    const MYON_UUID& id
    )
  {
    return MYON_CRC32(0, sizeof(MYON_UUID), &id);
  }

  MYON_ManifestHash32TableItemFixedSizePool& m_fsp;

public:
  unsigned int ItemCount() const
  {
    return MYON_Hash32Table::ItemCount();
  }

  const MYON_ComponentManifestHash32TableItem* AddManifestItem(
    const class MYON_ComponentManifestItem_PRIVATE* manifest_item
    )
  {
    const MYON_UUID id = IdFromManifestItem(manifest_item);
    if ( MYON_nil_uuid == id )
      return nullptr;
    const MYON__UINT32 hash32 = MYON_ComponentIdHash32Table::IdHash32(id);
    
    MYON_ComponentManifestHash32TableItem* hash_table_item = m_fsp.AllocateHashTableItem(manifest_item);
    if ( AddItem( hash32, hash_table_item ) )
      return hash_table_item;

    m_fsp.ReturnHashTableItem(hash_table_item);
    return nullptr;
  }

  bool RemoveManifestItem(
    const class MYON_ComponentManifestItem_PRIVATE* manifest_item
    )
  {
    const MYON_UUID id = IdFromManifestItem(manifest_item);
    if ( MYON_nil_uuid == id )
      return false;
    const MYON__UINT32 hash32 = MYON_ComponentIdHash32Table::IdHash32(id);

    for (const MYON_Hash32TableItem* hash_table_item = FirstItemWithHash(hash32); nullptr != hash_table_item; hash_table_item = NextItemWithHash(hash_table_item))
    {
      if (manifest_item == ((const MYON_ComponentManifestHash32TableItem*)hash_table_item)->m_manifest_item)
      {
        if (RemoveItem(const_cast<MYON_Hash32TableItem*>(hash_table_item)))
        {
          m_fsp.ReturnHashTableItem((MYON_ComponentManifestHash32TableItem*)hash_table_item);
          return true;
        }
      }
    }

    return false;
  }

  const MYON_ComponentManifestHash32TableItem* FirstManifestItemWithId(
    MYON_UUID id
    ) const
  {
    if (MYON_nil_uuid == id)
      return nullptr;
    const MYON__UINT32 hash32 = MYON_ComponentIdHash32Table::IdHash32(id);
    for (const MYON_Hash32TableItem* hash_table_item = FirstItemWithHash(hash32); nullptr != hash_table_item; hash_table_item = NextItemWithHash(hash_table_item))
    {
      if (id == IdFromManifestItem(((const MYON_ComponentManifestHash32TableItem*)hash_table_item)->m_manifest_item) )
        return (const MYON_ComponentManifestHash32TableItem*)hash_table_item;
    }
    return nullptr;
  }

  const MYON_ComponentManifestHash32TableItem* NextManifestItemWithId(
    const MYON_ComponentManifestHash32TableItem* current_manifest_item
    ) const
  {
    if ( nullptr == current_manifest_item)
      return nullptr;
    const MYON_UUID id = IdFromManifestItem(current_manifest_item->m_manifest_item);
    if ( MYON_nil_uuid == id )
      return nullptr;
    for (const MYON_Hash32TableItem* hash_table_item = NextItemWithHash(current_manifest_item); nullptr != hash_table_item; hash_table_item = NextItemWithHash(hash_table_item))
    {
      if (id == IdFromManifestItem(((const MYON_ComponentManifestHash32TableItem*)hash_table_item)->m_manifest_item) )
        return (const MYON_ComponentManifestHash32TableItem*)hash_table_item;
    }
    return nullptr;
  }
};

//////////////////////////////////////////////////////////////////////////////////////
//
// MYON_ManifestMap hash table tools
//

bool operator==(const MYON_ManifestMapItem& lhs, const MYON_ManifestMapItem& rhs)
{
  return (0 == MYON_ManifestMapItem::Compare(lhs, rhs));
}

bool operator!=(const MYON_ManifestMapItem& lhs, const MYON_ManifestMapItem& rhs)
{
  return (0 != MYON_ManifestMapItem::Compare(lhs, rhs));
}

int MYON_ManifestMapItem::Compare(
  const MYON_ManifestMapItem& a,
  const MYON_ManifestMapItem& b)
{
  int rc = MYON_ManifestMapItem::CompareTypeAndSourceIndex(a, b);
  if (0 != rc)
    return rc;
  rc = MYON_ManifestMapItem::CompareTypeAndDestinationIndex(a, b);
  if (0 != rc)
    return rc;
  rc = MYON_UuidCompare(a.m_source_id, b.m_source_id);
  if (0 != rc)
    return rc;
  return MYON_UuidCompare(a.m_destination_id, b.m_destination_id);
}


int MYON_ManifestMapItem::CompareTypeAndSourceId(
  const MYON_ManifestMapItem& a,
  const MYON_ManifestMapItem& b
  )
{
  const unsigned int at = static_cast<unsigned int>(a.m_component_type);
  const unsigned int bt = static_cast<unsigned int>(b.m_component_type);
  if (at < bt)
    return -1;
  if (at > bt)
    return 1;
  return MYON_UuidCompare(a.m_source_id, b.m_source_id);
}

int MYON_ManifestMapItem::CompareTypeAndDestinationId(
  const MYON_ManifestMapItem& a,
  const MYON_ManifestMapItem& b
  )
{
  const unsigned int at = static_cast<unsigned int>(a.m_component_type);
  const unsigned int bt = static_cast<unsigned int>(b.m_component_type);
  if (at < bt)
    return -1;
  if (at > bt)
    return 1;
  return MYON_UuidCompare(a.m_destination_id, b.m_destination_id);
}

int MYON_ManifestMapItem::CompareTypeAndSourceIdAndIndex(
  const MYON_ManifestMapItem& a,
  const MYON_ManifestMapItem& b
  )
{
  const unsigned int at = static_cast<unsigned int>(a.m_component_type);
  const unsigned int bt = static_cast<unsigned int>(b.m_component_type);
  if (at < bt)
    return -1;
  if (at > bt)
    return 1;
  if (a.m_source_index < b.m_source_index)
    return -1;
  if (a.m_source_index > b.m_source_index)
    return 1;
  return MYON_UuidCompare(a.m_source_id, b.m_source_id);
}

int MYON_ManifestMapItem::CompareTypeAndDestinationIdAndIndex(
  const MYON_ManifestMapItem& a,
  const MYON_ManifestMapItem& b
  )
{
  const unsigned int at = static_cast<unsigned int>(a.m_component_type);
  const unsigned int bt = static_cast<unsigned int>(b.m_component_type);
  if (at < bt)
    return -1;
  if (at > bt)
    return 1;
  if (a.m_destination_index < b.m_destination_index)
    return -1;
  if (a.m_destination_index > b.m_destination_index)
    return 1;
  return MYON_UuidCompare(a.m_destination_id, b.m_destination_id);
}


int MYON_ManifestMapItem::CompareTypeAndSourceIndex(
  const MYON_ManifestMapItem& a,
  const MYON_ManifestMapItem& b)
{
  const unsigned int at = static_cast<unsigned int>(a.m_component_type);
  const unsigned int bt = static_cast<unsigned int>(b.m_component_type);
  if (at < bt)
    return -1;
  if (at > bt)
    return 1;
  if (a.m_source_index < b.m_source_index)
    return -1;
  if (a.m_source_index > b.m_source_index)
    return 1;
  return 0;
}

int MYON_ManifestMapItem::CompareTypeAndDestinationIndex(
  const MYON_ManifestMapItem& a,
  const MYON_ManifestMapItem& b)
{
  const unsigned int at = static_cast<unsigned int>(a.m_component_type);
  const unsigned int bt = static_cast<unsigned int>(b.m_component_type);
  if (at < bt)
    return -1;
  if (at > bt)
    return 1;
  if (a.m_destination_index < b.m_destination_index)
    return -1;
  if (a.m_destination_index > b.m_destination_index)
    return 1;
  return 0;
}

MYON__UINT32 MYON_ManifestMapItem::SourceIdHash32(
  const MYON_UUID& source_component_id
  )
{
  return MYON_CRC32(0, sizeof(MYON_UUID), &source_component_id);
}

MYON__UINT32 MYON_ManifestMapItem::SourceIndexHash32(
  MYON_ModelComponent::Type component_type,
  int source_component_index
  )
{
  MYON__UINT32 hash32 = MYON_CRC32(0, sizeof(component_type), &component_type);
  hash32 = MYON_CRC32(hash32, sizeof(source_component_index), &source_component_index);
  return hash32;
}

bool MYON_ManifestMapItem::SourceIsSet() const
{
  if (MYON_ModelComponent::Type::Unset == m_component_type)
    return false;
  if (MYON_nil_uuid == m_source_id)
    return false;
  if (MYON_ModelComponent::IndexRequired(m_component_type) && MYON_UNSET_INT_INDEX == m_source_index)
    return false;
  return true;
}

bool MYON_ManifestMapItem::SourceIsUnset() const
{
  return SourceIsSet() ? false : true;
}

bool MYON_ManifestMapItem::DestinationIsSet() const
{
  if (MYON_ModelComponent::Type::Unset == m_component_type)
    return false;
  if (MYON_nil_uuid == m_destination_id)
    return false;
  if (MYON_ModelComponent::IndexRequired(m_component_type) && MYON_UNSET_INT_INDEX == m_destination_index)
    return false;
  return true;
}

bool MYON_ManifestMapItem::DestinationIsUnset() const
{
  return DestinationIsSet() ? false : true;
}

bool MYON_ManifestMapItem::SourceAndDestinationAreSet() const
{
  if (MYON_ModelComponent::Type::Unset == m_component_type)
    return false;
  if (MYON_nil_uuid == m_source_id)
    return false;
  if (MYON_nil_uuid == m_destination_id)
    return false;
  if (MYON_ModelComponent::IndexRequired(m_component_type))
  {
    if (MYON_UNSET_INT_INDEX == m_source_index)
      return false;
    if (MYON_UNSET_INT_INDEX == m_destination_index)
      return false;
  }
  return true;
}

bool MYON_ManifestMapItem::SourceOrDestinationIsUnset() const
{
  return SourceAndDestinationAreSet() ? false : true;
}

class MYON_ManifestMap_Hash32TableItem : public MYON_Hash32TableItem
{
public:
  MYON_ManifestMap_Hash32TableItem() = default;
  ~MYON_ManifestMap_Hash32TableItem() = default;
  MYON_ManifestMap_Hash32TableItem(const MYON_ManifestMap_Hash32TableItem&) = default;
  MYON_ManifestMap_Hash32TableItem& operator=(const MYON_ManifestMap_Hash32TableItem&) = default;

public:
  MYON_ManifestMapItem m_map_item;
};

/*
MYON_ManifestMap_Hash32TableItemFixedSizePool is used to allocate hash table items for 
MYON_ManifestMap_Hash32Table,
MYON_ManifestMap_SourceIdHash32Table, and MYON_ManifestMap_SourceIndexHash32Table
*/
class MYON_ManifestMap_Hash32TableItemFixedSizePool : public MYON_FixedSizePool
{
public:
  MYON_ManifestMap_Hash32TableItemFixedSizePool() = default;
  ~MYON_ManifestMap_Hash32TableItemFixedSizePool() = default;
private:
  MYON_ManifestMap_Hash32TableItemFixedSizePool(const MYON_ManifestMap_Hash32TableItemFixedSizePool&) = delete;
  MYON_ManifestMap_Hash32TableItemFixedSizePool& operator=(const MYON_ManifestMap_Hash32TableItemFixedSizePool&) = delete;

public:
  class MYON_ManifestMap_Hash32TableItem* AllocateHashTableItem(
    const MYON_ManifestMapItem& map_item
    )
  {
    if (0 == this->SizeofElement())
      Create(sizeof(MYON_ManifestMap_Hash32TableItem),0,0);
    // Must use AllocateElement() instead of AllocateDirtyElement() because the actual item 
    // may be derived from MYON_ComponentManifestHash32TableItem.
    MYON_ManifestMap_Hash32TableItem* hash_table_item = (MYON_ManifestMap_Hash32TableItem*)AllocateElement();
    hash_table_item->m_map_item = map_item;
    return hash_table_item;
  }

  void ReturnHashTableItem(
    class MYON_ManifestMap_Hash32TableItem* hash_table_item
    )
  {
    if (nullptr != hash_table_item)
    {
      hash_table_item->m_map_item = MYON_ManifestMapItem::Unset;
      ReturnElement(hash_table_item);
    }
  }
};


/*
MYON_ManifestMap_Hash32Table is a pure virtual base class for
MYON_ManifestMap_SourceIdHash32Table and MYON_ManifestMap_SourceIndexHash32Table
*/
class MYON_ManifestMap_Hash32Table : public MYON_Hash32Table
{
protected:
  MYON_ManifestMap_Hash32Table(MYON_ManifestMap_Hash32TableItemFixedSizePool& fsp)
    :m_fsp(fsp)
  {}
  
  virtual ~MYON_ManifestMap_Hash32Table() {} // = default;

private:
  MYON_ManifestMap_Hash32Table() = delete;
  MYON_ManifestMap_Hash32Table(const MYON_ManifestMap_Hash32Table&) = delete;
  MYON_ManifestMap_Hash32Table& operator=(const MYON_ManifestMap_Hash32Table&) = delete;

protected:
  virtual MYON__UINT32 MapItemHash32(
    const class MYON_ManifestMapItem& map_item
    ) const = 0;

public:
  const MYON_ManifestMap_Hash32TableItem* AddManifestMapItem(
    const MYON_ManifestMapItem& map_item
    )
  {
    MYON_ManifestMap_Hash32TableItem* hash_table_item = m_fsp.AllocateHashTableItem(map_item);
    const MYON__UINT32 hash32 = MapItemHash32(hash_table_item->m_map_item);
    if (AddItem(hash32, hash_table_item))
      return hash_table_item;

    m_fsp.ReturnHashTableItem(hash_table_item);
    return nullptr;
  }

private:
  MYON_ManifestMap_Hash32TableItemFixedSizePool& m_fsp;
};

class MYON_ManifestMap_SourceIdHash32Table : public MYON_ManifestMap_Hash32Table
{
public:
  MYON_ManifestMap_SourceIdHash32Table(MYON_ManifestMap_Hash32TableItemFixedSizePool& fsp)
    :MYON_ManifestMap_Hash32Table(fsp)
  {}
  ~MYON_ManifestMap_SourceIdHash32Table() = default;

private:
  MYON_ManifestMap_SourceIdHash32Table() = delete;
  MYON_ManifestMap_SourceIdHash32Table(const MYON_ManifestMap_SourceIdHash32Table&) = delete;
  MYON_ManifestMap_SourceIdHash32Table& operator=(const MYON_ManifestMap_SourceIdHash32Table&) = delete;

protected:
    MYON__UINT32 MapItemHash32(
      const class MYON_ManifestMapItem& map_item
      ) const override
    {
      return MYON_ManifestMapItem::SourceIdHash32(map_item.SourceId());
    }

public:
  const MYON_ManifestMap_Hash32TableItem* FirstManifestMapItemWithSourceId(
    MYON_UUID source_component_id
    ) const
  {
    if (MYON_nil_uuid == source_component_id)
      return nullptr;
    const MYON__UINT32 hash32 = MYON_ManifestMapItem::SourceIdHash32(source_component_id);
    for (const MYON_Hash32TableItem* hash_table_item = FirstItemWithHash(hash32); nullptr != hash_table_item; hash_table_item = NextItemWithHash(hash_table_item))
    {
      if (source_component_id == ((const MYON_ManifestMap_Hash32TableItem*)hash_table_item)->m_map_item.SourceId() )
        return (const MYON_ManifestMap_Hash32TableItem*)hash_table_item;
    }
    return nullptr;
  }

  const MYON_ManifestMap_Hash32TableItem* NextManifestMapItemWithSourceId(
    const MYON_ManifestMap_Hash32TableItem* current_manifest_item
    ) const
  {
    if ( nullptr == current_manifest_item)
      return nullptr;
    const MYON_UUID source_component_id = current_manifest_item->m_map_item.SourceId();
    if ( MYON_nil_uuid == source_component_id )
      return nullptr;
    for (const MYON_Hash32TableItem* hash_table_item = NextItemWithHash(current_manifest_item); nullptr != hash_table_item; hash_table_item = NextItemWithHash(hash_table_item))
    {
      if (source_component_id == ((const MYON_ManifestMap_Hash32TableItem*)hash_table_item)->m_map_item.SourceId() )
        return (const MYON_ManifestMap_Hash32TableItem*)hash_table_item;
    }
    return nullptr;
  }
};


class MYON_ManifestMap_SourceIndexHash32Table : public MYON_ManifestMap_Hash32Table
{
public:
  MYON_ManifestMap_SourceIndexHash32Table(MYON_ManifestMap_Hash32TableItemFixedSizePool& fsp)
    :MYON_ManifestMap_Hash32Table(fsp)
  {}
  virtual ~MYON_ManifestMap_SourceIndexHash32Table() {} // = default;

private:
  MYON_ManifestMap_SourceIndexHash32Table() = delete;
  MYON_ManifestMap_SourceIndexHash32Table(const MYON_ManifestMap_SourceIndexHash32Table&) = delete;
  MYON_ManifestMap_SourceIndexHash32Table& operator=(const MYON_ManifestMap_SourceIndexHash32Table&) = delete;

protected:
    MYON__UINT32 MapItemHash32(
      const class MYON_ManifestMapItem& map_item
    ) const override
    {
      return MYON_ManifestMapItem::SourceIndexHash32(map_item.ComponentType(),map_item.SourceIndex());
    }

public:
  const MYON_ManifestMap_Hash32TableItem* FirstManifestMapItemWithSourceIndex(
    MYON_ModelComponent::Type component_type,
    int source_component_index
    ) const
  {
    if (MYON_ModelComponent::Type::Unset == component_type)
      return nullptr;
    const MYON__UINT32 hash32 = MYON_ManifestMapItem::SourceIndexHash32(component_type,source_component_index);
    for ( const MYON_Hash32TableItem* hash_table_item = FirstItemWithHash(hash32); nullptr != hash_table_item; hash_table_item = NextItemWithHash(hash_table_item) )
    {
      const MYON_ManifestMapItem& map_item = ((const MYON_ManifestMap_Hash32TableItem*)hash_table_item)->m_map_item;
      if (component_type == map_item.ComponentType() && source_component_index == map_item.SourceIndex() )
        return (const MYON_ManifestMap_Hash32TableItem*)hash_table_item;
    }
    return nullptr;
  }

  const MYON_ManifestMap_Hash32TableItem* NextManifestMapItemWithSourceIndex(
    const MYON_ManifestMap_Hash32TableItem* current_manifest_item
    ) const
  {
    if ( nullptr == current_manifest_item)
      return nullptr;
    const MYON_ModelComponent::Type component_type = current_manifest_item->m_map_item.ComponentType();
    const int source_component_index = current_manifest_item->m_map_item.SourceIndex();
    if ( MYON_ModelComponent::Type::Unset == component_type || MYON_UNSET_INT_INDEX == source_component_index)
      return nullptr;
    for (const MYON_Hash32TableItem* hash_table_item = NextItemWithHash(current_manifest_item); nullptr != hash_table_item; hash_table_item = NextItemWithHash(hash_table_item))
    {
      const MYON_ManifestMapItem& map_item = ((const MYON_ManifestMap_Hash32TableItem*)hash_table_item)->m_map_item;
      if (component_type == map_item.ComponentType() && source_component_index == map_item.SourceIndex() )
        return (const MYON_ManifestMap_Hash32TableItem*)hash_table_item;
    }
    return nullptr;
  }
};


class MYON_ManifestMapImpl
{
public:
  MYON_ManifestMapImpl()
    : m_source_id_hash_table(m_fsp)
    , m_source_index_hash_table(m_fsp)
  {}

  ~MYON_ManifestMapImpl() = default;

  MYON_ManifestMapImpl(const MYON_ManifestMapImpl& src)
    : m_source_id_hash_table(m_fsp)
    , m_source_index_hash_table(m_fsp)
  {
    Internal_Copy(src);
  }

  MYON_ManifestMapImpl& operator=(const MYON_ManifestMapImpl&);

  unsigned int MapItemCountImpl() const
  {
    return m_source_id_hash_table.ItemCount();
  }

private:
  // Allocates MYON_ManifestIdentificationMapItem classes
  MYON_ManifestMap_Hash32TableItemFixedSizePool m_fsp;

public:
  // Every item has a m_source_id_hash_table.
  MYON_ManifestMap_SourceIdHash32Table m_source_id_hash_table;

  // In addition, some of the items in m_source_id_hash_table have a corresponding
  // m_source_index_hash_table entry.
  MYON_ManifestMap_SourceIndexHash32Table m_source_index_hash_table;

private:
  void Internal_Copy(const MYON_ManifestMapImpl& src);
  void Internal_Destroy();
};

void MYON_ManifestMapImpl::Internal_Copy(const MYON_ManifestMapImpl& src)
{
  if (src.m_source_id_hash_table.ItemCount() > 0)
  {
    const MYON__UINT32 src_hash_table_sn = src.m_source_id_hash_table.HashTableSerialNumber();
    MYON_FixedSizePoolIterator fit(src.m_fsp);
    for (const MYON_ManifestMap_Hash32TableItem* src_item = static_cast<const MYON_ManifestMap_Hash32TableItem*>(fit.FirstElement());
      nullptr != src_item;
      src_item = static_cast<const MYON_ManifestMap_Hash32TableItem*>(fit.NextElement())
      )
    {
      if (src_hash_table_sn != src_item->HashTableSerialNumber())
        continue; // item not active
      const MYON_ManifestMapItem& map_item = src_item->m_map_item;
      if (MYON_ModelComponent::Type::Unset == map_item.ComponentType())
      {
        MYON_ERROR("Invalid map_item in src.m_source_id_hash_table.");
        continue;
      }
      m_source_id_hash_table.AddManifestMapItem(map_item);
      if ( MYON_UNSET_INT_INDEX != map_item.SourceIndex() )
        m_source_index_hash_table.AddManifestMapItem(map_item);
    }
  }
}
void MYON_ManifestMapImpl::Internal_Destroy()
{
  m_source_index_hash_table.RemoveAllItems();
  m_source_id_hash_table.RemoveAllItems();
  m_fsp.ReturnAll();
}


MYON_ManifestMapImpl& MYON_ManifestMapImpl::operator=(const MYON_ManifestMapImpl& src)
{
  if (this != &src)
  {
    Internal_Destroy();
    Internal_Copy(src);
  }
  return *this;
}

MYON_ManifestMapImpl* MYON_ManifestMap::Impl()
{
  if (nullptr == m_impl)
    m_impl = new MYON_ManifestMapImpl();
  return m_impl;
}

MYON_ManifestMap::MYON_ManifestMap() MYON_NOEXCEPT
{
  // explicity implementation to work around a bug in Apple's CLANG
}

MYON_ManifestMap::~MYON_ManifestMap()
{
  if (nullptr != m_impl)
  {
    delete m_impl;
    m_impl = nullptr;
  }
}

bool MYON_ManifestMap::UpdatetMapItemDestination(
  const class MYON_ManifestMapItem& map_item
)
{
  const bool bIgnoreSourceIndex = false;
  return UpdatetMapItemDestination(map_item, bIgnoreSourceIndex);
}

bool MYON_ManifestMap::UpdatetMapItemDestination(
    const class MYON_ManifestMapItem& map_item,
    bool bIgnoreSourceIndex
    )
{
  if (map_item.SourceIsUnset())
  {
    MYON_ERROR("map_item source is not set.");
    return false;
  }
  
  if (MYON_nil_uuid == map_item.DestinationId())
  {
    // empty destination 
    if (MYON_UNSET_INT_INDEX != map_item.DestinationIndex())
    {
      MYON_ERROR("map_item destination index or id is incorrectly set.");
      return false;
    }
  }
  else if (MYON_ModelComponent::IndexRequired(map_item.ComponentType()))
  {
    if (MYON_UNSET_INT_INDEX == map_item.DestinationIndex())
    {
      MYON_ERROR("map_item destination index or id is incorrectly set.");
      return false;
    }
  }

  // map_item_id = item in this manifest map for the SourceId().
  // map_item_id must be valid and have a matching type and id.
  const class MYON_ManifestMapItem& map_item_id = MapItemFromSourceId(map_item.SourceId());

  if (MYON_nil_uuid == map_item_id.SourceId())
  {
    MYON_ERROR("map_item.SourceId() is not in the MYON_ManifestMap as a source id.");
    return false;
  }
  if ( 0 != MYON_ManifestMapItem::CompareTypeAndSourceId(map_item_id,map_item) )
  {
    MYON_ERROR("map_item type is not equal to corresponding MYON_ManifestMap item source type.");
    return false;
  }
  if (
    false == bIgnoreSourceIndex
    && map_item_id.SourceIndex() != map_item.SourceIndex()
    )
  {
    MYON_ERROR("map_item source index is not equal to corresponding MYON_ManifestMap item source index.");
    return false;
  }

  const class MYON_ManifestMapItem& map_item_index
    = (MYON_UNSET_INT_INDEX != map_item_id.SourceIndex())
    ? MapItemFromSourceIndex(map_item_id.ComponentType(),map_item_id.SourceIndex())
    : map_item_id;
  if (
    map_item_index.ComponentType() != map_item.ComponentType()
    || map_item_index.SourceId() != map_item.SourceId()
    || (false == bIgnoreSourceIndex && map_item_index.SourceIndex() != map_item.SourceIndex())
    )
  {
    MYON_ERROR("map_item source settings are not equal to corresponding MYON_ManifestMap item source settings.");
    return false;
  }

  if ( map_item_id.SourceIsSet() )
    const_cast<MYON_ManifestMapItem&>(map_item_id).SetDestinationIdentification(&map_item);
  if ( map_item_index.SourceIsSet() && &map_item_index != &map_item_id )
    const_cast<MYON_ManifestMapItem&>(map_item_index).SetDestinationIdentification(&map_item);

  return true;
}

MYON_ManifestMap::MYON_ManifestMap(const MYON_ManifestMap& src)
{
  if (nullptr != src.m_impl)
    m_impl = new MYON_ManifestMapImpl(*src.m_impl);
}

MYON_ManifestMap& MYON_ManifestMap::operator=(const MYON_ManifestMap& src)
{
  if (m_impl != src.m_impl)
  {
    if (nullptr != m_impl)
    {
      delete m_impl;
      m_impl = nullptr;
    }
    if (nullptr != src.m_impl)
    {
      m_impl = new MYON_ManifestMapImpl(*src.m_impl);
    }
  }
  return *this;
}


bool MYON_ManifestMap::IsEmpty() const
{
  return (0 == MapItemCount());
}

bool MYON_ManifestMap::IsNotEmpty() const
{
  return (MapItemCount() > 0);
}

unsigned int MYON_ManifestMap::MapItemCount() const
{
  return ( (nullptr != m_impl) ? m_impl->MapItemCountImpl() : 0U );

}


bool MYON_ManifestMap::AddMapItem(
  const MYON_ManifestMapItem& map_item
  )
{
  if (MYON_ModelComponent::Type::Unset == map_item.ComponentType())
  {
    MYON_ERROR("map_item.ComponentType() must be set.");
    return false;
  }

  if (MYON_nil_uuid == map_item.SourceId())
  {
    MYON_ERROR("map_item.SourceId() must be set.");
    return false;
  }

  MYON_ManifestMapImpl* impl = Impl();
  const MYON_ManifestMap_Hash32TableItem* source_id_hash_table_item = impl->m_source_id_hash_table.AddManifestMapItem(map_item);
  if (nullptr == source_id_hash_table_item)
    return false;

  if (MYON_UNSET_INT_INDEX != source_id_hash_table_item->m_map_item.SourceIndex())
    impl->m_source_index_hash_table.AddManifestMapItem(source_id_hash_table_item->m_map_item);

  return true;
}

const class MYON_ManifestMapItem& MYON_ManifestMap::MapItemFromSourceId(
  const MYON_UUID& source_item_id
  ) const
{
  for (;;)
  {
    if ( MYON_nil_uuid == source_item_id )
      break;
    if (nullptr == m_impl)
      break;
    const MYON_ManifestMap_Hash32TableItem* hash_table_item = m_impl->m_source_id_hash_table.FirstManifestMapItemWithSourceId(source_item_id);
    if (nullptr == hash_table_item)
      break;
    return hash_table_item->m_map_item;
  }
  return MYON_ManifestMapItem::Unset;
}

const class MYON_ManifestMapItem& MYON_ManifestMap::MapItemFromSourceIndex(
  MYON_ModelComponent::Type component_type,
  int source_component_index
  ) const
{
  for (;;)
  {
    if ( MYON_ModelComponent::Type::Unset == component_type )
      break;
    if ( MYON_UNSET_INT_INDEX == source_component_index )
      break;
    if (nullptr == m_impl)
      break;
    const MYON_ManifestMap_Hash32TableItem* hash_table_item = m_impl->m_source_index_hash_table.FirstManifestMapItemWithSourceIndex(component_type,source_component_index);
    if (nullptr == hash_table_item)
      break;
    return hash_table_item->m_map_item;
  }
  return MYON_ManifestMapItem::Unset;
}

bool MYON_ManifestMapItem::DestinationInManifest(
  const MYON_ComponentManifest& destination_manifest
  ) const
{
  if (MYON_ModelComponent::Type::Unset == m_component_type)
    return false;
  if (MYON_nil_uuid == m_destination_id)
    return false;
  const MYON_ComponentManifestItem& manifest_item = destination_manifest.ItemFromId(m_component_type, m_destination_id);
  if (manifest_item.ComponentType() != m_component_type)
    return false;
  if (manifest_item.Id() != m_destination_id)
    return false;
  if (manifest_item.Index() != m_destination_index)
    return false;
  return true;
}

MYON_ManifestMapItem MYON_ManifestMapItem::SwapSourceAndDestiation() const
{
  MYON_ManifestMapItem swap;
  swap.m_component_type = m_component_type;
  swap.m_source_index = m_destination_index;
  swap.m_destination_index = m_source_index;
  swap.m_source_id = m_destination_id;
  swap.m_destination_id = m_source_id;
  return swap;
}

bool MYON_ManifestMapItem::ClearSourceIdentification()
{
  return SetSourceIdentification(MYON_ModelComponent::Type::Unset, MYON_nil_uuid, MYON_UNSET_INT_INDEX);
}

bool MYON_ManifestMapItem::ClearDestinationIdentification()
{
  return SetDestinationIdentification(MYON_ModelComponent::Type::Unset, MYON_nil_uuid, MYON_UNSET_INT_INDEX);
}

bool MYON_ManifestMapItem::Internal_SetSourceOrDestinationIdentification(
  unsigned int which_identification, // 0 = source, 1 = destination
  MYON_ModelComponent::Type component_type,
  MYON_UUID id,
  int index
)
{
  if (0 != which_identification && 1 != which_identification)
  {
    MYON_ERROR("Invalid which_identification parameter");
    return false;
  }

  MYON_UUID* this_id[2] = { &m_source_id, &m_destination_id };
  int* this_index[2] = { &m_source_index, &m_destination_index };

  if (
    (MYON_ModelComponent::Type::Unset == component_type  || m_component_type == component_type)
    && MYON_nil_uuid == id 
    && MYON_UNSET_INT_INDEX == index
    )
  {
    *(this_id[which_identification]) = MYON_nil_uuid;
    *(this_index[which_identification]) = MYON_UNSET_INT_INDEX;
    if (MYON_nil_uuid == *(this_id[1-which_identification]))
    {
      m_component_type = MYON_ModelComponent::Type::Unset;
      *(this_index[1-which_identification]) = MYON_UNSET_INT_INDEX;
    }
    return true;
  }

  if (MYON_nil_uuid == id)
  {
    if (MYON_UNSET_INT_INDEX != index)
    {
      if (0 == which_identification)
      {
        MYON_ERROR("source id is nil and source index is not MYON_UNSET_INT_INDEX");
      }
      else
      {
        MYON_ERROR("destination id is nil and destination index is not MYON_UNSET_INT_INDEX");
      }
      return false;
    }
  }
  else if ( 
    MYON_ModelComponent::IndexRequired(component_type) 
    && MYON_UNSET_INT_INDEX == index 
    )
  {
    if (MYON_ModelComponent::Type::InstanceDefinition != component_type)
    {
      if (0 == which_identification)
      {
        MYON_ERROR("A valid source index is required.");
      }
      else
      {
        MYON_ERROR("A valid destination index is required.");
      }
      return false;
    }
  }

  if (m_component_type != component_type)
  {
    if (MYON_ModelComponent::Type::Unset != m_component_type)
    {
      MYON_ERROR("source and destination component types not equal.");
      return false;
    }
    m_component_type = component_type;
  }
  *(this_id[which_identification]) = id;
  *(this_index[which_identification]) = index;

  return true;
}

bool MYON_ManifestMapItem::SetSourceIdentification(
  MYON_ModelComponent::Type component_type,
  MYON_UUID source_id,
  int source_index
)
{
  return Internal_SetSourceOrDestinationIdentification(0, component_type, source_id, source_index);
}


bool MYON_ManifestMapItem::SetDestinationIdentification(
  MYON_ModelComponent::Type component_type,
  MYON_UUID destination_id,
  int destination_index
)
{
  return Internal_SetSourceOrDestinationIdentification(1, component_type, destination_id, destination_index);
}

bool MYON_ManifestMapItem::SetSourceIdentification(
  const class MYON_ModelComponent* model_component
  )
{
  if (nullptr == model_component)
    return ClearSourceIdentification();
  return SetSourceIdentification(model_component->ComponentType(), model_component->Id(), model_component->Index());
}

bool MYON_ManifestMapItem::SetDestinationIdentification(
  const class MYON_ModelComponent* model_component
  )
{
  if (nullptr == model_component)
    return ClearDestinationIdentification();
  return SetDestinationIdentification(model_component->ComponentType(), model_component->Id(), model_component->Index());
}


bool MYON_ManifestMapItem::SetSourceIdentification(
  const class MYON_ComponentManifestItem* manifest_item
  )
{
  if (nullptr == manifest_item)
    return ClearSourceIdentification();
  return SetSourceIdentification(manifest_item->ComponentType(), manifest_item->Id(), manifest_item->Index());
}


bool MYON_ManifestMapItem::SetDestinationIdentification(
  const class MYON_ComponentManifestItem* manifest_item
  )
{
  if (nullptr == manifest_item)
    return ClearDestinationIdentification();
  return SetDestinationIdentification(manifest_item->ComponentType(), manifest_item->Id(), manifest_item->Index());
}

bool MYON_ManifestMapItem::SetSourceIdentification(
  const class MYON_ManifestMapItem* map_item
 )
{
  if (nullptr == map_item)
    return ClearSourceIdentification();
  return SetSourceIdentification(map_item->ComponentType(), map_item->SourceId(), map_item->SourceIndex());
}

bool MYON_ManifestMapItem::SetDestinationIdentification(
  const class MYON_ManifestMapItem* map_item
  )
{
  if (nullptr == map_item)
    return ClearDestinationIdentification();
  return SetDestinationIdentification(map_item->ComponentType(), map_item->DestinationId(), map_item->DestinationIndex());
}


MYON_ModelComponent::Type MYON_ManifestMapItem::ComponentType() const
{
  return m_component_type;
}

const MYON_UUID& MYON_ManifestMapItem::SourceId() const
{
  return m_source_id;
}

const MYON_UUID& MYON_ManifestMapItem::DestinationId() const
{
  return m_destination_id;
}

int MYON_ManifestMapItem::SourceIndex() const
{
  return m_source_index;
}

int MYON_ManifestMapItem::DestinationIndex() const
{
  return m_destination_index;
}

bool MYON_ManifestMapItem::SourceInManifest(
  const MYON_ComponentManifest& source_manifest
  ) const
{
  if (MYON_ModelComponent::Type::Unset == m_component_type)
    return false;
  if (MYON_nil_uuid == m_source_id)
    return false;
  const MYON_ComponentManifestItem& manifest_item = source_manifest.ItemFromId(m_component_type, m_source_id);
  if (manifest_item.ComponentType() != m_component_type)
    return false;
  if (manifest_item.Id() != m_source_id)
    return false;
  if (manifest_item.Index() != m_source_index)
    return false;
  return true;
}

bool MYON_ManifestMap::GetAndValidateDestinationIndex(
  MYON_ModelComponent::Type component_type,
  int source_component_index,
  const MYON_ComponentManifest& destination_manifest,
  int* destination_component_index
  ) const
{
  bool rc = false;
  int i = source_component_index;
  for (;;)
  {
    if (MYON_ModelComponent::Type::Unset == component_type)
      break;
    const MYON_ManifestMapItem& map_item = MapItemFromSourceIndex(component_type, source_component_index);
    if (map_item.SourceOrDestinationIsUnset())
      break;
    
    rc =
      (&destination_manifest == &MYON_ComponentManifest::Empty)
      ? true
      : map_item.DestinationInManifest(destination_manifest);

    if (rc)
      i = map_item.DestinationIndex();
    break;
  }
  if (nullptr != destination_component_index)
    *destination_component_index = i;
  return rc;
}

bool MYON_ManifestMap::GetAndValidateDestinationIndex(
  MYON_ModelComponent::Type component_type,
  const MYON_UUID& source_component_id,
  const MYON_ComponentManifest& destination_manifest,
  int* destination_component_index
  ) const
{
  bool rc = false;
  int i = MYON_UNSET_INT_INDEX;
  for (;;)
  {
    if (MYON_ModelComponent::Type::Unset == component_type)
      break;
    const MYON_ManifestMapItem& map_item = MapItemFromSourceId(source_component_id);
    if (map_item.SourceOrDestinationIsUnset())
      break;
    if (component_type != map_item.ComponentType())
      break;
    rc = map_item.DestinationInManifest(destination_manifest);
    if (rc)
      i = map_item.DestinationIndex();
    break;
  }
  if (nullptr != destination_component_index)
    *destination_component_index = i;
  return rc;
}

bool MYON_ManifestMap::GetAndValidateDestinationId(
  MYON_ModelComponent::Type component_type,
  const MYON_UUID& source_component_id,
  const MYON_ComponentManifest& destination_manifest,
  MYON_UUID* destination_component_id
  ) const
{
  bool rc = false;
  MYON_UUID id = MYON_nil_uuid;
  for (;;)
  {
    if (MYON_ModelComponent::Type::Unset == component_type)
      break;
    const MYON_ManifestMapItem& map_item = MapItemFromSourceId(source_component_id);
    if (map_item.SourceOrDestinationIsUnset())
      break;
    if (component_type != map_item.ComponentType())
      break;
    rc = map_item.DestinationInManifest(destination_manifest);
    if (rc)
      id = map_item.DestinationId();
    break;
  }
  if (nullptr != destination_component_id)
    *destination_component_id = id;
  return rc;
}

int MYON_ComponentManifestItem::CompareComponentType(
  const MYON_ComponentManifestItem* a,
  const MYON_ComponentManifestItem* b
  )
{
  const unsigned int a_type = static_cast<unsigned int>(a->m_component_type);
  const unsigned int b_type = static_cast<unsigned int>(b->m_component_type);
  if (a_type < b_type)
    return -1;
  if (a_type > b_type)
    return 1;
  return 0;
}

int MYON_ComponentManifestItem::CompareIndex(
  const MYON_ComponentManifestItem*const* a,
  const MYON_ComponentManifestItem*const* b
  )
{
  const MYON_ComponentManifestItem* a_item = *a;
  const MYON_ComponentManifestItem* b_item = *b;
  int rc = MYON_ComponentManifestItem::CompareComponentType(a_item,b_item);
  if (0 != rc)
    return rc;
  const auto a_index = a_item->m_index;
  const auto b_index = b_item->m_index;
  if ( a_index < b_index )
    return -1;
  if ( a_index > b_index )
    return 1;
  return 0;
}


int MYON_ComponentManifestItem::CompareId(
  const MYON_ComponentManifestItem*const* a, 
  const MYON_ComponentManifestItem*const* b
  )
{
  const MYON_ComponentManifestItem* a_item = *a;
  const MYON_ComponentManifestItem* b_item = *b;
  int rc = MYON_ComponentManifestItem::CompareComponentType(a_item,b_item);
  if (0 == rc)
    rc = MYON_UuidCompare(a_item->m_id,b_item->m_id);
  return rc;
}

int MYON_ComponentManifestItem::CompareNameHash(
  const MYON_ComponentManifestItem*const* a,
  const MYON_ComponentManifestItem*const* b
  )
{
  const MYON_ComponentManifestItem* a_item = *a;
  const MYON_ComponentManifestItem* b_item = *b;
  int rc = MYON_ComponentManifestItem::CompareComponentType(a_item,b_item);
  if (0 == rc)
    rc = MYON_NameHash::Compare(a_item->m_name_hash,b_item->m_name_hash);
  return rc;
}

MYON_ComponentManifestItem::MYON_ComponentManifestItem(
  const class MYON_ModelComponent& component
  )
  : m_component_type(component.ComponentType())
  , m_index(component.Index())
  , m_component_runtime_serial_number(component.RuntimeSerialNumber())
  , m_id(component.Id())
  , m_name_hash(component.NameHash())
{}


MYON_ComponentManifestItem::MYON_ComponentManifestItem(
  const class MYON_ModelComponent& component,
  const MYON_UUID& manifest_id,
  const class MYON_NameHash& manifest_name_hash
  )
  : m_component_type(component.ComponentType())
  , m_component_runtime_serial_number(component.RuntimeSerialNumber())
  , m_id(manifest_id)
  , m_name_hash(manifest_name_hash)
{}

MYON_ComponentManifestItem::MYON_ComponentManifestItem(
  MYON_ModelComponent::Type component_type,
  MYON__UINT64 component_runtime_serial_number,
  const MYON_UUID& manifest_id,
  const class MYON_NameHash& manifest_name_hash
  )
  : m_component_type(component_type)
  , m_component_runtime_serial_number(component_runtime_serial_number)
  , m_id(manifest_id)
  , m_name_hash(manifest_name_hash)
{}


MYON_ComponentManifestItem::MYON_ComponentManifestItem(
  const class MYON_ModelComponent& component,
  int manifest_index,
  const MYON_UUID& manifest_id,
  const class MYON_NameHash& manifest_name_hash
  )
  : m_component_type(component.ComponentType())
  , m_index(manifest_index)
  , m_component_runtime_serial_number(component.RuntimeSerialNumber())
  , m_id(manifest_id)
  , m_name_hash(manifest_name_hash)
{}

MYON_ComponentManifestItem::MYON_ComponentManifestItem(
  MYON_ModelComponent::Type component_type,
  MYON__UINT64 component_runtime_serial_number,
  int manifest_index,
  const MYON_UUID& manifest_id,
  const class MYON_NameHash& manifest_name_hash
  )
  : m_component_type(component_type)
  , m_index(manifest_index)
  , m_component_runtime_serial_number(component_runtime_serial_number)
  , m_id(manifest_id)
  , m_name_hash(manifest_name_hash)
{}

bool MYON_ComponentManifestItem::IsValid() const
{
  return ( MYON_ModelComponent::Type::Unset != m_component_type && MYON_nil_uuid != m_id);
}

bool MYON_ComponentManifestItem::IsUnset() const
{
  return ( MYON_ModelComponent::Type::Unset == m_component_type || MYON_nil_uuid == m_id);
}

bool MYON_ComponentManifestItem::IsDeleted() const
{
  return 0 != (m_status_bits & 0x01U);
}

bool MYON_ComponentManifestItem::IsSystemComponent() const
{
  return 0 != (m_status_bits & 0x02U);
}

int MYON_ComponentManifestItem::Index() const
{
  return m_index;
}

void MYON_ComponentManifestItem::SetIndex( int index )
{
  m_index = index;
}

MYON_UUID MYON_ComponentManifestItem::Id() const
{
  return m_id;
}

void MYON_ComponentManifestItem::SetId(MYON_UUID id)
{
  m_id = id;
}

MYON_ModelComponent::Type MYON_ComponentManifestItem::ComponentType() const
{
  return m_component_type;
}

void MYON_ComponentManifestItem::SetComponentType(
  MYON_ModelComponent::Type component_type
)
{
  m_component_type = component_type;
}

const MYON_NameHash& MYON_ComponentManifestItem::NameHash() const
{
  return m_name_hash;
}

void MYON_ComponentManifestItem::SetNameHash(
  const MYON_NameHash& name_hash
)
{
  m_name_hash = name_hash;
}

MYON__UINT64 MYON_ComponentManifestItem::ComponentRuntimeSerialNumber() const
{
  return m_component_runtime_serial_number;
}

void MYON_ComponentManifestItem::SetComponentRuntimeSerialNumber(
  MYON__UINT64 component_runtime_serial_number
)
{
  m_component_runtime_serial_number = component_runtime_serial_number;
}

void MYON_ComponentManifestItem::Internal_SetDeletedState(
  bool bDeleted
  )
{
  MYON__UINT32 deleted_status_bit = 0x01U;
  if (bDeleted)
  {
    m_status_bits |= deleted_status_bit;
    if (false == m_name_hash.IsEmptyNameHash())
      m_name_hash = MYON_NameHash::CreateIdAndUnsetName(m_name_hash.ParentId());
  }
  else
    m_status_bits &= ~deleted_status_bit;
}

////class MYON_ModelComponentIndexChange
////{
////public:
////  MYON_ModelComponentIndexChange() = default;
////  ~MYON_ModelComponentIndexChange() = default;
////  MYON_ModelComponentIndexChange(const MYON_ModelComponentIndexChange&) = default;
////  MYON_ModelComponentIndexChange& operator=(const MYON_ModelComponentIndexChange&) = default;
////
////  MYON_ModelComponentIndexChange(
////    int original_index,
////    unsigned int manifest_index
////    );
////
////  static const MYON_ModelComponentIndexChange Unset;
////
////  static int CompareOriginalIndex(
////    const MYON_ModelComponentIndexChange* a,
////    const MYON_ModelComponentIndexChange* b
////    );
////
////  static int CompareOriginalIndexAndManifestIndex(
////    const MYON_ModelComponentIndexChange* a,
////    const MYON_ModelComponentIndexChange* b
////    );
////
////  int m_original_index = MYON_ComponentManifest::UnsetComponentIndex;
////  unsigned int m_manifest_index = MYON_ComponentManifest::UnsetManifestItemIndex;
////};
////
////const MYON_ModelComponentIndexChange MYON_ModelComponentIndexChange::Unset;
////
////MYON_ModelComponentIndexChange::MYON_ModelComponentIndexChange(
////  int original_index,
////  unsigned int manifest_index
////  )
////  : m_original_index(original_index)
////  , m_manifest_index(manifest_index)
////{}
////
////int MYON_ModelComponentIndexChange::CompareOriginalIndex(
////  const MYON_ModelComponentIndexChange* a,
////  const MYON_ModelComponentIndexChange* b
////  )
////{
////  if ( a == b )
////    return 0;
////  if ( nullptr == a )
////    return 1;
////  if ( nullptr == b)
////    return -1;
////  if ( a->m_original_index < b->m_original_index )
////    return -1;
////  if ( a->m_original_index > b->m_original_index )
////    return 1;
////  return 0;
////}

////int MYON_ModelComponentIndexChange::CompareOriginalIndexAndManifestIndex(
////  const MYON_ModelComponentIndexChange* a,
////  const MYON_ModelComponentIndexChange* b
////  )
////{
////  if ( a == b )
////    return 0;
////  if ( nullptr == a )
////    return 1;
////  if ( nullptr == b)
////    return -1;
////  if (MYON_ComponentManifest::UnsetManifestItemIndex == a->m_manifest_index)
////  {
////    if ( MYON_ComponentManifest::UnsetManifestItemIndex != b->m_manifest_index )
////      return 1; // a is removed and b is not removed
////  }
////  if (MYON_ComponentManifest::UnsetManifestItemIndex == b->m_manifest_index)
////    return -1; // b is removed and a is not removed.
////  if ( a->m_original_index < b->m_original_index )
////    return -1;
////  if ( a->m_original_index > b->m_original_index )
////    return 1;
////  if ( a->m_manifest_index < b->m_manifest_index )
////    return -1;
////  if ( a->m_manifest_index > b->m_manifest_index )
////    return 1;
////  return 0;
////}


////class MYON_ModelComponentReindexMap
////{
////public:
////  MYON_ModelComponentReindexMap() = default;
////  ~MYON_ModelComponentReindexMap() = default;
////  MYON_ModelComponentReindexMap(const MYON_ModelComponentReindexMap&) = default;
////  MYON_ModelComponentReindexMap& operator=(const MYON_ModelComponentReindexMap&) = default;
////
////  bool AddIndexChange(
////    int original_index,
////    unsigned int manifest_index
////    );
////
////  bool RemoveIndexChange(
////    int original_index
////    );
////
////  /*
////  Returns:
////    If original_index has been remapped, then the remapped value is returned.
////    If original_index is not remapped and original_index >= 0, then original_index is returned.
////    Otherwise MYON_ComponentManifest::UnsetManifestItemIndex is returned.
////  */
////  unsigned int FirstManifestIndexFromOriginalIndex(
////    int original_index,
////    bool bUniqueItemRequired
////    ) const;
////
////  void RemoveAll()
////  {
////    m_most_recent_change = MYON_ModelComponentIndexChange::Unset;
////    m_sorted_count = 0;
////    m_removed_count = 0;
////    m_reindex_map.Empty();
////  }
////
////private:
////  MYON_ModelComponentIndexChange m_most_recent_change = MYON_ModelComponentIndexChange::Unset;
////  mutable unsigned int m_sorted_count = 0;
////  mutable unsigned int m_removed_count = 0;
////  mutable MYON_SimpleArray<MYON_ModelComponentIndexChange> m_reindex_map;
////
////  unsigned int SortList() const;
////};
////
////bool MYON_ModelComponentReindexMap::AddIndexChange(
////  int original_index,
////  unsigned int manifest_index
////  )
////{
////  MYON_ModelComponentIndexChange index_change(original_index,manifest_index);
////  if (index_change.m_original_index >= 0 && ((unsigned int)index_change.m_original_index) == index_change.m_manifest_index)
////    return false;
////  if ( MYON_ModelComponentIndexChange::Unset.m_original_index == index_change.m_original_index )
////    return false;
////  if ( MYON_ModelComponentIndexChange::Unset.m_manifest_index == index_change.m_manifest_index )
////    return false;
////
////  if (0 == m_reindex_map.Capacity())
////    m_reindex_map.Reserve(32);
////
////  const unsigned int count = m_reindex_map.UnsignedCount();
////  if ( 0 == count || (count == m_sorted_count  && MYON_ModelComponentIndexChange::CompareOriginalIndexAndManifestIndex(&m_reindex_map[count-1], &index_change) <= 0 ) )
////    m_sorted_count++;
////  m_reindex_map.Append(index_change);
////
////  m_most_recent_change = index_change;
////
////  return true;
////}
////
////
////bool MYON_ModelComponentReindexMap::RemoveIndexChange(
////  int original_index
////  )
////{
////  if ( m_most_recent_change.m_original_index == original_index )
////    m_most_recent_change = MYON_ModelComponentIndexChange::Unset;
////
////  const unsigned int count = m_reindex_map.UnsignedCount();
////  if ( count <= 0 )
////    return false;
////
////  MYON_ModelComponentIndexChange* a = m_reindex_map.Array();
////  for (unsigned int i = m_sorted_count; i < count - 1; i++)
////  {
////    if (original_index == a[i].m_original_index)
////    {
////      if ( a[i].m_manifest_index == MYON_ComponentManifest::UnsetManifestItemIndex )
////        return false;
////      m_removed_count++;
////      a[i].m_manifest_index = MYON_ComponentManifest::UnsetManifestItemIndex;
////      return true;
////    }
////  }
////
////  const MYON_ModelComponentIndexChange key(original_index,MYON_ComponentManifest::UnsetManifestItemIndex);
////
////  a = (MYON_ModelComponentIndexChange*)bsearch(
////    &key,
////    a,
////    m_sorted_count,
////    sizeof(key),
////    (int(*)(const void *, const void *)) MYON_ModelComponentIndexChange::CompareOriginalIndex
////    );
////  if (nullptr != a)
////  {
////    if ( a->m_manifest_index == MYON_ComponentManifest::UnsetManifestItemIndex )
////      return false;
////    m_removed_count++;
////    a->m_manifest_index = MYON_ComponentManifest::UnsetManifestItemIndex;
////    return true;
////  }
////
////  return false;
////}
////
////
////unsigned int MYON_ModelComponentReindexMap::FirstManifestIndexFromOriginalIndex(
////  int original_index,
////  bool bUniqueItemRequired
////  ) const
////{
////  unsigned int manifest_index = MYON_ComponentManifest::UnsetManifestItemIndex;
////
////  // Frequently the most recently added item is searched for.
////  if (
////    original_index == m_most_recent_change.m_original_index 
////    && MYON_ComponentManifest::UnsetManifestItemIndex != m_most_recent_change.m_manifest_index
////    )
////  {
////    if (!bUniqueItemRequired)
////      return m_most_recent_change.m_manifest_index;
////    manifest_index = m_most_recent_change.m_manifest_index;
////  }
////
////  const unsigned int count = SortList();
////  if (count <= m_removed_count)
////  {
////    // no index remapping is present.
////    return 
////      (original_index >= 0)
////      ? ((unsigned int)original_index) // original index not remapped when added to manifest
////      : MYON_ComponentManifest::UnsetManifestItemIndex;
////  }
////
////  const MYON_ModelComponentIndexChange* a = m_reindex_map.Array();
////  for (unsigned int i = m_sorted_count; i < count; i++)
////  {
////    if (original_index == a[i].m_original_index && MYON_ComponentManifest::UnsetManifestItemIndex != a[i].m_manifest_index)
////    {
////      if ( !bUniqueItemRequired )
////        return a[i].m_manifest_index;
////      if (MYON_ComponentManifest::UnsetManifestItemIndex == manifest_index)
////        manifest_index = a[i].m_manifest_index;
////      else if (manifest_index != a[i].m_manifest_index)
////        return MYON_ComponentManifest::UnsetManifestItemIndex; // not unique
////    }
////  }
////
////  if (m_sorted_count > 0)
////  {
////    const MYON_ModelComponentIndexChange key(original_index, MYON_ComponentManifest::UnsetManifestItemIndex);
////
////    const MYON_ModelComponentIndexChange* f = (const MYON_ModelComponentIndexChange*)bsearch(
////      &key,
////      a,
////      m_sorted_count,
////      sizeof(key),
////      (int(*)(const void *, const void *)) MYON_ModelComponentIndexChange::CompareOriginalIndex
////      );
////
////    if (nullptr != f)
////    {
////      const MYON_ModelComponentIndexChange* a1 = a + (m_sorted_count-1);
////      while (f > a && f[-1].m_original_index == original_index)
////        f--;
////      while (f < a1 && MYON_ComponentManifest::UnsetManifestItemIndex == f->m_manifest_index && f[1].m_original_index == original_index)
////        f++;
////      if (MYON_ComponentManifest::UnsetManifestItemIndex != f->m_manifest_index)
////      {
////        if (!bUniqueItemRequired)
////          return f->m_manifest_index;
////        if (MYON_ComponentManifest::UnsetManifestItemIndex == manifest_index)
////          manifest_index = f->m_manifest_index;
////        else if (f->m_manifest_index != manifest_index)
////          return MYON_ComponentManifest::UnsetManifestItemIndex;
////        for (const MYON_ModelComponentIndexChange* p = f; p <= a1; p++)
////        {
////          if (p->m_original_index != original_index)
////            break;
////          if (p->m_manifest_index != manifest_index && MYON_ComponentManifest::UnsetManifestItemIndex != p->m_manifest_index)
////            return MYON_ComponentManifest::UnsetManifestItemIndex; // original index used multiple times
////        }
////      }
////    }
////  }
////
////  if ( MYON_ComponentManifest::UnsetManifestItemIndex != manifest_index )
////    return manifest_index;  // original index was remapped when added to manifest
////
////  return 
////    (original_index >= 0)
////    ? ((unsigned int)original_index) // original index not remapped when added to manifest
////    : MYON_ComponentManifest::UnsetManifestItemIndex;
////}
////
////unsigned int MYON_ModelComponentReindexMap::SortList() const
////{
////  unsigned int count = m_reindex_map.UnsignedCount();
////  if (m_removed_count >= count)
////  {
////    m_reindex_map.SetCount(0);
////    return 0;
////  }
////  if (m_sorted_count + 4 >= count && m_removed_count <= m_sorted_count/16)
////    return count;
////  
////  MYON_ModelComponentIndexChange* a = const_cast<MYON_ModelComponentIndexChange*>(m_reindex_map.Array());
////
////  const unsigned int unsorted_count = count-m_sorted_count;
////  if ( m_sorted_count <= 32 || unsorted_count > count/2 || m_removed_count > 0 )
////  {
////    MYON_qsort(a, count, sizeof(a[0]), (int(*)(const void*, const void*))MYON_ModelComponentIndexChange::CompareOriginalIndexAndManifestIndex);
////    if (m_removed_count > 0)
////    {
////      count -= m_removed_count;
////      m_removed_count = 0;
////      m_reindex_map.SetCount(count);
////    }
////    m_sorted_count = count;
////    return count;
////  }
////
////  // The sorted portion of item_list[] is at least as large as the unsorted portion.
////  // Sort the unsorted portion of item_list[] and shuffle it into the sorted portion.
////  // The assumption is that this will be faster that simply sorting the entire list
////  // again. For some value of sorted_count and count, this will be true. 
////  // The threshhold ( m_sorted_count <= 32 || unsorted_count > count/2 )
////  // is a guess and needs testing and tuning to be optimal.
////  else
////  {
////    // copy unsorted items to buffer[]
////    MYON_ModelComponentIndexChange stack_buffer[64];
////    const size_t sizeof_buffer = unsorted_count*sizeof(stack_buffer[0]);
////    MYON_ModelComponentIndexChange* buffer 
////      = (sizeof(stack_buffer) >= sizeof_buffer)
////      ? stack_buffer
////      : (MYON_ModelComponentIndexChange*)onmalloc(sizeof_buffer);
////    memcpy(buffer,a+m_sorted_count,sizeof_buffer);
////
////    // sort buffer[]
////    MYON_qsort(buffer, unsorted_count, sizeof(buffer[0]), (int(*)(const void*, const void*))MYON_ModelComponentIndexChange::CompareOriginalIndexAndManifestIndex);
////
////    // shuffle sorted portion of a[] and sorted buffer[] together in order.
////    MYON_ModelComponentIndexChange* e = a + m_sorted_count - 1;      // e = last element in original sorted array.
////    MYON_ModelComponentIndexChange* f = buffer + m_sorted_count - 1; // f = last element in new sorted bufffer[]
////    MYON_ModelComponentIndexChange* dst = a + count;
////    while(dst-- > a)
////    {
////      if (MYON_ModelComponentIndexChange::CompareOriginalIndexAndManifestIndex(e, f) <= 0)
////      {
////        *dst = *f;
////        if ( buffer == f )
////          break;
////        f--;
////      }
////      else
////      {
////        *dst = *e;
////        if (a == e)
////        {
////          memcpy(a, buffer, ((f - buffer) + 1)*sizeof(a[0]));
////          break;
////        }
////        e--;
////      }
////    }
////    if ( buffer != stack_buffer )
////      onfree(buffer);
////  }
////  m_sorted_count = count;
////  return count;
////}
////

class MYON_ComponentManifestTableIndex
{
public:
  MYON_ComponentManifestTableIndex();
  ~MYON_ComponentManifestTableIndex() = default;

private:
  MYON_ComponentManifestTableIndex(const MYON_ComponentManifestTableIndex&) = delete;
  MYON_ComponentManifestTableIndex& operator=(const MYON_ComponentManifestTableIndex&) = delete;

public:
  /*
  Description:
    For indexed item, manifest indices are >= 0 and < IndexLimit().
  Returns 
    manifest index limit.
  */
  int IndexLimit() const
  {
    return m_item_list.Count();
  }

  /*
    Returns number of active, deleted, and system items.
  */
  unsigned int TotalItemCount() const
  {
    return m_active_and_deleted_item_count + m_system_item_count;
  }

  /*
    Returns number of active and deleted items.
  */
  unsigned int ActiveAndDeletedItemCount() const
  {
    return m_active_and_deleted_item_count;
  }

  /*
    Returns number of active and deleted items.
  */
  unsigned int ActiveItemCount() const
  {
    return 
      (m_active_and_deleted_item_count > m_deleted_item_count)
      ? (m_active_and_deleted_item_count - m_deleted_item_count)
      : 0;
  }

  /*
    Returns number of active items.
  */
  unsigned int DeletedItemCount() const
  {
    return m_deleted_item_count;
  }

  /*
    Returns number of system items.
  */
  unsigned int SystemItemCount() const
  {
    return m_system_item_count;
  }

  bool AddSystemItem(
    MYON_ComponentManifestItem_PRIVATE* system_item
    );


  MYON_ModelComponent::Type ComponentType() const
  {
    return m_component_type;
  }

  bool AddItemAndSetManifestIndex(
    MYON_ComponentManifestItem_PRIVATE* item
    );

  void IncrementDeletedCount()
  {
    m_deleted_item_count++;
  }

  bool DecrementDeletedCount()
  {
    if (m_deleted_item_count > 0)
    {
      m_deleted_item_count--;
      return true;
    }

    MYON_ERROR("No items are deleted.");
    return false;
  }

  const MYON_ComponentManifestItem_PRIVATE* SystemItemFromIndex(
    int system_item_index
    ) const; 

  const MYON_ComponentManifestItem_PRIVATE* ItemFromManifestItemIndex(
    int manifest_item_index
    ) const
  {
    return
      ( manifest_item_index >= 0 && manifest_item_index < m_item_list.Count() )
      ? m_item_list[manifest_item_index]
      : nullptr;
  }

  const MYON_ComponentManifestItem_PRIVATE* FirstItem() const
  {
    return m_first_item;
  }

  const MYON_ComponentManifestItem_PRIVATE* LastItem() const
  {
    return m_last_item;
  }

  const MYON_ComponentManifestItem_PRIVATE* NextItem(
    const MYON_ComponentManifestItem_PRIVATE* item
    ) const
  {
    return
      ( nullptr != item && m_manifest_table_sn == item->m_manifest_table_sn)
      ? item->m_next
      : nullptr;
  }

  const MYON_ComponentManifestItem_PRIVATE* PreviousItem(
    const MYON_ComponentManifestItem_PRIVATE* item
    ) const
  {
    return
      ( nullptr != item && m_manifest_table_sn == item->m_manifest_table_sn)
      ? item->m_prev
      : nullptr;
  }

  bool RemoveItem(
    const MYON_ComponentManifestItem_PRIVATE* item
    );

  bool ItemInTable(
    const MYON_ComponentManifestItem_PRIVATE* item
    ) const
  {
    return ( nullptr != item && m_manifest_table_sn == item->m_manifest_table_sn);
  }

  void RemoveAllItems(
    bool bResetManifestIndex
    );

  bool SetComponentType(
    MYON_ModelComponent::Type component_type
    );

private:
  const unsigned int m_manifest_table_sn;
  MYON_ModelComponent::Type m_component_type = MYON_ModelComponent::Type::Unset;
  bool m_bIndexedComponent = false;

  unsigned int m_system_item_count = 0; // "system" components
  unsigned int m_active_and_deleted_item_count = 0; // model components (includes deleted components)
  unsigned int m_deleted_item_count = 0; // deleted components

  // linked list of active and deleted model components.
  MYON_ComponentManifestItem_PRIVATE* m_first_item = nullptr;
  MYON_ComponentManifestItem_PRIVATE* m_last_item = nullptr;

  // linked list of system model components.
  MYON_ComponentManifestItem_PRIVATE* m_first_system_item = nullptr;
  MYON_ComponentManifestItem_PRIVATE* m_last_system_item = nullptr;

  // Used for indexed items
  // m_item_list[] is always complete and sorted by item->m_index for
  // entries that are not nullptr. Includes active, deleted, and system components.
  MYON_SimpleArray<const MYON_ComponentManifestItem_PRIVATE*> m_item_list;
};

static unsigned int MYON_ComponentManifestTableIndex_sn_generator=0;

MYON_ComponentManifestTableIndex::MYON_ComponentManifestTableIndex()
  : m_manifest_table_sn(++MYON_ComponentManifestTableIndex_sn_generator)
{}

bool MYON_ComponentManifestTableIndex::SetComponentType(
  MYON_ModelComponent::Type component_type
  )
{
  if (MYON_ModelComponent::Type::Unset != m_component_type || false != m_bIndexedComponent)
  {
    MYON_ERROR("component type is already set.");
    return false;
  }
  if (MYON_ModelComponent::Type::Unset == component_type)
  {
    MYON_ERROR("invalid  component_type parameter.");
    return false;
  }
  if (MYON_ModelComponent::Type::Mixed == component_type)
  {
    MYON_ERROR("invalid  component_type parameter.");
    return false;
  }

  m_component_type = component_type;
  m_bIndexedComponent = MYON_ModelComponent::IndexRequired(component_type);

  return true;
}


bool MYON_ComponentManifestTableIndex::AddSystemItem(
  MYON_ComponentManifestItem_PRIVATE* system_item
  )
{
  if ( nullptr != system_item
    && system_item->ComponentType() == m_component_type
    && nullptr == SystemItemFromIndex(system_item->Index())
    && 0 == system_item->m_manifest_table_sn
    && MYON_ModelComponent::Type::Unset != m_component_type
    )
  {
    const bool bValidSystemComponentIndex
      = m_bIndexedComponent
      ? (system_item->Index() < 0 && system_item->Index() > MYON_UNSET_INT_INDEX)
      : (MYON_UNSET_INT_INDEX == system_item->Index());
    if (bValidSystemComponentIndex)
    {
      system_item->m_prev = m_last_system_item;
      system_item->m_next = nullptr;
      if (nullptr != m_last_system_item)
        m_last_system_item->m_next = system_item;
      else
        m_first_system_item = system_item;
      m_last_system_item = system_item;
      m_system_item_count++;
      system_item->m_manifest_table_sn = m_manifest_table_sn;
      return true;
    }
  }
  MYON_ERROR("Invalid system item parameter.");
  return false;
}


bool MYON_ComponentManifestTableIndex::AddItemAndSetManifestIndex(
  MYON_ComponentManifestItem_PRIVATE* item
  )
{
  if ( nullptr == item )
    return false;

  if (0 != item->m_manifest_table_sn)
  {
    MYON_ERROR("item is already in a table");
    return false;
  }
  
  if (m_component_type != item->ComponentType() || MYON_ModelComponent::Type::Unset == m_component_type)
  {
    MYON_ERROR("Item has component type.");
    return false;
  }

  if (m_bIndexedComponent)
  {
    item->SetIndex(m_item_list.Count());
    m_item_list.Append(item);
  }
  else
  {
    item->SetIndex(MYON_UNSET_INT_INDEX);
  }
  
  if (nullptr == m_last_item)
  {
    m_first_item = item;
    item->m_prev = nullptr;
  }
  else
  {
    m_last_item->m_next = item;
    item->m_prev = m_last_item;
  }
  m_last_item = item;
  item->m_next = nullptr;
  
  item->m_manifest_table_sn = m_manifest_table_sn;
  m_active_and_deleted_item_count++;

  if (item->IsDeleted())
    IncrementDeletedCount();

  return true;
}

const MYON_ComponentManifestItem_PRIVATE* MYON_ComponentManifestTableIndex::SystemItemFromIndex(
  int sytem_item_index
  ) const
{
  if (m_bIndexedComponent && sytem_item_index < 0 && sytem_item_index > MYON_UNSET_INT_INDEX )
  {
    // The linked list of system items is always short - hash tables and sorting are a waste of time
    for (const MYON_ComponentManifestItem_PRIVATE* system_item = m_first_system_item; nullptr != system_item; system_item = system_item->m_next)
    {
      if ( system_item->Index() == sytem_item_index )
        return system_item;
    }
  }

  return nullptr;
}

bool MYON_ComponentManifestTableIndex::RemoveItem(
  const MYON_ComponentManifestItem_PRIVATE* item
  )
{
  if ( 
    nullptr != item 
    && m_manifest_table_sn == item->m_manifest_table_sn
    && m_active_and_deleted_item_count > 0 
    )
  {
    if (m_bIndexedComponent)
    {
      if (item->Index() >= 0 && item->Index() < m_item_list.Count() && item == m_item_list[item->Index()])
      {
        m_item_list[item->Index()] = nullptr;
      }
      else
      {
        MYON_ERROR("item cannot be removed.");
        return false;
      }
    }

    if (item->m_prev)
      item->m_prev->m_next = item->m_next;
    else
      m_first_item = item->m_next;
    if ( item->m_next )
      item->m_next->m_prev = item->m_prev;
    else
      m_last_item = item->m_prev;
    
    const_cast<MYON_ComponentManifestItem_PRIVATE*>(item)->m_next = nullptr;
    const_cast<MYON_ComponentManifestItem_PRIVATE*>(item)->m_prev = nullptr;
    const_cast<MYON_ComponentManifestItem_PRIVATE*>(item)->m_manifest_table_sn = 0;

    m_active_and_deleted_item_count--;
    if (item->IsDeleted())
      DecrementDeletedCount();
    return true;
  }

  MYON_ERROR("item cannot be removed.");

  return false;
}

void MYON_ComponentManifestTableIndex::RemoveAllItems(
  bool bResetManifestIndex
  )
{
  m_item_list.Zero();
  if ( bResetManifestIndex )
    m_item_list.SetCount(0);
  
  m_active_and_deleted_item_count = 0;
  m_deleted_item_count = 0;

  m_first_item = nullptr;
  m_last_item = nullptr;

  m_system_item_count = 0;
  m_first_system_item = nullptr;
  m_last_system_item = nullptr;
}

class MYON_ComponentManifestImpl
{
public:
  MYON_ComponentManifestImpl() MYON_NOEXCEPT;
  ~MYON_ComponentManifestImpl() = default;

private:
  MYON_ComponentManifestImpl(const MYON_ComponentManifestImpl&) = delete;
  MYON_ComponentManifestImpl& operator=(const MYON_ComponentManifestImpl&) = delete;

public:
  int IndexLimit(
    MYON_ModelComponent::Type component_type
    ) const;

  // All items (including system and deleted items)
  unsigned int ItemCountImpl() const;

  // Number of items of a specific type.
  // Includes active, deleted, and system items.
  unsigned int TotalItemCountImpl(
    MYON_ModelComponent::Type component_type
    ) const;

  // Number of items of a specific type.
  // Includes deleted items.
  // Excludes system items.
  unsigned int ActiveAndDeletedItemCountImpl(
    MYON_ModelComponent::Type component_type
    ) const;

  // Number of items of a specific type.
  // Includes deleted items.
  // Excludes system items.
  unsigned int ActiveItemCountImpl(
    MYON_ModelComponent::Type component_type
    ) const;

  unsigned int DeletedItemCountImpl(
    MYON_ModelComponent::Type component_type
    ) const;

  // Number of system items.
  unsigned int SystemItemCountImpl(
    MYON_ModelComponent::Type component_type
    ) const;

  MYON_UUID UnusedId(
    MYON_UUID candidate_id
    ) const
  {
    if ( false == (MYON_nil_uuid == candidate_id) && IdIsAvailable(candidate_id) )
      return candidate_id;
    return MYON_CreateId();
  }

  bool IdIsAvailable(
    MYON_UUID id
    ) const
  {
    for (;;)
    {
      if ( MYON_nil_uuid == id )
        break;
      if (nullptr != m_manifest_id_hash_table.FirstManifestItemWithId(id))
        break;
      return true;
    }
    return false;
  }

  const MYON_wString UnusedName(
    MYON_ModelComponent::Type component_type,
    const wchar_t* candidate_name,
    const wchar_t* base_name,
    const wchar_t* suffix_separator,
    unsigned int suffix0,
    unsigned int* suffix_value
    ) const;

  bool NameIsAvailable(
    MYON_ModelComponent::Type component_type,
    const MYON_NameHash& candidate_name_hash
    ) const
  {
    const MYON_ComponentNameHash32Table& name_hash_table = ComponentNameHash32TableConst(component_type);
    return nullptr == name_hash_table.FirstManifestItemWithName(component_type,candidate_name_hash);
  }

  const MYON_ComponentManifestItem_PRIVATE* AddItem(
    const MYON_ComponentManifestItem& item,
    const MYON_UUID& component_parent_id,
    bool bResolveIdAndNameCollisions,
    const wchar_t* candidate_name,
    MYON_wString* assigned_name
    );

  const MYON_ComponentManifestItem_PRIVATE* AddSystemItem(
    const MYON_ComponentManifestItem& item
    );

  const MYON_ComponentManifestItem_PRIVATE* ItemFromManifestIndex(
    MYON_ModelComponent::Type component_type,
    int manifest_item_index
    ) const;

  const MYON_ComponentManifestItem_PRIVATE* ItemFromManifestId(
    const MYON_UUID& manifest_item_id
    ) const;

  const MYON_ComponentManifestItem_PRIVATE* ItemFromNameHash(
    MYON_ModelComponent::Type component_type,
    const MYON_NameHash& manifest_item_name_hash
    ) const;

  const MYON_ComponentManifestItem_PRIVATE* ItemFromComponentRuntimeSerialNumber(
    MYON__UINT64 component_runtime_serial_number
    ) const;

  const MYON_ComponentManifestItem_PRIVATE* ChangeItemNameHash(
    const MYON_UUID& manifest_item_id,
    const MYON_NameHash& new_name_hash
    );

  const MYON_ComponentManifestItem_PRIVATE* DeleteItem(
    const MYON_ComponentManifestItem_PRIVATE* manifest_item
    );

  const MYON_ComponentManifestItem_PRIVATE* UndeleteItem(
    const MYON_ComponentManifestItem_PRIVATE* manifest_item,
    const MYON_UUID& parent_id,
    const wchar_t* candidate_name,
    MYON_wString& assigned_name
    );

  const MYON_ComponentManifestItem_PRIVATE* ChangeItemComponentRuntimeSerialNumber(
    const MYON_ComponentManifestItem_PRIVATE* manifest_item,
    MYON__UINT64 new_component_runtime_serial_number
    );

  bool RemoveItem(
    const MYON_ComponentManifestItem_PRIVATE* item
    );

  bool RemoveAllItems(
    MYON_ModelComponent::Type component_type,
    bool bResetManifestIndex
    );

  const MYON_ComponentManifestItem_PRIVATE* SystemItemFromIndex(
    MYON_ModelComponent::Type component_type,
    int system_item_index
    ) const;

  const MYON_ComponentManifestItem_PRIVATE* SystemItemFromNameHash(
    MYON_ModelComponent::Type component_type,
    const MYON_NameHash& system_item_name_hash
    ) const;

  const class MYON_ComponentManifestItem_PRIVATE* FirstItem(
    MYON_ModelComponent::Type component_type
    ) const;

  const class MYON_ComponentManifestItem_PRIVATE* LastItem(
    MYON_ModelComponent::Type component_type
    ) const;

  const class MYON_ComponentManifestItem_PRIVATE* NextItem(
    const class MYON_ComponentManifestItem* item
    ) const;

  const class MYON_ComponentManifestItem_PRIVATE* PreviousItem(
    const class MYON_ComponentManifestItem* item
    ) const;

  const class MYON_ComponentManifestItem_PRIVATE* ManagedItemFromItem(
    const class MYON_ComponentManifestItem* item
    ) const
  {
    // First guess is that the id is the best choice.
    // Note that item may be from anyplace including a stack variable
    if ( nullptr == item )
      return nullptr;
    const MYON_ComponentManifestHash32TableItem* hash_item = m_manifest_id_hash_table.FirstManifestItemWithId(item->m_id);
    return ( nullptr != hash_item && nullptr != hash_item->m_manifest_item && item->m_component_type == hash_item->m_manifest_item->m_component_type)
      ? hash_item->m_manifest_item
      : nullptr;
  }

  MYON__UINT64 ManifestContentVersionNumber() const
  {
    return m_manifest_content_version_number;
  }

private:
  mutable MYON__UINT64 m_manifest_content_version_number = 0;

  // One for each MYON_ModelComponent type.
  MYON_ComponentManifestTableIndex* TableIndexFromType(
    MYON_ModelComponent::Type component_type
    );

  const MYON_ComponentManifestTableIndex* TableIndexFromTypeConst(
    MYON_ModelComponent::Type component_type
    ) const;

public:
  enum : unsigned int { TableCount = 17 }; // Count of items in MYON_ModelComponent::Type

private:
  MYON_ComponentManifestTableIndex m_table_index[MYON_ComponentManifestImpl::TableCount];

public:
  const unsigned int m_manifest_impl_sn;

private:
  // Fixed size pool for managing MYON_ComponentManifestItem_PRIVATE memory
  MYON_ComponentManifestItem_PRIVATE* AllocateItem();
  MYON_ComponentManifestItem_PRIVATE* AllocateItem(
    const MYON_ComponentManifestItem& src
    );
  void DeallocateItem(MYON_ComponentManifestItem_PRIVATE*);
  MYON_FixedSizePool m_item_PRIVATE__fsp;

private:
  // Fixed size pool for the following hash tables
  MYON_ManifestHash32TableItemFixedSizePool m_hash_table_item_fsp;
  
  // Temporary - Hash table used for component ids. 
  //   When all components are derived from MYON_ModelComponent,
  //   m_manifest_id_hash_table can be removed and 
  //   m_component_serial_number_map can be used to store
  //   the manifest id values.
  //  Currently Rhino CRhinoDoc model geometry and model lights are not derived from MYON_ModelComponent.
  MYON_ComponentIdHash32Table m_manifest_id_hash_table;

  
  MYON_ComponentNameHash32Table& ComponentNameHash32Table(
    MYON_ModelComponent::Type component_type
    );

  MYON_SerialNumberMap m_component_serial_number_map;

public:
  const MYON_ComponentNameHash32Table& ComponentNameHash32TableConst(
    MYON_ModelComponent::Type component_type
    ) const;

private:
  // Hash table used for model components that require unique names
  // A model component may have the same name as a system component.
  MYON_ComponentNameHash32Table m_unique_name_hash_table;

  // Hash table used for model components that have nonunique names
  MYON_ComponentNameHash32Table m_nonunique_name_hash_table;

  // Hash table used for system components that require unique names
  MYON_ComponentNameHash32Table m_system_name_hash_table;
};

int MYON_ComponentManifestImpl_TableCount(void)
{
  return MYON_ComponentManifestImpl::TableCount;
}

static unsigned int MYON_ComponentManifestImpl_SerialNumberGenerator()
{
  static unsigned int manifest_impl_sn = 0;
  return ++manifest_impl_sn;
}

MYON_ComponentManifestImpl::MYON_ComponentManifestImpl() MYON_NOEXCEPT
  : m_manifest_impl_sn(MYON_ComponentManifestImpl_SerialNumberGenerator())
  , m_manifest_id_hash_table(m_hash_table_item_fsp)
  , m_unique_name_hash_table(m_hash_table_item_fsp)
  , m_nonunique_name_hash_table(m_hash_table_item_fsp)
  , m_system_name_hash_table(m_hash_table_item_fsp)
{
  m_item_PRIVATE__fsp.Create(sizeof(MYON_ComponentManifestItem_PRIVATE),0,0);
  for (unsigned int i = 0; i < TableCount; i++)
  {
    MYON_ModelComponent::Type component_type = MYON_ModelComponent::ComponentTypeFromUnsigned(i);
    if ( MYON_ModelComponent::Type::Unset == component_type)
      continue;
    if ( MYON_ModelComponent::Type::Mixed == component_type)
      continue;
    m_table_index[i].SetComponentType(component_type);
  }
}

MYON_ComponentManifestItem_PRIVATE* MYON_ComponentManifestImpl::AllocateItem()
{
  MYON_ComponentManifestItem_PRIVATE* manifest_item = new (m_item_PRIVATE__fsp.AllocateDirtyElement()) MYON_ComponentManifestItem_PRIVATE();
  manifest_item->m_manifest_impl_sn = m_manifest_impl_sn;
  return manifest_item;
}

MYON_ComponentManifestItem_PRIVATE* MYON_ComponentManifestImpl::AllocateItem(
  const MYON_ComponentManifestItem& item
  )
{
  MYON_ComponentManifestItem_PRIVATE* manifest_item = new (m_item_PRIVATE__fsp.AllocateDirtyElement()) MYON_ComponentManifestItem_PRIVATE(item);
  manifest_item->m_manifest_impl_sn = m_manifest_impl_sn;
  return manifest_item;
}

void MYON_ComponentManifestImpl::DeallocateItem(
  MYON_ComponentManifestItem_PRIVATE* item
  )
{
  if (nullptr != item)
  {
    if (m_manifest_impl_sn != item->m_manifest_impl_sn)
    {
      MYON_ERROR("item memory corruption.");
    }
    else
    {
      item->m_manifest_impl_sn = 0;
      item->m_manifest_table_sn = 0;
      m_item_PRIVATE__fsp.ReturnElement(item);
    }
  }
}

const MYON_wString MYON_ComponentManifestImpl::UnusedName(
  MYON_ModelComponent::Type component_type,
  const wchar_t* candidate_name,
  const wchar_t* base_name,
  const wchar_t* suffix_separator,
  unsigned int suffix0,
  unsigned int* suffix_value
  ) const
{
  if ( nullptr != suffix_value )
    *suffix_value = suffix0;
  if (MYON_UNSET_UINT_INDEX == suffix0)
    suffix0 = ActiveAndDeletedItemCountImpl(component_type);

  if (false == MYON_ModelComponent::ComponentTypeIsValidAndNotMixed(component_type))
  {
    MYON_ERROR("Invalid component_type parameter.");
    return MYON_wString::EmptyString;
  }

  const MYON_ComponentNameHash32Table& hash_table = ComponentNameHash32TableConst(component_type);

  const bool bIgnoreParentId = (false == MYON_ModelComponent::UniqueNameIncludesParent(component_type));
  const bool bIgnoreCase = MYON_ModelComponent::UniqueNameIgnoresCase(component_type);

  MYON_NameHash name_hash;
  MYON_wString unused_component_name(candidate_name);
  unused_component_name.TrimLeftAndRight();
  if (unused_component_name.IsNotEmpty() )
  {
    name_hash = MYON_NameHash::Create(MYON_nil_uuid,candidate_name,bIgnoreCase);
    if ( name_hash.IsValidAndNotEmpty() )
    {
      if ( 
        false == hash_table.NameInUse(component_type, name_hash, bIgnoreParentId)
        && false == m_system_name_hash_table.NameInUse(component_type, name_hash, bIgnoreParentId)
        )
      {
        return unused_component_name;
      }
    }
    else
    {
      MYON_ERROR("Invalid candidate_name parameter.");
    }
    name_hash = MYON_NameHash::EmptyNameHash;
    unused_component_name = MYON_wString::EmptyString;
  }
  
  MYON_wString local_base_name(base_name);
  MYON_wString s;
  if (local_base_name.IsEmpty())
  {
    if (nullptr == base_name)
    {
      local_base_name = candidate_name;
      local_base_name.TrimLeftAndRight();
      local_base_name.TrimRight(L"0123456789 _-");
    }
    if (local_base_name.IsEmpty() || false == MYON_ModelComponent::IsValidComponentName(local_base_name))
      local_base_name = MYON_ModelComponent::ComponentTypeToString(component_type);
  }
  else
  {
    s = local_base_name;
    s += 'X';
    s.TrimLeftAndRight();
    if (s.Length() > 1 && MYON_ModelComponent::IsValidComponentName(s))
    {
      s.SetLength(s.Length()-1);
      local_base_name = s;
    }
    else
    {
      MYON_ERROR("Invalid base_name parameter.");
      local_base_name = MYON_ModelComponent::ComponentTypeToString(component_type);
    }
  }
  
  if (nullptr == suffix_separator || 0 != suffix_separator[0])
  {
    // suffix separator is either the default or needs to be validated.
    if (nullptr != suffix_separator)
    {
      s = local_base_name;
      s += suffix_separator;
      if (s.Length() > local_base_name.Length())
      {
        s += 'X';
        if (MYON_ModelComponent::IsValidComponentName(s))
          local_base_name += suffix_separator;
        else
        {
          MYON_ERROR("Invalid suffix_separator parameter.");
          suffix_separator = nullptr;
        }
      }
    }
    if (nullptr == suffix_separator)
      suffix_separator = L" ";
  }

  const unsigned int index_limit = IndexLimit(component_type);
  const unsigned int item_count = ActiveAndDeletedItemCountImpl(component_type);
  const int max_attempt_count = ((index_limit >= item_count) ? index_limit : item_count) + 20;
  for (int pass = 0; pass < 3; pass++)
  {
    MYON_RandomNumberGenerator RNG;
    if ( 1 == pass)
      RNG.Seed();
    for (int attempt_count = 0; attempt_count < max_attempt_count; attempt_count++)
    {
      switch (pass)
      {
      case 0:
        unused_component_name.Format(L"%ls%ls%02u", static_cast<const wchar_t*>(local_base_name), suffix_separator, ++suffix0);
        break;
      case 1:
        unused_component_name.Format(L"%ls%ls%08X", static_cast<const wchar_t*>(local_base_name), suffix_separator, RNG.RandomNumber());
        break;
      default:
        {
          MYON_wString id;
          MYON_UuidToString(MYON_CreateId(),id);
          id.Remove('-');
          unused_component_name.Format(L"%ls%ls%ls", static_cast<const wchar_t*>(local_base_name), suffix_separator, static_cast<const wchar_t*>(id));
        }
        break;
      }
      name_hash = MYON_NameHash::Create(MYON_nil_uuid, unused_component_name);
      if ( hash_table.NameInUse(component_type, name_hash, bIgnoreParentId) )
        continue;
      if ( m_system_name_hash_table.NameInUse(component_type, name_hash, bIgnoreParentId) )
        continue;
      if (suffix_value)
        *suffix_value = suffix0;
      return unused_component_name;
    }
  }

  // It is likely there is a bug in this function or the manifest searching code.
  MYON_ERROR("Unable to find a unique name.");
  return MYON_wString::EmptyString;
}

MYON_ComponentNameHash32Table& MYON_ComponentManifestImpl::ComponentNameHash32Table(
  MYON_ModelComponent::Type component_type
  )
{
  return
    (MYON_ModelComponent::UniqueNameRequired(component_type) || MYON_ModelComponent::Type::Image == component_type)
    ? m_unique_name_hash_table
    : m_nonunique_name_hash_table;
}
  
const MYON_ComponentNameHash32Table& MYON_ComponentManifestImpl::ComponentNameHash32TableConst(
  MYON_ModelComponent::Type component_type
  ) const
{
  return const_cast<MYON_ComponentManifestImpl*>(this)->ComponentNameHash32Table(component_type);
}

MYON_ComponentManifestTableIndex* MYON_ComponentManifestImpl::TableIndexFromType(
  MYON_ModelComponent::Type component_type
  ) 
{
  return const_cast<MYON_ComponentManifestTableIndex*>(TableIndexFromTypeConst(component_type));
}

const MYON_ComponentManifestTableIndex* MYON_ComponentManifestImpl::TableIndexFromTypeConst(
  MYON_ModelComponent::Type component_type
  ) const
{
  const unsigned int i = static_cast<unsigned int>(component_type);
  if ( i < MYON_ComponentManifestImpl::TableCount )
    return &m_table_index[i];

  MYON_ERROR("component_type is not valid");
  return nullptr;
}

int MYON_ComponentManifestImpl::IndexLimit(
  MYON_ModelComponent::Type component_type
  ) const
{
  const MYON_ComponentManifestTableIndex* table_index = TableIndexFromTypeConst(component_type);
  return (nullptr == table_index) ? 0 : table_index->IndexLimit();
}

unsigned int MYON_ComponentManifestImpl::ItemCountImpl() const
{
  return m_manifest_id_hash_table.ItemCount();
}

unsigned int MYON_ComponentManifestImpl::SystemItemCountImpl(
  MYON_ModelComponent::Type component_type
) const
{
  unsigned int count = 0;
  if (MYON_ModelComponent::ComponentTypeIsValidAndNotMixed(component_type))
  {
    const MYON_ComponentManifestTableIndex* table_index = TableIndexFromTypeConst(component_type);
    count
      = (nullptr == table_index) 
      ? 0
      : table_index->SystemItemCount();
  }
  else
  {
    MYON_ModelComponentTypeIterator type_it = MYON_ModelComponentTypeIterator::ExplicitComponentTypes;
    for (MYON_ModelComponent::Type t = type_it.FirstType(); MYON_ModelComponent::Type::Unset != t; t = type_it.NextType())
    {
      if (MYON_ModelComponent::ComponentTypeIsValidAndNotMixed(t))
       count += SystemItemCountImpl(t);
    }
  }
  return count;
}

unsigned int MYON_ComponentManifestImpl::TotalItemCountImpl(
  MYON_ModelComponent::Type component_type
  ) const
{
  unsigned int count = 0;
  if (MYON_ModelComponent::ComponentTypeIsValidAndNotMixed(component_type))
  {
    const MYON_ComponentManifestTableIndex* table_index = TableIndexFromTypeConst(component_type);
    count
      = (nullptr == table_index) 
      ? 0
      : table_index->TotalItemCount();
  }
  else
  {
    MYON_ModelComponentTypeIterator type_it = MYON_ModelComponentTypeIterator::ExplicitComponentTypes;
    for (MYON_ModelComponent::Type t = type_it.FirstType(); MYON_ModelComponent::Type::Unset != t; t = type_it.NextType())
    {
      if (MYON_ModelComponent::ComponentTypeIsValidAndNotMixed(t))
       count += TotalItemCountImpl(t);
    }
  }
  return count;
}

unsigned int MYON_ComponentManifestImpl::ActiveAndDeletedItemCountImpl(
  MYON_ModelComponent::Type component_type
  ) const
{
  unsigned int count = 0;
  if (MYON_ModelComponent::ComponentTypeIsValidAndNotMixed(component_type))
  {
    const MYON_ComponentManifestTableIndex* table_index = TableIndexFromTypeConst(component_type);
    count
      = (nullptr == table_index) 
      ? 0
      : table_index->ActiveAndDeletedItemCount();
  }
  else
  {
    MYON_ModelComponentTypeIterator type_it = MYON_ModelComponentTypeIterator::ExplicitComponentTypes;
    for (MYON_ModelComponent::Type t = type_it.FirstType(); MYON_ModelComponent::Type::Unset != t; t = type_it.NextType())
    {
      if (MYON_ModelComponent::ComponentTypeIsValidAndNotMixed(t))
       count += ActiveAndDeletedItemCountImpl(t);
    }
  }
  return count;
}

unsigned int MYON_ComponentManifestImpl::ActiveItemCountImpl(
  MYON_ModelComponent::Type component_type
  ) const
{
  unsigned int count = 0;
  if (MYON_ModelComponent::ComponentTypeIsValidAndNotMixed(component_type))
  {
    const MYON_ComponentManifestTableIndex* table_index = TableIndexFromTypeConst(component_type);
    count
      = (nullptr == table_index) 
      ? 0
      : table_index->ActiveItemCount();
  }
  else
  {
    MYON_ModelComponentTypeIterator type_it = MYON_ModelComponentTypeIterator::ExplicitComponentTypes;
    for (MYON_ModelComponent::Type t = type_it.FirstType(); MYON_ModelComponent::Type::Unset != t; t = type_it.NextType())
    {
      if (MYON_ModelComponent::ComponentTypeIsValidAndNotMixed(t))
       count += ActiveItemCountImpl(t);
    }
  }
  return count;
}

unsigned int MYON_ComponentManifestImpl::DeletedItemCountImpl(
  MYON_ModelComponent::Type component_type
  ) const
{
  unsigned int count = 0;
  if (MYON_ModelComponent::ComponentTypeIsValidAndNotMixed(component_type))
  {
    const MYON_ComponentManifestTableIndex* table_index = TableIndexFromTypeConst(component_type);
    count
      = (nullptr == table_index) 
      ? 0
      : table_index->DeletedItemCount();
  }
  else
  {
    MYON_ModelComponentTypeIterator type_it = MYON_ModelComponentTypeIterator::ExplicitComponentTypes;
    for (MYON_ModelComponent::Type t = type_it.FirstType(); MYON_ModelComponent::Type::Unset != t; t = type_it.NextType())
    {
      if (MYON_ModelComponent::ComponentTypeIsValidAndNotMixed(t))
       count += DeletedItemCountImpl(t);
    }
  }
  return count;
}

const MYON_ComponentManifestItem_PRIVATE* MYON_ComponentManifestImpl::AddItem(
  const MYON_ComponentManifestItem& item,
  const MYON_UUID& component_parent_id,
  bool bResolveIdAndNameCollisions,
  const wchar_t* candidate_name,
  MYON_wString* assigned_name
  )
{
  MYON_ComponentManifestItem_PRIVATE* manifest_item = nullptr;
  MYON__UINT64 component_serial_number = 0;

  for (;;)
  {
    const MYON_ModelComponent::Type component_type = item.m_component_type;
    MYON_ComponentManifestTableIndex* table_index = TableIndexFromType(component_type);
    if (nullptr == table_index)
    {
      MYON_ERROR("Invalid component type.");
      break;
    }

    const bool bIndexRequired = MYON_ModelComponent::IndexRequired(component_type);
    const unsigned int assigned_index = IndexLimit(component_type);
    if (bIndexRequired && assigned_index >= 0x7FFFFFFFU)
    {
      MYON_ERROR("unable to assign component index.");
      break;
    }


    MYON_UUID id = item.m_id;
    if ( false == IdIsAvailable(id) )
    {
      if (false == bResolveIdAndNameCollisions)
      {
        MYON_ERROR("component id is in use.");
        break;
      }
      id = MYON_CreateId();
    }

    MYON_NameHash name_hash = item.m_name_hash;

    const bool bIsFilePathHash = (MYON_ModelComponent::Type::Image == component_type && name_hash.IsFilePathHash());
    MYON_wString name;
    const bool bUniqueNameRequired = bIsFilePathHash || MYON_ModelComponent::UniqueNameRequired(component_type);
    if (MYON_ModelComponent::Type::Image == component_type)
    {
      if (false == bIsFilePathHash )
      {
        MYON_ERROR("Embedded files require a non-empty file path name hash.");
        break;
      }
    }
    else
    {
      if (bIsFilePathHash)
      {
        MYON_ERROR("Ordinary components cannot have a file path name hash.");
        break;
      }
    }

    if ( bUniqueNameRequired )
    {
      bool bAssignNewName = false;
      if (false == item.m_name_hash.IsValidAndNotEmpty())
      {
        if (bIsFilePathHash)
        {
          MYON_ERROR("embedded file path is not valid.");
          break;
        }
        if (false == bResolveIdAndNameCollisions)
        {
          MYON_ERROR("component name is not valid.");
          break;
        }
        bAssignNewName = true;
      }
      else if (nullptr != m_unique_name_hash_table.FirstManifestItemWithName(component_type, item.m_name_hash))
      {
        if ( bIsFilePathHash )
        {
          // embedded file - cannot change name
          MYON_ERROR("embedded file name is in use.");
          break;
        }
        if (false == bResolveIdAndNameCollisions)
        {
          MYON_ERROR("component name is in use.");
          break;
        }
        bAssignNewName = true;
      }

      if (bAssignNewName && false == bIsFilePathHash )
      {
        MYON_wString local_candidate_name(candidate_name);
        local_candidate_name.TrimLeftAndRight();
        if (false == MYON_ModelComponent::IsValidComponentName(local_candidate_name))
          local_candidate_name = MYON_wString::EmptyString;
        MYON_wString base_name(local_candidate_name);
        base_name.TrimRight(L"0123456789 _-");
        const MYON_UUID name_parent_id 
          = MYON_ModelComponent::UniqueNameIncludesParent(component_type)
          ? component_parent_id
          : MYON_nil_uuid;
        name = UnusedName(component_type,local_candidate_name,base_name,nullptr,MYON_UNSET_UINT_INDEX,nullptr);
        if ( name.IsEmpty() )
        {
          MYON_ERROR("unable to assign component name.");
          break;
        }
        name_hash = MYON_NameHash::Create(name_parent_id,name);
      }
    }

    MYON_SerialNumberMap::SN_ELEMENT* e = nullptr;
    if (0 != item.m_component_runtime_serial_number)
    {
      component_serial_number = item.m_component_runtime_serial_number;
      e = m_component_serial_number_map.AddSerialNumber(component_serial_number);
      if (nullptr != e->m_value.m_u.ptr)
      {
        MYON_ERROR("item.m_component_runtime_serial_number is already in the manifest.");
        component_serial_number = 0;
        break;
      }
    }
    
    m_manifest_content_version_number++;
    manifest_item = AllocateItem(item);
    manifest_item->m_next = nullptr;
    manifest_item->m_prev = nullptr;
    manifest_item->m_name_hash = name_hash;
    manifest_item->m_id = id;
    if (false == table_index->AddItemAndSetManifestIndex(manifest_item))
      break;

    m_manifest_id_hash_table.AddManifestItem(manifest_item);

    if ( bUniqueNameRequired )
      m_unique_name_hash_table.AddManifestItem(manifest_item);
    else if ( manifest_item->m_name_hash.IsValidAndNotEmpty())
      m_nonunique_name_hash_table.AddManifestItem(manifest_item);

    if ( nullptr != assigned_name )
      *assigned_name = name;

    if (nullptr != e)
    {
      e->m_value.m_u_type = static_cast<unsigned int>(manifest_item->m_component_type);
      e->m_value.m_u.ptr = (void*)manifest_item;
    }

    return manifest_item;
  }

  // failure
  if (nullptr != manifest_item)
    DeallocateItem(manifest_item);

  if ( nullptr != assigned_name )
    *assigned_name = MYON_wString::EmptyString;

  if (0 != component_serial_number)
  {
    MYON_SerialNumberMap::SN_ELEMENT* e = m_component_serial_number_map.RemoveSerialNumberAndId(component_serial_number);
    if (nullptr != e)
    {
      e->m_value.m_u_type = 0;
      e->m_value.m_u.u64 = 0;
    }
  }

  return nullptr;
}

const MYON_ComponentManifestItem_PRIVATE* MYON_ComponentManifestImpl::ChangeItemNameHash(
  const MYON_UUID& manifest_item_id,
  const MYON_NameHash& new_name_hash
  )
{
  if (new_name_hash.IsInvalidNameHash())
  {
    MYON_ERROR("new_name_hash parameter is not valid.");
    return nullptr;
  }

  MYON_ComponentManifestItem_PRIVATE* item = const_cast<MYON_ComponentManifestItem_PRIVATE*>(ItemFromManifestId(manifest_item_id));

  if (nullptr == item)
  {
    MYON_ERROR("id not valid.");
    return nullptr;
  }

  const MYON_NameHash old_name_hash = item->m_name_hash;
  
  if ( old_name_hash == new_name_hash )
    return item;
  
  const bool bUniqueNameRequired = MYON_ModelComponent::UniqueNameRequired(item->m_component_type);
  if (bUniqueNameRequired)
  {
    if (false == new_name_hash.IsValidAndNotEmpty())
    {
      MYON_ERROR("new_name_hash must be valid and not empty.");
      return nullptr;
    }
    const MYON_ComponentManifestHash32TableItem* hash_item = m_unique_name_hash_table.FirstManifestItemWithName(item->m_component_type,new_name_hash);
    if (nullptr != hash_item )
    {
      MYON_ERROR("new name is in use.");
      return nullptr;
    }
  }

  MYON_ComponentNameHash32Table& name_hash_table = ComponentNameHash32Table(item->m_component_type);

  if (old_name_hash.IsValidAndNotEmpty())
  {
    if (false == name_hash_table.RemoveManifestItem(item))
    {
      MYON_ERROR("old name not in manifest.");
    }
  }

  m_manifest_content_version_number++;
  item->m_name_hash = new_name_hash;
  if ( new_name_hash.IsValidAndNotEmpty() )
    name_hash_table.AddManifestItem(item);

  return item;
}

const MYON_ComponentManifestItem_PRIVATE* MYON_ComponentManifestImpl::ChangeItemComponentRuntimeSerialNumber(
  const MYON_ComponentManifestItem_PRIVATE* manifest_item,
  MYON__UINT64 new_component_runtime_serial_number
  )
{
  if (nullptr == manifest_item)
  {
    MYON_ERROR("manifest_item is nullptr.");
    return nullptr;
  }

  if (this->m_manifest_impl_sn != manifest_item->m_manifest_impl_sn )
  {
    MYON_ERROR("manifest_item is corrupt.");
    return nullptr;
  }

  if ( manifest_item->m_component_runtime_serial_number == new_component_runtime_serial_number )
    return manifest_item;

  MYON_SerialNumberMap::SN_ELEMENT* e;
  if (0 != new_component_runtime_serial_number)
  {
    e = m_component_serial_number_map.FindSerialNumber(new_component_runtime_serial_number);
    if (nullptr != e)
    {
      MYON_ERROR("new_component_runtime_serial_number is already in the manifest.");
      return nullptr;
    }
  }

  MYON_SerialNumberMap::SN_ELEMENT e0;
  memset(&e0, 0, sizeof(e0));
  if (0 != manifest_item->m_component_runtime_serial_number)
  {
    e = m_component_serial_number_map.RemoveSerialNumberAndId(manifest_item->m_component_runtime_serial_number);
    if (nullptr == e)
    {
      MYON_ERROR("component not in m_component_serial_number_map.");
    }
    else
    {
      e0 = *e;
      if (e->m_value.m_u.ptr != manifest_item)
      {
        MYON_ERROR("m_component_serial_number_map id corrupt.");
      }
    }
  }

  e = m_component_serial_number_map.AddSerialNumber(new_component_runtime_serial_number);
  if (nullptr != e->m_value.m_u.ptr)
  {
    MYON_ERROR("new_component_runtime_serial_number is already in the manifest.");
    return nullptr;
  }
  e->m_value.m_u.ptr = (void*)manifest_item;
  const_cast<MYON_ComponentManifestItem_PRIVATE*>(manifest_item)->m_component_runtime_serial_number = new_component_runtime_serial_number;

  m_manifest_content_version_number++;

  return manifest_item;
}

bool MYON_ComponentManifestImpl::RemoveItem(
  const MYON_ComponentManifestItem_PRIVATE* manifest_item
  )
{
  if ( nullptr == manifest_item )
    return false;
  
  if (this->m_manifest_impl_sn != manifest_item->m_manifest_impl_sn )
  {
    MYON_ERROR("manifest_item is corrupt.");
    return false;
  }

  MYON_ComponentManifestItem_PRIVATE* item = const_cast<MYON_ComponentManifestItem_PRIVATE*>(manifest_item);
  m_manifest_content_version_number++;
  MYON_ComponentManifestTableIndex* table_index = TableIndexFromType(item->m_component_type);
  if (nullptr != table_index)
    table_index->RemoveItem(item);
  if ( 0 != item->m_component_runtime_serial_number )
    m_component_serial_number_map.RemoveSerialNumberAndId(item->m_component_runtime_serial_number);
  if (MYON_UuidIsNotNil(item->m_id))
    m_manifest_id_hash_table.RemoveManifestItem(item);
  if (item->m_name_hash.IsValidAndNotEmpty())
   ComponentNameHash32Table(item->m_component_type).RemoveManifestItem(item);
  DeallocateItem(item);
  return true;
}

bool MYON_ComponentManifestImpl::RemoveAllItems(
  MYON_ModelComponent::Type component_type,
  bool bResetManifestIndex
  )
{
  MYON_ComponentManifestTableIndex* table_index = TableIndexFromType(component_type);
  if (nullptr == table_index)
    return false;
  MYON_ComponentManifestItem_PRIVATE* next_item = const_cast<MYON_ComponentManifestItem_PRIVATE*>(table_index->FirstItem());
  MYON_ComponentNameHash32Table& name_hash_table = ComponentNameHash32Table(component_type);
  while (nullptr != next_item)
  {
    MYON_ComponentManifestItem_PRIVATE* item = next_item;
    next_item = next_item->m_next;
    if ( 0 != item->m_component_runtime_serial_number )
      m_component_serial_number_map.RemoveSerialNumberAndId(item->m_component_runtime_serial_number);
    if (MYON_UuidIsNotNil(item->m_id))
      m_manifest_id_hash_table.RemoveManifestItem(item);
    if (item->m_name_hash.IsValidAndNotEmpty())
      name_hash_table.RemoveManifestItem(item);
    DeallocateItem(item);
  }
  table_index->RemoveAllItems(bResetManifestIndex);
  return true;
}

const MYON_ComponentManifestItem_PRIVATE* MYON_ComponentManifestImpl::DeleteItem(
  const MYON_ComponentManifestItem_PRIVATE* manifest_item
  )
{
  if ( nullptr == manifest_item )
    return nullptr;

  if ( manifest_item->IsSystemComponent() )
  {
    MYON_ERROR("cannot delete system components.");
    return nullptr;
  }

  if (m_manifest_impl_sn != manifest_item->m_manifest_impl_sn)
  {
    MYON_ERROR("manifest_item is corrupt.");
    return nullptr;
  }

  MYON_ComponentManifestItem_PRIVATE* item = const_cast<MYON_ComponentManifestItem_PRIVATE*>(manifest_item);
  if ( nullptr == item )
    return nullptr;
  if (item->IsDeleted())
    return item;
  if (item->m_name_hash.IsValidAndNotEmpty())
  {
    // remove name;
    if (MYON_ModelComponent::UniqueNameRequired(item->m_component_type))
    {
      m_unique_name_hash_table.RemoveManifestItem(item);
      if ( MYON_ModelComponent::UniqueNameIncludesParent(item->m_component_type) )
        item->m_name_hash = MYON_NameHash::CreateIdAndUnsetName(item->m_name_hash.ParentId());
      else
        item->m_name_hash = MYON_NameHash::UnsetNameHash;
    }
    else
    {
      m_nonunique_name_hash_table.RemoveManifestItem(item);
      item->m_name_hash = MYON_NameHash::EmptyNameHash;
    }
  }
  
  item->Internal_SetDeletedState(true);
  if (item->IsDeleted())
  {
    MYON_ComponentManifestTableIndex* table_index = TableIndexFromType(item->ComponentType());
    if (nullptr != table_index)
      table_index->IncrementDeletedCount();
  }

  return item;
}

const MYON_ComponentManifestItem_PRIVATE* MYON_ComponentManifestImpl::UndeleteItem(
  const MYON_ComponentManifestItem_PRIVATE* manifest_item,
  const MYON_UUID& parent_id,
  const wchar_t* candidate_name,
  MYON_wString& assigned_name
  )
{
  MYON_wString name(candidate_name);
  assigned_name = MYON_wString::EmptyString;

  if ( nullptr == manifest_item )
    return nullptr;

  if (m_manifest_impl_sn != manifest_item->m_manifest_impl_sn)
  {
    MYON_ERROR("manifest_item is corrupt");
    return nullptr;
  }

  MYON_ComponentManifestItem_PRIVATE* item = const_cast<MYON_ComponentManifestItem_PRIVATE*>(manifest_item);

  const MYON_UUID name_parent_id
    = MYON_ModelComponent::UniqueNameRequired(item->m_component_type)
    ? parent_id
    : MYON_nil_uuid;

  name.TrimLeftAndRight();
  MYON_NameHash name_hash = MYON_NameHash::Create(name_parent_id,name);
  if (false == name_hash.IsValidAndNotEmpty())
    name_hash = MYON_NameHash::EmptyNameHash;

  if (item->IsDeleted())
  {
    MYON_ComponentManifestTableIndex* table_index = TableIndexFromType(item->ComponentType());
    if (nullptr != table_index)
      table_index->DecrementDeletedCount();
  }
  else if (name_hash == item->m_name_hash)
  {
    assigned_name = name;
    return item;
  }

  item->Internal_SetDeletedState(false);

  if (MYON_ModelComponent::UniqueNameRequired(item->m_component_type))
  {
    if (name_hash.IsValidAndNotEmpty()
      && nullptr == m_unique_name_hash_table.FirstManifestItemWithName(item->m_component_type, name_hash))
    {
      assigned_name = name;
    }
    else
    {
      // need a unique name
      assigned_name = UnusedName(
        item->m_component_type,
        candidate_name,
        nullptr,
        nullptr,
        MYON_UNSET_UINT_INDEX,
        nullptr
        );
      name_hash = MYON_NameHash::Create(name_parent_id,assigned_name);
    }
    item->m_name_hash = name_hash;
    m_unique_name_hash_table.AddManifestItem(item);
  }
  else 
  {
    m_nonunique_name_hash_table.RemoveManifestItem(item);
    assigned_name = name;
    item->m_name_hash = name_hash;
    m_nonunique_name_hash_table.AddManifestItem(item);
  }


  return item;
}

const MYON_ComponentManifestItem_PRIVATE* MYON_ComponentManifestImpl::SystemItemFromIndex(
  MYON_ModelComponent::Type component_type,
  int system_item_index
  ) const
{
 const  MYON_ComponentManifestTableIndex* table_index = TableIndexFromTypeConst(component_type);
  if (nullptr != table_index)
    return table_index->SystemItemFromIndex(system_item_index);
  return nullptr;
}

const MYON_ComponentManifestItem_PRIVATE* MYON_ComponentManifestImpl::ItemFromManifestId(
  const MYON_UUID& manifest_item_id
  ) const
{
  if ( MYON_nil_uuid == manifest_item_id )
    return nullptr;
  const MYON_ComponentManifestHash32TableItem* hash_item = m_manifest_id_hash_table.FirstManifestItemWithId(manifest_item_id);
  return (nullptr != hash_item) ? hash_item->m_manifest_item : nullptr;
}

const MYON_ComponentManifestItem_PRIVATE* MYON_ComponentManifestImpl::ItemFromNameHash(
  MYON_ModelComponent::Type component_type,
  const MYON_NameHash& component_name_hash
  ) const
{
  const MYON_ComponentNameHash32Table& name_hash_table = ComponentNameHash32TableConst(component_type);
  const MYON_ComponentManifestHash32TableItem* hash_item = name_hash_table.FirstManifestItemWithName(component_type,component_name_hash);
  return (nullptr != hash_item) ? hash_item->m_manifest_item : nullptr;
}

const MYON_ComponentManifestItem_PRIVATE* MYON_ComponentManifestImpl::SystemItemFromNameHash(
  MYON_ModelComponent::Type component_type,
  const MYON_NameHash& system_item_name_hash
  ) const
{
  const MYON_ComponentManifestHash32TableItem* hash_item = m_system_name_hash_table.FirstManifestItemWithName(component_type,system_item_name_hash);
  return (nullptr != hash_item) ? hash_item->m_manifest_item : nullptr;
}

const MYON_ComponentManifestItem_PRIVATE* MYON_ComponentManifestImpl::ItemFromComponentRuntimeSerialNumber(
  MYON__UINT64 component_runtime_serial_number
  ) const
{
  const MYON_SerialNumberMap::SN_ELEMENT* e = m_component_serial_number_map.FindSerialNumber(component_runtime_serial_number);
  if ( nullptr == e )
    return nullptr;
  const MYON_ComponentManifestItem_PRIVATE* item = (const MYON_ComponentManifestItem_PRIVATE*)e->m_value.m_u.ptr;
  if (nullptr == item || item->m_component_runtime_serial_number != component_runtime_serial_number )
  {
    MYON_ERROR("m_component_serial_number_map is corrupt.");
    return nullptr;
  }
  return item;
}


const MYON_ComponentManifestItem_PRIVATE* MYON_ComponentManifestImpl::ItemFromManifestIndex(
  MYON_ModelComponent::Type component_type,
  int manifest_item_index
   ) const
{
  if (manifest_item_index >= 0)
  {
    const MYON_ComponentManifestTableIndex* table_index = TableIndexFromTypeConst(component_type);
    if (nullptr != table_index)
      return table_index->ItemFromManifestItemIndex(manifest_item_index);
  }
  return nullptr;
}


const class MYON_ComponentManifestItem_PRIVATE* MYON_ComponentManifestImpl::FirstItem(
  MYON_ModelComponent::Type component_type
  ) const
{
  const MYON_ComponentManifestTableIndex* table = TableIndexFromTypeConst(component_type); 
  return (nullptr != table) ? table->FirstItem() : nullptr;
}

const class MYON_ComponentManifestItem_PRIVATE* MYON_ComponentManifestImpl::LastItem(
  MYON_ModelComponent::Type component_type
  ) const
{
  const MYON_ComponentManifestTableIndex* table = TableIndexFromTypeConst(component_type); 
  return (nullptr != table) ? table->LastItem() : nullptr;
}

const class MYON_ComponentManifestItem_PRIVATE* MYON_ComponentManifestImpl::NextItem(
  const class MYON_ComponentManifestItem* item
  ) const
{
  const MYON_ComponentManifestItem_PRIVATE* manifest_item = ManagedItemFromItem(item);
  return (nullptr != manifest_item ) ? manifest_item->m_next : nullptr;
}

const class MYON_ComponentManifestItem_PRIVATE* MYON_ComponentManifestImpl::PreviousItem(
  const class MYON_ComponentManifestItem* item
  ) const
{
  const MYON_ComponentManifestItem_PRIVATE* manifest_item = ManagedItemFromItem(item);
  return (nullptr != manifest_item ) ? manifest_item->m_prev : nullptr;
}

MYON_ComponentManifest::MYON_ComponentManifest() MYON_NOEXCEPT
{
  // explicity implementation to work around a bug in Apple's CLANG
}

MYON_ComponentManifest::~MYON_ComponentManifest()
{
  Reset();
}

class MYON_ComponentManifestImpl* MYON_ComponentManifest::Impl() const
{
  if ( nullptr == m_impl )
    m_impl = new MYON_ComponentManifestImpl();
  return m_impl;
}


void MYON_ComponentManifest::Reset()
{
  if (nullptr != m_impl)
  {
    delete m_impl;
    m_impl = nullptr;
  }
}

MYON__UINT64 MYON_ComponentManifest::ManifestContentVersionNumber() const
{
  return 
    (nullptr != m_impl)
    ? m_impl->ManifestContentVersionNumber()
    : 0;
}

unsigned int MYON_ComponentManifest::ItemCount() const
{
  return (nullptr == m_impl) ? 0 : m_impl->ItemCountImpl();
}

unsigned int MYON_ComponentManifest::ActiveComponentCount(
  MYON_ModelComponent::Type component_type
  ) const
{
  return (nullptr == m_impl) ? 0 : m_impl->ActiveItemCountImpl(component_type);
}

unsigned int MYON_ComponentManifest::TotalComponentCount(
  MYON_ModelComponent::Type component_type
) const
{
  return (nullptr == m_impl) ? 0 : m_impl->TotalItemCountImpl(component_type);
}

unsigned int MYON_ComponentManifest::ActiveAndDeletedComponentCount(
  MYON_ModelComponent::Type component_type
  ) const
{
  return (nullptr == m_impl) ? 0 : m_impl->ActiveAndDeletedItemCountImpl(component_type);
}

unsigned int MYON_ComponentManifest::DeletedComponentCount(
  MYON_ModelComponent::Type component_type
  ) const
{
  return (nullptr == m_impl) ? 0 : m_impl->DeletedItemCountImpl(component_type);
}

unsigned int MYON_ComponentManifest::SystemComponentCount(
  MYON_ModelComponent::Type component_type
) const
{
  return (nullptr == m_impl) ? 0 : m_impl->SystemItemCountImpl(component_type);
}

int MYON_ComponentManifest::ComponentIndexLimit(
  MYON_ModelComponent::Type component_type
  ) const
{
  return (nullptr == m_impl) ? 0 : m_impl->IndexLimit(component_type);
}

const class MYON_ComponentManifestItem* MYON_ComponentManifest::Internal_AddItem(
  class MYON_ComponentManifestItem& item,
  MYON_UUID component_parent_id,
  bool bResolveIdAndNameCollisions,
  const wchar_t* candidate_name,
  MYON_wString* assigned_name
  )
{
  if (MYON_ModelComponent::Type::Unset == item.ComponentType())
  {
    if (nullptr != assigned_name )
      *assigned_name = MYON_wString::EmptyString;
    return nullptr;
  }

  return Impl()->AddItem(
    item,
    component_parent_id,
    bResolveIdAndNameCollisions,
    candidate_name,
    assigned_name
    );
}


const class MYON_ComponentManifestItem& MYON_ComponentManifest::AddComponentToManifest(
  MYON_ModelComponent::Type component_type,
  MYON__UINT64 component_serial_number,
  MYON_UUID manifest_id,
  const MYON_NameHash& component_name_hash
  )
{
  MYON_ComponentManifestItem item(
      component_type,
      component_serial_number,
      manifest_id,
      component_name_hash
      );
  const bool bResolveIdAndNameCollisions = false;
  const MYON_ComponentManifestItem* manifest_item = Internal_AddItem(
    item,
    component_name_hash.ParentId(),
    bResolveIdAndNameCollisions,
    nullptr,
    nullptr
    );
  return (nullptr != manifest_item) ? *manifest_item : MYON_ComponentManifestItem::UnsetItem;
}

const class MYON_ComponentManifestItem& MYON_ComponentManifest::AddComponentToManifest(
  MYON_ModelComponent::Type component_type,
  MYON__UINT64 component_serial_number,
  MYON_UUID component_parent_id,
  MYON_UUID original_id,
  const wchar_t* original_name,
  MYON_wString& assigned_name
  )
{
  MYON_UUID name_parent_id
    = MYON_ModelComponent::UniqueNameIncludesParent(component_type)
    ? component_parent_id
    : MYON_nil_uuid;
  return AddComponentToManifest(
    component_type,
    component_serial_number,
    component_parent_id,
    original_id,
    MYON_NameHash::Create(name_parent_id,original_name),
    original_name,
    assigned_name
    );
}


const class MYON_ComponentManifestItem& MYON_ComponentManifest::AddComponentToManifest(
  MYON_ModelComponent::Type component_type,
  MYON__UINT64 component_serial_number,
  MYON_UUID component_parent_id,
  MYON_UUID component_id,
  const MYON_NameHash& component_name_hash,
  const wchar_t* candidate_name,
  MYON_wString& assigned_name
  )
{
  MYON_ComponentManifestItem item(
      component_type,
      component_serial_number,
      component_id,
      component_name_hash
      );
  const bool bResolveIdAndNameCollisions = true;
  const MYON_ComponentManifestItem* manifest_item = Internal_AddItem(
    item,
    component_parent_id,
    bResolveIdAndNameCollisions,
    candidate_name,
    &assigned_name
    );
  return (nullptr != manifest_item) ? *manifest_item : MYON_ComponentManifestItem::UnsetItem;
}

const MYON_ComponentManifestItem_PRIVATE* MYON_ComponentManifestImpl::AddSystemItem(
  const MYON_ComponentManifestItem& item
  )
{
  MYON_ComponentManifestItem_PRIVATE* manifest_item = nullptr;
  MYON__UINT64 component_serial_number = 0;

  for (;;)
  {
    const MYON_ModelComponent::Type component_type = item.m_component_type;
    MYON_ComponentManifestTableIndex* table_index = TableIndexFromType(component_type);
    if (nullptr == table_index)
    {
      MYON_ERROR("Invalid component type.");
      break;
    }

    if (0 == item.m_component_runtime_serial_number)
    {
      MYON_ERROR("Invalid system component runtime serial number.");
      break;
    }

    const MYON_UUID id = item.m_id;
    if (MYON_UuidIsNil(id))
    {
      MYON_ERROR("Invalid system component id.");
      break;
    }

    if ( false == IdIsAvailable(id) )
    {
      MYON_ERROR("component id is in use.");
      break;
    }
    
    if (MYON_ModelComponent::IndexRequired(component_type))
    {
      if (item.m_index >= 0 || item.m_index <= MYON_UNSET_INT_INDEX )
      {
        MYON_ERROR("index must negative and set.");
        break;
      }
    }
    else
    {
      if (MYON_UNSET_INT_INDEX != item.m_index)
      {
        MYON_ERROR("index must be unset.");
        break;
      }
    }

    const MYON_NameHash name_hash = item.m_name_hash;
    if (MYON_UuidIsNotNil(name_hash.ParentId()))
    {
      MYON_ERROR("system component name cannot have parent id.");
      break;
    }

    const bool bUniqueNameRequired = MYON_ModelComponent::UniqueNameRequired(component_type);
    if (bUniqueNameRequired)
    {
      if (false == name_hash.IsValidAndNotEmpty())
      {
        MYON_ERROR("name must be set.");
        break;
      }
      if (nullptr != m_system_name_hash_table.FirstManifestItemWithName(component_type, name_hash))
      {
        MYON_ERROR("system component name must be unique.");
        break;
      }
    }
    else
    {
      if (false == name_hash.IsEmptyNameHash())
      {
        MYON_ERROR("name must be empty.");
        break;
      }
    }

    component_serial_number = item.m_component_runtime_serial_number;
    MYON_SerialNumberMap::SN_ELEMENT* e = m_component_serial_number_map.AddSerialNumber(component_serial_number);
    if (nullptr != e->m_value.m_u.ptr)
    {
      MYON_ERROR("item.m_component_runtime_serial_number is already in the manifest.");
      component_serial_number = 0;
      break;
    }
    
    m_manifest_content_version_number++;
    manifest_item = AllocateItem(item);
    manifest_item->m_next = nullptr;
    manifest_item->m_prev = nullptr;
    manifest_item->m_name_hash = name_hash;
    manifest_item->m_id = id;
    manifest_item->m_status_bits = 0x02U;
    if (false == table_index->AddSystemItem(manifest_item))
    {
      break;
    }

    m_manifest_id_hash_table.AddManifestItem(manifest_item);

    if ( bUniqueNameRequired )
      m_system_name_hash_table.AddManifestItem(manifest_item);

    e->m_value.m_u_type = static_cast<unsigned int>(manifest_item->m_component_type);
    e->m_value.m_u.ptr = (void*)manifest_item;

    return manifest_item;
  }

  // failure
  if (nullptr != manifest_item)
    DeallocateItem(manifest_item);

  if (0 != component_serial_number)
  {
    MYON_SerialNumberMap::SN_ELEMENT* e = m_component_serial_number_map.RemoveSerialNumberAndId(component_serial_number);
    if (nullptr != e)
    {
      e->m_value.m_u_type = 0;
      e->m_value.m_u.u64 = 0;
    }
  }

  return nullptr;
}

const MYON_ComponentManifestItem& MYON_ComponentManifest::AddSystemComponentToManifest(
  const class MYON_ModelComponent& component
  )
{
  if (false == component.IsSystemComponent())
  {
    MYON_ERROR("component cannot be a system component.");
    return MYON_ComponentManifestItem::UnsetItem;
  }

  const MYON_ComponentManifestItem system_item(component);

  const MYON_ComponentManifestItem_PRIVATE* manifest_item = Impl()->AddSystemItem(system_item);

  return 
    (nullptr != manifest_item)
    ? *manifest_item
    : MYON_ComponentManifestItem::UnsetItem;
}

const MYON_ComponentManifestItem& MYON_ComponentManifest::AddComponentToManifest(
  const class MYON_ModelComponent& component,
  bool bResolveIdAndNameCollisions,
  MYON_wString* assigned_name
  )
{
  MYON_ModelComponent::Type component_type = component.ComponentType();
  MYON__UINT64 component_serial_number = component.RuntimeSerialNumber();
  MYON_UUID component_id = component.Id();
  MYON_UUID component_parent_id = component.ParentId();

  const MYON_Bitmap* embedded_file = nullptr;
  bool bIsEmbeddedFile = MYON_ModelComponent::Type::Image == component_type && nullptr != (embedded_file =MYON_Bitmap::Cast(&component));

  const MYON_NameHash component_name_hash 
    = (bIsEmbeddedFile)
    ? MYON_NameHash::CreateFilePathHash(embedded_file->FileReference())
    : component.NameHash();

  if (bResolveIdAndNameCollisions)
  {
    // modify component identification information as required
    MYON_wString manifest_assigned_name;
    const class MYON_ComponentManifestItem& manifest_item = AddComponentToManifest(
      component_type,
      component_serial_number,
      component_parent_id,
      component_id,
      component_name_hash,
      component.NameAsPointer(),
      manifest_assigned_name
      );
    if (nullptr != assigned_name)
    {
      if ( manifest_item.NameHash() == component_name_hash )
        *assigned_name = component.Name();
      else
        *assigned_name = manifest_assigned_name;
    }
    return manifest_item;
  }

  // do not modify component identification information
  if ( nullptr != assigned_name )
    *assigned_name = component.Name();
  return AddComponentToManifest(
    component_type,
    component_serial_number,
    component_id,
    component_name_hash
    );
}

bool MYON_ComponentManifest::RemoveComponent(
  const MYON_ModelComponent& component
  )
{
  return RemoveComponent(component.RuntimeSerialNumber());
}

bool MYON_ComponentManifest::RemoveComponent(
  MYON__UINT64 component_runtime_serial_number
)
{
  if ( nullptr == m_impl )
    return false;
  const MYON_ComponentManifestItem_PRIVATE* manifest_item = m_impl->ItemFromComponentRuntimeSerialNumber(component_runtime_serial_number);
  if ( nullptr == manifest_item )
    return false;
  return m_impl->RemoveItem(manifest_item);
}

bool MYON_ComponentManifest::RemoveComponent(
  MYON_UUID manifest_id
  )
{
  if ( nullptr == m_impl )
    return false;
  const MYON_ComponentManifestItem_PRIVATE* manifest_item = m_impl->ItemFromManifestId(manifest_id);
  if ( nullptr == manifest_item )
    return false;
  return m_impl->RemoveItem(manifest_item);
}

bool MYON_ComponentManifest::RemoveIndexedComponent(
  MYON_ModelComponent::Type component_type,
  int manifest_item_index
  )
{
  if ( nullptr == m_impl )
    return false;
  const MYON_ComponentManifestItem_PRIVATE* manifest_item = m_impl->ItemFromManifestIndex(component_type,manifest_item_index);
  if ( nullptr == manifest_item )
    return false;
  return m_impl->RemoveItem(manifest_item);
}

bool MYON_ComponentManifest::RemoveAllComponents(
  MYON_ModelComponent::Type component_type,
  bool bResetManifestIndex
  )
{
  if ( nullptr == m_impl )
    return false;
  return m_impl->RemoveAllItems(component_type,bResetManifestIndex);
}


const class MYON_ComponentManifestItem& MYON_ComponentManifest::ChangeComponentName(
  const class MYON_ModelComponent& component
)
{
  return ChangeComponentName(
    component.Id(),
    component.ComponentType(),
    component.ParentId(),
    component.Name()
  );
}

const MYON_ComponentManifestItem& MYON_ComponentManifest::ChangeComponentName(
  MYON_UUID manifest_item_id,
  MYON_ModelComponent::Type component_type,
  MYON_UUID component_parent_id,
  const wchar_t* name
  )
{
  if (nullptr == m_impl)
    return MYON_ComponentManifestItem::UnsetItem;

  MYON_UUID name_parent_id
    = MYON_ModelComponent::UniqueNameIncludesParent(component_type)
    ? component_parent_id
    : MYON_nil_uuid;
  const bool bIgnoreCase = MYON_ModelComponent::UniqueNameIgnoresCase(component_type);

  const MYON_NameHash new_name_hash = MYON_NameHash::Create(name_parent_id,name,bIgnoreCase);
  
  const MYON_ComponentManifestItem_PRIVATE* manifest_item = m_impl->ChangeItemNameHash(
    manifest_item_id,
    new_name_hash
    );

  return (nullptr != manifest_item) ? *manifest_item : MYON_ComponentManifestItem::UnsetItem;
}

const MYON_ComponentManifestItem& MYON_ComponentManifest::ChangeComponentNameHash(
  MYON_UUID manifest_item_id,
  const MYON_NameHash& component_name_hash
  )
{
  if (nullptr == m_impl)
    return MYON_ComponentManifestItem::UnsetItem;

  const MYON_ComponentManifestItem_PRIVATE* manifest_item = m_impl->ChangeItemNameHash(
    manifest_item_id,
    component_name_hash
    );

  return (nullptr != manifest_item) ? *manifest_item : MYON_ComponentManifestItem::UnsetItem;
}

const MYON_ComponentManifestItem& MYON_ComponentManifest::DeleteComponent(
  MYON_UUID manifest_item_id
  )
{
  if (nullptr == m_impl)
    return MYON_ComponentManifestItem::UnsetItem;

  const MYON_ComponentManifestItem_PRIVATE* manifest_item = m_impl->DeleteItem(
    m_impl->ItemFromManifestId(manifest_item_id)
    );

  return (nullptr != manifest_item) ? *manifest_item : MYON_ComponentManifestItem::UnsetItem;
}


const MYON_ComponentManifestItem& MYON_ComponentManifest::DeleteComponent(
  MYON__UINT64 component_runtime_serial_number
  )
{
  if (nullptr == m_impl)
    return MYON_ComponentManifestItem::UnsetItem;

  const MYON_ComponentManifestItem_PRIVATE* manifest_item = m_impl->DeleteItem(
    m_impl->ItemFromComponentRuntimeSerialNumber(component_runtime_serial_number)
    );

  return (nullptr != manifest_item) ? *manifest_item : MYON_ComponentManifestItem::UnsetItem;
}

const MYON_ComponentManifestItem& MYON_ComponentManifest::UndeleteComponent(
  MYON_UUID manifest_item_id,
  MYON_UUID parent_id,
  const wchar_t* candidate_name,
  MYON_wString& assigned_name
  )
{
  if (nullptr == m_impl)
    return MYON_ComponentManifestItem::UnsetItem;

  const MYON_ComponentManifestItem_PRIVATE* manifest_item = m_impl->UndeleteItem(
    m_impl->ItemFromManifestId(manifest_item_id),
    parent_id,
    candidate_name,
    assigned_name
    );

  return (nullptr != manifest_item) ? *manifest_item : MYON_ComponentManifestItem::UnsetItem;
}

const class MYON_ComponentManifestItem& MYON_ComponentManifest::UndeleteComponentAndChangeRuntimeSerialNumber(
  MYON_UUID item_id,
  MYON_UUID parent_id,
  MYON__UINT64 new_component_runtime_serial_number,
  const wchar_t* candidate_name,
  MYON_wString& assigned_name
)
{
  if (0 != new_component_runtime_serial_number)
  {
    const MYON_ComponentManifestItem& item_from_sn = ItemFromComponentRuntimeSerialNumber(new_component_runtime_serial_number);
    if (item_from_sn.IsValid() && item_from_sn.Id() != item_id)
    {
      MYON_ERROR("new_component_runtime_serial_number in use.");
      return MYON_ComponentManifestItem::UnsetItem;
    }
  }
  const MYON_ComponentManifestItem& manifest_item = UndeleteComponent(
    item_id,
    parent_id,
    candidate_name,
    assigned_name
  );
  if ( manifest_item.IsValid() 
    && item_id == manifest_item.Id()
    && new_component_runtime_serial_number != manifest_item.ComponentRuntimeSerialNumber() 
    )
  {
    this->ChangeComponentRuntimeSerialNumber(
      item_id,
      new_component_runtime_serial_number
    );
  }
  return manifest_item;
}

const class MYON_ComponentManifestItem& MYON_ComponentManifest::ChangeComponentRuntimeSerialNumber(
  MYON_UUID manifest_item_id,
  MYON__UINT64 component_runtime_serial_number
  )
{
  if (nullptr == m_impl)
    return MYON_ComponentManifestItem::UnsetItem;

  const MYON_ComponentManifestItem_PRIVATE* manifest_item = m_impl->ChangeItemComponentRuntimeSerialNumber(
    m_impl->ItemFromManifestId(manifest_item_id),
    component_runtime_serial_number
    );

  return (nullptr != manifest_item) ? *manifest_item : MYON_ComponentManifestItem::UnsetItem;
}

bool MYON_ComponentManifest::NameIsAvailable(
  MYON_ModelComponent::Type component_type,
  const MYON_NameHash& candidate_name_hash
  ) const
{
  if (false == candidate_name_hash.IsValidAndNotEmpty() )
    return false;

  if (nullptr == m_impl)
    return true;

  return m_impl->NameIsAvailable(component_type,candidate_name_hash);
}

const MYON_wString MYON_ComponentManifest::UnusedName(
  const MYON_ModelComponent& model_component
  ) const
{
  return UnusedName(
    model_component.ComponentType(),
    model_component.ParentId(),
    model_component.NameAsPointer(),
    nullptr,
    nullptr,
    0,
    nullptr
  );
}

const MYON_wString MYON_ComponentManifest::UnusedName(
  MYON_ModelComponent::Type component_type,
  MYON_UUID component_parent_id,
  const wchar_t* candidate_name,
  const wchar_t* base_name,
  const wchar_t* suffix_separator,
  unsigned int suffix0,
  unsigned int* suffix_value
  ) const
{
  if ( nullptr != suffix_value )
    *suffix_value = suffix0;

  if (false == MYON_ModelComponent::ComponentTypeIsValidAndNotMixed(component_type))
  {
    MYON_ERROR("Invalid component_type parameter.");
    return MYON_wString::EmptyString;
  }

  if (
    MYON_ModelComponent::UniqueNameIncludesParent(component_type)
    && MYON_UuidIsNotNil(component_parent_id)
    && nullptr != candidate_name
    && 0 != candidate_name[0]
    )
  {
    // See if candidate name  with parent specified is valid and not in use.
    MYON_wString local_candidate_name(candidate_name);
    local_candidate_name.TrimLeftAndRight();
    const bool bIgnoreCase = MYON_ModelComponent::UniqueNameIgnoresCase(component_type);
    MYON_NameHash candidate_name_hash = MYON_NameHash::Create(component_parent_id,local_candidate_name,bIgnoreCase);
    if (candidate_name_hash.IsValidAndNotEmpty())
    {
      if (ItemFromNameHash(component_type, candidate_name_hash).IsUnset())
        return local_candidate_name;
    }
    MYON_wString local_base_name(base_name);
    local_base_name.TrimLeftAndRight();
    if (local_base_name.IsEmpty())
    {
      base_name = candidate_name;
    }
    candidate_name = nullptr;
  }

  return Impl()->UnusedName(
    component_type,
    candidate_name,
    base_name,
    suffix_separator,
    suffix0,
    suffix_value
    );
}


MYON_UUID MYON_ComponentManifest::UnusedId(
  MYON_UUID candidate_id
  ) const
{
  return
     ( !(MYON_nil_uuid == candidate_id) && IdIsAvailable(candidate_id))
     ? candidate_id
     : MYON_CreateId();
}

bool MYON_ComponentManifest::IdIsAvailable(
  MYON_UUID id
  ) const
{
  if (MYON_nil_uuid == id)
    return false;
  if ( nullptr == m_impl )
    return true;
  return m_impl->IdIsAvailable(id);
}

const class MYON_ComponentManifestItem& MYON_ComponentManifest::SystemItemFromNameHash(
  MYON_ModelComponent::Type component_type,
  const MYON_NameHash& system_item_name_hash
  ) const
{
  const MYON_ComponentManifestItem_PRIVATE* item
    = (nullptr != m_impl)
    ? m_impl->SystemItemFromNameHash(component_type,system_item_name_hash)
    : nullptr;
  return
    (nullptr != item )
    ? *item
    : MYON_ComponentManifestItem::UnsetItem;
}


const class MYON_ComponentManifestItem& MYON_ComponentManifest::SystemItemFromIndex(
  MYON_ModelComponent::Type component_type,
  int system_item_index
  ) const
{
  const MYON_ComponentManifestItem_PRIVATE* item
    = (nullptr != m_impl)
    ? m_impl->SystemItemFromIndex(component_type,system_item_index)
    : nullptr;
  return
    (nullptr != item )
    ? *item
    : MYON_ComponentManifestItem::UnsetItem;
}

const class MYON_ComponentManifestItem& MYON_ComponentManifest::ItemFromComponentRuntimeSerialNumber(
  MYON__UINT64 component_runtime_serial_number
  ) const
{
  const MYON_ComponentManifestItem_PRIVATE* item
    = (nullptr != m_impl)
    ? m_impl->ItemFromComponentRuntimeSerialNumber(component_runtime_serial_number)
    : nullptr;
  return
    (nullptr != item )
    ? *item
    : MYON_ComponentManifestItem::UnsetItem;
}

const class MYON_ComponentManifestItem& MYON_ComponentManifest::ItemFromId(
  MYON_UUID manifest_item_id
  ) const
{
  const MYON_ComponentManifestItem_PRIVATE* item
    = (nullptr != m_impl)
    ? m_impl->ItemFromManifestId(manifest_item_id)
    : nullptr;
  return
    (nullptr != item )
    ? *item
    : MYON_ComponentManifestItem::UnsetItem;
}

const class MYON_ComponentManifestItem& MYON_ComponentManifest::ItemFromId(
  MYON_ModelComponent::Type component_type,
  MYON_UUID manifest_item_id
  ) const
{
  const MYON_ComponentManifestItem& manifest_item = ItemFromId(manifest_item_id);
  if ( manifest_item.ComponentType() == component_type )
    return manifest_item;
  return MYON_ComponentManifestItem::UnsetItem;
}

const class MYON_ComponentManifestItem& MYON_ComponentManifest::ItemFromName(
  const class MYON_ModelComponent* model_component
) const
{
  return
    (nullptr != model_component)
    ? MYON_ComponentManifest::ItemFromNameHash(model_component->ComponentType(), model_component->NameHash())
    : MYON_ComponentManifestItem::UnsetItem;
}

const class MYON_ComponentManifestItem& MYON_ComponentManifest::ItemFromName(
  MYON_ModelComponent::Type component_type,
  MYON_UUID parent_id,
  const wchar_t* name
) const
{
  const bool bIgnoreCase = MYON_ModelComponent::UniqueNameIgnoresCase(component_type);
  const MYON_UUID parent_id_or_nil
    = MYON_ModelComponent::UniqueNameIncludesParent(component_type)
    ? parent_id
    : MYON_nil_uuid;
  const MYON_NameHash component_name_hash = MYON_NameHash::Create(parent_id_or_nil, name, bIgnoreCase);
  return MYON_ComponentManifest::ItemFromNameHash(component_type, component_name_hash);
}

const class MYON_ComponentManifestItem& MYON_ComponentManifest::ItemFromNameHash(
  MYON_ModelComponent::Type component_type,
  const MYON_NameHash& component_name_hash
  ) const
{
  const MYON_ComponentManifestItem_PRIVATE* item
    = (nullptr != m_impl)
    ? m_impl->ItemFromNameHash(component_type,component_name_hash)
    : nullptr;
  return
    (nullptr != item )
    ? *item
    : MYON_ComponentManifestItem::UnsetItem;
}


const class MYON_ComponentManifestItem& MYON_ComponentManifest::ItemFromIndex(
  MYON_ModelComponent::Type component_type,
  int manifest_item_index
  ) const
{
  const MYON_ComponentManifestItem_PRIVATE* item
    = (nullptr != m_impl)
    ? m_impl->ItemFromManifestIndex(component_type,manifest_item_index)
    : nullptr;
  return
    (nullptr != item )
    ? *item
    : MYON_ComponentManifestItem::UnsetItem;
}

const class MYON_ComponentManifestItem& MYON_ComponentManifest::ItemFromUnsignedIndex(
  MYON_ModelComponent::Type component_type,
  unsigned int unsigned_manifest_item_index
  ) const
{
  return 
    (unsigned_manifest_item_index <= 2147483647 )
    ? ItemFromIndex(component_type,(int)unsigned_manifest_item_index)
    : MYON_ComponentManifestItem::UnsetItem;
}

const class MYON_ComponentManifestItem* MYON_ComponentManifest::FirstItem(
  MYON_ModelComponent::Type component_type
  ) const
{
  return
    (nullptr != m_impl)
    ? m_impl->FirstItem(component_type)
    : nullptr;
}

const class MYON_ComponentManifestItem* MYON_ComponentManifest::LastItem(
  MYON_ModelComponent::Type component_type
  ) const
{
  return
    (nullptr != m_impl)
    ? m_impl->LastItem(component_type)
    : nullptr;
}


const class MYON_ComponentManifestItem* MYON_ComponentManifest::NextItem(
  const class MYON_ComponentManifestItem* item
  ) const
{
  return
    (nullptr != m_impl)
    ? m_impl->NextItem(item)
    : nullptr;
}

const class MYON_ComponentManifestItem* MYON_ComponentManifest::PreviousItem(
  const class MYON_ComponentManifestItem* item
  ) const
{
  return
    (nullptr != m_impl)
    ? m_impl->PreviousItem(item)
    : nullptr;
}

const class MYON_ComponentManifestItem* MYON_ComponentManifest::NextItem(
  MYON_UUID manifest_item_id
  ) const
{
  const MYON_ComponentManifestItem_PRIVATE* item
    = (nullptr != m_impl)
    ? m_impl->ItemFromManifestId(manifest_item_id)
    : nullptr;
  return
    (nullptr != item )
    ? item->m_next
    : nullptr;
}

const class MYON_ComponentManifestItem* MYON_ComponentManifest::PreviousItem(
  MYON_UUID manifest_item_id
  ) const
{
  const MYON_ComponentManifestItem_PRIVATE* item
    = (nullptr != m_impl)
    ? m_impl->ItemFromManifestId(manifest_item_id)
    : nullptr;
  return
    (nullptr != item )
    ? item->m_prev
    : nullptr;
}

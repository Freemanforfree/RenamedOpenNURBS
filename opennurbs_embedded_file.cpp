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

class MYON_EmbeddedFile::CImpl final
{
public:
  class Data final
  {
  public:
    Data() { }
    Data(const Data& d) { *this = d; }

    const Data& operator = (const Data& d);

    void SetLength(size_t len);

  public:
    std::unique_ptr<MYON__UINT8[]> m_buffer;
    size_t m_length = 0;
    size_t m_compressed_length = 0;
  };

  bool LoadFile(const wchar_t* filename);
  bool SaveFile(const wchar_t* filename) const;

public:
  MYON_wString m_orig_file;
  Data m_data;
};

const MYON_EmbeddedFile::CImpl::Data& MYON_EmbeddedFile::CImpl::Data::operator = (const Data& d)
{
  if (this == &d)
    return *this;

  SetLength(d.m_length);

  m_compressed_length = d.m_compressed_length;

  if (m_buffer)
  {
    memcpy(m_buffer.get(), d.m_buffer.get(), m_length);
  }
  else
  {
    m_length = m_compressed_length = 0;
  }

  return *this;
}

void MYON_EmbeddedFile::CImpl::Data::SetLength(size_t len)
{
  if (0 != len)
  {
    m_buffer = std::unique_ptr<MYON__UINT8[]>(new MYON__UINT8[len]);
  }
  else
  {
    m_buffer = nullptr;
  }

  m_length = len;
}

bool MYON_EmbeddedFile::CImpl::LoadFile(const wchar_t* filename)
{
  auto& d = m_data;

  // Open the file.
  FILE* pFile = MYON_FileStream::Open(filename, L"rb");
  if (nullptr == pFile)
    return false;

  // Get the length of the file data.
  MYON_FileStream::SeekFromEnd(pFile, 0);
  const size_t data_length = size_t(MYON_FileStream::CurrentPosition(pFile));
  MYON_FileStream::SeekFromStart(pFile, 0);

  // Allocate a buffer for the file data.
  d.SetLength(data_length);

  // Read the file data into the buffer.
  const bool bOK = (MYON_FileStream::Read(pFile, d.m_length, d.m_buffer.get()) == d.m_length);

  // Close the file.
  MYON_FileStream::Close(pFile);

  return bOK;
}

bool MYON_EmbeddedFile::CImpl::SaveFile(const wchar_t* filename) const
{
  if (0 == m_data.m_length)
    return false; // Not loaded.

  // Open the file for writing.
  FILE* pFile = MYON_FileStream::Open(filename, L"wb");
  if (nullptr == pFile)
    return false;

  // Write the buffer to the file.
  if (MYON_FileStream::Write(pFile, m_data.m_length, m_data.m_buffer.get()) != m_data.m_length)
    return false;

  // Close the file.
  MYON_FileStream::Close(pFile);

  return true;
}

MYON_OBJECT_IMPLEMENT(MYON_EmbeddedFile, MYON_ModelComponent, "E3BBE02E-F3D5-490D-9719-E21D8BF982EF");

MYON_EmbeddedFile::MYON_EmbeddedFile()
  :
  MYON_ModelComponent(MYON_ModelComponent::Type::EmbeddedFile)
{
  m_impl = new CImpl;
}

MYON_EmbeddedFile::MYON_EmbeddedFile(const MYON_EmbeddedFile& ef)
  :
  MYON_ModelComponent(MYON_ModelComponent::Type::EmbeddedFile, ef)
{
  m_impl = new CImpl;

  *this = ef;
}

MYON_EmbeddedFile::~MYON_EmbeddedFile()
{
  Clear();

  delete m_impl;
  m_impl = nullptr;
}

const MYON_EmbeddedFile& MYON_EmbeddedFile::operator = (const MYON_EmbeddedFile& ef)
{
  if (&ef != this)
  {
    m_impl->m_orig_file = ef.m_impl->m_orig_file;
    m_impl->m_data = ef.m_impl->m_data;
  }

  return *this;
}

MYON_wString MYON_EmbeddedFile::Filename(void) const
{
  return m_impl->m_orig_file;
}

void MYON_EmbeddedFile::SetFilename(const wchar_t* filename)
{
  m_impl->m_orig_file = filename;
}

bool MYON_EmbeddedFile::LoadFromFile(const wchar_t* filename)
{
  Clear();

  m_impl->m_orig_file = MYON_FileSystemPath::CleanPath(filename);

  if (m_impl->m_orig_file.IsEmpty())
    return false;

  if (!m_impl->LoadFile(m_impl->m_orig_file))
    return false;

  return true;
}

bool MYON_EmbeddedFile::SaveToFile(const wchar_t* filename) const
{
  const MYON_wString file = MYON_FileSystemPath::CleanPath(filename);
  if (!m_impl->SaveFile(file))
    return false;

  return true;
}

bool MYON_EmbeddedFile::LoadFromBuffer(MYON_Buffer& buf)
{
  Clear();

  // Allocate a buffer for the data.
  auto& d = m_impl->m_data;
  d.SetLength(buf.Size());

  // Load the buffer from 'buf'.
  if (buf.Read(d.m_length, d.m_buffer.get()) != d.m_length)
    return false;

  return true;
}

bool MYON_EmbeddedFile::SaveToBuffer(MYON_Buffer& buf) const
{
  // Write the data to 'buf'.
  buf.Write(m_impl->m_data.m_length, m_impl->m_data.m_buffer.get());

  return true;
}

bool MYON_EmbeddedFile::Read(MYON_BinaryArchive& archive)
{
  Clear();

  // Read the full file path of the original file.
  MYON_wString filename;
  if (!archive.ReadString(filename))
    return false;

  m_impl->m_orig_file = MYON_FileSystemPath::CleanPath(filename);

  // Read the original (uncompressed) size of the compressed buffer.
  size_t uncompressed_size = 0;
  if (!archive.ReadCompressedBufferSize(&uncompressed_size))
    return false;

  // Allocate a buffer for the uncompressed data.
  auto& d = m_impl->m_data;
  d.SetLength(uncompressed_size);

  // Read the compressed buffer and uncompress it into uncompressed_buffer.
  bool bFailedCRC = false;

  const MYON__UINT64 pos_before = archive.CurrentPosition();

  if (!archive.ReadCompressedBuffer(uncompressed_size, d.m_buffer.get(), &bFailedCRC) && !bFailedCRC)
      return false;

  d.m_compressed_length = archive.CurrentPosition() - pos_before;

  return true;
}

bool MYON_EmbeddedFile::Write(MYON_BinaryArchive& archive) const
{
  auto& d = m_impl->m_data;

  // Write the original filename to the archive.
  if (!archive.WriteString(m_impl->m_orig_file))
    return false;

  // Write the temp file data to the archive.
  if (!archive.WriteCompressedBuffer(d.m_length, d.m_buffer.get()))
    return false;

  return true;
}

size_t MYON_EmbeddedFile::Length(void) const
{
  return m_impl->m_data.m_length;
}

size_t MYON_EmbeddedFile::CompressedLength(void) const
{
  return m_impl->m_data.m_compressed_length;
}

bool MYON_EmbeddedFile::Clear(void)
{
  m_impl->m_orig_file.Empty();

  m_impl->m_data.SetLength(0);
  m_impl->m_data.m_compressed_length = 0;

  return true;
}

const MYON_EmbeddedFile* MYON_EmbeddedFile::FromModelComponentRef(const MYON_ModelComponentReference& ref,
                                                              const MYON_EmbeddedFile* none_return_value) // Static.
{
  const auto* ef = MYON_EmbeddedFile::Cast(ref.ModelComponent());
  if (nullptr != ef)
    return ef;

  return none_return_value;
}

// This MYON_Buffer stuff was already in file opennurbs_embedded_file.cpp even though it has nothing to do with files.
// I need this file for actual EMBEDDED FILE code so I'm hijacking it. This MYON_Buffer stuff should be moved.

MYON_Buffer::MYON_Buffer()
: m_buffer_size(0)
, m_current_position(0)
, m_first_segment(0)
, m_last_segment(0)
, m_current_segment(0)
, m_error_handler(0)
, m_last_error(0)
{
  memset(m_reserved,0,sizeof(m_reserved));
}


void MYON_Buffer::Destroy()
{
  ChangeSize(0);
}

void MYON_Buffer::EmergencyDestroy()
{
  m_buffer_size = 0;
  m_current_position = 0;
  m_first_segment = 0;
  m_last_segment = 0;
  m_current_segment = 0;
  m_error_handler = 0;
  m_last_error = 0;
}

struct MYON_BUFFER_SEGMENT
{
  struct MYON_BUFFER_SEGMENT* m_prev_segment;
  struct MYON_BUFFER_SEGMENT* m_next_segment;
  MYON__UINT64 m_segment_position0; // postion of first byte in this segment
  MYON__UINT64 m_segment_position1; // position of the first byte in the next segment
                                  // When a segment is the last one in an MYON_Buffer,
                                  // is is common for m_segment_position1 > m_buffer_size.
  unsigned char* m_segment_buffer; // null or an array of length (m_segment_position1 - m_segment_position0)
  void* m_reserved;
};

int MYON_Buffer::Compare( const MYON_Buffer& a, const MYON_Buffer& b )
{
  if ( &a == &b )
    return 0;
  if ( a.m_buffer_size < b.m_buffer_size )
    return -1;
  if ( a.m_buffer_size > b.m_buffer_size )
    return 1;

  struct MYON_BUFFER_SEGMENT* aseg = a.m_first_segment;
  struct MYON_BUFFER_SEGMENT* bseg = b.m_first_segment;
  const MYON__UINT64 buffer_size = a.m_buffer_size;
  MYON__UINT64 size = 0;
  size_t aoffset = 0;
  size_t boffset = 0;
  size_t asegsize = 0;
  size_t bsegsize = 0;  
  size_t asize = 0;
  size_t bsize = 0;  
  size_t sz;
  int rc = 0;

  while ( 0 != aseg && 0 != bseg && size < buffer_size )
  {
    if ( 0 == asegsize )
    {
      if ( aseg->m_segment_position0 >= aseg->m_segment_position1 )
      {
        aseg = aseg->m_next_segment;
        continue;
      }
      asegsize = (size_t)(aseg->m_segment_position1 - aseg->m_segment_position0);
      aoffset = 0;
    }

    if ( 0 == bsegsize )
    {
      if ( bseg->m_segment_position0 >= bseg->m_segment_position1 )
      {
        bseg = bseg->m_next_segment;
        continue;
      }
      bsegsize = (size_t)(bseg->m_segment_position1 - bseg->m_segment_position0);
      boffset = 0;
    }
    
    if ( aoffset >= asegsize )
    {
      asegsize = 0;
      aseg = aseg->m_next_segment;
      continue;
    }

    if ( boffset >= bsegsize )
    {
      bsegsize = 0;
      bseg = bseg->m_next_segment;
      continue;
    }

    if ( 0 == aseg->m_segment_buffer )
    {
      return (0 == bseg->m_segment_buffer) ? 0 : -1;
    }

    if ( 0 == bseg->m_segment_buffer )
    {
      return 1;
    }

    asize = asegsize - aoffset;
    bsize = bsegsize - boffset;
    sz = (asize <= bsize) ? asize : bsize;
    if ( size + sz > buffer_size )
      sz = (size_t)(buffer_size - size);
    rc = memcmp( aseg->m_segment_buffer + aoffset, bseg->m_segment_buffer + boffset, (size_t)sz );
    if ( 0 != rc )
      return ((rc<0)?-1:1);
    aoffset += sz;
    boffset += sz;
    size += sz;
  }

  return 0;
}


MYON_Buffer::~MYON_Buffer()
{
  ChangeSize(0); // frees all heap and zeros everything but m_current_position.
  m_current_position = 0;
}

MYON_Buffer::MYON_Buffer( const MYON_Buffer& src )
: m_buffer_size(0)
, m_current_position(0)
, m_first_segment(0)
, m_last_segment(0)
, m_current_segment(0)
, m_error_handler(0)
, m_last_error(0)
{
  memset(m_reserved,0,sizeof(m_reserved));
  Copy(src);
}

MYON_Buffer& MYON_Buffer::operator=( const MYON_Buffer& src )
{
  if ( this != &src )
  {
    ChangeSize(0); // frees all heap and zeros everything but m_current_position.
    m_current_position = 0;
    Copy(src);
  }
  return *this;
}

bool MYON_Buffer::Seek( MYON__INT64 offset, int origin )
{
  MYON__UINT64 pos0, pos1;

  switch(origin)
  {
  case 0: // Seek from beginning of start.
    pos0 = 0;
    break;

  case 1: // Seek from current position.
    pos0 = m_current_position;
    break;

  case 2: // Seek from end.
    pos0 = m_buffer_size;
    break;

  default:
    {
      MYON_ERROR("Invalid origin parameter");
      return false;
    }
    break;
  }

  if ( offset < 0 )
  {
    if ( pos0 < (MYON__UINT64)(-offset) )
    {
      // current position cannot be negative
      MYON_ERROR("Attempt to seek before start of buffer.");
      return false; 
    }    
    pos1 = pos0 - (MYON__UINT64)(-offset); // overflow cannot happen in this operation
  }
  else if ( offset > 0 )
  {
    // current position can be >= m_buffer_size
    pos1 = pos0 + (MYON__UINT64)(offset); // overflow is possible in this operation
    if ( pos1 <= pos0 )
    {
      // overflow
      MYON_ERROR("Attempt to seek to a position that is too large for 64-bit unsigned int storage.");
      return false; 
    }
  }
  else
  {
    pos1 = pos0;
  }

  if ( pos1 != m_current_position )
  {
    m_current_position = pos1;
    m_current_segment = 0;
  }

  return true;
}

bool MYON_Buffer::SeekFromStart( MYON__INT64 offset )
{
  return Seek(offset,0);
}

bool MYON_Buffer::SeekFromCurrentPosition( MYON__INT64 offset )
{
  return Seek(offset,1);
}

bool MYON_Buffer::SeekFromEnd( MYON__INT64 offset )
{
  return Seek(offset,2);
}

bool MYON_Buffer::Compact()
{
  bool rc = false;
  if ( 0 == m_buffer_size )
  {
    rc = ChangeSize(0); // frees all heap and zeros everything but m_current_position.
    m_current_segment = 0;
  }
  else if ( 0 != m_last_segment 
            && m_buffer_size > m_last_segment->m_segment_position0
            && m_buffer_size <= m_last_segment->m_segment_position1
            )
  {
    if ( m_buffer_size == m_last_segment->m_segment_position1 )
      rc = true;
    else
    {
      MYON__UINT64 sizeof_segment_buffer = m_buffer_size - m_last_segment->m_segment_position0;
      struct MYON_BUFFER_SEGMENT* prev_segment = m_last_segment->m_prev_segment;
      void* last_buffer = ( 0 != m_last_segment->m_segment_buffer && m_last_segment->m_segment_buffer != (unsigned char*)(m_last_segment+1) )
                        ? m_last_segment->m_segment_buffer
                        : 0;
      struct MYON_BUFFER_SEGMENT* new_last_segment = (struct MYON_BUFFER_SEGMENT*)onrealloc(m_last_segment,sizeof(*m_last_segment) + ((size_t)sizeof_segment_buffer)); // sizeof_segment_buffer always < 0xFFFFFFFF
      if ( 0 != new_last_segment )
      {
        if ( new_last_segment != m_last_segment || 0 != last_buffer )
        {
          new_last_segment->m_segment_buffer = (unsigned char*)(new_last_segment+1);
          if ( 0 != last_buffer )
          {
            memcpy(new_last_segment->m_segment_buffer,last_buffer,(size_t)sizeof_segment_buffer);
            onfree(last_buffer);
            last_buffer = 0;
          }
          new_last_segment->m_prev_segment = prev_segment;
          new_last_segment->m_next_segment = 0;
          if ( m_first_segment == m_last_segment )
            m_first_segment = new_last_segment;
          if ( m_current_segment == m_last_segment )
            m_current_segment = new_last_segment;
          m_last_segment = new_last_segment;
          if ( 0 != prev_segment )
          {
            prev_segment->m_next_segment = m_last_segment;
          }
        }
        m_last_segment->m_segment_position1 = m_buffer_size;
        rc = true;
      }
    }
  }
  return true;
}

bool MYON_Buffer::ChangeSize(MYON__UINT64 buffer_size)
{
  if ( buffer_size <= 0 )
  {
    struct MYON_BUFFER_SEGMENT* p0 = m_last_segment;
    struct MYON_BUFFER_SEGMENT* p1 = 0;
    m_buffer_size = 0;
    m_first_segment = 0;
    m_last_segment = 0;
    m_current_segment = 0;

    // free in reverse order of allocation
    while ( 0 != p0 )
    {
      p1 = p0->m_prev_segment;
      if ( 0 != p0->m_segment_buffer && (void*)(p0->m_segment_buffer) != (void*)(p0+1) )
        onfree(p0->m_segment_buffer);
      onfree(p0);
      p0 = p1;
    }
  }
  else if ( buffer_size < m_buffer_size )
  {
    m_current_segment = 0;

    if ( 0 == m_first_segment || 0 == m_last_segment )
    {
      MYON_ERROR("Corrupt MYON_Buffer");
      return false;
    }

    while ( 0 != m_last_segment )
    {
      if ( m_last_segment->m_segment_position0 < buffer_size )
      {
        if ( buffer_size > m_last_segment->m_segment_position1 )
        {
          MYON_ERROR("Corrupt MYON_Buffer.");
          // Set m_buffer_size and m_last_segment to valid values
          // to prevent possible crashes if the return code is
          // ignored.
          if ( m_buffer_size > m_last_segment->m_segment_position1 )
            m_buffer_size = m_last_segment->m_segment_position1;
          m_last_segment->m_next_segment = 0;
          if ( m_current_position > m_buffer_size )
            m_current_position = m_buffer_size;
          return false;
        }
        if ( 0 != m_last_segment->m_segment_buffer && m_last_segment->m_segment_position1 > buffer_size )
        {
          memset(m_last_segment->m_segment_buffer + (buffer_size - m_last_segment->m_segment_position0),
                 0,
                 (size_t)(m_last_segment->m_segment_position1 - buffer_size)
                 );
        }
        m_buffer_size = buffer_size;
        break;
      }
      struct MYON_BUFFER_SEGMENT* p = m_last_segment->m_prev_segment;
      if ( 0 != p )
        p->m_next_segment = 0;
      if ( 0 != m_last_segment->m_segment_buffer && (void*)(m_last_segment->m_segment_buffer) != (void*)(m_last_segment+1) )
        onfree(m_last_segment->m_segment_buffer);
      onfree(m_last_segment);
      m_last_segment = p;
    }
  }
  else if ( buffer_size > m_buffer_size )
  {
    // save current position;
    const MYON__UINT64 saved_pos = CurrentPosition();
    if ( SeekFromStart(buffer_size-1) )
    {
      // calling Write with the current position at buffer_size-1
      // will pad with zeros from offset m_buffer_size to 
      // offset buffer_size-2, write a zero at offset buffer_size-1,
      // and set m_buffer_size to buffer size.
      const unsigned char zero_byte = 0;
      Write(1,&zero_byte);
    }
    // restore current position.
    SeekFromStart(saved_pos);
  }

  return (buffer_size == m_buffer_size);
}

void MYON_Buffer::Copy( const MYON_Buffer& src )
{
  const struct MYON_BUFFER_SEGMENT* src_seg;
  struct MYON_BUFFER_SEGMENT* dst_seg;
  for ( src_seg = src.m_first_segment; 0 != src_seg; src_seg = src_seg->m_next_segment )
  {
    if (    m_buffer_size != src_seg->m_segment_position0 
         || src_seg->m_segment_position0 >= src.m_buffer_size
        )
    {
      MYON_ERROR("Attempt to copy corrupt source.");
      break;
    }
    if ( src_seg->m_segment_position0 >= src_seg->m_segment_position1 
        )
    {
      MYON_ERROR("Attempt to copy corrupt source.");
      continue;
    }
    MYON__UINT64 segment_buffer_size = ( 0 != src_seg->m_segment_buffer)
                               ? src_seg->m_segment_position1 - src_seg->m_segment_position0
                               : 0;
    dst_seg = (struct MYON_BUFFER_SEGMENT*)onmalloc(sizeof(*dst_seg) + ((size_t)segment_buffer_size) );
    memset(dst_seg,0,sizeof(*dst_seg));

    if ( segment_buffer_size > 0 )
    {
      dst_seg->m_segment_buffer = (unsigned char*)(dst_seg+1);
      memcpy( dst_seg->m_segment_buffer, src_seg->m_segment_buffer, (size_t)segment_buffer_size ); // segment_buffer_size always < 0xFFFFFFFF
    }

    if ( 0 == m_first_segment )
      m_first_segment = dst_seg;
    dst_seg->m_prev_segment = m_last_segment;
    if ( 0 != m_last_segment )
      m_last_segment->m_next_segment = dst_seg;
    m_last_segment = dst_seg;
    dst_seg->m_segment_position0 = src_seg->m_segment_position0;
    dst_seg->m_segment_position1 = src_seg->m_segment_position1;
    m_buffer_size = (src.m_buffer_size < dst_seg->m_segment_position1)
                  ? src.m_buffer_size
                  : dst_seg->m_segment_position1;
  }
  if ( src.m_current_position <= m_buffer_size )
    m_current_position = src.m_current_position;
  // 27 June, 2001 Dale Lear: Should this copy m_last_error and m_error_handler? Not sure.
}

static bool MYON_Buffer_IsNotValid()
{
  return false;
}

bool MYON_Buffer::IsValid( const MYON_TextLog* text_log ) const
{
  // This function is primarily used to discover bugs
  // in the MYON_Buffer member function code.

  if ( 0 == m_buffer_size )
  {

    if ( 0 != m_first_segment )
      return MYON_Buffer_IsNotValid();
    if ( 0 != m_last_segment )
      return MYON_Buffer_IsNotValid();
    if ( 0 != m_current_segment )
      return MYON_Buffer_IsNotValid();

    return true;
  }


  if ( 0 == m_first_segment )
    return MYON_Buffer_IsNotValid();
  if ( 0 != m_first_segment->m_prev_segment )
    return MYON_Buffer_IsNotValid();
  if ( 0 == m_last_segment )
    return MYON_Buffer_IsNotValid();
  if ( 0 != m_last_segment->m_next_segment )
    return MYON_Buffer_IsNotValid();

  bool bCurrentSegInList = (0 == m_current_segment);
  MYON__UINT64 pos = 0;
  MYON__UINT64 u;
  const struct MYON_BUFFER_SEGMENT* prev_seg = 0;
  const struct MYON_BUFFER_SEGMENT* seg;
  for ( seg = m_first_segment; seg != 0; seg = seg->m_next_segment )
  {
    if ( prev_seg != seg->m_prev_segment )
      return MYON_Buffer_IsNotValid();
    if ( 0 != prev_seg && prev_seg->m_segment_position1 != seg->m_segment_position0 )
      return MYON_Buffer_IsNotValid();
    if ( seg->m_segment_position1 <= seg->m_segment_position0 )
      return MYON_Buffer_IsNotValid();
    if ( pos != seg->m_segment_position0 )
      return MYON_Buffer_IsNotValid();

    if ( m_current_segment == seg )
      bCurrentSegInList = true;

    // pos checks prevent infinite loop when the linked list has a cycle;
    u = pos + (seg->m_segment_position1 - seg->m_segment_position0);
    if ( pos >= u )
      return MYON_Buffer_IsNotValid(); // addition wrapped value
    pos = u;
    prev_seg = seg;
  }

  if ( m_last_segment != prev_seg )
    return MYON_Buffer_IsNotValid();

  if ( pos < m_buffer_size )
    return MYON_Buffer_IsNotValid();

  if (    m_buffer_size <= m_last_segment->m_segment_position0 
       || m_buffer_size > m_last_segment->m_segment_position1 
     )
    return MYON_Buffer_IsNotValid();

  return true;
}

bool MYON_Buffer::AtEnd() const
{
  return (m_current_position == m_buffer_size);
}

MYON__UINT64 MYON_Buffer::Size() const
{
  return m_buffer_size;
}

MYON__UINT32 MYON_Buffer::CRC32( MYON__UINT32 current_remainder ) const
{
  MYON__UINT64 size, seg_size;
  const struct MYON_BUFFER_SEGMENT* prev_seg;
  const struct MYON_BUFFER_SEGMENT* seg;
  const struct MYON_BUFFER_SEGMENT* seg0 = 0;

  size = 0;
  for ( seg = m_first_segment; 0 != seg; seg = seg->m_next_segment )
  {
    // prev_seg is set this way so that the error handling
    // code can use continue statments for non-fatal errors.
    prev_seg = seg0;
    seg0 = seg;

    if ( seg->m_segment_position0 > seg->m_segment_position1 )
    {
      // This is really bad!  If you can determine how the corruption occurs,
      // plase make a bug report and tell Dale Lear as soon as possible.
      MYON_ERROR("corrupt buffer - segment's position values are invalid.");
      continue;
    }

    if ( 0 == prev_seg )
    {
      if ( 0 != seg->m_segment_position0 )
      {
        // The first segment should have seg->m_segment_position0 = 0.
        // We'll keep going after the call to MYON_ERROR.
        //
        // If you can determine how the corruption occured, please
        // make a bug report and assign it to Dale Lear.
        MYON_ERROR("corrupt buffer - first segment has non-zero value for position0.");
      }
    }
    else if ( prev_seg->m_segment_position1 != seg->m_segment_position0 )
    {
      // Every segment after the first should have 
      // seg->m_segment_position0 = previous_segment->m_segment_position1.
      // We'll keep going after the call to MYON_ERROR.
      //
      // If you can determine how the corruption occured, please
      // make a bug report and assign it to Dale Lear.
      MYON_ERROR("corrupt buffer - previous segment's position1 !- segment's position0.");
    }

    seg_size = seg->m_segment_position1 - seg->m_segment_position0;

    if ( 0 == seg_size )
    {
      // If you can determine how the corruption occured, please
      // make a bug report and assign it to Dale Lear.
      MYON_ERROR("corrupt buffer - empty segment buffer.");
      continue;
    }
    
    if ( seg_size + size > m_buffer_size )
    {
      if ( seg != m_last_segment || seg->m_next_segment )
      {
        // If you can determine how the corruption occured, please
        // make a bug report and assign it to Dale Lear.
        MYON_ERROR("corrupt buffer - segments contain more bytes than m_buffer_size.");
      }
      seg_size = m_buffer_size - size;
    }

    current_remainder = MYON_CRC32(current_remainder,(size_t)seg_size,seg->m_segment_buffer);
    size += seg_size;
    if ( size >= m_buffer_size )
    {
      if ( seg != m_last_segment || 0 != seg->m_next_segment || size > m_buffer_size )
      {
        // If you can determine how the corruption occured, please
        // make a bug report and assign it to Dale Lear.
        MYON_ERROR("corrupt buffer - list of segments is too long.");
      }
      break;
    }
  }

  return current_remainder;
}


MYON__UINT64 MYON_Buffer::CurrentPosition() const
{
  return m_current_position;
}

bool MYON_Buffer::SetCurrentSegment( bool bWritePending )
{
  // When MYON_Buffer::Write() needs to write at least on byted, it 
  // calls MYON_Buffer::SetCurrentSegment(true).
  //   In this case true is returned in all cases unless the information
  //   in the MYON_Buffer class is corrupt.
  // When MYON_Buffer::Read() needs to read a at least one byte, it
  // calls MYON_Buffer::SetCurrentSegment(false).  
  //   In this case, true is returned when m_current_position < m_buffer_size
  //   and false is returned in all other cases.
  //
  // If seeks have occured since the last read or write, m_current_segment
  // and m_current_segment_offset may need to be updated.
  //

  if ( 0 == m_current_segment )
    m_current_segment = (m_current_position <= m_buffer_size/2) ? m_first_segment : m_last_segment;

  if ( !bWritePending && m_current_position >= m_buffer_size )
  {
    m_current_segment = 0;
    return false; // cannot read past end of buffer
  }

  if ( 0 != m_current_segment 
       && m_current_segment->m_segment_position0 <= m_current_position 
       && m_current_position < m_current_segment->m_segment_position1 
     )
  {
    // The current position is inside of m_current_segment.
    // This happens most of the time which is why this code is at the top
    // of this function.
    return true;
  }

  if ( 0 == m_first_segment )
  {
    // m_current_position can be > 0 if we are writing
    m_current_segment = 0;
    return bWritePending;
  }

  if ( 0 == m_last_segment )
  {
    m_current_segment = 0;
    MYON_ERROR("Corrupt MYON_Buffer");
    return false;
  }

  if ( m_current_position >= m_last_segment->m_segment_position1 )
  {
    m_current_segment = 0;
    return bWritePending;
  }

  while ( m_current_position < m_current_segment->m_segment_position0 )
  {
    m_current_segment = m_current_segment->m_prev_segment;
    if ( 0 == m_current_segment )
    {
      MYON_ERROR("Corrupt MYON_Buffer");
      return false;
    }
  }

  while ( m_current_position >= m_current_segment->m_segment_position1 )
  {
    m_current_segment = m_current_segment->m_next_segment;
    if ( 0 == m_current_segment )
      return bWritePending;
  }

  return true;
}

MYON__UINT64 MYON_Buffer::Write( MYON__UINT64 size, const void* buffer )
{
  if ( 0 == size )
    return 0; // not an error condition

  if ( 0 == buffer )
  {
    MYON_ERROR("size parameter > 0 and buffer parameter is null.");
    return 0;
  }

  if ( !SetCurrentSegment(true) )
  {
    MYON_ERROR("Corrupt MYON_Buffer");
    return 0;
  }

  // m_current_position >= m_buffer_size is ok - it is not an error condition.

  MYON__UINT64 rc = 0;
  while ( size > 0 )
  {
    if ( 0 == m_current_segment )
    {
      // allocate a new segment
      const MYON__UINT64 padding_size = 4*sizeof(void*); // room for os heap info
      const MYON__UINT64 header_size = sizeof(*m_current_segment);
      MYON__UINT64 page_size = MYON_MemoryPageSize();
      if ( page_size <= 4096 )
        page_size = 4096;
      const MYON__UINT64 max_malloc_size = 16*page_size;  // largest request we want to make

      MYON__UINT64 malloc_size = ( 0 != m_last_segment && m_last_segment->m_segment_position1 > m_last_segment->m_segment_position0 )
                          ? padding_size + header_size + (m_last_segment->m_segment_position1 - m_last_segment->m_segment_position0)
                          : 0;
      if ( malloc_size < page_size/2 )
        malloc_size = page_size/2;      
      if ( malloc_size < max_malloc_size )
        malloc_size *= 2;
      while ( malloc_size < max_malloc_size && size > malloc_size - header_size - padding_size )
        malloc_size *= 2;

      malloc_size -= padding_size;
      // (size_t) cast is safe because malloc_size is always <= max_malloc_size = 16*page_size <  0xFFFFFFFF
      m_current_segment = (struct MYON_BUFFER_SEGMENT*)onmalloc((size_t)malloc_size); 
      memset(m_current_segment,0,(size_t)malloc_size);
      m_current_segment->m_prev_segment = m_last_segment;
      m_current_segment->m_segment_buffer = (unsigned char*)(m_current_segment + 1);
      if ( 0 != m_last_segment )
      {
        m_last_segment->m_next_segment = m_current_segment;
        m_current_segment->m_segment_position0 = m_last_segment->m_segment_position1;
      }
      else
        m_first_segment = m_current_segment;
      m_last_segment = m_current_segment;
      m_current_segment->m_segment_position1 = m_current_segment->m_segment_position0 + (MYON__UINT64)(malloc_size - header_size);
    }

    if (    m_current_position < m_current_segment->m_segment_position0 
         || m_current_segment->m_segment_position1 <= m_current_segment->m_segment_position0 
       )
    {
      MYON_ERROR("Corrupt MYON_Buffer");
      return 0;
    }

    if ( m_current_position >= m_current_segment->m_segment_position1 )
    {
      // happens when a seek puts the current position beyond the end of the buffer.
      if ( m_current_segment->m_segment_position1 > m_buffer_size )
        m_buffer_size = m_current_segment->m_segment_position1;
      m_current_segment = m_current_segment->m_next_segment;
      continue;
    }
            
    MYON__UINT64 offset = m_current_position - m_current_segment->m_segment_position0;
    MYON__UINT64 sz = (m_current_segment->m_segment_position1 - m_current_position);

    if ( sz > size )
      sz = size;
    memcpy( m_current_segment->m_segment_buffer + offset, buffer, (size_t)sz );
    m_current_position += sz;
    if ( m_buffer_size < m_current_position )
    {
      // wrote past the old end of the file
      m_buffer_size = m_current_position;
    }
    rc += sz;
    size -= sz;
    buffer = ((const unsigned char*)buffer) + sz;
    if ( size > 0 )
      m_current_segment = m_current_segment->m_next_segment;
  }

  return rc;
}

MYON__UINT64 MYON_Buffer::Read( MYON__UINT64 size, void* buffer )
{
  if ( 0 == size )
  {
    // not an error condition
    return 0;
  }

  if ( 0 == buffer )
  {
    // MYON_Buffer error
    MYON_ERROR("size parameter > 0 and buffer parameter is null.");
    return 0;
  }

  if ( m_current_position >= m_buffer_size )
  {
    // m_current_position == m_buffer_size is a common situation
    // and is not an error condition.
    // For example, it occurs when a previous Read() read up to the
    // end of the buffer and the caller is testing the number of 
    // bytes read to detect the end of buffer condition.
    if ( m_current_position > m_buffer_size )
    {
      MYON_ERROR("Read attempted when current position > buffer size.");
    }
    return 0;
  }

  if ( !SetCurrentSegment(false) )
  {
    MYON_ERROR("Corrupt MYON_Buffer");
    return 0;
  }

  MYON__UINT64 rc = 0;
  while ( size > 0 )
  {
    if( 0 == m_current_segment || 0 == m_current_segment->m_segment_buffer )
    {
      MYON_ERROR("Corrupt MYON_Buffer");
      return 0;
    }

    // set pos1 to the maximum position to be read from m_current_segment.
    MYON__UINT64 pos1 = (m_buffer_size < m_current_segment->m_segment_position1)
                    ? m_buffer_size
                    : m_current_segment->m_segment_position1;
    if ( m_current_position < m_current_segment->m_segment_position0 || m_current_position >= pos1 )
    {
      MYON_ERROR("Corrupt MYON_Buffer");
      return 0;
    }

    MYON__UINT64 offset = m_current_position - m_current_segment->m_segment_position0;           
    MYON__UINT64 sz = pos1 - m_current_position;

    if ( sz > size )
      sz = size;
    memcpy( buffer, m_current_segment->m_segment_buffer + offset, (size_t)sz );
    m_current_position += sz;
    rc += sz;
    size -= sz;
    buffer = ((unsigned char*)buffer) + sz;
    if ( size > 0 )
    {
      if ( m_current_position == m_buffer_size && m_current_segment == m_last_segment )
      {
        // This is a common situation that occures when the read request is for a 
        // size larger than the remaining number of bytes in the buffer. For example,
        // when repeatedly reading into a fixed size buffer until reasing the end
        // of the file. This is not an error condition.
        break;
      }
      m_current_segment = m_current_segment->m_next_segment;
    }
  }

  return rc;
}

MYON__UINT32 MYON_Buffer::LastError() const
{
  return m_last_error;
}

  
void MYON_Buffer::ClearLastError()
{
  m_last_error = 0;
}


MYON_Buffer_ErrorHandler MYON_Buffer::ErrorHandler() const
{
  return m_error_handler;
}
  
void MYON_Buffer::SetErrorHandler(MYON_Buffer_ErrorHandler error_handler)
{
  m_error_handler = error_handler;
}

bool MYON_Buffer::WriteToBinaryArchive( MYON_BinaryArchive& archive ) const
{
  // The MYON_Buffer::CRC32() calculation will call MYON_ERROR if the segment list
  // is not perfect.  The code below that goes through the segments
  // checks for errors so that crashes are avoided, but does not make
  // additional calls to MYON_ERROR.
  MYON__UINT32 buffer_crc = CRC32(0);

  if ( !archive.BeginWrite3dmChunk(TCODE_OPENNURBS_BUFFER,1,0) )
    return false;

  bool rc = false;
  for(;;)
  {
    if ( !archive.WriteBigInt(m_buffer_size) )
      break;
    if ( !archive.WriteInt(buffer_crc) )
      break;

    bool buffer_rc = true;
    MYON__UINT64 size = 0;
    for ( struct MYON_BUFFER_SEGMENT* seg = m_first_segment; 
          0 != seg && size < m_buffer_size; 
          seg = seg->m_next_segment 
        )
    {
      if ( 0 == seg->m_segment_buffer )
        continue;
      if ( seg->m_segment_position1 <= seg->m_segment_position0 )
        continue;
      MYON__UINT64 seg_size = (seg->m_segment_position1 - seg->m_segment_position0);
      if ( seg_size + size > m_buffer_size )
        seg_size = m_buffer_size - size;
      if ( !archive.WriteByte( (size_t)seg_size, seg->m_segment_buffer ) )
      {
        buffer_rc = false;
        break;
      }
      size += seg_size;
    }
    
    rc = true;
    break;
  }

  if ( !archive.EndWrite3dmChunk() )
    rc = false;

  return rc;
}


bool MYON_Buffer::ReadFromBinaryArchive( MYON_BinaryArchive& archive )
{
  Destroy();
  
  int major_version = 0;
  int minor_version = 0;
  if ( !archive.BeginRead3dmChunk(TCODE_OPENNURBS_BUFFER,&major_version,&minor_version) )
    return false;

  MYON_3DM_BIG_CHUNK c0;
  memset(&c0,0,sizeof(c0));
  archive.GetCurrentChunk(c0);

  MYON__UINT64 saved_buffer_size = 0;
  MYON__UINT32 saved_buffer_crc = 0;
  bool rc = false;
  void* a = 0;
  for(;;)
  {
    if ( 1 != major_version )
      break;
    
    if ( !archive.ReadBigInt(&saved_buffer_size) )
      break;

    if ( !archive.ReadInt(&saved_buffer_crc) )
      break;

    const MYON__UINT64 extra_size = 24; // =
                                      //  4  ( major version number )
                                      // +4  ( minor version number )
                                      // +8  ( 64-bit buffer_size )
                                      // +4  ( 32-bit buffer_crc )
                                      // +4  ( 32-bit chunk crc )
    if ( 0 == minor_version )
    {
      if ( c0.Length() != extra_size + saved_buffer_size )
      {
        MYON_ERROR("corrupt archive");
        break;
      }
    }
    else if ( c0.Length() < extra_size + saved_buffer_size )
    {
      // later versions may add more information
      // but there still needs to be enough room
      // to store the buffer.
      MYON_ERROR("corrupt archive");
      break;
    }

    if ( saved_buffer_size > 0 )
    {
      MYON__UINT64 a_capacity = saved_buffer_size;
      if ( a_capacity > 16*4096 )
        a_capacity = 16*4096;
      a = onmalloc((size_t)a_capacity);
      if ( 0 == a )
        break;
      MYON__UINT64 size = 0;
      bool buffer_rc = true;
      while( size < saved_buffer_size )
      {
        MYON__UINT64 read_size = a_capacity;
        if ( read_size > saved_buffer_size - size )
          read_size = saved_buffer_size - size;
        if ( !archive.ReadByte((size_t)read_size,a) )
        {
          buffer_rc = false;
          break;
        }
        // add to buffer
        Write(read_size,a);
        size += read_size;
      }
    
      if ( !buffer_rc )
        break;
    }

    rc = true;
    break;
  }

  if ( 0 != a )
    onfree(a);

  if ( !archive.EndRead3dmChunk() )
    rc = false;

  if ( rc )
  {
    Compact();
    const MYON__UINT32 buffer_crc = CRC32(0);
    if ( buffer_crc != saved_buffer_crc || m_buffer_size != saved_buffer_size)
    {
      // The buffer's contents have been damaged.
      MYON_ERROR("The buffer contents were corrupted during, writing, storage or reading.");
    }
  }
  else
  {
    Destroy();
  }

  return rc;
}

static bool MYON_Buffer_StreamCallback( void* context, MYON__UINT32 size, const void* buffer )
{
  return ( size == ((MYON_Buffer*)context)->Write(size,buffer) );
}

bool MYON_Buffer::Compress( MYON_Buffer& compressed_buffer ) const
{
  bool rc = false;
  MYON_CompressStream compressor;
  MYON_Buffer* out = ( this == &compressed_buffer ) ? new MYON_Buffer() : &compressed_buffer;

  out->Destroy();

  for (;;)
  {
    MYON__UINT64 uncompressed_size = Size();
    if ( uncompressed_size <= 0 )
      break;
    if ( !compressor.SetCallback(MYON_Buffer_StreamCallback,out) )
      break;
    if ( !compressor.Begin() )
      break;

    struct MYON_BUFFER_SEGMENT* prev_seg = 0;
    struct MYON_BUFFER_SEGMENT* seg = 0;
    for ( seg = m_first_segment; 0 != seg; seg = seg->m_next_segment )
    {
      const MYON__UINT64 pos1 = (uncompressed_size < seg->m_segment_position1)
                            ? uncompressed_size 
                            : seg->m_segment_position1;
      if ( pos1 < seg->m_segment_position0 )
        break;
      if ( prev_seg != seg->m_prev_segment )
        break;
      if ( 0 == prev_seg )
      {
        if ( 0 != seg->m_segment_position0 )
          break;
      }
      else
      {
        if ( prev_seg->m_segment_position1 != seg->m_segment_position0 )
          break;
      }
      if ( !compressor.In(pos1 - seg->m_segment_position0,seg->m_segment_buffer) )
        break;
      prev_seg = seg;
    }
    if ( 0 != seg )
      break;

    if ( !compressor.End() )
      break;

    if ( compressor.InSize() != uncompressed_size )
      break;
    if ( compressor.InCRC() != CRC32(0) )
      break;
    if ( compressor.OutSize() != out->Size() )
      break;
    if ( compressor.OutCRC() != out->CRC32(0) )
      break;

    rc = true;
    break;
  }

  if ( !rc )
  {
    out->Destroy();
    if ( this == &compressed_buffer )
      delete out;
  }
  else
  {
    out->Compact();
    out->m_current_position = 0;
    out->m_current_segment = 0;
    if ( this == &compressed_buffer )
    {
      // transfer "out" to "this"
      compressed_buffer.Destroy();
      compressed_buffer.m_buffer_size = out->m_buffer_size;
      compressed_buffer.m_current_position = out->m_current_position;
      compressed_buffer.m_first_segment = out->m_first_segment;
      compressed_buffer.m_last_segment = out->m_last_segment;
      compressed_buffer.m_current_segment = out->m_current_segment;
      compressed_buffer.m_error_handler = out->m_error_handler;
      compressed_buffer.m_last_error = out->m_last_error;
      
      out->m_first_segment = 0;
      out->m_last_segment = 0;
      out->m_current_segment = 0;
      out->m_buffer_size = 0;
      delete out;
    }
  }

  return rc;
}

bool MYON_Buffer::Uncompress( MYON_Buffer& uncompressed_buffer ) const
{
  bool rc = false;
  MYON_UncompressStream uncompressor;
  MYON_Buffer* out = ( this == &uncompressed_buffer ) ? new MYON_Buffer() : &uncompressed_buffer;

  out->Destroy();

  for (;;)
  {
    MYON__UINT64 compressed_size = Size();
    if ( compressed_size <= 0 )
      break;
    if ( !uncompressor.SetCallback(MYON_Buffer_StreamCallback,out) )
      break;
    if ( !uncompressor.Begin() )
      break;

    struct MYON_BUFFER_SEGMENT* prev_seg = 0;
    struct MYON_BUFFER_SEGMENT* seg = 0;
    for ( seg = m_first_segment; 0 != seg; seg = seg->m_next_segment )
    {
      const MYON__UINT64 pos1 = (compressed_size < seg->m_segment_position1)
                            ? compressed_size 
                            : seg->m_segment_position1;
      if ( pos1 < seg->m_segment_position0 )
        break;
      if ( prev_seg != seg->m_prev_segment )
        break;
      if ( 0 == prev_seg )
      {
        if ( 0 != seg->m_segment_position0 )
          break;
      }
      else
      {
        if ( prev_seg->m_segment_position1 != seg->m_segment_position0 )
          break;
      }
      if ( !uncompressor.In(pos1 - seg->m_segment_position0,seg->m_segment_buffer) )
        break;
      prev_seg = seg;
    }
    if ( 0 != seg )
      break;

    if ( !uncompressor.End() )
      break;

    if ( uncompressor.InSize() != compressed_size )
      break;
    if ( uncompressor.InCRC() != CRC32(0) )
      break;
    if ( uncompressor.OutSize() != out->Size() )
      break;
    if ( uncompressor.OutCRC() != out->CRC32(0) )
      break;

    rc = true;
    break;
  }

  if ( !rc )
  {
    out->Destroy();
    if ( this == &uncompressed_buffer )
      delete out;
  }
  else
  {
    out->Compact();
    out->m_current_position = 0;
    out->m_current_segment = 0;
    if ( this == &uncompressed_buffer )
    {
      // transfer "out" to "this"
      uncompressed_buffer.Destroy();
      uncompressed_buffer.m_buffer_size = out->m_buffer_size;
      uncompressed_buffer.m_current_position = out->m_current_position;
      uncompressed_buffer.m_first_segment = out->m_first_segment;
      uncompressed_buffer.m_last_segment = out->m_last_segment;
      uncompressed_buffer.m_current_segment = out->m_current_segment;
      uncompressed_buffer.m_error_handler = out->m_error_handler;
      uncompressed_buffer.m_last_error = out->m_last_error;
      
      out->m_first_segment = 0;
      out->m_last_segment = 0;
      out->m_current_segment = 0;
      out->m_buffer_size = 0;
      delete out;
    }
  }

  return rc;
}

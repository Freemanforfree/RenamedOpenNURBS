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

#include "opennurbs_zlib.h"

struct MYON_ZlibImplementation
{
  z_stream m_strm;
  unsigned char m_zlib_out_buffer[16384];
};

MYON_CompressStream::MYON_CompressStream()
: m_out_callback_function(0)
, m_out_callback_context(0)
, m_in_size(0)
, m_out_size(0)
, m_in_crc(0)
, m_out_crc(0)
, m_implementation(0)
, m_reserved(0)
{}


MYON_CompressStream::~MYON_CompressStream()
{

  if ( 0 != m_implementation )
  {
    onfree(m_implementation);
    m_implementation = 0;
  }
}

void MYON_CompressStream::ErrorHandler()
{
  // place holder for error handing
  MYON_ERROR("MYON_CompressStream error");
}

bool MYON_CompressStream::Begin()
{
  if ( 0 != m_implementation )
  {
    onfree(m_implementation);
    m_implementation = 0;
  }

  // zero these because the same instance of an 
  // MYON_CompressStream class may be used multiple times.
  m_in_size = 0;
  m_out_size = 0;
  m_in_crc = 0;
  m_out_crc = 0;

  struct MYON_ZlibImplementation* imp = (struct MYON_ZlibImplementation*)onmalloc(sizeof(*imp));
  memset(&imp->m_strm,0,sizeof(imp->m_strm));

  if ( Z_OK != deflateInit( &imp->m_strm, Z_BEST_COMPRESSION ) )
  {
    onfree(imp);
    return false;
  }

  m_implementation = imp;

  return true;
}


bool MYON_CompressStream::In( MYON__UINT64 size, const void* uncompressed_buffer )
{
  if ( size <= 0 )
    return true;

  if ( 0 == m_implementation )
  {
    ErrorHandler();
    return false;
  }

  if ( 0 == uncompressed_buffer )
  {
    ErrorHandler();
    return false;
  }
  
  struct MYON_ZlibImplementation* imp = (struct MYON_ZlibImplementation*)m_implementation;
  z_stream& strm = imp->m_strm;
  if ( 0 != strm.avail_in || 0 != strm.next_in )
  {
    // strm.avail_in is always zero when we leave an MYON_CompressStream function.
    ErrorHandler();
    return false;
  }

  const MYON__UINT32 sizeof_out_buffer = (MYON__UINT32)(sizeof(imp->m_zlib_out_buffer));
  void* out_buffer = imp->m_zlib_out_buffer;
  int zrc = Z_OK;
  const MYON__UINT64 max_sz = 0x7FFFFFF0;
  bool rc = false;
  MYON__UINT32 deflate_output_count;

  // counter prevents infinte loops if there is a bug in zlib return codes.
  for( int counter = 512; counter > 0; counter-- )
  {
    // Call zlib's deflate function.  It can either process
    // more input from m_zlib.strm.next_in[], create more
    // compressed output in m_zlib.strm.next_out[], or do both.

    // provide storage for compressed stream output
    strm.next_out  = (z_Bytef*)out_buffer;
    strm.avail_out = sizeof_out_buffer;

    if ( strm.avail_in <= 0 )
    {
      if ( size <= 0 )
      {
        // finshed with uncompressed input
        break;
      }
      // submit a portion of uncompressed_buffer to zlib
      MYON__UINT64 sz = (size > max_sz) ? max_sz : size;
      m_in_size += sz;
      m_in_crc = MYON_CRC32(m_in_crc,(size_t)sz,uncompressed_buffer); // (size_t) cast is safe because sz <= max_sz = 0x7FFFFFF0
      strm.next_in = (z_Bytef*)uncompressed_buffer;
      strm.avail_in = (MYON__UINT32)sz;
      uncompressed_buffer = ((const unsigned char*)uncompressed_buffer) + sz;
      size -= sz;
      counter = 512; // added input - reset the counter that detects stalls
    }

    // calculate compression
    MYON__UINT32 avail_in0 = strm.avail_in;
    MYON__UINT32 avail_out0 = strm.avail_out;
    zrc = z_deflate( &strm, Z_NO_FLUSH ); 
    if ( zrc < 0 ) 
    {
      // Something went haywire - bail out.
      ErrorHandler();
      rc = false;
      break;
    }
    if ( strm.avail_in < avail_in0 || strm.avail_out > avail_out0 )
    {
      // zlib did something
      rc = true; 
    }    

    deflate_output_count = sizeof_out_buffer - strm.avail_out;
    if ( deflate_output_count > 0 ) 
    {
      // The last call to deflate created compressed output.  
      // Send the output to compressed stream handler.

      // Calculate the updated crc and size before we call
      // the output handler because someday sombody will
      // decide it's a good idea to modify the values
      // in the buffer argument.
      MYON__UINT32 out_crc1 = MYON_CRC32( m_out_crc, deflate_output_count, out_buffer);
      MYON__UINT64 out_size1 = m_out_size + deflate_output_count;
      
      rc = (0 != m_out_callback_function)
          ? m_out_callback_function( m_out_callback_context, deflate_output_count, out_buffer )
          : Out( m_out_callback_context, deflate_output_count, out_buffer );
      if ( !rc )
        break;

      // Update compressed stream crc and size
      m_out_crc = out_crc1;
      m_out_size = out_size1;
      counter = 512; // created output - reset counter that detects stalls
    }

    if ( size <= 0 && strm.avail_in <= 0 )
    {
      // no input left
      break;
    }
  }

  strm.avail_in = 0;
  strm.next_in = 0;
  strm.next_out  = 0;
  strm.avail_out = 0;

  return rc;
}

bool MYON_CompressStream::End()
{
  if ( 0 == m_implementation )
  {
    ErrorHandler();
    return false;
  }
  
  struct MYON_ZlibImplementation* imp = (struct MYON_ZlibImplementation*)m_implementation;
  z_stream& strm = imp->m_strm;
  if ( 0 != strm.avail_in || 0 != strm.next_in )
  {
    // strm.avail_in is always zero when we leave an MYON_CompressStream function.
    ErrorHandler();
    return false;
  }

  const MYON__UINT32 sizeof_out_buffer = (MYON__UINT32)(sizeof(imp->m_zlib_out_buffer));
  void* out_buffer = imp->m_zlib_out_buffer;
  int zrc = Z_OK;
  bool rc = false;
  MYON__UINT32 deflate_output_count;

  // counter prevents infinte loops if there is a bug in zlib return codes.
  for( int counter = 512; counter > 0; counter-- )
  {
    // provide storage for compressed stream output
    strm.avail_in = 0;
    strm.next_in = 0;
    strm.next_out  = (z_Bytef*)out_buffer;
    strm.avail_out = sizeof_out_buffer;

    // finish compression calculation
    zrc = z_deflate( &strm, Z_FINISH ); 
    if ( zrc < 0 ) 
    {
      // Something went haywire - bail out.
      ErrorHandler();
      rc = false;
      break;
    }

    deflate_output_count = sizeof_out_buffer - strm.avail_out;
    if ( deflate_output_count > 0 ) 
    {
      // The last call to deflate created compressed output.  
      // Send the output to compressed stream handler.

      // Calculate the updated crc and size before we call
      // the output handler because someday sombody will
      // decide it's a good idea to modify the values
      // in the buffer argument.
      MYON__UINT32 compressed_crc1 = MYON_CRC32( m_out_crc, deflate_output_count, out_buffer);
      MYON__UINT64 compressed_size1 = m_out_size + ((MYON__UINT64)deflate_output_count);
      
      rc = (0 != m_out_callback_function)
          ? m_out_callback_function( m_out_callback_context, deflate_output_count, out_buffer )
          : Out( m_out_callback_context, deflate_output_count, out_buffer );
      if ( !rc )
        break;

      // Update compressed stream crc and size
      m_out_crc = compressed_crc1;
      m_out_size = compressed_size1;
      counter = 512; // created output - reset counter that detects stalls
    }

    if ( Z_STREAM_END == zrc )
    {
      // no input left, all pending compressing is finished,
      // and all compressed output has been returned.
      rc = true;
      break;
    }
  }

  strm.avail_in = 0;
  strm.next_in = 0;
  strm.next_out  = 0;
  strm.avail_out = 0;

  deflateEnd(&strm);

  onfree(m_implementation);
  m_implementation = 0;

  return rc;
}

bool MYON_CompressStream::Out( void*, MYON__UINT32, const void* )
{
  // default compressed stream handler does nothing.
  return true;
}

bool MYON_CompressStream::SetCallback( 
    MYON_StreamCallbackFunction out_callback_function,
    void* out_callback_context
    )
{
  m_out_callback_function = out_callback_function;
  m_out_callback_context = out_callback_context;
  return true;
}

MYON_StreamCallbackFunction MYON_CompressStream::CallbackFunction() const
{
  return m_out_callback_function;
}

void* MYON_CompressStream::CallbackContext() const
{
  return m_out_callback_context;
}


MYON__UINT64 MYON_CompressStream::InSize() const
{
  return m_in_size;
}

MYON__UINT64 MYON_CompressStream::OutSize() const
{
  return m_out_size;
}

MYON__UINT32 MYON_CompressStream::InCRC() const
{
  return m_in_crc;
}

MYON__UINT32 MYON_CompressStream::OutCRC() const
{
  return m_out_crc;
}


///////////////////////////////////////////////////////////////////////////////////////////


MYON_UncompressStream::MYON_UncompressStream()
: m_out_callback_function(0)
, m_out_callback_context(0)
, m_in_size(0)
, m_out_size(0)
, m_in_crc(0)
, m_out_crc(0)
, m_implementation(0)
, m_reserved(0)
{}


MYON_UncompressStream::~MYON_UncompressStream()
{

  if ( 0 != m_implementation )
  {
    onfree(m_implementation);
    m_implementation = 0;
  }
}

void MYON_UncompressStream::ErrorHandler()
{
  // place holder for error handing
  MYON_ERROR("MYON_UncompressStream error");
}

bool MYON_UncompressStream::Begin()
{
  if ( 0 != m_implementation )
  {
    onfree(m_implementation);
    m_implementation = 0;
  }

  // zero these because the same instance of an 
  // MYON_UncompressStream class may be used multiple times.
  m_in_size = 0;
  m_out_size = 0;
  m_in_crc = 0;
  m_out_crc = 0;

  struct MYON_ZlibImplementation* imp = (struct MYON_ZlibImplementation*)onmalloc(sizeof(*imp));
  memset(&imp->m_strm,0,sizeof(imp->m_strm));

  if ( Z_OK != inflateInit( &imp->m_strm ) )
  {
    onfree(imp);
    return false;
  }

  m_implementation = imp;


  return true;
}


bool MYON_UncompressStream::In( MYON__UINT64 size, const void* compressed_buffer )
{
  if ( size <= 0 )
    return true;

  if ( 0 == m_implementation )
  {
    ErrorHandler();
    return false;
  }

  if ( 0 == compressed_buffer )
  {
    ErrorHandler();
    return false;
  }
  
  struct MYON_ZlibImplementation* imp = (struct MYON_ZlibImplementation*)m_implementation;
  z_stream& strm = imp->m_strm;
  if ( 0 != strm.avail_in || 0 != strm.next_in )
  {
    // strm.avail_in is always zero when we leave an MYON_UncompressStream function.
    ErrorHandler();
    return false;
  }

  const MYON__UINT32 sizeof_out_buffer = (MYON__UINT32)(sizeof(imp->m_zlib_out_buffer));
  void* out_buffer = imp->m_zlib_out_buffer;
  int zrc = Z_OK;
  const MYON__UINT64 max_sz = 0x7FFFFFF0;
  bool rc = false;
  MYON__UINT32 inflate_output_count;

  // counter prevents infinte loops if there is a bug in zlib return codes.
  for( int counter = 512; counter > 0; counter-- )
  {
    // Call zlib's inflate function.  It can process
    // more compressed input from strm.next_in[], create more
    // uncompressed output in strm.next_out[], or do both.

    // provide storage for uncompressed stream output
    strm.next_out  = (z_Bytef*)out_buffer;
    strm.avail_out = sizeof_out_buffer;

    if ( strm.avail_in <= 0 )
    {
      if ( size <= 0 )
      {
        // finshed with compressed input
        break;
      }
      // submit a portion of compressed_buffer to zlib
      MYON__UINT64 sz = (size > max_sz) ? max_sz : size;
      m_in_size += sz;
      m_in_crc = MYON_CRC32(m_in_crc,(size_t)sz,compressed_buffer); // (size_t) cast is safe because sz <= max_sz = 0x7FFFFFF0
      strm.next_in = (z_Bytef*)compressed_buffer;
      strm.avail_in = (MYON__UINT32)sz;
      compressed_buffer = ((const unsigned char*)compressed_buffer) + sz;
      size -= sz;
      counter = 512; // added input - reset the counter that detects stalls
    }

    // calculate compression
    MYON__UINT32 avail_in0 = strm.avail_in;
    MYON__UINT32 avail_out0 = strm.avail_out;
    zrc = z_inflate( &strm, Z_NO_FLUSH ); 
    if ( zrc < 0 ) 
    {
      // Something went haywire - bail out.
      ErrorHandler();
      rc = false;
      break;
    }
    if ( strm.avail_in < avail_in0 || strm.avail_out > avail_out0 )
    {
      // zlib did something
      rc = true; 
    }    

    inflate_output_count = sizeof_out_buffer - strm.avail_out;
    if ( inflate_output_count > 0 ) 
    {
      // The last call to inflate created uncompressed output.  
      // Send the output to the uncompressed stream handler.

      // Calculate the updated crc and size before we call
      // the output handler because someday sombody will
      // decide it's a good idea to modify the values
      // in the buffer argument.
      MYON__UINT32 out_crc1 = MYON_CRC32( m_out_crc, inflate_output_count, out_buffer);
      MYON__UINT64 out_size1 = m_out_size + inflate_output_count;
      
      rc = (0 != m_out_callback_function)
          ? m_out_callback_function( m_out_callback_context, inflate_output_count, out_buffer )
          : Out( m_out_callback_context, inflate_output_count, out_buffer );
      if ( !rc )
        break;

      // Update compressed stream crc and size
      m_out_crc = out_crc1;
      m_out_size = out_size1;
      counter = 512; // created output - reset counter that detects stalls
    }

    if ( size <= 0 && strm.avail_in <= 0 )
    {
      // no input left
      break;
    }
  }

  strm.avail_in = 0;
  strm.next_in = 0;
  strm.next_out  = 0;
  strm.avail_out = 0;

  return rc;
}

bool MYON_UncompressStream::End()
{
  if ( 0 == m_implementation )
  {
    ErrorHandler();
    return false;
  }
  
  struct MYON_ZlibImplementation* imp = (struct MYON_ZlibImplementation*)m_implementation;
  z_stream& strm = imp->m_strm;
  if ( 0 != strm.avail_in || 0 != strm.next_in )
  {
    // strm.avail_in is always zero when we leave an MYON_UncompressStream function.
    ErrorHandler();
    return false;
  }

  const MYON__UINT32 sizeof_out_buffer = (MYON__UINT32)(sizeof(imp->m_zlib_out_buffer));
  void* out_buffer = imp->m_zlib_out_buffer;
  int zrc = Z_OK;
  bool rc = false;
  MYON__UINT32 inflate_output_count;

  // counter prevents infinte loops if there is a bug in zlib return codes.
  for( int counter = 512; counter > 0; counter-- )
  {
    // provide storage for compressed stream output
    strm.avail_in = 0;
    strm.next_in = 0;
    strm.next_out  = (z_Bytef*)out_buffer;
    strm.avail_out = sizeof_out_buffer;

    // finish compression calculation
    zrc = z_inflate( &strm, Z_FINISH ); 
    if ( zrc < 0 ) 
    {
      // Something went haywire - bail out.
      ErrorHandler();
      rc = false;
      break;
    }

    inflate_output_count = sizeof_out_buffer - strm.avail_out;
    if ( inflate_output_count > 0 ) 
    {
      // The last call to inflate created uncompressed output.  
      // Send the output to the uncompressed stream handler.

      // Calculate the updated crc and size before we call
      // the output handler because someday sombody will
      // decide it's a good idea to modify the values
      // in the buffer argument.
      MYON__UINT32 out_crc1 = MYON_CRC32( m_out_crc, inflate_output_count, out_buffer);
      MYON__UINT64 out_size1 = m_out_size + inflate_output_count;
      
      rc = (0 != m_out_callback_function)
          ? m_out_callback_function( m_out_callback_context, inflate_output_count, out_buffer )
          : Out( m_out_callback_context, inflate_output_count, out_buffer );
      if ( !rc )
        break;

      // Update compressed stream crc and size
      m_out_crc = out_crc1;
      m_out_size = out_size1;
      counter = 512; // created output - reset counter that detects stalls
    }

    if ( Z_STREAM_END == zrc )
    {
      // no input left, all pending compressing is finished,
      // and all compressed output has been returned.
      rc = true;
      break;
    }
  }

  strm.avail_in = 0;
  strm.next_in = 0;
  strm.next_out  = 0;
  strm.avail_out = 0;

  inflateEnd(&strm);

  onfree(m_implementation);
  m_implementation = 0;

  return rc;
}

bool MYON_UncompressStream::Out( void*, MYON__UINT32, const void* )
{
  // default uncompressed stream handler does nothing.
  return true;
}

bool MYON_UncompressStream::SetCallback( 
    MYON_StreamCallbackFunction out_callback_function,
    void* out_callback_context
    )
{
  m_out_callback_function = out_callback_function;
  m_out_callback_context = out_callback_context;
  return true;
}

MYON_StreamCallbackFunction MYON_UncompressStream::CallbackFunction() const
{
  return m_out_callback_function;
}

void* MYON_UncompressStream::CallbackContext() const
{
  return m_out_callback_context;
}

MYON__UINT64 MYON_UncompressStream::InSize() const
{
  return m_in_size;
}

MYON__UINT64 MYON_UncompressStream::OutSize() const
{
  return m_out_size;
}

MYON__UINT32 MYON_UncompressStream::InCRC() const
{
  return m_in_crc;
}

MYON__UINT32 MYON_UncompressStream::OutCRC() const
{
  return m_out_crc;
}


class MYON_UncompressBuffer_Context
{
public:
  MYON_UncompressBuffer_Context(
    size_t sizeof_uncompressed_buffer,
    const void* uncompressed_buffer
    )
    : m_dst((sizeof_uncompressed_buffer > 0) ? ((unsigned char*)uncompressed_buffer) : nullptr)
    , m_dst_end((nullptr != m_dst) ? (m_dst + sizeof_uncompressed_buffer) : nullptr)
  {}

  unsigned char* m_dst = nullptr;
  unsigned char* m_dst_end = nullptr;
  static bool Callback(void* context, MYON__UINT32 size, const void* buffer)
  {
    MYON_UncompressBuffer_Context* p = (MYON_UncompressBuffer_Context*)context;
    unsigned char* dst = p->m_dst;
    unsigned char* dst_end = p->m_dst_end;
    const unsigned char* src = (size > 0) ? ((const unsigned char*)buffer) : nullptr;
    const unsigned char* src_end = (nullptr != src) ? (src + size) : nullptr;
    while (dst < dst_end && src < src_end)
    {
      *dst++ = *src++;
    }
    p->m_dst = dst;
    return (src == src_end);
  }
private:
  MYON_UncompressBuffer_Context() = delete;
  MYON_UncompressBuffer_Context(const MYON_UncompressBuffer_Context&) = delete;
  MYON_UncompressBuffer_Context& operator=(const MYON_UncompressBuffer_Context&) = delete;
};

size_t MYON_UncompressBuffer(
  size_t sizeof_compressed_buffer,
  const void* compressed_buffer,
  size_t sizeof_uncompressed_buffer,
  void* uncompressed_buffer
  )
{
  unsigned char* dst((sizeof_uncompressed_buffer > 0) ? ((unsigned char*)uncompressed_buffer) : nullptr);
  MYON_UncompressBuffer_Context context(sizeof_uncompressed_buffer, dst);
  MYON_UncompressStream unzipper;
  bool rc = unzipper.SetCallback(MYON_UncompressBuffer_Context::Callback, &context);
  if (rc)
  {
    rc = unzipper.Begin();
    if (rc)
    {
      rc = unzipper.In(sizeof_compressed_buffer, compressed_buffer);
      if (!unzipper.End())
        rc = false;
    }
  }
  return rc ? (context.m_dst - dst) : 0;
}

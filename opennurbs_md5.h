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

#if !defined(OPENNURBS_MD5_INC_)
#define OPENNURBS_MD5_INC_

/*
The MYON_MD5 class is based on code that is modified from C code with the following copyright.

Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All rights reserved.
 
  License to copy and use this software is granted provided that it
  is identified as the "RSA Data Security, Inc. MD5 Message-Digest
  Algorithm" in all material mentioning or referencing this software
  or this function.
 
  License is also granted to make and use derivative works provided
  that such works are identified as "derived from the RSA Data
  Security, Inc. MD5 Message-Digest Algorithm" in all material
  mentioning or referencing the derived work.
 
  RSA Data Security, Inc. makes no representations concerning either
  the merchantability of this software or the suitability of this
  software for any particular purpose. It is provided "as is"
  without express or implied warranty of any kind.
 
  These notices must be retained in any copies of any part of this
  documentation and/or software.
*/
class MYON_CLASS MYON_MD5_Hash
{
public:
  static const MYON_MD5_Hash ZeroDigest;       // all digest bytes are zero
  static const MYON_MD5_Hash EmptyContentHash; // MD5 hash of zero bytes

  // Default constructor is the zero digest hash
  MYON_MD5_Hash();

  ~MYON_MD5_Hash() = default;
  MYON_MD5_Hash(const MYON_MD5_Hash&) = default;
  MYON_MD5_Hash& operator=(const MYON_MD5_Hash&) = default;

  /*
  Parameters:
    buffer - [in]
    sizeof_buffer - [in]
      number of bytes in buffer
  Returns:
    MD5 hash of the buffer.
  */
  static MYON_MD5_Hash BufferHash(
    const void* buffer,
    size_t sizeof_buffer
    );

  /*
  Parameters:
    filename - [in]
      Name of file
    sizeof_file - [out]
      number of bytes in file
  Returns:
    MD5 hash of the buffer.
  */
  static MYON_MD5_Hash FileHash(
    const wchar_t* filename,
    MYON__UINT64& sizeof_file
    );

  static MYON_MD5_Hash FileHash(
    const char* filename,
    MYON__UINT64& sizeof_file
    );

  /*
  Parameters:
    file - [in]
      File stream from MYON_FileStream::Open(...,L"rb");
    sizeof_file - [out]
      number of bytes in file
  Returns:
    MD5 hash of the file stream from the current
    offset to the end of the file.
  */
  static MYON_MD5_Hash FileHash(
    FILE* file,
    MYON__UINT64& sizeof_file
    );

  /*
  Parameters:
    str - [in]
      string
    byte_count - [out]
      number of bytes in UTF-8 encoding of the string.
  Returns:
    MD5 hash of the UTF-8 encoding of the string. (Platforms and endian independent.)
  */
  static MYON_MD5_Hash StringHash(
    const MYON_wString& str,
    MYON__UINT64& byte_count
    );

  static MYON_MD5_Hash StringHash(
    const wchar_t* str,
    size_t str_length,
    MYON__UINT64& byte_count
    );

  /*
  Parameters:
    str - [in]
    byte_count - [out]
      number of bytes in the string.
  Returns:
    MD5 hash of the UTF-8 encoding of the string. (Platforms and endian independent.)
  */
  static MYON_MD5_Hash StringHash(
    const MYON_String& str,
    MYON__UINT64& byte_count
    );

  static MYON_MD5_Hash StringHash(
    const char* str,
    size_t str_length,
    MYON__UINT64& byte_count
    );

  static int Compare(
    const MYON_MD5_Hash& a,
    const MYON_MD5_Hash& b
    );

  /*
  Parameters:
    bUpperCaseHexadecimalDigits - [in]
      false - use 0-9, a-f
      true - use 0-9, A-F
  Returns:
    The MD5 hash value as a 32 hexadecimal digits.
    The first digit in the string is the hexadecimal value of m_digest[0].
  */
  const MYON_String ToUTF8String(
    bool bUpperCaseHexadecimalDigits
    ) const;

  /*
  Parameters:
    bUpperCaseHexadecimalDigits - [in]
      false - use 0-9, a-f
      true - use 0-9, A-F
  Returns:
    The MD5 hash value as a 32 hexadecimal digits.
    The first digit in the string is the hexadecimal value of m_digest[0].
  */
  const MYON_wString ToString(
    bool bUpperCaseHexadecimalDigits
    ) const;

  bool Read(
    class MYON_BinaryArchive& archive
    );

  bool Write(
    class MYON_BinaryArchive& archive
    ) const;

  void Dump(
    class MYON_TextLog& text_log
    ) const;

  MYON__UINT8 m_digest[16];
};

MYON_DECL
bool operator==(const MYON_MD5_Hash& a, const MYON_MD5_Hash& b);

MYON_DECL
bool operator!=(const MYON_MD5_Hash& a, const MYON_MD5_Hash& b);

/*
Description:
  MYON_MD5 is a small class for calculating the MD5 hash of a sequence of bytes.
  It may be use incrementally (the bytes do not have to be in a contiguous
  array in memory at one time).  
  
Remarks:
  The MYON_MD5 class cannot be used for cryptographic or security applications.
  The MD5 hash algorithm is not suitable for cryptographic or security applications.
  The MYON_MD5 class does not "wipe" intermediate results.
  
  The probability of two different randomly selected sequences of N bytes to have the
  same value MD5 hash depends on N, but it is roughly 2^-64 ~ 10^-19.

  MD5 hash values are 16 bytes. SHA-1 hash values are 20 bytes. If you need a hash
  and have room for 20 bytes, then MYON_SHA1 is preferred over MYON_MD5.

Legal:
  Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All rights reserved.
 
  License to copy and use this software is granted provided that it
  is identified as the "RSA Data Security, Inc. MD5 Message-Digest
  Algorithm" in all material mentioning or referencing this software
  or this function.
 
  License is also granted to make and use derivative works provided
  that such works are identified as "derived from the RSA Data
  Security, Inc. MD5 Message-Digest Algorithm" in all material
  mentioning or referencing the derived work.
 
  RSA Data Security, Inc. makes no representations concerning either
  the merchantability of this software or the suitability of this
  software for any particular purpose. It is provided "as is"
  without express or implied warranty of any kind.
 
  These notices must be retained in any copies of any part of this
  documentation and/or software.
*/
class MYON_CLASS MYON_MD5
{
public:
 
  MYON_MD5() = default;
  ~MYON_MD5() = default;
  MYON_MD5(const MYON_MD5&) = default;
  MYON_MD5& operator=(const MYON_MD5&) = default;

  /*
  Description:
    Make one or more calls to AccumulateBytes() as the sequenence of bytes is available.
  Parameters:
    buffer - [in]
    sizeof_buffer - [in]
      number of bytes in buffer
  */
#if defined(MYON_COMPILER_MSC) && defined(NDEBUG)
  // Reduces release build link time optimization by several hours for
  // large programs that make lots of calls to MYON_MD5.Accumulate*() functions.
  __declspec(noinline)
#endif
    void AccumulateBytes(
    const void* buffer,
    MYON__UINT64 sizeof_buffer
    );
  
  /*
  Returns:
    Total number of bytes passed to Update().
  */
  MYON__UINT64 ByteCount() const;

  /*
  Returns:
    MD5 hash value of the sequenence of ByteCount() bytes that have been
    passed to this MYON_MD5 classe's Update() function since construction
    or the last call to Reset().
  Remarks:
    You may use Hash() to compute intermediate MD5 hash values. 
    
    Put another way, you may call Update() zero or more times passing in N1 bytes, 
    call Digest() to get the MD5 hash of those N1 bytes, make zero or more additional
    calls to Update() passing in N2 additional bytes, call digest to get the MD5 hash
    of the sequence of (N1 + N2) bytes, and so on.
  */
  MYON_MD5_Hash Hash() const;

  /*
  Description:
    Reset this MYON_MD5 class so it can be reused.  
  */
  void Reset();

  /*
  Description:
    This is a static function that uses MYON_MD5 to compute MD5 hash values
    of sequences of bytes with known MD5 hash values and compares the 
    results from MYON_SHA1 with the known MD5 hash values.

    This function can be used to validate the MYON_MD5 class compiled correctly.

  Returns:
    true 
      All validation tests passed.
    false 
      At least one validation test failed.
  */ 
  static bool Validate();

private:
  void Internal_Accumulate(const MYON__UINT8* input, MYON__UINT32 length);
  void set_final_hash();
 
  MYON__UINT64 m_byte_count = 0; // number of bytes that have passed through calls to Update().
  // if 1 == m_status_bits & 1, then Update has been called at least once (perhaps with 0 bytes).
  // if 2 == m_status_bits & 2, then m_md5_hash is current.
  mutable MYON__UINT32 m_status_bits = 0;
  MYON__UINT32 m_reserved = 0;  

  // current "remainder"
  MYON__UINT8 m_buffer[64];      // bytes that didn't fit in last 64 byte chunk  
  MYON__UINT32 m_bit_count[2];   // number of bits (lo, hi)
  MYON__UINT32 m_state[4];       // current state
  
  // cached MD5 hash - valid if 2 = (2 & m_status_bits)
  mutable MYON_MD5_Hash m_md5_hash;
};

#endif

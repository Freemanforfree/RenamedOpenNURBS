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

#if !defined(OPENNURBS_SHA1_INC_)
#define OPENNURBS_SHA1_INC_

class MYON_CLASS MYON_SHA1_Hash
{
public:
  static const MYON_SHA1_Hash ZeroDigest;       // all digest bytes are zero
  static const MYON_SHA1_Hash EmptyContentHash; // SHA-1 hash of zero bytes

  // The default constructor creates a zero digest sha1 hash
  MYON_SHA1_Hash();

  ~MYON_SHA1_Hash() = default;
  MYON_SHA1_Hash(const MYON_SHA1_Hash&) = default;
  MYON_SHA1_Hash& operator=(const MYON_SHA1_Hash&) = default;

  static int Compare(
    const MYON_SHA1_Hash& a,
    const MYON_SHA1_Hash& b
    );

  /*
  Parameters:
    buffer - [in]
    sizeof_buffer - [in]
      number of bytes in buffer
  Returns:
    SHA1-1 hash of the buffer.
  */
  static MYON_SHA1_Hash BufferContentHash(
    const void* buffer,
    size_t sizeof_buffer
    );

  /*
  Parameters:
    file_name - [in]
      Name of file
    sizeof_file - [out]
      number of bytes in file
  Returns:
    SHA1-1 hash of the buffer.
  */
  static MYON_SHA1_Hash FileContentHash(
    const wchar_t* file_name,
    MYON__UINT64& sizeof_file
    );

  static MYON_SHA1_Hash FileContentHash(
    const char* file_name,
    MYON__UINT64& sizeof_file
    );
    
  /*
  Description:
    Return a hash of the file system path that is independent 
    of the size of wchar_t, constant across platforms, and 
    constant across variations in the way the path is formatted.

  Parameters:
    path - [in]
      File system path to a directory or file.

  Returns:
    SHA1-1 hash of the buffer.

  Example:
    These file system paths have identical values of FileSystemPathHash().
    /x/y/z/name.ext
    \x\y\z\name.ext
    /x//y//z/name.ext
    /x/y/a/b/c/../../../z/name.ext
    /X/Y/Z/NAME.EXT (When MYON_ComparePathIgnoreCase() is true)

  Remarks:
    This function uses the value of MYON_FileSystemPath::PlatformPathIgnoreCase()
    to determine  if case should be ignored. Use the version with a bIgnoreCase
    parameter if you want to explicitly control this decision.
  */
  static MYON_SHA1_Hash FileSystemPathHash(
    const wchar_t* path
    );

  static MYON_SHA1_Hash FileSystemPathHash(
    const char* path
    );

  static MYON_SHA1_Hash FileSystemPathHash(
    const wchar_t* path,
    bool bIgnoreCase
    );

  static MYON_SHA1_Hash FileSystemPathHash(
    const char* path,
    bool bIgnoreCase
    );

  /*
  Parameters:
    file - [in]
      File stream from MYON_FileStream::Open(...,L"rb");
    sizeof_file - [out]
      number of bytes in file
  Returns:
    SHA1-1 hash of the file stream from the current
    offset to the end of the file.
  */
  static MYON_SHA1_Hash FileContentHash(
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
    SHA1-1 hash of the UTF-8 encoding of the string. (Platforms and endian independent.)
  */
  static MYON_SHA1_Hash StringHash(
    const MYON_wString& str,
    MYON__UINT64& byte_count
    );

  static MYON_SHA1_Hash StringHash(
    const wchar_t* str,
    size_t str_length,
    MYON__UINT64& byte_count
    );

  static MYON_SHA1_Hash StringHash(
    const wchar_t* null_terminated_string
    );

  /*
  Parameters:
    str - [in]
    byte_count - [out]
      number of bytes in the string.
  Returns:
    SHA1-1 hash of the UTF-8 encoding of the string. (Platforms and endian independent.)
  */
  static MYON_SHA1_Hash StringHash(
    const MYON_String& str,
    MYON__UINT64& byte_count
    );

  static MYON_SHA1_Hash StringHash(
    const char* str,
    size_t str_length,
    MYON__UINT64& byte_count
    );


  static MYON_SHA1_Hash StringHash(
    const char* null_terminated_string
    );

  /*
  Parameters:
    bUpperCaseHexadecimalDigits - [in]
      false - use 0-9, a-f
      true - use 0-9, A-F
  Returns:
    The SHA-1 hash value as a 40 hexadecimal digits.
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
    The SHA-1 hash value as a 40 hexadecimal digits.
    The first digit in the string is the hexadecimal value of m_digest[0].
  */
  const MYON_wString ToString(
    bool bUpperCaseHexadecimalDigits
  ) const;

  /*
  Description:
    Same as ToString but prints EmptyContentSHA1 or ZeroSHA1 for those two special cases. 
  */
  const MYON_wString ToStringEx(
    bool bUpperCaseHexadecimalDigits
  ) const;

  /*
  Description:
    Parse a string of 40 hexadecimal digits to create a SHA-1 hash.
  Parameters:
    string_to_parse - [in]
    bParseLeadinglSpaces - [in]
      If true, leading space characters are parsed.
      Otherwise leading space characters cause parsing to fail.
    bParseInteriorSpace - [in]
      If true, interior space characters are  parsed.
      Otherwise interior space characters cause parsing to fail.
    bParseInteriorHyphen - [in]
      If true, interior hyphen characters are  parsed.
      Otherwise interior hyphen characters cause parsing to fail.
    bIgnoreInternalSpaces - [in]
      If true, isolated hyphens are ignored until 40 hex digits are read.
    bIgnoreInternalHyphens - [in]
      If true, leading spaces and isolated interior spacess are ignored until 40 hex digits are read.
    failure_return_value - [in]
      Value to return if string_to_parse cannot be parsed as 40 hex digits.
  Returns:
    If parsing is successful, the value of the SHA-1 hash is returned.
    Otherwise failure_return_value is returned.
  */
  static const MYON_SHA1_Hash FromString(
    const MYON_wString string_to_parse,
    bool bParseLeasingSpaces,
    bool bParseInteriorSpace,
    bool bParseInteriorHyphen,
    MYON_SHA1_Hash failure_return_value
  );

  bool Read(
    class MYON_BinaryArchive& archive
    );

  bool Write(
    class MYON_BinaryArchive& archive
    ) const;

  void Dump(
    class MYON_TextLog& text_log
    ) const;

  /*
  Returns:
    True if this and MYON_SHA1_Hash::ZeroDigest have identical digest values.
  */
  bool IsZeroDigest() const;

  /*
  Returns:
    True if this and MYON_SHA1_Hash::EmptyContentHash have identical digest values.
  */
  bool IsEmptyContentHash() const;

  bool IsZeroDigestOrEmptyContentHash() const;

  MYON_DEPRECATED_MSG("Use IsZeroDigestOrEmptyContentHash() instead. (Spelling error in this one's name.")
  bool IsZeroDigentOrEmptyContentHash() const;

  /*
  Returns:
    True if this hash is not equal to MYON_SHA1_Hash::EmptyContentHash or MYON_SHA1_Hash::ZeroDigest.
  Remarks:
    MYON_SHA1_Hash::EmptyContentHash is the SHA1 of hasing zero bytes and has a non zero digest.
    MYON_SHA1_Hash::ZeroDigest is 20 bytes of zeros. Opennurbs uses MYON_SHA1_Hash::ZeroDigest to
    indicate a SHA1 has is not initialized.
  */
  bool IsSet() const;

  MYON__UINT8 m_digest[20]; 
};



MYON_DECL
bool operator==(const MYON_SHA1_Hash& a, const MYON_SHA1_Hash& b);

MYON_DECL
bool operator!=(const MYON_SHA1_Hash& a, const MYON_SHA1_Hash& b);

/*
Description:
  MYON_SHA1 is a small class for calculating the SHA-1 hash of a sequence of bytes.
  It may be use incrementally (the bytes do not have to be in a contiguous
  array in memory at one time).  
  
Remarks:
  The MYON_SHA1 class cannot be used for cryptographic or security applications.
  The SHA-1 hash algorithm is not suitable for cryptographic or security applications.
  The MYON_SHA1 class does not "wipe" intermediate results.

  If you have two different sequences of N bytes storing information (lower entropy
  than a random sequence) are you are not intentionally calculating the information
  to create a SHA-1 hash collision, then the probability that the sequences have
  the same SHA-1 hash is approximately 2^-80 ~ 10^-24.
*/
class MYON_CLASS MYON_SHA1
{
public:

	MYON_SHA1() = default;
	~MYON_SHA1() = default;
  MYON_SHA1(const MYON_SHA1&) = default;
  MYON_SHA1& operator=(const MYON_SHA1&) = default;

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
  // large programs that make lots of calls to MYON_SHA1.Accumulate*() functions.
  __declspec(noinline)
#endif
  void AccumulateBytes(
    const void* buffer,
    MYON__UINT64 sizeof_buffer
    );

  /*
  Description:
    Add the double value to the SHA1 in a manner that
    -0.0 and +0.0 will generate identical SHA-1 values
    and the result is independent of endian byte order.
  */
  void AccumulateDouble(
    double x
    );

  /*
  Description:
    Add the double value to the SHA1 in a manner that
    -0.0 and +0.0 will generate identical SHA-1 values
    and the result is independent of endian byte order.
  */
  void AccumulateDoubleArray(
    size_t count,
    const double* a
    );

  /*
  Description:
    Add the double value to the SHA1 in a manner that
    -0.0 and +0.0 will generate identical SHA-1 values
    and the result is independent of endian byte order.
  */
  void AccumulateFloat(
    float x
  );

  /*
  Description:
    Add the double value to the SHA1 in a manner that
    -0.0 and +0.0 will generate identical SHA-1 values
    and the result is independent of endian byte order.
  */
  void AccumulateFloatArray(
    size_t count,
    const float* a
  );

  void Accumulate2fPoint(
    const class MYON_2fPoint& point
  );

  void Accumulate3fPoint(
    const class MYON_3fPoint& point
  );

  void Accumulate4fPoint(
    const class MYON_4fPoint& point
  );

  void Accumulate2fVector(
    const class MYON_2fVector& vector
  );

  void Accumulate3fVector(
    const class MYON_3fVector& vector
  );

  void Accumulate2dPoint(
    const class MYON_2dPoint& point
  );

  void Accumulate3dPoint(
    const class MYON_3dPoint& point
  );

  void Accumulate4dPoint(
    const class MYON_4dPoint& point
    );

  void Accumulate2dVector(
    const class MYON_2dVector& vector
  );

  void Accumulate3dVector(
    const class MYON_3dVector& vector
  );

  void AccumulateBoundingBox(
    const class MYON_BoundingBox& bbox
  );

  void AccumulateUnitSystem(
    const class MYON_UnitSystem& unit_system
  );

  void AccumulateLengthUnitSystem(
    const MYON::LengthUnitSystem length_unit_system
  );

  /*
  Description:
    Accumuates the full and relative path names.
  */
  void AccumulateFileReference(
    const class MYON_FileReference& file_reference
  );

  void AccumulateTransformation(
    const class MYON_Xform& xform
  );
  
  void AccumulateInteger8(
    char i
  );

  void AccumulateInteger8(
    MYON__INT8 i
    );
  
  void AccumulateUnsigned8(
    MYON__UINT8 u
    );
    
  void AccumulateInteger16(
    MYON__INT16 i
    );
  
  void AccumulateUnsigned16(
    MYON__UINT16 u
    );

  void AccumulateInteger32(
    MYON__INT32 i
    );

  void AccumulateInteger32Array(
    size_t count,
    const MYON__INT32* a
  );
  
  void AccumulateUnsigned32(
    MYON__UINT32 u
    );

  void AccumulateInteger64(
    MYON__INT64 i
    );
  
  void AccumulateUnsigned64(
    MYON__UINT64 u
    );


  void AccumulateBool(
    bool b
    );


  void AccumulateString(
    const class MYON_String& str
    );

  void AccumulateString(
    const class MYON_wString& str
    );

  void AccumulateId(
    const MYON_UUID& id
  );

  void AccumulateString(
    const char* sUTF8,
    int element_count,
    MYON_StringMapOrdinalType mapping
    );

  void AccumulateString(
    const wchar_t* sUTF8,
    int element_count,
    MYON_StringMapOrdinalType mapping
    );

  void AccumulateString(
    const class MYON_String& str,
    MYON_StringMapOrdinalType mapping
    );

  void AccumulateString(
    const class MYON_wString& str,
    MYON_StringMapOrdinalType mapping
    );


  void AccumulateSubHash(
    const class MYON_SHA1_Hash& sub_hash
    );

private:
  void Internal_SwapBigEndianUpdate(
    const void* buffer,
    MYON__UINT64 sizeof_buffer
  );

public:
  /*
  Returns:
    Total number of bytes passed to Update().
  */
  MYON__UINT64 ByteCount() const;

  /*
  Returns:
    SHA-1 hash value of the sequenence of ByteCount() bytes that have been
    passed to this MYON_SHA1 classe's Update() function since construction
    or the last call to Reset().
  Remarks:
    You may use Hash() to compute intermediate SHA-1 hash values. 
    
    Put another way, you may call Update() zero or more times passing in N1 bytes, 
    call Digest() to get the SHA-1 hash of those N1 bytes, make zero or more additional
    calls to Update() passing in N2 additional bytes, call digest to get the SHA-1 hash
    of the sequence of (N1 + N2) bytes, and so on.
  */
  MYON_SHA1_Hash Hash() const;

  /*
  Description:
    Reset this MYON_SHA1 class so it can be reused.  
  */
  void Reset();

  /*
  Description:
    This is a static function that uses MYON_SHA1 to compute SHA-1 hash values
    of sequences of bytes with known SHA-1 hash values and compares the 
    results from MYON_SHA1 with the known SHA-1 hash values.

    This function can be used to validate the MYON_SHA1 class compiled correctly.

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

  MYON__UINT64 m_byte_count = 0; // number of bytes that have passed through calls to update().
  // if 1 == m_status_bits & 1, then update has been called at least once (perhaps with 0 bytes).
  // if 2 == m_status_bits & 2, then m_sha1_hash is current.
  mutable MYON__UINT32 m_status_bits = 0;
  MYON__UINT32 m_reserved = 0;  

  // current "remainder"
  MYON__UINT8 m_buffer[64];      // bytes that didn't fit in last 64 byte chunk
	MYON__UINT32 m_bit_count[2];   // number of bits (lo, hi)
	MYON__UINT32 m_state[5];       // current state

  // cached SHA1 hash - valid if 2 = (2 & m_status_bits)
	mutable MYON_SHA1_Hash m_sha1_hash;
};

/*
Description:
  Test speeds of various hash algoritmhs.
Parameters:
  byte_count - [in]
    Number of bytes to hash. This number is rounded up to the nearest multiple of 1024.
  crc16 - [in/out]
    If crc16 is not nullptr, then 16 bit CRC hashing is tested using function MYON_CRC16().
  crc32 - [in/out]
    If crc32 is not nullptr, then 32 bit CRC hashing is tested using function MYON_CRC32().
  md5_hash - [in/out]
    If md5_hash is not nullptr, then MD5 hashing is tested using class MYON_MD5.
  sha1_hash - [in/out]
    If sha1_hash is not nullptr, then SHA-1 hashing is tested class MYON_SHA1.
  elapsed_time_in_seconds - [out]
    elapsed_time_in_seconds[0] = 16 bit CRC hash time in seconds.
    elapsed_time_in_seconds[1] = 32 bit CRC hash time in seconds.
    elapsed_time_in_seconds[2] = MD5 hash time in seconds.
    elapsed_time_in_seconds[3] = SHA-1 hash time in seconds.
    If a hash was tested, then number of seconds it took to compute the hash is returned.
    Otherwise MYON_DBL_QNAN is returned.
*/
MYON_DECL
void MYON_TestHashSpeed(
  size_t byte_count,
  MYON__UINT16* crc16,
  MYON__UINT32* crc32,
  MYON_MD5_Hash* md5_hash,
  MYON_SHA1_Hash* sha1_hash,
  double elapsed_time_in_seconds[4]
);

/*
Description:
  Test speeds of various hash algoritmhs and use text_log to print the results.
Parameters:
  byte_count - [in]
    Number of bytes to hash. This number is rounded up to the nearest multiple of 1024.
  bCRC16 - [in]
    True to test 16 bit CRC hashing speed.
  bCRC32 - [in]
    True to test 32 bit CRC hashing speed.
  bMD5 - [in]
    True to test MD5 hashing speed.
  bSHA1 - [in]
    True to test SHA-1 hashing speed.
  text_log - [in]
    Test results are printed using text_log.
*/
MYON_DECL
void MYON_TestHashSpeed(
  size_t byte_count,
  bool bTestCRC16,
  bool bTestCRC32,
  bool bTestMD5,
  bool bTestSHA1,
  MYON_TextLog& text_log
);

MYON_DECL
void MYON_TestHashSpeed(
  size_t byte_count,
  MYON_TextLog& text_log
);

#endif

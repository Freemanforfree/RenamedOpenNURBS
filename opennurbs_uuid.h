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

#if !defined(OPENNURBS_UUID_INC_)
#define OPENNURBS_UUID_INC_

// MYON_UUID is a 16 byte universally unique identifier
#if !defined(MYON_UUID_DEFINED)
  // This code also exists in version.h, but needs to be here
  // to solve include ordering problems, and for standalone
  // opennurbs.dll builds.
  #define MYON_UUID_DEFINED
  // MYON_UUID is a 16 byte universally unique identifier
  #if defined(UUID_DEFINED)
  typedef UUID MYON_UUID;
  #elif defined(GUID_DEFINED)
  typedef GUID MYON_UUID;
  #else
  typedef struct MYON_UUID_struct
  {
    unsigned int   Data1;    // 32 bit unsigned integer
    unsigned short Data2;    // 16 bit unsigned integer
    unsigned short Data3;    // 16 bit unsigned integer
    unsigned char  Data4[8];
    } MYON_UUID;
  #endif
#endif


MYON_BEGIN_EXTERNC

// All bits are zero in MYON_nil_uuid and
// MYON_UuidCompare( MYON_nil_uuid, U ) < 0 if U != MYON_nil_uuid.
extern MYON_EXTERN_DECL const MYON_UUID MYON_nil_uuid;

// All bits are one in MYON_max_uuid and
// MYON_UuidCompare( U, MYON_max_uuid ) < 0 if U != MYON_max_uuid.
extern MYON_EXTERN_DECL const MYON_UUID MYON_max_uuid;

// Application ids for the versions of Rhino that
// write 3dm files.  All userdata classed defined
// in the core Rhino.exe should use these ids
// as the application id.
// In situations where you want to use the id
// for the current version of Rhino, use
// MYON_rhino_id and you won't have to update
// your code when Rhino versions roll.
extern MYON_EXTERN_DECL const MYON_UUID MYON_rhino2_id;
extern MYON_EXTERN_DECL const MYON_UUID MYON_rhino3_id;
extern MYON_EXTERN_DECL const MYON_UUID MYON_rhino4_id;
extern MYON_EXTERN_DECL const MYON_UUID MYON_rhino5_id;
extern MYON_EXTERN_DECL const MYON_UUID MYON_rhino6_id;
extern MYON_EXTERN_DECL const MYON_UUID MYON_rhino7_id;
extern MYON_EXTERN_DECL const MYON_UUID MYON_rhino8_id;
extern MYON_EXTERN_DECL const MYON_UUID MYON_rhino_id;

// Application ids for usedata written by versions
// of opennurbs before userdata had application ids.
extern MYON_EXTERN_DECL const MYON_UUID MYON_v2_userdata_id;
extern MYON_EXTERN_DECL const MYON_UUID MYON_v3_userdata_id;
extern MYON_EXTERN_DECL const MYON_UUID MYON_v4_userdata_id;

// Application id for the versions of openNURBS that
// write userdata in 3dm files.  User data whose class
// definition is in opennurbs should use these
// ids as the user data application id.
// No other user data should use these ids.
//
// In situations where you want to use the id
// for the current version of opennurbs, use
// MYON_opennurbs_id and you won't have to update
// your code when opennurbs versions roll.
#if defined(MYON_COMPILING_OPENNURBS)
extern const MYON_UUID MYON_opennurbs4_id;
extern const MYON_UUID MYON_opennurbs5_id;
extern const MYON_UUID MYON_opennurbs6_id;
extern const MYON_UUID MYON_opennurbs7_id;
extern const MYON_UUID MYON_opennurbs8_id;
#endif
extern MYON_EXTERN_DECL const MYON_UUID MYON_opennurbs_id;

/*
Description:
  Determine if an id is a Rhino application id.
Parameters:
  id - [in]
    id to test
Returns:
  0: id is not a Rhino application id
  N: id == MYON_rhinoN_id, where "N" = 2,3,4,5,6,...
*/
MYON_DECL
unsigned int MYON_IsRhinoApplicationId(
  MYON_UUID id
  );

/*
Description:
  Determine if an id is an opennurbs application id.
Parameters:
  id - [in]
    id to test
Returns:
  0: id is not an opennurbs application id
  N: id == MYON_opennurbsN_id, where "N" = 4,5,6,...
*/
MYON_DECL
unsigned int MYON_IsOpennurbsApplicationId(
  MYON_UUID id
  );

MYON_END_EXTERNC

#if defined(MYON_CPLUSPLUS)

/*
Description:
  Creates a new uuid.(&a,&b) compares two uuids.
Parameters:
  new_uuid - [out]
Returns:
  True if successful.
*/
MYON_DECL 
bool MYON_CreateUuid( MYON_UUID& uuid );

MYON_DECL 
MYON_UUID MYON_CreateId();

/*
Description:
  This function is used for in testing situations when an
  MYON_UUID value needs to be repeatable.
Parameter:
  index_64_bit - [in]
    index_64_bit > 0 and index_64_bit <= 0X0000FFFFFFFFFFFF.
    Calling this function with the same index will create 
    an MYON_UUID with the same value.
Returns:
  If index_64_bit = 0 or >0X0000FFFFFFFFFFFF, 
  then MYON_nil_uuid is returned.
  Otherwise, a MYON_UUID with the value
  xxxxxxxx-yyyy-11dc-9885-001372C33878
  where xxxxxxxxyyyy = the hexadecimal representation
  of index_64_bit.
  The returned id is not unique.
*/
MYON_DECL
MYON_UUID MYON_NotUniqueIdFromIndex(
  MYON__UINT64 index_64_bit
);

/*
Description:
  This function is used for in testing situations when an
  MYON_UUID value needs to be repeatable.
Parameter:
  index_32_bit - [in]
    index_32_bit > 0.
    Calling this function with the same index will create 
    an MYON_UUID with the same value.
Returns:
  If index_32_bit = 0, then MYON_nil_uuid is returned.
  Otherwise, a MYON_UUID with the value
  xxxxxxxx-yyyy-11dc-9885-001372C33878
  where xxxxxxxxyyyy = the hexadecimal representation
  of index_32_bit.
  The returned id is not unique.
*/
MYON_DECL
MYON_UUID MYON_NotUniqueIdFromIndex(
  MYON__UINT32 index_32_bit
);


/*
Description:
  This function is used for in testing situations when an
  MYON_UUID value needs to be repeatable.
Parameter:
  not_unique_id - [in]
    A not unique id created by MYON_NotUniqueIdFromIndex() or MYON_NextNotUniqueId().
Returns:
  If not_unique_id was created by on of the NotUniqueId functions,
  then a MYON_UUID with the value
  xxxxxxxx-yyyy-11dc-9885-001372C33878
  where xxxxxxxxyyyy = the hexadecimal representation
  of the int id's index + 1 is returned.
  Otherwise, NO_nil_uuid is returned.
  The returned id is not unique.
*/
MYON_DECL
MYON_UUID MYON_NextNotUniqueId(
  MYON_UUID not_unique_id
);

/*
Description:
  This function is used for in testing situations when an
  MYON_UUID value needs to be repeatable.
Parameter:
  not_unique_id - [in]
    A not unique id created by MYON_NotUniqueIdFromIndex() or MYON_NextNotUniqueId().
Returns:
  If not_unique_id was created by on of the NotUniqueId functions,
  then the index of that id is returned. 
  Otherwise, 0 is return if not_unique_id is nil and
  0xFFFF00000000 is returned in all other cases.
*/MYON_DECL
MYON__UINT64 MYON_IndexFromNotUniqueId(
  MYON_UUID not_unique_id
);



//class MYON_CLASS MYON_Id : public MYON_UUID
//{
//public:
//  MYON_Id() = default;
//  ~MYON_Id() = default;
//  MYON_Id(const MYON_Id&) = default;
//  MYON_Id& operator=(const MYON_Id&) = default;
//
//  MYON_Id(const MYON_UUID&);
//  MYON_Id& operator=(const MYON_UUID&);
//
//  operator MYON_UUID&();
//  operator const MYON_UUID&() const;
//  
//  static MYON_Id Create();
//  static const MYON_Id nil;
//
//  static int Compare(
//    const MYON_Id& a, 
//    const MYON_Id& b
//    );
//
//  static int ComparePtr(
//    const MYON_Id* a, 
//    const MYON_Id* b
//    );
//
//  static int CompareUUID(
//    const MYON_UUID& a, 
//    const MYON_UUID& b
//    );
//
//  static int CompareUUIDPtr(
//    const MYON_UUID* a, 
//    const MYON_UUID* b
//    );
//
//  bool IsNil() const;
//  bool IsNotNil() const;
//};
//
//bool operator==(const MYON_Id& a, const MYON_Id& b );
//bool operator==(const MYON_UUID& a, const MYON_Id& b );
//bool operator==(const MYON_Id& a, const MYON_UUID& b );
//
//bool operator!=(const MYON_Id& a, const MYON_Id& b );
//bool operator!=(const MYON_UUID& a, const MYON_Id& b );
//bool operator!=(const MYON_Id& a, const MYON_UUID& b );
//
//bool operator<=(const MYON_Id& a, const MYON_Id& b );
//bool operator<=(const MYON_UUID& a, const MYON_Id& b );
//bool operator<=(const MYON_Id& a, const MYON_UUID& b );
//
//bool operator>=(const MYON_Id& a, const MYON_Id& b );
//bool operator>=(const MYON_UUID& a, const MYON_Id& b );
//bool operator>=(const MYON_Id& a, const MYON_UUID& b );
//
//bool operator<(const MYON_Id& a, const MYON_Id& b );
//bool operator<(const MYON_UUID& a, const MYON_Id& b );
//bool operator<(const MYON_Id& a, const MYON_UUID& b );
//
//
//bool operator>(const MYON_Id& a, const MYON_Id& b );
//bool operator>(const MYON_UUID& a, const MYON_Id& b );
//bool operator>(const MYON_Id& a, const MYON_UUID& b );


/*
Description:
  This class is used by MYON_UuidIndexList.  It is used when
  uuids are used to search for items that can be found by
  an integer index.
*/
class MYON_CLASS MYON_UuidIndex
{
public:
  static const MYON_UuidIndex NilIndex; // uuid = nil, index = 0

  MYON_UuidIndex() = default;
  ~MYON_UuidIndex() = default;
  MYON_UuidIndex(const MYON_UuidIndex&) = default;
  MYON_UuidIndex& operator=(const MYON_UuidIndex&) = default;

  MYON_UuidIndex(MYON_UUID id, int index);

  /*
  Dictionary compare m_id and then m_i.
  */
  static 
  int CompareIdAndIndex( const MYON_UuidIndex* a, const MYON_UuidIndex* b );

  /*
  Dictionary compare m_id and then m_i.
  */
  static 
  int CompareIndexAndId( const MYON_UuidIndex* a, const MYON_UuidIndex* b );

  /*
  Compare m_id and ignore m_i.
  */
  static 
  int CompareId( const MYON_UuidIndex* a, const MYON_UuidIndex* b );

  /*
  Compare m_i and ignore m_id.
  */
  static 
  int CompareIndex( const MYON_UuidIndex* a, const MYON_UuidIndex* b );

  // In cases when there is a discrepancy between the m_id and
  // m_i, m_id is assumed to be valid unless comments where this
  // class is used indicate otherwise.
  MYON_UUID m_id = MYON_nil_uuid;
  int m_i = 0;
};


/*
Description:
  This class is used by MYON_UuidIndexList.  It is used when
  uuids are used to search for items that can be found by
  an integer index.
*/
class MYON_CLASS MYON_UuidPtr
{
public:
  static const MYON_UuidPtr NilPtr; // uuid = nil, ptr = 0;

  MYON_UuidPtr() = default;
  ~MYON_UuidPtr() = default;
  MYON_UuidPtr(const MYON_UuidPtr&) = default;
  MYON_UuidPtr& operator=(const MYON_UuidPtr&) = default;

  /*
  Dictionary compare m_id and then m_ptr.
  */
  static 
  int CompareIdAndPtr( const MYON_UuidPtr* a, const MYON_UuidPtr* b );

  /*
  Dictionary compare m_id and then m_ptr.
  */
  static 
  int ComparePtrAndId( const MYON_UuidPtr* a, const MYON_UuidPtr* b );

  /*
  Compare m_id and ignore m_ptr.
  */
  static 
  int CompareId( const MYON_UuidPtr* a, const MYON_UuidPtr* b );

  /*
  Compare m_ptr and ignore m_id.
  */
  static 
  int ComparePtr( const MYON_UuidPtr* a, const MYON_UuidPtr* b );

  // In cases when there is a discrepancy between the m_id and
  // m_ptr, m_id is assumed to be valid unless comments where this
  // class is used indicate otherwise.
  MYON_UUID m_id = MYON_nil_uuid;
  MYON__UINT_PTR m_ptr = 0;
};

/*
Description:
  MYON_UuidCompare(&a,&b) compares two uuids.
Parameters:
  a - [in]
  b - [in]
Returns:
  @untitled table
  -1    a < b
   0    a == b
  +1    a > b
Remarks:
  A nullptr pointer is considered < a non-nullptr pointer.
*/
MYON_DECL 
int MYON_UuidCompare( 
        const MYON_UUID* a, 
        const MYON_UUID* b 
        );

/*
Description:
  MYON_UuidCompare(a,b) compares two uuids.
Parameters:
  a - [in]
  b - [in]
Returns:
  @untitled table
  -1    a < b
   0    a == b
  +1    a > b
*/
MYON_DECL 
int MYON_UuidCompare( 
        const MYON_UUID& a, 
        const MYON_UUID& b
        );

/*
Description:
  Test uuid to see if it is nil (identically zero).
Parameters:
  uuid - [in]
Returns:
  true if uuid is nil.
*/
MYON_DECL
bool MYON_UuidIsNil( 
        const MYON_UUID& uuid 
        );

/*
Description:
  Test uuid to see if it is not nil (not identically zero).
Parameters:
  uuid - [in]
Returns:
  true if uuid is not nil (non zero)
*/
MYON_DECL
bool MYON_UuidIsNotNil( 
        const MYON_UUID& uuid 
        );

/*
Description:
  Converts a string like
    "{85A08515-f383-11d3-BFE7-0010830122F0}" 
  into a uuid.
  The brackets are optional and are ignored.
  Hyphens can appear anywhere or be missing.
  The hex digits can be upper or lower case.
Parameters:
  s - [in]
Returns:
  uuid.  
  If the string is not a uuid, then MYON_nil_uuid is returned.
*/
MYON_DECL 
MYON_UUID MYON_UuidFromString( const char* s );

/*
Description:
  Converts a string like
    "{85A08515-f383-11d3-BFE7-0010830122F0}" 
  into a uuid.
  The brackets are optional and are ignored.
  Hyphens can appear anywhere or be missing.
  The hex digits can be upper or lower case.
Parameters:
  s - [in]
Returns:
  uuid.  
  If the string is not a uuid, then MYON_nil_uuid is returned.
*/
MYON_DECL 
MYON_UUID MYON_UuidFromString( const wchar_t* s );

/*
Description:
  Converts a uuid to a null termintated ASCII string like 
     "85a08515-f383-11d3-bfe7-0010830122f0". 
Parameters:
  uuid - [in]
  s - [out]  The s[] char array must have length >= 37.  
             The returned char array will have a 36 
             character uuid in s[0..35] and a null in s[36].
Returns:
  The pointer to the array is returned.
*/
MYON_DECL 
char* MYON_UuidToString( const MYON_UUID& uuid, char* s );


/*
Description:
  Converts a uuid to a null termintated UNICODE string like 
     "85a08515-f383-11d3-bfe7-0010830122f0". 
Parameters:
  uuid - [in]
  s - [out]  The s[] wchar_t array must have length >= 37.  
             The returned char array will have a 36 
             character uuid in s[0..35] and a null in s[36].
Returns:
  The pointer to the array is returned.
*/
MYON_DECL 
wchar_t* MYON_UuidToString( const MYON_UUID& uuid, wchar_t* s );

/*
Description:
  Parses a string like "85a08515-f383-11d3-bfe7-0010830122f0" 
  and returns the value as a uuid. Hyphens can appear anywhere
  and are ignored. Parsing stops at any character that is not
  a hexadecimal digit or hyphen.  Parsing stops after 32 hexadecimal
  digits are read;
Parameters:
  sUuid - [in]
  uuid - [out]
Returns:
  null 
    Parsing failed and uuid is set to MYON_nil_uuid.
  non null 
    Parsing succeeded and uuid is set to the parsed value.
    The returned pointer is the character in sUUID immediately
    after the last parsed character.
*/
MYON_DECL
const char* MYON_ParseUuidString(
  const char* sUUID,
  MYON_UUID* uuid
  );

class MYON_String;

/*
Description:
  Converts a uuid to a null termintated string like 
     "85a08515-f383-11d3-bfe7-0010830122f0". 
Parameters:
  uuid - [in]
  s - [out]
Returns:
  The pointer to the array is returned.
*/
MYON_DECL 
const char* MYON_UuidToString( const MYON_UUID& uuid, MYON_String& s);

class MYON_wString;

/*
Description:
  Converts a uuid to a null termintated string like 
     "85a08515-f383-11d3-bfe7-0010830122f0". 
Parameters:
  uuid - [in]
  s - [out]
Returns:
  The pointer to the array is returned.
*/
MYON_DECL 
const wchar_t* MYON_UuidToString( const MYON_UUID& uuid, MYON_wString& s);

MYON_DECL
const MYON_wString MYON_IdToString(
  MYON_UUID id
);

MYON_DECL
const MYON_wString MYON_AddIdPrefixToString(
  const MYON_UUID id,
  const wchar_t* separator,
  const wchar_t* source
  );

MYON_DECL
const MYON_wString MYON_RemoveIdPrefixFromString(
  const MYON_UUID id,
  const wchar_t* separator,
  const wchar_t* source
);

MYON_DECL
const MYON_wString MYON_AddIdSuffixToString(
  const wchar_t* source,
  const wchar_t* separator,
  const MYON_UUID id
);

MYON_DECL
const MYON_wString MYON_RemoveIdSuffixFromString(
  const wchar_t* source,
  const wchar_t* separator,
  const MYON_UUID id
);


#endif

#endif

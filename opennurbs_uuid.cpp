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
#if defined(MYON_RUNTIME_LINUX)
#include "android_uuid/uuid.h"
#endif
#if !defined(MYON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// MYON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, MYON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error MYON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

#if !defined(UUID_DEFINED) && !defined(GUID_DEFINED)

// When MYON_UUID is a typdef for Microsoft 's UUID,
// the Microsoft compiler handles == and !=.
// When a MYON_UUID is not a typedef for a Microsoft UUID,
// it is declared as a class and operator== and operator!= 
// need to be explicitly defined.

bool operator==(const struct MYON_UUID_struct& a, const struct MYON_UUID_struct& b)
{
  return (0==memcmp(&a,&b,sizeof(struct MYON_UUID_struct)));
}

bool operator!=(const struct MYON_UUID_struct& a, const struct MYON_UUID_struct& b)
{
  return (0!=memcmp(&a,&b,sizeof(struct MYON_UUID_struct)));
}

#endif

// used to map the correspondence between uuid strings and
// MYON_UUIDs as an array of 16 bytes.

// for little endian CPUs (Intel, etc)
static const int little_endian_rho[16] = {3,2,1,0, 5,4, 7,6, 8,9, 10,11,12,13,14,15};

// for big endian CPUs (Motorola, MIPS, Sparc, etc.)
static const int big_endian_rho[16] = {0,1,2,3, 4,5, 6,7, 8,9, 10,11,12,13,14,15};

 
MYON_UUID MYON_CreateId()
{
  MYON_UUID id;
  MYON_CreateUuid(id);
  return id;
}

static const MYON_UUID not_unique_id_base = {
    0,                                        // unsigned long  Data1;
    0,                                        // unsigned short Data2;
    // The Data 3 and Data4 values are based on the MAC address of a
    // network card that was destroyed circa 2000.
    0x11dc,                                   // unsigned short Data3;
    {0x98,0x85,0x00,0x13,0x72,0xc3,0x38,0x78} // unsigned char  Data4[8];
  };

static const MYON_UUID MYON_Internal_CreateNotUniqueSequentialId(
  MYON__UINT64 index_64_bit
)
{
  // Creates a not unique and repeatable UUID value that has a valid format.
  //
  // It is based on the MAC address of a network card that
  // was destroyed circa 2000. The Time portion of the UUID is generated
  // from index_64_bit and will generall be well before the current time.
  // The reason for using this complicated approach is to insure
  // data structures usisg these i values will pass validty checking
  // that tests to see if the UUID has a valid format.

  if (0 == index_64_bit)
  {
    MYON_ERROR("index_64_bit parameter cannot be zero.");
    return MYON_nil_uuid;
  }

  const MYON__UINT64 d2 = 0x10000;
  const MYON__UINT64 data2 = index_64_bit % d2;
  const MYON__UINT64 data1 = index_64_bit / d2;
  if ( data1 > 0xFFFFFFFF )
  {
    MYON_ERROR("index_64_bit parameter is too large.");
    return MYON_nil_uuid;
  }

  MYON_UUID not_unique_id = not_unique_id_base;
  not_unique_id.Data1 = (MYON__UINT32)data1;
  not_unique_id.Data2 = (MYON__UINT16)data2;
  return not_unique_id;
}

MYON_UUID MYON_NotUniqueIdFromIndex(
  MYON__UINT64 index_64_bit
)
{
  return MYON_Internal_CreateNotUniqueSequentialId(index_64_bit);
}

MYON_UUID MYON_NotUniqueIdFromIndex(
  MYON__UINT32 index
)
{
  return MYON_Internal_CreateNotUniqueSequentialId((MYON__UINT64)index);
}

MYON_UUID MYON_NextNotUniqueId(
  MYON_UUID current_not_unique_id
)
{
  if (MYON_nil_uuid == current_not_unique_id)
    return MYON_Internal_CreateNotUniqueSequentialId(1);

  const MYON__UINT64 current_not_unique_id_index = MYON_IndexFromNotUniqueId(current_not_unique_id);
  return MYON_Internal_CreateNotUniqueSequentialId(current_not_unique_id_index+1);
}

MYON__UINT64 MYON_IndexFromNotUniqueId(
  MYON_UUID not_unique_id
)
{
  if (
    not_unique_id.Data3 == not_unique_id_base.Data3
    && *((const MYON__UINT32*)(&not_unique_id.Data4[0])) == *((const MYON__UINT32*)(&not_unique_id_base.Data4[0]))
    && *((const MYON__UINT32*)(&not_unique_id.Data4[4])) == *((const MYON__UINT32*)(&not_unique_id_base.Data4[4]))
    )
  {
    const MYON__UINT64 data1 = (MYON__UINT64)not_unique_id.Data1;
    const MYON__UINT64 data2 = (MYON__UINT64)not_unique_id.Data2;
    const MYON__UINT64 index = data1 * 0x10000 + data2;
    return index;
  }

  MYON_ERROR("not_unique_id was not created by MYON_NotUniqueIdFromIndex().");
  return (MYON_nil_uuid == not_unique_id) ? 0 : 0xFFFF00000000;
}

bool MYON_CreateUuid( MYON_UUID& new_uuid )
{
  // See http://www.faqs.org/rfcs/rfc4122.html for uuid details.

#if 0
  {
    // Use this code when testing reqires "repeatable uniqueness".
    // NEVER check in this code.
    static MYON_UUID x = MYON_nil_uid;
    x = MYON_NextNotUniqueId(x);
    new_uuid = x;
#pramga message("warning: NEVER COMMIT THIS CODE - MYON_CreateUuid in TEST MODE.")
  }
  return true;

#else

#if defined(MYON_COMPILER_MSC)
  // Header: Declared in Rpcdce.h.
  // Library: Use Rpcrt4.lib  
#pragma comment(lib, "Rpcrt4.lib")
  ::UuidCreate(&new_uuid);
  //::UuidCreateSequential(&new_uuid); // faster but computer MAC address
                                       // identifies the user and some
                                       // customers may object.
  return true;
#elif defined(MYON_RUNTIME_APPLE)
  // Header: #include <uuid/uuid.h>
  if ( MYON::endian::little_endian == MYON::Endian() )
  {
    // Intel cpu mac
    // The uuid_generate() function returns a UUID in network or 
    // big-endian order.  The rest of OpenNURBS assumes that a UUID
    // is stored in native byte order, so we switch the byte order
    // of the UUID.
    uuid_t apple_osx_uuid;
    uuid_generate(apple_osx_uuid);
    unsigned char* dst = (unsigned char*)&new_uuid;
    const unsigned char* src = (const unsigned char*)&apple_osx_uuid;
    *dst++ = src[little_endian_rho[ 0]]; 
    *dst++ = src[little_endian_rho[ 1]]; 
    *dst++ = src[little_endian_rho[ 2]]; 
    *dst++ = src[little_endian_rho[ 3]]; 
    *dst++ = src[little_endian_rho[ 4]]; 
    *dst++ = src[little_endian_rho[ 5]]; 
    *dst++ = src[little_endian_rho[ 6]]; 
    *dst++ = src[little_endian_rho[ 7]]; 
    *dst++ = src[little_endian_rho[ 8]]; 
    *dst++ = src[little_endian_rho[ 9]]; 
    *dst++ = src[little_endian_rho[10]]; 
    *dst++ = src[little_endian_rho[11]]; 
    *dst++ = src[little_endian_rho[12]]; 
    *dst++ = src[little_endian_rho[13]]; 
    *dst++ = src[little_endian_rho[14]]; 
    *dst   = src[little_endian_rho[15]]; 
  }
  else
  {
    // Motorola cpu mac
    uuid_generate((unsigned char*)&new_uuid);
  }
  
#if defined (MYON_DEBUG)
  // OS X generates version 4 UUIDs.  Check that this is still true after changing the byte order.
  if ((new_uuid.Data3 & 0xF000) != 0x4000)
    MYON_ERROR("MYON_CreateUuid() failure 1");
  if (new_uuid.Data4[0] < 0x80 || new_uuid.Data4[0] >= 0xC0)
    MYON_ERROR("MYON_CreateUuid() failure 2");
#endif

  return true;
#else

#if defined(MYON_RUNTIME_LINUX)
  uuid_generate((unsigned char*)&new_uuid);
  return true;
#else

  // You must supply a way to create unique ids or you 
  // will not be able to write 3dm files.
#error TODO - generate uuid
  memset(&new_uuid,0,sizeof(MYON_UUID));
  return false;
#endif

#endif
#endif
}

const char* MYON_ParseUuidString(const char* sUUID, MYON_UUID* uuid)
{
  // NOTE WELL: This code has to work on non-Windows OSs and on
  //            both big and little endian CPUs.  On Windows OSs
  //            is must return the same result as 
  //            Windows's UuidFromString().
  //

  // string has format like "85A08515-F383-11d3-BFE7-0010830122F0"
  // or like "85A08515-F383-11d3-BFE7-0010830122F0".
  // Hyphens are optional and ignored.
  //
  // Windows users can use "guidgen" to create UUID strings.

  /*
  #if defined(MYON_DEBUG) && defined(MYON_RUNTIME_WIN)
  RPC_STATUS st;
  union
  {
  MYON_UUID uuid;
  unsigned char b[16];
  } u1;
  st = UuidFromString( (unsigned char*)sUUID, &u1.uuid );
  #endif
  */

  static const int* rho = (MYON::endian::big_endian == MYON::Endian())
    ? big_endian_rho
    : little_endian_rho;

  union
  {
    MYON_UUID uuid;
    unsigned char b[16];
  } u;
  bool bFailed;
  int bi, ci;
  unsigned char c;
  unsigned char byte_value[2];

  memset(&u, 0, sizeof(u));
  //for ( bi = 0; bi < 16; bi++ ) 
  //  u.b[bi] = 0;

  bFailed = sUUID ? false : true;

  if (!bFailed)
  {
    for (bi = 0; bi < 16; bi++)
    {
      ci = 0;
      byte_value[0] = 0;
      byte_value[1] = 0;
      while (ci < 2)
      {
        c = *sUUID;
        if (!c) {
          bFailed = true;
          break;
        }
        if (c >= 'A' && c <= 'F') {
          byte_value[ci++] = (c - 'A' + 10);
        }
        else if (c >= '0' && c <= '9') {
          byte_value[ci++] = (c - '0');
        }
        else if (c >= 'a' && c <= 'f') {
          byte_value[ci++] = (c - 'a' + 10);
        }
        else if (c != '-') {
          bFailed = true;
          break;
        }
        sUUID++;
      }
      if (bFailed)
        break;
      u.b[rho[bi]] = 16 * byte_value[0] + byte_value[1];
    }
  }

  if (bFailed) 
  {
    // 09 August 2006 John Morse
    // There are times when Rhino is looking for a plug-in but the SDK or command
    // allows the plug-in to be specified by name or UUID.  Rhino calls MYON_UuidFromString()
    // to see if the string is a plug-in UUID so it knows if it should be comparing the string
    // or plug-in name when looking for a plug-in.  The MYON_ERROR line makes the Rhino commands
    // generate an OpenNURBS message box (in DEBUG builds) when the command completes and is
    // a pain so I commented it out per Dale Lear.
    //MYON_ERROR("MYON_UuidFromString(): bad string passed in");
    u.uuid = MYON_nil_uuid;
  }

  if (uuid)
    *uuid = u.uuid;

  return bFailed ? 0 : sUUID;
}

 
MYON_UUID MYON_UuidFromString( const char* sUUID )
{
  // NOTE WELL: This code has to work on non-Windows OSs and on
  //            both big and little endian CPUs.  On Windows OSs
  //            is must return the same result as 
  //            Windows's UuidFromString().
  //

  // string has format like "85A08515-F383-11d3-BFE7-0010830122F0"
  // or like "{85A08515-F383-11d3-BFE7-0010830122F0}".  Brackets
  // and hyphens are optional and ignored.
  //
  // Windows users can use "guidgen" to create UUID strings.

  /*
#if defined(MYON_DEBUG) && defined(MYON_RUNTIME_WIN)
  RPC_STATUS st;
  union 
  {
    MYON_UUID uuid;
    unsigned char b[16];
  } u1;
  st = UuidFromString( (unsigned char*)sUUID, &u1.uuid );
#endif
*/

  static const int* rho = (MYON::endian::big_endian == MYON::Endian())
                        ? big_endian_rho 
                        : little_endian_rho;

  union 
  {
    MYON_UUID uuid;
    unsigned char b[16];
  } u;
  bool bFailed;
  int bi, ci;
  unsigned char c;
  unsigned char byte_value[2];

  memset(&u,0,sizeof(u));
  //for ( bi = 0; bi < 16; bi++ ) 
  //  u.b[bi] = 0;

  bFailed = sUUID ? false : true;

  if ( !bFailed ) {
    while ( *sUUID && *sUUID <= ' ' ) // skip leading white space
      sUUID++;
    if ( *sUUID == '{' )
      sUUID++;
    for ( bi = 0; bi < 16; bi++ ) {
      ci = 0;
      byte_value[0] = 0;
      byte_value[1] = 0;
      while ( ci < 2 ) {
        c = *sUUID++;
        if ( !c ) {
          bFailed = true;
          break;
        }
        if ( c >= 'A' && c <= 'F' ) {
          byte_value[ci++] = (c-'A'+10);
        }
        else if ( c >= '0' && c <='9' ) {
          byte_value[ci++] = (c-'0');
        }
        else if ( c >= 'a' && c <= 'f' ) {
          byte_value[ci++] = (c-'a'+10);
        }
        else if ( c != '-' ) {
          bFailed = true;
          break;
        }
      }
      if ( bFailed )
        break;
      u.b[rho[bi]] = 16*byte_value[0] + byte_value[1];
    }
  }

  if ( bFailed ) {
    // 09 August 2006 John Morse
    // There are times when Rhino is looking for a plug-in but the SDK or command
    // allows the plug-in to be specified by name or UUID.  Rhino calls MYON_UuidFromString()
    // to see if the string is a plug-in UUID so it knows if it should be comparing the string
    // or plug-in name when looking for a plug-in.  The MYON_ERROR line makes the Rhino commands
    // generate an OpenNURBS message box (in DEBUG builds) when the command completes and is
    // a pain so I commented it out per Dale Lear.
    //MYON_ERROR("MYON_UuidFromString(): bad string passed in");
    u.uuid = MYON_nil_uuid;
  }

/*
#if defined(MYON_DEBUG) && defined(MYON_RUNTIME_WIN)
  if ( memcmp( &u.uuid, &u1.uuid, 16 ) ) {
    MYON_ERROR("MYON_UuidFromString() failed");
  }
  if ( UuidCompare( &u.uuid, &u1.uuid, &st ) ) {
    MYON_ERROR("MYON_UuidFromString() failed");
  }
  if ( MYON_UuidCompare( &u.uuid, &u1.uuid ) ) {
    MYON_ERROR("MYON_UuidCompare() failed");
  }
#endif
*/
  return u.uuid;
}


MYON_UUID MYON_UuidFromString( const wchar_t* sUUID )
{
  wchar_t w;
  char s[64];
  int i;
  if( nullptr == sUUID )
    return MYON_nil_uuid;
  while ( *sUUID && *sUUID <= ' ' ) // skip leading white space
    sUUID++;
  if ( *sUUID == '{' )
    sUUID++;
  i = 0;
  while (i < 63 )
  {
    w = *sUUID++;
    if ( w >= 'A' && w <= 'F' )
      s[i++] = (char)w;
    else if ( w >= '0' && w <='9' )
      s[i++] = (char)w;
    else if ( w >= 'a' && w <= 'f' )
      s[i++] = (char)w;
    else if ( w != '-' ) 
      break;
  }
  s[i] = 0;

  return MYON_UuidFromString(s);

}
 
MYON_UuidIndex::MYON_UuidIndex(MYON_UUID id, int index)
  : m_id(id)
  , m_i(index)
{}
int MYON_UuidIndex::CompareIdAndIndex( const MYON_UuidIndex* a, const MYON_UuidIndex* b )
{
  int i;
  if ( !a )
    return (b ? -1 : 0 );
  if ( !b )
    return 1;

  // compare id first
  if (0 == (i = MYON_UuidCompare(&a->m_id, &b->m_id)))
  {
    if (a->m_i < b->m_i)
      i = -1;
    else if (a->m_i > b->m_i)
      i = 1;
  }

  return i;
}

int MYON_UuidIndex::CompareIndexAndId( const MYON_UuidIndex* a, const MYON_UuidIndex* b )
{
  int i;
  if ( !a )
    return (b ? -1 : 0 );
  if ( !b )
    return 1;

  // compare index first
  if (a->m_i < b->m_i)
    i = -1;
  else if (a->m_i > b->m_i)
    i = 1;
  else
    i = MYON_UuidCompare(&a->m_id,&b->m_id);

  return i;
}

int MYON_UuidIndex::CompareId( const MYON_UuidIndex* a, const MYON_UuidIndex* b )
{
  if ( !a )
    return (b ? -1 : 0 );
  if ( !b )
    return 1;
  return MYON_UuidCompare(&a->m_id,&b->m_id);
}

int MYON_UuidIndex::CompareIndex( const MYON_UuidIndex* a, const MYON_UuidIndex* b )
{
  if ( !a )
    return (b ? -1 : 0 );
  if ( !b )
    return 1;
  if (a->m_i < b->m_i)
    return -1;
  if (a->m_i > b->m_i)
    return 1;
  return 0;
}

 
int MYON_UuidPtr::CompareIdAndPtr( const MYON_UuidPtr* a, const MYON_UuidPtr* b )
{
  int i;
  if ( !a )
    return (b ? -1 : 0 );
  if ( !b )
    return 1;

  // compare id first
  if (0 == (i = MYON_UuidCompare(&a->m_id, &b->m_id)))
  {
    if (a->m_ptr < b->m_ptr)
      i = -1;
    else if (a->m_ptr > b->m_ptr)
      i = 1;
  }

  return i;
}

int MYON_UuidPtr::ComparePtrAndId( const MYON_UuidPtr* a, const MYON_UuidPtr* b )
{
  int i;
  if ( !a )
    return (b ? -1 : 0 );
  if ( !b )
    return 1;

  if (a->m_ptr < b->m_ptr)
    i = -1;
  else if (a->m_ptr > b->m_ptr)
    i = 1;
  else
    i = MYON_UuidCompare(&a->m_id,&b->m_id);

  return i;
}

int MYON_UuidPtr::CompareId( const MYON_UuidPtr* a, const MYON_UuidPtr* b )
{
  if ( !a )
    return (b ? -1 : 0 );
  if ( !b )
    return 1;
  return MYON_UuidCompare(&a->m_id,&b->m_id);
}

int MYON_UuidPtr::ComparePtr( const MYON_UuidPtr* a, const MYON_UuidPtr* b )
{
  if ( !a )
    return (b ? -1 : 0 );
  if ( !b )
    return 1;
  if (a->m_ptr < b->m_ptr)
    return -1;
  if (a->m_ptr > b->m_ptr)
    return 1;
  return 0;
}


// Test code for MYON_UuidCompare
////{
////  RPC_STATUS rpc_status = 0;
////  MYON_UUID a,b;
////  size_t sz = sizeof(a);
////  unsigned char* pa = (unsigned char*)&a;
////  unsigned char* pb = (unsigned char*)&b;
////  unsigned char u[10] = {0,1,3,63,64,65,127,128,129,255};
////  int x,y,z;
////  for ( int aa = 0; aa < 10; aa++ ) for ( int bb = 0; bb < 10; bb++ )
////  {
////    for ( size_t i = 0; i < sz; i++ )
////    {
////      memset(pa,0,sz);
////      pa[i] = u[aa];
////      for ( size_t j = 0; j < sz; j++ )
////      {
////        memset(pb,0,sz);
////        pb[j] = u[bb];
////        rpc_status = 0;
////        y = MYON_UuidCompare(&a,&b);
////        z = ::UuidCompare(&a,&b,&rpc_status);
////        if ( y != z )
////        {
////          int mscomparediff = 99; 
////        }
////      }      
////    }
////  }
////}

int MYON_UuidCompare( const MYON_UUID* a, const MYON_UUID* b )
{
  // NOTE WELL: This code has to work the same way
  //            on Windows and non-Windows OSs and on
  //            both big and little endian CPUs
  //            taking into account the way MYON_UUIDs
  //            are read/written by MYON_BinaryArchive.
  //
  //            On Windows, ::UuidCompare() must agree 
  //            with this function.

  if ( !a ) 
  {
    return b ? -1 : 0;
  }
  if ( !b )
    return 1;

  if ( a->Data1 < b->Data1 ) return -1;
  if ( a->Data1 > b->Data1 ) return  1;

  if ( a->Data2 < b->Data2 ) return -1;
  if ( a->Data2 > b->Data2 ) return  1;

  if ( a->Data3 < b->Data3 ) return -1;
  if ( a->Data3 > b->Data3 ) return  1;
  return memcmp(a->Data4,b->Data4,sizeof(a->Data4));
}
 
int MYON_UuidCompare( const MYON_UUID& a, const MYON_UUID& b)
{
  return MYON_UuidCompare(&a,&b);
}

bool MYON_UuidIsNil( 
        const MYON_UUID& uuid 
        )
{
  const MYON__INT32* p = (const MYON__INT32*)&uuid;
  return ( p[0] || p[1] || p[2] || p[3] ) ? false : true;
}


bool MYON_UuidIsNotNil( 
        const MYON_UUID& uuid 
        )
{
  const MYON__INT32* p = (const MYON__INT32*)&uuid;
  return ( p[0] || p[1] || p[2] || p[3] ) ? true : false;
}


char* MYON_UuidToString( const MYON_UUID& uuid, char* s)
{
  // s - [out]  The s[] char array must have length >= 37.  
  //            The returned char array will have a 36 
  //            character uuid in s[0..35] and a null in s[36].

  // NOTE WELL: 
  //   This code has to work on non-Windows OSs and on both big and
  //   little endian CPUs.  The result must satisfy
  //   uuid == MYON_UuidFromString(MYON_UuidToString(uuid,s))

  // 31 August 2005 Dale Lear
  //     Changed upper case to lower case so result is
  //     identical to the string returned by Windows' ::UuidToString().
  //static const char x[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  static const char x[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
  static const int addhyphen[16] = {0,0,0,1, 0,1, 0,1, 0,1,  0, 0, 0, 0, 0, 0};
  const unsigned char* b = (const unsigned char*)&uuid;
  char* p;
  int i;
  
  static const int* rho = (MYON::endian::big_endian == MYON::Endian())
                        ? big_endian_rho 
                        : little_endian_rho;

  // 5 December 2002 Dale Lear:
  //   There is either a bug in Purify (likely) or perhaps a bug in the 
  //   way Microsoft compiles  c>>4 when c is an unsigned char.  In any
  //   case, changing c to an unsigned int makes purify happy and should
  //   work just as well.
  //
  //unsigned char c;

  unsigned int c;

  if ( !s )
    return 0;
  p = s;
  for ( i = 0; i < 16; i++ ) {
    c = b[rho[i]];
    *p++ = x[c>>4];  // purify gripes here if c is an unsigned char - the code runs fine.
    *p++ = x[c&0x0F];
    if ( addhyphen[i] )
      *p++ = '-';
  }
  *p = 0;

#if defined(MYON_DEBUG)
  {
    MYON_UUID u = MYON_UuidFromString(s);
    if ( MYON_UuidCompare(&u,&uuid) ) {
      MYON_ERROR("MYON_UuidToString() bug"); // <- breakpoint here
    }
  }
#endif

  return s;
}

wchar_t* MYON_UuidToString( const MYON_UUID& uuid, wchar_t* s)
{
  // s - [out]  The s[] char array must have length >= 37.  
  //            The returned char array will have a 36 
  //            character uuid in s[0..35] and a null in s[36].

  // NOTE WELL: 
  //   This code has to work on non-Windows OSs and on both big and
  //   little endian CPUs.  The result must satisfy
  //   uuid == MYON_UuidFromString(MYON_UuidToString(uuid,s))
  char x[37];
  if ( s && MYON_UuidToString(uuid,x) )
  {
    int i;
    for (i = 0; i < 37; i++ )
    {
      s[i] = (wchar_t)x[i];
    }
  }
  else
  {
    s = 0;
  }
  return s;
}

 
const char* MYON_UuidToString( const MYON_UUID& uuid, MYON_String& s )
{
  char x[37];
  s = MYON_UuidToString( uuid, x );
  return s.Array();
}

 
const wchar_t* MYON_UuidToString( const MYON_UUID& uuid, MYON_wString& s )
{
  wchar_t x[37];
  s = MYON_UuidToString( uuid, x );
  return s.Array();
}


const MYON_wString MYON_IdToString(
  MYON_UUID id
)
{
  MYON_wString s;
  MYON_UuidToString(id, s);
  return s;
}

const MYON_wString MYON_AddIdPrefixToString(
  const MYON_UUID id,
  const wchar_t* separator,
  const wchar_t* source
)
{
  MYON_wString s = MYON_IdToString(id);
  s += separator;
  s += source;
  return s;
}

MYON_DECL
const MYON_wString MYON_RemoveIdPrefixFromString(
  const MYON_UUID id,
  const wchar_t* separator,
  const wchar_t* source
)
{
  MYON_wString s(source);
  MYON_wString prefix = MYON_IdToString(id);
  prefix += separator;
  return s.RemovePrefix(prefix,MYON_Locale::Ordinal,true);
}

MYON_DECL
const MYON_wString MYON_AddIdSuffixToString(
  const wchar_t* source,
  const wchar_t* separator,
  const MYON_UUID id
)
{
  MYON_wString s(source);
  s += separator;
  s += MYON_IdToString(id);
  return s;

}

MYON_DECL
const MYON_wString MYON_RemoveIdSuffixFromString(
  const wchar_t* source,
  const wchar_t* separator,
  const MYON_UUID id
)
{
  MYON_wString s(source);
  MYON_wString suffix(separator);
  suffix += MYON_IdToString(id);
  return s.RemoveSuffix(suffix,MYON_Locale::Ordinal,true);
}

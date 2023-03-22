#include "opennurbs.h"
#include "opennurbs.h"
#include "opennurbs_testclass.h"
#include "opennurbs_subd_data.h"

#if !defined(MYON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// MYON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, MYON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error MYON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

#if defined(MYON_COMPILER_MSC)
// Force this module to be inited first so the important globals
// are initialized before there is any possibility they are used.
#pragma MYON_PRAGMA_WARNING_PUSH
#pragma MYON_PRAGMA_WARNING_DISABLE_MSC( 4073 )
#pragma init_seg(lib)
#pragma MYON_PRAGMA_WARNING_POP
#endif

const MYON_ErrorEvent MYON_ErrorEvent::Unset;

static unsigned int MYON_LibraryStatusInit()
{
  return 0;
}

unsigned int MYON::m_opennurbs_library_status = MYON_LibraryStatusInit();

unsigned int MYON_MemoryAllocationTracking::m_g_stack_depth = 0;
int MYON_MemoryAllocationTracking::m_g_crt_dbg_flag0 = 0;

MYON_MemoryAllocationTracking::MYON_MemoryAllocationTracking(bool bEnableAllocationTracking)
#if defined(MYON_DEBUG_MEMORY_MSC_WIN)
  : m_this_statck_depth(++MYON_MemoryAllocationTracking::m_g_stack_depth)
  , m_this_crt_dbg_flag0(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#else
  : m_this_statck_depth(0)
  , m_this_crt_dbg_flag0(0)
#endif
{
#if defined(MYON_DEBUG_MEMORY_MSC_WIN)
  // Dale Lear - Dec 2017
  //   Not thread safe but good enough for now and won't crash or assert.
  //   Relax, it's just a debugging tool based on ancient Microsoft debug heap tools.
  if (1 == m_this_statck_depth)
    MYON_MemoryAllocationTracking::m_g_crt_dbg_flag0 = m_this_crt_dbg_flag0;
  if (bEnableAllocationTracking)
  {
    if ( 0 == (m_this_crt_dbg_flag0 & _CRTDBG_ALLOC_MEM_DF))
      _CrtSetDbgFlag(m_this_crt_dbg_flag0 | _CRTDBG_ALLOC_MEM_DF);
  }
  else if ( 0 != m_this_crt_dbg_flag0 )
  {
    _CrtSetDbgFlag(0);
  }
#endif
}

MYON_MemoryAllocationTracking::~MYON_MemoryAllocationTracking()
{
#if defined(MYON_DEBUG_MEMORY_MSC_WIN)
  // Dale Lear - Dec 2017
  //   Not thread safe but good enough for now and won't crash or assert.
  //   Relax, it's just a debugging tool based on ancient Microsoft debug heap tools.
  if (MYON_MemoryAllocationTracking::m_g_stack_depth > 0)
  {
    const bool bOutOfOrder = (m_this_statck_depth != MYON_MemoryAllocationTracking::m_g_stack_depth);

    const int crt_dbg_flag0
      = bOutOfOrder
      ? MYON_MemoryAllocationTracking::m_this_crt_dbg_flag0
      : m_this_crt_dbg_flag0;

    const int crt_dbg_flag1 = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

    if ( crt_dbg_flag1 != crt_dbg_flag0 )
    {
      _CrtSetDbgFlag(crt_dbg_flag0);
    }
    MYON_MemoryAllocationTracking::m_g_stack_depth--;
  }
#endif
}

#if !defined(OPENNURBS_NO_STD_MUTEX)
// It is critical that MYON_TestClass::global_mutex, MYON_TestClass::CtorSerialNumberGenerator, and
// MYON_TestClass::PopulationCounter be initialized before any instance of MYON_TestClass is created. 
std::mutex MYON_TestClass::internal_counter_mutex;
MYON__UINT64 MYON_TestClass::internal_CtorSerialNumberGenerator = 0;
MYON__UINT64 MYON_TestClass::internal_PopulationCounter = 0;
#endif

MYON__UINT64 MYON_NextContentSerialNumber()
{
  static MYON__UINT64 serial_number = 0;
  // If it's ever an issue with multiple threads, 
  // this value can be made atomic, but that will slow down performance.
  return (0 != ++serial_number) ? serial_number : ++serial_number;
}


// It is critical that MYON_ModelComponent::Internal_RuntimeSerialNumberGenerator
// be constructed before any instance of a class derived from MYON_ModelComponent.
// That is why it is above the ClassId stuff in this .cpp file.
// Serial numbers below UINT32_MAX + 1 are reserved for Rhino use.
std::atomic<MYON__UINT64> MYON_ModelComponent::Internal_RuntimeSerialNumberGenerator(UINT32_MAX + 1ULL);

std::atomic<MYON__UINT64> MYON_SubDimple::Internal_RuntimeSerialNumberGenerator;

const double MYON_SubDExpandEdgesParameters::OffsetTolerance = 0.001;

const double MYON_SubDExpandEdgesParameters::MinimumOffset = 0.05;

const double MYON_SubDExpandEdgesParameters::MaximumOffset = 0.95;

const double MYON_SubDExpandEdgesParameters::SmallOffset = 0.125;

const double MYON_SubDExpandEdgesParameters::MediumOffset = 0.25;

const double MYON_SubDExpandEdgesParameters::LargeOffset = 0.5;

const MYON_SubDExpandEdgesParameters MYON_SubDExpandEdgesParameters::Default;

const MYON_SubDComponentLocation MYON_SubD::DefaultSubDAppearance = MYON_SubDComponentLocation::Surface;

// The default type must be packed, unpacked, zero, or nan and should be packed or upacked.
const MYON_SubDTextureCoordinateType MYON_SubD::DefaultTextureCoordinateType = MYON_SubDTextureCoordinateType::Packed;

const MYON_SubDEdgeSharpness MYON_SubDEdgeSharpness::Zero;

const MYON_SubDEdgeSharpness MYON_SubDEdgeSharpness::Nan = MYON_SubDEdgeSharpness::FromConstant(MYON_DBL_QNAN);

// NOTE WELL: 
// It is required that (3 + 2^MYON_SubDEdgeSharpness::Maximum) <= MYON_SubDEdgeSurfaceCurve::MaximumControlPointCapacity
const double MYON_SubDEdgeSharpness::Maximum = 4.0;
const double MYON_SubDEdgeSharpness::Tolerance = 0.01;

const double MYON_SubDSectorType::MinimumCornerAngleRadians = (2.0*MYON_PI)/((double)(MYON_SubDSectorType::MaximumCornerAngleIndex));
const double MYON_SubDSectorType::MaximumCornerAngleRadians = 2.0*MYON_PI - MYON_SubDSectorType::MinimumCornerAngleRadians;

const MYON_SubDSectorId MYON_SubDSectorId::Zero;
const MYON_SubDSectorId MYON_SubDSectorId::Invalid = MYON_SubDSectorId::Create(nullptr, nullptr);

const MYON_SubDHash MYON_SubDHash::Empty;


// {C3D8DD54-F8C8-4455-BB0E-2A2F4988EC81}
const MYON_UUID MYON_SubD::FastAndSimpleFacePackingId =
{ 0xc3d8dd54, 0xf8c8, 0x4455, { 0xbb, 0xe, 0x2a, 0x2f, 0x49, 0x88, 0xec, 0x81 } };

// MYON_SubD::DefaultFacePackingId must always identitify a built-in face packing
// algoritm. If a new built-in algorithm is developed that produces generally 
// better packings and is as fast and reliable as the current default, then
// MYON_SubD::DefaultFacePackingId can be changed. Under no circumstances, should
// the default be changed to anything that is more than 1.5 times slower than 
// the either the fast and simple or the current default on large models 
// like the Mt St Helens Subd. 
//
// ** If it's not really fast, then it cannot be the the default. **
const MYON_UUID MYON_SubD::DefaultFacePackingId = MYON_SubD::FastAndSimpleFacePackingId;

const MYON_SubDToBrepParameters Internal_SubDToBrepParameters(bool bPackedFaces)
{
  MYON_SubDToBrepParameters p;
  p.SetPackFaces(bPackedFaces);
  return p;
}

const MYON_SubDToBrepParameters MYON_SubDToBrepParameters::Default;
const MYON_SubDToBrepParameters MYON_SubDToBrepParameters::DefaultUnpacked = Internal_SubDToBrepParameters(false);
const MYON_SubDToBrepParameters MYON_SubDToBrepParameters::DefaultPacked = Internal_SubDToBrepParameters(true);


const MYON_SubDRTreeVertexFinder MYON_SubDRTreeVertexFinder::Unset;


MYON_ClassId* MYON_ClassId::m_p0 = 0; // static pointer to first id in list
MYON_ClassId* MYON_ClassId::m_p1 = 0; // static pointer to last id in list
int MYON_ClassId::m_mark0 = 0;

// {1311ADCB-D89E-4051-A3F0-F64441FB8EC6}
const MYON_UUID MYON_StandardDisplayModeId::Wireframe =
{ 0x1311ADCB,0xD89E,0x4051,{ 0xA3,0xF0,0xF6,0x44,0x41,0xFB,0x8E,0xC6 } };

// {8BC8DEBE-C83B-4c47-B13C-9DB074510CAC}
const MYON_UUID MYON_StandardDisplayModeId::Shaded =
{ 0x8BC8DEBE,0xC83B,0x4c47,{ 0xB1,0x3C,0x9D,0xB0,0x74,0x51,0x0C,0xAC } };

// {CAE60BAE-2D51-4299-ABF7-A339FCA86F3B}
const MYON_UUID MYON_StandardDisplayModeId::Rendered =
{ 0xCAE60BAE,0x2D51,0x4299,{ 0xAB,0xF7,0xA3,0x39,0xFC,0xA8,0x6F,0x3B } };

// {FF608B97-81D3-4186-831C-41F7DC140881}
const MYON_UUID MYON_StandardDisplayModeId::Ghosted =
{ 0xFF608B97,0x81D3,0x4186,{ 0x83,0x1C,0x41,0xF7,0xDC,0x14,0x08,0x81 } };

// {B5C19D5D-0AEC-4ff7-A10E-E052E660263A}
const MYON_UUID MYON_StandardDisplayModeId::XrayShade =
{ 0xB5C19D5D,0x0AEC,0x4ff7,{ 0xA1,0x0E,0xE0,0x52,0xE6,0x60,0x26,0x3A } };

// {A5545314-9D87-428d-95AE-91052EEAD0FA}
const MYON_UUID MYON_StandardDisplayModeId::RenderedShadows =
{ 0xA5545314,0x9D87,0x428d,{ 0x95,0xAE,0x91,0x05,0x2E,0xEA,0xD0,0xFA } };

// {63612C72-778F-4afd-B81B-17426FDFE8A6}
const MYON_UUID MYON_StandardDisplayModeId::Technical =
{ 0x63612C72,0x778F,0x4afd,{ 0xB8,0x1B,0x17,0x42,0x6F,0xDF,0xE8,0xA6 } };

// {B46AB226-05A0-4568-B454-4B1AB721C675}
const MYON_UUID MYON_StandardDisplayModeId::Artistic =
{ 0xB46AB226,0x05A0,0x4568,{ 0xB4,0x54,0x4B,0x1A,0xB7,0x21,0xC6,0x75 } };

// {F4616FA5-A831-4620-A97E-9B807D5EC376}
const MYON_UUID MYON_StandardDisplayModeId::Pen =
{ 0xF4616FA5,0xA831,0x4620,{ 0xA9,0x7E,0x9B,0x80,0x7D,0x5E,0xC3,0x76 } };

// {C32B72C3-41BD-4ADC-82A8-B7AEF4456A37}
const MYON_UUID MYON_StandardDisplayModeId::AmbientOcclusion = 
{ 0xc32b72c3, 0x41bd, 0x4adc, { 0x82, 0xa8, 0xb7, 0xae, 0xf4, 0x45, 0x6a, 0x37 } };

// {69E0C7A5-1C6A-46C8-B98B-8779686CD181}
const MYON_UUID MYON_StandardDisplayModeId::Raytraced = 
{ 0x69e0c7a5, 0x1c6a, 0x46c8, { 0xb9, 0x8b, 0x87, 0x79, 0x68, 0x6c, 0xd1, 0x81 } };


const MYON_UUID MYON_nil_uuid = { 0,0,0,{ 0,0,0,0,0,0,0,0 } };
const MYON_UUID MYON_max_uuid = { 0xFFFFFFFF,0xFFFF,0xFFFF,{ 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF } };

const MYON_UUID MYON_rhino2_id = { 0x16d0eca6, 0x359, 0x4e4c,{ 0x9f, 0xe, 0xf2, 0x69, 0xfd, 0x47, 0x6c, 0xc4 } };

const MYON_UUID MYON_rhino3_id = { 0xA7BBFF3C, 0xFF19, 0x4883,{ 0x85, 0x8D, 0xB1, 0xE7, 0xDB, 0x4F, 0x1A, 0x7E } };

// {E2143A46-BB01-4b0c-AC4D-C34B5652FAE0}
const MYON_UUID MYON_rhino4_id = { 0xe2143a46, 0xbb01, 0x4b0c,{ 0xac, 0x4d, 0xc3, 0x4b, 0x56, 0x52, 0xfa, 0xe0 } };

// {60515F84-8F7F-41da-801D-1C87E32F88F5}
const MYON_UUID MYON_rhino5_id = { 0x60515f84, 0x8f7f, 0x41da,{ 0x80, 0x1d, 0x1c, 0x87, 0xe3, 0x2f, 0x88, 0xf5 } };

// {06BB1079-5A56-47A1-AD6D-0B45183D894B}
const MYON_UUID MYON_rhino6_id = { 0x6bb1079, 0x5a56, 0x47a1,{ 0xad, 0x6d, 0xb, 0x45, 0x18, 0x3d, 0x89, 0x4b } };

// {78464C2C-9AEB-456E-8C27-865A524F5CA0}
const MYON_UUID MYON_rhino7_id = { 0x78464c2c, 0x9aeb, 0x456e,{ 0x8c, 0x27, 0x86, 0x5a, 0x52, 0x4f, 0x5c, 0xa0 } };

// {868c63f5-3760-4a45-8600-5399cc57b47c}
const MYON_UUID MYON_rhino8_id = { 0x868c63f5, 0x3760, 0x4a45,{ 0x86, 0x00, 0x53, 0x99, 0xcc, 0x57, 0xb4, 0x7c } };

// MYON_rhino_id is always set to the value for the current version
// of Rhino.  MYON_rhino_id is the id that should be used as the
// userdata application id for userdata class definitions that are
// in the core Rhino executable.
const MYON_UUID MYON_rhino_id = MYON_rhino8_id;

// Used to identifiy userdata read from V2 files
// which were written before userdata had application ids.
// {132F2340-DB90-494e-BF02-C36F0EA3197C}
const MYON_UUID MYON_v2_userdata_id = { 0x132f2340, 0xdb90, 0x494e,{ 0xbf, 0x2, 0xc3, 0x6f, 0xe, 0xa3, 0x19, 0x7c } };

// Used to identifiy userdata read from V3 files
// which were written before userdata had application ids.
// {4307B91D-6A9D-478e-B0A2-7C577997C663}
const MYON_UUID MYON_v3_userdata_id = { 0x4307b91d, 0x6a9d, 0x478e,{ 0xb0, 0xa2, 0x7c, 0x57, 0x79, 0x97, 0xc6, 0x63 } };

// Used to identifiy userdata read from V4 files
// which were written before opennurbs 200609190
// required application ids.
// {F73F2953-A244-44c2-B7C2-7E27390D1196}
const MYON_UUID MYON_v4_userdata_id = { 0xf73f2953, 0xa244, 0x44c2,{ 0xb7, 0xc2, 0x7e, 0x27, 0x39, 0xd, 0x11, 0x96 } };

// {17B3ECDA-17BA-4e45-9E67-A2B8D9BE520D}
const MYON_UUID MYON_opennurbs4_id = { 0x17b3ecda, 0x17ba, 0x4e45,{ 0x9e, 0x67, 0xa2, 0xb8, 0xd9, 0xbe, 0x52, 0xd } };

// {C8CDA597-D957-4625-A4B3-A0B510FC30D4}
const MYON_UUID MYON_opennurbs5_id = { 0xc8cda597, 0xd957, 0x4625,{ 0xa4, 0xb3, 0xa0, 0xb5, 0x10, 0xfc, 0x30, 0xd4 } };

// {7B0B585D-7A31-45D0-925E-BDD7DDF3E4E3}
const MYON_UUID MYON_opennurbs6_id = { 0x7b0b585d, 0x7a31, 0x45d0,{ 0x92, 0x5e, 0xbd, 0xd7, 0xdd, 0xf3, 0xe4, 0xe3 } };

// {523bfe6e-ef49-4b75-a8d6-253faf5044d3}
const MYON_UUID MYON_opennurbs7_id = { 0x523bfe6e, 0xef49, 0x4b75,{ 0xa8, 0xd6, 0x25, 0x3f, 0xaf, 0x50, 0x44, 0xd3 } };

// {50EDE5C9-1487-4B4C-B3AA-6840B460E3CF}
const MYON_UUID MYON_opennurbs8_id = { 0x50ede5c9, 0x1487, 0x4b4c, { 0xb3, 0xaa, 0x68, 0x40, 0xb4, 0x60, 0xe3, 0xcf } };


// MYON_opennurbs_id is always set to the value for the current version
// of opennurbs.  MYON_opennurbs_id is the id that should be used as
// the userdata application id for userdata classes definitions that
// are in the opennurbs library.
const MYON_UUID MYON_opennurbs_id = MYON_opennurbs8_id;

const MYON_UuidPairList MYON_UuidPairList::EmptyList;

const MYON_COMPONENT_INDEX MYON_COMPONENT_INDEX::UnsetComponentIndex;
const MYON_COMPONENT_INDEX MYON_COMPONENT_INDEX::WholeObject;
const MYON_ComponentIndexAndNumber MYON_ComponentIndexAndNumber::UnsetAndNan = MYON_ComponentIndexAndNumber::Create(MYON_COMPONENT_INDEX::UnsetComponentIndex, MYON_DBL_QNAN);
const MYON_ComponentIndexAndNumber MYON_ComponentIndexAndNumber::UnsetAndZero = MYON_ComponentIndexAndNumber::Create(MYON_COMPONENT_INDEX::UnsetComponentIndex, 0.0);

// All opennurbs static members are initialized here so that initialization happens in a predictable order.
/*
IEEE 754
Storage
size      sign     exponent         fraction
float    4 bytes   bit 31    8 bits (30-23)  23 bits (22-0)
double   8 bytes   bit 63   11 bits (62-52)  52 bits (51-0)
sign bit = 1 indicates negative
sign bit = 0 indicates positive
float  absolute value = 2^(e-127)  * 1+(f/2^23)
e = value of the 8 bit number in the exponent field
f = value of the 23 bit number in the fraction field
double absolute value = 2^(e-1023) * 1+(f/2^51)
e = value of the 11 bit number in the exponent field
f = value of the 51 bit number in the fraction field
Exceptions:
If all exponent bits are all 0 (e = 0) and the fraction bits
are all zero, then the value of the number is zero.
If all exponent bits are all 0 (e = 0) and at least one fraction
bits is not zero, then the representaion is "denormalized".
In this case, the float absolute value is 0.f*2^-126 and the
double absolute value is 0.f*2^-1022.
If all exponent bits are 1 (float e = 11111111binary = 255decimal
or double e = 11111111111 binary = 2047 decimal) and the fraction
bits are all zero, the number is infinity.  The sign bit
determines the sign of infinity.

If all exponent bits are 1 and at least one fraction bit is
not zero, the number is a "NaN" (not a number).  If the most
significant fraction bit is 1, the number is a quiet NaN or
"QNaN".  If the most significan fraction bit is 0, the number
is a signalling NaN or "SNaN".

Some authors (SH)   QNaNs are used to indicate
indeterminant operations, like sqrt(-1.0).  SNaNs are used
to indicate invalid operations.
SH - http://steve.hollasch.net/cgindex/coding/ieeefloat.html
Intel -
*/
static double MYON__dblinithelper(int i)
{
  // called twice - performance is not important
  union
  {
    double x;
    unsigned char b[8];
  } u;
  unsigned int i7, i6;

  // different bytes on
  u.x = 2.0; // sign = 0; fraction = 0; exponent = 100 0000 0000 binary

  if (0x40 == u.b[7] && 0 == u.b[0]
    && 0 == u.b[1] && 0 == u.b[2] && 0 == u.b[3]
    && 0 == u.b[4] && 0 == u.b[5] && 0 == u.b[6]
    )
  {
    // little endian doubles
    i7 = 7; i6 = 6;
  }
  else if (0x40 == u.b[0] && 0 == u.b[7]
    && 0 == u.b[1] && 0 == u.b[2] && 0 == u.b[3]
    && 0 == u.b[4] && 0 == u.b[5] && 0 == u.b[6]
    )
  {
    // big endian doubles
    i7 = 0; i6 = 1;
  }
  else
  {
    // this sitation is not handled by this algorithm
    // and that is a bug in the algorithm.
    MYON_ERROR("CPU has unexpected bit pattern in double 2.0.");
    i7 = 0;
    i6 = 0;
    i = 99;
  }

  if (1 == i) // positive quiet NaN
  {
    // all exponent bits = 1
    // fraction bits = 100...0
    u.b[i7] = 0x7F; // 0111 1111
    u.b[i6] = 0xF8; // 1111 1000
    u.b[5] = 0;    // 0...
    u.b[4] = 0;
    u.b[3] = 0;
    u.b[2] = 0;
    u.b[7 - i6] = 0;
    u.b[7 - i7] = 0;
  }
  else if (2 == i) // positive infinity
  {
    // all exponent bits = 1
    // all fraction bits = 0
    u.b[i7] = 0x7F; // 0111 1111
    u.b[i6] = 0xF0; // 1111 0000
    u.b[5] = 0;    // 0...
    u.b[4] = 0;
    u.b[3] = 0;
    u.b[2] = 0;
    u.b[7 - i6] = 0;
    u.b[7 - i7] = 0;
  }
  else
  {
    // invalid input 
    u.b[0] = 0xFF;
    u.b[1] = 0xFF;
    u.b[2] = 0xFF;
    u.b[3] = 0xFF;
    u.b[4] = 0xFF;
    u.b[5] = 0xFF;
    u.b[6] = 0xFF;
    u.b[7] = 0xFF;
  }

  return u.x;
}

static float MYON__fltinithelper(int i)
{
  // called twice - performance is not important
  union
  {
    float x;
    unsigned char b[4];
  } u;
  unsigned int i3, i2;

  // different bytes on
  u.x = 2.0f; // sign = 0; mantissa = 0; exponent = 1000 0000
  if (0x40 == u.b[3] && 0 == u.b[0] && 0 == u.b[1] && 0 == u.b[2])
  {
    // little endian doubles
    i3 = 3; i2 = 2;
  }
  else if (0x40 == u.b[0] && 0 == u.b[3] && 0 == u.b[1] && 0 == u.b[2])
  {
    // big endian doubles
    i3 = 0; i2 = 1;
  }
  else
  {
    // this sitation is not handled by this algorithm
    // and that is a bug in the algorithm.
    MYON_ERROR("CPU has unexpected bit pattern in float 2.0f.");
    i3 = 0;
    i2 = 0;
    i = 99;
  }

  if (1 == i) // positive quiet NaN
  {
    // all exponent bits = 1
    // fraction bits = 100...0
    u.b[i3] = 0x7F; // 0111 1111
    u.b[i2] = 0xC0; // 1100 0000
    u.b[3 - i2] = 0;    // 0...
    u.b[3 - i3] = 0;
  }
  else if (2 == i) // positive infinity
  {
    // all exponent bits = 1
    // all fraction bits = 0
    u.b[i3] = 0x7F; // 0111 1111
    u.b[i2] = 0x80; // 1000 0000
    u.b[3 - i2] = 0;    // 0...
    u.b[3 - i3] = 0;
  }
  else
  {
    // invalid input 
    u.b[0] = 0xFF;
    u.b[1] = 0xFF;
    u.b[2] = 0xFF;
    u.b[3] = 0xFF;
  }

  return u.x;
}

const double MYON_DBL_QNAN = MYON__dblinithelper(1);
const double MYON_DBL_PINF = MYON__dblinithelper(2);
const double MYON_DBL_NINF = -MYON__dblinithelper(2);

const float  MYON_FLT_QNAN = MYON__fltinithelper(1);
const float  MYON_FLT_PINF = MYON__fltinithelper(2);
const float  MYON_FLT_NINF = -MYON__fltinithelper(2);

const MYON_2dex MYON_2dex::Unset(MYON_UNSET_INT_INDEX, MYON_UNSET_INT_INDEX);
const MYON_2dex MYON_2dex::Zero(0, 0);
const MYON_3dex MYON_3dex::Unset(MYON_UNSET_INT_INDEX, MYON_UNSET_INT_INDEX, MYON_UNSET_INT_INDEX);
const MYON_3dex MYON_3dex::Zero(0, 0, 0);
const MYON_4dex MYON_4dex::Unset(MYON_UNSET_INT_INDEX, MYON_UNSET_INT_INDEX, MYON_UNSET_INT_INDEX, MYON_UNSET_INT_INDEX);
const MYON_4dex MYON_4dex::Zero(0, 0, 0, 0);

const MYON_2udex MYON_2udex::Unset(MYON_UNSET_UINT_INDEX, MYON_UNSET_UINT_INDEX);
const MYON_2udex MYON_2udex::Zero(0, 0);
const MYON_3udex MYON_3udex::Unset(MYON_UNSET_UINT_INDEX, MYON_UNSET_UINT_INDEX, MYON_UNSET_UINT_INDEX);
const MYON_3udex MYON_3udex::Zero(0, 0, 0);
const MYON_4udex MYON_4udex::Unset(MYON_UNSET_UINT_INDEX, MYON_UNSET_UINT_INDEX, MYON_UNSET_UINT_INDEX, MYON_UNSET_UINT_INDEX);
const MYON_4udex MYON_4udex::Zero(0, 0, 0, 0);

const MYON_Interval MYON_Interval::EmptyInterval(MYON_UNSET_VALUE,MYON_UNSET_VALUE);
const MYON_Interval MYON_Interval::ZeroToOne(0.0,1.0);
const MYON_Interval MYON_Interval::ZeroToTwoPi(0.0,2.0*MYON_PI);
const MYON_Interval MYON_Interval::Nan(MYON_DBL_QNAN,MYON_DBL_QNAN);

const MYON_TextBox MYON_TextBox::Unset;

const MYON_CheckSum MYON_CheckSum::UnsetCheckSum;

const MYONX_ErrorCounter MYONX_ErrorCounter::Zero;

static MYON_MD5_Hash MYON_MD5_Hash_EmptyContentHash()
{
  MYON_MD5 md5;
  return md5.Hash();
}
const MYON_MD5_Hash MYON_MD5_Hash::EmptyContentHash = MYON_MD5_Hash_EmptyContentHash();
const MYON_MD5_Hash MYON_MD5_Hash::ZeroDigest;

static MYON_SHA1_Hash MYON_SHA1_Hash_EmptyContentHash()
{
  MYON_SHA1 sha1;
  return sha1.Hash();
}
const MYON_SHA1_Hash MYON_SHA1_Hash::EmptyContentHash = MYON_SHA1_Hash_EmptyContentHash();
const MYON_SHA1_Hash MYON_SHA1_Hash::ZeroDigest;

const MYONX_ModelTest MYONX_ModelTest::Unset;

// Works with Microsoft's CL, fails for Apple's CLang
////   const struct MYON_UnicodeErrorParameters MYON_UnicodeErrorParameters::MaskErrors   = { 0, 0xFFFFFFFF, MYON_UnicodeCodePoint::MYON_ReplacementCharacter };
////   const struct MYON_UnicodeErrorParameters MYON_UnicodeErrorParameters::FailOnErrors = { 0, 0,          MYON_UnicodeCodePoint::MYON_ReplacementCharacter };
// Works with Microsoft's CL and less capable compilers
static struct MYON_UnicodeErrorParameters MYON_Internal_UnicodeErrorParameters_Default(unsigned int error_mask)
{
  struct MYON_UnicodeErrorParameters uep;
  memset(&uep, 0, sizeof(uep));
  uep.m_error_status = 0;
  uep.m_error_mask = error_mask;
  uep.m_error_code_point = MYON_UnicodeCodePoint::MYON_ReplacementCharacter;
  return uep;
}
const struct MYON_UnicodeErrorParameters MYON_UnicodeErrorParameters::MaskErrors = MYON_Internal_UnicodeErrorParameters_Default(0xFFFFFFFF);
const struct MYON_UnicodeErrorParameters MYON_UnicodeErrorParameters::FailOnErrors = MYON_Internal_UnicodeErrorParameters_Default(0);


const MYON_UnicodeShortCodePoint MYON_UnicodeShortCodePoint::Null = MYON_UnicodeShortCodePoint::Create(0);
const MYON_UnicodeShortCodePoint MYON_UnicodeShortCodePoint::ReplacementCharacter = MYON_UnicodeShortCodePoint::Create(0xFFFD);
const MYON_UnicodeShortCodePoint MYON_UnicodeShortCodePoint::ByteOrderMark = MYON_UnicodeShortCodePoint::Create(0xFFFE);
const MYON_UnicodeShortCodePoint MYON_UnicodeShortCodePoint::Error = MYON_UnicodeShortCodePoint::Create(0xFFFF);

const MYON_Big5CodePoint MYON_Big5CodePoint::Null = MYON_Big5CodePoint::Create(0);
const MYON_Big5CodePoint MYON_Big5CodePoint::WindowsEuro = MYON_Big5CodePoint::Create(0xA3E1);
const MYON_Big5CodePoint MYON_Big5CodePoint::Error = MYON_Big5CodePoint::Create(0xFFFF);

const MYON_Big5UnicodePair MYON_Big5UnicodePair::Null = MYON_Big5UnicodePair::Create(MYON_Big5CodePoint::Null, MYON_UnicodeShortCodePoint::Null);
const MYON_Big5UnicodePair MYON_Big5UnicodePair::Error = MYON_Big5UnicodePair::Create(MYON_Big5CodePoint::Error, MYON_UnicodeShortCodePoint::Error);

const MYON_String MYON_String::EmptyString;
static const MYON_String MYON_Internal_ByteOrderMark()
{
  // UTF-8 encoded byte order mark.
  const unsigned char bom[4] = {0xEFU,0xBBU,0xBFU,0x00U};
  return MYON_String((const char*)bom);
}
const MYON_String MYON_String::ByteOrderMark(MYON_Internal_ByteOrderMark());

const MYON_wString MYON_wString::EmptyString;
const MYON_wString MYON_wString::ByteOrderMark((wchar_t)MYON_UnicodeCodePoint::MYON_ByteOrderMark);

const MYON_NameHash MYON_NameHash::UnsetNameHash;
const MYON_NameHash MYON_NameHash::EmptyNameHash = MYON_NameHash::CreateIdAndEmptyName(MYON_nil_uuid);
const MYON_wString MYON_ModelComponent::ReferencePrefixDelimiter(L" : ");
const MYON_wString MYON_ModelComponent::ReferencePrefixSeparator(L">");
const MYON_wString MYON_ModelComponent::NamePathSeparator(L"::");

const char MYON_FileSystemPath::DirectorySeparatorAsChar =
#if defined(MYON_RUNTIME_WIN)
// Windows
MYON_String::Backslash
#else
// Apple, Android, Unix, ...
MYON_String::Slash
#endif
;
const wchar_t MYON_FileSystemPath::DirectorySeparator = (wchar_t)MYON_FileSystemPath::DirectorySeparatorAsChar;

const char MYON_FileSystemPath::AlternateDirectorySeparatorAsChar =
#if defined(MYON_RUNTIME_WIN)
// Windows
MYON_String::Slash
#else
// Apple, Android, Unix, ...
MYON_String::Backslash
#endif
;
const wchar_t MYON_FileSystemPath::AlternateDirectorySeparator = (wchar_t)MYON_FileSystemPath::AlternateDirectorySeparatorAsChar;



const MYON_ContentHash MYON_ContentHash::Unset;
const MYON_FileReference MYON_FileReference::Unset;

const MYON_3dmRevisionHistory MYON_3dmRevisionHistory::Empty;
const MYON_3dmProperties MYON_3dmProperties::Empty;



static MYON_Xform MYON_Xform_Init(double x, bool bDiagonal)
{
  MYON_Xform xform;
  memset(&xform, 0, sizeof(xform));
  if (bDiagonal)
  {
    xform.m_xform[0][0] = x;
    xform.m_xform[1][1] = x;
    xform.m_xform[2][2] = x;
    xform.m_xform[3][3] = 1.0;
  }
  else
  {
    double* p = &xform.m_xform[0][0];
    double* p1 = p + 16;
    while (p < p1)
      *p++ = x;
  }
  return xform;
}

const MYON_Xform MYON_Xform::IdentityTransformation = MYON_Xform_Init(1.0, true);
const MYON_Xform MYON_Xform::ZeroTransformation = MYON_Xform_Init(0.0, true);

const MYON_Xform MYON_Xform::Zero4x4 = MYON_Xform_Init(0.0, false);
const MYON_Xform MYON_Xform::Unset = MYON_Xform_Init(MYON_UNSET_VALUE, false);
const MYON_Xform MYON_Xform::Nan = MYON_Xform_Init(MYON_DBL_QNAN, false);

const MYON_SurfaceCurvature MYON_SurfaceCurvature::Nan = MYON_SurfaceCurvature::CreateFromPrincipalCurvatures(MYON_DBL_QNAN, MYON_DBL_QNAN);
const MYON_SurfaceCurvature MYON_SurfaceCurvature::Zero = MYON_SurfaceCurvature::CreateFromPrincipalCurvatures(0.0, 0.0);

const MYON_2dPoint MYON_2dPoint::Origin(0.0, 0.0);
const MYON_2dPoint MYON_2dPoint::UnsetPoint(MYON_UNSET_VALUE, MYON_UNSET_VALUE);
const MYON_2dPoint MYON_2dPoint::NanPoint(MYON_DBL_QNAN, MYON_DBL_QNAN);


const MYON_3dPoint MYON_3dPoint::Origin(0.0, 0.0, 0.0);
const MYON_3dPoint MYON_3dPoint::UnsetPoint(MYON_UNSET_VALUE, MYON_UNSET_VALUE, MYON_UNSET_VALUE);
const MYON_3dPoint MYON_3dPoint::NanPoint(MYON_DBL_QNAN, MYON_DBL_QNAN, MYON_DBL_QNAN);

const MYON_4dPoint MYON_4dPoint::Zero(0.0, 0.0, 0.0, 0.0);
const MYON_4dPoint MYON_4dPoint::Nan(MYON_DBL_QNAN, MYON_DBL_QNAN, MYON_DBL_QNAN, MYON_DBL_QNAN);


const MYON_Triangle MYON_Triangle::ZeroTriangle(MYON_3dPoint::Origin, MYON_3dPoint::Origin, MYON_3dPoint::Origin);
const MYON_Triangle MYON_Triangle::UnsetTriangle(MYON_3dPoint::UnsetPoint, MYON_3dPoint::UnsetPoint, MYON_3dPoint::UnsetPoint);
const MYON_Triangle MYON_Triangle::NanTriangle(MYON_3dPoint::NanPoint, MYON_3dPoint::NanPoint, MYON_3dPoint::NanPoint);

const MYON_2dVector MYON_2dVector::ZeroVector(0.0, 0.0);
const MYON_2dVector MYON_2dVector::XAxis(1.0, 0.0);
const MYON_2dVector MYON_2dVector::YAxis(0.0, 1.0);
const MYON_2dVector MYON_2dVector::UnsetVector(MYON_UNSET_VALUE, MYON_UNSET_VALUE);
const MYON_2dVector MYON_2dVector::NanVector(MYON_DBL_QNAN, MYON_DBL_QNAN);

const MYON_3dVector MYON_3dVector::ZeroVector(0.0, 0.0, 0.0);
const MYON_3dVector MYON_3dVector::XAxis(1.0, 0.0, 0.0);
const MYON_3dVector MYON_3dVector::YAxis(0.0, 1.0, 0.0);
const MYON_3dVector MYON_3dVector::ZAxis(0.0, 0.0, 1.0);
const MYON_3dVector MYON_3dVector::UnsetVector(MYON_UNSET_VALUE, MYON_UNSET_VALUE, MYON_UNSET_VALUE);
const MYON_3dVector MYON_3dVector::NanVector(MYON_DBL_QNAN, MYON_DBL_QNAN, MYON_DBL_QNAN);

const MYON_2fPoint MYON_2fPoint::Origin(0.0f, 0.0f);
const MYON_2fPoint MYON_2fPoint::NanPoint(MYON_FLT_QNAN, MYON_FLT_QNAN);

const MYON_3fPoint MYON_3fPoint::Origin(0.0f, 0.0f, 0.0f);
const MYON_3fPoint MYON_3fPoint::NanPoint(MYON_FLT_QNAN, MYON_FLT_QNAN, MYON_FLT_QNAN);

const MYON_4fPoint MYON_4fPoint::Zero(0.0f, 0.0f, 0.0f, 0.0f);
const MYON_4fPoint MYON_4fPoint::Nan(MYON_FLT_QNAN, MYON_FLT_QNAN, MYON_FLT_QNAN, MYON_FLT_QNAN);

const MYON_2fVector MYON_2fVector::ZeroVector(0.0f, 0.0f);
const MYON_2fVector MYON_2fVector::XAxis(1.0f, 0.0f);
const MYON_2fVector MYON_2fVector::YAxis(0.0f, 1.0f);

const MYON_3fVector MYON_3fVector::ZeroVector(0.0f, 0.0f, 0.0f);
const MYON_3fVector MYON_3fVector::XAxis(1.0f, 0.0f, 0.0f);
const MYON_3fVector MYON_3fVector::YAxis(0.0f, 1.0f, 0.0f);
const MYON_3fVector MYON_3fVector::ZAxis(0.0f, 0.0f, 1.0f);

const MYON_WindingNumber MYON_WindingNumber::Unset;

// Do not increase this tolerance to fix a specific bug.
// This tolerance is used after input has been cleaned up
// to detect flaws.
const double MYON_Symmetry::ZeroTolerance = 1.0e-8; 

const MYON_Symmetry MYON_Symmetry::Unset;

// {3C6C7ABD-F3D5-41C2-96C9-DB9AEAF06E90}
const MYON_UUID MYON_Symmetry::ReflectId =
{ 0x3c6c7abd, 0xf3d5, 0x41c2, { 0x96, 0xc9, 0xdb, 0x9a, 0xea, 0xf0, 0x6e, 0x90 } };

// {C1592254-DEAC-4E8E-B01E-0522450E03F7}
const MYON_UUID MYON_Symmetry::RotateId =
{ 0xc1592254, 0xdeac, 0x4e8e, { 0xb0, 0x1e, 0x5, 0x22, 0x45, 0xe, 0x3, 0xf7 } };

// {9133927D-5A4E-4DDD-9924-EF3A6360C19A}
const MYON_UUID MYON_Symmetry::ReflectAndRotateId =
{ 0x9133927d, 0x5a4e, 0x4ddd, { 0x99, 0x24, 0xef, 0x3a, 0x63, 0x60, 0xc1, 0x9a } };

static MYON_BoundingBox BoundingBoxInit(double x)
{
  MYON_BoundingBox bbox;
  bbox.m_min.x = x;
  bbox.m_min.y = x;
  bbox.m_min.z = x;
  bbox.m_max.x = x;
  bbox.m_max.y = x;
  bbox.m_max.z = x;
  return bbox;
}
const MYON_BoundingBox MYON_BoundingBox::EmptyBoundingBox;
const MYON_BoundingBox MYON_BoundingBox::UnsetBoundingBox = BoundingBoxInit(MYON_UNSET_VALUE);
const MYON_BoundingBox MYON_BoundingBox::NanBoundingBox = BoundingBoxInit(MYON_DBL_QNAN);

const MYON_UnitSystem MYON_UnitSystem::None(MYON::LengthUnitSystem::None);
const MYON_UnitSystem MYON_UnitSystem::Angstroms(MYON::LengthUnitSystem::Angstroms);
const MYON_UnitSystem MYON_UnitSystem::Nanometers(MYON::LengthUnitSystem::Nanometers);
const MYON_UnitSystem MYON_UnitSystem::Microns(MYON::LengthUnitSystem::Microns);
const MYON_UnitSystem MYON_UnitSystem::Millimeters(MYON::LengthUnitSystem::Millimeters);
const MYON_UnitSystem MYON_UnitSystem::Centimeters(MYON::LengthUnitSystem::Centimeters);
const MYON_UnitSystem MYON_UnitSystem::Decimeters(MYON::LengthUnitSystem::Decimeters);
const MYON_UnitSystem MYON_UnitSystem::Meters(MYON::LengthUnitSystem::Meters);
const MYON_UnitSystem MYON_UnitSystem::Dekameters(MYON::LengthUnitSystem::Dekameters);
const MYON_UnitSystem MYON_UnitSystem::Hectometers(MYON::LengthUnitSystem::Hectometers);
const MYON_UnitSystem MYON_UnitSystem::Kilometers(MYON::LengthUnitSystem::Kilometers);
const MYON_UnitSystem MYON_UnitSystem::Megameters(MYON::LengthUnitSystem::Megameters);
const MYON_UnitSystem MYON_UnitSystem::Gigameters(MYON::LengthUnitSystem::Gigameters);
const MYON_UnitSystem MYON_UnitSystem::Microinches(MYON::LengthUnitSystem::Microinches);
const MYON_UnitSystem MYON_UnitSystem::Mils(MYON::LengthUnitSystem::Mils);
const MYON_UnitSystem MYON_UnitSystem::Inches(MYON::LengthUnitSystem::Inches);
const MYON_UnitSystem MYON_UnitSystem::Feet(MYON::LengthUnitSystem::Feet);
const MYON_UnitSystem MYON_UnitSystem::Yards(MYON::LengthUnitSystem::Yards);
const MYON_UnitSystem MYON_UnitSystem::Miles(MYON::LengthUnitSystem::Miles);
const MYON_UnitSystem MYON_UnitSystem::PrinterPoints(MYON::LengthUnitSystem::PrinterPoints);
const MYON_UnitSystem MYON_UnitSystem::PrinterPicas(MYON::LengthUnitSystem::PrinterPicas);
const MYON_UnitSystem MYON_UnitSystem::NauticalMiles(MYON::LengthUnitSystem::NauticalMiles);
const MYON_UnitSystem MYON_UnitSystem::AstronomicalUnits(MYON::LengthUnitSystem::AstronomicalUnits);
const MYON_UnitSystem MYON_UnitSystem::LightYears(MYON::LengthUnitSystem::LightYears);
const MYON_UnitSystem MYON_UnitSystem::Parsecs(MYON::LengthUnitSystem::Parsecs);
const MYON_UnitSystem MYON_UnitSystem::Unset(MYON::LengthUnitSystem::Unset);

const MYON_LengthUnitName MYON_LengthUnitName::None;
const MYON_AngleUnitName MYON_AngleUnitName::None;

const MYON_LengthValue MYON_LengthValue::Unset;
const MYON_LengthValue MYON_LengthValue::Zero = MYON_LengthValue::Create(0.0, MYON::LengthUnitSystem::None, 0, MYON_LengthValue::StringFormat::CleanDecimal);

const MYON_AngleValue MYON_AngleValue::Unset;
const MYON_AngleValue MYON_AngleValue::Zero = MYON_AngleValue::Create(0.0, MYON::AngleUnitSystem::None, 0, MYON_AngleValue::StringFormat::CleanDecimal );

const MYON_ScaleValue MYON_ScaleValue::Unset;
const MYON_ScaleValue MYON_ScaleValue::OneToOne = MYON_ScaleValue::Create(
  MYON_LengthValue::Create(1.0, MYON::LengthUnitSystem::None, 0, MYON_LengthValue::StringFormat::CleanDecimal),
  MYON_LengthValue::Create(1.0, MYON::LengthUnitSystem::None, 0, MYON_LengthValue::StringFormat::CleanDecimal),
  MYON_ScaleValue::ScaleStringFormat::RatioFormat
);

// MYON_PTR_SEMAPHORE1 triggers a special case in the constructor.
// See MYON_TextLog::AppendText() and Internal_TextLogIsNull() for implementation
// details.
MYON_TextLog MYON_TextLog::Null( (FILE*)MYON_PTR_SEMAPHORE1 );

// Discuss any changes of these values with Dale Lear
//
// For 32 bit float based OpenGL drivers, the value of
// the MYON_Viewport::DefaultMinNearOverFar constant must 
// be <0.01 and >= 0.0001.  
// If you change this value, you need to retest RR 8902 on OpenGL
// drivers that (internally) use float precision transformations.
// Some OpenGL drivers, like the Microsoft software emulation
// driver for XP crash in some cases when near/far > 1e8.
//
// MYON_Viewport::DefaultMinNearOverFar = 0.001    // used in Rhino 3.0 beta testing until 11 Sep 2002
// MYON_Viewport::DefaultMinNearOverFar = 0.01     // used for Rhino 3.0 CD1 and CD2
// MYON_Viewport::DefaultMinNearOverFar = 0.000001 // used for Rhino 3.0 CD3
// MYON_Viewport::DefaultMinNearOverFar = 0.0001   // used for Rhino 4.0 Fixes RR 8902
//
// Discuss any changes of these values with Dale Lear
const double MYON_Viewport::DefaultNearDist = 0.005;
const double MYON_Viewport::DefaultFarDist = 1000.0;
const double MYON_Viewport::DefaultMinNearDist = 0.0001;
const double MYON_Viewport::DefaultMinNearOverFar = 0.0001;

const MYON_3dVector MYON_Viewport::Default3dCameraDirection(-0.43301270189221932338186158537647,0.75,-0.5);
const MYON_3dPoint MYON_Viewport::DefaultCameraLocation(0.0, 0.0, 100.0);
const MYON_Viewport MYON_Viewport::DefaultTopViewYUp;
static MYON_Viewport Internal_MYON_Viewport_DefaultPerspectiveView_Init()
{
  MYON_Viewport vp;
  vp.SetCameraDirection(MYON_Viewport::Default3dCameraDirection);
  vp.SetCameraUp(MYON_3dVector::ZAxis);
  vp.SetCameraLocation(MYON_Viewport::DefaultCameraLocation.DistanceTo(MYON_3dPoint::Origin)*vp.CameraZ());

  return vp;
}
const MYON_Viewport MYON_Viewport::DefaultPerspectiveViewZUp = Internal_MYON_Viewport_DefaultPerspectiveView_Init();


const MYON_3dmIOSettings MYON_3dmIOSettings::Default;
const MYON_3dmConstructionPlaneGridDefaults MYON_3dmConstructionPlaneGridDefaults::Default;

MYON_EarthAnchorPoint Internal_EarthAnchorPointConstant(int which)
{
  MYON_EarthAnchorPoint eap;
  switch (which)
  {
  case 1: // Seattle Space Needle
    {
      const double latitude_degrees = 47.620397; // Cal Topo Maps (downloaded June 24, 2016)
      const double longitude_degrees = -122.349179; // Cal Topo Maps (downloaded June 24, 2016)
      const double ground_elevation_feet = 207.0; // feet c. Geonames - Cal Topo Maps (downloaded June 24, 2016)
      const double observation_deck_height_feet = 520.0; // feet above the ground  http://www.spaceneedle.com/fun-facts/
      eap.SetEarthLocation(
        MYON::EarthCoordinateSystem::MeanSeaLevel, 
        MYON::LengthUnitSystem::Feet,
        latitude_degrees,
        longitude_degrees,
        ground_elevation_feet + observation_deck_height_feet
      );
    }
    break;
  }
  return eap;
}
const MYON_EarthAnchorPoint MYON_EarthAnchorPoint::Unset = Internal_EarthAnchorPointConstant(0);
const MYON_EarthAnchorPoint MYON_EarthAnchorPoint::SeattleSpaceNeedle = Internal_EarthAnchorPointConstant(1);

const MYON_3dmAnnotationSettings MYON_3dmAnnotationSettings::Default;

const MYON_3dmAnnotationContext MYON_3dmAnnotationContext::Default;

const MYON_3dmArchiveTableStatus MYON_3dmArchiveTableStatus::Unset;

const wchar_t* MYON_TextDot::DefaultFontFace = L"Arial";
const int MYON_TextDot::DefaultHeightInPoints = 14;
const int MYON_TextDot::MinimumHeightInPoints = 3;
const MYON_TextDot MYON_TextDot::Unset;

const MYON_Locale MYON_Locale::Ordinal = MYON_Locale::FromWindowsLCID(MYON_Locale::OrdinalLCID);
const MYON_Locale MYON_Locale::InvariantCulture = MYON_Locale::FromWindowsLCID(MYON_Locale::InvariantCultureLCID);

// DLL_Main() andc other appropriate places in Rhino application
// initialization call MYON_Locale::SetCurrentCulture() to initialize
// MYON_Locale::m_CurrentCulture.
MYON_Locale MYON_Locale::m_CurrentCulture = MYON_Locale::Ordinal;
const MYON_Locale& MYON_Locale::CurrentCulture = MYON_Locale::m_CurrentCulture;

static MYON_ClippingRegionPoints MYON_ClippingRegionPoints_EmptyInit()
{
  MYON_ClippingRegionPoints empty_clip_points;
  memset(&empty_clip_points, 0, sizeof(empty_clip_points));
  return empty_clip_points;
}

const MYON_ClippingRegionPoints MYON_ClippingRegionPoints::Empty = MYON_ClippingRegionPoints_EmptyInit();

static MYON_PickPoint MYON_PickPoint_UnsetInit()
{
  MYON_PickPoint unset_pick_point;
  double* p = unset_pick_point.m_t;
  double* p1 = p + sizeof(unset_pick_point.m_t) / sizeof(unset_pick_point.m_t[0]);
  while (p < p1)
    *p++ = MYON_UNSET_VALUE;
  return unset_pick_point;
}

const MYON_PickPoint MYON_PickPoint::Unset = MYON_PickPoint_UnsetInit();

const MYON_Color MYON_Color::UnsetColor(MYON_UNSET_COLOR);
const MYON_Color MYON_Color::Black(0, 0, 0);
const MYON_Color MYON_Color::White(255, 255, 255);
const MYON_Color MYON_Color::SaturatedRed(255, 0, 0);
const MYON_Color MYON_Color::SaturatedGreen(0, 255, 0);
const MYON_Color MYON_Color::SaturatedBlue(0, 0, 255);
const MYON_Color MYON_Color::SaturatedYellow(255, 255, 0);
const MYON_Color MYON_Color::SaturatedCyan(0, 255, 255);
const MYON_Color MYON_Color::SaturatedMagenta(255, 0, 255);
const MYON_Color MYON_Color::SaturatedGold(255, 191, 0);
const MYON_Color MYON_Color::Gray105(105, 105, 105);
const MYON_Color MYON_Color::Gray126(126, 126, 126);
const MYON_Color MYON_Color::Gray160(160, 160, 160);
const MYON_Color MYON_Color::Gray230(230, 230, 230);
const MYON_Color MYON_Color::Gray250(250, 250, 250);

const MYON_4fColor MYON_4fColor::Unset;

const MYON_UuidIndex MYON_UuidIndex::NilIndex = MYON_UuidIndex();
const MYON_UuidPtr MYON_UuidPtr::NilPtr = MYON_UuidPtr();

const MYON_Line MYON_Line::ZeroLine(MYON_3dPoint::Origin, MYON_3dPoint::Origin);
const MYON_Line MYON_Line::UnsetLine(MYON_3dPoint::UnsetPoint, MYON_3dPoint::UnsetPoint);
const MYON_Line MYON_Line::NanLine(MYON_3dPoint::NanPoint, MYON_3dPoint::NanPoint);

const MYON_PlaneEquation MYON_PlaneEquation::UnsetPlaneEquation(MYON_UNSET_VALUE, MYON_UNSET_VALUE, MYON_UNSET_VALUE, MYON_UNSET_VALUE);
const MYON_PlaneEquation MYON_PlaneEquation::ZeroPlaneEquation(0.0, 0.0, 0.0, 0.0);
const MYON_PlaneEquation MYON_PlaneEquation::NanPlaneEquation(MYON_DBL_QNAN, MYON_DBL_QNAN, MYON_DBL_QNAN, MYON_DBL_QNAN);
const MYON_PlaneEquation MYON_PlaneEquation::WorldXY(0.0, 0.0, 1.0, 0.0);
const MYON_PlaneEquation MYON_PlaneEquation::WorldYZ(1.0, 0.0, 0.0, 0.0);
const MYON_PlaneEquation MYON_PlaneEquation::WorldZX(0.0, 1.0, 0.0, 0.0);


const MYON_Plane MYON_Plane::World_xy(MYON_3dPoint::Origin, MYON_3dVector::XAxis, MYON_3dVector::YAxis);
const MYON_Plane MYON_Plane::World_yz(MYON_3dPoint::Origin, MYON_3dVector::YAxis, MYON_3dVector::ZAxis);
const MYON_Plane MYON_Plane::World_zx(MYON_3dPoint::Origin, MYON_3dVector::ZAxis, MYON_3dVector::XAxis);

// obsolete names for world planes
const MYON_Plane MYON_xy_plane = MYON_Plane::World_xy;
const MYON_Plane MYON_yz_plane = MYON_Plane::World_yz;
const MYON_Plane MYON_zx_plane = MYON_Plane::World_zx;

static MYON_Plane MYON_Plane_UnsetPlane()
{
  MYON_Plane unset_plane;
  unset_plane.xaxis = MYON_3dVector::UnsetVector;
  unset_plane.yaxis = MYON_3dVector::UnsetVector;
  unset_plane.zaxis = MYON_3dVector::UnsetVector;
  unset_plane.origin = MYON_3dPoint::UnsetPoint;
  unset_plane.plane_equation = MYON_PlaneEquation::UnsetPlaneEquation;
  return unset_plane;
}

const MYON_Plane MYON_Plane::UnsetPlane(MYON_Plane_UnsetPlane());

static MYON_Plane MYON_Plane_NanPlane()
{
  MYON_Plane nan_plane;
  nan_plane.xaxis = MYON_3dVector::NanVector;
  nan_plane.yaxis = MYON_3dVector::NanVector;
  nan_plane.zaxis = MYON_3dVector::NanVector;
  nan_plane.origin = MYON_3dPoint::NanPoint;
  nan_plane.plane_equation = MYON_PlaneEquation::NanPlaneEquation;
  return nan_plane;
}
const MYON_Plane MYON_Plane::NanPlane(MYON_Plane_NanPlane());

// MYON_SubDDisplayParameters statics before MYON_MeshParamters statics
const MYON_SubDDisplayParameters MYON_SubDDisplayParameters::Empty;
const MYON_SubDDisplayParameters MYON_SubDDisplayParameters::ExtraCoarse = MYON_SubDDisplayParameters::CreateFromDisplayDensity(MYON_SubDDisplayParameters::ExtraCoarseDensity);
const MYON_SubDDisplayParameters MYON_SubDDisplayParameters::Coarse = MYON_SubDDisplayParameters::CreateFromDisplayDensity(MYON_SubDDisplayParameters::CoarseDensity);
const MYON_SubDDisplayParameters MYON_SubDDisplayParameters::Medium = MYON_SubDDisplayParameters::CreateFromDisplayDensity(MYON_SubDDisplayParameters::MediumDensity);
const MYON_SubDDisplayParameters MYON_SubDDisplayParameters::Fine = MYON_SubDDisplayParameters::CreateFromDisplayDensity(MYON_SubDDisplayParameters::FineDensity);
const MYON_SubDDisplayParameters MYON_SubDDisplayParameters::ExtraFine = MYON_SubDDisplayParameters::CreateFromDisplayDensity(MYON_SubDDisplayParameters::ExtraFineDensity);
const MYON_SubDDisplayParameters MYON_SubDDisplayParameters::Default = MYON_SubDDisplayParameters::CreateFromDisplayDensity(MYON_SubDDisplayParameters::DefaultDensity);

// {F15F67AA-4AF9-4B25-A3B8-517CEDDAB134}
const MYON_UUID MYON_MeshParameters::RhinoLegacyMesherId = { 0xf15f67aa, 0x4af9, 0x4b25,{ 0xa3, 0xb8, 0x51, 0x7c, 0xed, 0xda, 0xb1, 0x34 } };

// {EB6F6F3F-F975-4546-9D1C-64E9423BEB7F}
const MYON_UUID MYON_MeshParameters::PangolinMesherId = { 0xeb6f6f3f, 0xf975, 0x4546,{ 0x9d, 0x1c, 0x64, 0xe9, 0x42, 0x3b, 0xeb, 0x7f } };

static MYON_MeshParameters Internal_MYON_MeshParameters_Constants(
  int selector
  )
{
  MYON_MeshParameters mp;

  // If you change any parameter values, put your name, the date, and the reasons for
  // the change in a comment by the changed value. Include the previous value in
  // your comment. This is crtically important so we can keep track of what we are
  // trying to accomplish.
  //
  // You must also update the mesh parameters file reading code so that settings with old defaults
  // are replaced with setting that have the new defaults AND old defaults get saved in earlier version
  // 3dm files. This requires somebody with a solid understanding of how MYON_MeshParameters::Read()/Write()
  // works, how saving earlier versions of 3dm fiels works, and how reading old version files works.

  switch (selector)
  {
  case 0: // MYON_MeshParameters::DefaultMesh
    break;

  case 1: 
    // MYON_MeshParameters::FastRenderMesh
    // Added 27 April 2006 for the Meshkateers
    //   Attempting to make jagged and faster render meshes a little
    //   more dense.
    //
    // Turn off everything ...
    mp.SetComputeCurvature(false);
    mp.SetTolerance(0.0);
    mp.SetJaggedSeams(false);
    mp.SetMaximumEdgeLength(0.0);
    mp.SetGridAspectRatio(0.0);
    mp.SetGridMaxCount(0);
    mp.SetGridAngleRadians(0.0);
    mp.SetGridAmplification(0.0);
    mp.SetRefineAngleRadians(0.0);

    // ... except ...
    // The m_relative_tolerance setting must be set so that
    // 0.0005 = MYON_MeshParameters::Tolerance(m_relative_tolerance,1.0).
    mp.SetRelativeTolerance(0.65);
    //double x = Tolerance(m_relative_tolerance,1.0);

    mp.SetGridMinCount(16);
    mp.SetMinimumEdgeLength(0.0001);
    mp.SetRefine(true);
    mp.SetSimplePlanes(true);

    mp.SetTextureRange(2); // Don't change this without speaking to Dale Lear

                            //{
                            //  // 16 July, 2002 - copied V2 hard coded "jagged and faster" render mesh settings
                            //  //
                            //  // Settings used in V2, V3 and early V4 beta
                            //  mp.m_refine_angle       = 20.0*MYON_PI/180.0;
                            //  mp.m_grid_angle         = 20.0*MYON_PI/180.0;
                            //  mp.m_grid_aspect_ratio  = 0.0;
                            //  mp.m_min_edge_length    = 0.0001;
                            //  mp.m_max_edge_length    = 0.0;
                            //  mp.m_tolerance          = 0.0;
                            //  mp.m_grid_min_count     = 16;
                            //  mp.m_bRefine            = 1;
                            //  mp.m_bJaggedSeams       = 0;
                            //  mp.m_bSimplePlanes      = 0;
                            //}
    break;

  case 2: // MYON_MeshParameters::QualityRenderMesh
          // Added 27 April 2006 for the Meshkateers
          //   Attempting to make smooth and slower render meshing a little
          //   faster.
          //
          // Turn off everything ...
    mp.SetComputeCurvature(false);
    mp.SetTolerance(0.0);
    mp.SetJaggedSeams(false);
    mp.SetMaximumEdgeLength(0.0);
    mp.SetGridAspectRatio(0.0);
    mp.SetGridMaxCount(0);
    mp.SetGridAngleRadians(0.0);
    mp.SetGridAmplification(0.0);

    // ... except ...
    // The m_relative_tolerance setting must be set so that
    // 0.00025 = MYON_MeshParameters::Tolerance(m_relative_tolerance,1.0).
    mp.SetRelativeTolerance(0.8);
    //double x = Tolerance(m_relative_tolerance,1.0);

    mp.SetGridMinCount(16);
    mp.SetMinimumEdgeLength(0.0001);
    mp.SetRefine(true);
    mp.SetSimplePlanes(true);
    mp.SetRefineAngleDegrees(20.0);

    mp.SetTextureRange(2); // Don't change this without speaking to Dale Lear


                            //// 16 July, 2002 - copied V2 hard coded "smooth and slower" render mesh settings
                            ////
                            //// Settings used in V2, V3 and early V4 beta
                            //mp.Setrefine_angle       = 15.0*MYON_PI/180.0;
                            //mp.Setgrid_angle         = 15.0*MYON_PI/180.0;
                            //mp.Setgrid_aspect_ratio  = 6.0;
                            //mp.Setmin_edge_length    = 0.0001;
                            //mp.Setmax_edge_length    = 0.0;
                            //mp.Settolerance          = 0.0;
                            //mp.Setgrid_min_count     = 16;
                            //mp.SetbRefine            = 1;
                            //mp.SetbJaggedSeams       = 0;
                            //mp.SetbSimplePlanes      = 0;
    break;

  case 3: // MYON_MeshParameters::DefaultAnalysisMesh
    // 7 July 2006 Dale Lear Rhino 5
    //    I switched from the default constructor to the density=0.5 constructor to fix RR 10330.
    //mp = MYON_MeshParameters(0.5, MYON_MeshParameters::DefaultMesh.MinimumEdgeLength());
    //mp.SetTextureRange(1); // m_texture_range must be 1.  Do not change this setting.

    // Rhino 6, 7 defaults;
    mp = MYON_MeshParameters(0.8, MYON_MeshParameters::DefaultMesh.MinimumEdgeLength());
    mp.SetTextureRange(1); // m_texture_range must be 1.  Do not change this setting.
    mp.SetGridAspectRatio(0.0);
    mp.SetGridAngleRadians(20.0*MYON_DEGREES_TO_RADIANS);
    mp.SetGridAmplification(1.0);
    mp.SetRefineAngleRadians(20.0 * MYON_DEGREES_TO_RADIANS);
    mp.SetComputeCurvature(true);
    mp.SetGridMinCount(16);
    mp.SetSubDDisplayParameters(MYON_SubDDisplayParameters::Default);
    mp.SetRefine(true);

    break;
  }

  if (0 != selector)
  {
    if (1 == selector )
    {
      // MYON_MeshParameters::DefaultMesh has been initialized.
      // MYON_MeshParameters::DefaultMesh must be initialized before ContentHash() can be used.
      MYON_MeshParameters::DefaultMesh.ContentHash();
    }
    // Compute cached sha1 content hash.
    mp.ContentHash();
  }

  return mp;
}

const MYON_MeshParameters MYON_MeshParameters::DefaultMesh = Internal_MYON_MeshParameters_Constants(0);
const MYON_MeshParameters MYON_MeshParameters::FastRenderMesh = Internal_MYON_MeshParameters_Constants(1);
const MYON_MeshParameters MYON_MeshParameters::QualityRenderMesh = Internal_MYON_MeshParameters_Constants(2);
const MYON_MeshParameters MYON_MeshParameters::DefaultAnalysisMesh = Internal_MYON_MeshParameters_Constants(3);

bool MYON_MeshParameters_AreValid()
{
  // This is a validation test to insure the code that sets default mesh parameters
  // and the code that detects default mesh parameters works correctly.
  // This validation test passes as of November 2020. If MYON_ERROR() is called in this function
  // it means new bugs have been introduced. These need to be fixed immediately in order to
  // keep the code that generates display meshes working properly.
  if (MYON_MeshParameters::Type::Default != MYON_MeshParameters::DefaultMesh.GeometrySettingsType())
  {
    MYON_ERROR("MYON_MeshParameters::DefaultMesh.GeometrySettingsType() returned an unexpected value.");
    return false;
  }
  if (MYON_MeshParameters::Type::FastRender != MYON_MeshParameters::FastRenderMesh.GeometrySettingsType())
  {
    MYON_ERROR("MYON_MeshParameters::FastRenderMesh.GeometrySettingsType() returned an unexpected value.");
    return false;
  }
  if (MYON_MeshParameters::Type::QualityRender != MYON_MeshParameters::QualityRenderMesh.GeometrySettingsType())
  {
    MYON_ERROR("MYON_MeshParameters::QualityRenderMesh.GeometrySettingsType() returned an unexpected value.");
    return false;
  }
  if (MYON_MeshParameters::Type::DefaultAnalysis != MYON_MeshParameters::DefaultAnalysisMesh.GeometrySettingsType())
  {
    MYON_ERROR("MYON_MeshParameters::DefaultAnalysisMesh.GeometrySettingsType() returned an unexpected value.");
    return false;
  }
  for (double normalized_mesh_density = 0.0; normalized_mesh_density <= 1.0; normalized_mesh_density += 0.125)
  {
    const MYON_MeshParameters mp = MYON_MeshParameters::CreateFromMeshDensity(normalized_mesh_density);
    if (MYON_MeshParameters::Type::FromMeshDensity != mp.GeometrySettingsType())
    {
      MYON_ERROR("MYON_MeshParameters::MYON_MeshParameters::CreateFromMeshDensity(...).GeometrySettingsType() returned an unexpected value.");
      return false;
    }
    if (normalized_mesh_density != mp.MeshDensity())
    {
      MYON_ERROR("MYON_MeshParameters::MYON_MeshParameters::CreateFromMeshDensity(...).MeshDensity() returned an unexpected value.");
      return false;
    }
  }
  return true;
}

const static bool MYON_MeshParameters_AreValid_ = MYON_MeshParameters_AreValid();

const MYON_3dmUnitsAndTolerances MYON_3dmUnitsAndTolerances::Millimeters;

const MYON_Circle MYON_Circle::UnitCircle;
const MYON_Arc MYON_Arc::UnitCircle;

const MYON_3dmRenderSettings MYON_3dmRenderSettings::Default;

const MYON_3dmSettings MYON_3dmSettings::Default;

const MYON_ProgressStepCounter MYON_ProgressStepCounter::Empty;

const MYON_HistoryRecord MYON_HistoryRecord::Empty;

const MYON_RTreeMemPool MYON_RTreeMemPool::Empty;
const MYON_RTree MYON_RTree::Empty;

// {F5E3BAA9-A7A2-49FD-B8A1-66EB274A5F91}
const MYON_UUID MYON_MeshCache::RenderMeshId =
{ 0xf5e3baa9, 0xa7a2, 0x49fd,{ 0xb8, 0xa1, 0x66, 0xeb, 0x27, 0x4a, 0x5f, 0x91 } };

// {AC12F955-A29F-437B-A4C9-06C85B7FF57C}
const MYON_UUID MYON_MeshCache::AnalysisMeshId =
{ 0xac12f955, 0xa29f, 0x437b,{ 0xa4, 0xc9, 0x6, 0xc8, 0x5b, 0x7f, 0xf5, 0x7c } };

// {3AACA7F2-0444-4587-8083-A13C5BA0AE50}
const MYON_UUID MYON_MeshCache::PreviewMeshId =
{ 0x3aaca7f2, 0x444, 0x4587,{ 0x80, 0x83, 0xa1, 0x3c, 0x5b, 0xa0, 0xae, 0x50 } };

// {85D9E08D-386C-45C3-83DC-0C354685D504}
const MYON_UUID MYON_MeshCache::CoarseMeshId =
{ 0x85d9e08d, 0x386c, 0x45c3,{ 0x83, 0xdc, 0xc, 0x35, 0x46, 0x85, 0xd5, 0x4 } };

// {A2DAF594-F44B-44A6-A44B-E856CD7C05E8}
const MYON_UUID MYON_MeshCache::FineMeshId =
{ 0xa2daf594, 0xf44b, 0x44a6,{ 0xa4, 0x4b, 0xe8, 0x56, 0xcd, 0x7c, 0x5, 0xe8 } };

// {A82C0F1B-13E8-4DEC-AD35-D3DDD277203C}
const MYON_UUID MYON_MeshCache::AnyMeshId =
{ 0xa82c0f1b, 0x13e8, 0x4dec,{ 0xad, 0x35, 0xd3, 0xdd, 0xd2, 0x77, 0x20, 0x3c } };

const MYON_MeshCache MYON_MeshCache::Empty;

#if defined(OPENNURBS_ANNOTATION_TABLE_WIP)
const double MYON_Table::MinColWidth    = 1.0;
const double MYON_Table::MinRowHeight   = 0.1;
const double MYON_Table::MinTableWidth  = 1.0;
const double MYON_Table::MinTableHeight = 0.1;
#endif

const MYON_2iPoint MYON_2iPoint::Origin(0, 0);
const MYON_2iPoint MYON_2iPoint::Unset(MYON_UNSET_INT_INDEX, MYON_UNSET_INT_INDEX);

const MYON_2iVector MYON_2iVector::Zero(0, 0);
const MYON_2iVector MYON_2iVector::UnitX(1, 0);
const MYON_2iVector MYON_2iVector::UnitY(0, 1);
const MYON_2iVector MYON_2iVector::Unset(MYON_UNSET_INT_INDEX, MYON_UNSET_INT_INDEX);

const MYON_2iBoundingBox MYON_2iBoundingBox::Zero(MYON_2iPoint::Origin,MYON_2iPoint::Origin);
const MYON_2iBoundingBox MYON_2iBoundingBox::Unset(MYON_2iPoint::Unset, MYON_2iPoint::Unset);

const MYON_2iSize MYON_2iSize::Zero(0, 0);
const MYON_2iSize MYON_2iSize::Unset(MYON_UNSET_INT_INDEX, MYON_UNSET_INT_INDEX);

const MYON_4iRect MYON_4iRect::Zero(0, 0, 0, 0);
const MYON_4iRect MYON_4iRect::Unset(MYON_UNSET_INT_INDEX, MYON_UNSET_INT_INDEX, MYON_UNSET_INT_INDEX, MYON_UNSET_INT_INDEX);

const MYON_2dSize MYON_2dSize::Zero(0.0, 0.0);
const MYON_2dSize MYON_2dSize::Unset(MYON_UNSET_VALUE, MYON_UNSET_VALUE);

const MYON_4dRect MYON_4dRect::Zero(0.0, 0.0, 0.0, 0.0);
const MYON_4dRect MYON_4dRect::Unset(MYON_UNSET_VALUE, MYON_UNSET_VALUE, MYON_UNSET_VALUE, MYON_UNSET_VALUE);

const MYON_OutlineFigure::Orientation MYON_Outline::DefaultOuterOrientation = MYON_OutlineFigure::Orientation::Clockwise;

const MYON_OutlineFigurePoint MYON_OutlineFigurePoint::Unset;
static MYON_OutlineFigurePoint Internal_FontGlyphOutlinePoint()
{
  MYON_OutlineFigurePoint p;
  p.m_point_type = MYON_OutlineFigurePoint::Type::Error;
  return p;
}
const MYON_OutlineFigurePoint MYON_OutlineFigurePoint::Error(Internal_FontGlyphOutlinePoint());

const MYON_OutlineFigure MYON_OutlineFigure::Unset;

const MYON_Outline MYON_Outline::Unset;

const MYON_FontGlyph MYON_FontGlyph::Unset;

const MYON_PANOSE1 MYON_PANOSE1::Zero;

const double MYON_FontMetrics::DefaultLineFeedRatio = 1.6;
const MYON__UINT32 MYON_FontMetrics::HeightOfCapitalCodePoint = 'I';
const MYON_FontMetrics MYON_FontMetrics::Unset;

static MYON_FontMetrics Internal_LastResortNormalizedFontMetrics()
{
  /*
    Normalized font metrics:
      Units per EM = 256
      Ascent = 232
      Descent = -55
      Distance between baselines = 295
      Ascent of I = 184
      Strikeout thickness = 10
      Strikeout position = 92
      Underscore thickness = 19
      Underscore position = -37
    Font file metrics:
      Units per EM = 2048
      Ascent = 1854
      Descent = -434
      Distance between baselines = 2355
      Ascent of I = 1466
      Strikeout thickness = 75
      Strikeout position = 733
      Underscore thickness = 150
      Underscore position = -292
  
  */
  const int last_restort_UPM = 256; // needs to be as large or larger than "real" UPM
  MYON_FontMetrics last_restort;
  last_restort.SetHeights(232,-55,last_restort_UPM,295);
  last_restort.SetAscentOfI(184);
  last_restort.SetStrikeout(92, 10);
  last_restort.SetUnderscore(-37, 19);

  if ( false == last_restort.IsSet() || false == last_restort.HeightsAreValid())
  {
    MYON_ERROR("Immediately fix MYON_FontMetrics::LastResortNormailizedMetrics");
    return last_restort;
  }

  // As of March 2018, MYON_Font::Constants::AnnotationFontCellHeight = 256 and 
  // normalized_scale = 1.0; The code below is in place to handle any future
  // changes to MYON_Font::Constants::AnnotationFontCellHeight.
  const double normalized_scale = ((double)MYON_Font::Constants::AnnotationFontCellHeight) / ((double)last_restort.UPM());
  if ( 1.0 == normalized_scale )
    return last_restort;

  const MYON_FontMetrics last_restort_normalized =  MYON_FontMetrics::Scale(last_restort, normalized_scale);
  if ( false == last_restort_normalized.IsSet() || false == last_restort_normalized.HeightsAreValid())
  {
    MYON_ERROR("Immediately fix scale section of MYON_FontMetrics::LastResortNormailizedMetrics");
    return last_restort;
  }

  return last_restort_normalized;
}

const MYON_FontMetrics MYON_FontMetrics::LastResortNormalizedMetrics( Internal_LastResortNormalizedFontMetrics());

static MYON_FontMetrics Internal_LastResortFontMetrics()
{
  const int last_restort_UPM = 2048; // needs to be as large or larger than any common "real" UPM
  const double scale = ((double)last_restort_UPM) / ((double)MYON_Font::Constants::AnnotationFontCellHeight);
  MYON_FontMetrics last_restort = MYON_FontMetrics::Scale(MYON_FontMetrics::LastResortNormalizedMetrics, scale);
  return last_restort;
}

const MYON_FontMetrics MYON_FontMetrics::LastResortMetrics( Internal_LastResortFontMetrics());

MYON_Font::MYON_GetGlyphMetricsFuncType MYON_Font::Internal_CustomGetGlyphMetricsFunc = nullptr;
MYON_Font::MYON_GetFontMetricsFuncType MYON_Font::Internal_CustomGetFontMetricsFunc = nullptr;
MYON_Font::MYON_GetGlyphOutlineFuncType MYON_Font::Internal_CustomGetGlyphOutlineFunc = nullptr;

// Used to test freetype
//MYON_Font::MYON_GetGlyphMetricsFuncType MYON_Font::Internal_CustomGetGlyphMetricsFunc = MYON_FreeTypeGetGlyphMetrics;
//MYON_Font::MYON_GetFontMetricsFuncType MYON_Font::Internal_CustomGetFontMetricsFunc = MYON_FreeTypeGetFontMetrics;
//MYON_Font::MYON_GetGlyphOutlineFuncType MYON_Font::Internal_CustomGetGlyphOutlineFunc = MYON_FreeTypeGetGlyphOutline;

//// Used to test WIndows DWrite
//#include "opennurbs_internal_glyph.h"
//MYON_Font::MYON_GetGlyphMetricsFuncType MYON_Font::Internal_CustomGetGlyphMetricsFunc = MYON_WindowsDWriteGetGlyphMetrics;
//MYON_Font::MYON_GetFontMetricsFuncType MYON_Font::Internal_CustomGetFontMetricsFunc = MYON_WindowsDWriteGetFontMetrics;
//MYON_Font::MYON_GetGlyphOutlineFuncType MYON_Font::Internal_CustomGetGlyphOutlineFunc = MYON_WindowsDWriteGetGlyphOutline;


unsigned int MYON_Font::__runtime_serial_number_generator = 1;

static void Internal_SystemModelComponentInit(
  MYON_UUID id,
  int index,
  const wchar_t* name,
  MYON_ModelComponent& model_component
  )
{
  if (MYON_nil_uuid != id)
    model_component.SetId(id);
  if (MYON_UNSET_INT_INDEX != index)
    model_component.SetIndex(index);
  if (nullptr != name && 0 != name[0])
    model_component.SetName(name);

  // non-empty names are locked after application localization
  model_component.LockAllSettingsExceptName();
  if (model_component.NameIsEmpty())
    model_component.LockName();
}

const wchar_t* MYON_Font::DefaultFamilyName()
{
  const wchar_t* s;
#if defined(MYON_RUNTIME_WIN)
  s = L"Arial";
#elif defined(MYON_RUNTIME_APPLE)
  s = L"Helvetica Neue";
#else
  s = L"Arial";
#endif
  return s;
}

const wchar_t* MYON_Font::DefaultFaceName()
{
  const wchar_t* s;
#if defined(MYON_RUNTIME_WIN)
  s = L"Regular";
#elif defined(MYON_RUNTIME_APPLE)
  s = L"Regular";
#else
  s = L"Regular";
#endif
  return s;
}

const wchar_t* MYON_Font::DefaultPostScriptName()
{
  const wchar_t* s;
#if defined(MYON_RUNTIME_WIN)
  s = L"ArialMT";
#elif defined(MYON_RUNTIME_APPLE)
  s = L"HelveticaNeue";
#else
  s = L"ArialMT";
#endif
  return s;
}

const wchar_t* MYON_Font::DefaultWindowsLogfontName()
{
  const wchar_t* s;
#if defined(MYON_RUNTIME_WIN)
  s = L"Arial";
#elif defined(MYON_RUNTIME_APPLE)
  s = L"Helvetica Neue";
#else
  s = L"Arial";
#endif
  return s;
}

#include "opennurbs_internal_glyph.h"

// MYON_GlyphMap::GlyphPool needs to be allocated before  MYON_ManagedFonts::List. 
// This is so the pool is still around when the MYON_ManagedFonts::List
// is being destroyed.
MYON_Internal_FontGlyphPool MYON_Internal_FontGlyphPool::theGlyphItemPool;

// MYON_ManagedFonts::List needs to be allocated before MYON_Font::Default
// This list of installed fonts is used to initialize MYON_Font::Default.
MYON_ManagedFonts MYON_ManagedFonts::List((MYON__UINT_PTR)0);

// The MYON_PTR_SEMAPHORE1 parameter to the
// MYON_Font::MYON_Font(const MYON_Font&) copy ctor triggers special
// initialization for the static font MYON_Font::Unset.
// See MYON_Font::MYON_Font(const MYON_Font&) and MYON_Font::Internal_CopyFrom()
// for implementation details.
const MYON_Font MYON_Font::Unset(*((const MYON_Font*)MYON_PTR_SEMAPHORE1));

// The MYON_PTR_SEMAPHORE2 parameter to the
// MYON_Font::MYON_Font(const MYON_Font&) copy ctor triggers special
// initialization for the static font MYON_Font::Default.
// See MYON_Font::MYON_Font(const MYON_Font&) and MYON_Font::Internal_CopyFrom()
// for implementation details.
const MYON_Font MYON_Font::Default(*((const MYON_Font*)MYON_PTR_SEMAPHORE2));

const MYON_Geometry MYON_Geometry::Unset;

const MYON_3dmObjectAttributes MYON_3dmObjectAttributes::Unset;

const MYON_3dmObjectAttributes MYON_3dmObjectAttributes::DefaultAttributes;


static const MYON_ModelComponentTypeIterator MYON_ModelComponentIterator_Init(
  int list_selector
  )
{
  //const MYON_ModelComponent::Type all_types[] =
  //{
  //  MYON_ModelComponent::Type::Unset,
  //  MYON_ModelComponent::Type::Image,
  //  MYON_ModelComponent::Type::TextureMapping,
  //  MYON_ModelComponent::Type::RenderMaterial,
  //  MYON_ModelComponent::Type::LinePattern,
  //  MYON_ModelComponent::Type::Layer,
  //  MYON_ModelComponent::Type::Group,
  //  MYON_ModelComponent::Type::TextStyle,
  //  MYON_ModelComponent::Type::DimStyle,
  //  MYON_ModelComponent::Type::RenderLight,
  //  MYON_ModelComponent::Type::HatchPattern,
  //  MYON_ModelComponent::Type::InstanceDefinition,
  //  MYON_ModelComponent::Type::ModelGeometry,
  //  MYON_ModelComponent::Type::HistoryRecord,
  //  MYON_ModelComponent::Type::Mixed,
  //};

  const MYON_ModelComponent::Type explicit_types[] =
  {
    //ALWAYS EXCLUDE// MYON_ModelComponent::Type::Unset,
    MYON_ModelComponent::Type::Image,
    MYON_ModelComponent::Type::TextureMapping,
    MYON_ModelComponent::Type::RenderMaterial,
    MYON_ModelComponent::Type::LinePattern,
    MYON_ModelComponent::Type::Layer,
    MYON_ModelComponent::Type::Group,
    MYON_ModelComponent::Type::TextStyle,
    MYON_ModelComponent::Type::DimStyle,
    MYON_ModelComponent::Type::RenderLight,
    MYON_ModelComponent::Type::HatchPattern,
    MYON_ModelComponent::Type::InstanceDefinition,
    MYON_ModelComponent::Type::ModelGeometry,
    MYON_ModelComponent::Type::HistoryRecord,
    //ALWAYS EXCLUDE// MYON_ModelComponent::Type::Mixed,
  };

  const MYON_ModelComponent::Type table_types[] =
  {
    //ALWAYS EXCLUDE// MYON_ModelComponent::Type::Unset,
    //MYON_ModelComponent::Type::Image,
    MYON_ModelComponent::Type::TextureMapping,
    MYON_ModelComponent::Type::RenderMaterial,
    MYON_ModelComponent::Type::LinePattern,
    MYON_ModelComponent::Type::Layer,
    MYON_ModelComponent::Type::Group,
    MYON_ModelComponent::Type::TextStyle,
    MYON_ModelComponent::Type::DimStyle,
    //MYON_ModelComponent::Type::RenderLight,
    MYON_ModelComponent::Type::HatchPattern,
    MYON_ModelComponent::Type::InstanceDefinition,
    //MYON_ModelComponent::Type::ModelGeometry,
    //MYON_ModelComponent::Type::HistoryRecord,
    //ALWAYS EXCLUDE// MYON_ModelComponent::Type::Mixed,
  };

  switch (list_selector)
  {
    //case 0:
    //  return MYON_ModelComponentTypeIterator(sizeof(all_types) / sizeof(all_types[0]), all_types);
  case 1:
    return MYON_ModelComponentTypeIterator(sizeof(explicit_types) / sizeof(explicit_types[0]), explicit_types);
  case 2:
    return MYON_ModelComponentTypeIterator(sizeof(table_types) / sizeof(table_types[0]), table_types);
  }

  return MYON_ModelComponentTypeIterator(0, nullptr);
}

const MYON_ModelComponentTypeIterator MYON_ModelComponentTypeIterator::ExplicitComponentTypes(MYON_ModelComponentIterator_Init(1));
const MYON_ModelComponentTypeIterator MYON_ModelComponentTypeIterator::TableComponentTypes(MYON_ModelComponentIterator_Init(2));

const MYON_ModelComponent MYON_ModelComponent::Unset(MYON_ModelComponent::Type::Unset, (MYON__UINT64) 0);

const MYON_ModelComponentReference MYON_ModelComponentReference::Empty;
const MYON_ModelComponentWeakReference MYON_ModelComponentWeakReference::Empty;

const MYON_ModelComponentContentMark MYON_ModelComponentContentMark::Unset;


// explicit constructor added to work around Apple CLANG bug.
const MYON_ComponentManifest MYON_ComponentManifest::Empty;

const MYON_ManifestMapItem MYON_ManifestMapItem::Unset;

// explicit constructor added to work around Apple CLANG bug.
const MYON_ManifestMap MYON_ManifestMap::Empty;

const MYON_ComponentManifestItem MYON_ComponentManifestItem::UnsetItem;

const MYON_Bitmap MYON_Bitmap::Unset;

const MYON_EmbeddedBitmap MYON_EmbeddedBitmap::Unset;

const MYON_WindowsBitmap MYON_WindowsBitmap::Unset;

const MYON_WindowsBitmapEx MYON_WindowsBitmapEx::Unset;

const MYON_Light MYON_Light::Unset;

const MYON_InstanceDefinition MYON_InstanceDefinition::Unset;

const MYON_ModelGeometryComponent MYON_ModelGeometryComponent::Unset = MYON_ModelGeometryComponent(MYON_ModelComponent::Type::Unset);

const MYON_Texture MYON_Texture::Default;

const double MYON_Material::MaxShine = 255.0;

const MYON_Material MYON_Material::Unset;

static MYON_Material Internal_SystemMaterialInit(int index)
{
  MYON_Material m;
  switch (index)
  {
  case -1: // Default
    {
      // {CF2BE1DE-B81C-4D69-9418-DDE91D266086}
      const MYON_UUID id =
      { 0xcf2be1de, 0xb81c, 0x4d69, { 0x94, 0x18, 0xdd, 0xe9, 0x1d, 0x26, 0x60, 0x86 } };
      Internal_SystemModelComponentInit(id, index, nullptr, m);
      m.m_diffuse = MYON_Color::Gray250;
    }
    break;

  case -2: // Locked Object Color
    {
      // {70BD1640-E92D-4FFF-818A-9A150BAE3139}
      const MYON_UUID id =
      { 0x70bd1640, 0xe92d, 0x4fff, { 0x81, 0x8a, 0x9a, 0x15, 0xb, 0xae, 0x31, 0x39 } };
      Internal_SystemModelComponentInit(id, index, nullptr, m);
      m.m_diffuse = MYON_Color::Gray105;
    }
    break;

  default:
    MYON_ERROR("Invalid index.");
    break;
  }
  return m;
}
const MYON_Material MYON_Material::Default(Internal_SystemMaterialInit(-1));
const MYON_Material MYON_Material::DefaultLockedObject(Internal_SystemMaterialInit(-2));

const MYON_TextureMapping MYON_TextureMapping::Unset;

static MYON_TextureMapping SurfaceParameterTextureMappingInitializer()
{
  //// {B988A6C2-61A6-45a7-AAEE-9AED7EF4E316}
  const MYON_UUID srfp_mapping_id = { 0xb988a6c2, 0x61a6, 0x45a7,{ 0xaa, 0xee, 0x9a, 0xed, 0x7e, 0xf4, 0xe3, 0x16 } };

  MYON_TextureMapping tm;
  tm.SetId(srfp_mapping_id);
  tm.SetIndex(-1);
  // name = empty

  tm.m_type = MYON_TextureMapping::TYPE::srfp_mapping;

  return tm;
}
const MYON_TextureMapping MYON_TextureMapping::SurfaceParameterTextureMapping(SurfaceParameterTextureMappingInitializer());

const MYON_MappingTag MYON_MappingTag::Unset;
const MYON_MappingTag MYON_MappingTag::SurfaceParameterMapping(MYON_TextureMapping::SurfaceParameterTextureMapping,nullptr);

const MYON_LinetypeSegment MYON_LinetypeSegment::Unset;
const MYON_LinetypeSegment MYON_LinetypeSegment::OneMillimeterLine(1.0, MYON_LinetypeSegment::eSegType::stLine);


const MYON_Group MYON_Group::Unset;

const MYON_Linetype MYON_Linetype::Unset;

static MYON_Linetype Internal_BuiltInLinePattern( int index )
{
  MYON_Linetype line_pattern;
  MYON_SimpleArray<double> segments(8);

  switch (index)
  {
  case -1: // Continuous
    {
      const MYON_UUID line_pattern_id =
      { 0x3999bed5, 0x78ee, 0x4d73,{ 0xa0, 0x59, 0x3, 0x22, 0x24, 0xc6, 0xfd, 0x55 } };
      Internal_SystemModelComponentInit(
        line_pattern_id,
        index,
        L"Continuous",
        line_pattern
      );
      line_pattern.AppendSegment(MYON_LinetypeSegment::OneMillimeterLine);
    }
    break;

  case -2: // By Layer
    {
      const MYON_UUID line_pattern_id =
      { 0x913882da, 0xbce9, 0x4a67,{ 0x8d, 0x86, 0xd4, 0x49, 0xfd, 0x58, 0x50, 0xb8 } };
      Internal_SystemModelComponentInit(
        line_pattern_id,
        index,
        L"By Layer",
        line_pattern
        );
      line_pattern.AppendSegment(MYON_LinetypeSegment::OneMillimeterLine);
    }
    break;

  case -3: // By Parent
    {
      const MYON_UUID line_pattern_id =
      { 0xef59d771, 0x5099, 0x4f60,{ 0x99, 0x14, 0xc1, 0x83, 0x6a, 0xeb, 0xe4, 0x84 } };
      Internal_SystemModelComponentInit(
        line_pattern_id,
        index,
        L"By Parent",
        line_pattern
        );
      line_pattern.AppendSegment(MYON_LinetypeSegment::OneMillimeterLine);
    }
    break;

  case -4: // Hidden
    {
      // {7A55AC0F-803C-431B-A7FE-A43319436C8A}
      const MYON_UUID line_pattern_id = 
      { 0x7a55ac0f, 0x803c, 0x431b, { 0xa7, 0xfe, 0xa4, 0x33, 0x19, 0x43, 0x6c, 0x8a } };
      Internal_SystemModelComponentInit(
        line_pattern_id,
        index,
        L"Hidden",
        line_pattern
        );
      segments.Append(2.0);
      segments.Append(-2.0);
    }
    break;

  case -5: // Dashed
    {
      // {864526FB-1EEC-40B9-85E1-4619C1F670C5}
      const MYON_UUID line_pattern_id =
      { 0x864526fb, 0x1eec, 0x40b9, { 0x85, 0xe1, 0x46, 0x19, 0xc1, 0xf6, 0x70, 0xc5 } };
      Internal_SystemModelComponentInit(
        line_pattern_id,
        index,
        L"Dashed",
        line_pattern
        );
      segments.Append(5.0);
      segments.Append(-5.0);
    }
    break;

  case -6: // DashDot
    {
      // {249AD50D-96C7-44A7-AB20-136EAE74C34D}
      const MYON_UUID line_pattern_id =
      { 0x249ad50d, 0x96c7, 0x44a7, { 0xab, 0x20, 0x13, 0x6e, 0xae, 0x74, 0xc3, 0x4d } };
      Internal_SystemModelComponentInit(
        line_pattern_id,
        index,
        L"DashDot",
        line_pattern
        );
      segments.Append(4.0);
      segments.Append(-1.0);
      segments.Append(0.0);
      segments.Append(-1.0);
    }
    break;

  case -7: // Center
    {
      // {B59B1B65-F88F-4854-BED4-E52C51970D0C}
      const MYON_UUID line_pattern_id =
      { 0xb59b1b65, 0xf88f, 0x4854, { 0xbe, 0xd4, 0xe5, 0x2c, 0x51, 0x97, 0xd, 0xc } };
      Internal_SystemModelComponentInit(
        line_pattern_id,
        index,
        L"Center",
        line_pattern
        );
      segments.Append(2.5);
      segments.Append(-0.5);
      segments.Append(0.5);
      segments.Append(-0.5);
    }
    break;

  case -8: // Border
    {
      // {93721775-52EA-4193-83A5-3A541A335630}
      const MYON_UUID line_pattern_id =
      { 0x93721775, 0x52ea, 0x4193, { 0x83, 0xa5, 0x3a, 0x54, 0x1a, 0x33, 0x56, 0x30 } };
      Internal_SystemModelComponentInit(
        line_pattern_id,
        index,
        L"Border",
        line_pattern
        );
      segments.Append(4.0);
      segments.Append(-1.0);
      segments.Append(4.0);
      segments.Append(-1.0);
      segments.Append(1.0);
      segments.Append(-1.0);
    }
    break;

  case -9: // Dots
    {
      // {E6A283A4-EBBE-4C25-A20C-B1135A791908}
      const MYON_UUID line_pattern_id =
      { 0xe6a283a4, 0xebbe, 0x4c25, { 0xa2, 0xc, 0xb1, 0x13, 0x5a, 0x79, 0x19, 0x8 } };
      Internal_SystemModelComponentInit(
        line_pattern_id,
        index,
        L"Dots",
        line_pattern
        );
      segments.Append(0.0);
      segments.Append(-1.0);
    }
    break;

  default:
    MYON_ERROR("Invalid index parameter");
    break;
  }

  if (segments.Count() > 0)
  {
    MYON_LinetypeSegment segment;
    for( int i = 0; i < segments.Count(); i++)
    {
      segment.m_length = fabs(segments[i]);
      segment.m_seg_type = (segments[i] >=  0.0)
                         ? MYON_LinetypeSegment::eSegType::stLine
                         : MYON_LinetypeSegment::eSegType::stSpace;    
      line_pattern.AppendSegment(segment);
    }
  }
  line_pattern.LockPattern();

  return line_pattern;
}

const MYON_Linetype MYON_Linetype::Continuous(Internal_BuiltInLinePattern(-1));
const MYON_Linetype MYON_Linetype::ByLayer(Internal_BuiltInLinePattern(-2));
const MYON_Linetype MYON_Linetype::ByParent(Internal_BuiltInLinePattern(-3));
const MYON_Linetype MYON_Linetype::Hidden(Internal_BuiltInLinePattern(-4));
const MYON_Linetype MYON_Linetype::Dashed(Internal_BuiltInLinePattern(-5));
const MYON_Linetype MYON_Linetype::DashDot(Internal_BuiltInLinePattern(-6));
const MYON_Linetype MYON_Linetype::Center(Internal_BuiltInLinePattern(-7));
const MYON_Linetype MYON_Linetype::Border(Internal_BuiltInLinePattern(-8));
const MYON_Linetype MYON_Linetype::Dots(Internal_BuiltInLinePattern(-9));

static void TextStyleInit(
  const wchar_t* name,
  MYON_UUID id,
  int index,
  const MYON_Font* font,
  MYON_TextStyle& text_style
  )
{
  MYON_wString font_description;
  bool bLockName = false;
  if (nullptr != font)
  {
    font_description = font->Description();
    text_style.SetFont(font);

    if ( (nullptr == name || 0 == name[0]) && font_description.IsNotEmpty() )
    {
      name = font_description;
      bLockName = true;
    }
  }

  Internal_SystemModelComponentInit(id, index, name, text_style);

  if (bLockName)
    text_style.LockName();
}

const MYON_Layer MYON_Layer::Unset;

static MYON_Layer MYON_Layer_Default()
{
  // {061DF99E-2EF8-4A3F-8F2D-4B123A166089}
  const MYON_UUID id = { 0x61df99e, 0x2ef8, 0x4a3f,{ 0x8f, 0x2d, 0x4b, 0x12, 0x3a, 0x16, 0x60, 0x89 } };
  MYON_Layer layer;
  layer.SetParentId(MYON_nil_uuid);
  Internal_SystemModelComponentInit(id, -1, L"Default", layer);
  return layer;
}
const MYON_Layer MYON_Layer::Default = MYON_Layer_Default();

static MYON_TextStyle UnsetTextStyle()
{
  MYON_TextStyle text_style;
  TextStyleInit(nullptr, MYON_nil_uuid, 0, nullptr, text_style);
  return text_style;
}

static MYON_TextStyle DefaultTextStyle()
{
  // {8F3A5848-7741-4AA9-B6A0-FA4F76C9D918}
  const MYON_UUID default_text_style_id =
  { 0x8f3a5848, 0x7741, 0x4aa9,{ 0xb6, 0xa0, 0xfa, 0x4f, 0x76, 0xc9, 0xd9, 0x18 } };

  MYON_TextStyle text_style;
  TextStyleInit(L"Default", default_text_style_id, -1, &MYON_Font::Default, text_style);
  return text_style;
}

static MYON_TextStyle ByLayerTextStyle()
{
  // {DA800C9A-EB00-4251-8237-615017F3BB67}
  const MYON_UUID ByLayer_text_style_id =
  { 0xda800c9a, 0xeb00, 0x4251,{ 0x82, 0x37, 0x61, 0x50, 0x17, 0xf3, 0xbb, 0x67 } };

  MYON_TextStyle text_style;
  TextStyleInit(L"By Layer", ByLayer_text_style_id, -2, nullptr, text_style);
  return text_style;
}

static MYON_TextStyle ByParentTextStyle()
{
  // {4D82AFFA-0433-4CE0-92C8-BD328E23C49F}
  const MYON_UUID ByParent_text_style_id =
  { 0x4d82affa, 0x433, 0x4ce0,{ 0x92, 0xc8, 0xbd, 0x32, 0x8e, 0x23, 0xc4, 0x9f } };

  MYON_TextStyle text_style;
  TextStyleInit(L"By Parent", ByParent_text_style_id, -3, nullptr, text_style);
  return text_style;
}

const MYON_TextStyle MYON_TextStyle::Unset(UnsetTextStyle());
const MYON_TextStyle MYON_TextStyle::Default(DefaultTextStyle());
const MYON_TextStyle MYON_TextStyle::ByLayer(ByLayerTextStyle());
const MYON_TextStyle MYON_TextStyle::ByParent(ByParentTextStyle());

const MYON_TextMask MYON_TextMask::None;

//static bool ValidateCpyStuff(const MYON_DimStyle& src)
//{
//  MYON_DimStyle cpyctor(src);
//  MYON_DimStyle opeq;
//  opeq = src;
//  const MYON_ScaleValue v1 = src.ScaleValue();
//  const MYON_ScaleValue v2 = cpyctor.ScaleValue();
//  const MYON_ScaleValue v3 = opeq.ScaleValue();
//
//  bool rc = v1.RightToLeftScale() == v2.RightToLeftScale() && v1.RightToLeftScale() == v2.RightToLeftScale();
//
//  return rc;
//}

const MYON_FontFaceQuartet MYON_FontFaceQuartet::Empty;

static void DimStyleInit(
  const wchar_t* name,
  int index,
  MYON_UUID id,
  MYON_DimStyle& dimension_style
  )
{
  Internal_SystemModelComponentInit(id, index, name, dimension_style);

  dimension_style.SetAlternatePrefix(L" [");
  dimension_style.SetAlternateSuffix(L"]");
  // identical values for most or all built-in dimension styles
  dimension_style.SetFont(MYON_Font::Default);
  dimension_style.SetDimTextLocation(MYON_DimStyle::TextLocation::AboveDimLine);
}

static void Internal_SystemDimStyleFinalize(
  MYON_DimStyle& dimension_style
)
{
  dimension_style.ClearAllFieldOverrides();
  dimension_style.ContentHash();
}

// Static DimStyle definitions

static void DimStyleDefaultInit(MYON_DimStyle& ds)
{
  ds.SetExtExtension                     (0.5);
  ds.SetExtOffset                        (0.5);
  ds.SetArrowSize                        (1.0);
  ds.SetLeaderArrowSize                  (1.0);
  ds.SetCenterMark                       (0.5);
  ds.SetCenterMarkStyle                  (MYON_DimStyle::centermark_style::Mark);
  ds.SetTextGap                          (0.25);
  ds.SetTextHeight                       (1.0);
  ds.SetDimTextLocation                  (MYON_DimStyle::TextLocation::AboveDimLine);
  ds.SetDimRadialTextLocation            (MYON_DimStyle::TextLocation::InDimLine);
  ds.SetAngleFormat                      (MYON_DimStyle::angle_format::DecimalDegrees);
  ds.SetAngleResolution                  (2);
  ds.SetDimensionLengthDisplay           (MYON_DimStyle::LengthDisplay::ModelUnits);
  ds.SetAlternateDimensionLengthDisplay  (MYON_DimStyle::LengthDisplay::ModelUnits);
  ds.SetLengthResolution                 (2);
  ds.SetAlternateLengthResolution        (2);
  ds.SetLengthFactor                     (1.0);
  ds.SetAlternateLengthFactor            (1.0);
  ds.SetAlternate                        (false);
  ds.SetForceDimLine                     (true);
  ds.SetDecimalSeparator                 (L'.');
  ds.SetPrefix                           (L"");
  ds.SetSuffix                           (L"");
  ds.SetAlternatePrefix                  (L" [");
  ds.SetAlternateSuffix                  (L"]");
  ds.SetDimExtension                     (0.0);
  ds.SetSuppressExtension1               (false);
  ds.SetSuppressExtension2               (false);
  ds.SetToleranceFormat                  (MYON_DimStyle::tolerance_format::None);
  ds.SetToleranceResolution              (4);
  ds.SetToleranceUpperValue              (0.0);
  ds.SetToleranceLowerValue              (0.0);
  ds.SetToleranceHeightScale             (1.0);
  ds.SetBaselineSpacing                  (3.0);
     MYON_TextMask tm;
  ds.SetTextMask                         (tm);
  ds.SetDimScaleSource                   (0);
  ds.SetExtensionLineColorSource         (MYON::object_color_source::color_from_layer);
  ds.SetDimensionLineColorSource         (MYON::object_color_source::color_from_layer);
  ds.SetArrowColorSource                 (MYON::object_color_source::color_from_layer);
  ds.SetTextColorSource                  (MYON::object_color_source::color_from_layer);
  ds.SetExtensionLineColor               (MYON_Color(0));
  ds.SetDimensionLineColor               (MYON_Color(0));
  ds.SetArrowColor                       (MYON_Color(0));
  ds.SetTextColor                        (MYON_Color(0));
  ds.SetExtensionLinePlotColorSource     (MYON::plot_color_source::plot_color_from_layer);
  ds.SetDimensionLinePlotColorSource     (MYON::plot_color_source::plot_color_from_layer);
  ds.SetArrowPlotColorSource             (MYON::plot_color_source::plot_color_from_layer);
  ds.SetTextPlotColorSource              (MYON::object_color_source::color_from_layer);
  ds.SetExtensionLinePlotColor           (MYON_Color(0));
  ds.SetDimensionLinePlotColor           (MYON_Color(0));
  ds.SetArrowPlotColor                   (MYON_Color(0));
  ds.SetTextPlotColor                    (MYON_Color(0));
  ds.SetExtensionLinePlotWeightSource    (MYON::plot_weight_source::plot_weight_from_layer);
  ds.SetDimensionLinePlotWeightSource    (MYON::plot_weight_source::plot_weight_from_layer);
  ds.SetExtensionLinePlotWeight          (0.0);
  ds.SetDimensionLinePlotWeight          (0.0);
  ds.SetFixedExtensionLen                (1.0);
  ds.SetFixedExtensionLenOn              (false);
  ds.SetTextRotation                     (0.0);
  ds.SetAlternateLengthResolution        (4);
  ds.SetToleranceHeightScale             (0.6);
  ds.SetSuppressArrow1                   (false);
  ds.SetSuppressArrow2                   (false);
  ds.SetTextMoveLeader                   (0);
  ds.SetArcLengthSymbol                  (L'\0');
  ds.SetStackHeightScale                 (0.7);
  ds.SetStackFractionFormat              (MYON_DimStyle::stack_format::StackHorizontal);
  ds.SetAlternateRoundOff                (0.0);
  ds.SetRoundOff                         (0.0);
  ds.SetAngleRoundOff                    (0.0);
  ds.SetZeroSuppress                     (MYON_DimStyle::suppress_zero::None);
  ds.SetAlternateZeroSuppress            (MYON_DimStyle::suppress_zero::None);
  ds.SetAngleZeroSuppress                (MYON_DimStyle::suppress_zero::None);
  ds.SetAlternateBelow                   (false);
  ds.SetArrowType1                       (MYON_Arrowhead::arrow_type::SolidTriangle);
  ds.SetArrowType2                       (MYON_Arrowhead::arrow_type::SolidTriangle);
  ds.SetLeaderArrowType                  (MYON_Arrowhead::arrow_type::SolidTriangle);
  ds.SetTextVerticalAlignment            (MYON::TextVerticalAlignment::Top);
  ds.SetTextHorizontalAlignment          (MYON::TextHorizontalAlignment::Left);
  ds.SetLeaderTextVerticalAlignment      (MYON::TextVerticalAlignment::MiddleOfTop);
  ds.SetLeaderTextHorizontalAlignment    (MYON::TextHorizontalAlignment::Left);
  ds.SetLeaderContentAngleStyle          (MYON_DimStyle::ContentAngleStyle::Horizontal);
  ds.SetLeaderCurveType                  (MYON_DimStyle::leader_curve_type::Polyline);
  ds.SetLeaderContentAngleRadians        (0.0);
  ds.SetLeaderHasLanding                 (false);
  ds.SetLeaderLandingLength              (1.0);
  ds.SetDrawForward                      (true);
  ds.SetSignedOrdinate                   (true);
  ds.SetDimScale                         (1.0);
  ds.SetUnitSystem                       (MYON::LengthUnitSystem::None);
  ds.SetTextOrientation                  (MYON::TextOrientation::InPlane);
  ds.SetLeaderTextOrientation            (MYON::TextOrientation::InPlane);
  ds.SetDimTextOrientation               (MYON::TextOrientation::InPlane);
  ds.SetDimRadialTextOrientation         (MYON::TextOrientation::InPlane);
  ds.SetDimTextAngleStyle                (MYON_DimStyle::ContentAngleStyle::Aligned);
  ds.SetDimRadialTextAngleStyle          (MYON_DimStyle::ContentAngleStyle::Horizontal);
  ds.SetTextUnderlined                   (false);
}

static void DimStyleMillimeterArchitecturalInit(MYON_DimStyle& ds)
{
  DimStyleDefaultInit(ds);
  ds.SetExtExtension                     (1.0);
  ds.SetArrowSize                        (3.0);
  ds.SetLeaderArrowSize                  (3.0);
  ds.SetCenterMark                       (3.0);
  ds.SetTextGap                          (1.0);
  ds.SetTextHeight                       (3.0);
  ds.SetAngleResolution                  (0);
  ds.SetDimensionLengthDisplay           (MYON_DimStyle::LengthDisplay::Millmeters);
  ds.SetAlternateDimensionLengthDisplay  (MYON_DimStyle::LengthDisplay::InchesDecimal);
  ds.SetLengthResolution                 (0);
  ds.SetBaselineSpacing                  (9.0);
  ds.SetArrowType1                       (MYON_Arrowhead::arrow_type::Tick);
  ds.SetArrowType2                       (MYON_Arrowhead::arrow_type::Tick);
  ds.SetLeaderArrowType                  (MYON_Arrowhead::arrow_type::OpenArrow);
  ds.SetDimScale                         (100.0);
  ds.SetUnitSystem                       (MYON::LengthUnitSystem::Millimeters);
}

static void DimStyleMillimeterLargeInit(MYON_DimStyle& ds)
{
  DimStyleDefaultInit(ds);
  ds.SetExtExtension                     (1.0);
  ds.SetArrowSize                        (3.5);
  ds.SetLeaderArrowSize                  (3.5);
  ds.SetCenterMark                       (3.5);
  ds.SetTextGap                          (1.0);
  ds.SetTextHeight                       (3.5);
  ds.SetAngleResolution                  (0);
  ds.SetDimensionLengthDisplay           (MYON_DimStyle::LengthDisplay::Millmeters);
  ds.SetAlternateDimensionLengthDisplay  (MYON_DimStyle::LengthDisplay::InchesDecimal);
  ds.SetLengthResolution                 (1);
  ds.SetToleranceHeightScale             (1.0);
  ds.SetBaselineSpacing                  (10.5);
  ds.SetLeaderLandingLength              (3.5);
  ds.SetDimScale                         (100.0);
  ds.SetUnitSystem                       (MYON::LengthUnitSystem::Millimeters);
}

static void DimStyleMillimeterSmallInit (MYON_DimStyle& ds)
{
  DimStyleDefaultInit(ds);
  ds.SetExtExtension                     (1.0);
  ds.SetArrowSize                        (3.0);
  ds.SetLeaderArrowSize                  (3.0);
  ds.SetCenterMark                       (2.5);
  ds.SetTextGap                          (0.8);
  ds.SetTextHeight                       (2.5);
  ds.SetAngleResolution                  (1);
  ds.SetDimensionLengthDisplay           (MYON_DimStyle::LengthDisplay::Millmeters);
  ds.SetAlternateDimensionLengthDisplay  (MYON_DimStyle::LengthDisplay::InchesDecimal);
  ds.SetBaselineSpacing                  (7.5);
  ds.SetLeaderLandingLength              (2.5);
  ds.SetDimScale                         (10.0);
  ds.SetUnitSystem                       (MYON::LengthUnitSystem::Millimeters);
}

static void DimStyleInchDecimalInit(MYON_DimStyle& ds)
{
  DimStyleDefaultInit(ds);
  ds.SetExtExtension                     (0.125);
  ds.SetExtOffset                        (0.0625);
  ds.SetArrowSize                        (0.125);
  ds.SetLeaderArrowSize                  (0.125);
  ds.SetCenterMark                       (0.25);
  ds.SetTextGap                          (0.0625);
  ds.SetTextHeight                       (0.125);
  ds.SetAngleResolution                  (0);
  ds.SetDimensionLengthDisplay           (MYON_DimStyle::LengthDisplay::InchesDecimal);
  ds.SetToleranceHeightScale             (1.0);
  ds.SetBaselineSpacing                  (0.38);
  ds.SetLeaderLandingLength              (0.125);
  ds.SetDimScale                         (10.0);
  ds.SetUnitSystem                       (MYON::LengthUnitSystem::Inches);
}

static void DimStyleInchFractionalInit(MYON_DimStyle& ds)
{
  DimStyleDefaultInit(ds);
  ds.SetExtExtension                     (0.125);
  ds.SetExtOffset                        (0.0625);
  ds.SetArrowSize                        (0.1);
  ds.SetLeaderArrowSize                  (0.1);
  ds.SetCenterMark                       (0.25);
  ds.SetTextGap                          (0.0625);
  ds.SetTextHeight                       (0.125);
  ds.SetAngleResolution                  (1);
  ds.SetDimensionLengthDisplay           (MYON_DimStyle::LengthDisplay::InchesFractional);
  ds.SetLengthResolution                 (4);
  ds.SetDimExtension                     (0.1);
  ds.SetBaselineSpacing                  (0.38);
  ds.SetArrowType1                       (MYON_Arrowhead::arrow_type::Tick);
  ds.SetArrowType2                       (MYON_Arrowhead::arrow_type::Tick);
  ds.SetLeaderArrowType                  (MYON_Arrowhead::arrow_type::OpenArrow);
  ds.SetLeaderLandingLength              (0.125);
  ds.SetDimScale                         (12.0);
  ds.SetUnitSystem                       (MYON::LengthUnitSystem::Inches);
}

static void DimStyleFootInchArchitecturalInit(MYON_DimStyle& ds)
{
  DimStyleDefaultInit(ds);
  ds.SetExtExtension                     (0.125);
  ds.SetExtOffset                        (0.0625);
  ds.SetArrowSize                        (0.1);
  ds.SetLeaderArrowSize                  (0.1);
  ds.SetCenterMark                       (0.25);
  ds.SetTextGap                          (0.0625);
  ds.SetTextHeight                       (0.125);
  ds.SetAngleResolution                  (0);
  ds.SetDimensionLengthDisplay           (MYON_DimStyle::LengthDisplay::FeetAndInches);
  ds.SetAlternateLengthResolution        (1);
  ds.SetBaselineSpacing                  (0.38);
  ds.SetZeroSuppress                     (MYON_DimStyle::suppress_zero::SuppressZeroFeet);
  ds.SetArrowType1                       (MYON_Arrowhead::arrow_type::Tick);
  ds.SetArrowType2                       (MYON_Arrowhead::arrow_type::Tick);
  ds.SetLeaderArrowType                  (MYON_Arrowhead::arrow_type::OpenArrow);
  ds.SetLeaderLandingLength              (0.125);
  ds.SetDimScale                         (96.0);
  ds.SetUnitSystem                       (MYON::LengthUnitSystem::Inches);
}

static void DimStyleFeetDecimalInit(MYON_DimStyle& ds)
{
  DimStyleDefaultInit(ds);
  ds.SetExtExtension                     (0.125);
  ds.SetExtOffset                        (0.0625);
  ds.SetArrowSize                        (0.125);
  ds.SetLeaderArrowSize                  (0.125);
  ds.SetCenterMark                       (0.25);
  ds.SetTextGap                          (0.0625);
  ds.SetTextHeight                       (0.125);
  ds.SetAngleResolution                  (0);
  ds.SetDimensionLengthDisplay           (MYON_DimStyle::LengthDisplay::FeetDecimal);
  ds.SetBaselineSpacing                  (0.38);
  ds.SetLeaderLandingLength              (0.125);
  ds.SetDimScale                         (12.0);
  ds.SetUnitSystem                       (MYON::LengthUnitSystem::Inches);
}

static void DimStyleModelUnitsDecimalInit(MYON_DimStyle& ds)
{
  DimStyleDefaultInit(ds);
  ds.SetExtExtension                     (0.125);
  ds.SetExtOffset                        (0.0625);
  ds.SetArrowSize                        (0.125);
  ds.SetLeaderArrowSize                  (0.125);
  ds.SetCenterMark                       (0.25);
  ds.SetTextGap                          (0.0625);
  ds.SetTextHeight                       (0.125);
  ds.SetAngleResolution                  (0);
  ds.SetBaselineSpacing                  (0.38);
  ds.SetLeaderLandingLength              (0.125);
  ds.SetDimScale                         (10.0);
  ds.SetUnitSystem                       (MYON::LengthUnitSystem::Inches);
}

static void DimStyleFeetEngraveInit(MYON_DimStyle& ds)
{
  DimStyleDefaultInit(ds);
  ds.SetDimensionLengthDisplay           (MYON_DimStyle::LengthDisplay::FeetDecimal);
  ds.SetTextVerticalAlignment            (MYON::TextVerticalAlignment::Bottom);
  ds.SetDimScale                         (12.0);
  ds.SetUnitSystem                       (MYON::LengthUnitSystem::Inches);
}

static void DimStyleMillimeterEngraveInit(MYON_DimStyle& ds)
{
  DimStyleDefaultInit(ds);
  ds.SetExtExtension                     (1.5);
  ds.SetExtOffset                        (1.5);
  ds.SetArrowSize                        (3.0);
  ds.SetLeaderArrowSize                  (3.0);
  ds.SetCenterMark                       (1.5);
  ds.SetTextGap                          (0.75);
  ds.SetTextHeight                       (3.0);
  ds.SetDimensionLengthDisplay           (MYON_DimStyle::LengthDisplay::Millmeters);
  ds.SetBaselineSpacing                  (9.0);
  ds.SetFixedExtensionLen                (3.0);
  ds.SetLeaderLandingLength              (3.0);
  ds.SetDimScale                         (10.0);
  ds.SetUnitSystem                       (MYON::LengthUnitSystem::Millimeters);
}

static void DimStyleModelUnitsEngraveInit(MYON_DimStyle& ds)
{
  DimStyleDefaultInit(ds);
  ds.SetAlternateDimensionLengthDisplay  (MYON_DimStyle::LengthDisplay::Millmeters);
  ds.SetToleranceHeightScale             (1.0);
  ds.SetTextVerticalAlignment            (MYON::TextVerticalAlignment::Bottom);
  ds.SetDimScale                         (10.0);
  ds.SetUnitSystem                       (MYON::LengthUnitSystem::Inches);
}


static MYON_DimStyle DimStyleDefault()
{
  const MYON_UUID id =
  { 0x25b90869, 0x22, 0x4e04,{ 0xb4, 0x98, 0x98, 0xb4, 0x17, 0x5f, 0x65, 0xfd } };

  MYON_DimStyle dimstyle;
  DimStyleInit(L"Default", -1, id, dimstyle);
  DimStyleDefaultInit(dimstyle);
  Internal_SystemDimStyleFinalize(dimstyle);
  return dimstyle;
}

static MYON_DimStyle DimStyleInchDecimal()
{
  const MYON_UUID id =
  { 0x2105610c, 0xcfc7, 0x4473,{ 0xa5, 0x80, 0xc3, 0xd9, 0xc, 0xe8, 0xc7, 0xa3 } };

  MYON_DimStyle dimstyle;
  DimStyleInit(L"Inch Decimal", -2, id, dimstyle);
  DimStyleInchDecimalInit(dimstyle);
  Internal_SystemDimStyleFinalize(dimstyle);
  return dimstyle;
}

static MYON_DimStyle DimStyleInchFractional()
{
  const MYON_UUID id =
  { 0x6bcb1506, 0x699f, 0x445d,{ 0xa1, 0x22, 0x4f, 0xc7, 0x78, 0x2b, 0xc4, 0x86 } };

  MYON_DimStyle dimstyle;
  DimStyleInit(L"Inch Fractional", -3, id, dimstyle);
  DimStyleInchFractionalInit(dimstyle);
  Internal_SystemDimStyleFinalize(dimstyle);
  return dimstyle;
}

static MYON_DimStyle DimStyleFootInchArchitectural()
{
  const MYON_UUID id =
  { 0x50d6ef1b, 0xd1d0, 0x408a,{ 0x86, 0xc0, 0xee, 0x8b, 0x36, 0x8, 0x88, 0x3e } };

  MYON_DimStyle dimstyle;
  DimStyleInit(L"Foot-Inch Architectural", -4, id, dimstyle);
  DimStyleFootInchArchitecturalInit(dimstyle);
  Internal_SystemDimStyleFinalize(dimstyle);
  return dimstyle;
}

static MYON_DimStyle DimStyleMillimeterSmall()
{
  const MYON_UUID id =
  { 0xdbe22573, 0x8cad, 0x4ced,{ 0x89, 0x47, 0x3, 0xa0, 0x48, 0xed, 0xde, 0x56 } };

  MYON_DimStyle dimstyle;
  DimStyleInit(L"Millimeter Small", -5, id, dimstyle);
  DimStyleMillimeterSmallInit(dimstyle);
  Internal_SystemDimStyleFinalize(dimstyle);
  return dimstyle;
}

static MYON_DimStyle DimStyleMillimeterLarge()
{
  const MYON_UUID id =
  { 0xf7b30534, 0x773e, 0x45bc,{ 0x9d, 0x87, 0x9d, 0x14, 0x80, 0x9c, 0x96, 0x44 } };

  MYON_DimStyle dimstyle;
  DimStyleInit(L"Millimeter Large", -6, id, dimstyle);
  DimStyleMillimeterLargeInit(dimstyle);
  Internal_SystemDimStyleFinalize(dimstyle);
  return dimstyle;
}

static MYON_DimStyle DimStyleMillimeterArchitectural()
{
  const MYON_UUID id =
  { 0xe5a4c08f, 0x23b3, 0x4033,{ 0x90, 0xb2, 0xfb, 0x31, 0xec, 0x45, 0x92, 0x9b } };

  MYON_DimStyle dimstyle;
  DimStyleInit(L"Millimeter Architectural", -7, id, dimstyle);
  DimStyleMillimeterArchitecturalInit(dimstyle);
  Internal_SystemDimStyleFinalize(dimstyle);
  return dimstyle;
}

static MYON_DimStyle DimStyleFeetDecimal()
{
  // {6F4B1840-8A12-4DE9-BF84-6A98B06C508D}
  const MYON_UUID id =
  { 0x6f4b1840, 0x8a12, 0x4de9, { 0xbf, 0x84, 0x6a, 0x98, 0xb0, 0x6c, 0x50, 0x8d } };

  MYON_DimStyle dimstyle;
  DimStyleInit(L"Feet Decimal", -8, id, dimstyle);
  DimStyleFeetDecimalInit(dimstyle);
  Internal_SystemDimStyleFinalize(dimstyle);
  return dimstyle;
}

static MYON_DimStyle DimStyleModelUnitsDecimal()
{
  const MYON_UUID id =
  { 0x93a38bdf, 0x4c1c, 0x428c, { 0x8b, 0x97, 0x93, 0x59, 0xf1, 0xbd, 0xed, 0x17 } };

  MYON_DimStyle dimstyle;
  DimStyleInit(L"Model Units Decimal", -9, id, dimstyle);
  DimStyleModelUnitsDecimalInit(dimstyle);
  Internal_SystemDimStyleFinalize(dimstyle);
  return dimstyle;
}

static MYON_DimStyle DimStyleFeetEngrave()
{
  const MYON_UUID id =
  { 0xc2d8846b, 0x918d, 0x4779, { 0x96, 0xec, 0x31, 0xb4, 0xe2, 0x75, 0xfb, 0x4e } };

  MYON_DimStyle dimstyle;
  DimStyleInit(L"Feet Engrave", -10, id, dimstyle);
  DimStyleFeetEngraveInit(dimstyle);
  const MYON_Font* font = MYON_Font::DefaultEngravingFont();
  if (nullptr != font)
    dimstyle.SetFont(*font);
  Internal_SystemDimStyleFinalize(dimstyle);
  return dimstyle;
}

static MYON_DimStyle DimStyleMillimeterEngrave()
{
  const MYON_UUID id =
  { 0x741980ff, 0xde0f, 0x4ed7, { 0xaa, 0x6f, 0xee, 0x91, 0xb3, 0xbe, 0x96, 0xc6 } };

  MYON_DimStyle dimstyle;
  DimStyleInit(L"Millimeter Engrave", -11, id, dimstyle);
  DimStyleMillimeterEngraveInit(dimstyle);
  const MYON_Font* font = MYON_Font::DefaultEngravingFont();
  if (nullptr != font)
    dimstyle.SetFont(*font);
  Internal_SystemDimStyleFinalize(dimstyle);
  return dimstyle;
}

static MYON_DimStyle DimStyleModelUnitsEngrave()
{
  const MYON_UUID id =
  { 0x2cc3a895, 0x5389, 0x467e, { 0x9d, 0xbe, 0x3a, 0xca, 0xb4, 0x38, 0x60, 0xfa } };

  MYON_DimStyle dimstyle;
  DimStyleInit(L"Model Units Engrave", -12, id, dimstyle);
  DimStyleModelUnitsEngraveInit(dimstyle);
  const MYON_Font* font = MYON_Font::DefaultEngravingFont();
  if (nullptr != font)
    dimstyle.SetFont(*font);
  Internal_SystemDimStyleFinalize(dimstyle);
  return dimstyle;
}

const MYON_DimStyle MYON_DimStyle::Unset;

const MYON_DimStyle MYON_DimStyle::Default(DimStyleDefault());

const MYON_DimStyle MYON_DimStyle::DefaultInchDecimal(DimStyleInchDecimal());
const MYON_DimStyle MYON_DimStyle::DefaultInchFractional(DimStyleInchFractional());
const MYON_DimStyle MYON_DimStyle::DefaultFootInchArchitecture(DimStyleFootInchArchitectural());

const MYON_DimStyle MYON_DimStyle::DefaultMillimeterSmall(DimStyleMillimeterSmall());
const MYON_DimStyle MYON_DimStyle::DefaultMillimeterLarge(DimStyleMillimeterLarge());
const MYON_DimStyle MYON_DimStyle::DefaultMillimeterArchitecture(DimStyleMillimeterArchitectural());

const MYON_DimStyle MYON_DimStyle::DefaultFeetDecimal(DimStyleFeetDecimal());
const MYON_DimStyle MYON_DimStyle::DefaultModelUnitsDecimal(DimStyleModelUnitsDecimal());

const MYON_DimStyle MYON_DimStyle::DefaultFeetEngrave(DimStyleFeetEngrave());
const MYON_DimStyle MYON_DimStyle::DefaultMillimeterEngrave(DimStyleMillimeterEngrave());
const MYON_DimStyle MYON_DimStyle::DefaultModelUnitsEngrave(DimStyleModelUnitsEngrave());

const MYON_StackedText MYON_StackedText::Empty;
const MYON_TextRun MYON_TextRun::Empty;
const MYON_TextRunArray MYON_TextRunArray::EmptyArray;
const MYON_TextContent MYON_TextContent::Empty;

const MYON_Text MYON_Text::Empty;
const MYON_Leader MYON_Leader::Empty;
const MYON_DimLinear MYON_DimLinear::Empty;
const MYON_DimAngular MYON_DimAngular::Empty;
const MYON_DimRadial MYON_DimRadial::Empty;
const MYON_DimOrdinate MYON_DimOrdinate::Empty;
const MYON_Centermark MYON_Centermark::Empty;

const MYON_HatchLine MYON_HatchLine::Unset(MYON_UNSET_VALUE);           // angle = unset
const MYON_HatchLine MYON_HatchLine::SolidHorizontal(0.0); // angle = 0
const MYON_HatchLine MYON_HatchLine::SolidVertical(MYON_PI / 2);   // angle = pi/2

static MYON_HatchPattern Internal_SolidHatchPatternInit()
{
  // {B319435D-86B6-4D89-972D-1F75FEF0D950}
  MYON_UUID id = { 0xb319435d, 0x86b6, 0x4d89,{ 0x97, 0x2d, 0x1f, 0x75, 0xfe, 0xf0, 0xd9, 0x50 } };


  MYON_HatchPattern hatch_pattern;
  hatch_pattern.SetFillType(MYON_HatchPattern::HatchFillType::Solid);
  Internal_SystemModelComponentInit(id, -1, L"Solid", hatch_pattern);

  return hatch_pattern;
}

static MYON_HatchPattern Internal_LineHatchPatternInit(
  const int hatch_index
  )
{
  MYON_UUID id = MYON_nil_uuid;
  MYON_HatchLine lines[4];
  unsigned int line_count = 0;
  const wchar_t* name = nullptr;
  switch (hatch_index)
  {
  case -2:
  {
    // {3C25B9D3-0521-4120-8877-111FB5EA0016}
    const MYON_UUID idctor = { 0x3c25b9d3, 0x521, 0x4120,{ 0x88, 0x77, 0x11, 0x1f, 0xb5, 0xea, 0x0, 0x16 } };
    id = idctor;
    name = L"Hatch1";
    MYON_HatchLine& line0 = lines[line_count++];
    line0.SetAngleRadians(0.0);
    line0.SetBase(MYON_2dPoint::Origin);
    line0.SetOffset(MYON_2dVector(0.0, 0.125));
  }
  break;

  case -3:
  {
    // {23287413-6AE6-4409-93DC-140796FA7864}
    const MYON_UUID idctor = { 0x23287413, 0x6ae6, 0x4409,{ 0x93, 0xdc, 0x14, 0x7, 0x96, 0xfa, 0x78, 0x64 } };
    id = idctor;
    name = L"Hatch2";
    MYON_HatchLine& line0 = lines[line_count++];
    line0.SetAngleRadians(0.0);
    line0.SetBase(MYON_2dPoint::Origin);
    line0.SetOffset(MYON_2dVector(0.0, 0.375));

    MYON_HatchLine& line1 = lines[line_count++];
    line1 = line0;
    line1.SetBase(MYON_2dPoint(0.0, 0.0625));
  }
  break;

  case -4:
  {
    // {282CBE0A-DB30-4241-BB5B-7290AF0BBD55}
    const MYON_UUID idctor = { 0x282cbe0a, 0xdb30, 0x4241,{ 0xbb, 0x5b, 0x72, 0x90, 0xaf, 0xb, 0xbd, 0x55 } };
    id = idctor;
    name = L"Hatch3";
    MYON_HatchLine& line0 = lines[line_count++];
    line0.SetAngleRadians(0.0);
    line0.SetBase(MYON_2dPoint::Origin);
    line0.SetOffset(MYON_2dVector(0.0, 0.5));

    MYON_HatchLine& line1 = lines[line_count++];
    line1 = line0;
    line1.SetBase(MYON_2dPoint(0.0, 0.0625));

    MYON_HatchLine& line2 = lines[line_count++];
    line2 = line0;
    line2.SetBase(MYON_2dPoint(0.0, 0.125));
  }
  break;

  case -5:
  {
    // {70710CA6-185D-42FE-AEB5-6437A8C15E7B}
    const MYON_UUID idctor = { 0x70710ca6, 0x185d, 0x42fe,{ 0xae, 0xb5, 0x64, 0x37, 0xa8, 0xc1, 0x5e, 0x7b } };
    id = idctor;
    name = L"HatchDash";
    MYON_HatchLine& line0 = lines[line_count++];
    line0.SetAngleRadians(0.0);
    line0.SetBase(MYON_2dPoint::Origin);
    line0.SetOffset(MYON_2dVector(0.0, 0.25));

    MYON_HatchLine& line1 = lines[line_count++];
    line1.SetAngleRadians(0.0);
    line1.SetBase(MYON_2dPoint(0.0, 0.125));
    line1.SetOffset(MYON_2dVector(0.0, 0.25));
    line1.AppendDash(0.125);
    line1.AppendDash(-0.125);
  }
  break;

  case -6:
  {
    // {86D5A4E7-AA8F-4FBA-BAA3-C844EF1704A1}
    const MYON_UUID idctor = { 0x86d5a4e7, 0xaa8f, 0x4fba,{ 0xba, 0xa3, 0xc8, 0x44, 0xef, 0x17, 0x4, 0xa1 } };
    id = idctor;
    name = L"Grid";
    MYON_HatchLine& line0 = lines[line_count++];
    line0.SetAngleRadians(0.0);
    line0.SetBase(MYON_2dPoint::Origin);
    line0.SetOffset(MYON_2dVector(0.0, 0.25));

    MYON_HatchLine& line1 = lines[line_count++];
    line1 = line0;
    line1.SetAngleRadians(MYON_PI / 2.0);
  }
  break;

  case -7:
  {
    // {060226AB-2BA8-4550-BB0A-BC1A3CD8E2A1}
    const MYON_UUID idctor = { 0x60226ab, 0x2ba8, 0x4550,{ 0xbb, 0xa, 0xbc, 0x1a, 0x3c, 0xd8, 0xe2, 0xa1 } };
    id = idctor;
    name = L"Grid60";
    MYON_HatchLine& line0 = lines[line_count++];
    line0.SetAngleRadians(0.0);
    line0.SetOffset(MYON_2dVector(0.0, 0.25));

    MYON_HatchLine& line1 = lines[line_count++];
    line1 = line0;
    line1.SetAngleDegrees(60.0);

    MYON_HatchLine& line2 = lines[line_count++];
    line2 = line0;
    line2.SetAngleDegrees(120.0);
  }
  break;

  case -8:
  {
    // {5FA0C0A0-B5F6-4799-ADED-AE202E260888}
    const MYON_UUID idctor = { 0x5fa0c0a0, 0xb5f6, 0x4799,{ 0xad, 0xed, 0xae, 0x20, 0x2e, 0x26, 0x8, 0x88 } };
    id = idctor;
    name = L"Plus";
    MYON_HatchLine& line0 = lines[line_count++];
    line0.SetAngleRadians(0.0);
    line0.SetBase(MYON_2dPoint(-0.125, 0.0));
    line0.SetOffset(MYON_2dVector(0.0, 1.0));
    line0.AppendDash(0.25);
    line0.AppendDash(-0.75);

    MYON_HatchLine& line1 = lines[line_count++];
    line1 = line0;
    line1.SetAngleRadians(MYON_PI / 2.0);
    line1.SetBase(MYON_2dPoint(0.0, -0.125));
  }
  break;

  case -9:
  {
    // {05B0404B-77AD-4206-8022-FD7D9D0ECECF}
    const MYON_UUID idctor = { 0x5b0404b, 0x77ad, 0x4206,{ 0x80, 0x22, 0xfd, 0x7d, 0x9d, 0xe, 0xce, 0xcf } };
    id = idctor;
    name = L"Squares";
    MYON_HatchLine& line0 = lines[line_count++];
    line0.SetAngleRadians(0.0);
    line0.SetBase(MYON_2dPoint::Origin);
    line0.SetOffset(MYON_2dVector(0.0, 0.25));
    line0.AppendDash(0.25);
    line0.AppendDash(-0.25);

    MYON_HatchLine& line1 = lines[line_count++];
    line1 = line0;
    line1.SetAngleRadians(MYON_PI / 2.0);
  }
  break;
  }

  MYON_HatchPattern hatch_pattern;
  hatch_pattern.SetHatchLines(line_count, lines);
  Internal_SystemModelComponentInit(id, hatch_index, name, hatch_pattern);
  return hatch_pattern;
}

const MYON_HatchPattern MYON_HatchPattern::Unset;
const MYON_HatchPattern MYON_HatchPattern::Solid(Internal_SolidHatchPatternInit()); // index = -1, id set, unique and persistent
const MYON_HatchPattern MYON_HatchPattern::Hatch1(Internal_LineHatchPatternInit(-2));    // index = -2, id set, unique and persistent
const MYON_HatchPattern MYON_HatchPattern::Hatch2(Internal_LineHatchPatternInit(-3));    // index = -3, id set, unique and persistent
const MYON_HatchPattern MYON_HatchPattern::Hatch3(Internal_LineHatchPatternInit(-4));    // index = -4, id set, unique and persistent
const MYON_HatchPattern MYON_HatchPattern::HatchDash(Internal_LineHatchPatternInit(-5)); // index = -5, id set, unique and persistent
const MYON_HatchPattern MYON_HatchPattern::Grid(Internal_LineHatchPatternInit(-6));      // index = -6, id set, unique and persistent
const MYON_HatchPattern MYON_HatchPattern::Grid60(Internal_LineHatchPatternInit(-7));    // index = -7, id set, unique and persistent
const MYON_HatchPattern MYON_HatchPattern::Plus(Internal_LineHatchPatternInit(-8));      // index = -8, id set, unique and persistent
const MYON_HatchPattern MYON_HatchPattern::Squares(Internal_LineHatchPatternInit(-9));   // index = -9, id set, unique and persistent


const MYON_Mesh MYON_Mesh::Empty;
const MYON_MeshRef MYON_MeshRef::Empty;

unsigned int MYON_SubD::ErrorCount = 0;
unsigned int MYON_Brep::ErrorCount = 0;

const bool MYON_SubD::AutomaticRhino5BoxModeTSplineToSubDDefault = true;
const bool MYON_SubD::AutomaticFBXMeshWithDivisionLevelsToSubDDefault = false;

bool MYON_SubD::AutomaticRhino5BoxModeTSplineToSubD = MYON_SubD::AutomaticRhino5BoxModeTSplineToSubDDefault;
bool MYON_SubD::AutomaticFBXMeshWithDivisionLevelsToSubD = MYON_SubD::AutomaticFBXMeshWithDivisionLevelsToSubDDefault;


void MYON_SubD::AutomaticMeshToSubDRestoreDefaults(
  MYON_SubD::AutomaticMeshToSubDContext context
  )
{
  switch (context)
  {
  case MYON_SubD::AutomaticMeshToSubDContext::Unset:
  default:
    MYON_SubD::AutomaticRhino5BoxModeTSplineToSubD = MYON_SubD::AutomaticRhino5BoxModeTSplineToSubDDefault;
    MYON_SubD::AutomaticFBXMeshWithDivisionLevelsToSubD = MYON_SubD::AutomaticFBXMeshWithDivisionLevelsToSubDDefault;
    break;
  case MYON_SubD::AutomaticMeshToSubDContext::Rhino5BoxModeTSpline:
    MYON_SubD::AutomaticRhino5BoxModeTSplineToSubD = MYON_SubD::AutomaticRhino5BoxModeTSplineToSubDDefault;
    break;
  case MYON_SubD::AutomaticMeshToSubDContext::FBXMeshWithDivisionLevels:
    MYON_SubD::AutomaticFBXMeshWithDivisionLevelsToSubD = MYON_SubD::AutomaticFBXMeshWithDivisionLevelsToSubDDefault;
    break;
  }
}

bool MYON_SubD::AutomaticMeshToSubD(
  MYON_SubD::AutomaticMeshToSubDContext context
)
{
  bool bAutomaticallyCreateSubD;
  switch (context)
  {
  case MYON_SubD::AutomaticMeshToSubDContext::Unset:
    bAutomaticallyCreateSubD = false;
    break;
  case MYON_SubD::AutomaticMeshToSubDContext::Rhino5BoxModeTSpline:
    bAutomaticallyCreateSubD = MYON_SubD::AutomaticRhino5BoxModeTSplineToSubD;
    break;
  case MYON_SubD::AutomaticMeshToSubDContext::FBXMeshWithDivisionLevels:
    bAutomaticallyCreateSubD = MYON_SubD::AutomaticFBXMeshWithDivisionLevelsToSubD;
    break;
  default:
    bAutomaticallyCreateSubD = false;
    break;
  }
  return bAutomaticallyCreateSubD;
}

void MYON_SubD::SetAutomaticMeshToSubD(
  MYON_SubD::AutomaticMeshToSubDContext context,
  bool bAutomaticallyCreateSubD
)
{
  // remove possiblity of hacks to use this as a char value
  bAutomaticallyCreateSubD = bAutomaticallyCreateSubD ? true : false;

  switch (context)
  {
  case MYON_SubD::AutomaticMeshToSubDContext::Unset:
    break;
  case MYON_SubD::AutomaticMeshToSubDContext::Rhino5BoxModeTSpline:
    MYON_SubD::AutomaticRhino5BoxModeTSplineToSubD = bAutomaticallyCreateSubD;
    break;
  case MYON_SubD::AutomaticMeshToSubDContext::FBXMeshWithDivisionLevels:
    MYON_SubD::AutomaticFBXMeshWithDivisionLevelsToSubD = bAutomaticallyCreateSubD;
    break;
  default:
    break;
  }
}


const MYON_SubDVertexEdgeProperties MYON_SubDVertexEdgeProperties::Zero;

const MYON_SubDVertexPtr MYON_SubDVertexPtr::Null = { 0 };
const MYON_SubDEdgePtr MYON_SubDEdgePtr::Null = { 0 };
const MYON_SubDFacePtr MYON_SubDFacePtr::Null = { 0 };
const MYON_SubDComponentPtr MYON_SubDComponentPtr::Null = { 0 };
const MYON_SubDComponentPtrPair MYON_SubDComponentPtrPair::Null = MYON_SubDComponentPtrPair::Create(MYON_SubDComponentPtr::Null,MYON_SubDComponentPtr::Null);

const MYON_SubDComponentAndNumber MYON_SubDComponentAndNumber::NullAndNan = MYON_SubDComponentAndNumber::Create(MYON_SubDComponentPtr::Null, MYON_DBL_QNAN);
const MYON_SubDComponentAndNumber MYON_SubDComponentAndNumber::NullAndZero = MYON_SubDComponentAndNumber::Create(MYON_SubDComponentPtr::Null, 0.0);

const MYON_SubDComponentAndPoint MYON_SubDComponentAndPoint::NullAndNan = MYON_SubDComponentAndPoint::Create(MYON_SubDComponentPtr::Null, MYON_3dPoint::NanPoint);
const MYON_SubDComponentAndPoint MYON_SubDComponentAndPoint::NullAndOrigin = MYON_SubDComponentAndPoint::Create(MYON_SubDComponentPtr::Null, MYON_3dPoint::Origin);

const MYON_SubDComponentList MYON_SubDComponentList::Empty;
const MYON_SubD_ComponentIdTypeAndTag MYON_SubD_ComponentIdTypeAndTag::Unset;

const MYON_SubDComponentId MYON_SubDComponentIdUnset(MYON_SubDComponentPtr::Type::Unset, 0U);

// Passes() returns true for every non nullptr component.
const MYON_SubDComponentTest MYON_SubDComponentTest::AllPass((MYON__UINT_PTR)1);
const MYON_SubDComponentTest MYON_SubDComponentTest::AllFail((MYON__UINT_PTR)0);


const MYON_SubDEdgeChain MYON_SubDEdgeChain::Empty;

static MYON_SubDSectorSurfacePoint MYON_SubDSectorLimitPoint_Init(double x)
{
  MYON_SubDSectorSurfacePoint lp;
  memset(&lp, 0, sizeof(lp));

  lp.m_limitP[0] = x;
  lp.m_limitP[1] = x;
  lp.m_limitP[2] = x;

  lp.m_limitT1[0] = x;
  lp.m_limitT1[1] = x;
  lp.m_limitT1[2] = x;

  lp.m_limitT2[0] = x;
  lp.m_limitT2[1] = x;
  lp.m_limitT2[2] = x;

  lp.m_limitN[0] = x;
  lp.m_limitN[1] = x;
  lp.m_limitN[2] = x;

  return lp;
}

const MYON_SubDSectorSurfacePoint MYON_SubDSectorSurfacePoint::Unset = MYON_SubDSectorLimitPoint_Init(MYON_UNSET_VALUE);
const MYON_SubDSectorSurfacePoint MYON_SubDSectorSurfacePoint::Nan = MYON_SubDSectorLimitPoint_Init(MYON_DBL_QNAN);
const MYON_SubDSectorSurfacePoint MYON_SubDSectorSurfacePoint::Zero = MYON_SubDSectorLimitPoint_Init(0.0);

const MYON_SubDVertexSurfacePointCoefficient MYON_SubDVertexSurfacePointCoefficient::Zero = MYON_SubDVertexSurfacePointCoefficient::Create(nullptr,nullptr,0.0);
const MYON_SubDVertexSurfacePointCoefficient MYON_SubDVertexSurfacePointCoefficient::Nan = MYON_SubDVertexSurfacePointCoefficient::Create(nullptr,nullptr,MYON_DBL_QNAN);
const MYON_SubDVertexSurfacePointCoefficient MYON_SubDVertexSurfacePointCoefficient::Unset = MYON_SubDVertexSurfacePointCoefficient::Create(nullptr,nullptr,MYON_UNSET_VALUE);

const double MYON_SubDSectorType::IgnoredCornerSectorAngle = 0.0;
const double MYON_SubDSectorType::UnsetCornerSectorAngle = -8881.0;
const double MYON_SubDSectorType::ErrorCornerSectorAngle = -9991.0;

const double MYON_SubDSectorType::SmoothSectorTheta = 0.5*MYON_PI;
const double MYON_SubDSectorType::UnsetSectorTheta = -8882.0;
const double MYON_SubDSectorType::ErrorSectorTheta = -9992.0;

const double MYON_SubDSectorType::IgnoredSectorCoefficient = 0.0;
const double MYON_SubDSectorType::UnsetSectorCoefficient = -8883.0;
const double MYON_SubDSectorType::ErrorSectorCoefficient = -9993.0;


const MYON_SubDComponentRegionIndex MYON_SubDComponentRegionIndex::Zero;

static MYON_SubDComponentRegionIndex InternalON_SubDComponentRegionIndex_Unset()
{
  MYON_SubDComponentRegionIndex unset;
  memset(&unset, 0xFF, sizeof(unset));
  return unset;
}
const MYON_SubDComponentRegionIndex MYON_SubDComponentRegionIndex::Unset = InternalON_SubDComponentRegionIndex_Unset();

const MYON_SubDComponentRegion MYON_SubDComponentRegion::Empty;
const MYON_SubDFaceRegion MYON_SubDFaceRegion::Empty;

const MYON_ComponentStatus MYON_ComponentStatus::NoneSet = MYON_ComponentStatus(MYON_ComponentState::Clear);
const MYON_ComponentStatus MYON_ComponentStatus::Selected = MYON_ComponentStatus(MYON_ComponentState::Selected);
const MYON_ComponentStatus MYON_ComponentStatus::SelectedPersistent = MYON_ComponentStatus(MYON_ComponentState::SelectedPersistent);
const MYON_ComponentStatus MYON_ComponentStatus::Highlighted = MYON_ComponentStatus(MYON_ComponentState::Highlighted);
const MYON_ComponentStatus MYON_ComponentStatus::Hidden = MYON_ComponentStatus(MYON_ComponentState::Hidden);
const MYON_ComponentStatus MYON_ComponentStatus::Locked = MYON_ComponentStatus(MYON_ComponentState::Locked);
const MYON_ComponentStatus MYON_ComponentStatus::Deleted = MYON_ComponentStatus(MYON_ComponentState::Deleted);
const MYON_ComponentStatus MYON_ComponentStatus::Damaged = MYON_ComponentStatus(MYON_ComponentState::Damaged);
const MYON_ComponentStatus MYON_ComponentStatus::Marked = MYON_ComponentStatus(MYON_ComponentState::RuntimeMarkSet);
static MYON_ComponentStatus MYON_ComponentStatus_AllSet()
{
  MYON_ComponentStatus s;
  s.SetStates(MYON_ComponentStatus::SelectedPersistent);
  s.SetStates(MYON_ComponentStatus::Highlighted);
  s.SetStates(MYON_ComponentStatus::Hidden);
  s.SetStates(MYON_ComponentStatus::Locked);
  s.SetStates(MYON_ComponentStatus::Damaged);
  return s;
}
const MYON_ComponentStatus MYON_ComponentStatus::AllSet = MYON_ComponentStatus_AllSet();

static MYON_AggregateComponentStatus MYON_Internal_AggregateComponentStatus_Init(int k)
{
  MYON_AggregateComponentStatus s;
  memset(&s, 0, sizeof(s));
  if (1 == k)
    s.MarkAsNotCurrent();
  return s;
}
const MYON_AggregateComponentStatus MYON_AggregateComponentStatus::Empty = MYON_Internal_AggregateComponentStatus_Init(0);
const MYON_AggregateComponentStatus MYON_AggregateComponentStatus::NotCurrent = MYON_Internal_AggregateComponentStatus_Init(1);


static MYON_AggregateComponentStatusEx MYON_Internal_AggregateComponentStatusEx_Init(int k)
{
  MYON_AggregateComponentStatusEx s(MYON_Internal_AggregateComponentStatus_Init(k));
  *((MYON__UINT64*)(((char*)(&s)) + sizeof(MYON_AggregateComponentStatus))) = 0; // m_component_status_serial_number = 0 
  return s;
}
const MYON_AggregateComponentStatusEx MYON_AggregateComponentStatusEx::Empty = MYON_Internal_AggregateComponentStatusEx_Init(0);
const MYON_AggregateComponentStatusEx MYON_AggregateComponentStatusEx::NotCurrent = MYON_Internal_AggregateComponentStatusEx_Init(1);



const MYON_SubDComponentPoint MYON_SubDComponentPoint::Unset = MYON_SubDComponentPoint();

static MYON_SubDMeshFragmentGrid EmptyLimitMeshFragmentGridInit()
{
  MYON_SubDMeshFragmentGrid empty;
  memset(&empty, 0, sizeof(empty));
  return empty;
}

static MYON_SubDMeshFragment EmptyLimitMeshFragmentInit()
{
  MYON_SubDMeshFragment empty;
  memset(&empty, 0, sizeof(empty));
  return empty;
}

const MYON_SubDMeshFragmentGrid MYON_SubDMeshFragmentGrid::Empty = EmptyLimitMeshFragmentGridInit();
const MYON_SubDMeshFragment MYON_SubDMeshFragment::Empty = EmptyLimitMeshFragmentInit();



const MYON_SubDMeshFragmentGrid MYON_SubDMeshFragmentGrid::OneQuadGrid = MYON_SubDMeshFragmentGrid::QuadGridFromSideSegmentCount(1, 0);

static MYON_SubDComponentBase UnsetComponentBaseInit()
{
  // For efficiency, MYON_SubDComponentBase() does not waste time
  // m_cache_subd_P[], m_displacementV[]
  // but the offical "Unset" MYON_SubDComponentBase should have every byte initialized.
  MYON_SubDComponentBase unset;
  memset(&unset, 0, sizeof(unset));
  return unset;
}

static MYON_SubDVertex EmptyVertexInit()
{
  // For efficiency, MYON_SubDVertex() does not waste time
  // initializing m_limitP[], ..., m_cache_subd_P[], m_displacementV[]
  // but the offical "Empty" vertex should have every byte initialized.
  MYON_SubDVertex empty;
  memset(&empty, 0, sizeof(empty));
  return empty;
}

static MYON_SubDEdge EmptyEdgeInit()
{
  // For efficiency, MYON_SubDEdge() does not waste time
  // initializing m_cache_subd_P[], m_displacementV[]
  // but the offical "Empty" edge should have every byte initialized.
  MYON_SubDEdge empty;
  memset(&empty, 0, sizeof(empty));
  return empty;
}

static MYON_SubDFace EmptyFaceInit()
{
  // For efficiency, MYON_SubDFace() does not waste time
  // initializing m_cache_subd_P[], m_displacementV[]
  // but the offical "Empty" face should have every byte initialized.
  MYON_SubDFace empty;
  memset(&empty, 0, sizeof(empty));
  return empty;
}

const MYON_SubDComponentBase MYON_SubDComponentBase::Unset = UnsetComponentBaseInit();
const MYON_SubDVertex MYON_SubDVertex::Empty = EmptyVertexInit();
const MYON_SubDEdge MYON_SubDEdge::Empty = EmptyEdgeInit();
const MYON_SubDFace MYON_SubDFace::Empty = EmptyFaceInit();

const MYON_SubD MYON_SubD::Empty;
const MYON_SubDRef MYON_SubDRef::Empty;
const MYON_SubDMesh MYON_SubDMesh::Empty;
const MYON_SubDSectorType MYON_SubDSectorType::Empty;
const MYON_SubDMatrix MYON_SubDMatrix::Empty;
const MYON_SubDComponentRef MYON_SubDComponentRef::Empty;

static MYON_SubDFromMeshParameters Internal_InteriorCreases()
{
  MYON_SubDFromMeshParameters cp;
  cp.SetInteriorCreaseOption(MYON_SubDFromMeshParameters::InteriorCreaseOption::AtMeshDoubleEdge);
  return cp;
}

static MYON_SubDFromMeshParameters Internal_ConvexCornersAndInteriorCreases()
{
  MYON_SubDFromMeshParameters cp;
  cp.SetInteriorCreaseOption(MYON_SubDFromMeshParameters::InteriorCreaseOption::AtMeshDoubleEdge);
  cp.SetConvexCornerOption(MYON_SubDFromMeshParameters::ConvexCornerOption::AtMeshCorner);
  return cp;
}

const MYON_SubDFromMeshParameters MYON_SubDFromMeshParameters::Smooth;
const MYON_SubDFromMeshParameters MYON_SubDFromMeshParameters::InteriorCreases = Internal_InteriorCreases();
const MYON_SubDFromMeshParameters MYON_SubDFromMeshParameters::ConvexCornersAndInteriorCreases = Internal_ConvexCornersAndInteriorCreases();

const MYON_SubDFromSurfaceParameters MYON_SubDFromSurfaceParameters::Default;

static const MYON_SubDFromSurfaceParameters Internal_InitSubDFromSurfaceParameters(
  MYON_SubDFromSurfaceParameters::Methods method,
  bool bCorners
  )
{
  MYON_SubDFromSurfaceParameters p;
  if (MYON_SubDFromSurfaceParameters::Methods::Unset != method)
    p.SetMethod(method);
  p.SetCorners(bCorners);
  return p;
}

const MYON_SubDFromSurfaceParameters MYON_SubDFromSurfaceParameters::DefaultWithCorners = Internal_InitSubDFromSurfaceParameters(MYON_SubDFromSurfaceParameters::Methods::Unset, true);
const MYON_SubDFromSurfaceParameters MYON_SubDFromSurfaceParameters::ControlNet = Internal_InitSubDFromSurfaceParameters(MYON_SubDFromSurfaceParameters::Methods::FromNurbsControlNet, false);
const MYON_SubDFromSurfaceParameters MYON_SubDFromSurfaceParameters::ControlNetWithCorners = Internal_InitSubDFromSurfaceParameters(MYON_SubDFromSurfaceParameters::Methods::FromNurbsControlNet, true);


const MYON_SubDComponentFilter MYON_SubDComponentFilter::Unset = MYON_SubDComponentFilter::Create(true, true, true);
const MYON_SubDComponentFilter MYON_SubDComponentFilter::OnlyVertices = MYON_SubDComponentFilter::Create(true, false, false);
const MYON_SubDComponentFilter MYON_SubDComponentFilter::OnlyEdges = MYON_SubDComponentFilter::Create(false, true, false);
const MYON_SubDComponentFilter MYON_SubDComponentFilter::OnlyFaces = MYON_SubDComponentFilter::Create(false, false, true);

unsigned int MYON_ModelComponent::Internal_SystemComponentHelper()
{
  static unsigned int rc = 0;
  if (0 == rc)
  {
    rc = 1;
    const MYON_ModelComponent* list[] =
    {
      &MYON_ModelComponent::Unset,
      &MYON_InstanceDefinition::Unset,
      &MYON_Linetype::Unset,
      &MYON_Layer::Unset,
      &MYON_TextStyle::Unset,
      &MYON_DimStyle::Unset,
      &MYON_Material::Unset,
      &MYON_Material::Default,
      &MYON_Material::DefaultLockedObject,
      &MYON_TextureMapping::Unset,
      &MYON_TextureMapping::SurfaceParameterTextureMapping,
      &MYON_HatchPattern::Unset,
      &MYON_Group::Unset,
      &MYON_HistoryRecord::Empty
    };

    const MYON_ModelComponent* list_localized_name[] =
    {
      &MYON_Linetype::Continuous,
      &MYON_Linetype::ByLayer,
      &MYON_Linetype::ByParent,
      &MYON_Linetype::Hidden,
      &MYON_Linetype::Dashed,
      &MYON_Linetype::DashDot,
      &MYON_Linetype::Center,
      &MYON_Linetype::Border,
      &MYON_Linetype::Dots,

      &MYON_Layer::Default,

      &MYON_TextStyle::Default,
      &MYON_TextStyle::ByLayer,
      &MYON_TextStyle::ByParent,

      &MYON_DimStyle::Default,
      &MYON_DimStyle::DefaultInchDecimal,
      &MYON_DimStyle::DefaultInchFractional,
      &MYON_DimStyle::DefaultFootInchArchitecture,
      &MYON_DimStyle::DefaultMillimeterSmall,
      &MYON_DimStyle::DefaultMillimeterLarge,
      &MYON_DimStyle::DefaultMillimeterArchitecture,
      &MYON_DimStyle::DefaultFeetDecimal,
      &MYON_DimStyle::DefaultModelUnitsDecimal,
      &MYON_DimStyle::DefaultFeetEngrave,
      &MYON_DimStyle::DefaultMillimeterEngrave,
      &MYON_DimStyle::DefaultModelUnitsEngrave,
 
      &MYON_HatchPattern::Solid,
      &MYON_HatchPattern::Hatch1,
      &MYON_HatchPattern::Hatch2,
      &MYON_HatchPattern::Hatch3,
      &MYON_HatchPattern::HatchDash,
      &MYON_HatchPattern::Grid,
      &MYON_HatchPattern::Grid60,
      &MYON_HatchPattern::Plus,
      &MYON_HatchPattern::Squares
    };

    const size_t list_count = sizeof(list) / sizeof(list[0]);
    for (size_t i = 0; i < list_count; i++)
    {
      *(const_cast<unsigned short*>(&list[i]->m_locked_status)) = 0xFFFFU;
      *(const_cast<MYON__UINT64*>(&list[i]->m_content_version_number)) = 0;
    }

    const unsigned int name_bit = MYON_ModelComponent::Attributes::NameAttribute;
    const unsigned int name_mask = ~name_bit;
    const unsigned short bits
      = (unsigned short)((name_mask & MYON_ModelComponent::Attributes::AllAttributes) | MYON_ModelComponent::Attributes::SystemComponentAttribute);
    const size_t list_localized_name_count = sizeof(list_localized_name) / sizeof(list_localized_name[0]);
    for (size_t i = 0; i < list_localized_name_count; i++)
    {
      *(const_cast<unsigned short*>(&list_localized_name[i]->m_locked_status)) = bits;
      *(const_cast<MYON__UINT64*>(&list_localized_name[i]->m_content_version_number)) = 0;
    }

    rc = (unsigned int)(list_count + list_localized_name_count);
  }
  return rc;
}


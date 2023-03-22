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
#include "opennurbs_internal_defines.h"

#if !defined(MYON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// MYON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, MYON_COMPILING_OPENNURBS is defined
// and the opennurbs .h files alter what is declared and how it is declared.
#error MYON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

#define MYON_TYPE_NAME               L"type-name"
#define MYON_TYPE_ID                 L"type-id"
#define MYON_INSTANCE_ID             L"instance-id"
#define MYON_RENDER_ENGINE_ID        L"render-engine-id"
#define MYON_PLUG_IN_ID              L"plug-in-id"
#define MYON_GROUP_ID                L"group-id"
#define MYON_INSTANCE_NAME           L"instance-name"
#define MYON_CHILD_SLOT_NAME         L"child-slot-name"
#define MYON_NOTES                   L"notes"
#define MYON_TAGS                    L"tags"
#define MYON_HIDDEN                  L"hidden"
#define MYON_REFERENCE               L"reference"
#define MYON_AUTO_DELETE             L"auto-delete"
#define MYON_ENV_BACKGROUND_COLOR    L"background-color"
#define MYON_POSTFIX_SECTION         L"-section"
#define MYON_PARAMETERS              L"parameters"
#define MYON_PARAMETERS_V8           L"parameters-v8"
#define MYON_SIMULATION              L"simulation"

#define MYON_POSTFIX_SECTION         L"-section"
#define MYON_PARAMETERS              L"parameters"
#define MYON_PARAMETERS_V8           L"parameters-v8"
#define MYON_SIMULATION              L"simulation"

#define MYON_ENV_PROJ_BOX            L"box"
#define MYON_ENV_PROJ_CUBE_MAP       L"cubemap"
#define MYON_ENV_PROJ_CUBE_MAP_HORZ  L"horizontal-cross-cubemap"
#define MYON_ENV_PROJ_CUBE_MAP_VERT  L"vertical-cross-cubemap"
#define MYON_ENV_PROJ_EMAP           L"emap"
#define MYON_ENV_PROJ_HEMISPHERICAL  L"hemispherical"
#define MYON_ENV_PROJ_LIGHT_PROBE    L"lightprobe"
#define MYON_ENV_PROJ_PLANAR         L"planar"
#define MYON_ENV_PROJ_SPHERICAL      L"spherical"

// General parameters used by materials that simulate MYON_Material.
//
// These are copied from the FS_ strings in the RDK. Perhaps the RDK should start
// using these instead of the FS_ ones. Or the FS_ ones can be defined as these.
// Remember, they are in the RDK SDK. Check with Andy.

#define MYON_MAT_ALPHA_TRANSPARENCY                 L"alpha-transparency"
#define MYON_MAT_AMBIENT                            L"ambient"
#define MYON_MAT_DIFFUSE                            L"diffuse"
#define MYON_MAT_DISABLE_LIGHTING                   L"disable-lighting"
#define MYON_MAT_EMISSION                           L"emission"
#define MYON_MAT_FLAMINGO_LIBRARY                   L"flamingo-library"
#define MYON_MAT_FRESNEL_ENABLED                    L"fresnel-enabled"
#define MYON_MAT_CLARITY_AMOUNT                     L"clarity-amount"
#define MYON_MAT_IOR                                L"ior"
#define MYON_MAT_POLISH_AMOUNT                      L"polish-amount"
#define MYON_MAT_SHINE                              L"shine"      // Value is 0.0..1.0, NOT MYON_Material::MaxShine.
#define MYON_MAT_SPECULAR                           L"specular"
#define MYON_MAT_REFLECTIVITY_AMOUNT                L"reflectivity"
#define MYON_MAT_REFLECTION                         L"reflection" // Simulation is different to Custom Material.
#define MYON_MAT_TRANSPARENCY_AMOUNT                L"transparency"
#define MYON_MAT_TRANSPARENT                        L"transparent" // Simulation is different to Custom Material.
#define MYON_MAT_IS_PHYSICALLY_BASED                L"is-physically-based"

#define MYON_MAT_POSTFIX_ON                         L"on"
#define MYON_MAT_POSTFIX_AMOUNT                     L"amount"
#define MYON_MAT_POSTFIX_FILTER_ON                  L"filter-on"

#define MYON_MAT_BITMAP_TEXTURE                     L"bitmap-texture"
#define MYON_MAT_BUMP_TEXTURE                       L"bump-texture"
#define MYON_MAT_TRANSPARENCY_TEXTURE               L"transparency-texture"
#define MYON_MAT_ENVIRONMENT_TEXTURE                L"environment-texture"

#define MYON_MAT_PBR_BRDF                           L"pbr-brdf"
#define   MYON_MAT_PBR_BRDF_GGX                       L"ggx"
#define   MYON_MAT_PBR_BRDF_WARD                      L"ward"
#define MYON_MAT_PBR_ALPHA                          L"pbr-alpha"
#define MYON_MAT_PBR_ANISOTROPIC                    L"pbr-anisotropic"
#define MYON_MAT_PBR_ANISOTROPIC_ROTATION           L"pbr-anisotropic-rotation"
#define MYON_MAT_PBR_BASE_COLOR                     L"pbr-base-color"
#define MYON_MAT_PBR_CLEARCOAT                      L"pbr-clearcoat"
#define MYON_MAT_PBR_CLEARCOAT_ROUGHNESS            L"pbr-clearcoat-roughness"
#define MYON_MAT_PBR_EMISSION_COLOR                 L"pbr-emission"
#define MYON_MAT_PBR_METALLIC                       L"pbr-metallic"
#define MYON_MAT_PBR_OPACITY                        L"pbr-opacity"
#define MYON_MAT_PBR_OPACITY_IOR                    L"pbr-opacity-ior"
#define MYON_MAT_PBR_OPACITY_ROUGHNESS              L"pbr-opacity-roughness"
#define MYON_MAT_PBR_ROUGHNESS                      L"pbr-roughness"
#define MYON_MAT_PBR_SHEEN                          L"pbr-sheen"
#define MYON_MAT_PBR_SHEEN_TINT                     L"pbr-sheen-tint"
#define MYON_MAT_PBR_SPECULAR                       L"pbr-specular"
#define MYON_MAT_PBR_SPECULAR_TINT                  L"pbr-specular-tint"
#define MYON_MAT_PBR_SUBSURFACE                     L"pbr-subsurface"
#define MYON_MAT_PBR_SUBSURFACE_SCATTERING_COLOR    L"pbr-subsurface-scattering-color"
#define MYON_MAT_PBR_SUBSURFACE_SCATTERING_RADIUS   L"pbr-subsurface-scattering-radius"
#define MYON_MAT_PBR_USE_BASE_COLOR_TEXTURE_ALPHA   L"pbr-use-base-color-texture-alpha"

#define MYON_TEX_FILENAME                    L"filename"

// Material's texture simulation (check with Andy what the heck this is even for).
#define MYON_MAT_TEXSIM_FORMAT               L"Texture-%u-"
#define MYON_MAT_TEXSIM_FILENAME             L"filename"
#define MYON_MAT_TEXSIM_ON                   L"on"
#define MYON_MAT_TEXSIM_AMOUNT               L"amount"
#define MYON_MAT_TEXSIM_TYPE                 L"type"
#define MYON_MAT_TEXSIM_FILTER               L"filter"
#define MYON_MAT_TEXSIM_MODE                 L"mode"
#define MYON_MAT_TEXSIM_UVW                  L"uvw"
#define MYON_MAT_TEXSIM_WRAP_U               L"wrap-u"
#define MYON_MAT_TEXSIM_WRAP_V               L"wrap-v"

// Environment simulation.
#define MYON_ENVSIM_BACKGROUND_COLOR         L"background-color"
#define MYON_ENVSIM_BACKGROUND_IMAGE         L"background-image"
#define MYON_ENVSIM_BACKGROUND_PROJECTION    L"background-projection"

// Texture simulation.
#define MYON_TEXSIM_FILENAME                 L"filename"
#define MYON_TEXSIM_REPEAT                   L"repeat"
#define MYON_TEXSIM_OFFSET                   L"offset"
#define MYON_TEXSIM_ROTATION                 L"rotation"
#define MYON_TEXSIM_WRAP_TYPE                L"wrap-type"
#define MYON_TEXSIM_MAPPING_CHANNEL          L"mapping-channel"
#define MYON_TEXSIM_PROJECTION_MODE          L"projection-mode" // Still have to write this out.

static void MYON_ConstructXform(double scale_x, double scale_y, double scale_z,
                              double angle_x, double angle_y, double angle_z,
                              double trans_x, double trans_y, double trans_z, MYON_Xform& xform)
{
  // All angles in degrees.

  const MYON_Xform S = MYON_Xform::DiagonalTransformation(scale_x, scale_y, scale_z);

  MYON_Xform R;
  R.Rotation(MYON_RadiansFromDegrees(angle_x), MYON_3dVector::XAxis, MYON_3dPoint::Origin);

  auto vRotate = MYON_3dVector::YAxis;
  vRotate.Transform(R.Inverse());

  MYON_Xform Ry;
  Ry.Rotation(MYON_RadiansFromDegrees(angle_y), vRotate, MYON_3dPoint::Origin);

  R = R * Ry;

  vRotate = MYON_3dVector::ZAxis;
  vRotate.Transform(R.Inverse());

  MYON_Xform Rz;
  Rz.Rotation(MYON_RadiansFromDegrees(angle_z), vRotate, MYON_3dPoint::Origin);

  R = R * Rz;

  const auto T = MYON_Xform::TranslationTransformation(-trans_x, -trans_y, -trans_z);

  xform = S * R * T;
}

static void MYON_DeconstructXform(const MYON_Xform& xformIn,
                                double& scale_x, double& scale_y, double& scale_z,
                                double& angle_x, double& angle_y, double& angle_z,
                                double& trans_x, double& trans_y, double& trans_z)
{
  // Returns all angles in degrees.

  MYON_Xform xform = xformIn;

  scale_x = sqrt(xform[0][0] * xform[0][0] + xform[0][1] * xform[0][1] + xform[0][2] * xform[0][2]);
  scale_y = sqrt(xform[1][0] * xform[1][0] + xform[1][1] * xform[1][1] + xform[1][2] * xform[1][2]);
  scale_z = sqrt(xform[2][0] * xform[2][0] + xform[2][1] * xform[2][1] + xform[2][2] * xform[2][2]);

  MYON_Xform S;
  MYON_ConstructXform(scale_x, scale_y, scale_z, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, S);
  S.Invert();

  xform = S * xform;

  const double dSinBeta = -xform[2][0];

  double dCosBeta = sqrt(1.0 - dSinBeta * dSinBeta);
  if (dCosBeta < 0.0)
  {
    dCosBeta = -dCosBeta;
  }

  double dSinAlpha, dCosAlpha, dSinGamma, dCosGamma;

  if (dCosBeta < 1e-6)
  {
    dSinAlpha = -xform[1][2];
    dCosAlpha = xform[1][1];
    dSinGamma = 0.0;
    dCosGamma = 1.0;
  }
  else
  {
    dSinAlpha = xform[2][1] / dCosBeta;
    dCosAlpha = xform[2][2] / dCosBeta;
    dSinGamma = xform[1][0] / dCosBeta;
    dCosGamma = xform[0][0] / dCosBeta;
  }

  angle_x = (MYON_DegreesFromRadians(atan2(dSinAlpha, dCosAlpha)));
  angle_y = (MYON_DegreesFromRadians(atan2(dSinBeta, dCosBeta)));
  angle_z = (MYON_DegreesFromRadians(atan2(dSinGamma, dCosGamma)));

  MYON_Xform R;
  MYON_ConstructXform(scale_x, scale_y, scale_z, angle_x, angle_y, angle_z, 0.0, 0.0, 0.0, R);
  R.Invert();

  MYON_Xform T = R * xformIn;

  trans_x = -T[0][3];
  trans_y = -T[1][3];
  trans_z = -T[2][3];
}

struct XF
{
  double scale_x = 0.0, scale_y = 0.0, scale_z = 0.0;
  double angle_x = 0.0, angle_y = 0.0, angle_z = 0.0;
  double trans_x = 0.0, trans_y = 0.0, trans_z = 0.0;
};

static void MYON_DeconstructXform(const MYON_Xform& xform, XF& xf)
{
  MYON_DeconstructXform(xform, xf.scale_x, xf.scale_y, xf.scale_z, xf.angle_x, xf.angle_y, xf.angle_z, xf.trans_x, xf.trans_y, xf.trans_z);
}

static void MYON_ConstructXform(const XF& xf, MYON_Xform& xform)
{
  MYON_ConstructXform(xf.scale_x, xf.scale_y, xf.scale_z, xf.angle_x, xf.angle_y, xf.angle_z, xf.trans_x, xf.trans_y, xf.trans_z, xform);
}

MYON_OBJECT_IMPLEMENT(MYON_Environment, MYON_Object, "94BCA4D5-0FC7-435E-95F9-22F3927F9B2E");

class MYON_Environment::CImpl
{
public:
  CImpl();
  CImpl(const CImpl& src);

public:
  MYON_Color m_back_col;
  MYON_Texture m_back_image;
  BackgroundProjections m_back_proj = BackgroundProjections::Planar;
};

MYON_Environment::CImpl::CImpl()
  :
  m_back_col(MYON_Color(160, 160, 160))
{
}

MYON_Environment::CImpl::CImpl(const CImpl& src)
{
  m_back_col   = src.m_back_col;
  m_back_proj  = src.m_back_proj;
  m_back_image = src.m_back_image;
}

MYON_Environment::MYON_Environment()
{
  m_impl = new CImpl;
}

MYON_Environment::MYON_Environment(const MYON_Environment& src)
{
  m_impl = new CImpl(*src.m_impl);
}

MYON_Environment::~MYON_Environment()
{
  delete m_impl;
  m_impl = nullptr;
}

static MYON__UINT32 CRCOnlyRGB(MYON__UINT32 current_remainder, const MYON_Color& c)
{
  const auto x = (unsigned int)c & 0x00FFFFFF;
  return MYON_CRC32(current_remainder, sizeof(x), &x);
}

MYON__UINT32 MYON_Texture_CRC(const MYON_Texture& tex)
{
  MYON_wString file(tex.m_image_file_reference.FullPath());
#ifdef MYON_RUNTIME_WIN
  file.MakeLower();
#endif
  MYON__UINT32 crc = file.DataCRC(0);

  crc = MYON_CRC32(crc, sizeof(tex.m_mapping_channel_id), &tex.m_mapping_channel_id);

  crc = MYON_CRC32(crc, sizeof(tex.m_bOn),       &tex.m_bOn);
  crc = MYON_CRC32(crc, sizeof(tex.m_type),      &tex.m_type);
  crc = MYON_CRC32(crc, sizeof(tex.m_minfilter), &tex.m_minfilter);
  crc = MYON_CRC32(crc, sizeof(tex.m_magfilter), &tex.m_magfilter);
  crc = MYON_CRC32(crc, sizeof(tex.m_wrapu),     &tex.m_wrapu);
  crc = MYON_CRC32(crc, sizeof(tex.m_wrapv),     &tex.m_wrapv);
  crc = MYON_CRC32(crc, sizeof(tex.m_wrapw),     &tex.m_wrapw);

  crc = CRCOnlyRGB(crc, tex.m_border_color);
  crc = CRCOnlyRGB(crc, tex.m_transparent_color);
  crc = CRCOnlyRGB(crc, tex.m_blend_constant_RGB);

  crc = MYON_CRC32(crc, sizeof(tex.m_blend_order), &tex.m_blend_order);

  const double amount = int(tex.m_blend_constant_A * 100.0) / 100.0;
  crc = MYON_CRC32(crc, sizeof(amount), &amount);

  crc = MYON_CRC32(crc, sizeof(tex.m_bump_scale), &tex.m_bump_scale);
  crc = MYON_CRC32(crc, sizeof(tex.m_uvw),        &tex.m_uvw);

  crc = MYON_CRC32(crc, sizeof(tex.m_blend_A0),   &tex.m_blend_A0);
  crc = MYON_CRC32(crc, sizeof(tex.m_blend_RGB0), &tex.m_blend_RGB0);
  crc = MYON_CRC32(crc, sizeof(tex.m_blend_A1),   &tex.m_blend_A1);
  crc = MYON_CRC32(crc, sizeof(tex.m_blend_RGB1), &tex.m_blend_RGB1);
  crc = MYON_CRC32(crc, sizeof(tex.m_blend_A2),   &tex.m_blend_A2);
  crc = MYON_CRC32(crc, sizeof(tex.m_blend_RGB2), &tex.m_blend_RGB2);
  crc = MYON_CRC32(crc, sizeof(tex.m_blend_A3),   &tex.m_blend_A3);
  crc = MYON_CRC32(crc, sizeof(tex.m_blend_RGB3), &tex.m_blend_RGB3);

  crc = MYON_CRC32(crc, sizeof(tex.m_bTreatAsLinear), &tex.m_bTreatAsLinear);

	return crc;
}

bool MYON_Environment::operator == (const MYON_Environment& env) const
{
  if (m_impl->m_back_col < env.m_impl->m_back_col)
    return false;

  if (m_impl->m_back_col > env.m_impl->m_back_col)
    return false;

  if (m_impl->m_back_proj != env.m_impl->m_back_proj)
    return false;

  if (MYON_Texture_CRC(m_impl->m_back_image) != MYON_Texture_CRC(env.m_impl->m_back_image))
    return false;

  return true;
}

bool MYON_Environment::operator != (const MYON_Environment& env) const
{
  return !(operator == (env));
}

const MYON_Environment& MYON_Environment::operator = (const MYON_Environment& src)
{
  if (this != &src)
  {
    delete m_impl;
    m_impl = new CImpl(*src.m_impl);
  }

  return *this;
}

MYON_Color MYON_Environment::BackgroundColor(void) const
{
  return m_impl->m_back_col;
}

void MYON_Environment::SetBackgroundColor(MYON_Color color)
{
  m_impl->m_back_col = color;
}

MYON_Texture MYON_Environment::BackgroundImage(void) const
{
  return m_impl->m_back_image;
}

void MYON_Environment::SetBackgroundImage(const MYON_Texture& tex)
{
  m_impl->m_back_image = tex;
}

MYON_Environment::BackgroundProjections MYON_Environment::BackgroundProjection(void) const
{
  return m_impl->m_back_proj;
}

void MYON_Environment::SetBackgroundProjection(MYON_Environment::BackgroundProjections proj)
{
  m_impl->m_back_proj = proj;
}

MYON_Environment::BackgroundProjections MYON_Environment::ProjectionFromString(const wchar_t* wsz) // Static.
{
  if (0 == on_wcsicmp(MYON_ENV_PROJ_PLANAR, wsz))        return BackgroundProjections::Planar;
  if (0 == on_wcsicmp(MYON_ENV_PROJ_SPHERICAL, wsz))     return BackgroundProjections::Spherical;
  if (0 == on_wcsicmp(MYON_ENV_PROJ_EMAP, wsz))          return BackgroundProjections::Emap;
  if (0 == on_wcsicmp(MYON_ENV_PROJ_BOX, wsz))           return BackgroundProjections::Box;
  if (0 == on_wcsicmp(MYON_ENV_PROJ_LIGHT_PROBE, wsz))   return BackgroundProjections::LightProbe;
  if (0 == on_wcsicmp(MYON_ENV_PROJ_CUBE_MAP, wsz))      return BackgroundProjections::CubeMap;
  if (0 == on_wcsicmp(MYON_ENV_PROJ_CUBE_MAP_VERT, wsz)) return BackgroundProjections::VerticalCrossCubeMap;
  if (0 == on_wcsicmp(MYON_ENV_PROJ_CUBE_MAP_HORZ, wsz)) return BackgroundProjections::HorizontalCrossCubeMap;
  if (0 == on_wcsicmp(MYON_ENV_PROJ_HEMISPHERICAL, wsz)) return BackgroundProjections::Hemispherical;

  MYON_ASSERT(false);
  return BackgroundProjections::Planar;
}

const wchar_t* MYON_Environment::StringFromProjection(BackgroundProjections proj) // Static.
{
  switch (proj)
  {
  case BackgroundProjections::Planar:                 return MYON_ENV_PROJ_PLANAR;
  case BackgroundProjections::Spherical:              return MYON_ENV_PROJ_SPHERICAL;
  case BackgroundProjections::Emap:                   return MYON_ENV_PROJ_EMAP;
  case BackgroundProjections::Box:                    return MYON_ENV_PROJ_BOX;
  case BackgroundProjections::LightProbe:             return MYON_ENV_PROJ_LIGHT_PROBE;
  case BackgroundProjections::CubeMap:                return MYON_ENV_PROJ_CUBE_MAP;
  case BackgroundProjections::VerticalCrossCubeMap:   return MYON_ENV_PROJ_CUBE_MAP_VERT;
  case BackgroundProjections::HorizontalCrossCubeMap: return MYON_ENV_PROJ_CUBE_MAP_HORZ;
  case BackgroundProjections::Hemispherical:          return MYON_ENV_PROJ_HEMISPHERICAL;
  default: break;
  }

  MYON_ASSERT(false);
  return MYON_ENV_PROJ_PLANAR;
}

void* MYON_Environment::EVF(const wchar_t* wszFunc, void* pvData)
{
  return nullptr;
}

MYON_UUID RdkPlugInId(void)
{
  static MYON_UUID uuid = { 0x16592D58, 0x4A2F, 0x401D, { 0xBF, 0x5E, 0x3B, 0x87, 0x74, 0x1C, 0x1B, 0x1B } };
  return uuid;
}

MYON_UUID UniversalRenderEngineId(void)
{
  static MYON_UUID uuid = { 0x99999999, 0x9999, 0x9999, { 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99 } };
  return uuid;
}

class MYON_RenderContent::CImpl
{
public:
  CImpl(MYON_RenderContent& rc, const wchar_t* kind);
  virtual ~CImpl();

  void SetXMLNode(const MYON_XMLNode& node);

  const MYON_XMLNode* XMLNode_Simulation(void) const;

  MYON_RenderContent& TopLevel(void);

  bool AddChild(MYON_RenderContent& rc);
  void DeleteAllChildren(void);

  MYON_XMLVariant GetPropertyValue(const wchar_t* name) const;
  void SetPropertyValue(const wchar_t* name, const MYON_XMLVariant& value);

  MYON_RenderContent* FindChild(const wchar_t* child_slot_name) const;
  bool SetChild(MYON_RenderContent* child, const wchar_t* child_slot_name);
  bool ChangeChild(MYON_RenderContent* old_child, MYON_RenderContent* new_child);

public:
  void InternalSetPropertyValue(const wchar_t* name, const MYON_XMLVariant& value);

private:
  MYON_RenderContent* FindLastChild(void) const;
  MYON_RenderContent* FindPrevSibling(MYON_RenderContent* child) const;

public:
  MYONX_Model* m_model = nullptr;
  MYON_XMLNode m_node;
  MYON_RenderContent& m_render_content;
  MYON_RenderContent* m_parent = nullptr;
  MYON_RenderContent* m_first_child = nullptr;
  MYON_RenderContent* m_next_sibling = nullptr;
  mutable std::recursive_mutex m_mutex;
};

MYON_RenderContent::CImpl::CImpl(MYON_RenderContent& rc, const wchar_t* kind)
  :
  m_node(kind),
  m_render_content(rc)
{
}

MYON_RenderContent::CImpl::~CImpl()
{
  DeleteAllChildren();
}

const MYON_XMLNode* MYON_RenderContent::CImpl::XMLNode_Simulation(void) const
{
  return m_node.GetNamedChild(MYON_SIMULATION);
}

MYON_XMLVariant MYON_RenderContent::CImpl::GetPropertyValue(const wchar_t* name) const
{
  // Gets a property from the content node. This is one of:
  //
  // - the material node <material... >
  // - the environment node <environment... >
  // - the texture node <texture... >

  std::lock_guard<std::recursive_mutex> lg(m_mutex);

  MYON_XMLVariant v;

  const MYON_XMLProperty* pProp = m_node.GetNamedProperty(name);
  if (nullptr != pProp)
  {
    v = pProp->GetValue();
  }

  return v;
}

void MYON_RenderContent::CImpl::SetPropertyValue(const wchar_t* name, const MYON_XMLVariant& value)
{
  std::lock_guard<std::recursive_mutex> lg(m_mutex);

  InternalSetPropertyValue(name, value);
}

void MYON_RenderContent::CImpl::InternalSetPropertyValue(const wchar_t* name, const MYON_XMLVariant& value)
{
  // Sets a property on the content node. This is one of:
  //
  // - the material node <material... >
  // - the environment node <environment... >
  // - the texture node <texture... >

  MYON_XMLProperty* pProp = m_node.GetNamedProperty(name);
  if (nullptr != pProp)
  {
    pProp->SetValue(value);
  }
  else
  {
    pProp = m_node.AttachProperty(new MYON_XMLProperty(name, value));
  }
}

void MYON_RenderContent::CImpl::SetXMLNode(const MYON_XMLNode& node)
{
  std::lock_guard<std::recursive_mutex> lg(m_mutex);

  // Copy the incoming XML node. The render content will only store a copy of its own XML, so
  // we will have to prune this copy as we find children and create render content children
  // for all the XML children.
  MYON_XMLNode node_copy = node;

  // Iterate over the child nodes of the XML node being set to this content.
  auto it = node_copy.GetChildIterator();
  MYON_XMLNode* child_node = nullptr;
  while (nullptr != (child_node = it.GetNextChild()))
  {
    // See if the child node is a content node.
    const MYON_wString& name = child_node->TagName();
    if ((MYON_KIND_MATERIAL == name) || (MYON_KIND_ENVIRONMENT == name) || (MYON_KIND_TEXTURE == name))
    {
      // Yes, so we are going to add a new render content to this hierarchy (recursively)
      // and remove this child node from the copy of the XML node.
      MYON_RenderContent* child_rc = NewRenderContentFromNode(*child_node);
      if (nullptr != child_rc)
      {
        // Add the new content as a child of this content.
        AddChild(*child_rc);
      }

      delete node_copy.DetachChild(*child_node);
    }
  }

  // Copy the pruned copy of the XML node. This node does not have any child content nodes.
  m_node = node_copy;

  // Copy the XML instance name to the component name.
  m_render_content.SetName(GetPropertyValue(MYON_INSTANCE_NAME).AsString());

  // Copy the XML instance id to the component id.
  m_render_content.SetId(GetPropertyValue(MYON_INSTANCE_ID).AsUuid());
}

bool MYON_RenderContent::CImpl::AddChild(MYON_RenderContent& child)
{
  if ((nullptr != child.m_impl->m_model) || (nullptr != child.m_impl->m_parent) || (nullptr != child.m_impl->m_next_sibling))
    return false;

  if (nullptr == m_first_child)
  {
    m_first_child = &child;
  }
  else
  {
    MYON_RenderContent* last_child = FindLastChild();
    if (nullptr == last_child)
      return false;

    last_child->m_impl->m_next_sibling = &child;
  }

  child.m_impl->m_next_sibling = nullptr;
  child.m_impl->m_parent = &m_render_content;

  return true;
}

void MYON_RenderContent::CImpl::DeleteAllChildren(void)
{
  std::lock_guard<std::recursive_mutex> lg(m_mutex);

  if (nullptr == m_first_child)
    return;

  MYON_RenderContent* child_rc = m_first_child;
  while (nullptr != child_rc)
  {
    auto* delete_rc = child_rc;
    child_rc = child_rc->m_impl->m_next_sibling;
    delete delete_rc;
  }

  m_first_child = nullptr;
}

MYON_RenderContent* MYON_RenderContent::CImpl::FindLastChild(void) const
{
  MYON_RenderContent* result = nullptr;

  MYON_RenderContent* candidate = m_first_child;
  while (nullptr != candidate)
  {
    result = candidate;
    candidate = candidate->m_impl->m_next_sibling;
  }

  return result;
}

MYON_RenderContent* MYON_RenderContent::CImpl::FindPrevSibling(MYON_RenderContent* child) const
{
  if (child != m_first_child)
  {
    MYON_RenderContent* candidate = m_first_child;
    while (nullptr != candidate)
    {
      if (child == candidate->m_impl->m_next_sibling)
        return candidate;

      candidate = candidate->m_impl->m_next_sibling;
    }
  }

  return nullptr;
}

MYON_RenderContent& MYON_RenderContent::CImpl::TopLevel(void)
{
  if (nullptr != m_parent)
  {
    return m_parent->m_impl->TopLevel();
  }

  return m_render_content;
}

bool MYON_RenderContent::CImpl::ChangeChild(MYON_RenderContent* old_child, MYON_RenderContent* new_child)
{
  if (nullptr == old_child)
    return false;

  if (old_child == m_first_child)
  {
    if (nullptr != new_child)
    {
      m_first_child = new_child;
    }
    else
    {
      m_first_child = old_child->m_impl->m_next_sibling;
    }
  }
  else
  {
    const MYON_RenderContent* prev_sibling = FindPrevSibling(old_child);
    if (nullptr == prev_sibling)
      return false;

    if (nullptr != new_child)
    {
      prev_sibling->m_impl->m_next_sibling = new_child;
    }
    else
    {
      prev_sibling->m_impl->m_next_sibling = old_child->m_impl->m_next_sibling;
    }
  }

  if (nullptr != new_child)
  {
    new_child->m_impl->m_next_sibling = old_child->m_impl->m_next_sibling;
    new_child->m_impl->m_parent = old_child->m_impl->m_parent;
  }

  delete old_child;

  return true;
}

MYON_RenderContent* MYON_RenderContent::CImpl::FindChild(const wchar_t* child_slot_name) const
{
  std::lock_guard<std::recursive_mutex> lg(m_mutex);

  MYON_RenderContent* child_rc = m_first_child;
  while (nullptr != child_rc)
  {
    if (child_rc->ChildSlotName() == child_slot_name)
      return child_rc;

    child_rc = child_rc->m_impl->m_next_sibling;
  }

  return nullptr;
}

bool MYON_RenderContent::CImpl::SetChild(MYON_RenderContent* child, const wchar_t* child_slot_name)
{
  std::lock_guard<std::recursive_mutex> lg(m_mutex);

  if (nullptr != child)
  {
    if (nullptr != child->m_impl->m_model)
      return false;

    if (nullptr != child->m_impl->m_parent)
      return false;

    if ((nullptr == child_slot_name) || (0 == child_slot_name[0]))
      return false;

    child->m_impl->SetPropertyValue(MYON_CHILD_SLOT_NAME, child_slot_name);
  }

  // Get any existing child with the same child slot name (may be null).
  auto* existing_child = FindChild(child_slot_name);
  if (nullptr != existing_child)
  {
    // There is an existing child with the same child slot name; replace it.
    if (!ChangeChild(existing_child, child)) // Deletes existing_child.
      return false;
  }
  else
  {
    // No existing child; just add the new one.
    if (nullptr != child)
    {
      if (!AddChild(*child))
        return false;
    }
  }

  if (nullptr != child)
  {
    auto* pModel = TopLevel().m_impl->m_model;
    child->m_impl->m_model = pModel;
  }

  return true;
}

void SetRenderContentNodeRecursive(const MYON_RenderContent& rc, MYON_XMLNode& node)
{
  // Copy the component name to the XML instance name.
  rc.m_impl->SetPropertyValue(MYON_INSTANCE_NAME, rc.Name());

  // Copy the component id to the XML instance id.
  rc.m_impl->SetPropertyValue(MYON_INSTANCE_ID, rc.Id());

  auto* child_node = new MYON_XMLNode(rc.XMLNode());
  node.AttachChildNode(child_node);

  auto it = rc.GetChildIterator();
  MYON_RenderContent* child_rc;
  while (nullptr != (child_rc = it.GetNextChild()))
  {
    SetRenderContentNodeRecursive(*child_rc, *child_node);
  }
}

static void BuildXMLHierarchy(const MYON_RenderContent& rc, MYON_XMLNode& node)
{
  // Recursively builds 'node' from the tree structure of the XML nodes in 'rc' and its children.

  node = rc.m_impl->m_node;

  auto* child_rc = rc.m_impl->m_first_child;
  while (nullptr != child_rc)
  {
    auto* child_node = new MYON_XMLNode(L"");
    BuildXMLHierarchy(*child_rc, *child_node);
    node.AttachChildNode(child_node);

    child_rc = child_rc->m_impl->m_next_sibling;
  }
}

MYON_VIRTUAL_OBJECT_IMPLEMENT(MYON_RenderContent, MYON_ModelComponent, "A98DEDDA-E4FA-4E1E-9BD3-2A0695C6D4E9");

MYON_RenderContent::MYON_RenderContent(const wchar_t* kind)
  :
  MYON_ModelComponent(MYON_ModelComponent::Type::RenderContent)
{
  m_impl = new (m_Impl) CImpl(*this, kind);

  // Set a unique instance id.
  MYON_UUID uuid;
  MYON_CreateUuid(uuid);
  SetId(uuid);

  // Set the plug-in id to the RDK plug-in id.
  m_impl->InternalSetPropertyValue(MYON_PLUG_IN_ID, RdkPlugInId());

  // Set the render engine id to 'universal'.
  m_impl->InternalSetPropertyValue(MYON_RENDER_ENGINE_ID, UniversalRenderEngineId());
}

MYON_RenderContent::MYON_RenderContent(const MYON_RenderContent& rc)
  :
  MYON_ModelComponent(MYON_ModelComponent::Type::RenderContent, rc)
{
  m_impl = new (m_Impl) CImpl(*this, L"");
  operator = (rc);
}

MYON_RenderContent::~MYON_RenderContent()
{
  m_impl->~CImpl();
  m_impl = nullptr;
}

const MYON_RenderContent& MYON_RenderContent::operator = (const MYON_RenderContent& rc)
{
  if (this != &rc)
  {
    MYON_XMLRootNode root;
    BuildXMLHierarchy(rc, root);
    m_impl->SetXMLNode(root);
  }

  return *this;
}

MYON_wString MYON_RenderContent::TypeName(void) const
{
  return m_impl->GetPropertyValue(MYON_TYPE_NAME).AsString();
}

void MYON_RenderContent::SetTypeName(const wchar_t* name)
{
  m_impl->SetPropertyValue(MYON_TYPE_NAME, name);
}

MYON_wString MYON_RenderContent::Notes(void) const
{
  return m_impl->GetPropertyValue(MYON_NOTES).AsString();
}

void MYON_RenderContent::SetNotes(const wchar_t* notes)
{
  m_impl->SetPropertyValue(MYON_NOTES, notes);
}

MYON_wString MYON_RenderContent::Tags(void) const
{
  return m_impl->GetPropertyValue(MYON_TAGS).AsString();
}

void MYON_RenderContent::SetTags(const wchar_t* tags)
{
  m_impl->SetPropertyValue(MYON_TAGS, tags);
}

MYON_UUID MYON_RenderContent::TypeId(void) const
{
  return m_impl->GetPropertyValue(MYON_TYPE_ID).AsUuid();
}

void MYON_RenderContent::SetTypeId(const MYON_UUID& uuid)
{
  m_impl->SetPropertyValue(MYON_TYPE_ID, uuid);
}

MYON_UUID MYON_RenderContent::RenderEngineId(void) const
{
  return m_impl->GetPropertyValue(MYON_RENDER_ENGINE_ID).AsUuid();
}

void MYON_RenderContent::SetRenderEngineId(const MYON_UUID& uuid)
{
  m_impl->SetPropertyValue(MYON_RENDER_ENGINE_ID, uuid);
}

MYON_UUID MYON_RenderContent::PlugInId(void) const
{
  return m_impl->GetPropertyValue(MYON_PLUG_IN_ID).AsUuid();
}

void MYON_RenderContent::SetPlugInId(const MYON_UUID& uuid)
{
  m_impl->SetPropertyValue(MYON_PLUG_IN_ID, uuid);
}

MYON_UUID MYON_RenderContent::GroupId(void) const
{
  return m_impl->GetPropertyValue(MYON_GROUP_ID).AsUuid();
}

void MYON_RenderContent::SetGroupId(const MYON_UUID& group)
{
  m_impl->SetPropertyValue(MYON_GROUP_ID, group);
}

bool MYON_RenderContent::Hidden(void) const
{
  return m_impl->GetPropertyValue(MYON_HIDDEN).AsBool();
}

void MYON_RenderContent::SetHidden(bool hidden)
{
  m_impl->SetPropertyValue(MYON_HIDDEN, hidden);
}

bool MYON_RenderContent::Reference(void) const
{
  return m_impl->GetPropertyValue(MYON_REFERENCE).AsBool();
}

void MYON_RenderContent::SetReference(bool ref)
{
  m_impl->SetPropertyValue(MYON_REFERENCE, ref);
}

bool MYON_RenderContent::AutoDelete(void) const
{
  return m_impl->GetPropertyValue(MYON_AUTO_DELETE).AsBool();
}

void MYON_RenderContent::SetAutoDelete(bool autodel)
{
  m_impl->SetPropertyValue(MYON_AUTO_DELETE, autodel);
}

MYON_XMLVariant MYON_RenderContent::GetParameter(const wchar_t* name) const
{
  std::lock_guard<std::recursive_mutex> lg(m_impl->m_mutex);

  MYON_XMLVariant value;
  value.SetNull();

  // Try to get the new V8 parameter section.
  const MYON_XMLNode* node = m_impl->m_node.GetNamedChild(MYON_PARAMETERS_V8);
  if (nullptr != node)
  {
    // Got it, so use the new MYON_XMLParametersV8 to get the parameter's value.
    MYON_XMLParametersV8 p(*node);
    p.GetParam(name, value);
  }
  else
  {
    // Either no V8 section was found or the parameter isn't there yet.
    // Try to get the legacy parameter section. This should not fail.
    node = m_impl->m_node.GetNamedChild(MYON_PARAMETERS);
    if (nullptr != node)
    {
      // Got it, so use the legacy MYON_XMLParameters to get the parameter's value.
      MYON_XMLParameters p(*node);
      p.GetParam(name, value);
    }
  }

  return value;
}

bool MYON_RenderContent::SetParameter(const wchar_t* name, const MYON_XMLVariant& value)
{
  std::lock_guard<std::recursive_mutex> lg(m_impl->m_mutex);

  bool success = false;

  // Create / get the new V8 parameter section.
  auto* node = m_impl->m_node.CreateNodeAtPath(MYON_PARAMETERS_V8);
  if (nullptr != node)
  {
    // Use the new MYON_XMLParametersV8 to write the parameter's value.
    MYON_XMLParametersV8 p(*node);
    if (nullptr != p.SetParam(name, value))
      success = true;
  }

  // Create / get the legacy parameter section.
  node = m_impl->m_node.CreateNodeAtPath(MYON_PARAMETERS);
  if (nullptr != node)
  {
    // Use the legacy MYON_XMLParameters to write the parameter's value.
    MYON_XMLParameters p(*node);
    if (nullptr != p.SetParam(name, value))
      success = true;
  }

  return success;
}

MYON_RenderContent::ChildIterator MYON_RenderContent::GetChildIterator(void) const
{
  return ChildIterator(this);
}

const MYON_RenderContent* MYON_RenderContent::Parent(void) const
{
  return m_impl->m_parent;
}

const MYON_RenderContent* MYON_RenderContent::FirstChild(void) const
{
  return m_impl->m_first_child;
}

const MYON_RenderContent* MYON_RenderContent::NextSibling(void) const
{
  return m_impl->m_next_sibling;
}

const MYON_RenderContent& MYON_RenderContent::TopLevel(void) const
{
  return m_impl->TopLevel();
}

bool MYON_RenderContent::IsTopLevel(void) const
{
	return nullptr == m_impl->m_parent;
}

bool MYON_RenderContent::IsChild(void) const
{
	return nullptr != m_impl->m_parent;
}

bool MYON_RenderContent::SetChild(const MYON_RenderContent& child, const wchar_t* child_slot_name)
{
  return m_impl->SetChild(child.Duplicate(), child_slot_name);
}

bool MYON_RenderContent::DeleteChild(const wchar_t* child_slot_name)
{
  return m_impl->SetChild(nullptr, child_slot_name);
}

MYON_wString MYON_RenderContent::ChildSlotName(void) const
{
  return m_impl->GetPropertyValue(MYON_CHILD_SLOT_NAME).AsString();
}

void MYON_RenderContent::SetChildSlotName(const wchar_t* csn)
{
  m_impl->SetPropertyValue(MYON_CHILD_SLOT_NAME, csn);
}

bool MYON_RenderContent::ChildSlotOn(const wchar_t* child_slot_name) const
{
  const auto s = MYON_wString(child_slot_name) + L"-" MYON_MAT_POSTFIX_ON;
  return GetParameter(s).AsBool();
}

bool MYON_RenderContent::SetChildSlotOn(bool on, const wchar_t* child_slot_name)
{
  const auto s = MYON_wString(child_slot_name) + L"-" MYON_MAT_POSTFIX_ON;
  return SetParameter(s, on);
}

const MYON_RenderContent* MYON_RenderContent::FindChild(const wchar_t* child_slot_name) const
{
  return m_impl->FindChild(child_slot_name);
}

static MYON_XMLNode* NewXMLNodeRecursive(const MYON_RenderContent& rc)
{
  MYON_XMLNode* node = new MYON_XMLNode(rc.m_impl->m_node);

  MYON_RenderContent* child_rc = rc.m_impl->m_first_child;
  while (nullptr != child_rc)
  {
    MYON_XMLNode* child_node = NewXMLNodeRecursive(*child_rc);
    if (nullptr != child_node)
    {
      node->AttachChildNode(child_node);
    }

    child_rc = child_rc->m_impl->m_next_sibling;
  }

  return node;
}

MYON_wString MYON_RenderContent::XML(bool recursive) const
{
  MYON_XMLNode* node = &m_impl->m_node;

  if (recursive)
  {
    node = NewXMLNodeRecursive(*this);
  }

  const MYON__UINT32 logical_count = node->WriteToStream(nullptr, 0);
  auto* stream = new wchar_t[size_t(logical_count) + 1];
  node->WriteToStream(stream, logical_count);
  stream[logical_count] = 0;
  const MYON_wString xml = stream;
  delete[] stream;

  if (recursive)
    delete node;

  return xml;
}

bool MYON_RenderContent::SetXML(const wchar_t* xml)
{
  MYON_XMLRootNode node;
  if (MYON_XMLNode::ReadError == node.ReadFromStream(xml))
    return false;

  m_impl->SetXMLNode(node);

  return true;
}

const MYON_XMLNode& MYON_RenderContent::XMLNode(void) const
{
  return m_impl->m_node;
}

MYON_wString MYON_RenderContent::Kind(void) const
{
  return m_impl->m_node.TagName();
}

const MYON_RenderContent* MYON_RenderContent::FromModelComponentRef(const MYON_ModelComponentReference& ref,
                                                                const MYON_RenderContent* none_return_value) // Static.
{
  const auto* rc = MYON_RenderContent::Cast(ref.ModelComponent());
  if (nullptr != rc)
    return rc;

  return none_return_value;
}

void* MYON_RenderContent::EVF(const wchar_t* func, void* data)
{
  return nullptr;
}

// MYON_RenderContent::ChildIterator

class MYON_RenderContent::ChildIterator::CImpl final
{
public:
  MYON_RenderContent* m_current = nullptr;
};

MYON_RenderContent::ChildIterator::ChildIterator(const MYON_RenderContent* parent_rc)
{
  m_impl = new CImpl;

  if (nullptr != parent_rc)
  {
    m_impl->m_current = parent_rc->m_impl->m_first_child;
  }
}

MYON_RenderContent::ChildIterator::~ChildIterator()
{
  delete m_impl;
  m_impl = nullptr;
}

MYON_RenderContent* MYON_RenderContent::ChildIterator::GetNextChild(void)
{
  MYON_RenderContent* rc = m_impl->m_current;
  if (nullptr != rc)
  {
    m_impl->m_current = rc->m_impl->m_next_sibling;
  }

  return rc;
}

void* MYON_RenderContent::ChildIterator::EVF(const wchar_t*, void*)
{
  return nullptr;
}

void SetModel(const MYON_RenderContent& rc, MYONX_Model& model)
{
  rc.m_impl->m_model = &model;

  auto it = rc.GetChildIterator();
  MYON_RenderContent* child_rc = nullptr;
  while (nullptr != (child_rc = it.GetNextChild()))
  {
    SetModel(*child_rc, model);
  }
}

MYON_RenderContent* NewRenderContentFromNode(const MYON_XMLNode& node)
{
  MYON_RenderContent* rc = nullptr;

  const MYON_wString& kind = node.TagName();

  if (MYON_KIND_MATERIAL == kind)
    rc = new MYON_RenderMaterial;
  else
  if (MYON_KIND_ENVIRONMENT == kind)
    rc = new MYON_RenderEnvironment;
  else
  if (MYON_KIND_TEXTURE == kind)
    rc = new MYON_RenderTexture;

  if (nullptr != rc)
  {
    rc->m_impl->SetXMLNode(node);
  }

  return rc;
}

// MYON_RenderMaterial

MYON_OBJECT_IMPLEMENT(MYON_RenderMaterial, MYON_RenderContent, "493E6601-F95B-4A5D-BB6F-2F6633076907");

MYON_RenderMaterial::MYON_RenderMaterial()
  :
  MYON_RenderContent(MYON_KIND_MATERIAL)
{
}

MYON_RenderMaterial::MYON_RenderMaterial(const MYON_RenderMaterial& mat)
  :
  MYON_RenderContent(mat)
{
}

MYON_RenderMaterial::~MYON_RenderMaterial()
{
}

void MYON_RenderMaterial::operator = (const MYON_RenderMaterial& mat)
{
  MYON_RenderContent::operator = (mat);
}

MYON_XMLVariant ParamHelper(const MYON_XMLParameters& p, const wchar_t* name)
{
  MYON_XMLVariant value;
  if (!p.GetParam(name, value))
    value = L"";

  return value;
}

MYON_Material MYON_RenderMaterial::SimulatedMaterial(void) const
{
  std::lock_guard<std::recursive_mutex> lg(m_impl->m_mutex);

  MYON_Material mat;

  const MYON_XMLNode* sim_node = m_impl->XMLNode_Simulation();
  if (nullptr != sim_node)
  {
    MYON_XMLParameters p(*sim_node);

    mat.m_ambient               =       ParamHelper(p, MYON_MAT_AMBIENT            ).AsColor();
    mat.m_diffuse               =       ParamHelper(p, MYON_MAT_DIFFUSE            ).AsColor();
    mat.m_emission              =       ParamHelper(p, MYON_MAT_EMISSION           ).AsColor();
    mat.m_specular              =       ParamHelper(p, MYON_MAT_SPECULAR           ).AsColor();
    mat.m_reflection            =       ParamHelper(p, MYON_MAT_REFLECTION         ).AsColor();
    mat.m_reflectivity          =       ParamHelper(p, MYON_MAT_REFLECTIVITY_AMOUNT).AsDouble();
    mat.m_shine                 =       ParamHelper(p, MYON_MAT_SHINE              ).AsDouble() * MYON_Material::MaxShine;
    mat.m_transparency          =       ParamHelper(p, MYON_MAT_TRANSPARENCY_AMOUNT).AsDouble();
    mat.m_index_of_refraction   =       ParamHelper(p, MYON_MAT_IOR                ).AsDouble();
    mat.m_reflection_glossiness = 1.0 - ParamHelper(p, MYON_MAT_POLISH_AMOUNT      ).AsDouble();
    mat.m_refraction_glossiness = 1.0 - ParamHelper(p, MYON_MAT_CLARITY_AMOUNT     ).AsDouble();
    mat.m_transparent           =       ParamHelper(p, MYON_MAT_TRANSPARENT        ).AsColor();
    mat.SetFresnelReflections   (       ParamHelper(p, MYON_MAT_FRESNEL_ENABLED    ).AsBool());
    mat.SetDisableLighting      (       ParamHelper(p, MYON_MAT_DISABLE_LIGHTING   ).AsBool());

    mat.m_fresnel_index_of_refraction = 1.56;

    if (ParamHelper(p, MYON_MAT_IS_PHYSICALLY_BASED).AsBool())
    {
      mat.ToPhysicallyBased();

      auto pbm = mat.PhysicallyBased();

      auto brdf = MYON_PhysicallyBasedMaterial::BRDFs::GGX;
      const MYON_wString s = ParamHelper(p, MYON_MAT_PBR_BRDF).AsString();
      if (s == MYON_MAT_PBR_BRDF_WARD)
        brdf = MYON_PhysicallyBasedMaterial::BRDFs::Ward;
      pbm->SetBRDF(brdf);

      pbm->SetBaseColor                 (ParamHelper(p, MYON_MAT_PBR_BASE_COLOR).AsColor());
      pbm->SetSubsurface                (ParamHelper(p, MYON_MAT_PBR_SUBSURFACE).AsDouble());
      pbm->SetSubsurfaceScatteringColor (ParamHelper(p, MYON_MAT_PBR_SUBSURFACE_SCATTERING_COLOR).AsColor());
      pbm->SetSubsurfaceScatteringRadius(ParamHelper(p, MYON_MAT_PBR_SUBSURFACE_SCATTERING_RADIUS).AsDouble());
      pbm->SetSpecular                  (ParamHelper(p, MYON_MAT_PBR_SPECULAR).AsDouble());
      pbm->SetSpecularTint              (ParamHelper(p, MYON_MAT_PBR_SPECULAR_TINT).AsDouble());
      pbm->SetMetallic                  (ParamHelper(p, MYON_MAT_PBR_METALLIC).AsDouble());
      pbm->SetRoughness                 (ParamHelper(p, MYON_MAT_PBR_ROUGHNESS).AsDouble());
      pbm->SetAnisotropic               (ParamHelper(p, MYON_MAT_PBR_ANISOTROPIC).AsDouble());
      pbm->SetAnisotropicRotation       (ParamHelper(p, MYON_MAT_PBR_ANISOTROPIC_ROTATION).AsDouble());
      pbm->SetSheen                     (ParamHelper(p, MYON_MAT_PBR_SHEEN).AsDouble());
      pbm->SetSheenTint                 (ParamHelper(p, MYON_MAT_PBR_SHEEN_TINT).AsDouble());
      pbm->SetClearcoat                 (ParamHelper(p, MYON_MAT_PBR_CLEARCOAT).AsDouble());
      pbm->SetClearcoatRoughness        (ParamHelper(p, MYON_MAT_PBR_CLEARCOAT_ROUGHNESS).AsDouble());
      pbm->SetOpacity                   (ParamHelper(p, MYON_MAT_PBR_OPACITY).AsDouble());
      pbm->SetOpacityIOR                (ParamHelper(p, MYON_MAT_PBR_OPACITY_IOR).AsDouble());
      pbm->SetOpacityRoughness          (ParamHelper(p, MYON_MAT_PBR_OPACITY_ROUGHNESS).AsDouble());
      pbm->SetEmission                  (ParamHelper(p, MYON_MAT_PBR_EMISSION_COLOR).AsColor());
      pbm->SetAlpha                     (ParamHelper(p, MYON_MAT_PBR_ALPHA).AsDouble());
      pbm->SetUseBaseColorTextureAlphaForObjectAlphaTransparencyTexture(ParamHelper(p, MYON_MAT_PBR_USE_BASE_COLOR_TEXTURE_ALPHA).AsBool());
    }

    mat.SetName(Name());

    mat.m_textures.Destroy();

    // Iterator over the children.
    int count = 1;
    while (true)
    {
      MYON_Texture tex;
      MYON_XMLVariant v;

      MYON_wString s;
      s.Format(MYON_MAT_TEXSIM_FORMAT, count);

      if (!p.GetParam(s + MYON_MAT_TEXSIM_FILENAME, v))
        break; // Not ideal.

      tex.m_image_file_reference.SetFullPath(v.AsString(), false);

      if (p.GetParam(s + MYON_MAT_TEXSIM_ON, v))
        tex.m_bOn = v;

      if (p.GetParam(s + MYON_MAT_TEXSIM_AMOUNT, v))
        tex.m_blend_constant_A = v;

      if (p.GetParam(s + MYON_MAT_TEXSIM_TYPE, v))
        tex.m_type = MYON_Texture::TYPE(v.AsInteger());

      if (p.GetParam(s + MYON_MAT_TEXSIM_FILTER, v))
        tex.m_minfilter = tex.m_magfilter = MYON_Texture::FILTER(v.AsInteger());

      if (p.GetParam(s + MYON_MAT_TEXSIM_MODE, v))
        tex.m_mode = MYON_Texture::MODE(v.AsInteger());

      if (p.GetParam(s + MYON_MAT_TEXSIM_UVW, v))
        tex.m_uvw = v.AsXform();

      if (p.GetParam(s + MYON_MAT_TEXSIM_WRAP_U, v))
        tex.m_wrapu = MYON_Texture::WRAP(v.AsInteger());

      if (p.GetParam(s + MYON_MAT_TEXSIM_WRAP_V, v))
        tex.m_wrapv = MYON_Texture::WRAP(v.AsInteger());

      mat.m_textures.Append(tex);
      count++;
    }
  }

  return mat;
}

// MYON_RenderEnvironment

MYON_OBJECT_IMPLEMENT(MYON_RenderEnvironment, MYON_RenderContent, "A0AB8EF9-5FD4-4320-BBDA-A1200D1846E4");

MYON_RenderEnvironment::MYON_RenderEnvironment()
  :
  MYON_RenderContent(MYON_KIND_ENVIRONMENT)
{
}

MYON_RenderEnvironment::MYON_RenderEnvironment(const MYON_RenderEnvironment& env)
  :
  MYON_RenderContent(env)
{
}

MYON_RenderEnvironment::~MYON_RenderEnvironment()
{
}

void MYON_RenderEnvironment::operator = (const MYON_RenderEnvironment& env)
{
}

MYON_Environment MYON_RenderEnvironment::SimulatedEnvironment(void) const
{
  std::lock_guard<std::recursive_mutex> lg(m_impl->m_mutex);

  MYON_Environment env;

  const MYON_XMLNode* sim_node = m_impl->XMLNode_Simulation();
  if (nullptr != sim_node)
  {
    MYON_XMLVariant v;
    MYON_XMLParameters p(*sim_node);

    if (p.GetParam(MYON_ENVSIM_BACKGROUND_COLOR, v))
      env.SetBackgroundColor(v.AsColor());

    if (p.GetParam(MYON_ENVSIM_BACKGROUND_IMAGE, v))
    {
      MYON_Texture tex;
      tex.m_image_file_reference.SetFullPath(v.AsString(), false);
      // TODO: What other MYON_Texture params need to be set?
      env.SetBackgroundImage(tex);
    }

    if (p.GetParam(MYON_ENVSIM_BACKGROUND_PROJECTION, v))
    {
      const auto proj = MYON_Environment::ProjectionFromString(v.AsString());
      env.SetBackgroundProjection(proj);
    }
  }

  return env;
}

// MYON_RenderTexture

MYON_OBJECT_IMPLEMENT(MYON_RenderTexture, MYON_RenderContent, "677D9905-CC8C-41E6-A7AD-2409DDE68ED0");

MYON_RenderTexture::MYON_RenderTexture()
  :
  MYON_RenderContent(MYON_KIND_TEXTURE)
{
}

MYON_RenderTexture::MYON_RenderTexture(const MYON_RenderTexture& tex)
  :
  MYON_RenderContent(tex)
{
}

MYON_RenderTexture::~MYON_RenderTexture()
{
}

void MYON_RenderTexture::operator = (const MYON_RenderTexture& tex)
{
}

MYON_Texture MYON_RenderTexture::SimulatedTexture(void) const
{
  std::lock_guard<std::recursive_mutex> lg(m_impl->m_mutex);

  MYON_Texture tex;

  const MYON_XMLNode* sim_node = m_impl->XMLNode_Simulation();
  if (nullptr != sim_node)
  {
    MYON_XMLVariant v;
    MYON_XMLParameters p(*sim_node);

    if (p.GetParam(MYON_TEXSIM_FILENAME, v))
    {
      tex.m_image_file_reference.SetFullPath(v.AsString(), false);
    }

    if (p.GetParam(MYON_TEXSIM_OFFSET, v))
    {
      XF xf;
      MYON_DeconstructXform(tex.m_uvw, xf);
      const auto pt = v.As2dPoint();
      xf.trans_x = pt[0];
      xf.trans_y = pt[1];
      MYON_ConstructXform(xf, tex.m_uvw);
    }

    if (p.GetParam(MYON_TEXSIM_REPEAT, v))
    {
      XF xf;
      MYON_DeconstructXform(tex.m_uvw, xf);
      const auto pt = v.As2dPoint();
      xf.scale_x = pt[0];
      xf.scale_y = pt[1];
      MYON_ConstructXform(xf, tex.m_uvw);
    }

    if (p.GetParam(MYON_TEXSIM_ROTATION, v))
    {
      XF xf;
      MYON_DeconstructXform(tex.m_uvw, xf);
      xf.angle_z = v.AsDouble();
      MYON_ConstructXform(xf, tex.m_uvw);
    }

    if (p.GetParam(MYON_TEXSIM_WRAP_TYPE, v))
    {
      const auto wt = MYON_Texture::WRAP(v.AsInteger());
      tex.m_wrapu = wt;
      tex.m_wrapv = wt;
      tex.m_wrapw = wt;
    }

    if (p.GetParam(MYON_TEXSIM_MAPPING_CHANNEL, v))
    {
      tex.m_mapping_channel_id = v.AsInteger();
    }
  }

  return tex;
}

int MYONX_Model::AddRenderMaterial(const wchar_t* mat_name)
{
  static MYON_UUID uuidPB = { 0x5a8d7b9b, 0xcdc9, 0x49de, { 0x8c, 0x16, 0x2e, 0xf6, 0x4f, 0xb0, 0x97, 0xab } };

  MYON_RenderMaterial mat;
  mat.SetTypeId(uuidPB);

  const MYON_wString unused_name = m_manifest.UnusedName(mat.ComponentType(), MYON_nil_uuid, mat_name, nullptr, nullptr, 0, nullptr);
  mat.SetName(unused_name);

  const MYON_ModelComponentReference mcr = AddModelComponent(mat, true);
  const auto* model_mat = MYON_RenderMaterial::Cast(mcr.ModelComponent());
  if (nullptr == model_mat)
  {
    MYON_ERROR("Failed to add render material; AddModelComponent() failed");
    return MYON_UNSET_INT_INDEX;
  }

  return model_mat->Index();
}

int MYONX_Model::AddRenderEnvironment(const wchar_t* env_name)
{
  static MYON_UUID uuidBE = { 0xba51ce00, 0xba51, 0xce00, { 0xba, 0x51, 0xce, 0xba, 0x51, 0xce, 0x00, 0x00 } };

  MYON_RenderEnvironment env;
  env.SetTypeId(uuidBE);

  const MYON_wString unused_name = m_manifest.UnusedName(env.ComponentType(), MYON_nil_uuid, env_name, nullptr, nullptr, 0, nullptr);
  env.SetName(unused_name);

  const MYON_ModelComponentReference mcr = AddModelComponent(env, true);
  const auto* model_env = MYON_RenderEnvironment::Cast(mcr.ModelComponent());
  if (nullptr == model_env)
  {
    MYON_ERROR("Failed to add render environment; AddModelComponent() failed");
    return MYON_UNSET_INT_INDEX;
  }

  return model_env->Index();
}

int MYONX_Model::AddRenderTexture(const wchar_t* fn)
{
  static const MYON_UUID uuidBM = { 0x57e0ed08, 0x1907, 0x4529, { 0xb0, 0x1b, 0x0c, 0x4a, 0x24, 0x24, 0x55, 0xfd } };

  const auto filename = MYON_FileSystemPath::CleanPath(fn);

  if (!MYON_FileSystem::PathExists(filename))
  {
    MYON_ERROR("Failed to add render texture; file does not exist");
    return MYON_UNSET_INT_INDEX;
  }

  MYON_RenderTexture tex;
  tex.SetTypeId(uuidBM);
  tex.SetParameter(MYON_TEX_FILENAME, filename);

  const MYON_wString tex_name = MYON_FileSystemPath::FileNameFromPath(filename, false);
  tex.SetName(tex_name);

  const MYON_wString unused_name = m_manifest.UnusedName(tex.ComponentType(), MYON_nil_uuid, tex_name, nullptr, nullptr, 0, nullptr);
  tex.SetName(unused_name);

  const MYON_ModelComponentReference mcr = AddModelComponent(tex, true);
  const auto* model_tex = MYON_RenderTexture::Cast(mcr.ModelComponent());
  if (nullptr == model_tex)
  {
    MYON_ERROR("Failed to add render texture; AddModelComponent() failed");

    return MYON_UNSET_INT_INDEX;
  }

  return model_tex->Index();
}

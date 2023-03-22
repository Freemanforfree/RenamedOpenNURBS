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

  #define MYON_DECALS                         L"decals"
    #define MYON_DECAL                          L"decal"
      #define MYON_DECAL_CUSTOM                   L"custom"
      #define MYON_DECAL_CUSTOM_RENDERER          L"renderer"
      #define MYON_DECAL_CUSTOM_PARAMS              L"parameters"

      #define MYON_DECAL_MAPPING                  L"mapping"
      //----- +++++++++++++++++++
      #define MYON_DECAL_MAPPING_PLANAR           L"planar"
      #define MYON_DECAL_MAPPING_CYLINDRICAL      L"cylindrical"
      #define MYON_DECAL_MAPPING_SPHERICAL        L"spherical"
      #define MYON_DECAL_MAPPING_UV               L"uv"
      #define MYON_DECAL_MAPPING_NONE             L"none"

      #define MYON_DECAL_PROJECTION               L"projection"
      //----- ++++++++++++++++++++++
      #define MYON_DECAL_PROJECTION_FORWARD       L"forward"
      #define MYON_DECAL_PROJECTION_BACKWARD      L"backward"
      #define MYON_DECAL_PROJECTION_BOTH          L"both"
      #define MYON_DECAL_PROJECTION_NONE          L"none"

      #define MYON_DECAL_MAP_TO_INSIDE_ON         L"map-to-inside-on"
      #define MYON_DECAL_TRANSPARENCY             L"transparency"
      #define MYON_DECAL_TEXTURE_INSTANCE         L"texture-instance"
      #define MYON_DECAL_HEIGHT                   L"height"
      #define MYON_DECAL_ORIGIN                   L"origin"
      #define MYON_DECAL_RADIUS                   L"radius"
      #define MYON_DECAL_HORZ_SWEEP_STA           L"latitude-start"
      #define MYON_DECAL_HORZ_SWEEP_END           L"latitude-stop"
      #define MYON_DECAL_VERT_SWEEP_STA           L"longitude-start"
      #define MYON_DECAL_VERT_SWEEP_END           L"longitude-stop"
      #define MYON_DECAL_VECTOR_UP                L"vector-up"
      #define MYON_DECAL_VECTOR_ACROSS            L"vector-across"
      #define MYON_DECAL_MIN_U                    L"min-u"
      #define MYON_DECAL_MIN_V                    L"min-v"
      #define MYON_DECAL_MAX_U                    L"max-u"
      #define MYON_DECAL_MAX_V                    L"max-v"
      #define MYON_DECAL_IS_TEMPORARY             L"is-temporary"
      #define MYON_DECAL_IS_VISIBLE               L"is-visible"
      #define MYON_DECAL_INSTANCE_ID              L"instance-id"

// MYON_Decal

static MYON_4dPoint UNSET_4D_POINT = MYON_4dPoint(MYON_UNSET_VALUE, MYON_UNSET_VALUE, MYON_UNSET_VALUE, MYON_UNSET_VALUE);

class MYON_Decal::CImpl : public MYON_InternalXMLImpl
{
public:
  CImpl() { MYON_CreateUuid(_decal_id); }
  CImpl(MYON_DecalCollection& dc, MYON_XMLNode& node) : _collection(&dc), MYON_InternalXMLImpl(&node) { MYON_CreateUuid(_decal_id); }

  MYON_UUID TextureInstanceId(void) const;
  void SetTextureInstanceId(const MYON_UUID& id);
  Mappings Mapping(void) const;
  void SetMapping(Mappings v);
  Projections Projection(void) const;
  void SetProjection(Projections v);
  bool MapToInside(void) const;
  void SetMapToInside(bool v);
  double Transparency(void) const;
  void SetTransparency(double v);
  MYON_3dPoint Origin(void) const;
  void SetOrigin(const MYON_3dPoint& v);
  MYON_3dVector VectorUp(void) const;
  void SetVectorUp(const MYON_3dVector& v);
  MYON_3dVector VectorAcross(void) const;
  void SetVectorAcross(const MYON_3dVector& v);
  double Height(void) const;
  void SetHeight(double d);
  double Radius(void) const;
  void SetRadius(double d);
  bool IsVisible(void) const;
  void SetIsVisible(bool b);
  void GetHorzSweep(double& sta, double& end) const;
  void SetHorzSweep(double sta, double end);
  void GetVertSweep(double& sta, double& end) const;
  void SetVertSweep(double sta, double end);
  void GetUVBounds(double& min_u, double& min_v, double& max_u, double& max_v) const;
  void SetUVBounds(double min_u, double min_v, double max_u, double max_v);

  MYON_UUID Id(void) const { return _decal_id; }
  MYON_XMLVariant GetParameter(const wchar_t* param_name, const MYON_XMLVariant& def) const;
  void SetParameter(const wchar_t* param_name, const MYON_XMLVariant& value);
  MYON_XMLNode* FindCustomNodeForRenderEngine(const MYON_UUID& renderEngineId) const;
  virtual MYON_wString NameOfRootNode(void) const override { return MYON_DECAL; }

  static const int unset_bool = 2;
  mutable struct Cache
  {
    double radius       = MYON_UNSET_VALUE;
    double height       = MYON_UNSET_VALUE;
    double transparency = MYON_UNSET_VALUE;
    MYON_UUID texture_instance_id = MYON_nil_uuid;
    MYON_3dPoint origin = MYON_3dPoint::UnsetPoint;
    MYON_3dVector vector_up = MYON_3dVector::UnsetVector;
    MYON_3dVector vector_across = MYON_3dVector::UnsetVector;
    MYON_2dPoint horz_sweep  = MYON_2dPoint::UnsetPoint;
    MYON_2dPoint vert_sweep  = MYON_2dPoint::UnsetPoint;
    MYON_4dPoint uv_bounds   = UNSET_4D_POINT;
    Mappings mapping = Mappings::None;
    Projections projection = Projections::None;
    bool texture_instance_id_set = false;
    int visible = unset_bool;
    int map_to_inside = unset_bool;
  }
  _cache;

private:
  MYON_DecalCollection* _collection = nullptr;
  MYON_UUID _decal_id;
};

static MYON_Decal::Mappings MappingFromString(const MYON_wString& s)
{
       if (s == MYON_DECAL_MAPPING_PLANAR)      return MYON_Decal::Mappings::Planar;
  else if (s == MYON_DECAL_MAPPING_SPHERICAL)   return MYON_Decal::Mappings::Spherical;
  else if (s == MYON_DECAL_MAPPING_CYLINDRICAL) return MYON_Decal::Mappings::Cylindrical;
  else if (s == MYON_DECAL_MAPPING_UV)          return MYON_Decal::Mappings::UV;
  else if (s == MYON_DECAL_MAPPING_NONE)        return MYON_Decal::Mappings::None;

  MYON_ASSERT(false);
  return MYON_Decal::Mappings::None;
}

MYON_XMLVariant MYON_Decal::CImpl::GetParameter(const wchar_t* param_name, const MYON_XMLVariant& def) const
{
  return MYON_InternalXMLImpl::GetParameter(L"", param_name, def);
}

void MYON_Decal::CImpl::SetParameter(const wchar_t* param_name, const MYON_XMLVariant& value)
{
  if (nullptr != _collection)
    _collection->SetChanged();

  MYON_InternalXMLImpl::SetParameter(L"", param_name, value);
}

MYON_UUID MYON_Decal::CImpl::TextureInstanceId(void) const
{
  if (!_cache.texture_instance_id_set)
  {
    _cache.texture_instance_id = GetParameter(MYON_DECAL_TEXTURE_INSTANCE, MYON_nil_uuid).AsUuid();
    _cache.texture_instance_id_set = true;
  }

  return _cache.texture_instance_id;
}

void MYON_Decal::CImpl::SetTextureInstanceId(const MYON_UUID& id)
{
  if (!_cache.texture_instance_id_set || (_cache.texture_instance_id != id))
  {
    _cache.texture_instance_id = id;
    _cache.texture_instance_id_set = true;
    SetParameter(MYON_DECAL_TEXTURE_INSTANCE, id);
  }
}

MYON_Decal::Mappings MYON_Decal::CImpl::Mapping(void) const
{
  if (Mappings::None == _cache.mapping)
  {
    const MYON_wString s = GetParameter(MYON_DECAL_MAPPING, MYON_DECAL_MAPPING_UV).AsString();
    _cache.mapping = MappingFromString(s);
  }

  return _cache.mapping;
}

void MYON_Decal::CImpl::SetMapping(Mappings m)
{
  if (_cache.mapping != m)
  {
    _cache.mapping = m;

    const wchar_t* s = L"";
    switch (m)
    {
    default: MYON_ASSERT(false);
    case Mappings::None:        s = MYON_DECAL_MAPPING_NONE;        break;
    case Mappings::Planar:      s = MYON_DECAL_MAPPING_PLANAR;      break;
    case Mappings::Spherical:   s = MYON_DECAL_MAPPING_SPHERICAL;   break;
    case Mappings::Cylindrical: s = MYON_DECAL_MAPPING_CYLINDRICAL; break;
    case Mappings::UV:          s = MYON_DECAL_MAPPING_UV;          break;
    }

    SetParameter(MYON_DECAL_MAPPING, s);
  }
}

MYON_Decal::Projections MYON_Decal::CImpl::Projection(void) const
{
  if (Projections::None == _cache.projection)
  {
    const MYON_wString s = GetParameter(MYON_DECAL_PROJECTION, MYON_DECAL_PROJECTION_NONE).AsString();
         if (s == MYON_DECAL_PROJECTION_FORWARD)  _cache.projection = MYON_Decal::Projections::Forward;
    else if (s == MYON_DECAL_PROJECTION_BACKWARD) _cache.projection = MYON_Decal::Projections::Backward;
    else if (s == MYON_DECAL_PROJECTION_BOTH)     _cache.projection = MYON_Decal::Projections::Both;
    else if (s == MYON_DECAL_PROJECTION_NONE)     _cache.projection = MYON_Decal::Projections::None;
    else MYON_ASSERT(false);
  }

  return _cache.projection;
}

void MYON_Decal::CImpl::SetProjection(Projections v)
{
  if (_cache.projection != v)
  {
    _cache.projection = v;

    const wchar_t* s = L"";
    switch (v)
    {
    default: MYON_ASSERT(false);
    case MYON_Decal::Projections::None:     s = MYON_DECAL_PROJECTION_NONE;     break;
    case MYON_Decal::Projections::Forward:  s = MYON_DECAL_PROJECTION_FORWARD;  break;
    case MYON_Decal::Projections::Backward: s = MYON_DECAL_PROJECTION_BACKWARD; break;
    case MYON_Decal::Projections::Both:     s = MYON_DECAL_PROJECTION_BOTH;     break;
    }

    SetParameter(MYON_DECAL_PROJECTION, s);
  }
}

bool MYON_Decal::CImpl::MapToInside(void) const
{
  if (unset_bool == _cache.map_to_inside)
  {
    _cache.map_to_inside = GetParameter(MYON_DECAL_MAP_TO_INSIDE_ON, false).AsBool() ? 1 : 0;
  }

  return 0 != _cache.map_to_inside;
}

void MYON_Decal::CImpl::SetMapToInside(bool b)
{
  const int i = b ? 1 : 0;
  if (_cache.map_to_inside != i)
  {
    _cache.map_to_inside = i;
    SetParameter(MYON_DECAL_MAP_TO_INSIDE_ON, b);
  }
}

double MYON_Decal::CImpl::Transparency(void) const
{
  if (MYON_UNSET_VALUE == _cache.transparency)
  {
    _cache.transparency = GetParameter(MYON_DECAL_TRANSPARENCY, 0.0).AsDouble();
  }

  return _cache.transparency;
}

void MYON_Decal::CImpl::SetTransparency(double v)
{
  if (_cache.transparency != v)
  {
    _cache.transparency = v;
    SetParameter(MYON_DECAL_TRANSPARENCY, v);
  }
}

MYON_3dPoint MYON_Decal::CImpl::Origin(void) const
{
  if (MYON_3dPoint::UnsetPoint == _cache.origin)
  {
    _cache.origin = GetParameter(MYON_DECAL_ORIGIN, MYON_3dPoint::Origin).As3dPoint();
  }

  return _cache.origin;
}

void MYON_Decal::CImpl::SetOrigin(const MYON_3dPoint& pt)
{
  if (_cache.origin != pt)
  {
    _cache.origin = pt;
    SetParameter(MYON_DECAL_ORIGIN, pt);
  }
}

MYON_3dVector MYON_Decal::CImpl::VectorUp(void) const
{
  if (MYON_3dVector::UnsetVector == _cache.vector_up)
  {
     _cache.vector_up = GetParameter(MYON_DECAL_VECTOR_UP, MYON_3dPoint::Origin).As3dPoint();
  }

  return _cache.vector_up;
}

void MYON_Decal::CImpl::SetVectorUp(const MYON_3dVector& v)
{
  if (_cache.vector_up != v)
  {
    _cache.vector_up = v;
    SetParameter(MYON_DECAL_VECTOR_UP, MYON_3dPoint(v));
  }
}

MYON_3dVector MYON_Decal::CImpl::VectorAcross(void) const
{
  if (MYON_3dVector::UnsetVector == _cache.vector_across)
  {
     _cache.vector_across = GetParameter(MYON_DECAL_VECTOR_ACROSS, MYON_3dPoint::Origin).As3dPoint();
  }

  return _cache.vector_across;
}

void MYON_Decal::CImpl::SetVectorAcross(const MYON_3dVector& v)
{
  if (_cache.vector_across != v)
  {
    _cache.vector_across = v;
    SetParameter(MYON_DECAL_VECTOR_ACROSS, MYON_3dPoint(v));
  }
}

double MYON_Decal::CImpl::Height(void) const
{
  if (MYON_UNSET_VALUE == _cache.height)
  {
    _cache.height = GetParameter(MYON_DECAL_HEIGHT, 1.0).AsDouble();
  }

  return _cache.height;
}

void MYON_Decal::CImpl::SetHeight(double v)
{
  if (_cache.height != v)
  {
    _cache.height = v;
    SetParameter(MYON_DECAL_HEIGHT, v);
  }
}

double MYON_Decal::CImpl::Radius(void) const
{
  if (MYON_UNSET_VALUE == _cache.radius)
  {
    _cache.radius = GetParameter(MYON_DECAL_RADIUS, 1.0).AsDouble();
  }

  return _cache.radius;
}

void MYON_Decal::CImpl::SetRadius(double v)
{
  if (_cache.radius != v)
  {
    _cache.radius = v;
    SetParameter(MYON_DECAL_RADIUS, v);
  }
}

bool MYON_Decal::CImpl::IsVisible(void) const
{
  if (unset_bool == _cache.visible)
  {
    _cache.visible = GetParameter(MYON_DECAL_IS_VISIBLE, true).AsBool();
  }

  return 0 != _cache.visible;
}

void MYON_Decal::CImpl::SetIsVisible(bool b)
{
  const int i = b ? 1 : 0;
  if (_cache.visible != i)
  {
    _cache.visible = i;
    SetParameter(MYON_DECAL_IS_VISIBLE, b);
  }
}

void MYON_Decal::CImpl::GetHorzSweep(double& sta, double& end) const
{
  if (MYON_2dPoint::UnsetPoint == _cache.horz_sweep)
  {
    _cache.horz_sweep.x = GetParameter(MYON_DECAL_HORZ_SWEEP_STA, 0.0).AsDouble();
    _cache.horz_sweep.y = GetParameter(MYON_DECAL_HORZ_SWEEP_END, 0.0).AsDouble();
  }

  sta = _cache.horz_sweep.x;
  end = _cache.horz_sweep.y;
}

void MYON_Decal::CImpl::SetHorzSweep(double sta, double end)
{
  const auto sweep = MYON_2dPoint(sta, end);
  if (_cache.horz_sweep != sweep)
  {
    _cache.horz_sweep = sweep;
    SetParameter(MYON_DECAL_HORZ_SWEEP_STA, sta);
    SetParameter(MYON_DECAL_HORZ_SWEEP_END, end);
  }
}

void MYON_Decal::CImpl::GetVertSweep(double& sta, double& end) const
{
  if (MYON_2dPoint::UnsetPoint == _cache.vert_sweep)
  {
    _cache.vert_sweep.x = GetParameter(MYON_DECAL_VERT_SWEEP_STA, 0.0).AsDouble();
    _cache.vert_sweep.y = GetParameter(MYON_DECAL_VERT_SWEEP_END, 0.0).AsDouble();
  }

  sta = _cache.vert_sweep.x;
  end = _cache.vert_sweep.y;
}

void MYON_Decal::CImpl::SetVertSweep(double sta, double end)
{
  const auto sweep = MYON_2dPoint(sta, end);
  if (_cache.vert_sweep != sweep)
  {
    _cache.vert_sweep = sweep;
    SetParameter(MYON_DECAL_VERT_SWEEP_STA, sta);
    SetParameter(MYON_DECAL_VERT_SWEEP_END, end);
  }
}

void MYON_Decal::CImpl::GetUVBounds(double& min_u, double& min_v, double& max_u, double& max_v) const
{
  if (UNSET_4D_POINT == _cache.uv_bounds)
  {
    _cache.uv_bounds.x = GetParameter(MYON_DECAL_MIN_U, 0.0).AsDouble();
    _cache.uv_bounds.y = GetParameter(MYON_DECAL_MIN_V, 0.0).AsDouble();
    _cache.uv_bounds.z = GetParameter(MYON_DECAL_MAX_U, 1.0).AsDouble();
    _cache.uv_bounds.w = GetParameter(MYON_DECAL_MAX_V, 1.0).AsDouble();
  }

  min_u = _cache.uv_bounds.x;
  min_v = _cache.uv_bounds.y;
  max_u = _cache.uv_bounds.z;
  max_v = _cache.uv_bounds.w;
}

void MYON_Decal::CImpl::SetUVBounds(double min_u, double min_v, double max_u, double max_v)
{
  const auto bounds = MYON_4dPoint(min_u, min_v, max_u, max_v);
  if (_cache.uv_bounds != bounds)
  {
    _cache.uv_bounds = bounds;
    SetParameter(MYON_DECAL_MIN_U, min_u);
    SetParameter(MYON_DECAL_MIN_V, min_v);
    SetParameter(MYON_DECAL_MAX_U, max_u);
    SetParameter(MYON_DECAL_MAX_V, max_v);
  }
}

MYON_XMLNode* MYON_Decal::CImpl::FindCustomNodeForRenderEngine(const MYON_UUID& renderEngineId) const
{
  MYON_XMLNode* child_node = nullptr;
  auto it = Node().GetChildIterator();
  while (nullptr != (child_node = it.GetNextChild()))
  {
    if (child_node->TagName() == MYON_DECAL_CUSTOM)
    {
      const MYON_XMLProperty* prop = child_node->GetNamedProperty(MYON_DECAL_CUSTOM_RENDERER);
      if ((nullptr != prop) && (prop->GetValue().AsUuid() == renderEngineId))
        return child_node;
    }
  }

  return nullptr;
}

MYON_Decal::MYON_Decal()
{
  _impl = new CImpl;
}

MYON_Decal::MYON_Decal(MYON_DecalCollection& dc, MYON_XMLNode& node)
{
  MYON_ASSERT(node.TagName() == MYON_DECAL);

  _impl = new CImpl(dc, node);
}

MYON_Decal::MYON_Decal(const MYON_Decal& d)
{
  _impl = new CImpl;
  operator = (d);
}

MYON_Decal::~MYON_Decal()
{
  delete _impl;
  _impl = nullptr;
}

const MYON_Decal& MYON_Decal::operator = (const MYON_Decal& d)
{
  if (this != &d)
  {
    _impl->Node() = d._impl->Node();
  }

  return *this;
}

bool MYON_Decal::operator == (const MYON_Decal& d)
{
  // This only checks if the basic parameters are equal. It ignores any custom data.

  if (TextureInstanceId() != d.TextureInstanceId()) return false;
  if (Mapping()           != d.Mapping())           return false;
  if (Projection()        != d.Projection())        return false;
  if (MapToInside()       != d.MapToInside())       return false;
  if (Transparency()      != d.Transparency())      return false;
  if (Origin()            != d.Origin())            return false;
  if (VectorUp()          != d.VectorUp())          return false;
  if (VectorAcross()      != d.VectorAcross())      return false;
  if (Height()            != d.Height())            return false;
  if (Radius()            != d.Radius())            return false;

  double sta1 = 0.0, end1 = 0.0, sta2 = 0.0, end2 = 0.0;
    GetHorzSweep(sta1, end1);
  d.GetHorzSweep(sta2, end2);
  if (!IsDoubleEqual(sta1, sta2)) return false;
  if (!IsDoubleEqual(end1, end2)) return false;

    GetVertSweep(sta1, end1);
  d.GetVertSweep(sta2, end2);
  if (!IsDoubleEqual(sta1, sta2)) return false;
  if (!IsDoubleEqual(end1, end2)) return false;

  double min_u1 = 0.0, min_v1 = 0.0, max_u1 = 0.0, max_v1 = 0.0;
  double min_u2 = 0.0, min_v2 = 0.0, max_u2 = 0.0, max_v2 = 0.0;
    GetUVBounds(min_u1, min_v1, max_u1, max_v1);
  d.GetUVBounds(min_u2, min_v2, max_u2, max_v2);
  if (!IsDoubleEqual(min_u1, min_u2)) return false;
  if (!IsDoubleEqual(min_v1, min_v2)) return false;
  if (!IsDoubleEqual(max_u1, max_u2)) return false;
  if (!IsDoubleEqual(max_v1, max_v2)) return false;

  return true;
}

bool MYON_Decal::operator != (const MYON_Decal& d)
{
  return !(operator == (d));
}

MYON_UUID MYON_Decal::TextureInstanceId(void) const
{
  return _impl->TextureInstanceId();
}

void MYON_Decal::SetTextureInstanceId(const MYON_UUID& id)
{
  _impl->SetTextureInstanceId(id);
}

MYON_Decal::Mappings MYON_Decal::Mapping(void) const
{
  return _impl->Mapping();
}

void MYON_Decal::SetMapping(Mappings m)
{
  _impl->SetMapping(m);
}

MYON_Decal::Projections MYON_Decal::Projection(void) const
{
  return _impl->Projection();
}

void MYON_Decal::SetProjection(Projections p)
{
  _impl->SetProjection(p);
}

bool MYON_Decal::MapToInside(void) const
{
  return _impl->MapToInside();
}

void MYON_Decal::SetMapToInside(bool b)
{
  _impl->SetMapToInside(b);
}

double MYON_Decal::Transparency(void) const
{
  return _impl->Transparency();
}

void MYON_Decal::SetTransparency(double d)
{
  _impl->SetTransparency(d);
}

MYON_3dPoint MYON_Decal::Origin(void) const
{
  return _impl->Origin();
}

void MYON_Decal::SetOrigin(const MYON_3dPoint& pt)
{
  _impl->SetOrigin(pt);
}

MYON_3dVector MYON_Decal::VectorUp(void) const
{
  return _impl->VectorUp();
}

void MYON_Decal::SetVectorUp(const MYON_3dVector& vec)
{
  _impl->SetVectorUp(vec);
}

MYON_3dVector MYON_Decal::VectorAcross(void) const
{
  return _impl->VectorAcross();
}

void MYON_Decal::SetVectorAcross(const MYON_3dVector& vec)
{
  _impl->SetVectorAcross(vec);
}

double MYON_Decal::Height(void) const
{
  return _impl->Height();
}

void MYON_Decal::SetHeight(double d)
{
  _impl->SetHeight(d);
}

double MYON_Decal::Radius(void) const
{
  return _impl->Radius();
}

void MYON_Decal::SetRadius(double d)
{
  _impl->SetRadius(d);
}

void MYON_Decal::GetHorzSweep(double& sta, double& end) const
{
  _impl->GetHorzSweep(sta, end);
}

void MYON_Decal::SetHorzSweep(double sta, double end)
{
  _impl->SetHorzSweep(sta, end);
}

void MYON_Decal::GetVertSweep(double& sta, double& end) const
{
  _impl->GetVertSweep(sta, end);
}

void MYON_Decal::SetVertSweep(double sta, double end)
{
  _impl->SetVertSweep(sta, end);
}

void MYON_Decal::GetUVBounds(double& min_u, double& min_v, double& max_u, double& max_v) const
{
  _impl->GetUVBounds(min_u, min_v, max_u, max_v);
}

void MYON_Decal::SetUVBounds(double min_u, double min_v, double max_u, double max_v)
{
  _impl->SetUVBounds(min_u, min_v, max_u, max_v);
}

bool MYON_Decal::IsVisible(void) const
{
  return _impl->IsVisible();
}

void MYON_Decal::SetIsVisible(bool b)
{
  _impl->SetIsVisible(b);
}

MYON_UUID MYON_Decal::Id(void) const
{
  return _impl->Id();
}

MYON__UINT32 MYON_Decal::DecalCRC(void) const
{
  return DataCRC(0);
}

MYON__UINT32 MYON_Decal::DataCRC(MYON__UINT32 current_remainder) const
{
  return ComputeDecalCRC(current_remainder, _impl->Node());
}

void MYON_Decal::GetCustomXML(const MYON_UUID& renderEngineId, MYON_XMLNode& custom_param_node) const
{
  custom_param_node.Clear();
  custom_param_node.SetTagName(MYON_DECAL_CUSTOM_PARAMS);

  // Find the node for 'renderEngineId'.
  const MYON_XMLNode* custom_node = _impl->FindCustomNodeForRenderEngine(renderEngineId);
  if (nullptr != custom_node)
  {
    // Get the parameter node and copy it to 'custom_param_node'.
    const MYON_XMLNode* param_node = custom_node->GetNamedChild(MYON_DECAL_CUSTOM_PARAMS);
    if (nullptr != param_node)
    {
      custom_param_node = *param_node;
    }
  }
}

bool MYON_Decal::SetCustomXML(const MYON_UUID& renderEngineId, const MYON_XMLNode& custom_param_node)
{
  if (custom_param_node.TagName() != MYON_DECAL_CUSTOM_PARAMS)
    return false;

  // If there is already a custom node for 'renderEngineId' then delete it.
  MYON_XMLNode* custom_node = _impl->FindCustomNodeForRenderEngine(renderEngineId);
  if (nullptr != custom_node)
  {
    MYON_XMLNode* parent = custom_node->GetParent();
    if (nullptr != parent)
    {
      delete parent->DetachChild(*custom_node);
    }
  }

  // Add the new custom node and set its 'renderer' property to be the render engine id.
  custom_node = _impl->Node().AttachChildNode(new MYON_XMLNode(MYON_DECAL_CUSTOM));
  MYON_XMLProperty prop(MYON_DECAL_CUSTOM_RENDERER, renderEngineId);
  custom_node->AddProperty(prop);

  // Attach a copy of the custom param node to the custom node.
  custom_node->AttachChildNode(new MYON_XMLNode(custom_param_node));

  return true;
}

// Copied from IRhRdkDecal::GetTextureMapping -- TODO: Refactor. [JOHN-DECAL-FIX]
bool MYON_Decal::GetTextureMapping(MYON_TextureMapping& mappingOut) const
{
  const auto& decal = *this;
  using M = MYON_Decal::Mappings;

  switch (decal.Mapping())
  {
  case M::Cylindrical:
    {
    // Orthogonal vectors in the end plane of cylinder.
    const auto vecPlaneXAxis = decal.VectorAcross();
    const auto vecPlaneYAxis = MYON_CrossProduct(decal.VectorAcross(), -decal.VectorUp());

    // Center for the end of the cylinder.
    const auto ptCylinderEndCenter = decal.Origin() - decal.VectorUp() * decal.Height() * 0.5;

    // Plane for the end of the cylinder.
    MYON_Plane plane(ptCylinderEndCenter, vecPlaneXAxis, vecPlaneYAxis);

    // Circle for the end of the cylinder.
    MYON_Circle circle(plane, decal.Radius());

    // The cylinder itself..
    MYON_Cylinder cylinder(circle, decal.Height());

    // Cylindrical mapping without caps.
    mappingOut.SetCylinderMapping(cylinder, false);

    return true;
    }

  case M::Spherical:
    {
    // Orthogonal vectors in the equatorial plane.
    const auto vecPlaneXAxis = decal.VectorAcross();
    const auto vecPlaneYAxis = MYON_CrossProduct(decal.VectorAcross(), -decal.VectorUp());

    // Equatorial plane.
    MYON_Plane plane(decal.Origin(), vecPlaneXAxis, vecPlaneYAxis);

    // The sphere itself.
    MYON_Sphere sphere;
    sphere.plane = plane;
    sphere.radius = decal.Radius();

    // Spherical mapping.
    mappingOut.SetSphereMapping(sphere);

    return true;
    }

  case M::Planar:
    {
    const auto vecAcross = decal.VectorAcross();
    const auto vecUp = decal.VectorUp();
    const MYON_Plane plane(decal.Origin(), vecAcross, vecUp);
    const MYON_Interval xInterval(0.0, vecAcross.Length());
    const MYON_Interval yInterval(0.0, vecUp.Length());
    const MYON_Interval zInterval(0.0, 1.0);
    mappingOut.SetPlaneMapping(plane, xInterval, yInterval, zInterval);

    return true;
    }

  default: break;
  }

  return false;
}

/** \class MYON_DecalNodeReader

    This object encapsulates the reading of all decal properties from XML nodes.
    It is used by the decal CRC calculation in ComputeDecalCRC().

    TODO: It could also be used by the MYON_Decal XML node acccess.

*/
class MYON_DecalNodeReader
{
public:
  MYON_DecalNodeReader(const MYON_XMLNode* p) : m_pNode(p) { }

  MYON_XMLVariant Mapping(void) const           { return Value(MYON_DECAL_MAPPING, MYON_DECAL_MAPPING_NONE); }
  MYON_XMLVariant Projection(void) const        { return Value(MYON_DECAL_PROJECTION, MYON_DECAL_PROJECTION_NONE); }
  MYON_XMLVariant MapToInside(void) const       { return Value(MYON_DECAL_MAP_TO_INSIDE_ON, m_def.MapToInside()); } 
  MYON_XMLVariant Transparency(void) const      { return Value(MYON_DECAL_TRANSPARENCY    , m_def.Transparency()); }
  MYON_XMLVariant TextureInstanceId(void) const { return Value(MYON_DECAL_TEXTURE_INSTANCE, m_def.TextureInstanceId()); }
  MYON_XMLVariant Height(void) const            { return Value(MYON_DECAL_HEIGHT          , m_def.Height()); }
  MYON_XMLVariant Radius(void) const            { return Value(MYON_DECAL_RADIUS          , m_def.Radius()); }
  MYON_XMLVariant Origin(void) const            { return Value(MYON_DECAL_ORIGIN          , m_def.Origin()); }
  MYON_XMLVariant VectorUp(void) const          { return Value(MYON_DECAL_VECTOR_UP       , MYON_3dPoint(m_def.VectorUp())); }
  MYON_XMLVariant VectorAcross(void) const      { return Value(MYON_DECAL_VECTOR_ACROSS   , MYON_3dPoint(m_def.VectorAcross())); }
  MYON_XMLVariant HorzSweepSta(void) const      { return Value(MYON_DECAL_HORZ_SWEEP_STA  , DefaultHorzSweepSta()); }
  MYON_XMLVariant HorzSweepEnd(void) const      { return Value(MYON_DECAL_HORZ_SWEEP_END  , DefaultHorzSweepEnd()); }
  MYON_XMLVariant VertSweepSta(void) const      { return Value(MYON_DECAL_VERT_SWEEP_STA  , DefaultVertSweepSta()); }
  MYON_XMLVariant VertSweepEnd(void) const      { return Value(MYON_DECAL_VERT_SWEEP_END  , DefaultVertSweepEnd()); }
  MYON_XMLVariant MinU(void) const              { return Value(MYON_DECAL_MIN_U           , DefaultMinU()); }
  MYON_XMLVariant MinV(void) const              { return Value(MYON_DECAL_MIN_V           , DefaultMinV()); }
  MYON_XMLVariant MaxU(void) const              { return Value(MYON_DECAL_MAX_U           , DefaultMaxU()); }
  MYON_XMLVariant MaxV(void) const              { return Value(MYON_DECAL_MAX_V           , DefaultMaxV()); }
  MYON_XMLVariant IsTemporary(void) const       { return Value(MYON_DECAL_IS_TEMPORARY    , false); }
  MYON_XMLVariant IsVisible(void) const         { return Value(MYON_DECAL_IS_VISIBLE      , m_def.IsVisible()); }
  MYON_XMLVariant InstanceId(void) const        { return Value(MYON_DECAL_INSTANCE_ID     , m_def.Id()); }

private:
  MYON_XMLVariant Value(const wchar_t* wszName, const MYON_XMLVariant& vDefault) const;

  double DefaultHorzSweepSta(void) const { double a, b; m_def.GetHorzSweep(a, b); return a; }
  double DefaultHorzSweepEnd(void) const { double a, b; m_def.GetHorzSweep(a, b); return b; }
  double DefaultVertSweepSta(void) const { double a, b; m_def.GetVertSweep(a, b); return a; }
  double DefaultVertSweepEnd(void) const { double a, b; m_def.GetVertSweep(a, b); return b; }

  double DefaultMinU(void) const { double a, b, c, d; m_def.GetUVBounds(a, b, c, d); return a; }
  double DefaultMinV(void) const { double a, b, c, d; m_def.GetUVBounds(a, b, c, d); return b; }
  double DefaultMaxU(void) const { double a, b, c, d; m_def.GetUVBounds(a, b, c, d); return c; }
  double DefaultMaxV(void) const { double a, b, c, d; m_def.GetUVBounds(a, b, c, d); return d; }

private:
  const MYON_XMLNode* m_pNode;
  const MYON_Decal m_def;
};

MYON_XMLVariant MYON_DecalNodeReader::Value(const wchar_t* wszName, const MYON_XMLVariant& vDefault) const
{
  MYON_XMLVariant vValue = vDefault;

  if (nullptr != m_pNode)
  {
    const MYON_XMLParameters p(*m_pNode);
    p.GetParam(wszName, vValue);
  }

  return vValue;
}

static void DecalUpdateCRC(MYON__UINT32& crc, const MYON_XMLVariant value)
{
  crc = value.DataCRC(crc);
}

MYON__UINT32 MYON_Decal::ComputeDecalCRC(MYON__UINT32 crc, const MYON_XMLNode& node) // Static.
{
  const MYON_DecalNodeReader d(&node);

  const MYON_wString s = d.Mapping().AsString();
  const auto mapping = MappingFromString(s);

  DecalUpdateCRC(crc, d.Mapping());
  DecalUpdateCRC(crc, d.IsVisible());
  DecalUpdateCRC(crc, d.IsTemporary());
  DecalUpdateCRC(crc, d.Transparency());
  DecalUpdateCRC(crc, d.TextureInstanceId());

  if (Mappings::Planar == mapping)
  {
    DecalUpdateCRC(crc, d.MinU());
    DecalUpdateCRC(crc, d.MinV());
    DecalUpdateCRC(crc, d.MaxU());
    DecalUpdateCRC(crc, d.MaxV());
  }
  else
  {
    DecalUpdateCRC(crc, d.Origin());
    DecalUpdateCRC(crc, d.VectorUp());
    DecalUpdateCRC(crc, d.VectorAcross());
    DecalUpdateCRC(crc, d.Projection());

    if ((Mappings::Cylindrical == mapping) || (Mappings::Spherical == mapping))
    {
      DecalUpdateCRC(crc, d.Radius());
      DecalUpdateCRC(crc, d.MapToInside());
      DecalUpdateCRC(crc, d.HorzSweepSta());
      DecalUpdateCRC(crc, d.HorzSweepEnd());

      if (Mappings::Cylindrical == mapping)
      {
        DecalUpdateCRC(crc, d.Height());
      }
      else
      if (Mappings::Spherical == mapping)
      {
        DecalUpdateCRC(crc, d.VertSweepSta());
        DecalUpdateCRC(crc, d.VertSweepEnd());
      }
    }
  }

  // Look for custom data nodes and for each one, find the parameter node and then iterate over its
  // children and CRC the properties. For now, we will have to rely on the raw XML. A better solution
  // would be to have the plug-in that created this XML calculate the CRC itself.
  auto it = node.GetChildIterator();
  MYON_XMLNode* pChildNode = nullptr;
  while (nullptr != (pChildNode = it.GetNextChild()))
  {
    if (pChildNode->TagName() != MYON_DECAL_CUSTOM)
      continue; // Not a custom data node.

    MYON_XMLProperty* prop = pChildNode->GetNamedProperty(MYON_DECAL_CUSTOM_RENDERER);
    if (nullptr != prop)
    {
      // Include the render engine id.
      const MYON_UUID uuid = prop->GetValue().AsUuid();
      crc = MYON_CRC32(crc, sizeof(uuid), &uuid);
    }

    // Find the custom parameter node.
    const MYON_XMLNode* pParamNode = pChildNode->GetNamedChild(MYON_DECAL_CUSTOM_PARAMS);
    if (nullptr != pParamNode)
    {
      // Iterate over the nodes inside the custom parameter node.
      const MYON_XMLParameters p(*pParamNode);
      auto* pIterator = p.NewIterator();

      MYON_wString sParamName;
      MYON_XMLVariant vParamValue;
      while (pIterator->Next(sParamName, vParamValue))
      {
        DecalUpdateCRC(crc, vParamValue);
      }

      delete pIterator;
    }
  }

  // Make sure it's not zero which would mean 'nil'.
  if (0 == crc)
    crc--;

  return crc;
}

// MYON_DecalCollection

MYON_DecalCollection::~MYON_DecalCollection()
{
  DeleteAllDecals();
}

int MYON_DecalCollection::FindDecalIndex(const MYON_Decal& decal) const
{
  for (int i = 0; i < m_decals.Count(); i++)
  {
    if (decal.Id() == m_decals[i]->Id())
      return i;
  }

  return -1;
}

bool MYON_DecalCollection::DeleteDecal(MYON_Decal& decal)
{
  // Ensure the array is populated before deleting a decal.
  GetDecalArray();

  const int index = FindDecalIndex(decal);
  if (index < 0)
    return false;

  delete m_decals[index];
  m_decals.Remove(index);

  return true;
}

void MYON_DecalCollection::DeleteAllDecals(void)
{
  m_root_node.Clear();
  m_root_node.CreateNodeAtPath(MYON_RDK_UD_ROOT);

  for (int i = 0; i < m_decals.Count(); i++)
  {
    delete m_decals[i];
  }

  m_decals.Destroy();

  m_populated = true;

  SetChanged();
}

const MYON_DecalCollection& MYON_DecalCollection::operator = (const MYON_DecalCollection& dc)
{
  DeleteAllDecals();

  for (int i = 0; i < dc.m_decals.Count(); i++)
  {
    MYON_Decal* decal = dc.m_decals[i];
    if (nullptr != decal)
    {
      m_decals.Append(new MYON_Decal(*decal));
    }
  }

  return *this;
}

const MYON_SimpleArray<MYON_Decal*>& MYON_DecalCollection::GetDecalArray(void) const
{
  if (!m_populated)
  {
    Populate();

    m_populated = true;
  }

  return m_decals;
}

void MYON_DecalCollection::Populate(void) const
{
  if (nullptr == m_attr)
    return;

  if (GetEntireDecalXML(*m_attr, m_root_node))
  {
    const wchar_t* path = MYON_RDK_UD_ROOT  MYON_RDK_SLASH  MYON_DECALS;
    MYON_XMLNode* decals_node = m_root_node.GetNodeAtPath(path);
    if (nullptr != decals_node)
    {
      // Iterate over the decals under the decals node adding a new decal for each one.
      auto it = decals_node->GetChildIterator();
      MYON_XMLNode* child_node = nullptr;
      while (nullptr != (child_node = it.GetNextChild()))
      {
        auto* decal = new MYON_Decal(*const_cast<MYON_DecalCollection*>(this), *child_node);
        m_decals.Append(decal);
      }
    }
  }
}

void MYON_DecalCollection::SetChanged(void)
{
  MYON_ASSERT(m_populated);

  if (m_populated)
    m_changed = true;
}

MYON_Decal* MYON_DecalCollection::AddDecal(void)
{
  // Ensure the array is populated before adding a new decal.
  GetDecalArray();

  MYON_Decal* decal = nullptr;

  MYON_XMLNode* decals_node = m_root_node.CreateNodeAtPath(MYON_RDK_UD_ROOT  MYON_RDK_SLASH  MYON_DECALS);
  if (nullptr != decals_node)
  {
    // Add an XML node for the new decal.
    auto* decal_node = new MYON_XMLNode(MYON_DECAL);
    decals_node->AttachChildNode(decal_node);

    // Add the new decal. It stores a pointer to the new XML node. This is safe because
    // the decals have the same lifetime as the root node that owns the XML nodes.
    decal = new MYON_Decal(*this, *decal_node);
    m_decals.Append(decal);

    SetChanged();
  }

  return decal;
}

void MYON_DecalCollection::UpdateUserData(unsigned int archive_3dm_version) const
{
  if (m_changed)
  {
    MYON_ASSERT(m_populated);

    SetRDKObjectInformation(*m_attr, m_root_node.String(), archive_3dm_version);

    m_changed = false;
  }
}

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

#if !defined(MYON_DECALS_INC_)
#define MYON_DECALS_INC_

class MYON_CLASS MYON_Decal
{
public:
  MYON_Decal();
  MYON_Decal(class MYON_DecalCollection& coll, MYON_XMLNode& node);
  MYON_Decal(const MYON_Decal& d);
  virtual ~MYON_Decal();

  const MYON_Decal& operator = (const MYON_Decal& d);

  bool operator == (const MYON_Decal& d);
  bool operator != (const MYON_Decal& d);

  enum class Mappings : MYON__INT32
  {
    None        = -1,
    Planar      =  0, // Planar mapping. Uses projection, origin, up and across vectors (not unitized).
    Cylindrical =  1, // Cylindrical mapping. Uses origin, up, across, height, radius, horz-sweep.
    Spherical   =  2, // Spherical mapping. Uses origin, up, across, radius, horz-sweep, vert-sweep.
    UV          =  3, // UV mapping. Uses UV bounds.
  };

  enum class Projections : MYON__INT32
  {
    None     = -1,
    Forward  =  0, // Project forward.
    Backward =  1, // Project backward.
    Both     =  2, // Project forward and backward.
  };

  // Returns the decal texture's instance id.
  MYON_UUID TextureInstanceId(void) const;

  // Sets the decal texture's instance id.
  void SetTextureInstanceId(const MYON_UUID& id);

  // Returns the decal's mapping.
  Mappings Mapping(void) const;

  // Sets the decal's mapping.
  void SetMapping(Mappings m);

  // Returns the decal's projection. Used only when mapping is planar.
  Projections Projection(void) const;

  // Sets the decal's projection. Used only when mapping is planar.
  void SetProjection(Projections p);

  // Returns true if the texture is mapped to inside of sphere or cylinder, else false.
  // Used only when mapping is cylindrical or spherical.
  bool MapToInside(void) const;

  // Sets if the texture is mapped to inside of sphere or cylinder.
  // Used only when mapping is cylindrical or spherical.
  void SetMapToInside(bool b);

  // Returns the decal's transparency in the range 0..1.
  double Transparency(void) const;

  // Sets the decal's transparency (clamped to the range 0..1).
  void SetTransparency(double d);

  // Returns the origin of the decal in world space. Not used when the mapping is UV.
  MYON_3dPoint Origin(void) const;

  // Sets the origin of the decal in world space. Not used when the mapping is UV.
  void SetOrigin(const MYON_3dPoint& pt);

  // Returns the 'up' vector of the decal. For planar mapping the length of the vector is relevant.
  // Not used when the mapping is UV. For cylindrical and spherical mapping, the vector is unitized.
  MYON_3dVector VectorUp(void) const;

  // Sets the 'up' vector of the decal. Not used when the mapping is UV.
  void SetVectorUp(const MYON_3dVector& vec);

  // Returns the 'across' vector of the decal. For planar mapping the length of the vector is relevant.
  // Not used when the mapping is UV.
  // For cylindrical and spherical mapping, the vector is unitized.
  MYON_3dVector VectorAcross(void) const;

  // Sets the 'across' vector of the decal. Not used when the mapping is UV.
  void SetVectorAcross(const MYON_3dVector& vec);

  // Returns the height of the decal. Only used when mapping is cylindrical.
  double Height(void) const;

  // Sets the height of the decal. Only used when mapping is cylindrical.
  void SetHeight(double d);

  // Returns the radius of the decal. Only used when mapping is cylindrical or spherical.
  double Radius(void) const;

  // Sets the radius of the decal. Only used when mapping is cylindrical or spherical.
  void SetRadius(double d);

  // Gets the start and end angles of the decal's 'horizontal sweep' (these are angles of longitude in radians).
  // Only used when mapping is cylindrical or spherical.
  void GetHorzSweep(double& sta, double& end) const;

  // Sets the start and end angles of the decal's 'horizontal sweep' (these are angles of longitude in radians).
  // Only used when mapping is cylindrical or spherical.
  void SetHorzSweep(double sta, double end);

  // Gets the start and end angles of the decal's 'vertical sweep' (these are angles of latitude in radians).
  // Only used when mapping is spherical.
  void GetVertSweep(double& sta, double& end) const;

  // Sets the start and end angles of the decal's 'vertical sweep' (these are angles of latitude in radians).
  // Only used when mapping is spherical.
  void SetVertSweep(double sta, double end);

  // Returns the UV bounds of the decal. Only used when mapping is UV.
  void GetUVBounds(double& min_u, double& min_v, double& max_u, double& max_v) const;

  // Sets the UV bounds of the decal. Only used when mapping is UV.
  void SetUVBounds(double min_u, double min_v, double max_u, double max_v);

  // Gets a texture mapping based on the properties of this decal. Only works and returns true if
  // the decal mapping is Planar, Spherical or Cylindrical. Otherwise returns false.
  bool GetTextureMapping(MYON_TextureMapping& tm) const;

  // Returns the Decal CRC of the decal.
  MYON__UINT32 DecalCRC(void) const;

  // Returns the Data CRC of the decal. This is not necessarily the same as the decal CRC
  // because it allows a starting current remainder.
  MYON__UINT32 DataCRC(MYON__UINT32 current_remainder) const;

  // Returns true if the decal is visible in the rendering.
  bool IsVisible(void) const;

  // Sets whether or not the decal is visible in the rendering.
  void SetIsVisible(bool visible);

  // Returns the unique id of the decal. This is a run-time id that is not persistent and is
  // only used for looking decals up in the model.
  MYON_UUID Id(void) const;

  // Get the custom XML for the specified render engine. The format of the XML is described below.
  void GetCustomXML(const MYON_UUID& renderEngineId, MYON_XMLNode& custom_param_node) const;

  // Set the custom XML for the specified render engine. This XML should have the following format:
  //
  //  <parameters>
  //    <param-name type="type"></param-name>
  //    ...
  //  </parameters>
  //
  // Therefore 'custom_param_node' must have a tag name of "<parameters>". The easiest way to produce
  // such XML is by using MYON_XMLParameters.
  bool SetCustomXML(const MYON_UUID& renderEngineId, const MYON_XMLNode& custom_param_node);

public: // For internal use only.
  static MYON__UINT32 ComputeDecalCRC(MYON__UINT32, const MYON_XMLNode&);

private:
  class CImpl;
  CImpl* _impl;
};

#endif

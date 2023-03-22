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

#if !defined(MYON_GROUND_PLANE_INC_)
#define MYON_GROUND_PLANE_INC_

class MYON_CLASS MYON_GroundPlane final
{
public:
  MYON_GroundPlane();
  MYON_GroundPlane(MYON_XMLNode& model_node);
  MYON_GroundPlane(const MYON_GroundPlane& gp);
  ~MYON_GroundPlane();

  const MYON_GroundPlane& operator = (const MYON_GroundPlane& gp);

  bool operator == (const MYON_GroundPlane& gp);
  bool operator != (const MYON_GroundPlane& gp);

  // Returns true if the ground plane is enabled, else false.
  bool On(void) const;

  // Returns true if ground plane backface is enabled, else false.
  bool ShowUnderside(void) const;

  // Returns the altitude of the ground plane.
  double Altitude(void) const;

  // Returns true if auto-altitude is enabled.
  bool AutoAltitude(void) const;

  // Returns true if the ground plane is set to shadow-only.
  bool ShadowOnly(void) const;

  // Returns the instance id of the ground plane's material.
  MYON_UUID MaterialInstanceId(void) const;

  // Returns the texture offset of the ground plane in model units.
  MYON_2dVector TextureOffset(void) const;

  // Returns true if the texture offset x and y are locked together.
  bool TextureOffsetLocked(void) const;

  // Returns true if the texture repeat x and y are locked together.
  bool TextureRepeatLocked(void) const;

  // Returns the texture size of the ground plane in model units.
  MYON_2dVector TextureSize(void) const;

  // Returns the texture rotation of the ground plane in degrees.
  double TextureRotation(void) const;

  // Set the ground plane enabled state.
  void SetOn(bool b);

  // Set if the ground plane backface is enabled.
  void SetShowUnderside(bool b);

  // Set the ground plane's altitude.
  void SetAltitude(double d);

  // Set if the ground plane is set to auto-altitude.
  void SetAutoAltitude(bool b);

  // Set if the ground plane is set to shadow-only.
  void SetShadowOnly(bool b);

  // Set the instance id of the ground plane's material.
  void SetMaterialInstanceId(const MYON_UUID& uuid);

  // Set the texture offset of the ground plane in model units.
  void SetTextureOffset(const MYON_2dVector& vec);

  // Set if the texture offset x and y are locked together.
  void SetTextureOffsetLocked(bool b);

  // Set if the texture repeat x and y are locked together.
  void SetTextureRepeatLocked(bool b);

  // Set the texture size of the ground plane in model units.
  void SetTextureSize(const MYON_2dVector& vec);

  // Set the texture rotation of the ground plane in degrees.
  void SetTextureRotation(double d);

private:
  class CImpl;
  CImpl* m_impl;
};

#endif

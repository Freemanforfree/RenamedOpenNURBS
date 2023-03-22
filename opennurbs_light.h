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

#if !defined(OPENNURBS_LIGHT_INC_)
#define OPENNURBS_LIGHT_INC_

class MYON_CLASS MYON_Light : public MYON_Geometry
{
  MYON_OBJECT_DECLARE(MYON_Light);

public:
  MYON_Light();
  ~MYON_Light();
  MYON_Light& operator=(const MYON_Light&) = default;
  MYON_Light(const MYON_Light&) = default;

  static const MYON_Light Unset;

  /////////////////////////////////////////////////////////////////
  //
  // MYON_Object virtual functions 
  //

  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;

  void Dump( MYON_TextLog& ) const override; // for debugging

  // Use MYON_BinaryArchive::WriteObject() and MYON_BinaryArchive::ReadObject()
  // for top level serialization.  These Read()/Write() members should just
  // write/read specific definitions.  In particular, they should not write/
  // read any chunk typecode or length information.  The default 
  // implementations return false and do nothing.
  bool Write(
         MYON_BinaryArchive&  // serialize definition to binary archive
       ) const override;

  bool Read(
         MYON_BinaryArchive&  // restore definition from binary archive
       ) override;

  MYON::object_type ObjectType() const override;

  // virtual
  MYON_UUID ModelObjectId() const override;


  /////////////////////////////////////////////////////////////////
  //
  // MYON_Geometry virtual functions 
  //
  int Dimension() const override;

  // virtual MYON_Geometry GetBBox override		
  bool GetBBox( double* boxmin, double* boxmax, bool bGrowBox = false ) const override;

  // virtual MYON_Geometry GetTightBoundingBox override		
  bool GetTightBoundingBox(MYON_BoundingBox& tight_bbox, bool bGrowBoxAsInt, const MYON_Xform* xform) const override;


  bool Transform( 
         const MYON_Xform&
         ) override;
 
  /////////////////////////////////////////////////////////
  //
  // Interface
  //

  void Default(); // make default light

  /////////////////////////////////////////////////////////
  //
  // turn light on/off
  //
  bool Enable( bool = true ); // returns previous state
  bool IsEnabled() const;
  
  /////////////////////////////////////////////////////////
  //
  // style, location, and direction
  //   direction is ignored for "point" and "ambient" lights
  //   location is ignored for "directional" and "ambient" lights
  void SetStyle(MYON::light_style);
  MYON::light_style Style() const;

  const bool IsPointLight() const;
  const bool IsDirectionalLight() const;
  const bool IsSpotLight() const;
  const bool IsLinearLight() const;
  const bool IsRectangularLight() const;

  MYON::coordinate_system CoordinateSystem() const; // determined by style

  /*
  Description:
    A light's location and direction can be defined with respect
    to world, camera, or view coordinates.  GetLightXform gets
    the transformation from the light's intrinsic coordinate
    system to the destination coordinate system specified
    by dest_cs.
  Parameters:
    vp - [in] viewport where light is being used
    dest_cs - [in] destination coordinate system
    xform - [out] transformation from the light's intrinsic
                  coordinate system to cs.
  Returns:
    true if successful.
  */
  bool GetLightXform( 
           const MYON_Viewport& vp,
           MYON::coordinate_system dest_cs, 
           MYON_Xform& xform 
           ) const;

  void SetLocation( const MYON_3dPoint& );
  void SetDirection( const MYON_3dVector& );

  MYON_3dPoint Location() const;
  MYON_3dVector Direction() const;
  MYON_3dVector PerpindicularDirection() const;

  double Intensity() const; // 0.0 = 0%  1.0 = 100%  Only clamped above zero - no maximum.
  void SetIntensity(double);

  double PowerWatts() const;
  double PowerLumens() const;
  double PowerCandela() const;
  
  void SetPowerWatts( double );
  void SetPowerLumens( double );
  void SetPowerCandela( double );

  /////////////////////////////////////////////////////////
  //
  // colors
  //
  void SetAmbient(  MYON_Color );
  void SetDiffuse(  MYON_Color );
  void SetSpecular( MYON_Color );
  MYON_Color Ambient() const;
  MYON_Color Diffuse() const;
  MYON_Color Specular() const;

  /////////////////////////////////////////////////////////
  //
  // attenuation settings (ignored for "directional" and "ambient" lights)
  // attenuation = 1/(a[0] + d*a[1] + d^2*a[2]) where d = distance to light
  //
  void SetAttenuation(double,double,double);
  void SetAttenuation(const MYON_3dVector&);
  MYON_3dVector Attenuation() const;
  double Attenuation(double) const; // computes 1/(a[0] + d*a[1] + d^2*a[2]) where d = argument
                                    // returns 0 if a[0] + d*a[1] + d^2*a[2] <= 0

  /////////////////////////////////////////////////////////
  //
  // spot light parameters (ignored for non-spot lights)
  //
  // angle = 0 to 90 degrees
  // exponent = 0 to 128 (0=uniform, 128=high focus)
  //
  void SetSpotAngleDegrees( double );
  double SpotAngleDegrees() const;

  void SetSpotAngleRadians( double );
  double SpotAngleRadians() const;

  //////////
  // The spot exponent varies from 0.0 to 128.0 and provides
  // an exponential interface for controlling the focus or 
  // concentration of a spotlight (like the 
  // OpenGL GL_SPOT_EXPONENT parameter).  The spot exponent
  // and hot spot parameters are linked; changing one will
  // change the other.
  // A hot spot setting of 0.0 corresponds to a spot exponent of 128.
  // A hot spot setting of 1.0 corresponds to a spot exponent of 0.0.
  void SetSpotExponent( double );
  double SpotExponent() const;

  //////////
  // The hot spot setting runs from 0.0 to 1.0 and is used to
  // provides a linear interface for controlling the focus or 
  // concentration of a spotlight.
  // A hot spot setting of 0.0 corresponds to a spot exponent of 128.
  // A hot spot setting of 1.0 corresponds to a spot exponent of 0.0.
  void SetHotSpot( double );
  double HotSpot() const;

  // The spotlight radii are useful for display UI.
  bool GetSpotLightRadii( double* inner_radius, double* outer_radius ) const;


  /////////////////////////////////////////////////////////
  //
  // linear and rectangular light parameters
  // (ignored for non-linear/rectangular lights)
  //
  void SetLength( const MYON_3dVector& );
  MYON_3dVector Length() const;

  void SetWidth( const MYON_3dVector& );
  MYON_3dVector Width() const;

  /////////////////////////////////////////////////////////
  //
  // shadow parameters (ignored for non-spot lights)
  //
  // shadow intensity 0.0 = does not cast any shadows
  //                  1.0 = casts black shadows
  //
  void SetShadowIntensity(double);
  double ShadowIntensity() const;
                                 

  /////////////////////////////////////////////////////////
  //
  // light index
  //
  void SetLightIndex( int );
  int LightIndex() const;

  /////////////////////////////////////////////////////////
  //
  // light name
  //
  void SetLightName( const char* );
  void SetLightName( const wchar_t* );
  const MYON_wString& LightName() const;

public:
  int           m_light_index;
  MYON_UUID       m_light_id;
  MYON_wString    m_light_name;

  bool                 m_bOn;   // true if light is on
  MYON::light_style      m_style; // style of light

  MYON_Color m_ambient;
  MYON_Color m_diffuse;
  MYON_Color m_specular;
  
  MYON_3dVector m_direction; // ignored for "point" and "ambient" lights
  MYON_3dPoint  m_location;  // ignored for "directional" and "ambient" lights
  MYON_3dVector m_length;    // only for linear and rectangular lights
                           // ends of linear lights are m_location and m_location+m_length
  MYON_3dVector m_width;     // only for rectangular lights
                           // corners of rectangular lights are m_location, m_location+m_length,
                           // m_location+m_width, m_location+m_width+m_length

  double      m_intensity; // Linear dimming/brightening factor: 0.0 = off, 1.0 = 100%.
                           // Values < 0.0 and values > 1.0 are permitted but are
                           // not consistently interpreted by various renderers.
                           // Renderers should clamp the range to [0.0, 1.0] if their
                           // lighting model does not support more exotic interpretations
                           // of m_intensity.
  double      m_watts;     // Used by lighting models that reference lighting fixtures.
                           // Values < 0.0 are invalid.  If m_watts is 0.0, the
                           // value is ignored.

  // spot settings - ignored for non-spot lights
  double       m_spot_angle;    // 0.0 to 90.0
  double       m_spot_exponent; // 0.0 to 128.0
                                // 0.0 = uniform
                                // 128.0 = high focus
  double       m_hotspot;       // 0.0 to 1.0 (See SetHotSpot() for details)

  // attenuation settings - ignored for "directional" and "ambient" lights
  MYON_3dVector m_attenuation;    // each entry >= 0.0
                                // att = 1/(a[0] + d*a[1] + d^2*a[2])
                                // where d = distance to light

  // shawdow casting
  double       m_shadow_intensity; // 0.0 = no shadow casting, 1.0 = full shadow casting
};



#endif

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

#if !defined(OPENNURBS_3DM_SETTINGS_INC_)
#define OPENNURBS_3DM_SETTINGS_INC_


///////////////////////////////////////////////////////////////////////
//
// units and tolerances
//

class MYON_CLASS MYON_3dmUnitsAndTolerances
{
public:
  // The default constructor set units to millimeters and tolerance = 0.001mm
  MYON_3dmUnitsAndTolerances() = default;
  ~MYON_3dmUnitsAndTolerances() = default;

  MYON_3dmUnitsAndTolerances(const MYON_3dmUnitsAndTolerances&) = default;
  MYON_3dmUnitsAndTolerances& operator=(const MYON_3dmUnitsAndTolerances&) = default;

  bool Read( MYON_BinaryArchive& );
  bool Write( MYON_BinaryArchive& ) const;

  void Dump( MYON_TextLog& ) const;

  bool IsValid() const;

  // Gets and sets the absolute tolerance in units, > 0.0
  double AbsoluteTolerance() const;
  void SetAbsoluteTolerance(double absolute_tolerance);

  // Gets and sets the angle tolerance in radians, > 0.0 and <= MYON_PI
  double AngleTolerance() const;
  void SetAngleTolerance(double angle_tolerance);

  // Gets and sets the relative tolerance, fraction > 0.0 and < 1.0
  double RelativeTolerance() const;
  void SetRelativeTolerance(double relative_tolerance);

  // Gets and sets the distance display mode
  MYON::OBSOLETE_DistanceDisplayMode DistanceDisplayMode() const;
  void SetDistanceDisplayMode(MYON::OBSOLETE_DistanceDisplayMode distance_display_mode);

  // Gets and sets the distance display precision, >= 0 and <= 7
  int DistanceDisplayPrecision() const;
  void SetDistanceDisplayPrecision(int distance_display_precision);

  // Returns true if absolute, angle, and relative tolerances values are valid.
  bool TolerancesAreValid() const;

  /*
  Description:
    If m_absolute_tolerance is not set to a valid value, it is set
    to MYON_3dmUnitsAndTolerances::DefaultValue.m_absolute_tolerance.
    If m_angle_tolerance is not set to a valid value, it is set
    to MYON_3dmUnitsAndTolerances::DefaultValue.m_angle_tolerance.
    If m_relative_tolerance is not set to a valid value, it is set
    to MYON_3dmUnitsAndTolerances::DefaultValue.m_relative_tolerance.
  Returns:
    0: all tolerances were valid
    0 != (rc & 1):
      m_absolute_tolerance was invalid and set to the default value
    0 != (rc & 2):
      m_angle_tolerance was invalid and set to the default value
    0 != (rc & 4):
      m_relative_tolerance was invalid and set to the default value
  */
  unsigned int SetInvalidTolerancesToDefaultValues();

  // Returns scale factor that needs to be applied to change from
  // the argument's unit system to m_unit_system.  
  // When m_unit_system is not MYON::LengthUnitSystem::CustomUnits,
  // Scale(us) = MYON::UnitScale(us,m_unit_system).  When Scale(us)
  // When m_unit_system is MYON::LengthUnitSystem::CustomUnits,
  // Scale(us) = MYON::UnitScale(us,MYON::LengthUnitSystem::Meters)*m_custom_unit_scale.
  double Scale( MYON::LengthUnitSystem ) const;

  // Expert access to member variables
  MYON_UnitSystem m_unit_system = MYON_UnitSystem::Millimeters;
  double m_absolute_tolerance = 0.001;    // in units > 0.0
  double m_angle_tolerance = MYON_PI/180.0; // in radians > 0.0 and <= MYON_PI
  double m_relative_tolerance = 0.01;     // fraction > 0.0 and < 1.0
  MYON::OBSOLETE_DistanceDisplayMode m_distance_display_mode = MYON::OBSOLETE_DistanceDisplayMode::Decimal; // decimal or fractional
  int m_distance_display_precision = 3;   // decimal mode: number of decimal places
                                          // fractional modes: denominator = (1/2)^m_distance_display_precision

public:
  /*
  DefaultValue
    m_unit_system                 MYON::LengthUnitSystem::Millimeters
    m_absolute_tolerance          0.001
    m_angle_tolerance             pi/180 = 1 degree
    m_relative_tolerance          0.01 = 1%
    m_distance_display_mode       MYON::OBSOLETE_DistanceDisplayMode::Decimal
    m_distance_display_precision  3
  */
  static const MYON_3dmUnitsAndTolerances Millimeters;
};

///////////////////////////////////////////////////////////////////////
//
// Model settings
//   render mesh defaults
//   viewports
//   construction planes
//

class MYON_CLASS MYON_3dmAnnotationSettings
{
public:
  MYON_3dmAnnotationSettings() = default;
  ~MYON_3dmAnnotationSettings() = default;
  MYON_3dmAnnotationSettings(const MYON_3dmAnnotationSettings&) = default;
  MYON_3dmAnnotationSettings& operator=(const MYON_3dmAnnotationSettings&) = default;

  static const MYON_3dmAnnotationSettings Default;

  bool Read( MYON_BinaryArchive& );
  bool Write( MYON_BinaryArchive& ) const;

  void Dump( MYON_TextLog& text_log ) const;

  // these are the running defaults for making dimensions
  // they are also the things written to the 3dm file as dimension settings
  double m_dimscale = 1.0;       // model size / plotted size
  double m_textheight = 1.0;
  double m_dimexe = 1.0;
  double m_dimexo = 1.0;
  double m_arrowlength = 1.0;
  double m_arrowwidth = 1.0;
  double m_centermark = 1.0;

  /*
  Returns:
    Value of m_world_view_text_scale;
  */
  double WorldViewTextScale() const;

  /*
  Parameters:
    world_view_text_scale - [in]
      Sets value of m_world_view_text_scale.
  */
  void SetWorldViewTextScale(double world_view_text_scale );

  /*
  Returns:
    Value of m_world_view_hatch_scale;
  */
  double WorldViewHatchScale() const;

  /*
  Parameters:
    world_view_hatch_scale - [in]
      Sets value of m_world_view_hatch_scale.
  */
  void SetWorldViewHatchScale(double world_view_hatch_scale );


  /*
  Returns:
  Value of m_b_V5_EnableAnnotationScaling;
  */
  bool Is_V5_AnnotationScalingEnabled() const;

  /*
  Parameters:
  bEnable - [in]
  Sets value of m_b_V5_EnableAnnotationScaling.
  */
  void Enable_V5_AnnotationScaling(bool bEnable);

  /*
  Parameters:
  bEnable - [in]
  Sets value of m_bEnableModelSpaceAnnotationScaling.
  */
  void EnableModelSpaceAnnotationScaling(bool bEnable);

  /*
  Returns:
  Value of m_bEnableModelSpaceAnnotationScaling;
  */
  bool IsModelSpaceAnnotationScalingEnabled() const;

  /*
  Parameters:
  bEnable - [in]
  Sets value of m_bEnableLayoutSpaceAnnotationScaling.
  */
  void EnableLayoutSpaceAnnotationScaling(bool bEnable);

  /*
  Returns:
  Value of m_bEnableLayoutSpaceAnnotationScaling;
  */
  bool IsLayoutSpaceAnnotationScalingEnabled() const;

  /*
  Returns:
    Value of m_bEnableHatchScaling;
  */
  bool IsHatchScalingEnabled() const;

  /*
  Parameters:
    bEnable - [in]
      Sets value of m_bEnableHatchScaling.
  */
  void EnableHatchScaling( bool bEnable );

  // Present but not used in V4 or V5 - removed 5 August 2010 to make room
  // for m_world_view_text_scale and m_bEnableAnnotationScaling
  //// added 12/28/05 LW
  //double m_dimdle;
  //double m_dimgap;
private:
  // If m_bEnableAnnotationScaling is true,
  // and MYON_OBSOLETE_V5_Annotation::m_annotative_scale is true,
  // and MYON_OBSOLETE_V5_Annotation::m_type == MYON::dtTextBlock,
  // and the text object is being displayed in a world
  // view (not a detail view and not a page view),
  // then the text will be scaled by m_world_view_text_scale.
  // The default is 1.0. Values <= 0.0 are not valid.
  float m_world_view_text_scale = 1.0f;
  float m_world_view_hatch_scale = 1.0f;
  
private:
  // If m_bEnableAnnotationScaling is false:
  //   * m_world_view_text_scale is ignored.
  //   * text is not scaled.
  //   * MYON_DimStyle::DimScale() determines the scale 
  //     applied to all other annotation objects in all 
  //     types of views.
  //   * The value of MYON_DetailView::m_page_per_model_ratio
  //     is applied to all objects (annotation and geometry)
  //     in the detail view.
  //
  // If m_bEnableAnnotationScaling is true:
  //   * m_world_view_text_scale is used as described above.
  //   * MYON_DimStyle::DimScale() determines the scale 
  //     applied to all non text annotation objects in 
  //     world views. 
  //   * MYON_DimStyle::DimScale() is ignored in page and 
  //     detail views. 
  //   * MYON_DetailView::m_page_per_model_ratio is ignored
  //     for annotation objects in detail views, other
  //     geometry is scaled.
  //
  // Default is true.
  unsigned char m_b_V5_EnableAnnotationScaling = 1;

  // [Lowell 3-28-2013] New fields for V6
  unsigned char m_bEnableModelSpaceAnnotationScaling = 1;
  unsigned char m_bEnableLayoutSpaceAnnotationScaling = 1;

  unsigned char m_bEnableHatchScaling = 1;

private:
  MYON__UINT32 m_reserved1 = 0;
  MYON__UINT8 m_reserved2 = 0;
  MYON__UINT8 m_reserved3 = 0;
  MYON__UINT8 m_reserved4 = 0;

public:
  MYON::LengthUnitSystem m_dimunits = MYON::LengthUnitSystem::None;  // units used to measure the dimension
  int m_arrowtype = 0;     // 0: filled narrow triangular arrow (= ((MYON_Arrowhead::arrow_type enum value as int ) - 2))
  int m_angularunits = 0;  // 0: degrees, 1: radians
  int m_lengthformat = 0;  // 2 = MYON_DimStyle::LengthDisplay::FeetAndInches, treat everything else as MYON_DimStyle::LengthDisplay::ModelUnits
  int m_angleformat = 0;   // 0: decimal degrees, ... ( MYON_DimStyle::angle_format enum as int )

  //MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode m_settings_textalign;     // In V2 files -  0: above line, 1: in line, 2: horizontal
  //                              // After V2 files - 0: normal (converts to above_line), 1: horizontal, 2: above_line, 3: in_line 

  int m_resolution = 0;    // depends on m_lengthformat
                       // for decimal, digits past the decimal point

  MYON_wString m_facename; // [LF_FACESIZE] // windows font name
};

//////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_3dmConstructionPlaneGridDefaults
//
// Default settings used for construction plane grids
class MYON_CLASS MYON_3dmConstructionPlaneGridDefaults
{
public:
  MYON_3dmConstructionPlaneGridDefaults() = default;
  ~MYON_3dmConstructionPlaneGridDefaults() = default;
  MYON_3dmConstructionPlaneGridDefaults(const MYON_3dmConstructionPlaneGridDefaults&) = default;
  MYON_3dmConstructionPlaneGridDefaults& operator=(const MYON_3dmConstructionPlaneGridDefaults&) = default;

  static const MYON_3dmConstructionPlaneGridDefaults Default;

  bool Write( MYON_BinaryArchive& ) const;
  bool Read( MYON_BinaryArchive& );

  void Dump( MYON_TextLog& text_log ) const;

	double m_grid_spacing = 1.0;   // distance between grid lines
  double m_snap_spacing = 1.0;   // when "grid snap" is enabled, the
                           // distance between snap points.  Typically
                           // this is the same distance as grid spacing.
	int m_grid_line_count = 70;   // number of grid lines in each direction
  int m_grid_thick_frequency = 5; // thick line frequency
                            // 0: none, 
                            // 1: all lines are thick, 
                            // 2: every other is thick, ...

  bool m_bShowGrid = true;
  bool m_bShowGridAxes = true;
  bool m_bShowWorldAxes = true;
};

//////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_3dmConstructionPlane
//
class MYON_CLASS MYON_3dmConstructionPlane
{
public:
  MYON_3dmConstructionPlane();
  ~MYON_3dmConstructionPlane();

  MYON_3dmConstructionPlane(const MYON_Plane& plane);

  // default copy constructor and operator= work fine
  //MYON_3dmConstructionPlane(const MYON_3dmConstructionPlane&);
  //MYON_3dmConstructionPlane& operator=(const MYON_3dmConstructionPlane&);

  void Default();

  bool Write( MYON_BinaryArchive& ) const;
  bool Read( MYON_BinaryArchive& );

  void Dump( MYON_TextLog& text_log ) const;

  MYON_Plane    m_plane;

  // construction grid appearance
	double m_grid_spacing;   // distance between grid lines
  double m_snap_spacing;   // when "grid snap" is enabled, the
                           // distance between snap points.  Typically
                           // this is the same distance as grid spacing.
	int m_grid_line_count;   // number of grid lines in each direction
  int m_grid_thick_frequency; // thick line frequency
                            // 0: none, 
                            // 1: all lines are thick, 
                            // 2: every other is thick, ...
  bool m_bDepthBuffer; // false=grid is always drawn behind 3d geometry
                       // true=grid is drawn at its depth as a 3d plane
                       // and grid lines obscure things behind the grid.

  MYON_wString  m_name;
};

#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_ClassArray<MYON_3dmConstructionPlane>;
#endif

//////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_3dmViewPosition
//
class MYON_CLASS MYON_3dmViewPosition
{
public:
  // view window relative position and state in parent frame
  MYON_3dmViewPosition();
  ~MYON_3dmViewPosition();
  MYON_3dmViewPosition(const MYON_3dmViewPosition&);
  MYON_3dmViewPosition& operator=(const MYON_3dmViewPosition&);

  void Default();

  bool Write( MYON_BinaryArchive& ) const;
  bool Read( MYON_BinaryArchive& );

  // relative position of view window in main frame
  // if m_floating_viewport>0, this is relative position of the view window
  // on the virtual screen (union of potentially multiple monitors)
  double m_wnd_left;    // 0.0 to 1.0
  double m_wnd_right;
  double m_wnd_top;
  double m_wnd_bottom;

  bool m_bMaximized; // true if view window is maximized

  // m_floating_viewport is used to track floating viewport information.
  //  0 = the view is docked in the main application window.
  // >0 = the view is floating. When floating, this corresponds to the
  //      number of monitors on on the user's computer when the file was saved
  unsigned char m_floating_viewport;
private:
  // reserved for future use
  unsigned char m_reserved_1;
  unsigned char m_reserved_2;
  unsigned char m_reserved_3;
};

//////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_3dmViewTraceImage
//
class MYON_CLASS MYON_3dmViewTraceImage
{
public:
  MYON_3dmViewTraceImage();
  ~MYON_3dmViewTraceImage();
  bool operator==( const MYON_3dmViewTraceImage& ) const;
  bool operator!=( const MYON_3dmViewTraceImage& ) const;

  void Default();

  bool Write( MYON_BinaryArchive& ) const;
  bool Read( MYON_BinaryArchive& );

  // view window relative position and state in parent frame
  MYON_Plane m_plane;
  double   m_width;
  double   m_height;

  MYON_FileReference m_image_file_reference;

  bool m_bGrayScale; // true if image should be black and white
  bool m_bHidden;    // true if image is currently hidden from view
  bool m_bFiltered;  // true if image should be filtered (bilinear) before displayed.
};


//////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_3dmViewTraceImage
//
class MYON_CLASS MYON_3dmWallpaperImage
{
public:
  MYON_3dmWallpaperImage();
  ~MYON_3dmWallpaperImage();
  bool operator==( const MYON_3dmWallpaperImage& ) const;
  bool operator!=( const MYON_3dmWallpaperImage& ) const;

  void Default();

  bool Write( MYON_BinaryArchive& ) const;
  bool Read( MYON_BinaryArchive& );

  MYON_FileReference m_image_file_reference;

  bool m_bGrayScale; // true if image should be black and white
  bool m_bHidden;    // true if image is currently hidden from view
};

//////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_3dmView
//

class MYON_CLASS MYON_3dmPageSettings
{
public:
  MYON_3dmPageSettings();
  ~MYON_3dmPageSettings();

  bool IsValid( MYON_TextLog* text_log = 0 ) const;

  void Default();

  int m_page_number;

  // Overall size of the page in millimeters
  double m_width_mm;
  double m_height_mm;

  // Page margins in millimeters
  double m_left_margin_mm;
  double m_right_margin_mm;
  double m_top_margin_mm;
  double m_bottom_margin_mm;

  MYON_wString m_printer_name;

  bool Write(MYON_BinaryArchive& archive) const;
  bool Read(MYON_BinaryArchive& archive);
};

class MYON_CLASS MYON_StandardDisplayModeId
{
public:
  static const MYON_UUID Wireframe;        // {1311ADCB-D89E-4051-A3F0-F64441FB8EC6}
  static const MYON_UUID Shaded;           // {8BC8DEBE-C83B-4c47-B13C-9DB074510CAC}
  static const MYON_UUID Rendered;         // {CAE60BAE-2D51-4299-ABF7-A339FCA86F3B}
  static const MYON_UUID Ghosted;          // {FF608B97-81D3-4186-831C-41F7DC140881}
  static const MYON_UUID XrayShade;        // {B5C19D5D-0AEC-4ff7-A10E-E052E660263A}
  static const MYON_UUID RenderedShadows;  // {A5545314-9D87-428d-95AE-91052EEAD0FA}
  static const MYON_UUID Technical;        // {63612C72-778F-4afd-B81B-17426FDFE8A6}
  static const MYON_UUID Artistic;         // {B46AB226-05A0-4568-B454-4B1AB721C675}
  static const MYON_UUID Pen;              // {F4616FA5-A831-4620-A97E-9B807D5EC376}
  static const MYON_UUID AmbientOcclusion; // {C32B72C3-41BD-4ADC-82A8-B7AEF4456A37}
  static const MYON_UUID Raytraced;        // {69E0C7A5-1C6A-46C8-B98B-8779686CD181}

  /*
  Parameters:
    id - [in]
  Returns:
    True if id is one of the standard display modes listed above.
  */
  static bool IsStandardDisplayModeId(
    MYON_UUID id
    );

  /*
  Parameters:
    id - [in]
  Returns:
    The legacy V3 display mode enum that is the closest match to
    the display mode id.
  */
  static MYON::v3_display_mode ToV3DisplayMode(
    MYON_UUID id
    );

  /*
  Parameters:
    dm - [in]
      v3 display mode enum value
  Returns:
    display mode id that corresponds to the enum value.
  */
  static MYON_UUID FromV3DisplayMode(
    MYON::v3_display_mode dm
    );


private:
  // prohibit instantiation
  MYON_StandardDisplayModeId(); // no implementation
  ~MYON_StandardDisplayModeId(); // no implementation
};

enum class MYON_FocalBlurModes : unsigned int
{
    None,      // No focal blur.
    Automatic, // Autofocus on selected objects.
    Manual,    // Fully manual focus.
};

class MYON_CLASS MYON_3dmView
{
public:
  MYON_3dmView();
  ~MYON_3dmView();

  // The C++ default copy constructor and operator= work fine.
  // Do not provide customized versions.
  // NO // MYON_3dmView(const MYON_3dmView&);
  // NO // MYON_3dmView& operator=(const MYON_3dmView&);

  void Default();

  bool Write( MYON_BinaryArchive& ) const;
  bool Read( MYON_BinaryArchive& );

  void Dump( MYON_TextLog& text_log ) const;

  bool IsValid( MYON_TextLog* text_log = 0 ) const;

  // view projection information
  MYON_Viewport m_vp;

  // clipping planes
  // Prior to Dec 14, 2010 m_clipping_planes was not saved with the view. 
  // After Dec 14, 2010 m_clipping_planes is saved.
  MYON_SimpleArray<MYON_ClippingPlaneInfo> m_clipping_planes;

  // If true, the the camera location, camera direction,
  // and lens angle should not be changed.
  // It is ok to adjust clipping planes.
  bool m_bLockedProjection;

  MYON::ViewSectionBehavior SectionBehavior() const;
  void SetSectionBehavior(MYON::ViewSectionBehavior behavior);
private:
  MYON::ViewSectionBehavior m_section_behavior = MYON::ViewSectionBehavior::ClipAndSection;
  unsigned char m_reserved[6];
public:
  ///////////////////////////////////////////////////////////////////////
  //
  // target point
  //

  /*
  Returns:
    Target point.  This point is saved on m_vp.m_target_point.
    The default constructor sets the target point to 
    MYON_3dPoint::UnsetPoint. You must explicitly set the target
    point if you want to use it.
  Remarks:
    The target point is stored on m_vp.m_target_point.  The
    value MYON_3dmView.m_target is obsolete. This function always
    returns the value of m_vp.m_target_point.

  */
  MYON_3dPoint TargetPoint() const;

  /*
  Description:
    Sets the viewport target point. 
  Parameters:
    target_point - [in]
      When in doubt, the point m_vp.FrustumCenterPoint(MYON_UNSET_VALUE)
      is a good choice.
  Remarks:
    This point is saved on m_vp.m_target_point.
  */
  bool SetTargetPoint(MYON_3dPoint target_point);

  //
  ///////////////////////////////////////////////////////////////////////

  MYON_wString  m_name; // name on window
  
  // The value of m_display_mode_id can be one of the "standard" ids
  // from MYON_StandardDisplayModeId, nil, or a custom display mode
  // settings on a particular computer.  If you encounter a nil id
  // or any other id that is not one of the "standard" display mode
  // ids, then your application should use a default display mode,
  // typically either wireframe or shaded, that is appropriate for
  // general model viewing.  The function MYON::RhinoV3DisplayMode(id)
  // will convert a display mode id into a legacy Rhino V3 display
  // mode enum value.
  MYON_UUID m_display_mode_id;

  // position of view in parent window 
  // (relative display device coordinates)
  MYON_3dmViewPosition m_position;

  MYON::view_type m_view_type; // model, page, nested or uveditor

  // If m_view_type == MYON::page_view_type, then the m_page_settings
  // records the page size.  Otherwise, m_page_settings should
  // be ignored.
  MYON_3dmPageSettings m_page_settings;

  ///////////////////////////////////////////////////////////////////////
  //
  // Named view information
  //
  // If this view was created from a named view, then m_named_view_id 
  // identifies the named view.
  //
  // The named views are MYON_3dmView classes saved in MYON_3dmSettings.m_named_views[].
  // A named view's id is the value returned by MYON_3dmView.m_vp.ViewportId()
  // A named view's name is the value returned by MYON_3dmView.m_name
  //
  // If this view is a named view, then m_named_view_id should be equal to
  // m_vp.m_viewport_id.
  //
  // If this view is not a named view and not created from a named view,
  // then m_named_view_id is equal to MYON_nil_uuid.
  MYON_UUID m_named_view_id; 

  ///////////////////////////////////////////////////////////////////////
  //
  // Construction plane
  //
  MYON_3dmConstructionPlane m_cplane;
  bool m_bShowConstructionGrid;
  bool m_bShowConstructionAxes;
  bool m_bShowConstructionZAxis;

  // world axes icon
  bool m_bShowWorldAxes;

  // tracing image
  MYON_3dmViewTraceImage m_trace_image;

  // wallpaper image
  MYON_3dmWallpaperImage m_wallpaper_image;

public:

  double FocalBlurDistance(void) const;
  void SetFocalBlurDistance(double d);

  double FocalBlurAperture(void) const;
  void SetFocalBlurAperture(double d);

  double FocalBlurJitter(void) const;
  void SetFocalBlurJitter(double d);

  unsigned int FocalBlurSampleCount(void) const;
  void SetFocalBlurSampleCount(unsigned int count);

  MYON_FocalBlurModes FocalBlurMode(void) const;
  void SetFocalBlurMode(MYON_FocalBlurModes m);

  MYON_2iSize RenderingSize() const;
  void SetRenderingSize(const MYON_2iSize& size);

  //Focal blur settings - per view for renderers.
private:
  double m_dFocalBlurDistance = 100.0;
  double m_dFocalBlurAperture = 64.0;
  double m_dFocalBlurJitter = 0.1;
  unsigned int m_uFocalBlurSampleCount = 10;
  MYON_FocalBlurModes m_FocalBlurMode = MYON_FocalBlurModes::None;
  MYON_2iSize m_sizeRendering = MYON_2iSize(640, 480);

private:
  MYON__INT_PTR reserved = 0;
};

#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_ClassArray<MYON_3dmView>;
#endif

//////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_3dmRenderSettings
//

class MYON_CLASS MYON_3dmRenderSettings : public MYON_Object
{
  MYON_OBJECT_DECLARE(MYON_3dmRenderSettings);

public:
  MYON_3dmRenderSettings() = default;
  virtual ~MYON_3dmRenderSettings();

  MYON_3dmRenderSettings(const MYON_3dmRenderSettings&);
  MYON_3dmRenderSettings& operator=(const MYON_3dmRenderSettings&);

  static const MYON_3dmRenderSettings Default;

  bool Write( MYON_BinaryArchive& ) const override;
  bool Read( MYON_BinaryArchive& ) override;
  void Dump( MYON_TextLog& text_log ) const override;

private:
  static bool UseV5ReadWrite(const MYON_BinaryArchive&);
  bool WriteV5( MYON_BinaryArchive& ) const;
  bool ReadV5( MYON_BinaryArchive& );

public:
  //New for V6, rendering source (render directly from a NamedView or Snapshot)
  //https://mcneel.myjetbrains.com/youtrack/issue/RH-39593
  enum class RenderingSources : unsigned int
  {
    ActiveViewport,      // Get the rendering view from the currently active viewport (as in all previous versions of Rhino)
    SpecificViewport,    // Get the rendering view from the named viewport (see NamedViewport below)
    NamedView,           // Get the rendering view from a specific named view (see NamedView below)
    SnapShot,            // Before rendering, restore the Snapshot specified in Snapshot below, then render.
  };

  RenderingSources RenderingSource(void) const;
  void SetRenderingSource(RenderingSources);

  MYON_wString SpecificViewport(void) const;
  void SetSpecificViewport(const MYON_wString&);

  MYON_wString NamedView(void) const;
  void SetNamedView(const MYON_wString&);

  MYON_wString Snapshot(void) const;
  void SetSnapshot(const MYON_wString&);

private:
  RenderingSources m_rendering_source = RenderingSources::ActiveViewport;
  MYON_wString m_specific_viewport;
  MYON_wString m_named_view;
  MYON_wString m_snapshot;

public:
  bool ScaleBackgroundToFit() const;
  void SetScaleBackgroundToFit( bool bScaleBackgroundToFit );

public:
  // Access to Dithering information.
  class MYON_Dithering& Dithering(void) const;

  // Access to Ground Plane information.
  class MYON_GroundPlane& GroundPlane(void) const;

  // Access to Linear Workflow information.
  class MYON_LinearWorkflow& LinearWorkflow(void) const;

  // Access to Render Channels information.
  class MYON_RenderChannels& RenderChannels(void) const;

  // Access to Safe Frame information.
  class MYON_SafeFrame& SafeFrame(void) const;

  // Access to Skylight information.
  class MYON_Skylight& Skylight(void) const;

  // Access to Sun information.
  class MYON_Sun& Sun(void) const;

  // Access to background rendering environment information.
  MYON_UUID BackgroundRenderEnvironment(void) const;
  void SetBackgroundRenderEnvironment(const MYON_UUID& id);

  // Access to skylighting rendering environment information.
  bool SkylightingRenderEnvironmentOverride(void) const;
  void SetSkylightingRenderEnvironmentOverride(bool on);
  MYON_UUID SkylightingRenderEnvironment(void) const;
  void SetSkylightingRenderEnvironment(const MYON_UUID& id);

  // Access to reflection / refraction rendering environment information.
  bool ReflectionRenderEnvironmentOverride(void) const;
  void SetReflectionRenderEnvironmentOverride(bool on);
  MYON_UUID ReflectionRenderEnvironment(void) const;
  void SetReflectionRenderEnvironment(const MYON_UUID& id);

  // Access to rendering presets.
  MYON_UUID CurrentRenderingPreset(void) const;
  void SetCurrentRenderingPreset(const MYON_UUID& uuid);
  void GetRenderingPresets(MYON_SimpleArray<MYON_UUID>& presets) const;

private:
  unsigned short m_reserved1 = 0;

public:
  //////////////////////////////////////////////////////////////
  //
  // Force viewport aspect ratio:
  //   If m_bCustomImageSize is true and m_bForceViewportAspectRatio is true
  //   then the image height should be calculated by multiplying the m_image_width
  //   by the viewport aspect ratio.  Note that this might be affected by m_rendering_source
  //   In this case, m_image_height should not be used.
  //
  bool m_bForceViewportAspectRatio = false;

  //////////////////////////////////////////////////////////////
  //
  // Custom image size:
  //   If m_bCustomImageSize is true, then the image pixel size
  //   is m_image_width X m_image_height pixels.
  //   If m_bCustomImageSize is false, then the image pixel size
  //   is the size of the viewport being rendered.
  //
  bool m_bCustomImageSize = false;
  int  m_image_width  = 800;  // image width in pixels
  int  m_image_height = 600;  // image height in pixels

private:
  unsigned int m_reserved3 = 0;

public:
  ////////
  // Number of dots/inch (dots=pixels) to use when printing and 
  // saving bitmaps. The default is 72.0 dots/inch.
  double m_image_dpi = 72.0; 

  //////////
  // unit system to use when converting image pixel size and dpi
  // information into a print size.  Default = inches
  MYON::LengthUnitSystem m_image_us = MYON::LengthUnitSystem::Inches;

  MYON_Color m_ambient_light = MYON_Color::Black;

  int m_background_style = 0; // 0 = solid color, 1 = "wallpaper" image, 2 = Gradient, 3 = Environment

  // m_background_color was changed from MYON_Color::Gray160 to MYON_Color::White for "white studio" look.
  // m_background_color = Top color of gradient...
  MYON_Color m_background_color = MYON_Color::White;
  MYON_Color m_background_bottom_color = MYON_Color::Gray160;

  MYON_wString m_background_bitmap_filename;
  // If m_background_bitmap_filename is not empty, the file cannot be found,
  // and m_embedded_file_id identifes an embedded image file in the model, 
  // then that file will be used as the background bitmap.
  MYON_UUID m_embedded_image_file_id = MYON_nil_uuid;

  bool m_bUseHiddenLights = false;

  bool m_bDepthCue = false;
  bool m_bFlatShade = false;

  bool m_bRenderBackfaces = true;
  bool m_bRenderPoints = false;
  bool m_bRenderCurves = false;
  bool m_bRenderIsoparams = false;
  bool m_bRenderMeshEdges = false;
  bool m_bRenderAnnotation = false;
  bool m_bScaleBackgroundToFit = false;
  bool m_bTransparentBackground = false;

private:
  unsigned char m_reserved4 = 0;
  unsigned int  m_reserved5 = 0;
public:
  
  int m_antialias_style = 1; // 0 = none, 1 = normal, 2 = medium, 3 = best

  int m_shadowmap_style = 1; // 0 = none, 1 = normal, 2 = best
  int m_shadowmap_width= 1000;
  int m_shadowmap_height = 1000;
  double m_shadowmap_offset = 0.75;

  // Flags that are used to determine which render settings a render
  // plugin uses, and which ones the display pipeline should use.
  // Note: Render plugins set these, and they don't need to persist
  //       in the document...Also, when set, they turn OFF their
  //       corresponding setting in the Display Attributes Manager's
  //       UI pages for "Rendered" mode.
  bool m_bUsesAmbientAttr = true;
  bool m_bUsesBackgroundAttr = true;
  bool m_bUsesBackfaceAttr = false;
  bool m_bUsesPointsAttr = false;
  bool m_bUsesCurvesAttr = true;
  bool m_bUsesIsoparmsAttr = true;
  bool m_bUsesMeshEdgesAttr = false;
  bool m_bUsesAnnotationAttr = true;
  bool m_bUsesHiddenLightsAttr = true;

private:
  unsigned char  m_reserved6 = 0;
  unsigned short m_reserved7 = 0;
  unsigned short m_reserved8 = 0;

public: // For internal use only.
  MYON_XMLNode& RdkDocNode(void) const;

private:
  friend class MYON_3dmRenderSettingsPrivate;
  mutable class MYON_3dmRenderSettingsPrivate* m_private = nullptr;
};


//////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_EarthAnchorPoint
//

class MYON_CLASS MYON_EarthAnchorPoint
{
public:
  MYON_EarthAnchorPoint() = default;
  ~MYON_EarthAnchorPoint() = default;
  MYON_EarthAnchorPoint(const MYON_EarthAnchorPoint&) = default;
  MYON_EarthAnchorPoint& operator=(const MYON_EarthAnchorPoint&) = default;

  // Latitude, longitude, and elevation are MYON_UNSET_VALUE.
  static const MYON_EarthAnchorPoint Unset;

  // Latitude, longitude, and elevation are the Seattle Space Needle. 
  static const MYON_EarthAnchorPoint SeattleSpaceNeedle;

  static
  int Compare(
          const MYON_EarthAnchorPoint*, 
          const MYON_EarthAnchorPoint*
          );

  static
  int CompareEarthLocation(
          const MYON_EarthAnchorPoint*, 
          const MYON_EarthAnchorPoint*
          );

  static
  int CompareModelDirection(
          const MYON_EarthAnchorPoint*, 
          const MYON_EarthAnchorPoint*
          );

  static
  int CompareIdentification(
          const MYON_EarthAnchorPoint*, 
          const MYON_EarthAnchorPoint*
          );

  bool Read( MYON_BinaryArchive& );
  bool Write( MYON_BinaryArchive& ) const;

  /*
  Returns:
    True if the latitude, longitude, and elevation are set.
  */
  bool EarthLocationIsSet() const;

  /*
  Returns:
    True if model basepoint, north and east are set.
  */
  bool ModelLocationIsSet() const;

  /*
  Parameters:
    elevation_unit_system - [in]
    elevation - [in]      
  */
  void SetEarthLocation(
    MYON::EarthCoordinateSystem earth_coordinate_system,
    const class MYON_UnitSystem& elevation_unit_system,
    double latitude_degrees,
    double longitude_degrees,
    double elevation
  );

  void SetEarthLocation(
    MYON::EarthCoordinateSystem earth_coordinate_system,
    MYON::LengthUnitSystem elevation_unit_system,
    double latitude_degrees,
    double longitude_degrees,
    double elevation
  );

  void SetLatitudeAndLongitude(
    double latitude_degrees,
    double longitude_degrees
  );

  /*
  Returns:
    A 3d point with coordinates (latitude in degrees, longitude in degrees, elevation in meters).
  Remarks:
    Some coordinates may be MYON_UNSET_VALUE.
  */
  MYON_3dPoint EarthLocation() const;

  /*
  Parameters:
    unset_location - [in]
      Location to return if EarlocationIsSet() is false.
  Returns:
    A 3d point with coordinates (latitude, longitude, elevation).
  */
  MYON_3dPoint EarthLocation(
    MYON_3dPoint unset_location
  ) const;

  /*
  Returns:
    Earth location latitude in degrees. Can be MYON_UNSET_VALUE
  */
  double Latitude() const;

  /*
  Parameters:
    unset_latitude - [in]
      Value to return if the Earth location latitude is not set.
  Returns:
    Earth location latitude in degrees.
  */
  double Latitude(
    double unset_latitude
  ) const;

  void SetLatitude(
    double latitude_degrees
  );

  /*
  Returns:
    Earth location longitude in degrees. Can be MYON_UNSET_VALUE
  */
  double Longitude() const;

  /*
  Parameters:
    unset_longitude - [in]
      Value to return if the Earth location latitude is not set.
  Returns:
    Earth location longitude in degrees.
  */
  double Longitude(
    double unset_longitude
  ) const;
  
  void SetLongitude(
    double longitude_degrees
  );

  /*
    System used to define latiude, longitude and elevation.
  */
  MYON::EarthCoordinateSystem EarthCoordinateSystem() const;

  /*
    System used to define Earth latiude, longitude, and elevation coordinates.
  */
  void SetEarthCoordinateSystem(
    MYON::EarthCoordinateSystem earth_coordinate_system
  );

  double ElevationInMeters() const;


  /*
  Parameters:
    elevation_unit_system - [in]
      length unit system for returned value.
  Returns:
    Earth location elevation in in elevation_unit_system.
    The value is with
    Can be MYON_UNSET_VALUE
  */
  double Elevation(
    const class MYON_UnitSystem& elevation_unit_system
    ) const;

  /*
  Parameters:
    elevation_unit_system - [in]
      length unit system for returned value.
  Returns:
    Earth location elevation in degrees. Can be MYON_UNSET_VALUE
  */
  double Elevation(
    MYON::LengthUnitSystem elevation_unit_system
    ) const;

  /*
  Parameters:
    elevation_unit_system - [in]
      length unit system for returned value.
    unset_elevation - [in]
      Value to return if the Earth location elevation is not set.
  */
  double Elevation(
    const class MYON_UnitSystem& elevation_unit_system,
    double unset_elevation
  ) const;

  /*
  Parameters:
    elevation_unit_system - [in]
      length unit system for returned value.
    unset_elevation - [in]
      Value to return if the Earth location elevation is not set.
  */
  double Elevation(
    MYON::LengthUnitSystem elevation_unit_system,
    double unset_elevation
  ) const;

  /*
  Parameters:
    elevation_unit_system - [in]
    elevation - [in]      
  */
  void SetElevation(
    const MYON_UnitSystem& elevation_unit_system,
    double elevation
  );

  void SetElevation(
    MYON::LengthUnitSystem elevation_unit_system,
    double elevation
  );

  const MYON_3dPoint& ModelPoint() const;
  const MYON_3dVector& ModelNorth() const;
  const MYON_3dVector& ModelEast() const;

  void SetModelPoint(
    MYON_3dPoint model_point
  );

  void SetModelNorth(
    MYON_3dVector model_north
  );

  void SetModelEast(
    MYON_3dVector model_east
  );

  void SetModelLocation(
    MYON_3dPoint model_point,
    MYON_3dVector model_north,
    MYON_3dVector model_east
    );

  /*
  Description:
    Find the Keyhole Markup Language (KML) orientation angles (in radians) of a rotation
    transformation that maps model (east,north,up) to ((1,0,0),(0,1,0),(0,0,1)).
    KML Earth Z axis = up, KML Earth X axis = east, KML Earth Y axis = north.
    NOTE WELL: In KML, positive rotations are CLOCKWISE looking down
    specified axis vector towards the origin. This is rotation direction
    is opposite the conventional "right hand rule."
  Parameters:
    heading_radians - [out]
      angle (in radians) of rotation around KML Earth Z axis (Earth up).
      NOTE WELL: In KML, positive rotations are CLOCKWISE looking down
      specified axis vector towards the origin. This is rotation direction
      is opposite the conventional "right hand rule."
    tilt_radians - [out]
      angle (in radians) of rotation around KML Earth X axis (Earth east).
      NOTE WELL: In KML, positive rotations are CLOCKWISE looking down
      specified axis vector towards the origin. This is rotation direction
      is opposite the conventional "right hand rule."
    roll_radians - [out]
      angle (in radians) of rotation around KML Earth Y axis (Earth north).
      NOTE WELL: In KML, positive rotations are CLOCKWISE looking down
      specified axis vector towards the origin. This is rotation direction
      is opposite the conventional "right hand rule."
  Returns:
    True if the model location is set (this->ModelLocationIsSet() is true)
    and the KML orientation angles are returned.
    Otherwise false is returned and all of the angle values are MYON_DLB_QNAN.
  See Also:
    https://developers.google.com/kml/documentation/kmlreference#orientation
  */
  bool GetKMLOrientationAnglesRadians(
    double& heading_radians, 
    double& tilt_radians, 
    double& roll_radians
  ) const;

  /*
    If the model location is set (this->ModelLocationIsSet() is true), then the
    Keyhole Markup Language orientation heading angle in radians is returned.
    Otherwise MYON_DBL_QNAN is returned.
  See Also:
    https://developers.google.com/kml/documentation/kmlreference#orientation
  */
  const double KMLOrientationHeadingAngleRadians() const;

  /*
  Returns:
    If the model location is set (this->ModelLocationIsSet() is true), then the
    Keyhole Markup Language orientation tilt angle in radians is returned.
    Otherwise MYON_DBL_QNAN is returned.
  See Also:
    https://developers.google.com/kml/documentation/kmlreference#orientation
  */
  const double KMLOrientationTiltAngleRadians() const;

  /*
  Returns:
    If the model location is set (this->ModelLocationIsSet() is true), then the
    Keyhole Markup Language orientation roll angle in radians is returned.
    Otherwise MYON_DBL_QNAN is returned.
  See Also:
    https://developers.google.com/kml/documentation/kmlreference#orientation
  */
  const double KMLOrientationRollAngleRadians() const;

  /*
  Description:
    Find the Keyhole Markup Language (KML) orientation angles (in degrees) of a rotation
    transformation that maps model (east,north,up) to ((1,0,0),(0,1,0),(0,0,1)).
    KML Earth Z axis = up, KML Earth X axis = east, KML Earth Y axis = north.
    NOTE WELL: In KML, positive rotations are CLOCKWISE looking down
    specified axis vector towards the origin. This is rotation direction
    is opposite the conventional "right hand rule."
  Parameters:
    heading_degrees - [out]
      angle (in degrees) of rotation around KML Earth Z axis (Earth up).
      NOTE WELL: In KML, positive rotations are CLOCKWISE looking down
      specified axis vector towards the origin. This is rotation direction
      is opposite the conventional "right hand rule."
    tilt_degrees - [out]
      angle (in degrees) of rotation around KML Earth X axis (Earth east).
      NOTE WELL: In KML, positive rotations are CLOCKWISE looking down
      specified axis vector towards the origin. This is rotation direction
      is opposite the conventional "right hand rule."
    roll_degrees - [out]
      angle (in degrees) of rotation around KML Earth Y axis (Earth north).
      NOTE WELL: In KML, positive rotations are CLOCKWISE looking down
      specified axis vector towards the origin. This is rotation direction
      is opposite the conventional "right hand rule."
  Returns:
    True if the model location is set (this->ModelLocationIsSet() is true)
    and the KML orientation angles are returned.
    Otherwise false is returned and all of the angle values are MYON_DLB_QNAN.
  See Also:
    https://developers.google.com/kml/documentation/kmlreference#orientation
  */
  bool GetKMLOrientationAnglesDegrees(
    double& heading_degrees, 
    double& tilt_degrees, 
    double& roll_degrees
  ) const;

  /*
  Returns:
    If the model location is set (this->ModelLocationIsSet() is true), then the
    Keyhole Markup Language orientation heading angle in degrees is returned.
    Otherwise MYON_DBL_QNAN is returned.
  See Also:
    https://developers.google.com/kml/documentation/kmlreference#orientation
  */
  const double KMLOrientationHeadingAngleDegrees() const;

  /*
  Returns:
    If the model location is set (this->ModelLocationIsSet() is true), then the
    Keyhole Markup Language orientation tilt angle in degrees is returned.
    Otherwise MYON_DBL_QNAN is returned.
  See Also:
    https://developers.google.com/kml/documentation/kmlreference#orientation
  */
  const double KMLOrientationTiltAngleDegrees() const;

  /*
    If the model location is set (this->ModelLocationIsSet() is true), then the
    Keyhole Markup Language orientation roll angle in degrees is returned.
    Otherwise MYON_DBL_QNAN is returned.
  See Also:
    https://developers.google.com/kml/documentation/kmlreference#orientation
  */
  const double KMLOrientationRollAngleDegrees() const;

private:
  // Point on the Earth
  //   Latitude (degrees):  +90 = north pole, 0 = equator, -90 = south pole
  //   Longitude (degrees):   0 = prime meridian (Greenwich meridian)
  //   Elevation (meters):    
  double m_earth_latitude = MYON_UNSET_VALUE;  // in decimal degrees
  double m_earth_longitude = MYON_UNSET_VALUE; // in decimal degrees
  double m_earth_elevation_meters = 0.0;

  MYON::EarthCoordinateSystem m_earth_coordinate_system = MYON::EarthCoordinateSystem::Unset;

private:
  unsigned char m_reserved1 = 0;
  unsigned char m_reserved2 = 0;
  unsigned char m_reserved3 = 0;
  MYON__UINT32 m_reserved4 = 0;

private:
  // Corresponding model point in model coordinates.
  MYON_3dPoint  m_model_point = MYON_3dPoint::Origin; // in model coordinates

  // Earth directions in model coordinates
  MYON_3dVector m_model_north = MYON_3dVector::YAxis; // in model coordinates
  MYON_3dVector m_model_east = MYON_3dVector::XAxis;  // in model coordinates

public:
  // Identification information about this location
  MYON_UUID    m_id = MYON_nil_uuid;  // unique id for this anchor point
  MYON_wString m_name;
  MYON_wString m_description; 
  MYON_wString m_url;
  MYON_wString m_url_tag; // UI link text for m_url

  /*
  Parameters:
    model_compass - [out]
      A plane in model coordinates whose xaxis points East,
      yaxis points North and zaxis points up.  The origin
      is set to m_model_basepoint.
  */
  bool GetModelCompass( 
          MYON_Plane& model_compass 
          ) const;

  /*
  Description:
    Get a transformation from model coordinates to earth coordinates.
    This transformation assumes the model is small enough that
    the curvature of the earth can be ignored.  
  Parameters:
    model_unit_system - [in]
    model_to_earth - [out]
      Transformation from model coordinates to earth locations
      (degrees latitude,degrees longitude,elevation in meters)
  Remarks:
    If M is a point in model coordinates and E = model_to_earth*M,
    then 
       E.x = latitude in decimal degrees
       E.y = longitude in decimal degrees
       E.z = elevation in meters above mean sea level

    Because the earth is not flat, there is a small amount of error
    when using a linear transformation to calculate oblate spherical 
    coordinates.  This error is small.  If the distance from P to M
    is d meters, then the approximation error is

       latitude error  <=
       longitude error <=
       elevation error <= 6379000*((1 + (d/6356000)^2)-1) meters

    In particular, if every point in the model is within 1000 meters of
    the m_model_basepoint, then the maximum approximation errors are

       latitude error  <=
       longitude error <=
       elevation error <= 8 centimeters
  */
  bool GetModelToEarthXform(
          const MYON_UnitSystem& model_unit_system,
          MYON_Xform& model_to_earth
          ) const;

private:
  const MYON_Xform Internal_KMLOrientationXform() const;
};



class MYON_CLASS MYON_3dmIOSettings
{
public:
  MYON_3dmIOSettings() = default;
  ~MYON_3dmIOSettings() = default;
  MYON_3dmIOSettings(const MYON_3dmIOSettings&) = default;
  MYON_3dmIOSettings& operator=(const MYON_3dmIOSettings&) = default;

  static const MYON_3dmIOSettings Default;

  bool Read(MYON_BinaryArchive&);
  bool Write(MYON_BinaryArchive&) const;

  // bitmaps associated with rendering materials
  bool m_bSaveTextureBitmapsInFile = false;

  // As of 7 February 2012, the m_idef_link_update setting
  // controls if, when and how linked and linked_and_embedded
  // instance definitions are updated when the source archive
  // that was used to create the idef has changed.
  int m_idef_link_update = 1;  
      // 1 = prompt - ask the user if the idef should be updated.
      // 2 = always update - no prompting
      // 3 = never update - no prompting
      // Any value not equal to 1,2 or 3 shall be treated as 1.
};

//////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_3dmSettings
//

class MYON_CLASS MYON_3dmSettings
{
public:
  MYON_3dmSettings() = default;
  ~MYON_3dmSettings() = default;

  MYON_3dmSettings(const MYON_3dmSettings&) = default;
  MYON_3dmSettings& operator=(const MYON_3dmSettings&) = default;

  static const MYON_3dmSettings Default;

  bool Read(MYON_BinaryArchive&);
  bool Write(MYON_BinaryArchive&) const;

  void Dump( MYON_TextLog& ) const;

  // model URL (can be empty)
  MYON_wString m_model_URL = MYON_wString::EmptyString;

  // Model basepoint is used when the file is read as
  // an instance definition and is the point that is
  // mapped to the origin in the instance definition.
  MYON_3dPoint m_model_basepoint = MYON_3dPoint::Origin;

  // If set, this is the model's location on the earth.
  // This information is used when the model is used
  // with GIS information.
  MYON_EarthAnchorPoint m_earth_anchor_point = MYON_EarthAnchorPoint::Unset;

  // Model space tolerances and unit system
  MYON_3dmUnitsAndTolerances m_ModelUnitsAndTolerances = MYON_3dmUnitsAndTolerances::Millimeters;

  // Page space (printing/paper) tolerances and unit system
  MYON_3dmUnitsAndTolerances m_PageUnitsAndTolerances = MYON_3dmUnitsAndTolerances::Millimeters;

  // settings used for automatically created rendering meshes
  MYON_MeshParameters m_RenderMeshSettings = MYON_MeshParameters::DefaultMesh;

  // saved custom settings
  MYON_MeshParameters m_CustomRenderMeshSettings = MYON_MeshParameters::DefaultMesh;

  /*
  Returns:
    MYON_MeshParameters::render_mesh_fast
      m_RenderMeshSettings and MYON_MeshParameters::FastRenderMesh have
      the same mesh geometry parameter settings.
    MYON_MeshParameters::render_mesh_quality
      m_RenderMeshSettings and MYON_MeshParameters::QualityRenderMesh have
      the same mesh geometry parameter settings.
    MYON_MeshParameters::render_mesh_custom
      m_RenderMeshSettings and m_CustomRenderMeshSettings have
      the same mesh geometry parameter settings.
    no_match_found_result
      otherwise
  */
  MYON_MeshParameters::MESH_STYLE RenderMeshStyle(
    MYON_MeshParameters::MESH_STYLE no_match_found_result
    ) const;

  // settings used for automatically created analysis meshes
  MYON_MeshParameters m_AnalysisMeshSettings = MYON_MeshParameters::DefaultAnalysisMesh;

  // settings used when annotation objects are created
  MYON_3dmAnnotationSettings m_AnnotationSettings;

  MYON_ClassArray<MYON_3dmConstructionPlane> m_named_cplanes;
  MYON_ClassArray<MYON_3dmView>              m_named_views;
  MYON_ClassArray<MYON_3dmView>              m_views; // current viewports
  MYON_UUID m_active_view_id = MYON_nil_uuid; // id of "active" viewport              

  // These fields determine what layer, material, color, line style, and
  // wire density are used for new objects.

public:
  void SetCurrentLayerId(
    MYON_UUID layer_id
    );
  void SetV5CurrentLayerIndex(
    int V5_current_layer_index
    );
  int CurrentLayerIndex() const;
  MYON_UUID CurrentLayerId() const;
private:
  // The index is for reading V5 and earlier files.
  int m_V5_current_layer_index = MYON_UNSET_INT_INDEX;
  MYON_UUID m_current_layer_id = MYON_nil_uuid;

public:
  void SetCurrentMaterialId(
    MYON_UUID material_id
    );
  int CurrentMaterialIndex() const;
  MYON_UUID CurrentMaterialId() const;
private:
  // The index is for reading V5 and earlier files.
  int m_V5_current_render_material_index = MYON_UNSET_INT_INDEX;
  MYON_UUID m_current_render_material_id = MYON_nil_uuid;

public:
  MYON::object_material_source m_current_material_source = MYON::material_from_layer;
  
  MYON_Color m_current_color = MYON_Color::Black;
  MYON::object_color_source m_current_color_source = MYON::color_from_layer;

  MYON_Color m_current_plot_color = MYON_Color::UnsetColor;
  MYON::plot_color_source m_current_plot_color_source = MYON::plot_color_from_layer;

public:
  void SetCurrentLinePatternId(
    MYON_UUID line_pattern_id
    );
  int CurrentLinePatternIndex() const;
  MYON_UUID CurrentLinePatternId() const;
private:
  // The index is for reading V5 and earlier files.
  int m_V5_current_line_pattern_index = MYON_UNSET_INT_INDEX;
  MYON_UUID m_current_line_pattern_id = MYON_nil_uuid;

public:
  MYON::object_linetype_source m_current_linetype_source = MYON::linetype_from_layer;

public:
  void SetCurrentTextStyleId(
    MYON_UUID text_style_id
    );
  int CurrentTextStyleIndex() const;
  MYON_UUID CurrentTextStyleId() const;
private:
  // The index is for reading V5 and earlier files.
  int m_V5_current_text_style_index = MYON_UNSET_INT_INDEX;
  MYON_UUID m_current_text_style_id = MYON_nil_uuid;

public:
  void SetCurrentDimensionStyleId(
    MYON_UUID dimension_style_id
    );
  int CurrentDimensionStyleIndex() const;
  MYON_UUID CurrentDimensionStyleId() const;
private:
  // The index is for reading V5 and earlier files.
  int m_V5_current_dimension_style_index = MYON_UNSET_INT_INDEX;
  MYON_UUID m_current_dimension_style_id = MYON_nil_uuid;

public:
  void SetCurrentHatchPatternId(
    MYON_UUID hatch_pattern_id
    );
  MYON_UUID CurrentHatchPatternId() const;
private:
  MYON_UUID m_current_hatch_pattern_id = MYON_nil_uuid;
 
public:
  // Surface wireframe density
  //
  //   @untitled table
  //   0       boundary + "knot" wires 
  //   1       boundary + "knot" wires + 1 interior wire if no interior "knots"
  //   N>=2    boundary + "knot" wires + (N-1) interior wires
  int m_current_wire_density = 1;

  MYON_3dmRenderSettings m_RenderSettings = MYON_3dmRenderSettings::Default;

  // default settings for construction plane grids
  MYON_3dmConstructionPlaneGridDefaults m_GridDefaults = MYON_3dmConstructionPlaneGridDefaults::Default;

  // World scale factor to apply to non-solid linetypes
  // for model display.  For plotting, the linetype settings
  // are used without scaling.
  double m_linetype_display_scale = 1.0;

  // Plugins that were loaded when the file was saved.
  MYON_ClassArray<MYON_PlugInRef> m_plugin_list;

  MYON_3dmIOSettings m_IO_settings = MYON_3dmIOSettings::Default;
private:
  bool Read_v1(MYON_BinaryArchive&);
  bool Read_v2(MYON_BinaryArchive&);
  bool Write_v1(MYON_BinaryArchive&) const;
  bool Write_v2(MYON_BinaryArchive&) const;
};


//////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_3dmAnimationProperties
//

class MYON_CLASS MYON_3dmAnimationProperties
{
public:
  MYON_3dmAnimationProperties() = default;
  ~MYON_3dmAnimationProperties() = default;
  MYON_3dmAnimationProperties(const MYON_3dmAnimationProperties&) = default;
  MYON_3dmAnimationProperties& operator=(const MYON_3dmAnimationProperties&) = default;

  static const MYON_3dmAnimationProperties Default;

  bool Read(MYON_BinaryArchive&);
  bool Write(MYON_BinaryArchive&) const;

public:
  enum class CaptureTypes : int
  {
    path = 0,
    turntable,
    flythrough,
    day_sun_study,
    seasonal_sun_study,
    none
  };

  CaptureTypes CaptureType(void) const;
  void SetCaptureType(CaptureTypes t);

  MYON_wString FileExtension(void) const;
  void SetFileExtension(const MYON_wString& s);

  MYON_wString CaptureMethod(void) const;
  void SetCaptureMethod(const MYON_wString& s);

  MYON_wString ViewportName(void) const;
  void SetViewportName(const MYON_wString& s);

  MYON_wString HtmlFilename(void) const;
  void SetHtmlFilename(const MYON_wString& s);

  MYON_UUID DisplayMode(void) const;
  void SetDisplayMode(const MYON_UUID& id);

  MYON_3dPointArray& CameraPoints(void);
  const MYON_3dPointArray& CameraPoints(void) const;

  MYON_3dPointArray& TargetPoints(void);
  const MYON_3dPointArray& TargetPoints(void) const;

  int FrameCount(void) const;
  void SetFrameCount(int i);

  int CurrentFrame(void) const;
  void SetCurrentFrame(int i);

  MYON_UUID CameraPathId(void) const;
  void SetCameraPathId(const MYON_UUID& id);

  MYON_UUID TargetPathId(void) const;
  void SetTargetPathId(const MYON_UUID& id);

  double Latitude(void) const;
  void SetLatitude(double d);

  double Longitude(void) const;
  void SetLongitude(double d);

  double NorthAngle(void) const;
  void SetNorthAngle(double d);

  int StartDay(void) const;
  void SetStartDay(int i);

  int StartMonth(void) const;
  void SetStartMonth(int i);

  int StartYear(void) const;
  void SetStartYear(int i);

  int EndDay(void) const;
  void SetEndDay(int i);

  int EndMonth(void) const;
  void SetEndMonth(int i);

  int EndYear(void) const;
  void SetEndYear(int i);

  int StartHour(void) const;
  void SetStartHour(int i);

  int StartMinutes(void) const;
  void SetStartMinutes(int i);

  int StartSeconds(void) const;
  void SetStartSeconds(int i);

  int EndHour(void) const;
  void SetEndHour(int i);

  int EndMinutes(void) const;
  void SetEndMinutes(int i);

  int EndSeconds(void) const;
  void SetEndSeconds(int i);

  int DaysBetweenFrames(void) const;
  void SetDaysBetweenFrames(int i);

  int MinutesBetweenFrames(void) const;
  void SetMinutesBetweenFrames(int i);

  int LightIndex(void) const;
  void SetLightIndex(int i);

  MYON_wString FolderName(void) const;
  void SetFolderName(const MYON_wString& s);

  MYON_ClassArray<MYON_wString>& Images(void);
  const MYON_ClassArray<MYON_wString>& Images(void) const;

  MYON_ClassArray<MYON_wString>& Dates(void);
  const MYON_ClassArray<MYON_wString>& Dates(void) const;

  bool RenderFull(void) const;
  void SetRenderFull(const bool b);

  bool RenderPreview(void) const;
  void SetRenderPreview(const bool b);

private:
  CaptureTypes m_CaptureTypes  = CaptureTypes::none;
  MYON_wString m_sFileExtension = L"jpg";
  MYON_wString m_sCaptureMethod;
  MYON_wString m_sHtmlFilename;
  MYON_wString m_sViewport;
  MYON_UUID m_idDisplayMode = MYON_nil_uuid;
  MYON_3dPointArray m_aCameraPoints;
  MYON_3dPointArray m_aTargetPoints;
  int m_iFrameCount = 100;
  int m_iCurrentFrame = 1;
  MYON_UUID m_idCameraPath = MYON_nil_uuid;
  MYON_UUID m_idTargetPath = MYON_nil_uuid;
  double m_dLatitude = 51.2838;
  double m_dLongitude = 0.0;
  double m_dNorthAngle = 0.0;
  int m_iStartDay = 1;
  int m_iStartMonth = 6;
  int m_iStartYear = 2010;
  int m_iEndDay = 1;
  int m_iEndMonth = 6;
  int m_iEndYear = 2012;
  int m_iStartHour = 6;
  int m_iStartMinutes = 0;
  int m_iStartSeconds = 0;
  int m_iEndHour = 18;
  int m_iEndMinutes = 0;
  int m_iEndSeconds = 59;
  int m_iDaysBetweenFrames = 30;
  int m_iMinutesBetweenFrames = 30;
  int m_iLightIndex = -1;
  MYON_wString m_sFolderName;
  MYON_ClassArray<MYON_wString> m_aImages;
  MYON_ClassArray<MYON_wString> m_aDates;
  bool m_bRenderFull = false;
  bool m_bRenderPreview = false;

private:
  unsigned char m_reserved1 = 0;
  unsigned char m_reserved2 = 0;
  MYON__UINT32 m_reserved4 = 0;
  MYON__INT_PTR reserved = 0;
};

#endif

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

#if !defined(OPENNURBS_INTERNAL_V2_ANNOTATION_H_INC)
#define OPENNURBS_INTERNAL_V2_ANNOTATION_H_INC

#if defined(MYON_COMPILING_OPENNURBS)

#include "opennurbs_internal_defines.h"

// Annotation classes used in version 2 .3dm archives and Rhino version 2.
// All classes in this file are obsolete. They exist so that old files can be read.

// Legacy annotation arrow is in some old .3dm files.
// Gets converted to an MYON_Line with MYON_3dmObjectAttributes arrow head
// MYON_3dmObjectAttributes.m_object_decoration = (MYON::end_arrowhead | other bits)
class MYON_OBSOLETE_V2_AnnotationArrow : public MYON_Geometry
{
  // 3d annotation arrow
  MYON_OBJECT_DECLARE(MYON_OBSOLETE_V2_AnnotationArrow);
public:
  MYON_OBSOLETE_V2_AnnotationArrow();
  ~MYON_OBSOLETE_V2_AnnotationArrow();
  MYON_OBSOLETE_V2_AnnotationArrow(const MYON_OBSOLETE_V2_AnnotationArrow&);
  MYON_OBSOLETE_V2_AnnotationArrow& operator=(const MYON_OBSOLETE_V2_AnnotationArrow&);

  /////////////////////////////////////////////////////////////////
  //
  // MYON_Object overrides
  //
  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;

  void Dump( MYON_TextLog& ) const override; // for debugging

  bool Write(
         MYON_BinaryArchive&  // serialize definition to binary archive
       ) const override;

  bool Read(
         MYON_BinaryArchive&  // restore definition from binary archive
       ) override;

  MYON::object_type ObjectType() const override;

  /////////////////////////////////////////////////////////////////
  //
  // MYON_Geometry overrides
  //

  int Dimension() const override;

  // virtual MYON_Geometry GetBBox override		
  bool GetBBox( double* boxmin, double* boxmax, bool bGrowBox = false ) const override;

  bool Transform( 
         const MYON_Xform&
         ) override;

  /////////////////////////////////////////////////////////////////
  //
  // Interface
  //
  MYON_3dVector Vector() const;
  MYON_3dPoint Head() const;
  MYON_3dPoint Tail() const;

  MYON_3dPoint m_tail;
  MYON_3dPoint m_head;
};

class MYON_OBSOLETE_V2_TextDot : public MYON_Point
{
  // 3d annotation dot with text
  MYON_OBJECT_DECLARE(MYON_OBSOLETE_V2_TextDot);
public:
  MYON_OBSOLETE_V2_TextDot();
  ~MYON_OBSOLETE_V2_TextDot();
  MYON_OBSOLETE_V2_TextDot(const MYON_OBSOLETE_V2_TextDot&);
  MYON_OBSOLETE_V2_TextDot& operator=(const MYON_OBSOLETE_V2_TextDot&);

  /////////////////////////////////////////////////////////////////
  //
  // MYON_Object overrides
  //

  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;
  void Dump( MYON_TextLog& ) const override; // for debugging

  bool Write(
         MYON_BinaryArchive&  // serialize definition to binary archive
       ) const override;

  bool Read(
         MYON_BinaryArchive&  // restore definition from binary archive
       ) override;

  MYON_wString m_text;
};


////////////////////////////////////////////////////////////////
//
//   MYON_OBSOLETE_V2_Annotation - used to serialize definitions of annotation
//                   objects (dimensions, text blocks, etc.).
//

class MYON_OBSOLETE_V2_Annotation : public MYON_Geometry
{
  MYON_OBJECT_DECLARE(MYON_OBSOLETE_V2_Annotation);

protected:
  MYON_OBSOLETE_V2_Annotation() = default;
  MYON_OBSOLETE_V2_Annotation(const MYON_OBSOLETE_V2_Annotation&) = default;
  MYON_OBSOLETE_V2_Annotation& operator=(const MYON_OBSOLETE_V2_Annotation&) = default;

public:
  virtual ~MYON_OBSOLETE_V2_Annotation() = default;

protected:
  void Internal_Initialize();  // initialize class's fields assuming
                  // memory is uninitialized

public:
  static MYON_OBSOLETE_V2_Annotation* CreateFromV5Annotation(
    const class MYON_OBSOLETE_V5_Annotation& V5_annotation,
    const class MYON_3dmAnnotationContext* annotation_context
  );

  static MYON_OBSOLETE_V2_Annotation* CreateFromV6Annotation(
    const class MYON_Annotation& V6_annotation,
    const class MYON_3dmAnnotationContext* annotation_context
  );

protected:
  void Internal_InitializeFromV5Annotation(
    const MYON_OBSOLETE_V5_Annotation& V5_annotation,
    const class MYON_3dmAnnotationContext* annotation_context
  );

public:
  void Destroy();
  void EmergencyDestroy();

  /////////////////////////////////////////////////////////////////
  //
  // MYON_Object overrides
  //
  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;

  void Dump( MYON_TextLog& ) const override; // for debugging

  bool Write(
         MYON_BinaryArchive&  // serialize definition to binary archive
       ) const override;

  bool Read(
         MYON_BinaryArchive&  // restore definition from binary archive
       ) override;

  MYON::object_type ObjectType() const override;

  /////////////////////////////////////////////////////////////////
  //
  // MYON_Geometry overrides
  //

  int Dimension() const override;

  // virtual MYON_Geometry GetBBox override		
  bool GetBBox( double* boxmin, double* boxmax, bool bGrowBox = false ) const override;

  bool Transform( 
         const MYON_Xform&
         ) override;

  /////////////////////////////////////////////////////////////////
  //
  // MYON_OBSOLETE_V2_Annotation interface
  //

  // use these to get/set the current annotation settings
  static const MYON_3dmAnnotationSettings& AnnotationSettings();
  static void SetAnnotationSettings( const MYON_3dmAnnotationSettings* );

  bool IsText() const;
  bool IsLeader() const;
  bool IsDimension() const;

  virtual double NumericValue() const;
  virtual void SetTextToDefault();

  void SetType( MYON_INTERNAL_OBSOLETE::V5_eAnnotationType type );
  MYON_INTERNAL_OBSOLETE::V5_eAnnotationType Type() const;
  void SetTextDisplayMode( MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode mode);
  MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode TextDisplayMode() const;

  void SetPlane( const MYON_Plane& plane );
  MYON_Plane Plane() const;
  int PointCount() const;
  void SetPoints( const MYON_SimpleArray<MYON_2dPoint>& points );
  const MYON_SimpleArray<MYON_2dPoint>& Points() const;
  void SetPoint( int idx, MYON_3dPoint point );
  MYON_2dPoint Point( int idx ) const;
  void SetUserText( const wchar_t* string );
  const MYON_wString& UserText() const;
  void SetDefaultText( const wchar_t* string );
  const MYON_wString& DefaultText() const;
  void SetUserPositionedText( int bUserPositionedText );
  bool UserPositionedText() const;

  // to convert world 3d points to and from annotation 2d points
  bool GetECStoWCSXform( MYON_Xform& xform ) const;
  bool GeWCStoECSXform( MYON_Xform& xform ) const;

  MYON_INTERNAL_OBSOLETE::V5_eAnnotationType m_type = MYON_INTERNAL_OBSOLETE::V5_eAnnotationType::dtNothing;          // enum for type of annotation
                                       // DimLinear, DimRadius, etc.

  MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode m_textdisplaymode = MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode::kNormal; // how the text is displayed
                                       // Horizontal, InLine, AboveLine

  MYON_Plane m_plane = MYON_Plane::World_xy;                    // ECS reference plane in WCS coordinates
  MYON_SimpleArray<MYON_2dPoint> m_points; // Definition points for the dimension

  MYON_wString m_usertext;               // "<>", or user override
  MYON_wString m_defaulttext;            // The displayed text string

  bool m_userpositionedtext = false;   // true: User has positioned text 
                                       // false: use default location
};

class MYON_OBSOLETE_V2_DimLinear : public MYON_OBSOLETE_V2_Annotation
{
  MYON_OBJECT_DECLARE(MYON_OBSOLETE_V2_DimLinear);

public:
  MYON_OBSOLETE_V2_DimLinear();
  MYON_OBSOLETE_V2_DimLinear(const MYON_OBSOLETE_V2_DimLinear&);
  ~MYON_OBSOLETE_V2_DimLinear();
  MYON_OBSOLETE_V2_DimLinear& operator=(const MYON_OBSOLETE_V2_DimLinear&);

  double NumericValue() const override;
  void SetTextToDefault() override;
  void EmergencyDestroy();

  static MYON_OBSOLETE_V2_DimLinear* CreateFromV5LinearDimension(
    const class MYON_OBSOLETE_V5_DimLinear& V5_linear_dimension,
    const class MYON_3dmAnnotationContext* annotation_context,
    MYON_OBSOLETE_V2_DimLinear* destination
  );
};

class MYON_OBSOLETE_V2_DimRadial : public MYON_OBSOLETE_V2_Annotation
{
  MYON_OBJECT_DECLARE(MYON_OBSOLETE_V2_DimRadial);

public:
  MYON_OBSOLETE_V2_DimRadial();
  MYON_OBSOLETE_V2_DimRadial(const MYON_OBSOLETE_V2_DimRadial&);
  ~MYON_OBSOLETE_V2_DimRadial();
  MYON_OBSOLETE_V2_DimRadial& operator=(const MYON_OBSOLETE_V2_DimRadial&);

  double NumericValue() const override;
  void SetTextToDefault() override;

  void EmergencyDestroy();

  static MYON_OBSOLETE_V2_DimRadial* CreateFromV5RadialDimension(
    const class MYON_OBSOLETE_V5_DimRadial& V5_linear_dimension,
    const class MYON_3dmAnnotationContext* annotation_context,
    MYON_OBSOLETE_V2_DimRadial* destination
  );
};

class MYON_OBSOLETE_V2_DimAngular : public MYON_OBSOLETE_V2_Annotation
{
  MYON_OBJECT_DECLARE(MYON_OBSOLETE_V2_DimAngular);

public:
  MYON_OBSOLETE_V2_DimAngular();
  MYON_OBSOLETE_V2_DimAngular(const MYON_OBSOLETE_V2_DimAngular&);
  ~MYON_OBSOLETE_V2_DimAngular();
  MYON_OBSOLETE_V2_DimAngular& operator=(const MYON_OBSOLETE_V2_DimAngular&);

  static MYON_OBSOLETE_V2_DimAngular* CreateFromV5AngularDimension(
    const class MYON_OBSOLETE_V5_DimAngular& V5_angular_dimension,
    const class MYON_3dmAnnotationContext* annotation_context,
    MYON_OBSOLETE_V2_DimAngular* destination
  );

  void EmergencyDestroy();

  bool Write( MYON_BinaryArchive& file ) const override;
  bool Read( MYON_BinaryArchive& file ) override;

  void SetAngle( double angle ) { m_angle = angle; }
  double Angle() const { return m_angle; }
  void SetRadius( double radius ) { m_radius = radius; }
  double Radius() const { return m_radius; }

  double NumericValue() const override;
  void SetTextToDefault() override;

private:
  double m_angle;      // angle being dimensioned
  double m_radius;     // radius for dimension arc
};

class MYON_OBSOLETE_V2_TextObject : public MYON_OBSOLETE_V2_Annotation
{
  MYON_OBJECT_DECLARE(MYON_OBSOLETE_V2_TextObject);

public:
  MYON_OBSOLETE_V2_TextObject();
  MYON_OBSOLETE_V2_TextObject(const MYON_OBSOLETE_V2_TextObject&);
  ~MYON_OBSOLETE_V2_TextObject();
  MYON_OBSOLETE_V2_TextObject& operator=(const MYON_OBSOLETE_V2_TextObject&);

  static MYON_OBSOLETE_V2_TextObject* CreateFromV5TextObject(
    const class MYON_OBSOLETE_V5_TextObject& V5_text_object,
    const class MYON_3dmAnnotationContext* annotation_context,
    MYON_OBSOLETE_V2_TextObject* destination
  );

  void EmergencyDestroy();

  bool Write( MYON_BinaryArchive& file ) const override;
  bool Read( MYON_BinaryArchive& file ) override;

  void SetFaceName( MYON_wString string ) { m_facename = string; }
  MYON_wString FaceName() const { return m_facename; }
  void SetFontWeight( int weight ) { m_fontweight = weight; }
  int FontWeight() const { return m_fontweight; }
  void SetHeight( double height ) { m_height = height; }
  double Height() const { return m_height; }


private:
  MYON_wString m_facename;
  int m_fontweight;  // windows - 400 = NORMAL )
  double m_height;   // gets multiplied by dimscale
};

class MYON_OBSOLETE_V2_Leader : public MYON_OBSOLETE_V2_Annotation
{
  MYON_OBJECT_DECLARE(MYON_OBSOLETE_V2_Leader);

public:
  MYON_OBSOLETE_V2_Leader();
  MYON_OBSOLETE_V2_Leader(const MYON_OBSOLETE_V2_Leader&);
  ~MYON_OBSOLETE_V2_Leader();
  MYON_OBSOLETE_V2_Leader& operator=(const MYON_OBSOLETE_V2_Leader&);

  static MYON_OBSOLETE_V2_Leader* CreateFromV5Leader(
    const class MYON_OBSOLETE_V5_Leader& V5_leader,
    const class MYON_3dmAnnotationContext* annotation_context,
    MYON_OBSOLETE_V2_Leader* destination
  );

  void EmergencyDestroy();
};

#endif
#endif


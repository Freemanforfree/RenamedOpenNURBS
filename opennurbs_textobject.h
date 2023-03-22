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

// MYON_Table class
#ifndef OPENNURBS_TEXTOBJECT_H_INCLUDED
#define OPENNURBS_TEXTOBJECT_H_INCLUDED

class MYON_CLASS MYON_Text : public MYON_Annotation
{
  MYON_OBJECT_DECLARE(MYON_Text);

public:
  static const MYON_Text Empty;

  MYON_Text();
  ~MYON_Text();

  MYON_Text(const MYON_Text& src);
  MYON_Text& operator=(const MYON_Text& src);

public:

  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;

  // Parses text string and makes runs
  bool Create(
    const wchar_t* RtfString,
    const MYON_DimStyle* dimstyle,
    MYON_Plane plane,
    bool bWrapped,
    double rect_width,
    double text_rotation_radians
    );

  // Parses text string and makes runs - wrapped = false, rotation = 0
  bool Create(
    const wchar_t* RtfString,
    const MYON_DimStyle* dimstyle,
    MYON_Plane plane
    );

  void Destroy();

  // virtual
  void Dump(MYON_TextLog&) const override; // for debugging

  bool Write(
    MYON_BinaryArchive&  // serialize definition to binary archive
    ) const override;

  bool Read(
    MYON_BinaryArchive&  // restore definition from binary archive
    ) override;

  int Dimension() const override;

  /*
  Description:
    Create a V5 text object from a V6 text object. 
    The function is used when reading V5 files.
  Parameters:
    v5_text_object -[in]
    dimstyle - [in]
      Dimstyle referenced by v5_text_object or nullptr if not available.
    destination - [in]
      If destination is not nullptr, then the V6 text object is constructed
      in destination. If destination is nullptr, then the new V6 text object
      is allocated with a call to new MYON_Text().
  */
  static MYON_Text* CreateFromV5TextObject(
    const class MYON_OBSOLETE_V5_TextObject& V5_text_object,
    const class MYON_3dmAnnotationContext* annotation_context,
    MYON_Text* destination
  );

  //bool Explode(
  //  const MYON_DimStyle* dimstyle,
  //  MYON_SimpleArray<const MYON_Geometry*> object_parts) const;



  // virtual MYON_Geometry GetBBox override		
  bool GetBBox( double* boxmin, double* boxmax, bool bGrowBox = false ) const override;

  bool GetAnnotationBoundingBox(
    const MYON_Viewport* vp,
    const MYON_DimStyle* dimstyle,
    double dimscale,
    double* boxmin,
    double* boxmax,
    bool bGrow = false
  ) const override; // MYON_Annotation override
  
  bool Transform(const MYON_Xform& xform) override;

  /*
  Description:
  Transform the object by a 4x4 xform matrix and change text height
  override to accommodate scaling in the transform if necessary
  Parameters:
  [in] xform  - An MYON_Xform with the transformation information
  Returns:
  true = Success
  false = Failure
  Remarks:
  The object has been transformed when the function returns
  If the scale changed because of the transform, an override for text height
  is either changed, if one already was in place, or is added.
  */
  bool Transform(const MYON_Xform& xform, const MYON_DimStyle* dimstyle);

  // Transforms text from natural position at origin to 
  // 3d location as it displays in the text object
  bool GetTextXform(
    const MYON_Viewport* vp,
    const MYON_DimStyle* dimstyle,
    double dimscale,
    MYON_Xform& text_xform_out
  ) const override;

  bool GetTextXform(
    const MYON_Xform* model_xform,
    const MYON_Viewport* vp,
    const MYON_DimStyle* dimstyle,
    double dimscale,
    MYON_Xform& text_xform_out
  ) const;

  // returns the base point and with grip using the current alignments
  bool GetGripPoints(MYON_2dPoint& base, MYON_2dPoint& width, double textscale) const;

private:
  void Internal_SetObsoleteV5TextObjectInformation(
    const class MYON_3dmAnnotationContext* annotation_context,
    const class MYON_OBSOLETE_V5_TextObject& V5_text_object
  );
  public:
    bool GetTextXform(
      const MYON_Xform* model_xform,
      const MYON_3dVector view_x,
      const MYON_3dVector view_y,
      const MYON_3dVector view_z,
      MYON::view_projection projection,
      bool bDrawForward,
      const MYON_DimStyle* dimstyle,
      double dimscale,
      MYON_Xform& text_xform_out
    ) const;

};

#endif


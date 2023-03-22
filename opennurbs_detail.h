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

#if !defined(MYON_DETAIL_OBJECTY_INC_)
#define MYON_DETAIL_OBJECTY_INC_

class MYON_CLASS MYON_DetailView : public MYON_Geometry
{
  MYON_OBJECT_DECLARE(MYON_DetailView);

public:
  MYON_DetailView();
  ~MYON_DetailView();

  // C++ defaults for copy constructor and
  // operator= work fine.

  //////////////////////////////////////////////////////
  //
  // virtual MYON_Object overrides
  //
  void MemoryRelocate() override;

  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;

  void Dump( MYON_TextLog& ) const override;

  unsigned int SizeOf() const override;

  bool Write(
         MYON_BinaryArchive& binary_archive
       ) const override;

  bool Read(
         MYON_BinaryArchive& binary_archive
       ) override;

  MYON::object_type ObjectType() const override; // returns MYON::detail_object

  //////////////////////////////////////////////////////
  //
  // virtual MYON_Geometry overrides
  // The m_boundary determines all bounding boxes 
  //
  int Dimension() const override;

  // virtual MYON_Geometry GetBBox override		
  bool GetBBox( double* boxmin, double* boxmax, bool bGrowBox = false ) const override;

  // virtual MYON_Geometry GetTightBoundingBox override		
  bool GetTightBoundingBox( class MYON_BoundingBox& tight_bbox, bool bGrowBox = false, const class MYON_Xform* xform = nullptr ) const override;

  bool Transform( const MYON_Xform& xform ) override;

  // m_page_per_model_ratio is the ratio of page length / model length
  // where both lengths are in the same unit system
  // (ex. 1/4" on page = 1' in model = 0.25/12 = 0.02083)
  // (    1mm on page  = 1m in model = 1/1000  = 0.001)
  // If m_page_per_model_ratio > 0.0, then the detail
  // is drawn using the specified scale.
  double m_page_per_model_ratio;

  // A view with MYON_3dmView::m_view_type = MYON::nested_view_type
  // This field is used for IO purposes only.  Runtime detail
  // view projection information is on CRhDetailViewObject.
  MYON_3dmView m_view;

  // 2d curve in page layout coordinates in mm
  // (0,0) = lower left corner of page
  MYON_NurbsCurve m_boundary;

  // Update frustum to match bounding box and detail scale
  bool UpdateFrustum(
    MYON::LengthUnitSystem model_units,
    MYON::LengthUnitSystem paper_units
  );
};



#endif


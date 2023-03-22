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

#if !defined(OPENNURBS_POINT_GEOMETRY_INC_)
#define OPENNURBS_POINT_GEOMETRY_INC_

// NOTE:  MYON_3dPoint is much more efficient than MYON_Point.
//        Use MYON_Point when you need a polymorphic 3d point
//        that is derived from MYON_Geometry or MYON_Object.

class MYON_CLASS MYON_Point : public MYON_Geometry
{
public:
  MYON_3dPoint point;

  MYON_Point();
  MYON_Point(const MYON_Point&);
  MYON_Point(const MYON_3dPoint&);
  MYON_Point(double,double,double);
  ~MYON_Point();
  MYON_Point& operator=(const MYON_Point&);
  MYON_Point& operator=(const MYON_3dPoint&);
  
  operator double*();
  operator const double*() const;
  operator MYON_3dPoint*();
  operator const MYON_3dPoint*() const;
  operator MYON_3dPoint&();
  operator const MYON_3dPoint&() const;

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

  // virtual MYON_Geometry::IsDeformable() override
  bool IsDeformable() const override;

  // virtual MYON_Geometry::MakeDeformable() override
  bool MakeDeformable() override;

  bool SwapCoordinates(
        int, int        // indices of coords to swap
        ) override;


private:
  MYON_OBJECT_DECLARE(MYON_Point);
};

#endif

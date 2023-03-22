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

#if !defined(OPENNURBS_POINT_GRID_INC_)
#define OPENNURBS_POINT_GRID_INC_

class MYON_CLASS MYON_PointGrid : public MYON_Geometry
{
public:
  MYON_PointGrid();
  MYON_PointGrid(const MYON_PointGrid&);
  MYON_PointGrid(
          int,  // point count0 (>=1)
          int   // point count1 (>=1)
          );

  void Initialize(void);  // zeros all fields

  bool Create( 
          int,  // point count0 (>=1)
          int   // point count1 (>=1)
          );

  void Destroy();

  virtual ~MYON_PointGrid();
  void EmergencyDestroy(); // call if memory used by point grid becomes invalid

	MYON_PointGrid& operator=(const MYON_PointGrid&);

  // point_grid[i][j] returns GetPoint(i,j)
  MYON_3dPoint* operator[](int);             // 0 <= index < PointCount(0)
  const MYON_3dPoint* operator[](int) const; // 0 <= index < PointCount(0)
  
  /////////////////////////////////////////////////////////////////
  // MYON_Object overrides

  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;

  void Dump( MYON_TextLog& ) const override; // for debugging

  bool Write(
         MYON_BinaryArchive&  // open binary file
       ) const override;

  bool Read(
         MYON_BinaryArchive&  // open binary file
       ) override;

  MYON::object_type ObjectType() const override;

  /////////////////////////////////////////////////////////////////
  // MYON_Geometry overrides

  int Dimension() const override;

  // virtual MYON_Geometry GetBBox override		
  bool GetBBox( double* boxmin, double* boxmax, bool bGrowBox = false ) const override;

  // virtual MYON_Geometry GetTightBoundingBox override		
  bool GetTightBoundingBox( class MYON_BoundingBox& tight_bbox, bool bGrowBox = false, const class MYON_Xform* xform = nullptr ) const override;

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

  /////////////////////////////////////////////////////////////////
  // Interface

  bool IsClosed( 
        int // dir
        ) const;

  int PointCount(   // number of points in grid direction
        int         // dir 0 = "s", 1 = "t"
        ) const;

  int PointCount(   // total number of points in grid
        void
        ) const;

  MYON_3dPoint& Point(
        int, int // point index ( 0 <= i <= PointCount(0), 0 <= j <= PointCount(1)
        );

  MYON_3dPoint Point(
        int, int // point index ( 0 <= i <= PointCount(0), 0 <= j <= PointCount(1)
        ) const;

  double* PointArray();

  const double* PointArray() const;

  int PointArrayStride(  // point stride in grid direction
        int         // dir 0 = "s", 1 = "t"
        ) const;

  bool SetPoint(      // set a single point
        int, int, // point index ( 0 <= i <= PointCount(0), 0 <= j <= PointCount(1)
        const MYON_3dPoint& // value of point
        );

  bool GetPoint(              // get a single control vertex
        int, int,   // CV index ( 0 <= i <= CVCount(0), 0 <= j <= CVCount(1)
        MYON_3dPoint&      // gets euclidean cv when NURBS is rational
        ) const;

  bool Reverse(  // reverse grid order
    int // dir  0 = "s", 1 = "t"
    );

  bool Transpose(); // transpose grid points

  /////////////////////////////////////////////////////////////////
  // Implementation
protected:

  int m_point_count[2];   // number of points (>=1)
  int m_point_stride0;    // >= m_point_count[1]
  MYON_3dPointArray m_point;
  // point[i][j] = m_point[i*m_point_stride0+j]

private:
  static MYON_3dPoint m_no_point; // prevent crashes when sizes are 0

  MYON_OBJECT_DECLARE(MYON_PointGrid);
};


#endif

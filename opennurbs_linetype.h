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

#if !defined(OPENNURBS_LINETYPE_INC_)
#define OPENNURBS_LINETYPE_INC_


// Description:
//   Determine if a line width is deemed to be a "hairline width" in Rhino
//   Any width that is >0 and < 0.001 mm is a hairline width for printing
// Parameters:
//   width_mm: [in] the width to examine in millimeters
// Returns:
//   true if this is a hairline width
MYON_DECL bool MYON_IsHairlinePrintWidth( double width_mm );

// Description:
//   Return a width in millimeters that is a valid hairline width in rhino
MYON_DECL double MYON_HairlinePrintWidth();




//////////////////////////////////////////////////////////////////////
// class MYON_Linetype

class MYON_CLASS MYON_Linetype : public MYON_ModelComponent
{
  MYON_OBJECT_DECLARE(MYON_Linetype);

public:
  // no attributes are set.
  static const MYON_Linetype Unset;

  // index = -1, id, name and pattern are set.
  static const MYON_Linetype Continuous;

  // index = -2, id, name and pattern are set.
  static const MYON_Linetype ByLayer;

  // index = -3, id, name and pattern are set.
  static const MYON_Linetype ByParent;

  // index = -4, id, name and pattern are set.
  static const MYON_Linetype Hidden;

  // index = -5, id, name and pattern are set.
  static const MYON_Linetype Dashed;

  // index = -6, id, name and pattern are set.
  static const MYON_Linetype DashDot;

  // index = -7, id, name and pattern are set.
  static const MYON_Linetype Center;

  // index = -8, id, name and pattern are set.
  static const MYON_Linetype Border;

  // index = -9, id, name and pattern are set.
  static const MYON_Linetype Dots;

  /*
  Parameters:
    model_component_reference - [in]
    none_return_value - [in]
      value to return if MYON_Linetype::Cast(model_component_ref.ModelComponent())
      is nullptr
  Returns:
    If MYON_Linetype::Cast(model_component_ref.ModelComponent()) is not nullptr,
    that pointer is returned.  Otherwise, none_return_value is returned. 
  */
  static const MYON_Linetype* FromModelComponentRef(
    const class MYON_ModelComponentReference& model_component_reference,
    const MYON_Linetype* none_return_value
    );

public:

  MYON_Linetype() MYON_NOEXCEPT;
  ~MYON_Linetype();
  MYON_Linetype(const MYON_Linetype&);
  MYON_Linetype& operator=(const MYON_Linetype&);

  /*
    Description:
      Tests that name is set and there is at least one non-zero length segment
  */
  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;

  void Dump( MYON_TextLog& ) const override; // for debugging

  /*
    Description:
      Write to file
  */
  bool Write(
         MYON_BinaryArchive&  // serialize definition to binary archive
       ) const override;

  /*
    Description:
      Read from file
  */
  bool Read(
         MYON_BinaryArchive&  // restore definition from binary archive
       ) override;


  //////////////////////////////////////////////////////////////////////
  //
  // Interface

  bool PatternIsSet() const;
  bool ClearPattern();
  bool PatternIsLocked() const;
  void LockPattern();

  /*
    Description:
      Returns the total length of one repeat of the pattern
  */
  double PatternLength() const;


  /*
    Description:
      Returns the number of segments in the pattern
  */
  int SegmentCount() const;

  /*
  Description:
    Adds a segment to the pattern
  Returns:
    Index of the added segment.
  */
  int AppendSegment( const MYON_LinetypeSegment& segment);

  /*
  Description:
    Removes a segment in the linetype.
  Parameters:
    index - [in]
      Zero based index of the segment to remove.
  Returns:
    True if the segment index was removed.
  */
  bool RemoveSegment( int index );

  /*
    Description:
      Sets the segment at index to match segment
  */
  bool SetSegment( int index, const MYON_LinetypeSegment& segment);

  /*
    Description:
      Sets the length and type of the segment at index
  */
  bool SetSegment( int index, double length, MYON_LinetypeSegment::eSegType type);

  /*
  Description:
    Set all segments
  Parameters:
    segments - [in]
  */
  bool SetSegments(const MYON_SimpleArray<MYON_LinetypeSegment>& segments);


  /*
    Description:
      Returns a copy of the segment at index
  */
  MYON_LinetypeSegment Segment( int index) const;

  /*
    Description:
      Expert user function to get access to the segment array
      for rapid calculations.
  */
  // Returns nullptr if the line pattern is locked.
  MYON_SimpleArray<MYON_LinetypeSegment>* ExpertSegments();

  const MYON_SimpleArray<MYON_LinetypeSegment>& Segments() const;

  /*
    Description:
      Set style used for end caps on open curves
  */
  void SetLineCapStyle(MYON::LineCapStyle style);

  /*
    Description:
      End cap style used on open curves
  */
  MYON::LineCapStyle LineCapStyle() const;

  /*
    Description:
      Set style used for corners on curves
  */
  void SetLineJoinStyle(MYON::LineJoinStyle style);

  /*
    Description:
      Corner join style for curves
  */
  MYON::LineJoinStyle LineJoinStyle() const;


  /*
    Description:
      Width of the linetype
  */
  double Width() const;

  /*
    Description:
      Set the width of the linetype
  */
  void SetWidth(double width);

  /*
    Description:
      Units used to define the linetype width
      None = width is defined in pixels (default)
      Unset = width is the same as the document's unitsystem
  */
  MYON::LengthUnitSystem WidthUnits() const;

  /*
    Description:
      Set the units used to define the linetype width
      None = width is defined in pixels (default)
      Unset = width is the same as the document's unitsystem
  */
  void SetWidthUnits(MYON::LengthUnitSystem units);

  /*
    Description:
      List of points defining a taper. For each point in the taper
      the point's x value is between 0.00 and 1.00 and represents the % along the length of the curve
      the point's y value is the width used at x
      Returns nullptr if no taper points exist for this linetype
  */
  const MYON_SimpleArray<MYON_2dPoint>* TaperPoints() const;

  /*
    Description:
      Set the taper for this linetype to a simple start and end width
  */
  bool SetTaper(double startWidth, double endWidth);

  /*
    Description:
      Set the taper for this linetype with a single taper point
  */
  bool SetTaper(double startWidth, MYON_2dPoint taperPoint, double endWidth);

  /*
    Description:
      Remove taper points from this linetype
  */
  void RemoveTaper();

private:
  mutable class MYON_LinetypePrivate* m_private = nullptr;
  unsigned char m_is_set_bits = 0;
  unsigned char m_is_locked_bits = 0;
  MYON::LineCapStyle m_cap_style = MYON::LineCapStyle::Round;
  MYON::LineJoinStyle m_join_style = MYON::LineJoinStyle::Round;
  double m_width = 1.0;
  MYON::LengthUnitSystem m_width_units = MYON::LengthUnitSystem::None;
  unsigned char m_reserved[7] = { 0 };

  enum : unsigned char
  {
    pattern_bit = 1
  };
};

#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_Linetype*>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<const MYON_Linetype*>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_ObjectArray<MYON_Linetype>;
#endif

#endif


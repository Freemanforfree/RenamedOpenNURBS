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

#if !defined(MYON_DITHERING_INC_)
#define MYON_DITHERING_INC_

class MYON_CLASS MYON_Dithering final
{
public:
  MYON_Dithering();
  MYON_Dithering(MYON_XMLNode& model_node);
  MYON_Dithering(const MYON_Dithering& dit);
  ~MYON_Dithering();

  const MYON_Dithering& operator = (const MYON_Dithering& dit);

  bool operator == (const MYON_Dithering& dit);
  bool operator != (const MYON_Dithering& dit);

  // Get dithering on / off state.
  bool On(void) const;

  // Set dithering on or off.
  void SetOn(bool b);

  enum class Methods { SimpleNoise, FloydSteinberg };

  // Get the dithering method.
  Methods Method(void) const;

  // Set the dithering method.
  void SetMethod(Methods m);

  // Returns the CRC of the dithering state.
  MYON__UINT32 DataCRC(MYON__UINT32 current_remainder) const;

private:
  class CImpl;
  CImpl* m_impl;
};

#endif

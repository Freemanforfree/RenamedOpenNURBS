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

#if !defined(MYON_RENDER_CHANNELS_INC_)
#define MYON_RENDER_CHANNELS_INC_

class MYON_CLASS MYON_RenderChannels final
{
public:
  MYON_RenderChannels();
  MYON_RenderChannels(MYON_XMLNode& model_node);
  MYON_RenderChannels(const MYON_RenderChannels& rch);
  ~MYON_RenderChannels();

  const MYON_RenderChannels& operator = (const MYON_RenderChannels& rch);

  bool operator == (const MYON_RenderChannels& rch);
  bool operator != (const MYON_RenderChannels& rch);

  enum class Modes : unsigned int
  {
    Automatic,
    Custom,
  };

  // Get the mode.
  Modes Mode(void) const;

  // Set the mode.
  void SetMode(Modes m);

  // Get the list of channels to render when in 'custom' mode.
  // - 'chan' accepts the channel ids. */
  void GetCustomList(MYON_SimpleArray<MYON_UUID>& chan) const;

  // Set the list of channels to render when in 'custom' mode.
  // - 'chan' contains the channel ids.
  void SetCustomList(const MYON_SimpleArray<MYON_UUID>& chan);

private:
  class CImpl;
  CImpl* m_impl;
};

#endif

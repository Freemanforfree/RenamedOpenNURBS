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

#if !defined(MYON_POST_EFFECTS_INC_)
#define MYON_POST_EFFECTS_INC_

class MYON_CLASS MYON_PostEffect : public MYON_ModelComponent
{
  MYON_OBJECT_DECLARE(MYON_PostEffect);

public:
  enum class Types : int
  {
    Unset,
    Early,
    ToneMapping,
    Late
  };

  MYON_PostEffect();
  MYON_PostEffect(const MYON_XMLNode& node, Types type);
  MYON_PostEffect(const MYON_PostEffect& pep);
  virtual ~MYON_PostEffect();

  const MYON_PostEffect& operator = (const MYON_PostEffect& pep);

  bool operator == (const MYON_PostEffect& pep);
  bool operator != (const MYON_PostEffect& pep);

  // Returns the type of this post effect. 
  Types Type(void) const;

  // Returns the localized name of this post effect.
  MYON_wString LocalName(void) const;

  // Returns true if the post effect is visible to the user. For early and late post effects, this is equivalent to 'shown'.
  // For tone-mapping post effects, this is equivalent to 'selected'. 
  bool IsVisible(void) const;

  // Returns true if the post effect is active. For early and late post effects, this is equivalent to 'shown' and 'on'.
  // For tone-mapping post effects, this is equivalent to 'selected'. 
  bool IsActive(void) const;

  // Get a parameter.
  // - param_name is the name of the parameter to set.
  // Returns the value if successful or null if the parameter was not found.
  MYON_XMLVariant GetParameter(const wchar_t* param_name) const;

  // Set a parameter.
  // - param_name is the name of the parameter to set.
  // - param_value specifies the type and value to set.
  // Returns true if successful or false if the parameter could not be set.
  bool SetParameter(const wchar_t* param_name, const MYON_XMLVariant& param_value);

  // Returns the XML node that stores the state of this post effect.
  const MYON_XMLNode& XML(void) const;

  // Returns a CRC of the state of this post effect.
  virtual MYON__UINT32 DataCRC(MYON__UINT32 current_remainder) const override;

  // If MYON_PostEffect::Cast(ref.ModelComponent()) is not null,
  // that pointer is returned. Otherwise, none_return_value is returned.
  static const MYON_PostEffect* FromModelComponentRef(const MYON_ModelComponentReference& ref,
                                                    const MYON_PostEffect* none_return_value);

public:
  class CImpl;
  CImpl* m_impl;
};

#endif

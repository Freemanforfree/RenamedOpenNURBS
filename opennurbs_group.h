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

#if !defined(OPENNURBS_GROUP_INC_)
#define OPENNURBS_GROUP_INC_

class MYON_CLASS MYON_Group : public MYON_ModelComponent
{
  MYON_OBJECT_DECLARE(MYON_Group);

public:
  static const MYON_Group Unset;   // nil id
                                    
  /*
  Parameters:
    model_component_reference - [in]
    none_return_value - [in]
      value to return if MYON_Material::Cast(model_component_ref.ModelComponent())
      is nullptr
  Returns:
    If MYON_Material::Cast(model_component_ref.ModelComponent()) is not nullptr,
    that pointer is returned.  Otherwise, none_return_value is returned. 
  */
  static const MYON_Group* FromModelComponentRef(
    const class MYON_ModelComponentReference& model_component_reference,
    const MYON_Group* none_return_value
    );

public:
  MYON_Group() MYON_NOEXCEPT;
  MYON_Group(const MYON_Group& src);
  ~MYON_Group() = default;
  MYON_Group& operator=(const MYON_Group& src) = default;

private:
  
  //////////////////////////////////////////////////////////////////////
  //
  // MYON_Object overrides
  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;

  void Dump(
    MYON_TextLog& text_log
    ) const override;

  bool Write(
    MYON_BinaryArchive& archive
    ) const override;

  bool Read(
    MYON_BinaryArchive& archive
    ) override;

private:
  bool Internal_WriteV5(
    MYON_BinaryArchive& archive
    ) const;

  bool Internal_ReadV5(
    MYON_BinaryArchive& archive
    );
};

#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_Group*>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<const MYON_Group*>;
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_ObjectArray<MYON_Group>;
#endif

#endif

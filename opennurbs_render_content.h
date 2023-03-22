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

#if !defined(MYON_RENDER_CONTENT_INC_)
#define MYON_RENDER_CONTENT_INC_

// MYON_Environment

class MYON_CLASS MYON_Environment : public MYON_Object
{
  MYON_OBJECT_DECLARE(MYON_Environment);

public:
  MYON_Environment();
  MYON_Environment(const MYON_Environment& src);
  virtual ~MYON_Environment();

  const MYON_Environment& operator = (const MYON_Environment& src);

  bool operator == (const MYON_Environment& src) const;
  bool operator != (const MYON_Environment& src) const;

  enum class BackgroundProjections : unsigned int
  {
    Planar = 0,
    Spherical = 1, // Equirectangular projection.
    Emap = 2,      // Mirror ball.
    Box = 3,
    Automatic = 4,
    LightProbe = 5,
    CubeMap = 6,
    VerticalCrossCubeMap = 7,
    HorizontalCrossCubeMap = 8,
    Hemispherical = 9,
  };

  MYON_Color BackgroundColor(void) const;
  void SetBackgroundColor(MYON_Color color);

  MYON_Texture BackgroundImage(void) const;
  void SetBackgroundImage(const MYON_Texture& tex);

  BackgroundProjections BackgroundProjection(void) const;
  void SetBackgroundProjection(BackgroundProjections proj);

  static BackgroundProjections ProjectionFromString(const wchar_t* proj);
  static const wchar_t* StringFromProjection(BackgroundProjections proj);

protected:
  /** Emergency virtual function for future expansion. */
  virtual void* EVF(const wchar_t* wszFunc, void* pvData);

private:
  class CImpl;
  CImpl* m_impl;
};

class MYON_CLASS MYON_RenderContent : public MYON_ModelComponent
{
  MYON_OBJECT_DECLARE(MYON_RenderContent);

public:
  MYON_RenderContent(const wchar_t* kind);
  MYON_RenderContent(const MYON_RenderContent& c);
  virtual ~MYON_RenderContent();

  const MYON_RenderContent& operator = (const MYON_RenderContent& rc);

  // Returns: The internal name of the content type.
  MYON_wString TypeName(void) const;

  // Set the content's type name.
  void SetTypeName(const wchar_t* name);

  // Returns: The unique id of the content type.
  MYON_UUID TypeId(void) const;

  // Set the content's type id.
  void SetTypeId(const MYON_UUID& uuid);

  // Returns: The content's render-engine id.
  MYON_UUID RenderEngineId(void) const;

  // Set the content's render-engine id.
  void SetRenderEngineId(const MYON_UUID& uuid);

  // Returns: The content's plug-in id.
  MYON_UUID PlugInId(void) const;

  // Set the content's plug-in id.
  void SetPlugInId(const MYON_UUID& uuid);

  // Returns: The content's notes.
  MYON_wString Notes(void) const;

  // Sets the content's notes.
  void SetNotes(const wchar_t* notes);

  // Returns: The content's tags.
  MYON_wString Tags(void) const;

  // Sets the content's tags.
  void SetTags(const wchar_t* tags);

  // Returns: The content's group id.
  MYON_UUID GroupId(void) const;

  // Sets the content's group id.
  void SetGroupId(const MYON_UUID& group);

  // Returns: True if the content is hidden.
  bool Hidden(void) const;

  // Sets whether or not the content is hidden.
  void SetHidden(bool hidden);

  // Returns: True if the content is a reference content.
  bool Reference(void) const;

  // Sets whether or not the content is a reference content.
  void SetReference(bool ref);

  // Returns: True if the content is automatically deleted when not in use.
  bool AutoDelete(void) const;

  // Sets whether or not the content is automatically deleted when not in use.
  void SetAutoDelete(bool autodel);

  // Gets a variant giving the type and value of the parameter, if found.
  // If the parameter is not found, the function returns a null variant.
  MYON_XMLVariant GetParameter(const wchar_t* name) const;

  // Sets the value of a named parameter.
  // Returns: True if successful, else false.
  bool SetParameter(const wchar_t* name, const MYON_XMLVariant& value);

  class MYON_CLASS ChildIterator
  {
  public:
    ChildIterator(const MYON_RenderContent* pParent);
    virtual ~ChildIterator();

    virtual MYON_RenderContent* GetNextChild(void);

  protected:
    /** Emergency virtual function for future expansion. */
    virtual void* EVF(const wchar_t* func, void* data);

  private:
    class CImpl;
    CImpl* m_impl;
  };

  // Returns: An iterator for iterating over the content's children.
  ChildIterator GetChildIterator(void) const;

  // Returns: The parent content or null if this is the top level object.
  const MYON_RenderContent* Parent(void) const;

  // Returns: The first child of this content or null if none.
  const MYON_RenderContent* FirstChild(void) const;

  // Returns: The first sibling of this content or null if none.
  const MYON_RenderContent* NextSibling(void) const;

  // Returns: The top level parent of this content.
  const MYON_RenderContent& TopLevel(void) const;

  // Returns: True if this is a top-level content (i.e., has no parent; is not a child).
  bool IsTopLevel(void) const;

  // Returns: True if this is a child content (i.e., has a parent; is not top-level).
  bool IsChild(void) const;

  // Sets another content as a child of this content.
  // Param child is the content to set as a child of this content. This content is copied and the
  // copy is attached to the model. If the child is already attached to a model the function will fail.
  // If the child is already a child of this or another content, the function will fail.
  // Param childSlotName is the child slot name that will be assigned to this child.
  // The child slot name cannot be an empty string. If it is, the function will fail.
  // Returns: True if successful, else false.
  bool SetChild(const MYON_RenderContent& child, const wchar_t* child_slot_name);

  // Returns: The content's child-slot name.
  MYON_wString ChildSlotName(void) const;

  // Sets the content's child-slot name.
  void SetChildSlotName(const wchar_t* child_slot_name);

  // Returns true if the child slot with the specified child slot name is turned on, else false.
  // Also returns false if there is no child with the specified child slot name.
  bool ChildSlotOn(const wchar_t* child_slot_name) const;

  // Turns the child slot with the specified child slot name on or off.
  // Returns: True if successful, else false.
  bool SetChildSlotOn(bool on, const wchar_t* child_slot_name);

  // Deletes any existing child with the specified child slot name.
  // Returns: True if successful, else false.
  bool DeleteChild(const wchar_t* child_slot_name);

  // Returns: The child with the specified child slot name, or null if no such child exists.
  const MYON_RenderContent* FindChild(const wchar_t* child_slot_name) const;

  // Get the render content's state as an XML string.
  MYON_wString XML(bool recursive) const;

  // Set the render content's state from an XML string.
  bool SetXML(const wchar_t* xml);

  // Returns the XML node that stores the state of this render content. Note that this does not include
  // any children of the render content. An MYON_RenderContent only stores its own XML.
  const MYON_XMLNode& XMLNode(void) const;

  // Returns the kind of render content as a string.
  MYON_wString Kind(void) const;

  // If MYON_RenderContent::Cast(ref.ModelComponent()) is not null,
  // that pointer is returned. Otherwise, none_return_value is returned.
  static const MYON_RenderContent* FromModelComponentRef(const MYON_ModelComponentReference& ref,
                                                       const MYON_RenderContent* none_return_value);

protected:
  /** Emergency virtual function for future expansion. */
  virtual void* EVF(const wchar_t* func, void* data);

public:
  class CImpl;
  CImpl* m_impl;

protected:
  MYON__UINT8 m_Impl[328];
};

class MYON_CLASS MYON_RenderMaterial : public MYON_RenderContent
{
  MYON_OBJECT_DECLARE(MYON_RenderMaterial);

public:
  MYON_RenderMaterial();
  MYON_RenderMaterial(const MYON_RenderMaterial& m);
  virtual ~MYON_RenderMaterial();

  void operator = (const MYON_RenderMaterial&);

  MYON_Material SimulatedMaterial(void) const;
};

class MYON_CLASS MYON_RenderEnvironment : public MYON_RenderContent
{
  MYON_OBJECT_DECLARE(MYON_RenderEnvironment);

public:
  MYON_RenderEnvironment();
  MYON_RenderEnvironment(const MYON_RenderEnvironment& e);
  virtual ~MYON_RenderEnvironment();

  void operator = (const MYON_RenderEnvironment&);

  MYON_Environment SimulatedEnvironment(void) const;
};

class MYON_CLASS MYON_RenderTexture : public MYON_RenderContent
{
  MYON_OBJECT_DECLARE(MYON_RenderTexture);

public:
  MYON_RenderTexture();
  MYON_RenderTexture(const MYON_RenderTexture& t);
  virtual ~MYON_RenderTexture();

  void operator = (const MYON_RenderTexture&);

  MYON_Texture SimulatedTexture(void) const;
};

#endif

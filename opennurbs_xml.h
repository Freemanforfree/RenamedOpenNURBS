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

#if !defined(MYON_XML_INC_)
#define MYON_XML_INC_

typedef bool (*MYON_XMLRecurseChildrenCallback)(class MYON_XMLNode*, void*);

// This is the structure of the RDK document XML.

#define MYON_RDK_DOCUMENT                             L"render-content-manager-document"

  #define MYON_RDK_CURRENT_CONTENT                      L"content"
  #define MYON_RDK_ENVIRONMENT                          L"environment"
  #define MYON_RDK_DEFAULT_CONTENT_SECTION              L"default-content-section"

  #define MYON_RDK_SETTINGS                             L"settings"
    #define MYON_RDK_NAMED_VIEWS                          L"named-views"
    #define MYON_RDK_NAMED_CPLANES                        L"named-cplanes"
    #define MYON_RDK_NAMED_POSITIONS                      L"named-positions"
    #define MYON_RDK_NAMED_SNAPSHOTS                      L"named-snapshots"
    #define MYON_RDK_SORT_MODE                            L"sort-mode"
      #define MYON_RDK_SORT_MODE_ASCENDING                  L"ascending"
      #define MYON_RDK_SORT_MODE_DESCENDING                 L"descending"
      #define MYON_RDK_SORT_MODE_CUSTOM                     L"custom"

    #define MYON_RDK_MISCELLANEOUS                      L"miscellaneous"
      #define MYON_RDK_CUSTOM_IMAGE_SIZE_IS_PRESET        L"custom-image-size-is-preset"
      #define MYON_RDK_NAME_COLLISION_SUPPRESS            L"smart-merge-name-collision-suppress"
        #define MYON_RDK_IMPORT                             L"import"
        #define MYON_RDK_PASTE                              L"paste"

    #define MYON_RDK_EXCLUDED_RENDER_ENGINES            L"excluded-render-engines"
      #define MYON_RDK_UUIDS                              L"uuids"

    #define MYON_RDK_FILTERS                            L"filters"
      #define MYON_RDK_NAME_FILTER                        L"name-filter"
      #define MYON_RDK_NAME_FILTER_INVERT                 L"name-filter-invert"
      #define MYON_RDK_SHOW_UNASSIGNED                    L"show-unassigned-materials"
      #define MYON_RDK_SHOW_V4                            L"show-v4-materials"
      #define MYON_RDK_SHOW_HIDDEN                        L"show-hidden-materials"
      #define MYON_RDK_SHOW_REFERENCE                     L"show-reference-materials"

    #define MYON_RDK_POST_EFFECTS                       L"post-effects-new"
      #define MYON_RDK_PEP_TYPE_EARLY                     L"early"
      #define MYON_RDK_PEP_TYPE_TONE                      L"tone-mapping"
      #define MYON_RDK_PEP_TYPE_LATE                      L"late"
      #define MYON_RDK_PEP_SELECTION                      L"selection"
      #define MYON_RDK_PEP_SELECTION_POSTFIX              L"-" MYON_RDK_PEP_SELECTION
      #define MYON_RDK_PEP_EARLY_SELECTION                MYON_RDK_PEP_TYPE_EARLY MYON_RDK_PEP_SELECTION_POSTFIX
      #define MYON_RDK_PEP_TONE_SELECTION                 MYON_RDK_PEP_TYPE_TONE  MYON_RDK_PEP_SELECTION_POSTFIX
      #define MYON_RDK_PEP_LATE_SELECTION                 MYON_RDK_PEP_TYPE_LATE  MYON_RDK_PEP_SELECTION_POSTFIX

    #define MYON_RDK_RENDERING                          L"rendering"

      #define MYON_RDK_RENDER_CHANNELS                    L"render-channels"
        #define MYON_RDK_RCH_LIST                           L"list"
        #define MYON_RDK_RCH_MODE                           L"mode"
          #define MYON_RDK_RCH_MODE_AUTOMATIC                 L"automatic"
          #define MYON_RDK_RCH_MODE_CUSTOM                    L"custom"

      #define MYON_RDK_CURRENT_PRESET                     L"current-preset"
      #define MYON_RDK_EMBED_SUPPORT_FILES_ON             L"embed-support-files-on"
      #define MYON_RDK_GAMMA                              L"gamma"
      #define MYON_RDK_USE_DITHERING                      L"use-dithering"
      #define MYON_RDK_USE_POST_PROCESS_GAMMA             L"use-post-process-gamma"
      #define MYON_RDK_USE_LINEAR_WORKFLOW                L"use-linear-workflow"

      #define MYON_RDK_CUSTOM_REFLECTIVE_ENVIRONMENT_ON   L"custom-env-for-refl-and-refr-on"
        #define MYON_RDK_CUSTOM_REFLECTIVE_ENVIRONMENT      L"custom-env-for-refl-and-refr"

      #define MYON_RDK_DITHERING                          L"dithering"
        #define MYON_RDK_DITHERING_FLOYD_STEINBERG          L"floyd-steinberg"
        #define MYON_RDK_DITHERING_SIMPLE_NOISE             L"simple-noise"

    #define MYON_RDK_SUN                                L"sun"
      #define MYON_RDK_SUN_ENABLE_ALLOWED                 L"enable-allowed"
      #define MYON_RDK_SUN_ENABLE_ON                      L"enable-on"
      #define MYON_RDK_SUN_MANUAL_CONTROL_ALLOWED         L"manual-control-allowed"
      #define MYON_RDK_SUN_MANUAL_CONTROL_ON              L"manual-control-on"
      #define MYON_RDK_SUN_NORTH                          L"north"
      #define MYON_RDK_SUN_AZIMUTH                        L"sun-azimuth"
      #define MYON_RDK_SUN_ALTITUDE                       L"sun-altitude"
      #define MYON_RDK_SUN_DATE_YEAR                      L"year"
      #define MYON_RDK_SUN_DATE_MONTH                     L"month"
      #define MYON_RDK_SUN_DATE_DAY                       L"day"
      #define MYON_RDK_SUN_TIME_HOURS                     L"time"
      #define MYON_RDK_SUN_DAYLIGHT_SAVING_ON             L"daylight-saving-on"
      #define MYON_RDK_SUN_DAYLIGHT_SAVING_MINUTES        L"daylight-saving-minutes"
      #define MYON_RDK_SUN_OBSERVER_LATITUDE              L"observer-latitude"
      #define MYON_RDK_SUN_OBSERVER_LONGITUDE             L"observer-longitude"
      #define MYON_RDK_SUN_OBSERVER_TIMEZONE              L"observer-timezone"
      #define MYON_RDK_SUN_SKYLIGHT_ON                    L"skylight-on"
      #define MYON_RDK_SUN_SKYLIGHT_SHADOW_INTENSITY      L"skylight-shadow-intensity"
      #define MYON_RDK_SUN_SKYLIGHT_CUSTOM_ENVIRONMENT_ON L"skylight-custom-environment-on"
      #define MYON_RDK_SUN_SKYLIGHT_CUSTOM_ENVIRONMENT    L"skylight-custom-environment"
      #define MYON_RDK_SUN_SHADOW_INTENSITY               L"shadow-intensity"
      #define MYON_RDK_SUN_INTENSITY                      L"intensity"

    #define MYON_RDK_SAFE_FRAME                         L"safe-frame"
      #define MYON_RDK_SF_ON                              L"on"
      #define MYON_RDK_SF_PERSPECTIVE_ONLY                L"perspective-only"
      #define MYON_RDK_SF_4x3_FIELD_DISPLAY_ON            L"field-display-on"
      #define MYON_RDK_SF_LIVE_FRAME                      L"live-frame"
      #define MYON_RDK_SF_ACTION_FRAME                    L"action-frame"
      #define MYON_RDK_SF_TITLE_FRAME                     L"title-frame"
        #define MYON_RDK_SFF_ON                             L"on"
        #define MYON_RDK_SFF_XSCALE                         L"x-scale"
        #define MYON_RDK_SFF_YSCALE                         L"y-scale"
        #define MYON_RDK_SFF_LINK                           L"link"

    #define MYON_RDK_GROUND_PLANE                       L"ground-plane"
      #define MYON_RDK_GP_ON                              L"on"
      #define MYON_RDK_GP_ALTITUDE                        L"altitude"
      #define MYON_RDK_GP_MATERIAL                        L"material"
      #define MYON_RDK_GP_TEXTURE_OFFSET                  L"texture-offset"
      #define MYON_RDK_GP_TEXTURE_SIZE                    L"texture-size"
      #define MYON_RDK_GP_TEXTURE_ROTATION                L"texture-rotation"
      #define MYON_RDK_GP_OFFSET_LOCK                     L"offset-lock"
      #define MYON_RDK_GP_REPEAT_LOCK                     L"repeat-lock"
      #define MYON_RDK_GP_SHOW_UNDERSIDE                  L"show-underside"
      #define MYON_RDK_GP_AUTO_ALTITUDE                   L"auto-altitude"
      #define MYON_RDK_GP_SHADOW_ONLY                     L"shadow-only"

#define MYON_RDK_POSTFIX_SECTION  L"-section"
#define MYON_RDK_SLASH            L"/"

class MYON_CLASS MYON_XMLVariant final
{
public:
  enum class Types : unsigned int
  {
    Null,
    Bool, Integer, Float, Double, String,
    DoubleArray2, DoubleArray3, DoubleArray4, DoubleColor4,
    Matrix, Uuid, Time, Buffer
  };

  enum class ArrayTypes : unsigned int
  {
    Array2, Array3, Array4, Array16
  };

public:
  MYON_XMLVariant(); // Creates an empty string type.
  MYON_XMLVariant(bool value);
  MYON_XMLVariant(int value);
  MYON_XMLVariant(float value);
  MYON_XMLVariant(double value);
  MYON_XMLVariant(time_t time);
  MYON_XMLVariant(const MYON_wString& value);
  MYON_XMLVariant(const MYON_UUID& uuid);
  MYON_XMLVariant(const MYON_2dPoint& p);
  MYON_XMLVariant(const MYON_3dPoint& p);
  MYON_XMLVariant(const MYON_4fColor& c);
  MYON_XMLVariant(const MYON_Xform& xform);
  MYON_XMLVariant(const double* point, ArrayTypes at=ArrayTypes::Array3);
  MYON_XMLVariant(const float*  point, ArrayTypes at=ArrayTypes::Array3);
  MYON_XMLVariant(const wchar_t* wsz);
  MYON_XMLVariant(const void* buffer, size_t size);
  MYON_XMLVariant(const MYON_Buffer& buffer);
  MYON_XMLVariant(const MYON_XMLVariant& src);
  ~MYON_XMLVariant();

  const MYON_XMLVariant& operator = (const MYON_XMLVariant& src);

  bool operator == (const MYON_XMLVariant& v) const;
  bool operator != (const MYON_XMLVariant& v) const;

  MYON__UINT32 DataCRC(MYON__UINT32 current_remainder) const;

public:
  Types Type(void) const;
  MYON_wString TypeAsString(void) const;
  bool IsEmpty(void) const;
  bool IsNull(void) const;

  MYON::LengthUnitSystem Units(void) const;
  void SetUnits(MYON::LengthUnitSystem units);

  // Optimized version of MYON_wString::NeedsXMLEncode(). Always use this if possible
  // because it knows from the type of the variant whether entity ref encoded output is even possible.
  bool NeedsXMLEncode(void) const;

  MYON_Buffer& GetBuffer(void) const;
  void ClearBuffers(void);

public:
  void SetNull(void);
  void SetValue(int v);
  void SetValue(double v);
  void SetValue(float v);
  void SetValue(const wchar_t* s);
  void SetValue(const MYON_wString& s);
  void SetValue(const double* p, ArrayTypes at);
  void SetValue(const float* p, ArrayTypes at);
  void SetValue(const MYON_2dPoint& p);
  void SetValue(const MYON_3dPoint& p);
  void SetValue(const MYON_4dPoint& p);
  void SetValue(const MYON_4fColor& c);
  void SetValue(bool b);
  void SetValue(time_t time);
  void SetValue(const MYON_UUID& uuid);
  void SetValue(const MYON_Xform& xform);
  void SetValue(const void* buffer, size_t size);
  void SetValue(const MYON_Buffer& buffer);

  bool       AsBool(void) const;
  int        AsInteger(void) const;
  double     AsDouble(void) const;
  float      AsFloat(void) const;
  MYON_2dPoint As2dPoint(void) const;
  MYON_3dPoint As3dPoint(void) const;
  MYON_4dPoint As4dPoint(void) const;
  MYON_UUID    AsUuid(void) const;
  MYON_Xform   AsXform(void) const;
  time_t     AsTime(void) const;
  MYON_4fColor AsColor(void) const;
  MYON_Buffer  AsBuffer(void) const;
  void*      AsBuffer(size_t& size_out) const;
  MYON_wString AsString(void) const;

  operator bool() const;
  operator double() const;
  operator float() const;
  operator int() const;
  operator MYON_2dPoint() const;
  operator MYON_3dPoint() const;
  operator MYON_4dPoint() const;
  operator MYON_4fColor() const;
  operator MYON_UUID() const;
  operator time_t() const;
  operator MYON_Buffer() const;
  operator MYON_wString() const;

  void DoAutoTyping(Types t) const;

public:
  bool TypePending(void) const;
  void SetTypePendingFlag(bool bTypePending) const;

  // Format string as type and value. Useful for debugging.
  void Format(MYON_wString& s) const;

protected:
  void StringToPoint(int iValues) const;

private:
  class CImpl;
  CImpl* m_impl;
  MYON__UINT8 m_Impl[176];
};

class MYON_CLASS MYON_XMLProperty final
{
public:
  MYON_XMLProperty();
  MYON_XMLProperty(const MYON_XMLVariant& value);
  MYON_XMLProperty(const MYON_wString& sName, const MYON_XMLVariant& value);
  MYON_XMLProperty(const MYON_XMLProperty& prop);
  ~MYON_XMLProperty();

  const MYON_XMLProperty& operator = (const MYON_XMLProperty& prop);

  const MYON_wString& Name(void) const;
  void SetName(const wchar_t* name);

  MYON__UINT32 DataCRC(MYON__UINT32 current_remainder) const;

  MYON_XMLProperty* Next(void) const;

  bool IsDefaultProperty(void) const;

  const MYON_XMLVariant& GetValue(void) const;
  void SetValue(const MYON_XMLVariant& value);

  bool operator < (const MYON_XMLProperty& prop) const;

  MYON_XMLVariant& GetNonConstValue(void);

  void SetHugeStringValue(const MYON_wString& s);

private:
  class CImpl;
  CImpl* m_impl;
  MYON__UINT8 m_Impl[32];

public:
  CImpl& Impl(void) const;
};

class MYON_CLASS MYON_XMLSegmentedStream
{
public:
  MYON_XMLSegmentedStream();
  virtual ~MYON_XMLSegmentedStream();

  int  Count(void) const;
  void Append(wchar_t* s);
  wchar_t* Segment(int index) const;

protected:
  // Emergency virtual function for future expansion.
  virtual void* EVF(const wchar_t* func, void* data);

  MYON_XMLSegmentedStream(const MYON_XMLSegmentedStream&) = delete;
  const MYON_XMLSegmentedStream& operator = (const MYON_XMLSegmentedStream&) = delete;

private:
  class CImpl;
  CImpl* m_impl;
  MYON__UINT8 m_Impl[32];
};

class MYON_CLASS MYON_XMLNode
{
public:
  MYON_XMLNode(const wchar_t* name); // Always creates empty default property.
  MYON_XMLNode(const MYON_XMLNode&);
  MYON_XMLNode(MYON_XMLNode&&) noexcept = delete;
  virtual ~MYON_XMLNode();

  const MYON_XMLNode& operator = (const MYON_XMLNode&);

  bool MergeFrom(const MYON_XMLNode& src); // src node must have the same name

  const MYON_wString& TagName(void) const;
  void SetTagName(const wchar_t* name);

  static bool IsValidXMLNameWithDebugging(const wchar_t* name);
  static bool IsValidXMLName(const wchar_t* name);
  static MYON_wString GetNameFromTag(const wchar_t* tag);

public: // Hierarchy.
  // Get the first child of this node or null if none.
  MYON_XMLNode* FirstChild(void) const;

  // Get the previous sibling of this node or null if none.
  MYON_XMLNode* PrevSibling(void) const;

  // Get the next sibling of this node or null if none.
  MYON_XMLNode* NextSibling(void) const;

  // Get the parent of this node or null if none.
  MYON_XMLNode* GetParent(void) const;

  // Get the topmost parent of this node or null if none.
  const MYON_XMLNode& TopmostParent(void) const;

  // Recursively iterate over the children of this node, calling the supplied callback function for each child.
  // If the callback function returns false, this function will fail.
  // Returns true if successful, false on failure.
  bool RecurseChildren(MYON_XMLRecurseChildrenCallback callback, void* data) const;

public: // Change data.
  // Adds a node as a child of this node. Takes ownership of node.
  // Returns a pointer to node for convenience.
  MYON_XMLNode* AttachChildNode(MYON_XMLNode* node);

  // DEPRECATED, use AttachChildNode()
  MYON_DEPRECATED MYON_XMLNode* AddChildNode(MYON_XMLNode* node);

  // Attaches a property directly to the node. Takes ownership of the property.
  // Returns a pointer to the copy of the property.
  MYON_XMLProperty* AttachProperty(MYON_XMLProperty* prop);

  // Adds a property to the node. The property object is copied and ownership remains with the caller.
  // Returns a pointer to the copy of the property.
  MYON_XMLProperty* AddProperty(const MYON_XMLProperty& prop);

  // Removes and deletes this node.
  void Remove(void);

  // Removes and deletes the child node, if possible.
  // Returns true if successful, else false.
  bool RemoveChild(MYON_XMLNode* child);

  // Removes and deletes the named property, if possible.
  // Returns true if successful, else false.
  bool RemoveProperty(const wchar_t* prop_name);

  // Removes the child node and passes ownership to the caller.
  // Returns the detached node or null on failure.
  MYON_XMLNode* DetachChild(MYON_XMLNode& child);

  // Removes and deletes all child nodes.
  void RemoveAllChildren(void);

  // Removes and deletes all properties and adds an empty default property.
  void RemoveAllProperties(void);

  // Removes and deletes all child nodes and properties, and clears the tag name.
  virtual void Clear(void);

  // Moves this node before another node.
  void MoveBefore(MYON_XMLNode& other);

  // Moves this node after another node.
  void MoveAfter(MYON_XMLNode& other);

public: // Serialization.
  static constexpr MYON__UINT32 ReadError = 0xFFFFFFFF; // Indicates ReadFromStream() failure.
  MYON__UINT32 ReadFromStream(const wchar_t* buf, bool warnings_as_errors=false, bool validate_tags=true);

  void* LastReadBufferPointer(void) const;

  // This function is called on the top-most node during the reading process.
  virtual void OnNodeReadFromStream(const MYON_XMLNode* node) const;

  MYON_wString String(bool include_formatting=true, bool force_long_format=false, bool sorted_props=false) const;
  operator MYON_wString() const;

  // Writes the node to a wide buffer (AKA 'stream') as XML text.
  // max_chars is the maximum number of wide characters that the buffer can accommodate.
  // To find out how much space is needed without actually writing anything, pass zero.
  // When writing to the buffer, a terminator is written if there is room for it, but the
  // terminator is not included in the returned number of characters.
  MYON__UINT32 WriteToStream(wchar_t* stream, MYON__UINT32 max_chars, bool include_formatting = true, bool force_long_format = false, bool sorted_props = false) const;

  class MYON_CLASS CharacterCounts
  {
  public:
    void operator += (const CharacterCounts& cw);

    // Number of characters that would have been written if the buffer was big enough.
    // This value does not include the terminator.
    MYON__UINT32 m_logical = 0;

    // Number of characters that were physically written. Always zero if max_chars is zero.
    // It is otherwise usually the same as m_logical, but less when the buffer is too small.
    // This value does not include the terminator.
    MYON__UINT32 m_physical = 0;

    MYON__UINT64 m_reserved = 0;
  };

  CharacterCounts WriteToStreamEx      (wchar_t* stream, MYON__UINT32 max_chars, bool include_formatting = true, bool force_long_format = false, bool sorted_props = false) const;
  CharacterCounts WriteHeaderToStream  (wchar_t* stream, MYON__UINT32 max_chars, bool include_formatting = true, bool force_long_format = false, bool sorted_props = false) const;
  CharacterCounts WriteChildrenToStream(wchar_t* stream, MYON__UINT32 max_chars, bool include_formatting = true, bool force_long_format = false, bool sorted_props = false) const;
  CharacterCounts WriteFooterToStream  (wchar_t* stream, MYON__UINT32 max_chars, bool include_formatting = true, bool force_long_format = false) const;

  bool WriteToSegmentedStream(MYON_XMLSegmentedStream& stream, bool include_formatting=true, bool force_long_format=false, bool sorted_props=false) const;

public: // Utilities.
  int PropertyCount(void) const;
  int ChildCount(void) const;

  int GetNestedDepth(void) const;

  // Get the CRC of this node. This assumes the node has the following format:
  // <node prop1=value1 prop2=value2 ... >
  //   <param-name><type="float">1.23456</param-name>
  //   ...
  // </node>
  MYON__UINT32 DataCRC(MYON__UINT32 current_remainder) const;

  // Use this when you are looking for a node that is only one child below - non-recursive, but fast.
  MYON_XMLNode* GetNamedChild(const wchar_t* name) const;

  MYON_XMLProperty* GetNamedProperty(const wchar_t* name) const;

  MYON_XMLProperty& GetDefaultProperty(void) const;

  // Returns the full path to this node.
  MYON_wString GetPathFromRoot(void) const;

  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Gets at nodes deep into the tree using a slash-delimited path, i.e., "child/grandchild/great-grandchild".
  // There's no checking for multiple nodes with the same name at each level of the tree, so if you use these
  // methods, you have to make sure you have unique node names at each level.

  // Gets a child node given the relative path from the current node. If the node does not exist, the method
  // returns null. The returned pointer should not be deleted by the caller. The child node is owned by its
  // immediate parent at that position in the node hierarchy.
  MYON_XMLNode* GetNodeAtPath(const wchar_t* path) const;

  // Gets a child node given the relative path from the current node. If the node does not exist, it is
  // created. This method should therefore never return null. The returned pointer should not be deleted
  // by the caller. The child node is owned by its immediate parent at that position in the node hierarchy.
  MYON_XMLNode* CreateNodeAtPath(const wchar_t* path);

  /////////////////////////////////////////////////////////////////////////////////////////////////////////

protected:
  // Emergency virtual function for future expansion.
  virtual void* EVF(const wchar_t* func, void* data);

public: // Iteration.
  class MYON_CLASS ChildIterator
  {
  public:
    ChildIterator(const MYON_XMLNode* parent);
    ChildIterator(const ChildIterator& other);
    virtual ~ChildIterator();

    const ChildIterator& operator = (const ChildIterator& other);

    virtual MYON_XMLNode* GetNextChild(void);

  protected:
    // Emergency virtual function for future expansion.
    virtual void* EVF(const wchar_t* func, void* data);

  private:
    class CImpl;
    CImpl* m_impl;
    MYON__UINT8 m_Impl[24];
  };

  class MYON_CLASS PropertyIterator final
  {
  public:
    PropertyIterator(const MYON_XMLNode* parent, bool sorted = false);
    PropertyIterator(const PropertyIterator& other);
    ~PropertyIterator();

    MYON_XMLProperty* GetNextProperty(void);

    const PropertyIterator& operator = (const PropertyIterator& other);

  private:
    class CImpl;
    CImpl* m_impl;
    MYON__UINT8 m_Impl[64];
  };

  ChildIterator GetChildIterator(void) const;
  PropertyIterator GetPropertyIterator(bool alphabetized = false) const;

  static bool AutoTypePropValue(void);
  static void SetAutoTypePropValue(bool b=true);

  // For internal use only.
  void SetInternalDebuggingFlags(MYON__UINT64);

private:
  class CImpl;
  CImpl* m_impl;
  MYON__UINT8 m_Impl[168];
  CImpl& Impl(void) const;
};

class MYON_CLASS MYON_XMLRootNode : public MYON_XMLNode
{
public:
  MYON_XMLRootNode();
  MYON_XMLRootNode(const MYON_XMLRootNode&);
  virtual ~MYON_XMLRootNode();

  const MYON_XMLRootNode& operator = (const MYON_XMLRootNode&);

  const MYON_XMLRootNode& NodeForRead(void) const;
  MYON_XMLRootNode& NodeForWrite(void);

  bool ReadFromFile(const wchar_t* path, bool warnings_as_errors=false, bool validate_tags=true);
  bool WriteToFile (const wchar_t* path, bool include_formatting=true, bool utf8=false, bool sorted_properties=false) const;

  virtual void Clear(void) override;

private:
  class CImpl;
  CImpl* m_impl;
  MYON__UINT8 m_Impl[24];
  CImpl& Impl(void) const;
};

class MYON_CLASS MYON_XMLUserData : public MYON_UserData
{
public:
  MYON_XMLUserData();
  MYON_XMLUserData(const MYON_XMLUserData& ud);
  virtual ~MYON_XMLUserData();

  const MYON_XMLUserData& operator = (const MYON_XMLUserData& ud);

  const MYON_XMLRootNode& XMLRootForRead(void) const;
  MYON_XMLRootNode& XMLRootForWrite(void) const;

  MYON_XMLProperty* Property(const wchar_t* xml_path, const wchar_t* prop_name) const;

  MYON_XMLVariant Value(const wchar_t* xml_path, const wchar_t* prop_name = L"") const;
  bool SetValue(const wchar_t* xml_path, const wchar_t* prop_name, const MYON_XMLVariant& value);
  void SetValue(const wchar_t* xml_path, const MYON_XMLVariant& value);

  void Clear(void) const;

  int Version(void) const;
  void _Dump(const wchar_t* wszFileName) const;

  virtual bool Archive(void) const override;

  virtual void SetToDefaults(void) const = 0;
  virtual void ReportVersionError(void) const = 0;

  virtual bool Write(MYON_BinaryArchive&) const override;
  virtual bool Read(MYON_BinaryArchive&) override;

protected:
  // Emergency virtual function for future expansion.
  virtual void* EVF(const wchar_t* wszFunc, void* pvData);

protected:
  MYON_XMLProperty* InternalProperty(const wchar_t* xml_path, const wchar_t* prop_name) const;

public:
  void SetToDefaultsImpl(int) const;

private:
  class CImpl;
  CImpl* m_impl;
  MYON__UINT8 m_Impl[224];
  CImpl& Impl(void) const;
};

class MYON_CLASS MYON_XMLParameters
{
public:
  MYON_XMLParameters(MYON_XMLNode& node);
  MYON_XMLParameters(const MYON_XMLNode& node);
  MYON_XMLParameters(const MYON_XMLParameters&) = delete;
  virtual ~MYON_XMLParameters();

  const MYON_XMLParameters& operator = (const MYON_XMLParameters&) = delete;

  void SetWriteTypeProperty(bool b);
  void SetDefaultReadType(const wchar_t* type);

  virtual MYON_wString AsString(void) const;
  virtual void SetAsString(const wchar_t* s);

  virtual MYON_XMLNode* SetParam(const wchar_t* param_name, const MYON_XMLVariant& param_value);
  virtual bool GetParam(const wchar_t* param_name, MYON_XMLVariant& param_value_out) const;

  class MYON_CLASS CIterator
  {
  public:
    CIterator(const MYON_XMLParameters& p);
    virtual ~CIterator();

    bool Next(MYON_wString& param_name_out, MYON_XMLVariant& param_value_out) const;

  protected:
    virtual void* EVF(const wchar_t*, void*);

  private:
    class CImpl;
    CImpl* m_impl;
  };
  CIterator* NewIterator(void) const;

protected:
  const MYON_XMLNode& Node(void) const;
  virtual bool GetParamNode(const MYON_XMLNode& node, MYON_XMLVariant& param_value) const;
  virtual void* EVF(const wchar_t*, void*);
  virtual MYON_XMLNode* SetParamNode(MYON_XMLNode& node, const wchar_t* param_name, const MYON_XMLVariant& param_value);
  virtual MYON_XMLNode* ObtainChildNodeForWrite(MYON_XMLNode& node, const wchar_t* param_name) const;

private:
  class CImpl;
  CImpl* m_impl;
};

class MYON_CLASS MYON_XMLParametersV8 : public MYON_XMLParameters
{
public:
  MYON_XMLParametersV8(MYON_XMLNode& node);
  MYON_XMLParametersV8(const MYON_XMLNode& node);

  virtual bool GetParam(const wchar_t* param_name, MYON_XMLVariant& param_value_out) const override;

  MYON_XMLParametersV8(const MYON_XMLParametersV8&) = delete;
  const MYON_XMLParametersV8& operator = (const MYON_XMLParametersV8&) = delete;

protected:
  virtual MYON_XMLNode* ObtainChildNodeForWrite(MYON_XMLNode& node, const wchar_t* param_name) const override;
  MYON_XMLNode* FindNodeByNameProperty(const wchar_t* param_name) const;
};

MYON_DECL bool MYON_RunXMLTests(const wchar_t* test_folder);

////////////////////////////////////////////////////////////////////////////////////////////
//
// MYON_RdkUserData is for internal use only and is not intended for use by SDK clients.
//
////////////////////////////////////////////////////////////////////////////////////////////

class MYON_CLASS MYON_RdkUserData : public MYON_XMLUserData
{
private:
  MYON_OBJECT_DECLARE(MYON_RdkUserData);

public:
  MYON_RdkUserData();
  MYON_RdkUserData(const MYON_RdkUserData& ud);
  virtual ~MYON_RdkUserData();

  const MYON_RdkUserData& operator = (const MYON_RdkUserData& ud);

  static MYON_UUID Uuid(void);

  virtual void SetToDefaults(void) const override;
  virtual bool GetDescription(MYON_wString& s) override;
  virtual bool Read(MYON_BinaryArchive&) override;
  virtual bool Write(MYON_BinaryArchive&) const override;
  virtual bool DeleteAfterWrite(const MYON_BinaryArchive& archive, const MYON_Object* parent_object) const override;
  virtual bool DeleteAfterRead(const MYON_BinaryArchive& archive, MYON_Object* parent_object) const override;
  virtual bool Transform(const MYON_Xform& xform) override;
  virtual void ReportVersionError(void) const override;

  typedef bool (*ReadCallback)(MYON_RdkUserData& ud);
  typedef bool (*WriteCallback)(const MYON_RdkUserData& ud);
  typedef bool (*TransformCallback)(MYON_RdkUserData& ud, const MYON_Xform& xform);

  static void SetCallbacks(ReadCallback rc, WriteCallback wc, TransformCallback tc);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
// This class is for internal RDK use only.
class MYON_CLASS MYON_RdkDocumentDefaults
{
public:
  enum class ValueSets { All, New };

  MYON_RdkDocumentDefaults(int major_version, ValueSets vs, void* reserved=nullptr);
  MYON_RdkDocumentDefaults(const MYON_RdkDocumentDefaults&) = delete;
  virtual ~MYON_RdkDocumentDefaults();

  const MYON_RdkDocumentDefaults& operator = (const MYON_RdkDocumentDefaults&) = delete;

  const MYON_XMLNode& Node(void) const;

  void CopyDefaultsTo(MYON_XMLNode& dest) const;

private:
  void CreateXML(void);

private:
  MYON_XMLRootNode _root;
  const ValueSets _vs;
  const int _major_version;
  void* _reserved;
};
///////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

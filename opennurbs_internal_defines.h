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

// Internal header; not in the public SDK.

#if !defined(OPENNURBS_INTERNAL_DEFINES_INC_)
#define OPENNURBS_INTERNAL_DEFINES_INC_

#if defined(MYON_COMPILING_OPENNURBS)

#include <unordered_map>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#define MYON_KIND_MATERIAL     L"material"
#define MYON_KIND_ENVIRONMENT  L"environment"
#define MYON_KIND_TEXTURE      L"texture"
#define MYON_RDK_UD_ROOT       L"render-content-manager-data"

inline bool IsFloatEqual (float  f1, float  f2) { return (fabsf(f1 - f2) < 1e-6); }
inline bool IsDoubleEqual(double d1, double d2) { return (fabs (d1 - d2) < 1e-10); }

MYON_UUID RdkPlugInId(void);
MYON_UUID UniversalRenderEngineId(void);

MYON__INT64 Integerize(float dirty);
MYON__INT64 Integerize(double dirty);

void SetModel(const class MYON_RenderContent&, MYONX_Model&);
void SetModel(const class MYON_PostEffect&, MYONX_Model&);
MYON_3dmObjectAttributes* GetComponentAttributes(const MYON_ModelComponent& component);
MYON_RenderContent* NewRenderContentFromNode(const class MYON_XMLNode& node);
MYON_PostEffect* NewPostEffectFromNode(MYON_XMLNode& node);
void SetRenderContentNodeRecursive(const MYON_RenderContent& rc, MYON_XMLNode& node);
MYON_XMLNode* FindPostEffectNodeForId(const MYON_XMLNode& sectionNode, const MYON_UUID& id);
bool SetRDKObjectInformation(MYON_Object& object, const MYON_wString& xml, int archive_3dm_version);
bool GetRDKEmbeddedFiles(const MYONX_Model_UserData& docud, MYON_ClassArray<MYON_wString>& paths, MYON_SimpleArray<unsigned char*>& embedded_files_as_buffers, MYON_SimpleArray<size_t>& buffer_sizes);
bool GetEntireDecalXML(const MYON_3dmObjectAttributes& attr, MYON_XMLRootNode& xmlOut);
void CreateMeshModifiersFromXML(const MYONX_Model& model, int archive_3dm_version);
void CreateXMLFromMeshModifiers(const MYONX_Model& model, int archive_3dm_version);
bool GetMeshModifierObjectInformation(const MYON_Object& object, MYON_wString& xml, int archive_3dm_version);
void SetMeshModifierObjectInformation(MYON_Object& object, const MYON_MeshModifier* mm, int archive_3dm_version);
bool IsRDKDocumentInformation(const MYONX_Model_UserData& docud);

template <class T> inline T Lerp(float  t, const T& l, const T& h) { return l + T(t) * (h - l); }
template <class T> inline T Lerp(double t, const T& l, const T& h) { return l + T(t) * (h - l); }

class MYON_InternalXMLImpl
{
public:
  MYON_InternalXMLImpl(MYON_XMLNode* n=nullptr) : m_model_node(n) { }
  virtual ~MYON_InternalXMLImpl();

  MYON_XMLVariant GetParameter(const wchar_t* path_to_node, const wchar_t* param_name, const MYON_XMLVariant& def) const;
  bool SetParameter(const wchar_t* path_to_node, const wchar_t* param_name, const MYON_XMLVariant& value);

  MYON_XMLVariant GetParameter(const wchar_t* param_name, const MYON_XMLVariant& def) const { return GetParameter(L"", param_name, def); }
  bool SetParameter(const wchar_t* param_name, const MYON_XMLVariant& value) { return SetParameter(L"", param_name, value); }

  MYON_XMLVariant GetParameter_NoType(const wchar_t* path_to_node, const wchar_t* param_name, const wchar_t* default_type, const MYON_XMLVariant& def) const;
  bool SetParameter_NoType(const wchar_t* path_to_node, const wchar_t* param_name, const MYON_XMLVariant& value);

  MYON_XMLNode& Node(void) const;

protected:
  virtual MYON_wString NameOfRootNode(void) const;

private:
  MYON_XMLVariant InternalGetParameter(const wchar_t* path_to_node, const wchar_t* param_name, const wchar_t* default_type, const MYON_XMLVariant& def) const;
  bool InternalSetParameter(const wchar_t* path_to_node, const wchar_t* param_name, bool write_type, const MYON_XMLVariant& value);

public:
  mutable std::recursive_mutex m_mutex;
  mutable MYON_XMLNode* m_local_node = nullptr; // Used when m_model_node is null.
  MYON_XMLNode* m_model_node;
};

class MYON_DecalCollection final
{
public:
  MYON_DecalCollection(MYON_3dmObjectAttributes* a) : m_attr(a) { }
  MYON_DecalCollection(const MYON_DecalCollection& dc) = delete;
  ~MYON_DecalCollection();

  const MYON_DecalCollection& operator = (const MYON_DecalCollection& dc);

  MYON_Decal* AddDecal(void);
  bool DeleteDecal(MYON_Decal& decal);
  void DeleteAllDecals(void);
  const MYON_SimpleArray<MYON_Decal*>& GetDecalArray(void) const;

  void SetChanged(void);

  void UpdateUserData(unsigned int archive_3dm_version) const;

private:
  void Populate(void) const;
  int  FindDecalIndex(const MYON_Decal&) const;

private:
  MYON_3dmObjectAttributes* m_attr;
  mutable MYON_XMLRootNode m_root_node;
  mutable MYON_SimpleArray<MYON_Decal*> m_decals;
  mutable bool m_populated = false;
  mutable bool m_changed = false;
};

template<class T> inline void hash_combine(size_t& seed, const T& v)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9E3779B9 + (seed << 6) + (seed >> 2);
}

class UuidHasher // Hasher for using MYON_UUID as key with std::map
{
public:
	inline size_t operator()(const MYON_UUID& uuid) const
	{
		size_t seed = 0;

		const auto* d = reinterpret_cast<const MYON__UINT32*>(&uuid);
		::hash_combine(seed, d[0]);
		::hash_combine(seed, d[1]);
		::hash_combine(seed, d[2]);
		::hash_combine(seed, d[3]);

		return seed;
	}
};

class MYON_EnvironmentsPrivate final : public MYON_InternalXMLImpl
{
public:
  MYON_EnvironmentsPrivate() { }
  MYON_EnvironmentsPrivate(MYON_XMLNode& n) : MYON_InternalXMLImpl(&n) { }
  MYON_EnvironmentsPrivate(const MYON_EnvironmentsPrivate&);

  MYON_EnvironmentsPrivate& operator = (const MYON_EnvironmentsPrivate&);

  bool operator == (const MYON_EnvironmentsPrivate&);

  MYON_UUID BackgroundRenderEnvironment(void) const;
  void    SetBackgroundRenderEnvironment(const MYON_UUID& id);
  bool    SkylightingRenderEnvironmentOverride(void) const;
  void    SetSkylightingRenderEnvironmentOverride(bool on);
  MYON_UUID SkylightingRenderEnvironment(void) const;
  void    SetSkylightingRenderEnvironment(const MYON_UUID& id);
  bool    ReflectionRenderEnvironmentOverride(void) const;
  void    SetReflectionRenderEnvironmentOverride(bool on);
  MYON_UUID ReflectionRenderEnvironment(void) const;
  void    SetReflectionRenderEnvironment(const MYON_UUID& id);
};

class MYON_3dmRenderSettingsPrivate final
{
public:
  MYON_3dmRenderSettingsPrivate();
  MYON_3dmRenderSettingsPrivate(const MYON_3dmRenderSettingsPrivate&);

  const MYON_3dmRenderSettingsPrivate& operator = (const MYON_3dmRenderSettingsPrivate&);

  void SetToDefaults(void);

public:
  MYON_XMLRootNode _rdk_document_data;
  MYON_Dithering _dithering;
  MYON_GroundPlane _ground_plane;
  MYON_LinearWorkflow _linear_workflow;
  MYON_RenderChannels _render_channels;
  MYON_SafeFrame _safe_frame;
  MYON_Skylight _skylight;
  MYON_Sun _sun;
  MYON_EnvironmentsPrivate _environments;
};

//--------------------------------------------------------------------------------------------------

class MYON_INTERNAL_OBSOLETE
{
public:

  //// OBSOLETE V5 Dimension Types ///////////////////////////////////////////////////////////
  enum class V5_eAnnotationType : unsigned char
  {
    dtNothing,
    dtDimLinear,
    dtDimAligned,
    dtDimAngular,
    dtDimDiameter,
    dtDimRadius,
    dtLeader,
    dtTextBlock,
    dtDimOrdinate,
  };

  // convert integer to eAnnotationType enum
  static MYON_INTERNAL_OBSOLETE::V5_eAnnotationType V5AnnotationTypeFromUnsigned(
    unsigned int v5_annotation_type_as_unsigned
  );

  //// dim text locations ///////////////////////////////////////////////////////////
  enum class V5_TextDisplayMode : unsigned char
  {
    kNormal = 0, // antique name - triggers use of current default
    kHorizontalToScreen = 1,         // Horizontal to the screen
    kAboveLine = 2,
    kInLine = 3,
    kHorizontalInCplane = 4   // horizontal in the dimension's plane 
  };

  static MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode V5TextDisplayModeFromUnsigned(
    unsigned int text_display_mode_as_unsigned
  );

  static MYON_INTERNAL_OBSOLETE::V5_TextDisplayMode V5TextDisplayModeFromV6DimStyle(
    const MYON_DimStyle& V6_dim_style
  );
  
  /// <summary>
  /// Attachment of content
  /// </summary>
  enum class V5_vertical_alignment : unsigned char
  {
    /// <summary>
    /// Text centered on dimension line (does not apply to leaders or text)
    /// </summary>
    Centered = 0,
    /// <summary>
    /// Text above dimension line (does not apply to leaders or text)
    /// </summary>
    Above = 1,
    /// <summary>
    /// Text below dimension line (does not apply to leaders or text)
    /// </summary>
    Below = 2,
    /// <summary>
    /// Leader tail at top of text  (does not apply to text or dimensions)
    /// </summary>
    Top = 3, // = TextVerticalAlignment::Top
    /// <summary>
    /// Leader tail at middle of first text line (does not apply to text or dimensions)
    /// </summary>
    FirstLine = 4, // = MiddleOfTop
    /// <summary>
    /// Leader tail at middle of text or content (does not apply to text or dimensions)
    /// </summary>
    Middle = 5, // = Middle
    /// <summary>
    /// Leader tail at middle of last text line (does not apply to text or dimensions)
    /// </summary>
    LastLine = 6, // = MiddleOfBottom
    /// <summary>
    /// Leader tail at bottom of text (does not apply to text or dimensions)
    /// </summary>
    Bottom = 7, // = Bottom
    /// <summary>
    /// Leader tail at bottom of text, text underlined (does not apply to text or dimensions)
    /// </summary>
    Underlined = 8 // Underlined

    // nothing matched BottomOfTop
  };

  static MYON_INTERNAL_OBSOLETE::V5_vertical_alignment V5VerticalAlignmentFromUnsigned(
    unsigned int vertical_alignment_as_unsigned
  );

  static MYON_INTERNAL_OBSOLETE::V5_vertical_alignment V5VerticalAlignmentFromV5Justification(
    unsigned int v5_justification_bits
  );

  static MYON_INTERNAL_OBSOLETE::V5_vertical_alignment V5VerticalAlignmentFromV6VerticalAlignment(
    const MYON::TextVerticalAlignment text_vertical_alignment
  );

  static MYON::TextVerticalAlignment V6VerticalAlignmentFromV5VerticalAlignment(
    MYON_INTERNAL_OBSOLETE::V5_vertical_alignment V5_vertical_alignment
  );


  enum class V5_horizontal_alignment : unsigned char
  {
    /// <summary>
    /// Left aligned
    /// </summary>
    Left = 0, // Left
    /// <summary>
    /// Centered
    /// </summary>
    Center = 1,
    /// <summary>
    /// Right aligned
    /// </summary>
    Right = 2,
    /// <summary>
    /// Determined by orientation
    /// Primarily for leaders to make
    /// text right align when tail is to the left 
    /// and left align when tail is to the right
    /// </summary>
    Auto = 3,
  };

  static MYON_INTERNAL_OBSOLETE::V5_horizontal_alignment V5HorizontalAlignmentFromUnsigned(
    unsigned int horizontal_alignment_as_unsigned
  );

  static MYON_INTERNAL_OBSOLETE::V5_horizontal_alignment V5HorizontalAlignmentFromV5Justification(
    unsigned int v5_justification_bits
  );

  static MYON_INTERNAL_OBSOLETE::V5_horizontal_alignment V5HorizontalAlignmentFromV6HorizontalAlignment(
    const MYON::TextHorizontalAlignment text_horizontal_alignment
  );

  static MYON::TextHorizontalAlignment V6HorizontalAlignmentFromV5HorizontalAlignment(
    MYON_INTERNAL_OBSOLETE::V5_horizontal_alignment V5_vertical_alignment
  );
};

#endif

#endif

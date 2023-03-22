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

////////////////////////////////////////////////////////////////
//
//   Definition of subdivision surface
//
////////////////////////////////////////////////////////////////

#if !defined(OPENNURBS_SUBD_INC_)
#define OPENNURBS_SUBD_INC_

/// <summary>
/// MYON_SubDGetControlNetMeshPriority specifies what type of MYON_SubD information
/// is most important to transfer to the MYON_Mesh.
/// </summary>
enum class MYON_SubDGetControlNetMeshPriority : unsigned char
{
  ///<summary>
  /// Create a mesh that can be used to reliably transfer SubD control net geometry, 
  /// topology, and interior edge crease information. Use this option when the mesh 
  /// will be used to create a Catmull-Clark subdivision surface. SubD texture 
  /// coordinates cannot be recovered from the mesh.
  ///</summary>
  Geometry = 0,

  ///<summary>
  /// Create a mesh that has the shape of the SubD control net and
  /// includes SubD texture coordinate information.
  /// Use this option when the mesh will be used to create an image of the
  /// SubD control net that relies on texture coordinates. SubD interior edge
  /// crease information cannot be recovered from the mesh. Most applications will 
  /// not be able to use the mesh to recreate a Catmull-Clark subdivision surface.
  ///</summary>
  TextureCoordinates = 1
};


/// <summary>
/// MYON_SubDTextureCoordinateType identifies the way MYON_SubDMeshFragment texture coordinates are set from an MYON_SubDFace.
/// </summary>
enum class MYON_SubDTextureCoordinateType : unsigned char
{
  ///<summary>
  /// Texture domains are not set.
  ///</summary>
  Unset = 0,

  ///<summary>
  /// Each SubDFace uses the unit square in texture space.
  ///</summary>
  Unpacked = 1,

  ///<summary>
  /// The face's pack rect is used to set fragment texture coordinates.
  /// When possible, adjacent quads with the same MYON_SubDFace::PackId() value are assigned adjacent 
  /// rectangles in texture space.
  ///</summary>
  Packed = 2,

  ///<summary>
  /// All MYON_SubDMeshFragment texture coordinate values are zero.
  ///</summary>
  Zero = 3,

  ///<summary>
  /// All MYON_SubDMeshFragment texture coordinate values are MYON_DBL_QNAN.
  ///</summary>
  Nan = 4,

  ///<summary>
  /// All MYON_SubDMeshFragment texture coordinate values are set by
  /// barycentric interpolation of MYON_SubDFace.TexturePoint() values.
  ///</summary>
  FromFaceTexturePoints = 6,

  ///<summary>
  /// Texture coordinates are set from an MYON_TextureMapping and transformation specified
  /// by MYON_SubD::TextureMappingTag(). In all other cases, MYON_SubD::TextureMappingTag()
  /// can be set, but is ignored.
  ///</summary>
  FromMapping = 7,
};

#pragma region RH_C_SHARED_ENUM [MYON_SubDVertexTag] [Rhino.Geometry.SubDVertexTag] [byte]
/// <summary>
/// MYON_SubDVertexTag identifies the type of subdivision vertex.  Different tags use
/// different subdivision algorithms to determine where the subdivision point and
/// limit point are located.  There are toplological constraints that restrict which
/// tags can be assigned.
/// </summary>
enum class MYON_SubDVertexTag : unsigned char
{
  ///<summary>
  /// Not a valid vertex tag and the default value for MYON_SubDVertex.m_vertex_tag.
  /// This encourages developers to thoughtfully initialize MYON_SubDVertex.m_vertex_tag
  /// or use MYON_SubD.UpdateAllTagsAndSectorCoefficients() to automatically set the
  /// m_vertex_tag values at an appropriate time.
  ///</summary>
  Unset = 0,

  ///<summary>
  /// Must be an interior vertex.
  /// All edges attached to a smooth vertex must be tagged as MYON_SubDEdgeTag::Smooth
  /// and must have 2 faces. 
  ///</summary>
  Smooth = 1,

  ///<summary>
  /// Can be an interior or a boundary vertex.
  /// Exactly two edges ending at a crease vertex must be tagged as MYON_SubDEdgeTag::Crease and may
  /// have 1 or 2 faces. 
  /// All other edges ending at a crease must be tagged as tagON_SubD::EdgeTag::Smooth and have 2 faces.
  /// Below P = MYON_SubDVertex.ControlNetPoint() and Ai = MYON_SubDVertex.Edge(i)->OtherEndVertex()->ControlNetPoint().
  /// A crease vertex subdivision point is (6*P + A1 + A2)/8.
  /// A crease vertex limit surface point is (4*P + A1 + A2)/6.
  ///</summary>
  Crease = 2,

  ///<summary>
  /// Can be an interior, boundary, nonmanifold, or isolated vertex.
  /// The location of a corner vertex is fixed. 
  /// The all subdivision points and the limit point are at the initial vertex location.
  /// The edges ending at a corner vertex can be smooth or crease edges.
  /// A corner vertex subdivision point is P where P = MYON_SubDVertex.ControlNetPoint().
  /// A corner vertex limit surface point is P where P = MYON_SubDVertex.ControlNetPoint().
  ///</summary>
  Corner = 3,

  ///<summary>
  /// Must be an interior vertex.  
  /// Every edge attached to a dart vertex must have 2 faces.
  /// Exactly one edge attached to a dart vertex must be tagged as MYON_SubDEdgeTag::Crease
  /// and every other attached edge must be tagged as MYON_SubDEdgeTag::smooth.
  ///</summary>
  Dart = 4
};
#pragma endregion

#pragma region RH_C_SHARED_ENUM [MYON_SubDEdgeTag] [Rhino.Geometry.SubDEdgeTag] [byte]
/// <summary>
/// MYON_SubDEdgeTag identifies the type of subdivision edge.  Different tags use
/// different subdivision algorithms to calculate the subdivision point.
/// </summary>  
enum class MYON_SubDEdgeTag : unsigned char
{
  ///<summary>
  /// Not a valid edge tag and the default value for MYON_SubDEdge.m_edge_tag.
  /// This encourages developers to thoughtfully initialize MYON_SubDEdge.m_edge_tag.
  /// or use MYON_SubD.UpdateAllTagsAndSectorCoefficients() to automatically set the
  /// m_edge_tag values at an appropriate time.
  ///</summary>
  Unset = 0,

  ///<summary>
  /// At least one the edge's vertices must be tagged as MYON_SubDVertexTag::Smooth.
  /// The edge must have exactly two faces.
  /// The edge's subdivision point is (A1 + A2 + S(f1) + S(f2))/4, where 
  /// Ai = MYON_SubDEdge.Vertex(i)->ControlNetPoint() and
  /// S(fi) = MYON_SubDEdge.Face(i)->SubdivisionPoint().
  ///</summary>
  Smooth = 1,

  ///<summary>
  /// Both of the edge's vertices must be tagged as MYON_SubDVertexTag::Dart,
  /// MYON_SubDVertexTag::Crease, or MYON_SubDVertexTag::Corner. 
  /// (The vertex tags can be different.) The edge can have any number of faces.
  /// The edge's subdivision point is (A1+A2)/2 where Ai = MYON_SubDEdge.Vertex(i)->ControlNetPoint().
  ///</summary>
  Crease = 2,

  ///<summary>
  /// This tag appears only on level 0 edges that have exactly two neighboring faces
  /// and both of the edge's vertices are tagged as MYON_SubDVertexTag::Dart,
  /// MYON_SubDVertexTag::Crease, or MYON_SubDVertexTag::Corner.
  /// The level 1 subdivision point for a level 0 edge tagged as MYON_SubDEdgeTag::SmoothX 
  /// is the standard smooth edge subdivision point.
  /// When subdivided, the new subdivision vertex will be tagged
  /// as MYON_SubDVertexTag::Smooth and the subdivided edges will
  /// be tagged as MYON_SubDEdgeTag::Smooth.  
  /// The tag MYON_SubDEdgeTag::SmoothX can only appear on a level 0 edge.
  /// This tag exists because the MYON_SubD subdivision
  /// algorithm requires any edge with both end vertices
  /// tagged as not smooth must be subdivided at its midpoint.
  /// Sector iterators treat "SmoothX" edges as smooth.
  /// Both edge m_sector_coefficient[] values must be set so the smooth subdivided edges will be valid.
  ///</summary>
  SmoothX = 4
};
#pragma endregion

#pragma region RH_C_SHARED_ENUM [MYON_SubDHashType] [Rhino.Geometry.SubDHashType] [byte]
/// <summary>
/// MYON_SubDHashType used used to specify what type of SubD information is hashed (topology or geometry).
/// </summary>
enum class MYON_SubDHashType : unsigned char
{
  /// <summary>
  /// Unset indicates the hash type still needs to be selected. It is not a valid type
  /// for calculating a hash.
  /// </summary>
  Unset = 0,

  /// <summary>
  /// The Topology hash includes component ids, and all topological relationships 
  /// between vertices, edges, and faces. If two SubDs have the same topology hash,
  /// then the the have identical labeled control net topology.
  /// </summary>
  Topology = 3,

  /// <summary>
  /// The TopologyAndEdgeCreases includes all information in a Topology hash.
  /// The TopologyAndEdgeCreases adds edge crease/smooth attributes.
  /// Many contexts, including face packing and exploding, take edge creases
  /// into account when partitioning a SubD into regions.
  /// </summary>
  TopologyAndEdgeCreases = 1,

  /// <summary>
  /// A geometry hash includes all information in a TopologyAndEdgeCreases hash.
  /// The Geometry hash adds vertex tags, vertex control net points, 
  /// and nonzero subdivision displacements on vertices, edges, and faces.
  /// If two SubDs have the same geometry hash, then they have identical surface geometry.
  /// </summary>
  Geometry = 2
};
#pragma endregion

/// <summary>
/// MYON_SubDHash provides a simple way to save a SubD's vertex, edge, and face SHA1 hashes.
/// Typically it is used when a calculation needs to know if the current SubD has is geometrically
/// identical to a previous SubD. When speed is not important, comparing the current value of
/// MYON_SubD::GeometryHash() to a previously save value of MYON_SubD::GeometryHash() is functionally
/// identical but typically much slower when the SubDs are different.
/// </summary>
class MYON_CLASS MYON_SubDEdgeSharpness
{
public:
  MYON_SubDEdgeSharpness() = default;
  ~MYON_SubDEdgeSharpness() = default;
  MYON_SubDEdgeSharpness(const MYON_SubDEdgeSharpness&) = default;
  MYON_SubDEdgeSharpness& operator=(const MYON_SubDEdgeSharpness&) = default;

public:
  /// <summary>
  /// An edge sharpness with contant value 0.0.
  /// </summary>
  static const MYON_SubDEdgeSharpness Zero;

  /// <summary>
  /// An edge sharpness with both end values = MYON_DBL_QNAN.
  /// </summary>
  static const MYON_SubDEdgeSharpness Nan;

  /// <returns>True if the sharpness value is valid and contant.</returns>
  bool IsConstant() const;

  /// <returns>True if the sharpness value is valid and variable.</returns>
  bool IsVariable() const;

  /// <returns>True if the sharpness valid is zero.</returns>
  bool IsZero() const;

  /// <returns>True if the sharpness value is valid either end sharpness value is not zero.</returns>
  bool IsNotZero() const;

  /// <returns>True if both end sharpness values are &gt;= 0 and &lt;= MYON_SubDEdgeSharpness::Maximum.</returns>
  bool IsValid() const;

  /// <returns>True if IsValid() is false.</returns>
  bool IsNotValid() const;

  /// <summary>
  /// Create a constant MYON_SubDEdgeSharpness;
  /// </summary>
  /// <param name="sharpness">0 &lt;= sharpness &lt;= MYON_SubDEdgeSharpness::Maximum</param>
  /// <returns>
  /// If the input values is valid, an MYON_SubDEdgeSharpness 
  /// with constant value sharpness is returned. 
  /// If the input value is a nan, MYON_SubDEdgeSharpness Nan is returned.
  /// Otherwise MYON_SubDEdgeSharpness::Nan is returned.
  /// </returns>
  static const MYON_SubDEdgeSharpness FromConstant(double sharpness);

  /// <summary>
  /// Create a variable MYON_SubDEdgeSharpness;
  /// </summary>
  /// <param name="sharpness0">0 &lt;= sharpness0 &lt;= MYON_SubDEdgeSharpness::Maximum</param>
  /// <param name="sharpness1">0 &lt;= sharpness1 &lt;= MYON_SubDEdgeSharpness::Maximum</param>
  /// <returns>
  /// If both input values are valid, an edge sharpness 
  /// with start value sharpness0 and end value sharpness1 is returned. 
  /// If either end value is a nan, MYON_SubDEdgeSharpness Nan is returned.
  /// Otherwise MYON_SubDEdgeSharpness::Nan is returned.
  /// </returns>
  static const MYON_SubDEdgeSharpness FromInterval(double sharpness0, double sharpness1);

  /// <summary>
  /// Create a variable MYON_SubDEdgeSharpness;
  /// </summary>
  /// <param name="sharpness_interval">0 &lt;= sharpness0 &lt;= MYON_SubDEdgeSharpness::Maximum</param>
  /// <returns>
  /// If the interval's values are valid, an edge sharpness 
  /// with start value sharpness_interval[0] and end value sharpness_interval[1] is returned. 
  /// If either end value is a nan, MYON_SubDEdgeSharpness Nan is returned.
  /// Otherwise MYON_SubDEdgeSharpness::Nan is returned.
  /// </returns>
  static const MYON_SubDEdgeSharpness FromInterval(const class MYON_Interval& sharpness_interval);

  /// <summary>
  /// Return a sharpness interval that is the union of the nonzero input sharpness intervals.
  /// </summary>
  /// <param name="a"></param>
  /// <param name="b"></param>
  /// <returns>
  /// If a or b is not zero, then the returned interval is the union of the nonzero
  /// input intervals. Otherwise MYON_SubDEdgeSharpness::Zero is returned. 
  /// The returned sharpenss always has sharpness[0] &lt;= sharpness[1].
  /// </returns>
  static const MYON_SubDEdgeSharpness Union(
    const MYON_SubDEdgeSharpness& a, 
    const MYON_SubDEdgeSharpness& b 
  );

  /// <summary>
  /// Sharpness value for a subdivided edge.
  /// </summary>
  /// <param name="end_index"0 or 1.</param>
  /// <returns>Subdivided sharpness or MYON_SubDEdgeSharpness::Zero if index is out of range.</returns>
  const MYON_SubDEdgeSharpness Subdivided(int end_index) const;

  const MYON_SubDEdgeSharpness Reversed() const;

  /// <summary>
  /// Convert a user facing slider value to a SubD edge end sharpness value.
  /// </summary>
  /// <param name="slider_domain">Non empty slider domain. (Often MYON_Interval::ZeroToOne.)</param>
  /// <param name="slider_value">A value in the slider_domain. slider_domain[0] returns 0.0. slider_domain[1] returns MYON_SubDEdgeSharpness::Maximum.</param>
  /// <param name="invalid_input_result">Value to return if the input is not valid.</param>
  /// <returns>The slider value converted to an MYON_SubDEdge sharpness value.</returns>
  static double SharpnessFromSliderValue(
    MYON_Interval slider_domain,
    double slider_value,
    double invalid_input_result
  );

  /// <summary>
  /// Convert a user facing slider value to a SubD edge end sharpness value.
  /// </summary>
  /// <param name="slider_domain">Non empty slider domain. (Often MYON_Interval::ZeroToOne.)</param>
  /// <param name="normalized_slider_value">0 &lt;= normalized_slider_value &lt;= 1.</param>
  /// <param name="invalid_input_result">Value to return if the input is not valid.</param>
  /// <returns>
  /// If 0 &lt;= normalized_slider_value &lt;= 1, the normalized slider value converted to an MYON_SubDEdge sharpness value
  /// from 0 to MYON_SubDEdgeSharpness::Maximum. Otherwise, MYON_DBL_QNAN is returned.
  /// </returns>
  static double SharpnessFromNormalizedValue(
    double normalized_slider_value
  );

  /// <summary>
  /// Get the edge sharpness at the start or end.
  /// </summary>
  /// <param name="end_index"0 or 1.</param>
  /// <returns>EndSharpness(end_index).</returns>
  double operator[](int end_index) const;

  /// <summary>
  /// Return the average of the sharpness interval.
  /// </summary>
  /// <returns>0.5*(sharpness[0]+sharpness[1])</returns>
  double Average() const;

  /// <summary>
  /// Return the minimum of the sharpness interval.
  /// </summary>
  /// <returns>The minimum of the sharpness interval</returns>
  double MinimumEndSharpness() const;

  /// <summary>
  /// Return the maximum of the sharpness interval.
  /// </summary>
  /// <returns>The maximum of the sharpness interval</returns>
  double MaximumEndSharpness() const;


  /// <summary>
  /// Get the sharpness at the start or end.
  /// </summary>
  /// <param name="end_index"0 or 1.</param>
  /// <returns>Sharpness or MYON_DBL_QNAN if end_index is out of range.</returns>
  double EndSharpness(int end_index) const;

  /// <summary>
  /// Calculate the vertex sharpness from the attached sharp edge information.
  /// Note that vertices with a corner tag always have zero sharpness.
  /// </summary>
  /// <param name="vertex_tag">
  /// The vertex tag (smooth, crease, dart, corner). 
  /// For smooth, crease, and dart, this is used to determine the number of attached crease edges.
  /// COrner vertices always have sharpness = 0.
  /// </param>
  /// <param name="sharp_edge_end_count">
  /// Number of sharp edges attached to the vertex that have 
  /// nonzero end sharpness at the vertex.</param>
  /// <param name="maximum_edge_end_sharpness">
  /// The largest sharp edge end sharpness at the vertex.
  /// </param>
  /// <returns></returns>
  static double VertexSharpness(
    MYON_SubDVertexTag vertex_tag,
    unsigned sharp_edge_end_count,
    double maximum_edge_end_sharpness
  );

  /// <summary>
  /// MYON_SubDEdgeSharpness::Maximum = 4.
  /// SubD edge sharpness values are &lt;= MYON_SubDEdgeSharpness::Maximum.
  /// </summary>
  static const double Maximum;

  /// <summary>
  /// MYON_SubDEdgeSharpness::Tolerance = 0.01
  /// If an edge has sharpness within MYON_SubDEdgeSharpness::Tolerance of an integer value,
  /// the sharpness is set to that integer value.
  /// </summary>
  static const double Tolerance;

  /// <summary>
  /// Verify 0 &lt;= sharpness &lt;= MYON_SubDEdgeSharpness::Maximum and return an integer value when
  /// the input sharpenss is within MYON_SubDEdgeSharpness::Tolerance of an integer.
  /// </summary>
  /// <param name="sharpness"></param>
  /// <param name="invalid_input_result">Value returned when the sharpness parameter is invalid.</param>
  /// <returns>SubD edge sharpness value that makes sense.</returns>
  static double Sanitize(
    double sharpness,
    double invalid_input_result
  );

  /// <summary>
  /// Verify 0 &lt;= sharpness &lt;= MYON_SubDEdgeSharpness::Maximum and return an integer value when
  /// the input sharpenss is within MYON_SubDEdgeSharpness::Tolerance of an integer.
  /// </summary>
  /// <param name="sharpness"></param>
  /// <returns>SubD edge sharpness value that makes sense or 0.0 if the input sharpness is invalid.</returns>
  static double Sanitize(
    double sharpness
  );

private:
  float m_edge_sharpness[2] = {};
};

MYON_DECL
bool operator==(const MYON_SubDEdgeSharpness& lhs, const MYON_SubDEdgeSharpness& rhs);

MYON_DECL
bool operator!=(const MYON_SubDEdgeSharpness& lhs, const MYON_SubDEdgeSharpness& rhs);

/// <summary>
/// MYON_SubDHash provides a simple way to save a SubD's vertex, edge, and face SHA1 hashes.
/// Typically it is used when a calculation needs to know if the current SubD has is geometrically
/// identical to a previous SubD. When speed is not important, comparing the current value of
/// MYON_SubD::GeometryHash() to a previously save value of MYON_SubD::GeometryHash() is functionally
/// identical but typically much slower when the SubDs are different.
/// </summary>
class MYON_CLASS MYON_SubDHash
{
public:
  MYON_SubDHash() = default;
  ~MYON_SubDHash() = default;
  MYON_SubDHash(const MYON_SubDHash&) = default;
  MYON_SubDHash& operator=(const MYON_SubDHash&) = default;

public:
  /// <summary>
  /// All counts are zero and all hashes are MYON_SHA1::EmptyContentHash.
  /// </summary>
  static const MYON_SubDHash Empty;

  // Saves the counts and hashes of the specified type


  /// <summary>
  /// Saves the counts and hashes of the specified type.
  /// </summary>
  /// <param name="hash_type"></param>
  /// <param name="subd"></param>
  /// <returns></returns>
  static const MYON_SubDHash Create(MYON_SubDHashType hash_type, const class MYON_SubD& subd );

  static const MYON_SubDHash Create(MYON_SubDHashType hash_type, const class MYON_SubDimple* subdimple);

  /// <summary>
  /// Dictionary compare of VertexCount(), EdgeCount(), FaceCount(), VertexHash(), EdgeHash(), and FaceHash() in that order.
  /// 
  /// NOTE WELL:
  /// SubDRuntimeSerialNumber() and SubdGeometryContentSerialNumber() are not compared because the reason this
  /// class exists is for it to be used to see if two different SubDs have identical content.
  /// </summary>
  /// <param name="lhs"></param>
  /// <param name="rhs"></param>
  /// <returns>
  /// -1: lhs < rhs
  /// 0: lhs == rsh
  /// 1: lhs > rhs
  /// </returns>
  static int Compare(const MYON_SubDHash& lhs, const MYON_SubDHash& rhs);

  /*
  Returns:
    True if vertex count is 0 or HashType is unset.
  */
  bool IsEmpty() const;

  /*
  Returns:
    True if vertex count is > 0 and HashType is geometry or toplology.
  */
  bool IsNotEmpty() const;

  MYON_SubDHashType HashType() const;

  /// <summary>
  /// The runtime serial number can be used to identify the SubD that was hashed to created this MYON_SubDHash.
  /// </summary>
  /// <returns>
  /// If this hash was created from an MYON_SubD, then the value of subd.RuntimeSerialNumber() is returned.
  /// Otherwise, 0 is returned.
  /// </returns>
  MYON__UINT64 SubDRuntimeSerialNumber() const;

  /// <summary>
  /// The geometry content serial number can be used to quickly determine if a SubD is exactly the instance used
  /// to create this MYON_SubDHash. If the geometry content serial numbers are equal, then the SubD is identical
  /// to the one use to create the hash. If the geometry content serial numbers differ, the current SubD hashes 
  /// need to be checked against this to see what, if anything, changed. For example, moving a vertex will not 
  /// change a topology hash.
  /// </summary>
  /// <returns>
  /// Returns:
  ///   If this hash was created from an MYON_SubD, then the value of subd.GeometryContentSerialNumber() at the
  ///   at the instant this hash was created is returned.
  ///   Otherwise, 0 is returned.
  /// </returns>
  MYON__UINT64 SubDGeometryContentSerialNumber() const;

  /// <summary>
  /// Copied from the SubD when the hash is created.
  /// </summary>
  /// <returns>
  /// Number of hashed vertices.
  /// </returns>
  unsigned int VertexCount() const;

  /// <summary>
  /// Copied from the SubD when the hash is created.
  /// </summary>
  /// <returns>
  /// Number of hashed edges.
  /// </returns>
  unsigned int EdgeCount() const;

  /// <summary>
  /// Copied from the SubD when the hash is created.
  /// </summary>
  /// <returns>
  /// Number of hashed faces.
  /// </returns>
  unsigned int FaceCount() const;

  /// <summary>
  /// If two SubDs have identical VertexHash() values, 
  /// then the SubD vertex information associated with HashType() is identical.
  /// </summary>
  /// <returns>
  /// A SHA1 hash of the SubD's vertex information associated with HashType().
  /// </returns>
  const MYON_SHA1_Hash VertexHash() const;

  /// <summary>
  /// If two SubDs have identical EdgeHash() values, 
  /// then the SubD edge information associated with HashType() is identical.
  /// </summary>
  /// <returns>
  /// A SHA1 hash of the SubD's edge information associated with HashType().
  /// </returns>
  const MYON_SHA1_Hash EdgeHash() const;

  /// <summary>
  /// If two SubDs have identical FaceHash() values, 
  /// then the SubD face information associated with HashType() is identical.
  /// </summary>
  /// <returns>
  /// A SHA1 hash of the SubD's face information associated with HashType().
  /// </returns>
  const MYON_SHA1_Hash FaceHash() const;

  /// <summary>
  /// If two SubDs have identical SubDHash() values, 
  /// then the SubD vertex, edge, and face information associated with HashType() is identical.
  /// </summary>
  /// <returns>
  /// A SHA1 hash of the SubD's vertex, edge, and face information associated with HashType().
  /// </returns>
  const MYON_SHA1_Hash SubDHash() const;

  void Dump(MYON_TextLog&) const;
  bool Write(class MYON_BinaryArchive&) const;
  bool Read(class MYON_BinaryArchive&);

private:
  friend class MYON_SubDimple;
  MYON_SubDHashType m_hash_type = MYON_SubDHashType::Unset;
  unsigned int m_vertex_count = 0;
  unsigned int m_edge_count = 0;
  unsigned int m_face_count = 0;
  MYON__UINT64 m_subd_runtime_serial_number = 0;
  MYON__UINT64 m_subd_geometry_content_serial_number = 0;
  MYON_SHA1_Hash m_vertex_hash = MYON_SHA1_Hash::EmptyContentHash;
  MYON_SHA1_Hash m_edge_hash = MYON_SHA1_Hash::EmptyContentHash;
  MYON_SHA1_Hash m_face_hash = MYON_SHA1_Hash::EmptyContentHash;
};

bool operator==(const MYON_SubDHash& lhs, const MYON_SubDHash& rhs);

bool operator!=(const MYON_SubDHash& lhs, const MYON_SubDHash& rhs);


class MYON_CLASS MYON_SubDToBrepParameters
{
public:
  MYON_SubDToBrepParameters() = default;
  ~MYON_SubDToBrepParameters() = default;
  MYON_SubDToBrepParameters(const MYON_SubDToBrepParameters&) = default;
  MYON_SubDToBrepParameters& operator=(const MYON_SubDToBrepParameters&) = default;

  /*
  Description:
    Default MYON_SubDToBrepParameters settings.
  Remarks: 
    These are the settings used by MYON_SubD::BrepForm()
  */
  static const MYON_SubDToBrepParameters Default;

  /*
  Description:
    Default MYON_SubDToBrepParameters settings for creating an unpacked brep.
  */
  static const MYON_SubDToBrepParameters DefaultUnpacked;

  /*
  Description:
    Default MYON_SubDToBrepParameters settings for creating an packed brep.
  */
  static const MYON_SubDToBrepParameters DefaultPacked;

  static int Compare(
    const MYON_SubDToBrepParameters& lhs,
    const MYON_SubDToBrepParameters& rhs
    );
  static int CompareFromPointers(
    const MYON_SubDToBrepParameters* lhs,
    const MYON_SubDToBrepParameters* rhs
    );

#pragma region RH_C_SHARED_ENUM [MYON_SubDToBrepParameters::VertexProcess] [Rhino.Geometry.SubDToBrepOptions.ExtraordinaryVertexProcessOption] [nested:byte]
  /// <summary>
  /// MYON_SubDToBrepParameters::Vertex identifies the options for post processing extraorindary vertices.
  /// </summary>
  enum class VertexProcess : unsigned char
  {
    ///<summary>
    /// The NURBS patches are used as is. 
    /// At extraordinary vertices, the brep vertex may not be G1.
    /// Typically the deviation bewtween the brep and SubD surface is smallest with this option.
    ///</summary>
    None = 0,

    ///<summary>
    /// At extraordinary vertices, the NURBS patches are modified so they are G1 at the extraordinary vertex.
    /// Typically the deviation bewtween the brep and SubD surface is larger than None and smaller than
    /// LocalG1x and LocalG2.
    ///</summary>
    LocalG1 = 1,

    ///<summary>
    /// At extraordinary vertices, the NURBS patches are modified so they are G2 at the extraordinary vertex.
    /// Typically the deviation bewtween the brep and SubD surface is larger than LocalG1 and LocalG1x.
    ///</summary>
    LocalG2 = 2,

    ///<summary>
    /// At extraordinary vertices, the NURBS patches are modified so they are G1 at the extraordinary vertex
    /// and tend to be closer to G1 along edges near the extraordinary vertex.
    /// Typically the deviation bewtween the brep and SubD surface is larger than LocalG1 and smaller than LocalG2.
    ///</summary>
    LocalG1x = 3,

    ///<summary>
    /// At extraordinary vertices, the NURBS patches are modified so they are G1 at the extraordinary vertex
    /// and G1 along edges near the extraordinary vertex.
    /// The patches will have one double knot near the extraordinary vertex.
    ///</summary>
    LocalG1xx = 4,
  };
#pragma endregion

  static MYON_SubDToBrepParameters::VertexProcess VertexProcessFromUnsigned(
    unsigned int vertex_process_as_unsigned
  );

  static const MYON_wString VertexProcessToString(
    MYON_SubDToBrepParameters::VertexProcess vertex_process
  );

  /*
  Returns:
    Option used for post processing extraorindary vertices.
  */
  MYON_SubDToBrepParameters::VertexProcess ExtraordinaryVertexProcess() const;

  /*
  Description:
    Set the ExtraordinaryVertexProcess() property.
  Parameters:
    ev_process - [in]
  */
  void SetExtraordinaryVertexProcess(
    MYON_SubDToBrepParameters::VertexProcess ev_process
  );

  /*
  Returns:
    If true, then quad packs of SubD faces are returned as a single brep face.
    Otherwise each SubD face generates a brep face.
  Remarks:
    SubD n-gons with n != 4 always generate n brep faces.
  */
  bool PackFaces() const;

  /*
  Description:
    Set the PackFaces() property.
  Parameters:
    bPackFaces - [in]
  */
  void SetPackFaces(
    bool bPackFaces
  );

  const MYON_wString ToString(
  bool bVerbose
  ) const;

  bool Read(MYON_BinaryArchive& archive);
  
  bool Write(MYON_BinaryArchive& archive) const;

private:
  bool m_bPackFaces = false;
  MYON_SubDToBrepParameters::VertexProcess m_extraordinary_vertex_process = MYON_SubDToBrepParameters::VertexProcess::LocalG1x;
  unsigned short m_reserved1 = 0;
  unsigned int m_reserved2 = 0;
  double m_reserved3 = 0.0;
};

bool operator==(const MYON_SubDToBrepParameters& lhs, const MYON_SubDToBrepParameters& rhs);
bool operator!=(const MYON_SubDToBrepParameters& lhs, const MYON_SubDToBrepParameters& rhs);

class MYON_CLASS MYON_SubDVertexPtr
{
public:
  // For performance reasons, m_ptr is not initialized and no constructors are declared
  // or implemented.  If you require initialization, then use x = MYON_SubDVertexPtr::Null
  // or x = MYON_SubDVertexPtr::Create(...).
  MYON__UINT_PTR m_ptr;

  static const MYON_SubDVertexPtr Null;

  bool IsNull() const;
  bool IsNotNull() const;

  /*
  Returns:
    If Vertex() is not nullptr, Vertex()->m_id is returned.
    Otherwise, 0 is returned.
  */
  unsigned int VertexId() const;

  /*
  Returns:
    True if this vertex is active in its parent subd or other
    relevant context.
  Remarks:
    When a component is in use, IsActive() = true. 
    If was used and then deleted, IsActive() is false.
  */
  bool IsActive() const;

  class MYON_SubDVertex* Vertex() const;

  MYON__UINT_PTR VertexDirection() const;

  const MYON_ComponentStatus Status() const;

  static const MYON_SubDVertexPtr Create(
    const class MYON_SubDVertex* vertex
    );

  /*
  Parameters:
    vertex - [in]
    vertex_direction - [in]
      zero or one
  */
  static const MYON_SubDVertexPtr Create(
    const class MYON_SubDVertex* vertex,
    MYON__UINT_PTR vertex_direction
    );

  static const MYON_SubDVertexPtr Create(
    const class MYON_SubDComponentPtr& vertex_component
    );

  /*
  Returns:
    The current value of the component mark ( m_status->RuntimeMark() ).
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specific state.
  */
  bool Mark() const;

  /*
  Description:
    Clears (sets to false) the value of the component mark.
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specific state.
  Returns:
    Input value of Mark().
  */
  bool ClearMark() const;

  /*
  Description:
    Sets (sets to true) the value of the component mark.
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specific state.
  Returns:
    Input value of Mark().
  */
  bool SetMark() const;

  /*
  Description:
    Sets the value of the component mark to bMark.
  Parameter:
    bMark - [in]
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specific state.
  Returns:
    Input value of Mark().
  */
  bool SetMark(
    bool bMark
  ) const;


  MYON__UINT8 MarkBits() const;

  MYON__UINT8 SetMarkBits(
    MYON__UINT8 mark_bits
  ) const;

  MYON__UINT8 ClearMarkBits() const;

  void ClearSavedSubdivisionPoints() const;

  /*
  Description:
    Clears saved subdivision and limit surface information for this component.
  Parameters:
    bClearNeighborhood - [in]
      If true, all components attached to this component are also cleared.
  */
  void ClearSavedSubdivisionPoints(
    bool bClearNeighborhood
  ) const;

};


#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_SubDVertexPtr>;
#endif

class MYON_CLASS MYON_SubDEdgePtr
{
public:
  // For performance reasons, m_ptr is not initialized and no constructors are declared
  // or implemented.  If you require initialization, then use x = MYON_SubDEdgePtr::Null
  // or x = MYON_SubDEdgePtr::Create(...).
  MYON__UINT_PTR m_ptr;

  static const MYON_SubDEdgePtr Null;

  /*
  Returns:
    True if this->Edge() is nullptr.
  */
  bool IsNull() const;

  /*
  Returns:
    True if this->Edge() is not nullptr.
  */
  bool IsNotNull() const;


  /*
  Returns:
    True if this->Edge() is not nullptr and both vertex pointers are not null as well.
  */
  bool IsNotNullAndVerticesAreNotNull() const;

  /*
  Returns:
    True if this edge is active in its parent subd or other
    relevant context.
  Remarks:
    When a component is in use, IsActive() = true. 
    If was used and then deleted, IsActive() is false.
  */
  bool IsActive() const;


  /*
  Returns:
    The MYON_SubDEdge this points at.
  */
  class MYON_SubDEdge* Edge() const;

  /*
  Returns:
    If Edge() is not nullptr, Edge()->m_id is returned.
    Otherwise, 0 is returned.
  */
  unsigned int EdgeId() const;
  
  /*
  Returns:
    If Edge() is not nullptr, Edge()->FaceCount() is returned.
    Otherwise, 0 is returned.
  */
  unsigned int EdgeFaceCount() const;

  /*
  Returns:
    If Edge() is not nullptr, Edge()->m_edge_tag is returned.
    Otherwise, MYON_SubDEdgeTag::Unset is returned.
  */
  MYON_SubDEdgeTag EdgeTag() const;
    
  /*
  Returns:
    If Edge() is not nullptr, Edge()->IsSmooth() is returned.
    Otherwise, false is returned.
  */
  bool EdgeIsSmooth() const;
    
  /*
  Returns:
    If Edge() is not nullptr, Edge()->IsCrease() is returned.
    Otherwise, false is returned.
  */
  bool EdgeIsCrease() const;
   
  /*
  Returns:
    If Edge() is not nullptr, Edge()->IsHardCrease() is returned.
    Otherwise, false is returned.
  */
  bool EdgeIsHardCrease() const;
   
  /*
  Returns:
    If Edge() is not nullptr, Edge()->IsDartCrease() is returned.
    Otherwise, false is returned.
  */
  bool EdgeIsDartCrease() const;

  /*
  Returns:
    If Edge() is not nullptr, Edge()->HasInteriorEdgeTopology(bRequireOppositeFaceDirections) is returned.
    Otherwise, false is returned.
  */
  bool HasInteriorEdgeTopology(
    bool bRequireOppositeFaceDirections
  ) const;

  /*
  Returns:
    0: this MYON_SubDEdgePtr is oriented from Edge()->Vertex(0) to Edge()->Vertex(1).
    1: this MYON_SubDEdgePtr is oriented from Edge()->Vertex(1) to Edge()->Vertex(0).
  */
  MYON__UINT_PTR EdgeDirection() const;

  /*
  Parameters:
    relative_vertex_index - [in]
      0: return Edge()->Vertex(EdgeDirection())
      1: return Edge()->Vertex(1-EdgeDirection())
  Returns:
    The requested vertex with EdgeDirection() taken into account.
    nullptr if relative_vertex_index, Edge() is nullptr, or Edge()->Vertex() is nullptr.
  */
  const class MYON_SubDVertex* RelativeVertex(
    int relative_vertex_index
  ) const;

  /*
  Parameters:
    relative_vertex_index - [in]
      0: return Edge()->Vertex(EdgeDirection())
      1: return Edge()->Vertex(1-EdgeDirection())
  Returns:
    The requested id of the vertex with EdgeDirection() taken into account.
    0 if relative_vertex_index, Edge() is nullptr, or Edge()->Vertex() is nullptr.
  */
  unsigned RelativeVertexId(
    int relative_vertex_index
  ) const;

  /*
  Parameters:
    relative_vertex_index - [in]
      0: return Edge()->Vertex(EdgeDirection())
      1: return Edge()->Vertex(1-EdgeDirection())
  Returns:
    The requested vertex control net point with EdgeDirection() taken into account.
    MYON_3dPoint::NanPoint if relative_vertex_index, Edge() is nullptr, or Edge()->Vertex() is nullptr.
  */
  const MYON_3dPoint RelativeControlNetPoint(
    int relative_vertex_index
  ) const;


  /*
  Parameters:
    relative_vertex_index - [in]
      0: return Edge()->Vertex(EdgeDirection())->SurfacePoint()
      1: return Edge()->Vertex(1-EdgeDirection())->SurfacePoint()
  Returns:
    The requested vertex surface point with EdgeDirection() taken into account.
    MYON_3dPoint::NanPoint if relative_vertex_index, Edge() is nullptr, or Edge()->Vertex() is nullptr.
  */
  const MYON_3dPoint RelativeVertexSurfacePoint(
    int relative_vertex_index
  ) const;

  /*
  Parameters:
    relative_vertex_index - [in]
      0: return Edge()->Vertex(EdgeDirection())->Point(point_location)
      1: return Edge()->Vertex(1-EdgeDirection())->Point(point_location)
    point_location - [in]
      Used to select control net or limit surface point.
  Returns:
    The requested vertex point with with EdgeDirection() taken into account.
    MYON_3dPoint::NanPoint if relative_vertex_index, Edge() is nullptr, or Edge()->Vertex() is nullptr.
  */
  const MYON_3dPoint RelativeVertexPoint(
    int relative_vertex_index,
    MYON_SubDComponentLocation point_location
  ) const;

  bool RelativeVertexMark(
    int relative_vertex_index,
    bool missing_vertex_return_value
  ) const;

  MYON__UINT8 RelativeVertexMarkBits(
    int relative_vertex_index,
    MYON__UINT8 missing_vertex_return_value
  ) const;

  const MYON_Line RelativeControlNetLine() const;

  const MYON_3dVector RelativeControlNetDirection() const;

  /*
  Description:
    The sector coefficient is a property of a smooth edge end
    that is attached to a dart, crease, or corner vertex.
    In all other cases the sector coefficient is ignored.
    The value of the sector coefficient is constant throughout subdivision.    
    Every smooth edge in a sector has the same sector coefficient at the central vertex.
  Parameters:
    relative_vertex_index - [in]
  Returns:
    If Edge() not nullptr, then
    If (relative_vertex_index = 0), returns Edge()->m_sector_coefficient(EdgeDirection())
    If (relative_vertex_index = 0), returns Edge()->m_sector_coefficient(1-EdgeDirection())
    Otherwise MYON_SubDSectorType::ErrorSectorCoefficient is returned.
  Remarks:
    The name "sector coefficient" is used because is is a property of the
    vertex's sector (every edge in vertex sector has the same value at the tagged vertex).
    The sector coefficient does not change when a subdivision is applied.
  */
  double RelativeSectorCoefficient(
    int relative_vertex_index
    ) const;

  void  SetRelativeSectorCoefficientForExperts(
    int relative_vertex_index,
    double relative_sector_coefficient
  ) const;


  const MYON_SubDEdgeSharpness RelativeSharpness() const;

  void SetRelativeSharpness(MYON_SubDEdgeSharpness relative_sharpness) const;

  /*
  Description:
    Get the face on the left or right side of an oriented manifold or boundary edge.
    A face is on the "left side" if this MYON_SubDEdgePtr is oriented so it
    points in the same direction as the face's oriented boundary.
    A face is on the "right side" if this MYON_SubDEdgePtr is oriented so it
    points in the opposite direction as the face's oriented boundary.
    If an edge is nonmanifold (3 or more faces), then nullptr is always returned.
    If an edge has two faces that do not attach to this edge with opposite orientations
    (nonoriented manifold edge), then nullptr is returned.    
  Parameters:
    relative_face_index - [in]
      0: return face on the left side of the edge with respect to EdgeOrientation().
      1: return face on the right side of the edge with respect to EdgeOrientation().
  Returns:
    The requested face.
  */
  const class MYON_SubDFace* RelativeFace(
    int relative_face_index
  ) const;

  /*
  Description:
    Return the neighboring face.
  Parameters:
    face - [in]
      A face attached to this edge.
    bStopAtCrease - [in]
      If true and if m_edge_tag = MYON_SubDEdgeTag::Crease,
      then nullptr is returned.
  Returns:
    If the m_face_count = 2,
    m_edge_tag is smooth or x or passes the crease tag test,
    one of m_face2[0,1] points a face, then
    the neighboring face is returned.
    In any other case, nullptr is returned.
  */
  const MYON_SubDFace* NeighborFace(
    const MYON_SubDFace* face,
    bool bStopAtCrease
  ) const;

  /*
  Returns:
    this->RelativeFace(relative_face_index)->Mark();
  */
  bool RelativeFaceMark(
    int relative_face_index,
    bool missing_face_return_value
  ) const;

  /*
  Returns:
    this->RelativeFace(relative_face_index)->MarkBits();
  */
  MYON__UINT8 RelativeFaceMarkBits(
    int relative_face_index,
    MYON__UINT8 missing_face_return_value
  ) const;

  /*
  Returns:
    The vector from RelativeVertex(0)->ControlNetPoint() to RelativeVertex(1)->ControlNetPoint(),
    or MYON_3dVector::NanVector if the relative vertex pointers are nullptr.
  */
  const MYON_3dVector RelativeDirection() const;

  /*
  Returns:
    this->Edge()->m_status.
  */
  const MYON_ComponentStatus Status() const;

  /*
  Returns:
    A MYON_SubDEdgePtr pointing at the same edge with the direction reversed from this.
  */
  const MYON_SubDEdgePtr Reversed() const;

  /// <summary>
  /// Get the SubD edge Catmull-Clark subdivision point.
  /// </summary>
  /// <returns>Catmull-Clark edge subdivision point.</returns>
  const MYON_3dPoint SubdivisionPoint() const;

  void ClearSavedSubdivisionPoints() const;

  /*
  Description:
    Clears saved subdivision and limit surface information for this component.
  Parameters:
    bClearNeighborhood - [in]
      If true, all components attached to this component are also cleared.
  */
  void ClearSavedSubdivisionPoints(
    bool bClearNeighborhood
  ) const;



  /*
  Parameters:
    edge - [in]
  Returns:
    An MYON_SubDEdgePtr pointing at edge with direction = 0 (not reversed).
  */
  static const MYON_SubDEdgePtr Create(
    const class MYON_SubDEdge* edge
    );

  /*
  Parameters:
    edge - [in]
    direction - [in]
      0: not reversed
      1: reversed
  Returns:
    An MYON_SubDEdgePtr pointing at edge with the specified direction.
  */
  static const MYON_SubDEdgePtr Create(
    const class MYON_SubDEdge* edge,
    MYON__UINT_PTR direction
    );

  /*
  Parameters:
    v0 - [in]
    v1 - [in]
  Returns:
    If there is an edge connecting v0 and v1, then an MYON_SubDEdgePtr pointing to that edge 
    and oriented from v0 to v1 is returned. Otherwise MYON_SubDEdgePtr::Null is returned.
  */
  static const MYON_SubDEdgePtr Create(
    const class MYON_SubDVertex* v0,
    const class MYON_SubDVertex* v1
  );


  static const MYON_SubDEdgePtr Create(
    const class MYON_SubDComponentPtr& edge_component
    );

  /*
  Parameters:
    edge - [in]
    start_vertex - [in]
      One of the edge's vertices.
  Returns:
     An MYON_SubDEdgePtr pointing at edge with RelativeVertex(0) = start_vertex.
  */
  static const MYON_SubDEdgePtr CreateFromStartVertex(
    const class MYON_SubDEdge* edge,
    const MYON_SubDVertex* start_vertex
  );

  /*
  Parameters:
    edge - [in]
    end_vertex - [in]
      One of the edge's vertices.
  Returns:
     An MYON_SubDEdgePtr pointing at edge with RelativeVertex(1) = end_vertex.
  */
  static const MYON_SubDEdgePtr CreateFromEndVertex(
    const class MYON_SubDEdge* edge,
    const MYON_SubDVertex* end_vertex
  );


  /*
  Returns:
    The current value of the component mark ( m_status->RuntimeMark() ).
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specific state.
  */
  bool Mark() const;

  /*
  Description:
    Clears (sets to false) the value of the component mark.
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specific state.
  Returns:
    Input value of Mark().
  */
  bool ClearMark() const;

  /*
  Description:
    Sets (sets to true) the value of the component mark.
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specific state.
  Returns:
    Input value of Mark().
  */
  bool SetMark() const;

  /*
  Description:
    Sets the value of the component mark to bMark.
  Parameter:
    bMark - [in]
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specific state.
  Returns:
    Input value of Mark().
  */
  bool SetMark(
    bool bMark
  ) const;

  MYON__UINT8 MarkBits() const;

  MYON__UINT8 SetMarkBits(
    MYON__UINT8 mark_bits
  ) const;

  MYON__UINT8 ClearMarkBits() const;
}; // MYON_SubDFace

#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_SubDEdgePtr>;
#endif

class MYON_CLASS MYON_SubDFacePtr
{
public:
  // For performance reasons, m_ptr is not initialized and no constructors are declared
  // or implemented.  If you require initialization, then use x = MYON_SubDFacePtr::Null
  // or x = MYON_SubDFacePtr::Create(...).
  MYON__UINT_PTR m_ptr;

  static const MYON_SubDFacePtr Null;

  bool IsNull() const;

  bool IsNotNull() const;

  /*
  Returns:
    True if this face is active in its parent subd or other
    relevant context.
  Remarks:
    When a component is in use, IsActive() = true. 
    If was used and then deleted, IsActive() is false.
  */
  bool IsActive() const;


  class MYON_SubDFace* Face() const;
  
  /*
  Returns:
    If Face() is not nullptr, Face()->m_id is returned.
    Otherwise, 0 is returned.
  */
  unsigned int FaceId() const;

  /*
  Returns:
    If Face() is not nullptr, Face()->EdgeCount() is returned.
    Otherwise, 0 is returned.
  */
  unsigned int FaceEdgeCount() const;
  
  MYON__UINT_PTR FaceDirection() const;

  const MYON_ComponentStatus Status() const;

  /*
  Returns:
    A MYON_SubDFacePtr pointing at the same face with the direction reversed from this.
  */
  const MYON_SubDFacePtr Reversed() const;

  static const MYON_SubDFacePtr Create(
    const class MYON_SubDFace* face,
    MYON__UINT_PTR direction
    );

  static const MYON_SubDFacePtr Create(
    const class MYON_SubDComponentPtr& face_component
    );

  static int Compare(
    const MYON_SubDFacePtr* lhs,
    const MYON_SubDFacePtr* rhs
  );

  static int CompareFacePointer(
    const MYON_SubDFacePtr* lhs,
    const MYON_SubDFacePtr* rhs
  );

  /*
  Returns:
    The current value of the component mark ( m_status->RuntimeMark() ).
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specific state.
  */
  bool Mark() const;

  /*
  Description:
    Clears (sets to false) the value of the component mark.
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specific state.
  Returns:
    Input value of Mark().
  */
  bool ClearMark() const;

  /*
  Description:
    Sets (sets to true) the value of the component mark.
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specific state.
  Returns:
    Input value of Mark().
  */
  bool SetMark() const;

  /*
  Description:
    Sets the value of the component mark to bMark.
  Parameter:
    bMark - [in]
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specific state.
  Returns:
    Input value of Mark().
  */
  bool SetMark(
    bool bMark
  ) const;

  MYON__UINT8 MarkBits() const;

  MYON__UINT8 SetMarkBits(
    MYON__UINT8 mark_bits
  ) const;

  MYON__UINT8 ClearMarkBits() const;

  void ClearSavedSubdivisionPoints() const;

  /*
  Description:
    Clears saved subdivision and limit surface information for this component.
  Parameters:
    bClearNeighborhood - [in]
      If true, all components attached to this component are also cleared.
  */
  void ClearSavedSubdivisionPoints(
    bool bClearNeighborhood
  ) const;

};

#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_SubDFacePtr>;
#endif


class MYON_CLASS MYON_SubDComponentPtr
{
public:
  // For performance reasons, m_ptr is not initialized and no constructors are declared
  // or implemented.  If you require initialization, then use x = MYON_SubDComponentPtr::Null
  // or x = MYON_SubDComponentPtr::Create(...).
  MYON__UINT_PTR m_ptr;

  static const MYON_SubDComponentPtr Null; //  // nullptr, type = unset, mark = 0

  /// <summary>
  /// MYON_SubDComponentPtr::Type identifies the type of subdivision component referenced by
  /// the MYON_SubDComponentPtr.
  /// </summary>
  enum class Type : unsigned char
  {
    Unset = 0,
    Vertex = 2,
    Edge = 4,
    Face = 6
  };

  static MYON_SubDComponentPtr::Type ComponentPtrTypeFromUnsigned(
    unsigned int component_pointer_type_as_unsigned
    );

   /*
   Description:
     MYON_SubDComponentPtr::Type::Vertex 
     < MYON_SubDComponentPtr::Type::Edge
     < MYON_SubDComponentPtr::Type::Face
     < MYON_SubDComponentPtr::Type::Unset
     < invalid
   */
  static int CompareComponentPtrType(
    MYON_SubDComponentPtr::Type a,
    MYON_SubDComponentPtr::Type b
    );

  static int CompareType(
    const MYON_SubDComponentPtr* a,
    const MYON_SubDComponentPtr* b
    );

  /*
  Description:
    Dictionary compares type and ComponentBase() pointer as an unsigned.
  */
  static int CompareComponent(
    const MYON_SubDComponentPtr* a,
    const MYON_SubDComponentPtr* b
  );

  /*
  Description:
    Dictionary compares type, ComponentBase() pointer as an unsigned,
    and ComponentDirection().
  */
  static int CompareComponentAndDirection(
    const MYON_SubDComponentPtr* a,
    const MYON_SubDComponentPtr* b
  );


  /*
  Returns:
    True if the ComponentBase() pointer is nullptr. Note that type and mark may be set.
  */
  bool IsNull() const;

  /*
  Returns:
    True if type is set and ComponentBase() pointer is not nullptr. Note that mark may be set as well.
  */
  bool IsNotNull() const;

  /*
  Returns:
    True if this component is active in its parent subd or other
    relevant context.
  Remarks:
    When a component is in use, IsActive() = true. 
    If was used and then deleted, IsActive() is false.
  */
  bool IsActive() const;

  /*
  Returns:
    True if this component is marked as a primary motif component.
  Remarks:
    You must use MYON_SubD SymmetrySet member functions to get symmetry set contents.
  */
  bool IsSymmetrySetPrimaryMotif() const;

  /*
  Returns:
    True if this component is marked being in a symmetry set.
  Remarks:
    You must use MYON_SubD SymmetrySet member functions to get symmetry set contents.
  */
  bool InSymmetrySet() const;

  MYON_SubDComponentPtr::Type ComponentType() const;

  class MYON_SubDComponentBase* ComponentBase() const;
  
  /*
  type_filter - [in]
    If is MYON_SubDComponentPtr::Type::Unset, then any type of component will be returned.
    Otherwise only a component of the specified type will be returned.
  */
  class MYON_SubDComponentBase* ComponentBase(MYON_SubDComponentPtr::Type type_filter) const;

  class MYON_SubDVertex* Vertex() const;
  class MYON_SubDEdge* Edge() const;
  class MYON_SubDFace* Face() const;

  const MYON_SubDVertexPtr VertexPtr() const;
  const MYON_SubDEdgePtr EdgePtr() const;
  const MYON_SubDFacePtr FacePtr() const;

  const bool IsVertex() const;
  const bool IsEdge() const;
  const bool IsFace() const;

  void ClearSavedSubdivisionPoints() const;

  /*
  Description:
    Clears saved subdivision and limit surface information for this component.
  Parameters:
    bClearNeighborhood - [in]
      If true, all components attached to this component are also cleared.
  */
  void ClearSavedSubdivisionPoints(
    bool bClearNeighborhood
  ) const;

  unsigned int ComponentId() const;

  const MYON_COMPONENT_INDEX ComponentIndex() const;

  const MYON_3dPoint ControlNetCenterPoint() const;
  const MYON_BoundingBox ControlNetBoundingBox() const;

  /*
  Returns:
    A value suitable for hash table used based on the component type and id.
  */
  MYON__UINT16 Hash16FromTypeAndId() const;


  /*
  Returns:
    A value suitable for hash table used based on the value of ComponentBase().
  */
  MYON__UINT32 Hash32FromPointer() const;

  /*
  Returns:
    0 or 1.
    A runtime bit property on this MYON_SubDComponentPtr. 
    The use of this value varies depending on the context.
    Frequently, 0 means the referenced component is being used with its
    natural orientation and 1 means the referenced component is being used
    with the reverse of its natural oreientation.
  */
  MYON__UINT_PTR ComponentDirection() const;

  /*
  Returns:
    An MYON_SubDComponentPtr referencing the same MYON_SubDComponentBase
    with MYON_SubDComponentPtr.ComponentDirection() = 0.
  */
  const MYON_SubDComponentPtr ClearComponentDirection() const;

  /*
  Returns:
    An MYON_SubDComponentPtr referencing the same MYON_SubDComponentBase
    with MYON_SubDComponentPtr.ComponentDirection() = 1.
  */
  const MYON_SubDComponentPtr SetComponentDirection() const;

  /*
  Returns:
    An MYON_SubDComponentPtr referencing the same MYON_SubDComponentBase
    with MYON_SubDComponentPtr.ComponentDirection() = 1.
  */
  const MYON_SubDComponentPtr SetComponentDirection(MYON__UINT_PTR dir) const;
   
  /*
  Returns:
    An MYON_SubDComponentPtr referencing the same MYON_SubDComponentBase
    with ComponentDirection() = 1 - this->ComponentDirection().
  */
  const MYON_SubDComponentPtr Reversed() const;

  const MYON_ComponentStatus Status() const;

  /*
  Returns:
    1: status changed.
    0: status not changed.
  */
  unsigned int SetStates(
    MYON_ComponentStatus states_to_set
    );

  /*
  Returns:
    1: status changed.
    0: status not changed.
  */
  unsigned int ClearStates(
    MYON_ComponentStatus states_to_clear
    );

  /*
  Description:
    Makes "this" an exact copy of status.
  Parameters:
    status - [in]
  Returns:
    1: status changed.
    0: status not changed.
  */
  unsigned int SetStatus(
    MYON_ComponentStatus status
    );

  /*
  Returns:
    The current value of the component mark ( m_status->RuntimeMark() ).
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specific state.
  */
  bool Mark() const;

  /*
  Description:
    Clears (sets to false) the value of the component mark.
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specific state.
  Returns:
    Input value of Mark().
  */
  bool ClearMark() const;

  /*
  Description:
    Sets (sets to true) the value of the component mark.
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specific state.
  Returns:
    Input value of Mark().
  */
  bool SetMark() const;

  /*
  Description:
    Sets the value of the component mark to bMark.
  Parameter:
    bMark - [in]
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specific state.
  Returns:
    Input value of Mark().
  */
  bool SetMark(
    bool bMark
  ) const;

  MYON__UINT8 MarkBits() const;

  MYON__UINT8 SetMarkBits(
    MYON__UINT8 mark_bits
  ) const;

  MYON__UINT8 ClearMarkBits() const;

  /*
  Parameters:
    null_component_value - [in]
      Value to return if the component is null.
  Returns:
    If this is not null, the group id of the component is returned.
    Otherwise null_component_value is returned.
  */
  unsigned int GroupId(
    unsigned int null_component_value
  ) const;

  /*
  Description:
    Sets MYON_SubDComponentBase.m_group_id.
  Parameters:
    group_id - [in]
      Value to return if the component is null.
  Returns:
    True if the component is not null and the group id was set.
    False if the component is null.
  */
  bool SetGroupId(
    unsigned int group_id
  );

  static
  const MYON_SubDComponentPtr CreateNull(
    MYON_SubDComponentPtr::Type component_type,
    MYON__UINT_PTR component_direction
    );

  static
  const MYON_SubDComponentPtr Create(
    const class MYON_SubDVertex* vertex
    );

  static
  const MYON_SubDComponentPtr Create(
    const class MYON_SubDEdge* edge
    );

  static
  const MYON_SubDComponentPtr Create(
    const class MYON_SubDFace* face
    );

  static
  const MYON_SubDComponentPtr Create(
    const class MYON_SubDVertex* vertex,
    MYON__UINT_PTR vertex_direction
    );

  static
  const MYON_SubDComponentPtr Create(
    const class MYON_SubDEdge* edge,
    MYON__UINT_PTR edge_direction
    );

  static
  const MYON_SubDComponentPtr Create(
    const class MYON_SubDFace* face,
    MYON__UINT_PTR face_direction
    );
  
  static
  const MYON_SubDComponentPtr Create(
    MYON_SubDVertexPtr vertexptr
    );
  
  static
  const MYON_SubDComponentPtr Create(
    MYON_SubDEdgePtr edgeptr
    );

  static
  const MYON_SubDComponentPtr Create(
    MYON_SubDFacePtr faceptr
    );

  wchar_t* ToString(
    wchar_t* s,
    size_t s_capacity
  ) const;

  const MYON_wString ToString() const;

};

#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_SubDComponentPtr>;
#endif

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDComponentPtrAndNumber
//

class MYON_CLASS MYON_SubDComponentAndNumber
{
public:
  MYON_SubDComponentAndNumber() = default;
  ~MYON_SubDComponentAndNumber() = default;
  MYON_SubDComponentAndNumber(const MYON_SubDComponentAndNumber&) = default;
  MYON_SubDComponentAndNumber& operator=(const MYON_SubDComponentAndNumber&) = default;

public:
  static const MYON_SubDComponentAndNumber NullAndNan;
  static const MYON_SubDComponentAndNumber NullAndZero;
  static const MYON_SubDComponentAndNumber Create(
    MYON_SubDComponentPtr cptr,
    double x
  );

  static const MYON_SubDComponentAndNumber Create(
    const MYON_SubDVertex* v,
    double x
  );
  static const MYON_SubDComponentAndNumber Create(
    const MYON_SubDEdge* e,
    double x
  );
  static const MYON_SubDComponentAndNumber Create(
    const MYON_SubDFace* f,
    double x
  );

  static const MYON_SubDComponentAndNumber Create(
    const MYON_SubDVertexPtr vptr,
    double x
  );

  static const MYON_SubDComponentAndNumber Create(
    const MYON_SubDEdgePtr eptr,
    double x
  );

  static const MYON_SubDComponentAndNumber Create(
    const MYON_SubDFacePtr fptr,
    double x
  );

public:

  /*
  Description:
    Compare Component() using MYON_SubDComponentPtr::CompareComponent().
  */
  static int CompareComponent(
    const MYON_SubDComponentAndNumber* a,
    const MYON_SubDComponentAndNumber* b
  );

  /*
  Description:
    Compare Component() using MYON_SubDComponentPtr::CompareComponentAndDirection().
  */
  static int CompareComponentAndDirection(
    const MYON_SubDComponentAndNumber* a,
    const MYON_SubDComponentAndNumber* b
  );


  /*
  Description:
    Compare Number() nans are treated as equal and sort last.
  */
  static int CompareNumber(
    const MYON_SubDComponentAndNumber* a,
    const MYON_SubDComponentAndNumber* b
  );

  /*
  Description:
    Dictionary compare Component() and Number() in that order.
  */
  static int CompareComponentAndNumber(
    const MYON_SubDComponentAndNumber* a,
    const MYON_SubDComponentAndNumber* b
  );

  /*
  Description:
    Dictionary compare using MYON_SubDComponentAndNumber::CompareComponentAndDirection() and MYON_SubDComponentAndNumber::Number() in that order.
  */
  static int CompareComponentAndDirectionAndNumber(
    const MYON_SubDComponentAndNumber* a,
    const MYON_SubDComponentAndNumber* b
  );


public:
  const MYON_SubDComponentPtr Component() const;
  void SetComponent(MYON_SubDComponentPtr cptr);

  double Number() const;
  void SetNumber(double x);

public:
  MYON_SubDComponentPtr m_cptr = MYON_SubDComponentPtr::Null;
  double m_x = MYON_DBL_QNAN;
};

#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_SubDComponentAndNumber>;
#endif

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDComponentAndPoint
//

class MYON_CLASS MYON_SubDComponentAndPoint
{
public:
  MYON_SubDComponentAndPoint() = default;
  ~MYON_SubDComponentAndPoint() = default;
  MYON_SubDComponentAndPoint(const MYON_SubDComponentAndPoint&) = default;
  MYON_SubDComponentAndPoint& operator=(const MYON_SubDComponentAndPoint&) = default;

public:
  static const MYON_SubDComponentAndPoint NullAndNan;
  static const MYON_SubDComponentAndPoint NullAndOrigin;
  static const MYON_SubDComponentAndPoint Create(
    MYON_SubDComponentPtr cptr,
    MYON_3dPoint P
  );

  static const MYON_SubDComponentAndPoint Create(
    const MYON_SubDVertex* v,
    MYON_3dPoint P
  );
  static const MYON_SubDComponentAndPoint Create(
    const MYON_SubDEdge* e,
    MYON_3dPoint P
  );
  static const MYON_SubDComponentAndPoint Create(
    const MYON_SubDFace* f,
    MYON_3dPoint P
  );

  static const MYON_SubDComponentAndPoint Create(
    const MYON_SubDVertexPtr vptr,
    MYON_3dPoint P
  );

  static const MYON_SubDComponentAndPoint Create(
    const MYON_SubDEdgePtr eptr,
    MYON_3dPoint P
  );

  static const MYON_SubDComponentAndPoint Create(
    const MYON_SubDFacePtr fptr,
    MYON_3dPoint P
  );

public:

  /*
  Description:
    Compare Component() using MYON_SubDComponentPtr::CompareComponent().
  */
  static int CompareComponent(
    const MYON_SubDComponentAndPoint* lhs,
    const MYON_SubDComponentAndPoint* rhs
  );

  /*
  Description:
    Compare Component() using MYON_SubDComponentPtr::CompareComponentAndDirection().
  */
  static int CompareComponentAndDirection(
    const MYON_SubDComponentAndPoint* lhs,
    const MYON_SubDComponentAndPoint* rhs
  );


  /*
  Description:
    Compare Point() uses MYON_3dPoint::Compare() to compare lhs and rhs Point() values.
  */
  static int ComparePoint(
    const MYON_SubDComponentAndPoint* lhs,
    const MYON_SubDComponentAndPoint* rhs
  );

  /*
  Description:
    Dictionary compare Component() and Point() in that order.
  */
  static int CompareComponentAndPoint(
    const MYON_SubDComponentAndPoint* lhs,
    const MYON_SubDComponentAndPoint* rhs
  );

  /*
  Description:
    Dictionary compare using MYON_SubDComponentAndPoint::CompareComponentAndDirection() and MYON_SubDComponentAndPoint::Point() in that order.
  */
  static int CompareComponentAndDirectionAndPoint(
    const MYON_SubDComponentAndPoint* lhs,
    const MYON_SubDComponentAndPoint* rhs
  );


public:
  const MYON_SubDComponentPtr Component() const;
  void SetComponent(MYON_SubDComponentPtr cptr);

  const MYON_3dPoint Point() const;
  void SetPoint(MYON_3dPoint P);

public:
  MYON_SubDComponentPtr m_cptr = MYON_SubDComponentPtr::Null;
  MYON_3dPoint m_P = MYON_3dPoint::NanPoint;
};

#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_SubDComponentAndPoint>;
#endif

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDComponentTest
//

/*
Description:
  MYON_SubDComponentTest is used in contexts where custom testing or filtering of
  SubD components is required.
*/
class MYON_CLASS MYON_SubDComponentTest
{
public:
  MYON_SubDComponentTest() = default;
  virtual ~MYON_SubDComponentTest();
  MYON_SubDComponentTest(const MYON_SubDComponentTest&) = default;
  MYON_SubDComponentTest& operator=(const MYON_SubDComponentTest&) = default;

  /*
    Sets m_ptr=ptr
  */
  MYON_SubDComponentTest(MYON__UINT_PTR ptr);

  /*
  Description:
    Typically, a derived class overrides this function, uses it to inspect vertex properties,
    and returns true or false.
  Parameters:
    v - [in] vertex being tested.
  Returns:
    true if the vertex "passes" the test.
    false if the vertex "fails" the text.
  Remarks:
    The default implementation returns (cptr.IsNotNull() && 0 != m_ptr);
  */
  virtual bool Passes(const MYON_SubDComponentPtr cptr) const;

  /*
  Returns:
    this->Passes(nullptr != v ? v->ComponentPtr() : MYON_SubDComponentPtr::Null);
  */
  bool Passes(const class MYON_SubDVertex* v) const;

  /*
  Returns:
    this->Passes(nullptr != e ? e->ComponentPtr() : MYON_SubDComponentPtr::Null);
  */
  bool Passes(const class MYON_SubDEdge* e) const;

  /*
  Returns:
    this->Passes(nullptr != f ? f->ComponentPtr() : MYON_SubDComponentPtr::Null);
  */
  bool Passes(const class MYON_SubDFace* f) const;

  // Passes() returns true for every non nullptr component.
  static const MYON_SubDComponentTest AllPass;

  // Passes() returns false for every component.
  static const MYON_SubDComponentTest AllFail;

protected:
  // classes derived from MYON_SubDVertexFilter may use m_ptr as they see fit including to completely ignore it.
  MYON__UINT_PTR m_ptr = 0;
};

class MYON_CLASS MYON_SubDComponentId
{
public:

  // type = unset and id = 0;
  static const MYON_SubDComponentId Unset;

  MYON_SubDComponentId() = default;
  ~MYON_SubDComponentId() = default;
  MYON_SubDComponentId(const MYON_SubDComponentId&) = default;
  MYON_SubDComponentId& operator=(const MYON_SubDComponentId&) = default;

  MYON_SubDComponentId(MYON_SubDComponentPtr::Type component_type, unsigned int component_id);
  MYON_SubDComponentId(MYON_SubDComponentPtr cptr);
  MYON_SubDComponentId(const class MYON_SubDVertex* v);
  MYON_SubDComponentId(const class MYON_SubDEdge* e);
  MYON_SubDComponentId(const class MYON_SubDFace* f);

  static int CompareTypeAndId(const MYON_SubDComponentId& lhs, const MYON_SubDComponentId& rhs);
  static int CompareTypeAndIdFromPointer(const MYON_SubDComponentId* lhs, const MYON_SubDComponentId* rhs);

  unsigned int ComponentId() const;
  MYON_SubDComponentPtr::Type ComponentType() const;

  /*
  Returns:
    true if type is not unset and id > 0
  */
  bool IsSet() const;

private:
  unsigned int m_id = 0;
  MYON_SubDComponentPtr::Type m_type = MYON_SubDComponentPtr::Type::Unset;
  unsigned char m_reserved1 = 0;
  unsigned short m_reserved2 = 0;
};

#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_SubDComponentId>;
#endif

class MYON_CLASS MYON_SubDComponentIdList : public MYON_SubDComponentTest
{
public:
  MYON_SubDComponentIdList() = default;
  ~MYON_SubDComponentIdList() = default;
  MYON_SubDComponentIdList(const MYON_SubDComponentIdList&) = default;
  MYON_SubDComponentIdList& operator=(const MYON_SubDComponentIdList&) = default;

  /*
  Returns:
    returns InListPassesResult() when cptr id is in the list.
    returns !InListPassesResult() when cptr id is not the list.
  Remarks:
    AddId is safe to use in multithreaded contexts.
  */
  bool Passes(const MYON_SubDComponentPtr cptr) const override;

  void SetInListPassesResult(bool bInListPassesResult);

  // Value Passes(cptr) returns when cptr is in the list
  bool InListPassesResult() const;

  void AddId(MYON_SubDComponentId cid);

  void AddId(MYON_SubDComponentPtr cptr);

  /*
  Add all Ids in list to m_id_list. Do not sort or remove duplicates.
  Assumes list elements all pass el.IsSet().
  Parameters:
    list - in
  */
  void AddIdList(const MYON_SubDComponentIdList& list);

  unsigned int UnsignedCount() const;

  MYON_SubDComponentId operator[](unsigned int i) const;

  /*
  Parameters:
    cid - in
  Returns:
    true if cid is in this list.
  */
  bool InList(MYON_SubDComponentId cid) const;

  /*
  Parameters:
    cptr - in
  Returns:
    true if cptr's id is in this list.
  */
  bool InList(MYON_SubDComponentPtr cptr) const;

private:
  unsigned int m_reserved1 = 0;
private:
  unsigned short m_reserved2 = 0;
private:
  bool m_bInListPassesResult = true;
private:
  mutable bool m_bSorted = false;
  mutable MYON_SimpleArray<MYON_SubDComponentId> m_id_list;
};

class MYON_CLASS MYON_SubDComponentPtrPair
{
public:
  // For performance reasons, m_ptrpair is not initialized and no constructors are declared
  // or implemented.  If you require initialization, then use x = MYON_SubDComponentPtrPair::Null
  // or x = MYON_SubDComponentPtr::Create(first_ptr,second_ptr).
  MYON_SubDComponentPtr m_pair[2];

public:
  static const MYON_SubDComponentPtrPair Create(MYON_SubDComponentPtr first_ptr, MYON_SubDComponentPtr second_ptr);

  /*
  Description:
    Dictionary order compare using MYON_SubDComponentPtr::CompareComponent() on each element.
  */
  static int CompareComponent(const MYON_SubDComponentPtrPair * lhs, const MYON_SubDComponentPtrPair * rhs);

  /*
  Description:
    Dictionary order compare using MYON_SubDComponentPtr::CompareComponentAndDirection() on each element.
  */
  static int CompareComponentAndDirection(const MYON_SubDComponentPtrPair * lhs, const MYON_SubDComponentPtrPair * rhs);

  /*
  Description:
    Compare first pointer value.
  */
  static int CompareFirstPointer(const MYON_SubDComponentPtrPair * lhs, const MYON_SubDComponentPtrPair * rhs);

  /*
  Returns:
    A pair with components in the opposite order
  */
  const MYON_SubDComponentPtrPair SwapPair() const;


  /*
  Returns:
    A pair with components reversed.
  */
  const MYON_SubDComponentPtrPair ReversedPair() const;

  /*
  Returns:
    First MYON_SubDComponentPt in the pair.
  */
  const MYON_SubDComponentPtr First() const;

  /*
  Returns:
    Second MYON_SubDComponentPt in the pair.
  */
  const MYON_SubDComponentPtr Second() const;

  /*
  Returns:
    If both points have the same type, that type is returned.
    Otherwise MYON_SubDComponentPtr::Type::Unset is returned.
  */
  MYON_SubDComponentPtr::Type ComponentType() const;

  /*
    Returns FIrst().IsNull().
  */
  bool FirstIsNull() const;

  /*
    Returns Second().IsNull().
  */
  bool SecondIsNull() const;

  /*
    Returns First().IsNull() && Second().IsNull().
  */
  bool BothAreNull() const;
  
  /*
    Returns First().IsNotNull().
  */
  bool FirstIsNotNull() const;

  /*
    Returns Second().IsNotNull().
  */
  bool SecondIsNotNull() const;

  /*
    Returns FirstIsNotNull() && SecondIsNotNull().
  */
  bool BothAreNotNull() const;

public:
  static const MYON_SubDComponentPtrPair Null;
};

#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_SubDComponentPtrPair>;
#endif

class MYON_CLASS MYON_SubDComponentPtrPairHashTable : MYON_Hash32Table
{
public:
  MYON_SubDComponentPtrPairHashTable();

  MYON_SubDComponentPtrPairHashTable(const class MYON_SubD& subd);
  
  ~MYON_SubDComponentPtrPairHashTable() = default;
  
  bool AddComponentPair(
    MYON_SubDComponentPtr first_component,
    MYON_SubDComponentPtr second_component
  );

  bool AddVertexPair(
    const class MYON_SubDVertex* first_v,
    const class MYON_SubDVertex* second_v
  );

  bool AddEdgePair(
    const class MYON_SubDEdge* first_e,
    const class MYON_SubDEdge* second_e
  );

  bool AddEdgePair(
    const class MYON_SubDEdge* first_e,
    const MYON_SubDEdgePtr second_eptr
  );

  bool AddFacePair(
    const class MYON_SubDFace* first_f,
    const class MYON_SubDFace* second_f
  );

  const MYON_SubDComponentPtrPair PairFromSecondComponentPtr(
    MYON_SubDComponentPtr second_component
  );

  const MYON_SubDComponentPtrPair PairFromSecondVertex(
    const class MYON_SubDVertex* second_v
  );

  const MYON_SubDComponentPtrPair PairFromSecondEdge(
    const class MYON_SubDEdge* second_e
  );

private:
  MYON_FixedSizePool m_pairs_fsp;

private:
  MYON_SubDComponentPtrPairHashTable(const MYON_SubDComponentPtrPairHashTable&) = delete;
  MYON_SubDComponentPtrPairHashTable& operator=(const MYON_SubDComponentPtrPairHashTable&) = delete;
};

/// <summary>
/// Simple arrays of MYON_SubD_ComponentIdTypeAndTag elements are used to save 
/// original tag values so the can be retrieved after complex editing operations.
/// </summary>
class MYON_CLASS MYON_SubD_ComponentIdTypeAndTag
{
public:
  static const MYON_SubD_ComponentIdTypeAndTag Unset;

  /*
  Returns:
    If v is no nullptr and v->m_id > 0, a MYON_SubD_ComponentIdTypeAndTag with VertexTag() = v->m_vertex_tag is returned.
    Otherwise MYON_SubD_ComponentIdTypeAndTag::Unset is returned.
  */
  static const MYON_SubD_ComponentIdTypeAndTag CreateFromVertex(const class MYON_SubDVertex* v);

  /*
  Returns:
    If vertex_id > 0, a MYON_SubD_ComponentIdTypeAndTag with VertexTag() = vtag is returned.
    Otherwise MYON_SubD_ComponentIdTypeAndTag::Unset is returned.
  */
  static const MYON_SubD_ComponentIdTypeAndTag CreateFromVertexId(unsigned vertex_id, MYON_SubDVertexTag vtag);

  /*
  Parameters:
    e - [in]
      If e->m_edge_tag is MYON_SubDEdgeTag::SmoothX, the MYON_SubD_ComponentIdTypeAndTag EdgeTag() will be MYON_SubDEdgeTag::Smoooth.
  Returns:
    If e is not nullptr and e->m_id > 0, a MYON_SubD_ComponentIdTypeAndTag with EdgeTag() = e->m_edge_tag is returned.
    Otherwise MYON_SubD_ComponentIdTypeAndTag::Unset is returned.
  */
  static const MYON_SubD_ComponentIdTypeAndTag CreateFromEdge(const class MYON_SubDEdge* e);


  /*
  Parameters:
    edge_id - [in]
    etag - [in]
      If etag is MYON_SubDEdgeTag::SmoothX, the MYON_SubD_ComponentIdTypeAndTag EdgeTag() will be MYON_SubDEdgeTag::Smoooth.
  Returns:
    If edge_id > 0, a MYON_SubD_ComponentIdTypeAndTag with EdgeTag() = etag is returned.
    Otherwise MYON_SubD_ComponentIdTypeAndTag::Unset is returned.
  */
  static const MYON_SubD_ComponentIdTypeAndTag CreateFromEdgeId(unsigned edge_id, MYON_SubDEdgeTag etag);


  /*
  Parameters:
    f - [in]
    ftag - [in]
      Any value and the interpretation is up to the context using the MYON_SubD_ComponentIdTypeAndTag.
  Returns:
    If f is no nullptr and f->m_id > 0, a MYON_SubD_ComponentIdTypeAndTag with FaceTag() = ftag is returned.
    Otherwise MYON_SubD_ComponentIdTypeAndTag::Unset is returned.
  Remarks:
    SubD faces do not have a tag in the sense that vertices and edges do, but in complex editing tasks
    it is sometimes useful to include faces in an array of MYON_SubD_ComponentIdTypeAndTag elements.
  */
  static const MYON_SubD_ComponentIdTypeAndTag CreateFromFace(const class MYON_SubDFace* f, unsigned char ftag);

  /*
  Parameters:
    face_id - [in]
    ftag - [in]
      Any value and the interpretation is up to the context using the MYON_SubD_ComponentIdTypeAndTag.
  Returns:
    If face_id > 0, a MYON_SubD_ComponentIdTypeAndTag with FaceTag() = vtag is returned.
    Otherwise MYON_SubD_ComponentIdTypeAndTag::Unset is returned.
  Remarks:
    SubD faces do not have a tag in the sense that vertices and edges do, but in complex editing tasks
    it is sometimes useful to include faces in an array of MYON_SubD_ComponentIdTypeAndTag elements.
  */
  static const MYON_SubD_ComponentIdTypeAndTag CreateFromFaceId(unsigned face_id, unsigned char ftag);

  /*
  Description:
    Dictionary compare on ComponentType() and ComponentId() in that order.
  */
  static int CompareTypeAndId(const MYON_SubD_ComponentIdTypeAndTag* lhs, const MYON_SubD_ComponentIdTypeAndTag* rhs);

  /*
  Description:
    Dictionary compare on ComponentType(), ComponentId(), and tag in that order.
  */
  static int CompareTypeAndIdAndTag(const MYON_SubD_ComponentIdTypeAndTag* lhs, const MYON_SubD_ComponentIdTypeAndTag* rhs);

  /*
  Parameters:
    v - [in]
    sorted_tags[] - [in]
      Array sorted by MYON_SubD_ComponentIdTypeAndTag::CompareTypeAndId().
  Returns:
    If v is in sorted_tags[], the VertexTag() from from sorted_tags[] is returned.
    Otherwise v->m_vertex_tag is returned.
  */
  static MYON_SubDVertexTag OriginalVertexTag(
    const class MYON_SubDVertex* v,
    const MYON_SimpleArray< MYON_SubD_ComponentIdTypeAndTag>& sorted_tags
  );

  /*
  Parameters:
    vertex_id - [in]
    sorted_tags[] - [in]
      Array sorted by MYON_SubD_ComponentIdTypeAndTag::CompareTypeAndId().
  Returns:
    If vertex_id is in sorted_tags[], the VertexTag() from from sorted_tags[] is returned.
    Otherwise MYON_SubDVertexTag::Unset is returned.
  */
  static MYON_SubDVertexTag OriginalVertexTag(
    unsigned vertex_id,
    const MYON_SimpleArray< MYON_SubD_ComponentIdTypeAndTag>& sorted_tags
  );

  /*
  Parameters:
    e - [in]
    sorted_tags[] - [in]
      Array sorted by MYON_SubD_ComponentIdTypeAndTag::CompareTypeAndId().
  Returns:
    If e is in sorted_tags[], the EdgeTag() from from sorted_tags[] is returned.
    Otherwise e->m_edge_tag is returned.
  */
  static MYON_SubDEdgeTag OriginalEdgeTag(
    const class MYON_SubDEdge* e,
    const MYON_SimpleArray< MYON_SubD_ComponentIdTypeAndTag>& sorted_tags
  );

  /*
  Parameters:
    edge_id - [in]
    sorted_tags[] - [in]
      Array sorted by MYON_SubD_ComponentIdTypeAndTag::CompareTypeAndId().
  Returns:
    If edge_id is in sorted_tags[], the EdgeTag() from from sorted_tags[] is returned.
    Otherwise MYON_SubDEdgeTag::Unset is returned.
  */
  static MYON_SubDEdgeTag OriginalEdgeTag(
    unsigned edge_id,
    const MYON_SimpleArray< MYON_SubD_ComponentIdTypeAndTag>& sorted_tags
  );

  /*
  Parameters:
    f - [in]
    sorted_tags[] - [in]
      Array sorted by MYON_SubD_ComponentIdTypeAndTag::CompareTypeAndId().
  Returns:
    If f is in sorted_tags[], the FaceTag() from from sorted_tags[] is returned.
    Otherwise 0 is returned.
  */
  static unsigned char OriginalFaceTag(
    const class MYON_SubDFace* f,
    const MYON_SimpleArray< MYON_SubD_ComponentIdTypeAndTag>& sorted_tags
  );

  /*
  Parameters:
    face_id - [in]
    sorted_tags[] - [in]
      Array sorted by MYON_SubD_ComponentIdTypeAndTag::CompareTypeAndId().
  Returns:
    If face_id is in sorted_tags[], the FaceTag() from from sorted_tags[] is returned.
    Otherwise MYON_SubDFaceTag::Unset is returned.
  */
  static unsigned char OriginalFaceTag(
    unsigned face_id,
    const MYON_SimpleArray< MYON_SubD_ComponentIdTypeAndTag>& sorted_tags
  );

  MYON_SubDComponentPtr::Type ComponentType() const;

  MYON_SubDVertexTag VertexTag() const;

  unsigned VertexId() const;

  /*
  Returns:
    MYON_SubDEdgeTag::Unset, MYON_SubDEdgeTag::Smooth, or MYON_SubDEdgeTag::Crease.
  */
  MYON_SubDEdgeTag EdgeTag() const;

  unsigned EdgeId() const;

  unsigned char FaceTag() const;

  unsigned FaceId() const;

  const MYON_wString ToString() const;

private:
  MYON_SubDComponentPtr m_cptr = MYON_SubDComponentPtr::Null;
  unsigned m_id = 0;
  MYON_SubDComponentPtr::Type m_type = MYON_SubDComponentPtr::Type::Unset;
  unsigned char m_tag = 0;
  unsigned char m_bits = 0;
};

#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_SubD_ComponentIdTypeAndTag>;
#endif


//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDSectorId
//

class MYON_CLASS MYON_SubDSectorId
{
public:
  MYON_SubDSectorId() = default;
  ~MYON_SubDSectorId() = default;
  MYON_SubDSectorId(const MYON_SubDSectorId&) = default;
  MYON_SubDSectorId& operator=(const MYON_SubDSectorId&) = default;

public:
  // VertexId(), MinimumFaceId(), and SectorFaceCount() are all zero.
  static const MYON_SubDSectorId Zero;

  // VertexId() and MinimumFaceId() are zero. SectorFaceCount() = 0xFFFFFFFF;
  static const MYON_SubDSectorId Invalid;

public:
  /*
  Description:
    Dictionary compare of VertexId() and MinimumFaceId() in that order.
  */
  static int CompareVertexIdAndMinimumFaceId(MYON_SubDSectorId lhs, MYON_SubDSectorId rhs);

  static int CompareVertexId(MYON_SubDSectorId lhs, MYON_SubDSectorId rhs);
  static int CompareMinimumFaceId(MYON_SubDSectorId lhs, MYON_SubDSectorId rhs);

  /*
  Description:
    Dictionary compare of VertexId() and MinimumFaceId() in that order.
  */
  static int CompareVertexIdAndMinimumFaceIdFromPointers(const MYON_SubDSectorId* lhs, const MYON_SubDSectorId* rhs);

  static int CompareVertexIdFromPointers(const MYON_SubDSectorId* lhs, const MYON_SubDSectorId* rhs);
  static int CompareMinimumFaceIdFromPointers(const MYON_SubDSectorId* lhs, const MYON_SubDSectorId* rhs);

  /*
  Description:
    Dictionary compare of VertexId(), MinimumFaceId(), and SectorFaceCount() in that order.
  */
  static int Compare(MYON_SubDSectorId lhs, MYON_SubDSectorId rhs);

  /*
  Description:
    Dictionary compare of VertexId(), MinimumFaceId(), and SectorFaceCount() in that order.
  */
  static int CompareFromPointers(const MYON_SubDSectorId* lhs, const MYON_SubDSectorId* rhs);

  // No initialized construction for performance reasons.
  // If initialization is required, then use sector_id = MYON_SubDSectorId::Zero or sector_id = MYON_SubDSectorId::Create(...).

  /*
  Description:
    Create a sector id from a vertex and face in the sector. A sector id uniquely identifies
    a sector in the context of a single MYON_SubD.
  Parameters:
    vertex - [in]
    face - [in]
      A face in the sector.
      If vertex->IsSingleSectorVertex() is true, then face can be nullptr.
  Returns:
    If the vertex and face are not nullptr and the face is in a sector of the vertex,
    a nonzero zector id is returned. Otherwise MYON_SubDSectorId::Invalid is returned.
  */
  static const MYON_SubDSectorId Create(
    const class MYON_SubDVertex* vertex,
    const class MYON_SubDFace* face
  );

  /*
  Description:
    This function is used to create MYON_SubDSectorId values for searching or comparing
    to values from MYON_SubDSectorId::Create(). Use MYON_SubDSectorId::Create() for all
    other uses.
  Parameters:
    vertex - [in]
    face - [in]
      A face in the sector.
  Returns:
    A MYON_SubDSectorId with the specified values for vertex_id and minimum_face_id.
    The sector face count will be zero.
  */
  static const MYON_SubDSectorId CreateFromIds(
    unsigned int vertex_id,
    unsigned int minimum_face_id
  );

  /*
  Returns:
    The sector's center vertex id.
  Remarks:
    Either SectorId(), VertexId(), and MinimumFaceId() are all zero or are all nonzero.
  */
  const unsigned int VertexId() const;

  /*
  Returns:
    The sector's minimum face id.
  */
  const unsigned int MinimumFaceId() const;

  /*
  Returns:
    Number of faces in the sector.
  */
  const unsigned int SectorFaceCount() const;

  /*
  Returns:
    True if this sector id is zero.
  */
  bool IsZero() const;

  /*
  Returns:
    True if VertexId(), MinimumFaceId(), and SectorFaceCount() are all not zero.
  */
  bool IsSet() const;

  /*
  Parameters:
    bVerbose - [in]
      If true, the returned string begins with MYON_SubDSectorId.
  Returns:
    "Zero", "Invalid", or "vX.fYxN" where X is VertexId(), Y is MinimumFaceId(), and N is the SectorFaceCount().
  */
  const MYON_wString ToString(bool bVerbose) const;

  /*
  Parameters:
     s - [in]
       beginning of string buffer
     s_capacity
       wchar_t element capacity of the string buffer
  Returns:
    nullptr if ther is not enough room in the buffer.
    Otherwise a pointer to the null terminator of the returned string.
  Remarks
    The returned string will be "0" for a zero sector id, "X" for an invalid sector id, 
    or "vX.fYxN" where X is VertexId(), Y is MinimumFaceId(), and N is the SectorFaceCount().
  */
  wchar_t* ToString(
    wchar_t* s,
    size_t s_capacity
  ) const;

private:
  unsigned int m_vertex_id = 0;
  // minimum face id in the sector. Since a face can be in only one sector, the
  // combination of m_vertex_id and m_minimum_face_id uniquely identify a sector
  // in the context of a single MYON_SubD.
  unsigned int m_minimum_face_id = 0;
  // number of faces in the sector
  unsigned int m_sector_face_count = 0;
};

MYON_DECL
bool operator==(MYON_SubDSectorId, MYON_SubDSectorId);

MYON_DECL
bool operator!=(MYON_SubDSectorId, MYON_SubDSectorId);

MYON_DECL
bool operator>(MYON_SubDSectorId, MYON_SubDSectorId);

MYON_DECL
bool operator<(MYON_SubDSectorId, MYON_SubDSectorId);

MYON_DECL
bool operator>=(MYON_SubDSectorId, MYON_SubDSectorId);

MYON_DECL
bool operator<=(MYON_SubDSectorId, MYON_SubDSectorId);

class MYON_CLASS MYON_SubDVertexSurfacePointCoefficient
{
public: 
  MYON_SubDVertexSurfacePointCoefficient() = default;
  ~MYON_SubDVertexSurfacePointCoefficient() = default;
  MYON_SubDVertexSurfacePointCoefficient(const MYON_SubDVertexSurfacePointCoefficient&) = default;
  MYON_SubDVertexSurfacePointCoefficient& operator=(const MYON_SubDVertexSurfacePointCoefficient&) = default;

public:

  // MYON_SubDVertexSurfacePointCoefficient::Zero.m_c = 0.0
  static const MYON_SubDVertexSurfacePointCoefficient Zero;

  // MYON_SubDVertexSurfacePointCoefficient::Nan.m_c = MYON_DBL_QNAN
  static const MYON_SubDVertexSurfacePointCoefficient Nan;

  // MYON_SubDVertexSurfacePointCoefficient::Unset.m_c = MYON_UNSET_VALUE
  static const MYON_SubDVertexSurfacePointCoefficient Unset;

  static const MYON_SubDVertexSurfacePointCoefficient Create(
    const MYON_SubDVertex* limit_point_vertex,
    const MYON_SubDVertex* ring_vertex,
    double x
  );

public: 
  static int CompareSurfacePointVertexId(
    const MYON_SubDVertexSurfacePointCoefficient* lhs,
    const MYON_SubDVertexSurfacePointCoefficient* rhs
  );

  static int CompareRingVertexId(
    const MYON_SubDVertexSurfacePointCoefficient* lhs,
    const MYON_SubDVertexSurfacePointCoefficient* rhs
  );

  static int CompareSurfacePointAndRingVertexId(
    const MYON_SubDVertexSurfacePointCoefficient* lhs,
    const MYON_SubDVertexSurfacePointCoefficient* rhs
  );

  static int CompareRingAndSurfacePointVertexId(
    const MYON_SubDVertexSurfacePointCoefficient* lhs,
    const MYON_SubDVertexSurfacePointCoefficient* rhs
  );


public:
  const MYON_SubDVertex* m_limit_point_vertex = nullptr;
  const MYON_SubDVertex* m_ring_vertex = nullptr;
  // The limit point of m_limit_point_vertex
  // = sum of m_c*m_ring_vertex->ControlNetPoint()
  // for every point in the ring of m_limit_point_vertex, including m_limit_point_vertex.
  double m_c = 0.0;

public:
  unsigned int SurfacePointVertexId() const;
  unsigned int RingVertexId() const;
  double Coefficient() const;
};

#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_SubDVertexSurfacePointCoefficient>;
#endif

class MYON_CLASS MYON_SubDComponentRegionIndex
{
public:
  MYON_SubDComponentRegionIndex() = default;
  ~MYON_SubDComponentRegionIndex() = default;
  MYON_SubDComponentRegionIndex(const MYON_SubDComponentRegionIndex&) = default;
  MYON_SubDComponentRegionIndex& operator=(const MYON_SubDComponentRegionIndex&) = default;

public:
  enum : unsigned short
  {
    /// Capacity of the m_index[] array;
    IndexCapacity = 9
  };

  // All values are zero
  static const MYON_SubDComponentRegionIndex Zero;

  // All values are 0xFFFF
  static const MYON_SubDComponentRegionIndex Unset;

  /*
  Description:
    Compares subdivision counts. If the counts are the same, compares m_indices[].    
  */
  static int Compare(
    const MYON_SubDComponentRegionIndex* lhs,
    const MYON_SubDComponentRegionIndex* rhs
  );

  /*
  Description:
    Compares subdivision indices for minimum(lhs->m_subdivision_count,rhs->m_subdivision_count).
  */
  static int CompareMinimumSubregion(
    const MYON_SubDComponentRegionIndex* lhs,
    const MYON_SubDComponentRegionIndex* rhs
  );

  unsigned short Index(
    unsigned short i
  ) const;

  unsigned short m_subdivision_count = 0;

  // If m_subdivision_count > 0, then m_index[0], ..., m_index[m_subdivision_count-1]
  // identifies a subregion of the level 0 component.
  //
  // Faces with quad subdivision:
  //   m_index[n] is the subdivision quad for the region that contains
  //   the parent face's corner at face->m_vertex[m_index[n]].
  // Edges
  //   m_region_index[n] = 0 indicates the beginning half of the parent edge.
  //                       (begins at edge->Vertex(0))
  //   m_region_index[n] = 1 indicates the ending half of the parent edge.
  //                       (ends at edge->Vertex(1))
  //
  // When a component is created during a subdivision step, the value 0xFFFF
  // is used to mark the non-existent regions at earlier subdivision levels.
  // For example, if a level 1 edge is created by connecting 
  // a level0 edge subdivision point (middle-ish of the edge)
  // to a level0 face subdivision point (center-ish of the face),
  // then the level 1 edge would have 
  //   m_level0_component = MYON_SubDComponentPtr::CreateNull(MYON_SubDComponentPtr::Type::Edge, bReversed),
  //     (m_level0_component.IsNull() will be true)
  //   m_level0_component_id = MYON_SubDComponentRegion::NewTransientId()
  //   m_subdivision_count = 1,
  //   m_region_index[0] = 0xFFFF.
  // 
  unsigned short m_index[MYON_SubDComponentRegionIndex::IndexCapacity] = {};

  void Push(
    unsigned int region_index
  );
  
  void Pop();

  /*
  Description:
    Get a string of the form .a.b.c .a.b.c = m_index[] values.
  */
  wchar_t* ToString(
    wchar_t* s,
    size_t s_capacity
  ) const;

  const MYON_wString ToString() const;

  /*
  Description:
    Encodes MYON_SubDComponentRegionIndex information in 32 bits.
    (m_subdivision_count) << 24
    | (0x00FF0000 & ((m_region_index[0]) << 16))
    | (m_region_index[1] & 0x0003) << (14)
    | (m_region_index[2] & 0x0003) << (12)
    ...
    | (m_index[m_subdivision_count] & 0x0003) <<(16-(2*m_subdivision_count))
  Remarks:
    This is useful when quick compare and sorting of regions is required,
    m_subdivision_count < 256, m_index[0] < 256, m_index[1] < 4, ..., m_index[m_subdivision_count] < 4
    Regions of N-gons with N < 256 and regions of edges
    satisfy these condition when m_subdivision_count < 256 
    (which is always in real world situations).
  */
  MYON__UINT32 ToCompressedRegionIndex() const;

  static const MYON_SubDComponentRegionIndex FromCompressedRegionIndex(
    MYON__UINT32 compressed_region_index
  );

  static MYON__UINT32 ToCompressedRegionIndex(
    unsigned short subdivision_count,
    const unsigned short* region_index
  );

  /*
  Description:
    Decompress a 32-bit region.
  Parameters:
    region32 - [in]
      Value returned from To32BitRegion().
    subdivision_count - [out]
      Subdivision count
    region_index[] - out
      Region indices. The region_index[] array must have a capcity of at 
      least MYON_SubDComponentRegion::region_index_capacity elements.
  */
  static void FromCompressedRegionIndex(
    MYON__UINT32 compressed_region_index,
    unsigned short* subdivision_count,
    unsigned short* region_index
  );
};

class MYON_CLASS MYON_SubDComponentRegion
{
public:
  MYON_SubDComponentRegion() = default;
  ~MYON_SubDComponentRegion() = default;
  MYON_SubDComponentRegion(const MYON_SubDComponentRegion&) = default;
  MYON_SubDComponentRegion& operator=(const MYON_SubDComponentRegion&) = default;

public:
  static const MYON_SubDComponentRegion Create(
    const class MYON_SubDFace* level0_face
  );

  static const MYON_SubDComponentRegion Create(
    unsigned int component_id,
    MYON_SubDComponentPtr::Type component_type,
    bool bComponentMark
  );

  /*
  Description:
    Creates a region that can be used to identify a component
    created at a certain level of subdivision that does not
    come from dividing a component from the previous level.
    For example, Catmull Clark subdivision edges on level N+1
    that run from the level N edge subdivision point to the 
    level N face subdivision point.

    m_level0_component = MYON_SubDComponentPtr::CreateNull(component_type, bComponentDirection?1:0),
       (m_level0_component.IsNull() will be true)
    m_level0_component_id = MYON_SubDComponentRegion::NewTransientId()
    m_subdivision_count = subdivision_count,
     m_region_index[0, ..., (subdivision_count-1)] = 0xFFFF.
   
  */
  static const MYON_SubDComponentRegion CreateSubdivisionRegion(
    MYON_SubDComponentPtr::Type component_type,
    bool bComponentDirection,
    unsigned short subdivision_count,
    bool bAssignTransientId
  );

public:
  static const MYON_SubDComponentRegion Empty;

public:
  MYON_SubDComponentPtr m_level0_component = MYON_SubDComponentPtr::Null;

  unsigned int m_level0_component_id = 0;

  unsigned short SubdivisionCount() const;

  MYON_SubDComponentRegionIndex m_region_index;

  /*
  Returns:
    True if m_level0_component_id is a transient id.
  */
  bool IsTransientId() const;

  /*
  Returns:
    True if m_level0_component_id is the id of a persistent MYON_SubD level 0 component.
  */
  bool IsPersistentId() const;

  /*
  Description:
    Compares 
    m_level0_component.ComponentType(), 
    m_level0_component_id,
    m_level0_component.ComponentDirection(),
    the entire sub region,
    and m_level0_component.m_ptr.
  */
  static int Compare(
    const MYON_SubDComponentRegion* lhs,
    const MYON_SubDComponentRegion* rhs
    );

  /*
  Descriptions:
    Compares 
    m_level0_component.ComponentType(), 
    m_level0_component_id,
    m_level0_component.ComponentDirection().
  */
  static int CompareTypeIdDirection(
    const MYON_SubDComponentRegion* lhs,
    const MYON_SubDComponentRegion* rhs
    );

  /*
  Description:
    Compares 
    m_level0_component.ComponentType(), 
    m_level0_component_id,
    m_level0_component.ComponentDirection(),
    and the m_region_index[] values for the 
    minimum subdivision count lhs and rhs.    
  */
  static int CompareTypeIdDirectionMinimumSubregion(
    const MYON_SubDComponentRegion* lhs,
    const MYON_SubDComponentRegion* rhs
    );
  
  /*
  Description:
    Compares 
    m_level0_component.ComponentType(), 
    m_level0_component_id,
    m_level0_component.ComponentDirection(),
    and the entire sub region.
  */
  static int CompareTypeIdDirectionSubregion(
    const MYON_SubDComponentRegion* lhs,
    const MYON_SubDComponentRegion* rhs
    );

  void SetLevel0Component(
    MYON_SubDComponentPtr component_ptr
  );

  void SetLevel0Face(
    const MYON_SubDFace* face
  );

  void SetLevel0EdgePtr(
    const MYON_SubDEdgePtr edge_ptr
  );

  void SetLevel0Vertex(
    const MYON_SubDVertex* vertex
  );

  /*
  Description:
    region_index - [in]
      If m_level0_component identifies an edge, region_index is 0 or 1,
      and the edge is reversed (1=m_level0_component.ComponentMark()),
      then PushAbsolute(1-region_index) is called.
      In every other case, PushAbsolute(region_index) is called.
  */
  void PushAdjusted(
    unsigned int region_index
  );

  /*
  Parameters:
    region_index - [in]
      If m_level0_component identifies a face, then region_index is the index of the
      corner vertex for the subdivision quad.
      If m_level0_component identifies an edge, then region_index must be 0 or 1.
  Description:
    Increments if m_subdivision_count and appends region_index to m_region_index[]
    (m_region_index[m_subdivision_count++] = region_index)
  */
  void PushAbsolute(
    unsigned int region_index
  );

  /*
  Description:
    Get a string of the form fN.a.b.c where N = m_level0_face-m_id, a.b.c = m_region_index[] values.
  */
  wchar_t* ToString(
    wchar_t* s,
    size_t s_capacity
  ) const;

  const MYON_wString ToString() const;

  void Pop();

  bool IsEmptyRegion() const;

  enum : unsigned int
  {
    TransientIdBit = 0x80000000U
  };


  /*
  Returns:
    A value that can be used to identify transient subdivision components that do not
    exist in the persistent levels of a SubD.
    Transient ids always satisfy (MYON_SubDComponentRegion::TransientIdBit & transient_id) is not zero and 
    (~MYON_SubDComponentRegion::TransientIdBit & transient_id) is not zero.
  Remarks:
    Transient ids are used to identify subdivision components at levels that do
    not persist in the MYON_SubD. They are unique within the context where they are
    being used. They generally vary with each repetition of a calcultion in that
    context.
  */
  static const unsigned int NewTransientId();

  /*
  Description:
    Resets the value used to generate transient ids.
    This is useful during debugging session so that transient id
    values are predictable. Otherwise, use of this function 
    should be avoided.
  */
  static void ResetTransientId();

  /*
  Parameters:
    id - [in]
      Value to test to see if it is a transient subd component id.
  Returns:
    True if (MYON_SubDComponentRegion::TransientIdBit & id) is not zero and 
    (~MYON_SubDComponentRegion::TransientIdBit & id) is not zero.
  Remarks:
    Transient ids are used to identify subdivision components at levels that do
    not persist in the MYON_SubD. They are unique within the context where they are
    being used. They generally vary with each repetition of a calcultion in that
    context.
  */
  static bool IsTransientId(unsigned int id);

  /*
  Parameters:
    id - [in]
      Value to test to see if it is a transient subd component id.
  Returns:
    If the id is a transient id, then its id value is returned.
    Otherwise, 0 is returned.
  Remarks:
    Transient ids are used to identify subdivision components at levels that do
    not persist in the MYON_SubD. They are unique within the context where they are
    being used. They generally vary with each repetition of a calcultion in that
    context.
  */
  static unsigned int TransientId(unsigned int id);

  /*
  Parameters:
    id - [in]
      Value to test to see if it is a persitsent subd component id.
  Returns:
    True if (MYON_SubDComponentRegion::TransientIdBit & id) is not zero and 
    (~MYON_SubDComponentRegion::TransientIdBit & id) is not zero.
  Remarks:
    Transient ids are used to identify subdivision components at levels that do
    not persist in the MYON_SubD. They are unique within the context where they are
    being used. They generally vary with each repetition of a calcultion in that
    context.
  */
  static bool IsPersistentId(unsigned int id);
};

class MYON_CLASS MYON_SubDFaceRegion
{
public:
  MYON_SubDFaceRegion() = default;
  ~MYON_SubDFaceRegion() = default;
  MYON_SubDFaceRegion(const MYON_SubDFaceRegion&) = default;
  MYON_SubDFaceRegion& operator=(const MYON_SubDFaceRegion&) = default;

  static const MYON_SubDFaceRegion Empty;

public:
  // Identifies a region of an MYON_SubDFace
  MYON_SubDComponentRegion m_face_region;

  const MYON_SubDFace* Level0Face() const;

  // When the face region is a quad, m_edge_region[4] identifies regions of MYON_SubDEdge elements.
  // When the face region is a sub-quad, these edges may be null or have null MYON_SubDEdge pointers 
  // and the ids will be zero or MYON_SubDComponentRegion::IsTransientId() will be true.
  // When MYON_SubDComponentRegion::IsTransientId() is true, the id does not identify
  // a persistent edge in the MYON_SubD.
  MYON_SubDComponentRegion m_edge_region[4];

  unsigned int m_level0_edge_count = 0;

private:
  unsigned int m_reserved = 0;

public:
  /*
  Returns:
    If vertex_id > 0 and there is a unique element of m_vertex_id[] with the same value,
    the index of that element is returned (0,1,2 or 3).
    Otherwise MYON_UNSET_UNINT_INDEX is returned.
  */
  unsigned int CornerIndexFromVertexId(
    unsigned int vertex_id
  ) const;


  // If set, these are the vertice ids at the region's limit surface corners.
  // m_vertex_id[] is mutable because these values appear during recursive calculations.
  // When the face region is a sub-quad, these ids will be zero or MYON_SubDComponentRegion::IsTransientId()
  // will be true. 
  // When MYON_SubDComponentRegion::IsTransientId() is true, the id does not identify
  // a persistent vertex in the MYON_SubD.
  mutable unsigned int m_vertex_id[4] = {};

  // When a vertex is exceptional, a NURBS conversion is typically an approximation
  // of the SubD around the exceptional vertex. There are a variety of post processes
  // that can be applied in this case and the processes need to be applied 
  // sector by sector. 
  // Note well that when a level zero face is an N-gon with N != 4,
  // the face subdivision point is an exceptional smooth vertex with valence = N.
  // In this case the corresponding m_vertex_id[] value will be zero.
  mutable MYON_SubDSectorId m_sector_id[4];

public:
  void Push(unsigned int quadrant_index);

  bool IsValid(
    bool bSilentError
  ) const;

  wchar_t* ToString(
    wchar_t* s,
    size_t s_capacity
  ) const;

  const MYON_wString ToString() const;
};


class MYON_CLASS MYON_SubDFaceRegionAndNurbs
{
public:
  MYON_SubDFaceRegionAndNurbs() = default;
  ~MYON_SubDFaceRegionAndNurbs() = default;
  MYON_SubDFaceRegionAndNurbs(const MYON_SubDFaceRegionAndNurbs&) = default;
  MYON_SubDFaceRegionAndNurbs& operator=(const MYON_SubDFaceRegionAndNurbs&) = default;

  static const MYON_SubDFaceRegionAndNurbs Empty;

public:
  MYON_SubDFaceRegion m_face_region;
  // This pointer is not managed by MYON_SubDFaceRegionAndNurbs
  class MYON_NurbsSurface* m_nurbs_surface = nullptr;
};


//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDFromMeshParameters
//
class MYON_CLASS MYON_SubDFromSurfaceParameters
{
public:

  // Default construction is identical to MYON_SubDFromMeshParameters::Smooth.
  MYON_SubDFromSurfaceParameters() = default;
  ~MYON_SubDFromSurfaceParameters() = default;
  MYON_SubDFromSurfaceParameters(const MYON_SubDFromSurfaceParameters&) = default;
  MYON_SubDFromSurfaceParameters& operator=(const MYON_SubDFromSurfaceParameters&) = default;

  static const MYON_SubDFromSurfaceParameters Default;
  static const MYON_SubDFromSurfaceParameters DefaultWithCorners;
  static const MYON_SubDFromSurfaceParameters ControlNet;
  static const MYON_SubDFromSurfaceParameters ControlNetWithCorners;

#pragma region RH_C_SHARED_ENUM [MYON_SubDFromSurfaceParameters::Methods] [Rhino.Geometry.SubDFromSurfaceMethods] [byte]
  /// <summary>
  /// MYON_SubDFromSurfaceParameters::Method are ways to create a SubD from a surface.
  /// </summary>
  enum class Methods : unsigned char
  {
    /// <summary>
    /// Used to indicate the method is not set.
    /// </summary>
    Unset = 0,

    /// <summary>
    /// The surface is approximated with a SubD friendly NURBS surface and the SubD is created
    /// to match the subd friendly nurbs surface. 
    /// If the input surface is a subd friendly NURBS surface, the subd and surface have the same geometry.
    /// </summary>
    SubDFriendlyFit = 1,

    /// <summary>
    /// The surface is converted to a NURBS surface and then a subd with one face per NURBS bispan 
    /// is created by using an appropriate subset of the NURBS surface control net.
    /// If the input surface is a subd friendly NURBS surface, the subd and surface have the same geometry.
    /// </summary>
    FromNurbsControlNet = 2
  };
#pragma endregion

  MYON_SubDFromSurfaceParameters::Methods Method() const;

  void SetMethod(
    MYON_SubDFromSurfaceParameters::Methods method
    );

  bool Corners() const;

  void SetCorners(
    bool bCorners
    );

private:
  MYON_SubDFromSurfaceParameters::Methods m_method = MYON_SubDFromSurfaceParameters::Methods::SubDFriendlyFit;
  bool m_bCorners = false;
  unsigned short m_reserved1 = 0;
  unsigned int m_reserved2 = 0;
  double m_reserved3 = 0.0;
  double m_reserved4 = 0.0;
};


#pragma region RH_C_SHARED_ENUM [MYON_SubDEndCapStyle] [Rhino.Geometry.SubDEndCapStyle] [byte]
/// <summary>
/// MYON_SubDEndCapStyle enumerates the type of end caps functions like MYON_SubD::CreateCylinder will create.
/// Use MYON_SubDEndCapStyleFromUnsigned(integer value) to convert integer values to an MYON_SubDEndCapStyle.
/// Use MYON_SubDEndCapStyleToString(end_cap_style) to convert MYON_SubDEndCapStyle values to string descriptions.
/// </summary>
enum class MYON_SubDEndCapStyle : unsigned char
{
  /// <summary>
  /// Used to indicate the end cap style is not set.
  /// </summary>
  Unset = 0,

  /// <summary>
  /// Ends are not capped.
  /// </summary>
  None = 1,

  /// <summary>
  /// Ends are capped with triangles.
  /// </summary>
  Triangles = 2,

  /// <summary>
  /// When the end has an even number of edges, is will be capped with quads. Otherwise it will be capped with triangles.
  /// </summary>
  Quads = 3,

  /// <summary>
  /// Ends are capped with a n-gon. This is a poor choice when there are a large number of edges in the end boundary.
  /// </summary>
  Ngon = 4
};
#pragma endregion


/// <summary>
/// Convert an integer value to an MYON_SubDEndCapStyle enum value.
/// </summary>
/// <param name="subd_cap_style_as_unsigned"></param>
/// If subd_cap_style_as_unsigned is not valid, then MYON_SubDEndCapStyle::Unset is returned.
/// <returns></returns>
MYON_DECL
MYON_SubDEndCapStyle MYON_SubDEndCapStyleFromUnsigned(
  unsigned int subd_cap_style_as_unsigned
);

/// <summary>
/// Convert subd_cap_style to a string description.
/// </summary>
/// <param name="subd_cap_style"></param>
/// <returns></returns>
MYON_DECL
const MYON_wString MYON_SubDEndCapStyleToString(
  MYON_SubDEndCapStyle subd_cap_style
);

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubD
//
//// MYON_WIP_SDK 
//// This class is in the WIP SDK but the [[deprecated]] tag is 
//// failing to compile in tl_precompiledheader.cpp
class MYON_CLASS MYON_SubDExpandEdgesParameters
{
public:
  MYON_SubDExpandEdgesParameters() = default;
  ~MYON_SubDExpandEdgesParameters() = default;
  MYON_SubDExpandEdgesParameters(const MYON_SubDExpandEdgesParameters&) = default;
  MYON_SubDExpandEdgesParameters& operator=(const MYON_SubDExpandEdgesParameters&) = default;

  static const MYON_SubDExpandEdgesParameters Default;

  /// <summary>
  /// MYON_SubDExpandEdgesParameters::Style specifies options for how faces are inserted along input edges.
  /// </summary>
  enum class Style : unsigned char
  {
    /// <summary>
    /// Indicates the variable has not be initialized.
    /// </summary>
    Unset = 0,

    /// <summary>
    /// One quad replaces each input manifold edge. Corner cases where three or more edges meet
    /// receive special handling.
    /// </summary>
    Single = 1,

    /// <summary>
    /// Two quads are added for each input manifold edge. Corner cases where three or more edges meet
    /// receive special handling.
    /// </summary>
    Double = 2,

    /// <summary>
    /// This option applies only when the the input is an array of MYON_SubDEdgePtrs
    /// that form a single oriented edge chain of manifold interior edges.
    /// A single quad is added to the left of the input edges.
    /// (The left side of of an oriented interior manifold edge is the face
    /// whose natural boundary orientation is the same as with the MYON_SubDEdgePtr direction.)
    /// </summary>
    HalfLeft = 3,

    /// <summary>
    /// This option applies only when the the input is an array of MYON_SubDEdgePtrs
    /// that form a single oriented edge chain of manifold interior edges.
    /// A single quad is added to the right of the input edges.
    /// (The right side of of an oriented interior manifold edge is the face
    /// whose natural boundary orientation is opposite the MYON_SubDEdgePtr direction.)
    /// </summary>
    HalfRight = 4,
  };

public:
  /// OffsetTolerance = 0.001
  static const double OffsetTolerance; 

  /// Minimum permitted offset value (0.05)
  static const double MinimumOffset;

  /// Maximum permitted offset value (0.95)
  static const double MaximumOffset;

  /// Small offset value (0.125)
  static const double SmallOffset;

  /// Medium offset value (0.25)
  static const double MediumOffset;

  /// Large offset value (0.5)
  static const double LargeOffset;

public:
  /// <summary>
  /// This SHA1 hash can be used to determine if sets of parameters are the same.
  /// </summary>
  /// <returns>A SHA1 hash of all parameter values.</returns>
  const MYON_SHA1_Hash Hash() const;

public:
  static double CleanupOffset(double x);


  /// <summary>
  /// Normalized constant offset parameter for inserting edges parallel to an input edge.
  /// Smaller values create narrower offsets.
  /// </summary>
  /// <returns>Offset value in the range MYON_SubDExpandEdgesParameters::MinimumOffset to MYON_SubDExpandEdgesParameters::MaximumOffset.</returns>
  double ConstantOffset() const;

  /// <summary>
  /// Set the constant offset value. Values within OffsetTolerance of a 
  /// predefined offset value are set to the predefined offset value.
  /// </summary>
  /// <param name="offset">
  /// MYON_SubDExpandEdgesParameters::MinimumOffset &lt;= offset &lt;= MYON_SubDExpandEdgesParameters::MaximumOffset;
  /// </param>
  /// <returns></returns>
  void SetConstantOffset(double offset);


  /// <summary>
  /// Determine if the set of MYON_SubDEdgePtrs can be sorted into a 
  /// single edge chain that supports half side offsets.
  /// </summary>
  /// <param name="edge_chain"></param>
  /// <returns>True if variable offsets can be applied to edge_chain.</returns>
  static bool IsValidForHalfOffset(
    const MYON_SimpleArray<MYON_SubDEdgePtr>& edges
  );

  /// <summary>
  /// Determine if the set of MYON_SubDEdgePtrs can be sorted into a 
  /// single open edge chain supports variable offsets.
  /// </summary>
  /// <param name="edge_chain"></param>
  /// <returns>True if variable offsets can be applied to edge_chain.</returns>
  static bool IsValidForVariableOffset(
    const MYON_SimpleArray<MYON_SubDEdgePtr>& edges
  );

  /// This option applies only when the the input is an array of MYON_SubDEdgePtrs
  /// that form a single oriented edge chain. You may use
  /// MYON_SubDExpandEdgesParameters::IsValidForVariableOffset() to determine if an
  /// array of MYON_SubDEdgePtrs meets the variable offset requirements.
  /// In all other cases, the constant Offset() is used.
  /// To apply variable offsets to several edge chains, expand them one at a time.
  const MYON_Interval VariableOffset() const;

  /*
  Description:
    This option applies only when the the input is an array of MYON_SubDEdgePtrs
    that form a single oriented edge chain. You may use
    MYON_SubDExpandEdgesParameters::IsValidForVariableOffset() to determine if an
    array of MYON_SubDEdgePtrs meets the variable offset requirements.
    In all other cases, the constant Offset() is used.
    To apply variable offsets to several edge chains, expand them one at a time.

  Parameters:
    variable_offsets - [in]
      The two values must be between 0 and 1 and differ by more than MYON_SubDExpandEdgesParameters::OffsetTolerance.
  */
  void SetVariableOffset(MYON_Interval variable_offsets);

  void ClearVariableOffset();

  static bool IsValidConstantOffset(
    double constant_offset_candidate
  );

  static bool IsValidVariableOffset(
    MYON_Interval variable_offset_candidate
  );

  /*
  Returns:
    True if variable offsets are set.
  */
  bool IsVariableOffset() const;


  MYON_SubDExpandEdgesParameters::Style FaceStyle() const;

  /// <summary>
  /// Set the style for new faces.
  /// </summary>
  /// <param name="face_style"></param>
  /// <returns>Style for new faces.</returns>
  void SetFaceStyle(MYON_SubDExpandEdgesParameters::Style face_style);

  /// <returns>True if the FaceStyle() is HalfLeft of HalfRight.</returns>
  bool IsHalfFaceStyle() const;

  /// <returns>Per face color for new faces.</returns>
  const MYON_Color FaceColor() const;

  /// <summary>
  /// Set the perf face color for new faces.
  /// </summary>
  /// <param name="face_color">Color for new face. Pass MYON_Color::Unset for no per face color.</param>
  void SetFaceColor(MYON_Color face_color);

  /// <returns>Status for new faces.</returns>
  const MYON_ComponentStatus FaceStatus() const;

  /// <summary>
  /// Set the Mark(), MarkBits(), Selected(), and Highlighted() status for new faces.
  /// </summary>
  /// <param name="face_status">Status for new faces.</param>
  void SetFaceStatus(MYON_ComponentStatus face_status);

private:
  double m_constant_offset = MYON_SubDExpandEdgesParameters::MediumOffset;
  MYON_Interval m_variable_offsets = MYON_Interval::Nan;
  MYON_Color m_face_color = MYON_Color::UnsetColor;
  MYON_ComponentStatus m_face_status;
  MYON_SubDExpandEdgesParameters::Style m_face_style = MYON_SubDExpandEdgesParameters::Style::Double;

private:
  unsigned char m_reserved1 = 0;

private:
  MYON__UINT64 m_reserved2 = 0;
  MYON__UINT64 m_reserved3 = 0;
};

bool operator==(const MYON_SubDExpandEdgesParameters& lhs, const MYON_SubDExpandEdgesParameters& rhs);
bool operator!=(const MYON_SubDExpandEdgesParameters& lhs, const MYON_SubDExpandEdgesParameters& rhs);


//////////////////////////////////////////////////////////////////////////
//
// MYON_SubD
//
class MYON_CLASS MYON_SubD : public MYON_Geometry
{
  MYON_OBJECT_DECLARE(MYON_SubD);

public:
  static const MYON_SubD Empty;

  enum : unsigned int
  {
    maximum_subd_level = 128        // uses as a sanity check on input parameters
  };

  /*
  Returns:
    A runtime serial number identifying this subd.
  Remarks:
    MYON_SubD is a shared pointer to an implementation. As such, there can
    be multiple MYON_SubD instances that reference the same implementation.
    The runtime serial number uniquely identifies a particular instance
    of an implementation.
    The empty subd has runtime serial number = 0.
  */
  MYON__UINT64 RuntimeSerialNumber() const;

  /*
  Returns:
    A runtime serial number that is changed every time a the active level,
    vertex location, vertex or edge flag, or subd topology is changed.
  */
  MYON__UINT64 GeometryContentSerialNumber() const;

  /*
  Returns:
    A runtime serial number that is incremented every time a component status
    (locked, hidden, selected, highlighted, ...) is changed.
  */
  MYON__UINT64 ComponentStatusSerialNumber() const;

  /*
  Description:
    Change the geometry content serial number to indicate something affecting
    the geometric shape of the subd has changed. This includes topologial changes,
    vertex and edge tag changes, and changes to vertex control net locations.
  Parameters:
    bChangePreservesSymmetry - [in]
      When in doubt, pass false.
      If the change preserves global symmtery, then pass true.
      (For example, a global subdivide preserves all types of symmetry.)
      Note well:
        Transformations do not preserve symmetries that are
        set with respect to world coordinate systems.
  Returns:
    The new value of GeometryConentSerialNumber().
  Remarks:
    The value can change by any amount.
    Changing the geometry content serial number automatically changes
    the render content serial number.
  */
  MYON__UINT64 ChangeGeometryContentSerialNumberForExperts(
    bool bChangePreservesSymmetry
  );

  /*
  Description:
    The render content serial number changes whenever a change the might effect
    rendered appearance changes. This includes both geometry changes and
    changes that affect rendered appeance including changes to per face colors,
    per face materials, texture coordinates, and texture mappings.
  */
  MYON__UINT64 RenderContentSerialNumber() const;

  /*
  Description:
    Change the render content serial number to indicate something affecting
    only rendered appearance has changed. This includes changes to per face colors,
    per face materials, texture coordinates, and texture mappings.
  Remarks:
    Changing the geometry content serial number automatically changes
    the render content serial number. If you call ChangeGeometryContentSerialNumberForExperts(),
    there is no need to also call ChangeRenderContentSerialNumber().
  */
  MYON__UINT64 ChangeRenderContentSerialNumber() const;


  /*
  Description:
    Get a hash that uniquely identifies the topology or geometry of this SubD.
  Parameters:
    hash_type - [in]
     To see what is included in the various hashes, read the documentation for the MYON_SubDHashType enum.
   bForceUpdate - [in]
     When in doubt pass false.
     The SubD hashes are mutable and cached. When code properly manages GeometryContentSerialNumber(),
     then SubDHash(hash_type,false) will always return the correct answer. This is the expected behavior.
     If code directly modifies SubD components and fails to call ChangeGeometryContentSerialNumberForExperts(),
     then it is possible a stale hash will be returned. Setting bForceUpdate = true forces the SHA1
     values to be recalculated from scratch. For extremely large SubDs, this recalculation can be time consuming.
  */
  const MYON_SubDHash SubDHash(
    MYON_SubDHashType hash_type,
    bool bForceUpdate
  ) const;

  /*
  Description:
    If two SubDs have identical values of GeometryHash(), then they have
    identical surface geometry.
  Returns:
    this->SubDHash(MYON_SubDHashType::Geometry,false).SubDHash().
  Remarks:
    If the geometry hashes are equal, the topology hashes are equal.
  */
  const MYON_SHA1_Hash GeometryHash() const;

  /*
  Description:
    If two SubDs have identical values of TopologyHash(), then they have
    identical labeled control net topology. The labels are the vertex, edge,
    and face ids.
  Returns:
    this->SubDHash(MYON_SubDHashType::Topology,false).SubDHash().
  Remarks:
    Two SubDs can have the same topology hash but different geometry hashes.
  */
  const MYON_SHA1_Hash TopologyHash() const;

  /*
  Description:
    Get the SubD appearance (surface or control net);
  Returns:
    MYON_SubDComponentLocation::Surface or MYON_SubDComponentLocation::ControlNet.
  */
  MYON_SubDComponentLocation SubDAppearance() const;

  /*
  Description:
    Set the SubD appearance (surface or control net).
  Parameters:
    subd_appearance - [in]
      MYON_SubDComponentLocation::Surface or MYON_SubDComponentLocation::ControlNet.
  Remarks:
    This makes no changes to the information that defines the SubD.
    It does not require regeneration of the MYON_SubDMeshFragments.
    Application display will need to be updated.
  */
  void SetSubDAppearance(MYON_SubDComponentLocation subd_appearance) const;

  /*
  Description:
    Get the SubD appearance (surface or control net);
  Returns:
    MYON_SubDComponentLocation::Surface or MYON_SubDComponentLocation::ControlNet.
  */
  static MYON_SubDComponentLocation ToggleSubDAppearanceValue(MYON_SubDComponentLocation subd_appearance);
  static const MYON_SubDComponentLocation DefaultSubDAppearance; // = MYON_SubDComponentLocation::Surface
  static const MYON_SubDTextureCoordinateType DefaultTextureCoordinateType; // = MYON_SubDTextureCoordinateType::Packed


public:
  static MYON_SubD* CreateCylinder(
    const class MYON_Cylinder& cylinder,
    unsigned circumference_face_count,
    unsigned height_face_count,
    MYON_SubDEndCapStyle end_cap_style,
    MYON_SubDEdgeTag end_cap_edge_tag,
    MYON_SubDComponentLocation radius_location,
    MYON_SubD* destination_subd
  );

public:
  static MYON_SubDVertexTag VertexTagFromUnsigned( 
    unsigned int vertex_tag_as_unsigned
    );


  /*
  Parameters:
    vertex_tag - [in]
    bVerbose - [in]
      If verbose, the tag name is preceded with "MYON_SubDVertexTag::".
  Returns:
    vertex_tag as a string.
  */
  static const MYON_wString VertexTagToString(
    MYON_SubDVertexTag vertex_tag,
    bool bVertose
  );

  /*
  Parameters:
    vertex_tag - [in]
  Returns:
    True if vertex_tag is Smooth, Crease, Corner, or Dart.
    False otherwise.
  */
  static bool VertexTagIsSet(
    MYON_SubDVertexTag vertex_tag
  );

  static MYON_SubDEdgeTag EdgeTagFromUnsigned( 
    unsigned int edge_tag_as_unsigned
    );


  /*
  Parameters:
    edge_tag - [in]
    bVerbose - [in]
      If verbose, the tag name is preceded with "MYON_SubDEdgeTag::".
  Returns:
    edge_tag as a string.
  */
  static const MYON_wString EdgeTagToString(
    MYON_SubDEdgeTag edge_tag,
    bool bVertose
  );


  /*
  Parameters:
    edge_tag - [in]
  Returns:
    True if edge_tag is Smooth, Crease, or SmoothX.
    False otherwise.
  */
  static bool EdgeTagIsSet(
    MYON_SubDEdgeTag edge_tag
  );

  
  //enum class VertexEdgeOrder : unsigned char
  //{
  //  unset = 0,
  //  radial, // The MYON_SubDVertex edge and face information satisfies:
  //          // 1) m_face_count = m_edge_count or m_face_count+1 == m_edge_count
  //          // 2) m_faces[i] is between m_edges[i] and m_edges[(i+1)%m_edge_count]
  //          // 3) When 0 < i < m_edge_count-1, m_edges[i].m_edge_count = 2
  //          //    and m_edges[i].m_face2[] references m_faces[i-1] and m_faces[i]
  //          //    in an unspecified order.
  //  notradial // one of the conditions conditions for radial is not satisfied.
  //};

  //static MYON_SubD::VertexEdgeOrder VertexEdgeOrderFromUnsigned( 
  //  unsigned int vertex_edge_order_as_unsigned
  //  );

#pragma region RH_C_SHARED_ENUM [MYON_SubD::VertexFacetType] [Rhino.Geometry.SubDVertexFacetType] [byte]
  /// <summary>
  /// Summarizes the number of edges in faces in the whole object.
  /// </summary>
  enum class VertexFacetType : unsigned char
  {
    ///<summary>Not a valid vertex face type.</summary>
    Unset = 0,

    ///<summary>All faces are triangular.</summary>
    Tri = 3,

    ///<summary>All faces are quads.</summary>
    Quad = 4,

    ///<summary>Edge count of faces is constant and &gt; 4.</summary>
    Ngon = 5,

    ///<summary>Edge count of faces is not constant.</summary>
    Mixed = 0xFF 
  };
#pragma endregion

  static MYON_SubD::VertexFacetType VertexFacetTypeFromUnsigned( 
    unsigned int vertex_facet_type_as_unsigned
    );
  
#pragma region RH_C_SHARED_ENUM [MYON_SubD::ChainType] [Rhino.Geometry.SubDChainType] [byte]
  /// <summary>
  /// SubD::ChainType specifies what edge and vertex tag tests are used when creating edge chains.
  /// </summary>
  enum class ChainType : unsigned char
  {
    ///<summary>
    /// Unset.
    ///</summary>
    Unset = 0,

    ///<summary>
    /// All types of edges and vertices can be in the chain.
    ///</summary>
    MixedTag = 1,

    ///<summary>
    /// Every edge in an edge chain has the same smooth/crease property.
    ///</summary>
    EqualEdgeTag = 2,

    ///<summary>
    /// Every edge in an edge chain has the same smooth/crease edge tag 
    /// and interior vertices have the corresponding smooth/crease vertex tag.
    ///</summary>
    EqualEdgeAndVertexTag = 3,

    ///<summary>
    /// Every edge in an edge chain has the same smooth/crease property
    /// and every edge has the same number of faces.
    /// If the edges have 1 face, then interior vertices have valence = 3.
    /// If the edges have 2 faces, then interior vertices have valence = 4.
    ///</summary>
    EqualEdgeTagAndOrdinary = 4,

    ///<summary>
    /// Every edge in an edge chain has the same smooth/crease edge tag,
    /// every edge has the same number of faces,
    /// and interior vertices have the corresponding smooth/crease vertex tag.
    /// If the edges have 1 face, then interior vertices have valence = 3.
    /// If the edges have 2 faces, then interior vertices have valence = 4.
    ///</summary>
    EqualEdgeAndVertexTagAndOrdinary = 5
  };
#pragma endregion

  /*
  Parameters:
    sit - [in]
      vertex sector iterator
    component_ring - [out]
      A sorted list of MYON_SubDComponentPtr values are returned in component_ring[]
      component_ring[0] is the central vertex.
      component_ring[1] and subsequent components with odd indices are sector edges.
      component_ring[2] and subsequent components with even indices are sector faces.
      For edge components (i is odd), component_ring[i].ComponentMark() is the index of
      the center vertex in MYON_SubDEge.m_vertex[].
    component_ring_capacity - [in]
      capacity of component_ring[] array
      1 + center_vertex.m_edge_count + center_vertex.m_face_count
      will be large enough.
  Returns:
    Number of components set in component_ring[].
    
    0: failure

    >= 4 and even:
      component_ring[0] = center vertex
      component_ring[1] = starting crease edge
      component_ring[2] = starting face
      ... zero or more interior smooth edge, face pairs ...
      component_ring[component_count-1] = ending crease edge

    >= 5 and odd:
      component_ring[0] = vit.CenterVertex()
      component_ring[1] = first edge (smooth)
      component_ring[2] = first face
      ... zero or more smooth edge, face, pairs ...
      component_ring[component_count-2] = last edge (smooth)
      component_ring[component_count-1] = last face

  Example:
    unsigned int component_ring_count = GetVertexComponentRing(vit,component_ring);
    unsigned int N = component_ring_count/2; // number of edges in ring
    const bool bSectorHasCreaseBoundary = (0 == (component_ring_count % 2));
  */
  static unsigned int GetSectorComponentRing(
    const class MYON_SubDSectorIterator& sit,
    MYON_SubDComponentPtr* component_ring,
    size_t component_ring_capacity
    );

 /*
  Parameters:
    sit - [in]
      vertex sector iterator
    component_ring - [out]
      A sorted listof MYON_SubDComponentPtr values are returned in component_ring[]



  Returns:
    Number of components set in component_ring[].
    
    0: failure

    >= 4 and even:
      component_ring[0] = vit.CenterVertex()
      component_ring[1] = starting crease edge
      component_ring[2] = starting face
      ... zero or more interior smooth edge, face pairs ...
      component_ring[component_count-1] = ending crease edge

    >= 5 and odd:
      component_ring[0] = center vertex
      component_ring[1] = first edge (smooth)
      component_ring[2] = first face
      ... zero or more smooth edge, face, pairs ...
      component_ring[component_count-2] = last edge (smooth)
      component_ring[component_count-1] = last face

  Example:
    unsigned int component_ring_count = GetVertexComponentRing(vit,component_ring);
    unsigned int N = component_ring_count/2; // number of edges in ring
    const bool bSectorHasCreaseBoundary = (0 == (component_ring_count % 2));
  */
  static unsigned int GetSectorComponentRing(
    const class MYON_SubDSectorIterator&  sit,
    MYON_SimpleArray<MYON_SubDComponentPtr>& component_ring
    );

  /*
  Returns:
    Number of edges in an component ring returned by MYON_SubD::GetVertexComponentRing();
  */
  static unsigned int ComponentRingEdgeCount(
    size_t component_ring_count
    );

  /*
  Returns:
    Number of faces in an component ring returned by MYON_SubD::GetVertexComponentRing();
  */
  static unsigned int ComponentRingFaceCount(
    size_t component_ring_count
    );

  static bool ComponentRingIsValid(
    const MYON_SubDComponentPtr* component_ring,
    size_t component_ring_count
    );

  /*
  Returns:
    Number of points in the subdivision ring or 0 if the call fails.
  */
  static unsigned int GetSectorSubdivsionPointRing(
    const MYON_SubDComponentPtr* component_ring,
    size_t component_ring_count,
    double* point_ring,
    size_t point_ring_capacity,
    size_t point_ring_stride
    );

  static unsigned int GetSectorSubdivisionPointRing(
    const MYON_SubDComponentPtr* component_ring,
    size_t component_ring_count,
    MYON_SimpleArray<MYON_3dPoint>& subd_point_ring
    );

  static unsigned int GetSectorPointRing(
    bool bSubdivideIfNeeded,
    const MYON_SubDComponentPtr* component_ring,
    size_t component_ring_count,
    double* subd_point_ring,
    size_t subd_point_ring_capacity,
    size_t subd_point_ring_stride
    );

  static unsigned int GetSectorPointRing(
    bool bSubdivideIfNeeded,
    size_t component_ring_count,
    const MYON_SubDComponentPtr* component_ring,
    MYON_SimpleArray<MYON_3dPoint>& point_ring
    );

  static unsigned int GetSectorPointRing(
    bool bSubdivideIfNeeded,
    const class MYON_SubDSectorIterator& sit,
    double* point_ring,
    size_t point_ring_capacity,
    size_t point_ring_stride
    );

  static unsigned int GetSectorPointRing(
    bool bSubdivideIfNeeded,
    const class MYON_SubDSectorIterator& sit,
    MYON_SimpleArray<MYON_3dPoint>& point_ring
    );

  /*
  Parameters:
    subd_type - [in]
      A quad based subdivision algorithm.
    bFirstPass - [in]
      If bFirstPass is true and the components are in standard form for the vertex
      and subdivision type, then locations of the component vertices opposite the 
      center vertex are returned in the point ring.
    bSecondPass - [in]
      If bSecondtPass is true and the first pass is disable or does not succeed,
      then the component subdivision locations are returned in the point ring.
    vertex0 - [in]
      If not null, then vertex0->m_edges and vertex0->m_faces must
      be radially sorted and span a single sector and component_ring[]
      is ignored.
    component_ring_count - [in]
      If vertex0 is null, then component_ring_count specifies the number
      of components in the component_ring[] array.
    component_ring[] - [in]
      If vertex0 is null, then component_ring[0] is the central vertex,
      component_ring[1] and subsequent components with odd indices are
      sector edges, component_ring[2] and subsequent components with even
      indices are sector faces, all sorted radially.
    point_ring_stride - [in]
    point_ring - [out]
      point locations are returned here.
  Returns:
    Number of points in the subdivision ring or 0 if the call fails.
    The number of  points is 
    1 + MYON_SubD::ComponentRingEdgeCount(component_ring_count) + MYON_SubD::ComponentRingFaceCount(component_ring_count).
  Remarks:
    No validation checking is performed.  This function will crash
    if the input is not valid.  Call GetSubdivisionPointRing() if
    you want a crash proof call.
  */
  static unsigned int GetQuadSectorPointRing(
    bool bFirstPass,
    bool bSecondPass,
    const class MYON_SubDVertex* vertex0,
    const class MYON_SubDComponentPtr* component_ring,
    size_t component_ring_count,
    double* point_ring,
    size_t point_ring_stride
    );

  static const class MYON_SubDVertex* SubdivideSector(
    const class MYON_SubDVertex* center_vertex,
    const class MYON_SubDComponentPtr* component_ring,
    size_t component_ring_count,
    class MYON_SubD_FixedSizeHeap& fsh
    );

  /*
  Returns:
    true if sector_edge_count is valid for the vertex type
  */
  static bool IsValidSectorEdgeCount(
    MYON_SubDVertexTag vertex_tag,
    unsigned int sector_edge_count
    );  
  
  static bool IsValidSectorFaceCount(
    MYON_SubDVertexTag vertex_tag,
    unsigned int sector_face_count
    );

#pragma region RH_C_SHARED_ENUM [MYON_SubD::SubDFriendlyKnotType] [Rhino.Geometry.SubDFriendlyKnotType] [byte]
  /// <summary>
  /// MYON_SubD::SubDFriendlyKnotType identifies the types of subd friendly NURBS knot vectors.
  /// SubD friendly NURBS curves and surfacaes are always cubic and nonrational.
  /// Any time there is a multiple knot, the 2nd derivative is zero at the corresponding parameter.
  /// SubD friendly NURBS curves are either periodic or have zero 2nd derivative at the ends.
  /// </summary>    
  enum class SubDFriendlyKnotType : unsigned char
  {
    ///<summary>
    /// Not a valid type. Used to indicate the type has not been set and to encourage developers to explicitly specify a type.
    ///</summary>
    Unset = 0,

    ///<summary>
    /// NURBS knot vector is an unclamped uniform cubic knot vector. 
    /// Every knot interval has the same length.
    /// Every knot has multiplicity 1. 
    ///</summary>
    UnclampedUniform = 1,

    ///<summary>
    /// NURBS knot vector is a clamped uniform cubic knot vector. 
    /// Every interior knot interval has the same length.
    /// End knots have multiplicity 3 and interior knots have multiplicity 1.
    ///</summary>
    ClampedUniform = 2,

    ///<summary>
    /// NURBS knot vector is a clamped piecewise uniform cubic knot vector.
    /// All nonzero knot intervals have the same length.
    /// End knots have multiplicity 3 and interior knots have multiplicity 1 or 3.
    /// Interior knots with multiplicity 3 correspond to interior SubD creases.
    ///</summary>
    ClampedPiecewiseUniform = 4,

    ///<summary>
    /// NURBS knot vector is not subd friendly.
    ///</summary>
    Unfriendly = 127
  };
 #pragma endregion

  /*
  Parameters:
    order - [in]
      NURBS knot vector order.
    cv_count - [in]
      Number of NURBS knot vector control points.
    knots - [in]
      NURBS knot vector. This is an array of (cv_count+2) knot values.
  Returns:
    SubD friendly knot vector type.
  Remarks:
    If order is not 4, cv_count is not valid, or knot is nullptr, then
    MYON_SubD::SubDFriendlyKnotType::Unfriendly is returned.
  */
  static MYON_SubD::SubDFriendlyKnotType NurbsKnotType(
    int order,
    int cv_count,
    const double* knots
  );

  static MYON_SubD::SubDFriendlyKnotType NurbsKnotType(
    int order,
    int cv_count,
    const double* knots,
    MYON_SimpleArray<double>* triple_knots
  );

  /*
  Parameters:
    point_count - [in] >= 3
      Number of control points in a regular planar SubD ngon with creased edges.
    control_point_radius - [in]
      Distance from an ngon control point to the ngon center.
  Returns:
    Distance from an ngon surface point to the ngon center.
  See Also:
    MYON_SubD::SurfacePointRadiusFromControlPointRadius()
  */
  static double SurfacePointRadiusFromControlPointRadius(
    unsigned int point_count,
    double control_point_radius
  );

  /*
  Parameters:
    point_count - [in] >= 3
      Number of control points in a regular planar SubD ngon with creased edges.
    surface_radius - [in]
    Distance from an ngon surface point to the ngon center.
  Returns:
    Distance from an ngon control point to the ngon center.
  See Also:
    MYON_SubD::ControlPointRadiusFromSurfacePointRadius()
  */
  static double ControlPointRadiusFromSurfacePointRadius(
    unsigned int point_count,
    double surface_point_radius
  );







  MYON_SubD() MYON_NOEXCEPT;
  virtual ~MYON_SubD();



  /*
  Description:
    Creates an independent copy of src.
  */
  MYON_SubD( const MYON_SubD& src );

  /*
  Description:
    Creates an independent copy of src.
  */
  MYON_SubD& operator=(const MYON_SubD& src);

#if defined(MYON_HAS_RVALUEREF)
  // rvalue copy constructor
  MYON_SubD( MYON_SubD&& ) MYON_NOEXCEPT;

  // The rvalue assignment operator calls MYON_Object::operator=(MYON_Object&&)
  // which could throw exceptions.  See the implementation of
  // MYON_Object::operator=(MYON_Object&&) for details.
  MYON_SubD& operator=( MYON_SubD&& );
#endif

  /*
  Description:
    The subdivision information referenced by src_subd will be shared with this
  Remarks:
    MYON_Geometry base class information, like MYON_UserData, is not copied or shared.
  */
  void ShareContentsFrom(
    MYON_SubD& subd
    );

  static void SwapContents(
    MYON_SubD& a,
    MYON_SubD& b
    );

  //virtual 
  void MemoryRelocate() override;

  //virtual
  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;

  //virtual
  void Dump(
    MYON_TextLog&
    ) const override;

  //virtual
  unsigned int SizeOf() const override;

  /*
  Description:
    This is a debugging too used to study how efficiently SubDs are using memory.
  Returns:
    Total operating system heap (in bytes) used by this SubD's MYON_FixedSizePools, include the mesh fragments pool.
  Remarks:
    SizeOfAllElements() = SizeOfActiveElements() + SizeOfUnusedElements().
  */
  size_t SizeOfAllElements() const;

  /*
  Description:
    This is a debugging too used to study how efficiently SubDs are using memory.
  Returns:
    Total operating system heap (in bytes) of memory in this SubD's MYON_FixedSizePools
    that is currently used by active elements, including active mesh fragments.
  Remarks:
    SizeOfAllElements() = SizeOfActiveElements() + SizeOfUnusedElements().
  */
  size_t SizeOfActiveElements() const;

  /*
  Description:
    This is a debugging too used to study how efficiently SubDs are using memory.
  Returns:
    Total operating system heap (in bytes) of memory in this SubD's MYON_FixedSizePools
    that is reserved but not currently used, including unused mesh fragments.
  Remarks:
    SizeOfAllElements() = SizeOfActiveElements() + SizeOfUnusedElements().
  */
  size_t SizeOfUnusedElements() const;

  /*
  Description:
    Tool for debugging mesh fragments memory use.
  Returns:
    Total operating system heap memory (in bytes) used by the mesh fragments pool.
  Remarks:
    SizeOfAllMeshFragments() = SizeOfActiveMeshFragments() + SizeOfUnusedMeshFragments().
  */
  size_t SizeOfAllMeshFragments() const;

  /*
  Description:
    Tool for debugging mesh fragments memory use.
  Returns:
    Operating system heap memory (in bytes) that are used by active mesh fragments.
  Remarks:
    SizeOfAllMeshFragments() = SizeOfActiveMeshFragments() + SizeOfUnusedMeshFragments().
  */
  size_t SizeOfActiveMeshFragments() const;


  /*
  Description:
    Tool for debugging mesh fragments memory use.
  Returns:
    Operating system heap memory (in bytes) that has been reserved for mesh fragments
    but is not currently used by active mesh fragments.
  Remarks:
    SizeOfAllMeshFragments() = SizeOfActiveMeshFragments() + SizeOfUnusedMeshFragments().
  */
  size_t SizeOfUnusedMeshFragments() const;

  //virtual
  MYON__UINT32 DataCRC(
    MYON__UINT32 current_remainder
    ) const override;

  //virtual
  bool Write(
         MYON_BinaryArchive& archive
         ) const override;

  //virtual
  bool Read(
         MYON_BinaryArchive& archive
         ) override;

  //virtual
  MYON::object_type ObjectType() const override;


  //virtual
  unsigned int ClearComponentStates(
    MYON_ComponentStatus states_to_clear
    ) const override;

  //virtual
  unsigned int GetComponentsWithSetStates(
    MYON_ComponentStatus states_filter,
    bool bAllEqualStates,
    MYON_SimpleArray< MYON_COMPONENT_INDEX >& components
    ) const override;
  
  //virtual
  unsigned int SetComponentStates(
    MYON_COMPONENT_INDEX component_index,
    MYON_ComponentStatus states_to_set
    ) const override;

  //virtual
  unsigned int ClearComponentStates(
    MYON_COMPONENT_INDEX component_index,
    MYON_ComponentStatus states_to_clear
    ) const override;

  //virtual
  unsigned int SetComponentStatus(
    MYON_COMPONENT_INDEX component_index,
    MYON_ComponentStatus status_to_copy
    ) const override;

  //virtual
  MYON_AggregateComponentStatus AggregateComponentStatus() const override;

  //virtual
  void MarkAggregateComponentStatusAsNotCurrent() const override;

  //virtual
  bool DeleteComponents(
    const MYON_COMPONENT_INDEX* ci_list,
    size_t ci_count
    ) override;

  /*
  Remarks:
    For MYON_SubD objects, ClearBoundingBox() and DestroyRuntimeCache()
    are identical.
  */
  //virtual
  void DestroyRuntimeCache(
    bool bDelete = true
    ) override;

  //virtual 
  int Dimension() const override;

  // virtual MYON_Geometry GetBBox override		
  bool GetBBox( double* boxmin, double* boxmax, bool bGrowBox = false ) const override;


  /*
  Description:
    Clears all saved information that depends on vertex locations,
    subdivision algorithms, vertex or edge tags, or control net topology.  
    If you modify any of the above, then call ClearBoundingBox().
  Remarks:
    For MYON_SubD objects, ClearBoundingBox() and DestroyRuntimeCache()
    are identical.
  */
  //virtual
  void ClearBoundingBox() override;

  //virtual
  bool Transform( 
         const MYON_Xform& xform
         ) override;

  //virtual
  bool IsDeformable() const override;

  //virtual
  bool MakeDeformable() override;

  //virtual
  bool SwapCoordinates(
        int i,
        int j
        ) override;
  
  
  //virtual
  bool HasBrepForm() const override;

  //virtual
  /*
  Returns:
    GetSurfaceBrep( MYON_SubDToBrepParameters::Default, nullptr );
  */
  MYON_Brep* BrepForm(
    MYON_Brep* brep = nullptr
  ) const override;

  //virtual
  MYON_COMPONENT_INDEX ComponentIndex() const override;

  //virtual
  bool EvaluatePoint(
    const class MYON_ObjRef& objref, 
    MYON_3dPoint& P
    ) const override;

  /*
  Description:
    Uses the input mesh to define the level zero control polygon.
  Parameters:
    level_zero_mesh - [in]
    from_mesh_parameters - [in]
      To get the smoothest possible result, pass nullptr 
      or MYON_SubDFromMeshParameters::Smooth. To get a sub-D with interior 
      creases use other static MYON_SubDFromMeshParameters values or
      create one with custom settings.
  */
  static MYON_SubD* CreateFromMesh( 
    const class MYON_Mesh* level_zero_mesh,
    const class MYON_SubDFromMeshParameters* from_mesh_parameters,
    MYON_SubD* subd
    );

public:
#pragma region RH_C_SHARED_ENUM [MYON_SubD::AutomaticMeshToSubDContext] [Rhino.Geometry.SubDAutomaticMeshToSubDContext] [byte]
  /// <summary>
  /// MYON_SubD::AutomaticMeshToSubDContext identifies a context where meshes can automatically
  /// be converted to subds.
  /// </summary>
  enum class AutomaticMeshToSubDContext : unsigned char
  {
    ///<summary>
    /// Indicates the context has not been initialized.
    ///</summary>
    Unset = 0,

    ///<summary>
    /// A mesh in a Rhino 5 3dm file that is a representation of a box mode T-spline.
    /// By default, these meshes are automatically converted to subds.
    ///</summary>
    Rhino5BoxModeTSpline = 1,

    ///<summary>
    /// A mesh in an FBX file that has nonzero values for either preview division levels or render division levels.
    /// Some FBX files created by Maya save subdivision objects as meshes with nonzero division level values.
    /// By default, FBX division levels are ignored.
    ///</summary>
    FBXMeshWithDivisionLevels = 2
  };
#pragma endregion

  /*
  Returns:
    true if SubDs are automatically created when an MYON_Mesh is found in the specified context.
    false otherwise.
  */
  static bool AutomaticMeshToSubD(
    MYON_SubD::AutomaticMeshToSubDContext context
  );

  /*
  Parameters:
    context - [in]
      Situation where an MYON_Mesh can automatically be converted into a subd.
    bAutomaticallyCreateSubD - [in]
      true if SubDs are automatically created when an MYON_Mesh is found in the specified context.
      false otherwise.
  */
  static void SetAutomaticMeshToSubD(
    MYON_SubD::AutomaticMeshToSubDContext context,
    bool bAutomaticallyCreateSubD
    );

  /*
  Parameters:
    context - [in]
      If context is MYON_SubD::AutomaticMeshToSubDContext::Unset, all defaults will be restored.
      Otherwise, the default for the specific context will be restored.
  */
  static void AutomaticMeshToSubDRestoreDefaults(
    MYON_SubD::AutomaticMeshToSubDContext context
    );

private:
  static const  bool AutomaticRhino5BoxModeTSplineToSubDDefault; // = true
  static const bool AutomaticFBXMeshWithDivisionLevelsToSubDDefault; // = false
  static bool AutomaticRhino5BoxModeTSplineToSubD; // current setting
  static bool AutomaticFBXMeshWithDivisionLevelsToSubD; // current setting

private:
  static MYON_SubD* Internal_CreateFromMeshWithValidNgons(
    const class MYON_Mesh* level_zero_mesh_with_valid_ngons,
    const class MYON_SubDFromMeshParameters* from_mesh_parameters,
    MYON_SubD* subd
  );

public:

  /*
  Description:
    Creates a SubD box
  Parameters:
    corners - [in] 
      Box corners.
      The bottom quad is specified by the first 4 points
      and the top quad specified by the last 4 points.
    edge_tag - [in]
      If edge_tag = MYON_SubDEdgeTag::Crease, then the box will have 
      creases and corners. Otherwise the box will be smooth.
    facecount_x - [in] Number of faces in x direction
    facecount_y - [in] Number of faces in y direction
    facecount_z - [in] Number of faces in z direction
    destination_subd [out] - 
      If destination_subd is not null, make the SubD box there
  Returns:
    Pointer to the resulting SubD if successful
    Null for error
  */
  static MYON_SubD* CreateSubDBox(
    const MYON_3dPoint corners[8],
    MYON_SubDEdgeTag edge_tag,
    unsigned int facecount_x,
    unsigned int facecount_y,
    unsigned int facecount_z,
    MYON_SubD* destination_subd
  );

  /*
  Description:
    Creates a SubD sphere with 24 quad faces
  Parameters:
    sphere - [in]
      Location, size and orientation of the sphere
    destination_subd [out] -
      If destination_subd is not null, make the SubD box there
  Returns:
    Pointer to the resulting SubD if successful
    Null for error
  */
  //static MYON_SubD* CreateSubDSphere(
  //  const MYON_Sphere sphere,
  //  MYON_SubD* destination_subd);

  /*
  Description:
    Creates a SubD cylinder
  Parameters:
    box - [in]
      Location, size and orientation of the cylinder
    facecount_around - [in] Number of faces around the cylinder
    facecount_length - [in] Number of faces in the axis direction
    facecouont_z - [in] Number of faces in z direction
    destination_subd [out] -
      If destination_subd is not null, make the SubD box there
  Returns:
    Pointer to the resulting SubD if successful
    Null for error
  */
  //static MYON_SubD* CreateSubDCylinder(
  //  const MYON_Cylinder& cylinder,
  //  unsigned int facecount_around,
  //  unsigned int facecount_length,
  //  MYON_SubD* destination_subd);

  /*
  Description:
    Creates a SubD cone
  Parameters:
    cone - [in]
      Location, size and orientation of the cone
    facecount_around - [in] Number of faces around the cone
    facecount_length - [in] Number of faces in the axis direction
    destination_subd [out] -
      If destination_subd is not null, make the SubD cone there
  Returns:
    Pointer to the resulting SubD if successful
    Null for error
  */
  //static MYON_SubD* CreateSubDCone(
  //  const MYON_Cone& cone,
  //  unsigned int facecount_around,
  //  unsigned int facecount_length,
  //  MYON_SubD* destination_subd);

  /*
  Description:
    Creates a SubD truncated cone
  Parameters:
    cone - [in]
      Location, size and orientation of the cone
    truncate_param - [in] 0.0 < truncate_param <= 1.0
        Normalized parameter for truncation
        0.0: Base of cone
        1.0: Tip of cone
    facecount_around - [in] Number of faces around the cone
    facecount_length - [in] Number of faces in the axis direction
    destination_subd [out] -
      If destination_subd is not null, make the SubD cone there
  Returns:
    Pointer to the resulting SubD if successful
    Null for error
  */
  //static MYON_SubD* CreateSubDTruncatedCone(
  //  const MYON_Cone& cone,
  //  const double truncate_param,
  //  unsigned int facecount_around,
  //  unsigned int facecount_length,
  //  MYON_SubD* destination_subd);

  /*
  Description:
    Creates a SubD torus
  Parameters:
    torus - [in]
      Location, size and orientation of the torus
    major_facecount - [in] Number of faces around the major axis
    minor_facecount - [in] Number of faces around the minor axis
    destination_subd [out] -
      If destination_subd is not null, make the SubD torus there
  Returns:
    Pointer to the resulting SubD if successful
    Null for error
  */
  //static MYON_SubD* CreaptSubDTorus(
  //  MYON_Torus& torus,
  //  unsigned int major_facecount,
  //  unsigned int minor_facecount,
  //  MYON_SubD* destination_subd);

  unsigned int DumpTopology(
    MYON_TextLog&
    ) const;

  unsigned int DumpTopology(
    MYON_2udex vertex_id_range,
    MYON_2udex edge_id_range,
    MYON_2udex face_id_range,
    MYON_TextLog&
    ) const;

  /*
  Description:
    Discard all contents of this MYON_SubD.
  Remarks:
    More efficient than Destroy() if this MYON_SubD will be reused soon.
  */
  void Clear();

  /*
  Description:
    Delete all contents release all memory used by this MYON_SubD.
  */
  void Destroy();

  /*
  Returns:
    The number of explicitly computed levels that are currently available.
    A value of 0 indicates this SubD is empty.
  */
  unsigned int LevelCount() const;

  /*
  Returns:
    If the SubD is not empty, then the index of the active level is returned. This value will be < LevelCount().
    If the SubD is empty, then MYON_UNSET_UINT_INDEX is returned.
  */
  unsigned int ActiveLevelIndex() const;

  /*
  Description:
    Remove subdivision levels
  Parameters:
    max_level_index - [in] 
      Remove all levels after max_level_index
  Returns:
    Number of removed levels.
  */
  unsigned int ClearHigherSubdivisionLevels(
    unsigned int max_level_index
    );

  /*
  Description:
    Remove subdivision levels
  Parameters:
    min_level_index - [in] 
      Remove all levels before min_level_index
  Returns:
    Number of removed levels.
  */
  unsigned int ClearLowerSubdivisionLevels(
    unsigned int min_level_index
    );

  /*
  Remove all levels except the active level.
  Returns: 
    Number of removed levels.
  */
  unsigned ClearInactiveLevels();

  bool IsEmpty() const;
  bool IsNotEmpty() const;

  /// <summary>
  /// Determine if this SubD has sharp edges. 
  /// See MYON_SubDEdge::IsSharp() for more information about sharp edges.
  /// </summary>
  /// <returns>True if the SubD has at lease one sharp edge.</returns>
  bool HasSharpEdges() const;

  /// <summary>
  /// Get the range of sharpness values assigned to sharp edges
  /// and return the number of sharp edges.
  /// See MYON_SubDEdge::IsSharp() for more information about sharp edges.
  /// </summary>
  /// <param name="sharpness_range">The range of sharpness values is returned here. (0,0) is returned if there are no sharp edges.</param>
  /// <returns>Number of sharp edges.</returns>
  unsigned int SharpEdgeCount(MYON_SubDEdgeSharpness& sharpness_range) const;

  /// <summary>
  /// Number of sharp edges.
  /// See MYON_SubDEdge::IsSharp() for more information about sharp edges.
  /// </summary>
  /// <returns>Number of sharp edges.</returns>
  unsigned int SharpEdgeCount() const;

  /// <summary>
  /// Converts all sharp edges to smooth edges.
  /// See MYON_SubDEdge::IsSharp() for more information about sharp edges.
  /// </summary>
  /// <returns>Number of sharp edges that were converted to smooth edges.</returns>
  unsigned int ClearEdgeSharpness();


  /*
  Description:
    Get aggregate edge demographics for the subd.
  Returns:
    Bitwise or of MYON_ComponentAttributes::EdgeAttributes values for every edge in the subd.
  */
  unsigned int AggregateEdgeAttributes() const;
  
  /////////////////////////////////////////////////////////
  //
  // Component (Vertex, Edge, Face) access
  //
  const MYON_SubDComponentPtr ComponentPtrFromComponentIndex(
    MYON_COMPONENT_INDEX component_index
    ) const;

  unsigned int ComponentPtrFromComponentIndex(
    const MYON_COMPONENT_INDEX* ci_list,
    size_t ci_count,
    MYON_SimpleArray<MYON_SubDComponentPtr>& cptr_list
  ) const;

  unsigned int ComponentPtrFromComponentIndex(
    const MYON_COMPONENT_INDEX* ci_list,
    size_t ci_count,
    bool bIncludeVertices,
    bool bIncludeEdges,
    bool bIncludeFaces,
    MYON_SimpleArray<MYON_SubDComponentPtr>& cptr_list
  ) const;

  /////////////////////////////////////////////////////////
  //
  // Membership query
  //
  bool InSubD(const class MYON_SubDVertex* vertex) const;
  bool InSubD(const class MYON_SubDEdge* edge) const;
  bool InSubD(const class MYON_SubDFace* face) const;


  /*
  Returns:
    MYON_SubDComponentPtr::Type::Unset if b is not in this SubD.
    Otherwise the type of the component is returned.
  */
  bool InSubD(MYON_SubDComponentPtr cptr) const;
  
  /*
  Returns:
    If b is in this SubD, a MYON_SubDComponentPtr to b is returned.
    Otherwise MYON_SubDComponentPtr::Null is returned.
  Remarks:
    This function is the slowest of the InSubD() overrides.
    When b is an unknown component type, this function can be used to
    safely determine what type of component (vertex/edge/face).
  */
  const MYON_SubDComponentPtr InSubD(const class MYON_SubDComponentBase* b) const;

  /////////////////////////////////////////////////////////
  //
  // Vertex access
  //

  /*
  Returns:
    Number of vertices in the active level.
  */
  unsigned int VertexCount() const;

  /*
  Parameters:
    hash_type - [in]
      All hashes include vertex id.
      Geometry hashes include vertex tag, vertex control point, and vertex displacement.
  Returns:
    A SHA1 hash of the SubD's vertices.
  */
  const MYON_SHA1_Hash VertexHash( MYON_SubDHashType hash_type ) const;

  const class MYON_SubDVertex* FirstVertex() const;
  const class MYON_SubDVertex* LastVertex() const;

  /*
  Example:
    MYON_SubDVertexIterator vit = subd.VertexIterator();
    for ( const MYON_SubDVertex* v = vit.FirstVertex(); nullptr != v; v = vit.NextVertex())
    {
      ...
    }
  */
  class MYON_SubDVertexIterator VertexIterator() const;

  /*
  Description:
    Avoid using this class. It is more efficient to use
    an MYON_SubDVertexIterator returned by VertexIterator().
  */
  class MYON_SubDVertexArray VertexArray() const;

  /*
  Parameters:
    vertex_id - [in]
  Returns:
    If vertex_id identifies a valid vertex in this MYON_SubD, then
    a pointer to that vertex is returned.
    Otherwise, nullptr is returned.
  */
  const class MYON_SubDVertex* VertexFromId(
    unsigned int vertex_id
  ) const;

  const class MYON_SubDVertex* VertexFromComponentIndex(
    MYON_COMPONENT_INDEX component_index
  ) const;

  /////////////////////////////////////////////////////////
  //
  // Edge access
  //

  /*
  Returns:
    Number of edges in the active level.
  */
  unsigned int EdgeCount() const;

  /*
  Parameters:
    hash_type - [in]
      All hashes include edge id, edge vertex ids in order, the edge crease/smooth attribute.
      Geometry hashes include edge displacements.
  Returns:
    A SHA1 hash of the SubD's edges.
  */
  const MYON_SHA1_Hash EdgeHash(MYON_SubDHashType hash_type) const;

  const class MYON_SubDEdge* FirstEdge() const;
  const class MYON_SubDEdge* LastEdge() const;

  /*
  Example:
    MYON_SubDEdgeIterator eit = subd.EdgeIterator();
    for ( const MYON_SubDEdge* e = eit.FirstEdge(); nullptr != e; e = eit.NextEdge())
    {
      ...
    }
  */
  class MYON_SubDEdgeIterator EdgeIterator() const;

  /*
  Description:
    Avoid using this class. It is more efficient to use
    an MYON_SubDEdgeIterator returned by EdgeIterator().
  */
  class MYON_SubDEdgeArray EdgeArray() const;

  /*
  Parameters:
    edge_id - [in]
  Returns:
    If edge_id identifies a valid edge in this MYON_SubD, then
    a pointer to that edge is returned.
    Otherwise, nullptr is returned.
  */
  const class MYON_SubDEdge* EdgeFromId(
    unsigned int edge_id
    ) const;

  const class MYON_SubDEdge* EdgeFromComponentIndex(
    MYON_COMPONENT_INDEX component_index
  ) const;

  /////////////////////////////////////////////////////////
  //
  // Face access
  //

  /*
  Returns:
    Number of faces in the active level.
  */
  unsigned int FaceCount() const;

  /*
  Parameters:
    hash_type - [in]
  Returns:
    A SHA1 hash of the SubD's faces.
  */
  const MYON_SHA1_Hash FaceHash(MYON_SubDHashType hash_type) const;

  const class MYON_SubDFace* FirstFace() const;
  const class MYON_SubDFace* LastFace() const;

  /*
  Example:
    MYON_SubDFaceIterator fit = subd.FaceIterator();
    for ( const MYON_SubDFace* f = fit.FirstFace(); nullptr != f; f = fit.NextFace())
    {
      ...
    }
  */
  class MYON_SubDFaceIterator FaceIterator() const;

  /*
  Description:
    Avoid using this class. It is more efficient to use
    an MYON_SubDFaceIterator returned by FaceIterator().
  */
  class MYON_SubDFaceArray FaceArray() const;

  /*
  Parameters:
    face_id - [in]
  Returns:
    If face_id identifies a valid face in this MYON_SubD, then
    a pointer to that face is returned.
    Otherwise, nullptr is returned.
  */
  const class MYON_SubDFace* FaceFromId(
    unsigned int face_id
    ) const;


  const class MYON_SubDFace* FaceFromComponentIndex(
    MYON_COMPONENT_INDEX component_index
  ) const;

  /////////////////////////////////////////////////////////
  //
  // Component (vertex, edge, face) state ( selected, highlighted, ... ) tools
  //   NOTE: 
  //    All component status settings are mutable
  //    All are copied. 
  //    None are saved.
  //

  /*
  Parameters:
    states_filter - [in]
    bAllEqualStates - [in]
      If a state is set in states_filter, all active level components
      with the same state set will be included in the 
      components_with_set_states[] array.  
      If bAllEqualStates is true, then MYON_ComponentStatus::AllEqualStates()
      is used to test for inclusion.  
      If bAllEqualStates is false, then MYON_ComponentStatus::SomeEqualStates()
      is used to test for inclusion.
    components_with_set_states - [out]
  Returns:
    Number of returned components.
  */
  unsigned int GetComponentsWithSetStates(
    MYON_ComponentStatus states_filter,
    bool bAllEqualStates,
    MYON_SimpleArray< MYON_SubDComponentPtr >& components_with_set_states
    ) const;

  
  /*
  Description:
    Set states on an individual component.
  Parameters:
    component_ptr - [in]
      The states will be set on this component.
    states_to_set - [in]
      If a state is set in the states_to_set parameter, the same
      state will be set on the component.      
  Returns:
    0: no state settings changed on the component.
    1: some state setting changed on the component.
  */
  unsigned int SetComponentStates(
    MYON_SubDComponentPtr component_ptr,
    MYON_ComponentStatus states_to_set
    ) const;

  /*
  Description:
    Clear states on an individual component.
  Parameters:
    component_ptr - [in]
      The states will be cleared on this component.
    states_to_clear - [in]
      If a state is set in the states_to_clear parameter, the same
      state will be cleared on the component.      
  Returns:
    0: no state settings changed on the component.
    1: some state setting changed on the component.
  */
  unsigned int ClearComponentStates(
    MYON_SubDComponentPtr component_ptr,
    MYON_ComponentStatus states_to_clear
    ) const;
  
  /*
  Description:
    Copy status settings to an individual component.
  Parameters:
    component_ptr - [in]
      The states will be copied to this component.
    status_to_copy - [in]
  Returns:
    1: some state settings changed on the component.
    1: some state setting changed on the component.
  */
  unsigned int SetComponentStatus(
    MYON_SubDComponentPtr component_ptr,
    MYON_ComponentStatus status_to_copy
    ) const;

  /*
  Description:
    Delete components in cptr_list[]. 
    If a vertex is in cptr_list[], the vertex and every edge and face attached
    to the vertex are deleted.
    If an edge is in cptr_list[], the edge and every face attached
    to the edge are deleted.
    If a face is in cptr_list[], the face is deleted.
  Parameters:
    cptr_list - [in]
    cptr_count - [in]
      length of cptr_list[] array.
    bMarkDeletedFaceEdges - [in]
      If true, surviving edges attached to delete faces 
      have their runtmime mark set.
  Returns:
    1: some state settings changed on the component.
    1: some state setting changed on the component.
  */
  bool DeleteComponents(
    const MYON_SubDComponentPtr* cptr_list,
    size_t cptr_count,
    bool bMarkDeletedFaceEdges
    );

  bool DeleteComponents(
    const MYON_SimpleArray<MYON_SubDComponentPtr>& cptr_list,
    bool bMarkDeletedFaceEdges
  );

  bool DeleteComponentsForExperts(
    const MYON_SubDComponentPtr* cptr_list,
    size_t cptr_count,
    bool bDeleteIsolatedEdges,
    bool bUpdateTagsAndCoefficients,
    bool bMarkDeletedFaceEdges
    );

  /*
  Description:
    Delete marked components.
  Parameters:
    bDeleteMarkedComponents - [in]
      If true, marked components are deleted.
      If false, unmarked components are deleted.      
    mark_bits - [in]
      A component is marked if component.m_status.IsMarked(mark_bits) is true.
  */
  bool DeleteMarkedComponents(
    bool bDeleteMarkedComponents,
    MYON__UINT8 mark_bits,
    bool bMarkDeletedFaceEdges
  );

  /*
  Description:
    Delete marked components.
  Parameters:
    bDeleteMarkedComponents - [in]
      If true, marked components are deleted.
      If false, unmarked components are deleted.
    mark_bits - [in]
      A component is marked if component.m_status.IsMarked(mark_bits) is true.
  */
  bool DeleteMarkedComponentsForExperts(
    bool bDeleteMarkedComponents,
    MYON__UINT8 mark_bits,
    bool bDeleteIsolatedEdges,
    bool bUpdateTagsAndCoefficients,
    bool bMarkDeletedFaceEdges
  );

public:
  /*
  Description:
    Removes all per face material channel index overrides on the active level.
  Returns:
    Number of changed faces.
  Remarks:
    Per face material channel indices are a mutable property on MYON_SubDFace and are set with MYON_SubDFace.SetMaterialChannelIndex().
  */
  unsigned int ClearPerFaceMaterialChannelIndices();

  /*
  Returns:
    True if one or more faces on the active level have per face material channel index overrides.
  Remarks:
    Per face material channel indices are a mutable property on MYON_SubDFace and are set with MYON_SubDFace.SetMaterialChannelIndex().
  */
  bool HasPerFaceMaterialChannelIndices() const;

  /*
  Description:
    Removes all per face color overrides on the active level.
  Returns:
    Number of changed faces.
  Remarks:
    Per face colors are a mutable property on MYON_SubDFace and are set with MYON_SubDFace.SetPerFaceColor().
  */
  unsigned int ClearPerFaceColors() const;

  /*
  Returns:
    True if one or more faces on the active level have per face color overrides.
  Remarks:
    Per face colors are a mutable property on MYON_SubDFace and are set with MYON_SubDFace.SetPerFaceColor().
  */
  bool HasPerFaceColors() const;

  /*
  Description:
    If a face has a nonzero PackId(), then its per face color is set to MYON_Color::RandomColor(f->PackId()).
    Otherwise, its per face color is cleared.
  */
  void SetPerFaceColorsFromPackId() const;

  /*
  Returns:
    True if per face colors were set by SetPerFaceColorsFromPackId().
  */
  bool HasPerFaceColorsFromPackId() const;


  /*
  Description:
    If a SubD is symmetric and a face belongs to a symmetry set, then per face color is
    set according to the motif the face belongs to.
    Otherwise, its per face color is cleared.
  */
  void SetPerFaceColorsFromSymmetryMotif() const;

  /*
  Returns:
    True if per face colors were set by SetPerFaceColorsFromSymmetryMotif().
  */
  bool HasPerFaceColorsFromSymmetryMotif() const;

  /*
  Description:
    Sets MYON_SubDComponent MarkBits() to
    0: component is not in a symmetry set motif
    n>=1: 
      The component is the the n-th element in the symmetry set
      with n=1 indicating the component in the primary motif.
  */
  void SetComponentMarkBitsFromSymmetryMotif() const;


  ///*
  //Description:
  //  The MYON__INT_PTRs in the tree are const MYON_SubDMeshFragment* pointers.
  //  The bounding boxes are from the surface points.
  //*/
  //MYON_RTreeRef FragmentTree() const;

  ///*
  //Description:
  //  If the tree is not needed and memory resources are tight, then call ClearTree()
  //  to remove the RTree.
  //*/
  //void ClearFragmentTree();

  ///*
  //Description:
  //  The MYON__INT_PTRs in the tree are const MYON_SubDComponentPtrs.
  //  The bounding boxs are based on the subd
  //*/
  //MYON_RTreeRef ControlNetComponentTree(
  //  bool bIncludeVertices,
  //  bool bIncludeEdges,
  //  bool bIncludeFaces
  //  ) const;

  ///*
  //Description:
  //  If the tree is not needed and memory resources are tight, then call ClearTree()
  //  to remove the RTree.
  //*/
  //void ClearControlNetComponentTree();

  /////////////////////////////////////////////////////////
  //
  // Topology Queries
  //

  /*
  Description:
    Determine solid orientation of the active level.
  Returns:
    +2     subd is a solid but orientation cannot be computed
    +1     subd is a solid with outward facing normals
    -1     subd is a solid with inward facing normals
     0     subd is not a solid
  See Also:
    MYON_SubD::IsSolid
  */
 int SolidOrientation() const;

  /*
  Description:
    Test subd to see if the active level is a solid.  
    A "solid" is a closed oriented manifold.
  Returns:
    true       subd is a solid
    fals       subd is not a solid
  See Also:
    MYON_SubDp::SolidOrientation
    MYON_SubDp::IsManifold
  */
  bool IsSolid() const;
  
  /*
  Description:
    Test subd to see if the active level is an oriented manifold.
  Parameters:
    bIsOriented - [out]
      True if every edge that has two faces is oriented.
      Note that non-manifold edges are ignored.
    bHasBoundary - [in]
      True if there is at least one edge with a single face.
  Returns:
    True if the subd is a manifold (has at lease one face and every edge has 1 or 2 faces).
    False if the subd is not a manifold (has no faces or at least one edge with 0 or 3 or more faces)
  See Also:
    MYON_SubDp::IsSolid
  */
  bool IsManifold(
    bool& bIsOriented,
    bool& bHasBoundary
    ) const;

  bool IsManifold() const;



    /*
  Description:
    Automatically get a  boundary from a seed edge.
  Parameters:
    first_edge - [in]
      An edge with FaceCount() <= 1. 
      The search for the second edge occurs and first_edge.RelativeVertex(1)
      and all edges added to boundary_edge_chain[] have FaceCount() = first_edge.Edge()->FaceCount().
    bUseEdgeMarks -[in]
      If true, only unmarked edges will be added to boundary_edge_chain[]
      and they will be marked when added to boundary_edge_chain[].
    boundary_edge_chain - [out]
      Edge chain beginning with first_edge. 
      When true is returned, boundary_edge_chain[] has 3 or more edges and is a closed loop.
      When false is returned, boundary_edge_chain[] will contain an open chain with 0 or more edges.
  Returns:
    true if boundary_edge_chain[] is a closed loop of 3 or more boundary edges.
  */
  static bool GetBoundaryEdgeChain(
    MYON_SubDEdgePtr first_edge,
    bool bUseEdgeMarks,
    MYON_SimpleArray< MYON_SubDEdgePtr >& boundary_edge_chain
  );

  /////////////////////////////////////////////////////////
  //
  // Editing tools
  //

  unsigned int MergeColinearEdges(
    bool bMergeBoundaryEdges,
    bool bMergeInteriorCreaseEdges,
    bool bMergeInteriorSmoothEdges,
    double distance_tolerance,
    double maximum_aspect,
    double sin_angle_tolerance
    );

  /*
  Description:
    Merge consecutive edges into a single edge.
  eptr0 - [in]
    first edge (will not be deleted)
  eptr1 - [in]
    second edge (will be deleted if edges can be merged)
  Returns:
    Merged edge (eptr0) or MYON_SubDEdgePtr::Null if edges could not be merged
  */
  MYON_SubDEdgePtr MergeConsecutiveEdges(
    MYON_SubDEdgePtr eptr0,
    MYON_SubDEdgePtr eptr1
    );

  /*
  Returns:
    True if eptr0.RelativeVetex(1) == eptr1.RelativeVetex(0) and both edges
    have the same set of faces.
  */
  static bool EdgesAreConsecutive(
    MYON_SubDEdgePtr eptr0,
    MYON_SubDEdgePtr eptr1
    );

  // returns true if all facets are consistently oriented
  bool IsOriented() const;

  // reverses the orientation of all facets
  bool ReverseOrientation() const;

  // Attempts to orient all facet to match the first facet.
  bool Orient() const;

  /*
  Description:
    Divide an edge into two contiguous edges.
    The input edge is modified to terminate at the new vertex.
    The new edge begins at the new vertex and ends at the original edge's m_vertex[1].
  edge - [in]
    edge to split.
  vertex_location - [in]
    location of the new vertex vertex.
    If vertex_location == MYON_MYON_3dPoint::UnsetPoint,
    then the edge's midpoint is used.
  Returns:
    A pointer to the new edge or nullptr if the input is not valid.
  Remarks:
    After all editing operations are completed, you must call this->UpdateEdgeSectorCoefficients(true) before
    evaluation.
  */
  const class MYON_SubDEdge* SplitEdge(
    class MYON_SubDEdge* edge,
    MYON_3dPoint vertex_location
  );

  /*
  Description:
    Divide an edge into two contiguous edges.
  edge - [in]
    edge to split.
  vertex_location - [in]
    location of the new vertex vertex.
    If vertex_location == MYON_MYON_3dPoint::UnsetPoint,
    then the edge's midpoint is used.
  new_edge_end - [in]
    This paratmer is 0 or 1 and dtermines where the new edge is inserted.
    Below v0 = input eptr.RelativeVertex(0), v1 = new vertex, v2 = input eptr.RelativeVertex(1),
    and new_eptr = returned edge pointer.
    If edge_end is 0, new_eptr is first: v0 = new_eptr.RelativeVertex(0), v1 = new_eptr.RelativeVertex(1)=eptr.RelativeVertex(0), v2 = eptr.RelativeVertex(1).
    If edge_end is 1, new_eptr is last: v0 = eptr.RelativeVertex(0), v1 = eptr.RelativeVertex(1)=new_eptr.RelativeVertex(0), v2 = new_eptr.RelativeVertex(1).
  Returns:
    A pointer to the new edge or MYON_SubDEdgePtr::Null if the input is not valid.
  Remarks:
    After all editing operations are completed, you must clear the evaluation cache
    and call this->UpdateEdgeSectorCoefficients(true).
  */
  const MYON_SubDEdgePtr SplitEdge(
    MYON_SubDEdgePtr eptr,
    MYON_3dPoint vertex_location,
    unsigned new_edge_end
  );
  
  /*
  Description:
    Spin an edge's endpoints around the boundary of its neighboring faces.
    In a counter-clockwise spin (looking at faces from their shared up orientation):
      The edge's start vertex is moved to the next vertex in the boundary
      of the face on the right-hand side of the edge.
      The edge's end vertex is moved to the next vertex in the boundary
      of the face on the left-hand side of the edge.
    Note that reversing the input edge does not change the result.
  Parameters:
    edge - [in]
      edge to spin.
    spin_clockwise - [in]
      false spins the edge counter-clockwise, true spins the edge clockwise
      in the adjacent faces.
  Returns:
    A pointer to the spun edge or nullptr if the input is not valid.
  */
  const class MYON_SubDEdge* SpinEdge(
    class MYON_SubDEdge* edge,
    bool spin_clockwise = false
  );
  
  /*
  Description:
    Spin an edge's endpoints around the boundary of its neighboring faces.
    In a counter-clockwise spin (looking at faces from their shared up orientation):
      The edge's start vertex is moved to the next vertex in the boundary
      of the face on the right-hand side of the edge.
      The edge's end vertex is moved to the next vertex in the boundary
      of the face on the left-hand side of the edge.
    Note that reversing the input edge does not change the result.
  Parameters:
    edge - [in]
      edge to spin.
    spin_clockwise - [in]
      false spins the edge counter-clockwise, true spins the edge clockwise
      in the adjacent faces.
  Returns:
    A pointer to the spun edge or nullptr if the input is not valid.
  */
  const MYON_SubDEdgePtr SpinEdge(
    MYON_SubDEdgePtr eptr,
    bool spin_clockwise = false
  );

  /*
  Description:
    Divide a face into two faces by inserting an edge connecting the specified vertices.
  Parameters:
    face - [in]
      A face with at least four edges.
    fvi0 - [in]
    fvi1 - [in]
      Face vertex indices of the inserted edge's ends.
  Returns:
    A pointer to the inserted edge.
    The inserted edge runs from face->Vertex(fvi0) to face->Vertex(fvi1).
    MYON_SubDEdge.Face(0) is the original face and MYON_SubDEdge::Face(1) is 
    the added face.
    The first edge of both faces is the inserted edge.
  */
  const class MYON_SubDEdge* SplitFace(
    class MYON_SubDFace* face,
    unsigned int fvi0,
    unsigned int fvi1
    );

  /*
  Description:
    Divide a face into two faces by inserting an edge connecting the specified vertices.
  Parameters:
    face - [in]
      A face with at least four edges.
    v0 - [in]
    v1 - [in]
      Face vertices of the inserted edge's ends.
  Returns:
    A pointer to the inserted edge.
    The inserted edge runs from v0 to v1.
    MYON_SubDEdge.Face(0) is the original face and MYON_SubDEdge::Face(1) is 
    the added face.
    The first edge of the input face remains the first edge of face.  
    The inserted edge is the first edge of the added face.
  */
  const class MYON_SubDEdge* SplitFace(
    class MYON_SubDFace* face,
    const class MYON_SubDVertex* v0,
    const class MYON_SubDVertex* v1
    );


  /*
  Description:
    Divide a face into two faces by inserting an edge connecting the specified vertices.
  Parameters:
    face - [in]
      A face with at least four edges.
    fvi0 - [in]
    fvi1 - [in]
      Face vertex indices of the inserted edge's ends.
    new_face_side - [in]
      0: The new face will be on the left side of the inserted edge.
      0: The new face will be on the right side of the inserted edge.
  Returns:
    The edge and edgeptr are both being af both oriented 
    The inserted edge and returned edge ptr runs from face->Vertex(fvi0) to face->Vertex(fvi1).
    the new face is SplitFace(...).RelativeFace(new_face_side) and the original face is SplitFace(...).RelativeFace(new_face_side).
  */
  const MYON_SubDEdgePtr SplitFace(
    class MYON_SubDFace* face,
    unsigned int fvi0,
    unsigned int fvi1,
    unsigned new_face_side
  );

  /*
  Description:
    Divide a face into two faces by inserting an edge connecting the specified vertices.
  Parameters:
    face - [in]
      A face with at least four edges.
    v0 - [in]
    v1 - [in]
      Face vertices of the inserted edge's ends.
    new_face_side - [in]
      0: The new face will be on the left side of the inserted edge.
      0: The new face will be on the right side of the inserted edge.
  Returns:
    The edge and edgeptr are both being af both oriented
    The inserted edge and returned edge ptr runs from face->Vertex(fvi0) to face->Vertex(fvi1).
    the new face is SplitFace(...).RelativeFace(new_face_side) and the original face is SplitFace(...).RelativeFace(new_face_side).
  */
  const MYON_SubDEdgePtr SplitFace(
    class MYON_SubDFace* face,
    const class MYON_SubDVertex* v0,
    const class MYON_SubDVertex* v1,
    unsigned new_face_side
  );

  /*
  Description:
    Replace a face with a triangle fan  by adding a single new vertex at fan_center_point 
    and adding tringle made from the face's edes to the center point.
  Parameters:
    face - [in]
      This face is replaced with a triangle fan and becomes the first triangle in the fan.
    fan_center_point - [in]
      If valid, this point is used as the fan's center.
      Otherwise the centriod of the face's vertices is used s the fan's center.
      When in doubt, pass MYON_3dPoint::UnsetPoint.
    bMarkFaces - [in]
      If true, face and new triangles are marked.
      Existing marks are not modified.
  Returns:
    If successful, the new vertex at the center of the triangle fan.
    Otherwise, nullptr is returned.
  */
  const class MYON_SubDVertex* ReplaceFaceWithTriangleFan(
    class MYON_SubDFace* face,
    MYON_3dPoint fan_center_point,
    bool bMarkFaces
    );



  /*
  Description:
    When finished editing a SubD, call this function to delete all cached evaluation
    values and update all vertex tag, edge tags, edge sector coefficients.
  Remarks:
    This function is the most reliable (and heavy handed) way to update SubD component
    information. Expert users can choose to be more selective when certain about 
    exactly what needs to be modified.
  */
  void SubDModifiedNofification();


  /*
  Description:
    Updates vertex tag, edge tag, and edge coefficient values
    on the active level.

    After completing custom editing operations that modify the
    topology of the SubD control net or changing values of
    vertex or edge tags, the tag and sector coefficients
    information on nearby components in the edited areas
    need to be updated.

  Parameters:
    bUnsetValuesOnly - [in]
      If true, the update is restricted to vertices tagged as 
      MYON_SubDVertexTag::Unset and edges tagged as MYON_SubDEdgeTag::Unset.

  Returns:
    Number of vertices and edges that were changed during the update.
  */
  unsigned int UpdateAllTagsAndSectorCoefficients(
    bool bUnsetValuesOnly
  );

  /*
  Description:
    This tool if for expert users writing advanced editing tools.
    After completing custom editing operations that modify the
    topology of the SubD control net or changing values of
    vertex or edge tags, the tag and sector coefficients
    information on nearby components in the edited areas
    need to be updated.
  Parameters:
    bUnsetTagsOnly - [in]
      If bUnsetTagsOnly is true, then only unset tags and
      ill be updated.
      If bUnsetTagsOnly is false, then all tags and
      will be checked and updated as needed.
  Returns:
    Number of vertices that changed during the update.
  Remarks:
    It is easiest to call UpdateAllTagsAndSectorCoefficients().
  */
  unsigned int UpdateVertexTags(
    bool bUnsetVertexTagsOnly
    );

  /*
  Description:
    This tool if for expert users writing advanced editing tools.
    After completing custom editing operations that modify the
    topology of the SubD control net or changing values of
    vertex or edge tags, the tag and sector coefficients
    information on nearby components in the edited areas
    need to be updated.
  Parameters:
    bUnsetValuesOnly - [in]
      If bUnsetValuesOnly is true, then only unset tags and
      sector coefficients will be updated.
      If bUnsetValuesOnly is false, then all tags and
      sector coefficients will be checked and updated as needed.
  Returns:
    Number of edges that had a tag value changed or sector
    coefficient set to MYON_SubDSectorType::UnsetSectorCoefficient.
  Remarks:
    It is easiest to call UpdateAllTagsAndSectorCoefficients().
  */
  unsigned int UpdateEdgeTags(
    bool bUnsetEdgeTagsOnly
    );

  /*
  Description:
    This tool if for expert users writing advanced editing tools.
    After completing custom editing operations that modify the
    topology of the SubD control net or changing values of
    vertex or edge tags, the tag and sector coefficients
    information on nearby components in the edited areas
    need to be updated.
  Parameters:
    bUnsetValuesOnly - [in]
      If bUnsetValuesOnly is true, then only unset tags and
      sector coefficients will be updated.
      If bUnsetValuesOnly is false, then all tags and
      sector coefficients will be checked and updated as needed.
  Returns:
    Number of edges that had a tag value changed or sector
    coefficient set to MYON_SubDSectorType::UnsetSectorCoefficient.
  Remarks:
    It is easiest to call UpdateAllTagsAndSectorCoefficients().
  */
  unsigned int UpdateEdgeSectorCoefficients(
    bool bUnsetSectorCoefficientsOnly
    ) const;


  /*
  Descripiton:
    Sets the m_group_id value to 0 for every vertex, edge and face.
  Returns:
    Number of marks that were cleared.
  */
  unsigned int ClearGroupIds() const;

  /*
  Descripiton:
    Sets the m_group_id value to 0 for every vertex.
  Returns:
    Number of group id values that were changed.
  */
  unsigned int ClearVertexGroupIds() const;

  /*
  Descripiton:
    Sets the m_group_id value to 0 for every edge.
  Returns:
    Number of group id values that were changed.
  */
  unsigned int ClearEdgeGroupIds() const;

  /*
  Descripiton:
    Sets the m_group_id value to 0 for every face.
  Returns:
    Number of group id values that were changed.
  */
  unsigned int ClearFaceGroupIds() const;

  /*
  Descripiton:
    Sets the m_group_id value to 0 for the specified components.
  Parameters:
    bClearVertexGroupIds - [in]
      If true, m_group_id for every vertex is set to zero.
    bClearEdgeGroupIds - [in]
      If true, m_group_id for every edge is set to zero.
    bClearFaceGroupIds - [in]
      If true, m_group_id for every face is set to zero.
  Returns:
    Number of group id values that were changed.
  */
  unsigned int ClearComponentGroupIds(
    bool bClearVertexGroupIds,
    bool bClearEdgeGroupIds,
    bool bClearFaceGroupIds
  ) const;


  /*
  Descripiton:
    Sets the m_status.MarkBits() value to 0 for every vertex, edge and face.
  Returns:
    Number of marks that were cleared.
  */
  unsigned int ClearMarkBits() const;

  /*
  Descripiton:
    Sets the m_status.MarkBits() value to 0 for every vertex.
  Returns:
    Number of group id values that were changed.
  */
  unsigned int ClearVertexMarkBits() const;

  /*
  Descripiton:
    Sets the m_status.MarkBits() value to 0 for every edge.
  Returns:
    Number of group id values that were changed.
  */
  unsigned int ClearEdgeMarkBits() const;

  /*
  Descripiton:
    Sets the m_status.MarkBits() value to 0 for every face.
  Returns:
    Number of group id values that were changed.
  */
  unsigned int ClearFaceMarkBits() const;

  /*
  Descripiton:
    Sets the m_status.MarkBits() value to 0 for the specified components.
  Parameters:
    bClearVertexMarkBits - [in]
      If true, m_status.MarkBits() for every vertex is set to zero.
    bClearEdgeMarkBits - [in]
      If true, m_status.MarkBits() for every edge is set to zero.
    bClearFaceMarkBits - [in]
      If true, m_status.MarkBits() for every face is set to zero.
  Returns:
    Number of group id values that were changed.
  */
  unsigned int ClearComponentMarkBits(
    bool bClearVertexMarkBits,
    bool bClearEdgeMarkBits,
    bool bClearFaceMarkBits
  ) const;

  /*
  Descripiton:
    Clears the m_status.RuntimeMark() for every vertex, edge and face.
  Returns:
    Number of marks that were cleared.
  */
  unsigned int ClearComponentMarks() const;

  /*
  Descripiton:
    Clears the m_status.RuntimeMark() for every vertex.
  Returns:
    Number of marks that were cleared.
  */
  unsigned int ClearVertexMarks() const;

  /*
  Descripiton:
    Clears the m_status.RuntimeMark() for every edge.
  Returns:
    Number of marks that were cleared.
  */
  unsigned int ClearEdgeMarks() const;

  /*
  Descripiton:
    Clears the m_status.RuntimeMark() for every face.
  Returns:
    Number of marks that were cleared.
  */
  unsigned int ClearFaceMarks() const;

  /*
  Descripiton:
    Selectively clear m_status.RuntimeMark()
  Parameters:
    bClearVertexMarks - [in]
      If true, m_status.ClearRuntimeMark() is called for every vertex.
    bClearEdgeMarks - [in]
      If true, m_status.ClearRuntimeMark() is called for every edge.
    bClearFaceMarks - [in]
      If true, m_status.ClearRuntimeMark() is called for every face.
    marked_component_list - [out]
      If not nullptr, then pointer to components that were marked 
      are returned in this marked_component_list[]
  Returns:
    Number of marks that were cleared.
  */
  unsigned int ClearComponentMarks(
    bool bClearVertexMarks,
    bool bClearEdgeMarks,
    bool bClearFaceMarks,
    MYON_SimpleArray< const class MYON_SubDComponentBase* >* marked_component_list
  ) const;

  unsigned int SetComponentMarks(
    bool bClearBeforeSet,
    const MYON_SimpleArray< const class MYON_SubDComponentBase* >& marked_component_list
  ) const;

  unsigned int GetMarkedComponents(
    bool bIncludeVertices,
    bool bIncludeEdges,
    bool bIncludeFaces,
    MYON_SimpleArray< const class MYON_SubDComponentBase* >& marked_component_list
  ) const;

  /*
  Parameters:
    bAddMarkedComponents - [in]
      If true, marked components are added to component_list[].
      If false, unmarked components are added to component_list[].
    mark_bits - [in]
      If mark_bits is zero, then a component is "marked" if component.Mark() is true.
      Otherwise a component is "marked" if mark_bits = component.MarkBits().
   */
   unsigned int GetMarkedComponents(
    bool bAddMarkedComponents,
    MYON__UINT8 mark_bits,
    bool bIncludeVertices,
    bool bIncludeEdges,
    bool bIncludeFaces,
    MYON_SimpleArray< class MYON_SubDComponentPtr >& component_list
  ) const;

  unsigned int UnselectComponents(
    bool bUnselectAllVertices,
    bool bUnselectAllEdges,
    bool bUnselectAllFaces
  ) const;

  /*
  Description:
    Save the current component status of the indictated subd components.
  Parameters:
    bGetVertexStatus - [in]
    bGetEdgeStatus - [in]
    bGetFaceStatus - [in]
    bClearStatus - [in]
      If true, the bits in status_mask will also be cleared for the components.
    status_mask - [in]
      Status bits to save.
    component_list - [out]
    status_list - [out]
      component_list[] and status_list[] are parallel arrays for components with
      a matching status bit set.
  */
  unsigned int GetComponentStatus(
    bool bGetVertexStatus,
    bool bGetEdgeStatus,
    bool bGetFaceStatus,
    bool bClearStatus,
    MYON_ComponentStatus status_mask,
    MYON_SimpleArray< const class MYON_SubDComponentBase* >& component_list,
    MYON_SimpleArray< MYON_ComponentStatus >& status_list
  ) const;

  unsigned int SetComponentStatus(
    MYON_ComponentStatus status_mask,
    const MYON_SimpleArray< const class MYON_SubDComponentBase* >& component_list,
    const MYON_SimpleArray< MYON_ComponentStatus >& status_list
  ) const;

  /*
  Description:
    Transforms the SubD components in ci_list[].
  Parameters:
    xform - [in]
    ci_list - [in]
    ci_count - [in]
    component_location - [in]
      Select between applying the transform to the control net (faster)
      or the surface points (slower).
  Returns:
    Number of vertex locations that changed.
  */
  unsigned int TransformComponents(
    const MYON_Xform& xform,
    const MYON_COMPONENT_INDEX* ci_list,
    size_t ci_count,
    MYON_SubDComponentLocation component_location
  );

  unsigned int TransformComponents(
    const MYON_Xform& xform,
    const MYON_SubDComponentPtr* cptr_list,
    size_t cptr_count,
    MYON_SubDComponentLocation component_location
  );

  /*
  Description:
    Extrude entire subd bay adding a ring of faces around the boundary and moving the original subd.
  */
  unsigned int Extrude(
    const MYON_Xform& xform
  );

  unsigned int ExtrudeComponents(
    const MYON_Xform& xform,
    const MYON_COMPONENT_INDEX* ci_list,
    size_t ci_count
  );

  unsigned int ExtrudeComponents(
    const MYON_Xform& xform,
    const MYON_SubDComponentPtr* cptr_list,
    size_t cptr_count
    );

  unsigned int ExtrudeComponents(
    const MYON_Xform& xform,
    const MYON_COMPONENT_INDEX* ci_list,
    size_t ci_count,
    bool bExtrudeBoundaries,
    bool bPermitNonManifoldEdgeCreation
  );

  unsigned int ExtrudeComponents(
    const MYON_Xform& xform,
    const MYON_SubDComponentPtr* cptr_list,
    size_t cptr_count,
    bool bExtrudeBoundaries,
    bool bPermitNonManifoldEdgeCreation
  );

private:
  unsigned int Internal_ExtrudeComponents(
    const MYON_Xform& xform,
    const MYON_SubDComponentPtr* cptr_list,
    size_t cptr_count,
    bool bExtrudeBoundaries,
    bool bPermitNonManifoldEdgeCreation
  );

public:

  /*
  Parameters:
    ci_list - [in]
      Array of ci_count MYON_COMPONENT_INDEX values that identify the vertices. 
      Elements that do not identify a SubD vertex are ignored.
    ci_count - [in]
      Number of elements in the ci_list[] array.
    vertex_tag - [in]
      Desired tag. If a vertex has the desired tag or cannot accept the desired tag, 
      then that vertex is skipped. 
      If vertex_tag is MYON_SubDVertexTag::Corner, then every edge touching 
      that vertex is converted to a crease.
  Returns:
    number of vertex tags that were changed.
  */
  unsigned int SetVertexTags(
    const MYON_COMPONENT_INDEX* ci_list,
    size_t ci_count,
    MYON_SubDVertexTag vertex_tag
  );

  /*
  Parameters:
    ci_list - [in]
      Array of ci_count MYON_COMPONENT_INDEX values that identify the vertices. 
      Elements that do not identify a SubD vertex are ignored.
    ci_count - [in]
      Number of elements in the ci_list[] array.
    vertex_tag - [in]
      Desired tag. If a vertex has the desired tag or cannot accept the desired tag, 
      then that vertex is skipped.
      If vertex_tag is MYON_SubDVertexTag::Corner, then every edge touching 
      that vertex is converted to a crease.
  Returns:
    number of vertex tags that were changed.
  */
  unsigned int SetVertexTags(
    const MYON_SubDComponentPtr* cptr_list,
    size_t cptr_count,
    MYON_SubDVertexTag vertex_tag
  );

  /*
  Returns:
    number of tags that were changed.
  */
  unsigned int SetEdgeTags(
    const MYON_COMPONENT_INDEX* ci_list,
    size_t ci_count,
    MYON_SubDEdgeTag edge_tag
  );

  /*
  Returns:
    number of tags that were changed.
  */
  unsigned int SetEdgeTags(
    const MYON_SubDComponentPtr* cptr_list,
    size_t cptr_count,
    MYON_SubDEdgeTag edge_tag
  );

  /*
  Returns:
    number of tags that were changed.
  */
  unsigned int SetEdgeTags(
    const MYON_SimpleArray<MYON_SubDComponentPtr>& cptr_list,
    MYON_SubDEdgeTag edge_tag
  );


  /*
  Description:
    Remove all interior creases.
  Returns:
    Number of edges converted from crease to smooth.
  */
  unsigned int RemoveAllCreases();

  /*
  Description:
    Apply the Catmull-Clark subdivision algorithm and save the results
    in this MYON_SubD.
  Parameters:
    level_index - [in]
      Level where subdivision starts
    count - [in] > 0
      Number of times to subdivide.
  Returns:
    Number of subdivision steps that succeeded. 
    (= count when everything works, < count when input is not valid)
  */
  bool GlobalSubdivide(
    unsigned int count
    );

  bool GlobalSubdivide();

  /// <returns>
  /// The number of quads a call to GlobalSubdivide() will create.
  ///< / returns>  
  unsigned int GlobalSubdivideQuadCount() const;

  /// <returns>
  /// The number of quads a call to GlobalSubdivide(subdivision_count) will create.
  ///< / returns>  
  unsigned int GlobalSubdivideQuadCount(unsigned int subdivision_count) const;

  /*
  Description:
    Apply the Catmull-Clark subdivision algorithm to the faces in face_list[].
  Parameters:
    face_list - [in]
      faces to subdivide
    face_count - [in]
      number of components.
  Returns:
    true if successful.
  */
  bool LocalSubdivide(
    class MYON_SubDFace const*const* face_list,
    size_t face_count
  );

  bool LocalSubdivide(
    const MYON_SimpleArray< const class MYON_SubDFace* >& face_list
  );

  bool LocalSubdivide(
    const MYON_SimpleArray<MYON_COMPONENT_INDEX>& face_list
  );

  /*
  Description:
    Adds a vertex with tag = MYON_SubDVertexTag::Unset.
  */
  class MYON_SubDVertex* AddVertex(
    const double* P
    );

  /*
  Description:
    Adds a vertex with specified tag.
  */
  class MYON_SubDVertex* AddVertex(
    MYON_SubDVertexTag vertex_tag,
    const double* P
    );

  /*
  Description:
    Expert user tool to add a vertex with specified information. This function
    is useful when copying portions of an existing SubD to a new SubD.
  Parameters:
    candidate_vertex_id - [in]
      If candidate_vertex_id is > 0 and is available, 
      the returned value with have id = candidate_vertex_id.
      Otherwise a new id will be assigned.
    vertex_tag - [in]
      Pass MYON_SubDVertexTag::Unset if not known.
    P - [in]
      nullptr or a 3d point.
   initial_edge_capacity - [in]
     Initial capacity of the m_edges[] array. Pass 0 if unknown.
   initial_face_capacity - [in]
     Initial capacity of the m_faces[] array. Pass 0 if unknown.
  */
  class MYON_SubDVertex* AddVertexForExperts(
    unsigned int candidate_vertex_id,
    MYON_SubDVertexTag vertex_tag,
    const double* P,
    unsigned int initial_edge_capacity,
    unsigned int initial_face_capacity
    );

  /*
  Parameters:
    v - [in]
      A vertex with zero edge and zero faces.
  */
  bool ReturnVertexForExperts(
    MYON_SubDVertex* v
  );

  /*
  Description:
    Search for a vertex with a specified control net point.
  Parameters:
    control_net_point - [in]
      Look for a vertex with this value for ControlNetPoint().
    tolerance - [in]
      Use 0.0 when in doubt. 
      If > 0.0, then the vertex closest to control_net_point 
      will be returned if the distance from that vertex to control_net_point
      is <= distance_tolerance.
  Returns:
    An MYON_SubDVertex pointer or nullptr if none exists.
  */
  const class MYON_SubDVertex* FindVertex(
    const double* control_net_point,
    double distance_tolerance
  ) const;

  /*
  Description:
    Search for a vertex with a specified control net point. If one does not
    exist, add a new one.
  Parameters:
    control_net_point - [in]
      Look for a vertex with this value for ControlNetPoint().
    tolerance - [in]
      Use 0.0 when in doubt.
      If > 0.0, then the vertex closest to control_net_point
      will be returned if the distance from that vertex to control_net_point
      is <= distance_tolerance.
  Returns:
    An MYON_SubDVertex pointer or nullptr if none exists.
    */
  const class MYON_SubDVertex* FindOrAddVertex(
    const double* control_net_point,
    double distance_tolerance
  );

  /*
  Parameters:
    edge_face_count - [in]
      Number of faces the edge will eventually have.
      Pass 0 if the value is not known.
    v0 - [in]
      starting vertex
    v1 - [in]
      ending vertex
  Returns:
    If edge_face_count > 0x7FFFU, then MYON_SubDEdgeTag::Unset is returned.

    If edge_face_count is 1 or >= 3, then MYON_SubDEdgeTag::Crease is returned.

    If both vertex tags are MYON_SubDVertexTag::Smooth, then MYON_SubDEdgeTag::Smooth is returned.
   
    If edge_face_count is 1 and both vertex tags are MYON_SubDVertexTag::Crease or MYON_SubDVertexTag::Corner,
    then MYON_SubDEdgeTag::Crease is returned.
   
    If edge_face_count is 2 and both vertex tags are set and both are not MYON_SubDVertexTag::Smooth,
   then MYON_SubDEdgeTag::SmoothX is returned.

   Otherwise, MYON_SubDEdgeTag::Unset is returned.
  */
  static MYON_SubDEdgeTag EdgeTagFromContext(
    unsigned int edge_face_count,
    const MYON_SubDVertexTag v0_tag,
    const MYON_SubDVertexTag v1_tag
  );

  static MYON_SubDEdgeTag EdgeTagFromContext(
    unsigned int edge_face_count,
    const MYON_SubDVertex* v0,
    const MYON_SubDVertex* v1
  );

  /*
  Description:
    Add an edge with tag = MYON_SubDEdgeTag::Unset to the subd.
  Parameters:
    v0 - [in]
    v1 - [in]
      The edge begins at v0 and ends at v1.
      The edge will be on the same level as the vertices.
  Returns:
    Pointer to the allocated edge.
  Remarks:
    MYON_SubD::EdgeTagFromContext() can be used to determine edge
    tag values in simple situations.
  */
  class MYON_SubDEdge* AddEdge(
    class MYON_SubDVertex* v0,
    class MYON_SubDVertex* v1
    );

  /*
  Description:
    Search for an edge connecting v0 and v1.
  Parameters:
    v0 - [in]
    v1 - [in]
      The edge begins at v0 and ends at v1.
      The edge will be on the same level as the vertices.
  Returns:
    An MYON_SubDEdgePtr to a connecting edge or MYON_SubDEdgePtr::Null if none exists.
  */
  const MYON_SubDEdgePtr FindEdge(
    const class MYON_SubDVertex* v0,
    const class MYON_SubDVertex* v1
    ) const;

  /*
  Description:
    Search for an edge connecting v0 and v1. If none exists, then add one.
  Parameters:
    v0 - [in]
    v1 - [in]
      The edge begins at v0 and ends at v1.
      The edge will be on the same level as the vertices.
  Returns:
    An MYON_SubDEdgePtr to a connecting edge or MYON_SubDEdgePtr::Null if none exists.
  */
  const MYON_SubDEdgePtr FindOrAddEdge(
    class MYON_SubDVertex* v0,
    class MYON_SubDVertex* v1
    );


  /*
  Description:
    Add an edge to the subd.
  Parameters:
    edge_tag - [in]
      MYON_SubDEdgeTag::Unset
        Edge tag is not known at this time.
      MYON_SubDEdgeTag::Smooth
        Smooth edge. If both vertices are tagged as not smooth, the
        tag on the returned edge will be MYON_SubDEdgeTag::SmoothX.  This
        tag is changed to MYON_SubDEdgeTag::Smooth on the first
        subdivision step.
      MYON_SubDEdgeTag::Crease.
        Crease edge.  Both vertices must be tagged as not smooth.
    v0 - [in]
    v1 - [in]
      The edge begins at v0 and ends at v1.
      The edge will be on the same level as the vertices.
  Returns:
    Pointer to the allocated edge.
  Remarks:
    MYON_SubD::EdgeTagFromContext() can be used to determine edge
    tag values in simple situations.
  */
  class MYON_SubDEdge* AddEdge(
    MYON_SubDEdgeTag edge_tag,
    class MYON_SubDVertex* v0,
    class MYON_SubDVertex* v1
    );

  /*
  Description:
    Add an edge to the subd.
  Parameters:
    edge_tag - [in]
      MYON_SubDEdgeTag::Unset
        Edge tag is not known at this time.
      MYON_SubDEdgeTag::Smooth
        Smooth edge. If both vertices are tagged as not smooth, the
        tag on the returned edge will be MYON_SubDEdgeTag::SmoothX.  This
        tag is changed to MYON_SubDEdgeTag::Smooth on the first
        subdivision step.
      MYON_SubDEdgeTag::Crease.
        Crease edge.  Both vertices must be tagged as not smooth.
    v0 - [in]
    v1 - [in]
      The edge begins at v0 and ends at v1.
      The edge will be on the same level as the vertices.
    sharpness - [in]
      If edge_tag is MYON_SubDEdge::Smooth or MYON_SubDEdge::SmoothX, then
      the the edge's sharpness is set to sharpness.
      Otherwise, the sharpness parameter is ignored.
  Returns:
    Pointer to the allocated edge.
  Remarks:
    MYON_SubD::EdgeTagFromContext() can be used to determine edge
    tag values in simple situations.
  */
  class MYON_SubDEdge* AddEdge(
    MYON_SubDEdgeTag edge_tag,
    class MYON_SubDVertex* v0,
    class MYON_SubDVertex* v1,
    MYON_SubDEdgeSharpness sharpness
  );

  /*
  Description:
    Expert use tool to add an edge with precomputed sector coefficients.
  Parameters:
    edge_tag - [in]
      This expert user function does not automatically set the edge tag.
    v0 - [in]
      The edge begins at v0 and ends at v1.
    v0_sector_coefficient - [in]
      Pass MYON_SubDSectorType::UnsetSectorCoefficient if unknown.
    v1 - [in]
      The edge begins at v0 and ends at v1.
    v1_sector_coefficient - [in]
      Pass MYON_SubDSectorType::UnsetSectorCoefficient if unknown.
  */
  class MYON_SubDEdge* AddEdgeWithSectorCoefficients(
    MYON_SubDEdgeTag edge_tag,
    class MYON_SubDVertex* v0,
    double v0_sector_coefficient,
    class MYON_SubDVertex* v1,
    double v1_sector_coefficient
  );

  /*
  Description:
    Expert user tool to add an edge with specified information. This function
    is useful when copying portions of an existing SubD to a new SubD.
  Parameters:
    candidate_edge_id - [in]
      If candidate_edge_id is > 0 and is available,
      the returned edge with have id = candidate_edge_id.
      Otherwise a new id will be assigned.
    edge_tag - [in]
      Pass MYON_SubDEdgeTag::Unset if not known.
    v0 - [in]
      The edge begins at v0 and ends at v1.
    v0_sector_coefficient - [in]
      Pass MYON_SubDSectorType::UnsetSectorCoefficient if unknown.
    v1 - [in]
      The edge begins at v0 and ends at v1.
    v1_sector_coefficient - [in]
      Pass MYON_SubDSectorType::UnsetSectorCoefficient if unknown.
   initial_face_capacity - [in]
     Initial face capacity. Pass 0 if unknown.
  */
  class MYON_SubDEdge* AddEdgeForExperts(
    unsigned int candidate_edge_id,
    MYON_SubDEdgeTag edge_tag,
    class MYON_SubDVertex* v0,
    double v0_sector_coefficient,
    class MYON_SubDVertex* v1,
    double v1_sector_coefficient,
    unsigned int initial_face_capacity
  );

  /*
  Parameters:
    e - [in]
      An edge in this subd with no vertices or faces.
  */
  bool ReturnEdgeForExperts(
    MYON_SubDEdge* e
  );

  /*
  Parameters:
    edge0 - [in]
    edge1 - [in]
    edge2 - [in]
     The face will be oriented so the boundary has the edges
     in the order (edge0, edge1, edge2).
     Consecutive edges must have a single common vertex.
     The orientations of the MYON_SubDEdgePtr elements in the 
     face's edge list are automatically calculated.
  Returns:
    A pointer to the added face. 
    The returned face is managed by the subd.
  */
  class MYON_SubDFace* AddTriangleFace(
    class MYON_SubDEdge* edge0,
    class MYON_SubDEdge* edge1,
    class MYON_SubDEdge* edge2
    );

  /*
  Parameters:
    edge_count - [in]
      Must be >= 3.
    edge0 - [in]
    edge1 - [in]
    edge2 - [in]
     The MYON_SubDEdgePtr parameters must be oriented so that for consecutive pairs of edges, 
     edge0.RelativeVertex(1) and edges1.RelativeVertex(0) are same vertex. 
     The face will be oriented so the boundary has the edges
     in the order (edge0, edge1, edge2).
  Returns:
    A pointer to the added face. 
    The returned face is managed by the subd.
  */
  class MYON_SubDFace* AddTriangleFace(
    MYON_SubDEdgePtr edge0,
    MYON_SubDEdgePtr edge1,
    MYON_SubDEdgePtr edge2
    );

  /*
  Parameters:
    edge0 - [in]
    edge1 - [in]
    edge2 - [in]
    edge3 - [in]
     The face will be oriented so the boundary has the edges
     in the order (edge0, edge1, edge2. edge3).
     Consecutive edges must have a single common vertex.
     The orientations of the MYON_SubDEdgePtr elements in the 
     face's edge list are automatically calculated.
  Returns:
    A pointer to the added face. 
    The returned face is managed by the subd.
  */
  class MYON_SubDFace* AddQuadFace(
    class MYON_SubDEdge* edge0,
    class MYON_SubDEdge* edge1,
    class MYON_SubDEdge* edge2,
    class MYON_SubDEdge* edge3
    );
 
  /*
  Parameters:
    edge_count - [in]
      Must be >= 3.
    edge0 - [in]
    edge1 - [in]
    edge2 - [in]
    edge3 - [in]
     The MYON_SubDEdgePtr parameters must be oriented so that for consecutive pairs of edges, 
     edge0.RelativeVertex(1) and edges1.RelativeVertex(0) are same vertex. 
     The face will be oriented so the boundary has the edges
     in the order (edge0, edge1, edge2,edge3).
  Returns:
    A pointer to the added face. 
    The returned face is managed by the subd.
  */
  class MYON_SubDFace* AddQuadFace(
    MYON_SubDEdgePtr edge0,
    MYON_SubDEdgePtr edge1,
    MYON_SubDEdgePtr edge2,
    MYON_SubDEdgePtr edge3
    );

  /*
  Parameters:
    edges[] - [in]
     edges[] must have 3 or more elements.
     edges[i] and edges[(i+1)%edge_count] must have a single common vertex.
     The face will be oriented so the boundary has the edges
     in the order (edges[0], edges[1], ..., edges[edge_count-1]).
     The orientations of the MYON_SubDEdgePtr elements in the 
     face's edge list are automatically calculated.
  Returns:
    A pointer to the added face. 
    The returned face is managed by the subd.
  */
  class MYON_SubDFace* AddFace(
    const MYON_SimpleArray<MYON_SubDEdge*>& edges
    );

  /*
  Parameters:
    edges[] - [in]
     edges[] must have 3 or more elements.
     The MYON_SubDEdgePtr parameters must be oriented so that
     edges[i].RelativeVertex(1) and edges[(i+1)%edge_count].RelativeVertex(0)
     are the same vertex.
     The face will be oriented so the boundary has the edges
     in the order (edges[0], edges[1], ..., edges[edge_count-1]).
  Returns:
    A pointer to the added face. 
    The returned face is managed by the subd.
  */
  class MYON_SubDFace* AddFace(
    const MYON_SimpleArray<MYON_SubDEdgePtr>& edges
    );

  /*
  Parameters:
    edge_count - [in]
      Must be >= 3.
    edges[] - [in]
     edges[i] and edges[(i+1)%edge_count] must have a single common vertex.
     The face will be oriented so the boundary has the edges
     in the order (edges[0], edges[1], ..., edges[edge_count-1]).
     The orientations of the MYON_SubDEdgePtr elements in the 
     face's edge list are automatically calculated.
  Returns:
    A pointer to the added face. 
    The returned face is managed by the subd.
  */
  class MYON_SubDFace* AddFace(
    class MYON_SubDEdge * const * edges,
    unsigned int edge_count
    );

  /*
  Parameters:
    edge_count - [in]
      Must be >= 3.
    edges[] - [in]
     The MYON_SubDEdgePtr parameters must be oriented so that
     edges[i].RelativeVertex(1) and edges[(i+1)%edge_count].RelativeVertex(0)
     are the same vertex.
     The face will be oriented so the boundary has the edges
     in the order (edges[0], edges[1], ..., edges[edge_count-1]).
  Returns:
    A pointer to the added face. 
    The returned face is managed by the subd.
  */
  class MYON_SubDFace* AddFace(
    const class MYON_SubDEdgePtr* edges,
    unsigned int edge_count
    );

  /*
  Description:
    Finds an existing face or adds a new face with corners at face_vertices[].
  Parameters:
    new_edge_tag - [in]
      If an edge needs to be added, this tag is assigned to the new edge.
      When in doubt, pass MYON_SubDEdgeTag::Unset and call this->UpdateAllTagsAndSectorCoefficients(true)
      after you are finished modifying the SubD.
    face_vertices - [in]
      Array of vertices at the face corners in counter-clockwise order.
    face_vertex_count - [in]
      Number of vertices in face_vertices[]
  Returns:
    If the input is not valid, nullptr is returned.
    If there is a face with the specified corners exists, then it is returned.
    Otherwise a new face is added and returned.
  */
  class MYON_SubDFace* FindOrAddFace(
    MYON_SubDEdgeTag new_edge_tag,
    const class MYON_SubDVertex* face_vertices[],
    size_t face_vertex_count
  );

  /*
  Parameters:
    candidate_face_id - [in]
      If candidate_face_id is > 0 and is available,
      the returned face with have id = candidate_face_id.
      Otherwise a new id will be assigned.
    edge[] - [in]
      The edge[] array must be sorted and correct oriented
      (edge[i].RelativeVertex(1) == edge[(i+1)%edge_count].RelativeVertex(0)).
    edge_count - [in]
      Must be >= 3.
  */
  class MYON_SubDFace* AddFaceForExperts(
    unsigned candidate_face_id,
    const class MYON_SubDEdgePtr* edge,
    unsigned int edge_count
    );

  /*
  Parameters:
    f - [in]
      A face with zero edges
  */
  bool ReturnFaceForExperts(
    MYON_SubDFace* f
  );

  /*
  Description:
    Add texture points to a face.
  Parameters:
    texture_points - [in]
      An array of face->EdgeCount() points.
    texture_points_count - [in]
      number of elements in texture_points[].
      Must be >= face->EdgeCount().
  Returns:
    True if texture points were set.
  Remarks:
    This function automatically handles the management of face texture point storage.
    Texture points are a mutable property on MYON_SubDFace.
  */
  bool AddFaceTexturePoints(
    const class MYON_SubDFace* face,
    const class MYON_3dPoint* texture_points,
    size_t texture_points_count
  ) const;

  /*
  Description:
    Add texture point storage capacity to a face.
  Parameters:
    face - [in]
      The ability to store at least face->EdgeCount() texture points will be added to this face.
  Returns:
    Number of texture points that can be set (>= face->EdgeCount()).
  */
  unsigned int AllocateFaceTexturePoints(
    const class MYON_SubDFace* face
  ) const;

public:

#pragma region RH_C_SHARED_ENUM [MYON_SubD::PatchStyle] [Rhino.Geometry.SubDPatchStyle] [byte]
  /// <summary>
  /// SubD::PatchStyle identifies the style of patch used to fill holes.
  /// </summary>
  enum class PatchStyle : unsigned char
  {
    ///<summary>
    /// Not a valid style.
    /// This encourages developers to thoughtfully select a patch style and can 
    /// be used to indicate a UI control is not initialized.
    ///</summary>
    Unset = 0,

    ///<summary>
    /// Automatically choose a patch style that will generally create a good looking result.
    /// If a hole boundary is not convex, it is triangulated. Otherwise:
    /// If a hole has 3 edges, a single triangle face is used.
    /// If a hole has 4 edges, a single quad face is used.
    /// If a hole has 5 or more edges and an odd number of edges, a triangle fan is used. 
    /// If a hole has 6 or more edges and an even number of edges, a quad fan is used. 
    ///</summary>
    Automatic = 1, 

    ///<summary>
    /// A single face is used under all conditions.
    ///</summary>
    SingleFace = 2,

    ///<summary>
    /// A triangle fan used under all conditions. The center of the fan
    /// is the average of the hole boundary vertex control net points.
    ///</summary>
    TriangleFan = 3,

    ///<summary>
    /// If the hole boundary has an even number of edges, a quad fan is used.
    /// Otherwise a triangle fan is used. The center of the fan
    /// is the average of the hole boundary vertex control net points.
    ///</summary>
    QuadOrTriangleFan = 4,

    ///<summary>
    /// The hole boundary is triangluated.
    /// Typically this style is selected when a boundary not convex 
    /// and the other styles produce faces with overlapping regions.
    ///</summary>
    Triangulated = 5
  };
#pragma endregion


  
public:
  /*
  Description:
    Expert user tool to insert an edge in the face's edge array.
  Parameters:
    face - [in]
    edge - [in]
    edge_direction -[in]
    i - [in]
      index where the edge should be inserted.
  Returns:
    true if successful.
  Remarks:
    This tool is used during construction or editing of a SubD and the 
    connection is added even if the result is an invalid face or edge.
    It is up to the expert user to make enough changes to create a valid SubD.
  */
  bool AddFaceEdgeConnection(
    MYON_SubDFace* face,
    unsigned int i,
    MYON_SubDEdge* edge,
    MYON__UINT_PTR edge_direction
    );

  /*
  Description:
    Expert user tool to insert an edge in the face's edge array.
  Parameters:
    face - [in]
    eptr - [in]
      direction must be set correctly
    i - [in]
      index where the edge should be inserted.
  Returns:
    true if successful.
  Remarks:
    This tool is used during construction or editing of a SubD and the 
    connection is added even if the result is an invalid face or edge.
    It is up to the expert user to make enough changes to create a valid SubD.
  */
  bool AddFaceEdgeConnection(
    MYON_SubDFace* face,
    unsigned int i,
    MYON_SubDEdgePtr eptr
    );


  /*
  Description:
    Expert user tool to insert an edge in the face's edge array.
  Parameters:
    face - [in]
    i - [in]
      index where the edge should be inserted.
    eptr - [in]
      direction must be set correctly
    bAddFaceToRelativeVertex0 - [in]
      If true, face is appended to the eptr.RelativeVertex(0)'s face array.
    bAddFaceToRelativeVertex1 - [in]
      If true, face is appended to the eptr.RelativeVertex(0)'s face array.
  Returns:
    true if successful.
  Remarks:
    This tool is used during construction or editing of a SubD and the
    connection is added even if the result is an invalid face or edge.
    It is up to the expert user to make enough changes to create a valid SubD.
  */
  bool AddFaceEdgeConnection(
    MYON_SubDFace* face,
    unsigned int i,
    MYON_SubDEdgePtr eptr,
    bool bAddbAddFaceToRelativeVertex0,
    bool bAddbAddFaceToRelativeVertex1
    );

  /*
  Description:
    Expert user tool to set a face's boundary.
  Parameters:
    face - [in]
      Face that is in the subd with no edges.
    edges - [in]
      Array of edge_count pointers that form a loop.
      Caller is responsible for insuring edges and vertices appear only
      one time in the loop.
    edge_count - [in]
      Number of edges in the boundary.      
  Returns:
    True if successful (all edge-face and vertex-face connections are set).
    False otherwise.
  Remarks:
    This tool is used during construction or editing of a SubD and the
    connection is added even if the result is an invalid face or edge.
    It is up to the expert user to make enough changes to create a valid SubD.
  */
  bool SetFaceBoundary(
    MYON_SubDFace* face,
    const MYON_SubDEdgePtr* edges,
    size_t edge_count
  );

  bool SetFaceBoundary(
    MYON_SubDFace* face,
    const MYON_SimpleArray<MYON_SubDEdgePtr>& edges
  );


  /*
  Description:
    Expert user tool to insert an edge in the face's edge array.
  Parameters:
    face - [in]
    edge - [in]
      edge to remove
  Returns:
    true if successful.
  Remarks:
    This tool is used during construction or editing of a SubD and the 
    connection is removed even if the result is an invalid face or edge.
    It is up to the expert user to make enough changes to create a valid SubD.
  */
  bool RemoveFaceEdgeConnection(
    MYON_SubDFace* face,
    MYON_SubDEdge* edge
    );

  /*
  Description:
    Expert user tool to remove the connection between and edge and face.
  Parameters:
    face - [in]
    i - [in]
      index where the edge should be removed.
      0 <= i < face->EdgeCount()
    removed_edge - [out]
      removed edge
  Remarks:
    This tool is used during construction or editing of a SubD and the 
    connection is removed even if the result is an invalid face or edge.
    It is up to the expert user to make enough changes to create a valid SubD.
  */
  bool RemoveFaceEdgeConnection(
    MYON_SubDFace* face,
    unsigned int i
    );

  /*
  Description:
    Expert user tool to remove the connection between and edge and face.
  Parameters:
    face - [in]
    i - [in]
      index where the edge should be removed.
    removed_edge - [out]
      removed edge
  Remarks:
    This tool is used during construction or editing of a SubD and the 
    connection is removed even if the result is an invalid face or edge.
    It is up to the expert user to make enough changes to create a valid SubD.
  */
  bool RemoveFaceEdgeConnection(
    MYON_SubDFace* face,
    unsigned int i,
    MYON_SubDEdgePtr& removed_edge
    );

  /*
  Description:
    Expert user tool to remove a connection between an edge and vertex
  Parameters:
    e - [in]
      An edge with zero attached faces.
    v - [in]
      A vertex attached to the e.
  Returns:
    If successful, true is returned.
    Otherwise false is returned.
  */
  bool RemoveEdgeVertexConnection(
    MYON_SubDEdge* e,
    MYON_SubDVertex* v
  );

  /*
  Description:
    Expert user tool to remove a connection between an edge and edge->vertex[evi]
  Parameters:
    e - [in]
      An edge with zero attached faces.
    evi - [in]
      0 or 1 specifying which vertex to remove.
  Returns:
    If successful, a pointer to the removed vertex is returned.
    Otherwise nullptr is returned.
  */
  MYON_SubDVertex* RemoveEdgeVertexConnection(
    MYON_SubDEdge* e,
    unsigned evi
  );

  /*
  Description:
    Expert user tool to remove all edge and vertex connections from a face
  Parameters:
    face - [in]
  Remarks:
    This tool is used during editing of a SubD and the 
    connections are removed even if the result is an invalid face or edge.
    It is up to the expert user to make enough changes to create a valid SubD.
  */
  bool RemoveFaceConnections(
    MYON_SubDFace* face
  );

  bool GrowVertexEdgeArray(
    MYON_SubDVertex* v,
    size_t capacity
    );
  bool GrowVertexFaceArray(
    MYON_SubDVertex* v,
    size_t capacity
    );
  bool GrowEdgeFaceArray(
    MYON_SubDEdge* e,
    size_t capacity
    );
  bool GrowFaceEdgeArray(
    MYON_SubDFace* f,
    size_t capacity
    );


  /*
  Description:
    Clear all cached evaluation information (meshes, surface points, boundiang boxes, ...) 
    that depends on edge tags, vertex tags, and the location of vertex control points.
  */
  void ClearEvaluationCache() const;

  /*
  Description:
    This function copies cached evaluations of component subdivision points and limit
    surface information from src to this. Typically this is done for performance critical
    situations like control point editing.
  */
  bool CopyEvaluationCacheForExperts(const MYON_SubD& src);


 /*
  Description:
    Get a mesh representation of the MYON_SubD control net.
  Parameters:
    mesh - [in]
      If not null, the returned mesh will be stored on
      the input class.

    priority - [in]
      Specifies what type of SubD information is most important to transfer to the mesh.
      For more details, see the comments for MYON_SubDGetControlNetMeshPriority.

  Returns:
    A mesh representation of the MYON_SubD control net.
  */
  class MYON_Mesh* GetControlNetMesh(
    class MYON_Mesh* mesh,
    MYON_SubDGetControlNetMeshPriority priority
  ) const;

private:
  bool Internal_GetGeometryControlNetMesh(
    const class MYON_SubDLevel& level,
    class MYON_SubDLevelComponentIdIterator& vit_by_id,
    class MYON_SubDLevelComponentIdIterator& fit_by_id,
    class MYON_Mesh& mesh
  ) const;
  bool Internal_GetTextureCoordinatesGeometryControlNetMesh(
    const class MYON_SubDLevel& level,
    class MYON_Mesh& mesh
  ) const;
public:








#pragma region RH_C_SHARED_ENUM [MYON_SubD::NurbsSurfaceType] [Rhino.Geometry.SubD.NurbsSurfaceType] [nested:byte]
  /// <summary>
  /// MYON_SubD::NurbsSurfaceType specifies what type of NURBS surfaces are returned by MYON_SubD.GetSurfaceNurbsFragments()
  /// </summary>
  enum class NurbsSurfaceType : unsigned char
  {
    ///<summary>
    /// Not a valid type. Used to indicate the type has not been set and to encourage developers to explicitly specify a type.
    /// When in doubt, specify NurbsSurfaceType::Large.
    ///</summary>
    Unset = 0,

    ///<summary>
    /// Onee NURBS surface will be generated for each SubD quad. 
    /// N NURBS surfaces will be generated for each SubD N-gon (N = 3, 5 or more). MYON_Brepface may cover multiple
    /// Near extraordinary vertices, the surfaces may have lots of knots.
    ///</summary>
    Large = 1,

    ///<summary>
    /// NURBS surfaces will be as large as possible without the addition of extra knots. 
    /// Near extraordinary vertices, the surfaces may have lots of knots.
    /// This option is preferred when a user wants larger NURBS surfaces but not at the cost of additional NURBS control points.
    ///</summary>
    Medium = 2,

    ///<summary>
    /// NURBS surfaces will not be merged and will have clamped knots.
    ///</summary>
    Small = 3,

    ///<summary>
    /// NURBS surfaces will not be merged and will have unclamped uniform knots.
    /// This is useful as a starting point for customized merging and modifying
    /// continuity at extraordinary vertices.
    ///</summary>
    Unprocessed = 4
  };
#pragma endregion


  






public:
  /*
  Parameters:
    minimum_rectangle_count - [in]
      >= 1 minimum number of rectangles in texture domain
    image_width - [in]
    image_height = [in]
      If a texture image size is known, pass it here. Otherwise pass 0.0 for both parameters.
  Returns:
    Suggested way to partition a texture coordinate domain into rectangles.
    MYON_2udex.i = "x" count
    MYON_2udex.j = "y" count
    For example (3,2) would mean divide the 2d texture domain into 3 segments across and 2 segments vertically.
  */
  static const MYON_2udex TextureDomainGridSize(
    unsigned minimum_rectangle_count,
    double image_width,
    double image_height
  );

  enum : unsigned int
  {
    /// <summary>
    /// MYON_SUbDFace packing rectangle information is calculated so that there is at least 
    /// one unused pixel between adjacent packing rectangles when a texture image size is 
    /// TextureImageSuggestedMinimumSize x TextureImageSuggestedMinimumSize 
    /// pixels or larger.
    /// Core subd code assumes TextureImageSuggestedMinimumSize is a power of 2 and >= 512.
    /// </summary>
    TextureImageSuggestedMinimumSize = 1024
  };

  static MYON_SubDTextureCoordinateType TextureCoordinateTypeFromUnsigned(
    unsigned int texture_coordinate_type_as_unsigned
  );

  static MYON_SubDTextureCoordinateType TextureCoordinateTypeFromObsoleteTextureDomainType(
    unsigned int obsolete_texture_domain_type_as_unsigned
  );

  static  unsigned char ObsoleteTextureDomainTypeFromTextureCoordinateType(
    MYON_SubDTextureCoordinateType texture_coordinate_type
  );


  static const MYON_wString TextureCoordinateTypeToString(
    MYON_SubDTextureCoordinateType texture_domain_type
    );

  /*
  Description:
    Set the texture coordinate type.
  Parameters:
    texture_coordinate_type - [in]
      Type of texture coordinates. 
      If MYON_SubDTextureCoordinateType::Unset or MYON_SubDTextureCoordinateType::Custom,
      is passed, the type setting is changed but no changes are made to texture coordinates.
  Remarks:
    Call SetTextureCoordinates() to restore them to the default values.

    If texture_coordinate_type is MYON_SubDTextureCoordinateType::FromMapping, then
    the mapping this->TextureCoordinateMapping() is used. You may call
    this->SetTextureCoordinateMapping() to set the mapping.

    Calling this->SetTextureCoordinateType() does not change existing cached
    texture coordinates. At an appropriate time, call SetFragmentTextureCoordinates()
    to update texture coordinates on any cached fragments.

    SubD texture coordinate type and fragment texture coordinates are a mutable property.
    They can be changed by rendering applications as needed.
  */
  void SetTextureCoordinateType(
    MYON_SubDTextureCoordinateType texture_coordinate_type
  ) const;

  MYON_SubDTextureCoordinateType TextureCoordinateType() const;

  /*
  Returns:
    Number of faces with MYON_SubDFace.TexturePointsAreSet() = true.
  */
  unsigned int TexturePointsAreSet() const;

  /*
  Description:
    Delete texture points from faces.
  Returns:
    Number of faces that had texture points.
  */
  unsigned int ClearTexturePoints() const;
    
  /*
  Parameters:
    bIgnoreTextureCoordinateType - [in]
      If true, the current texture mapping tag is returned.
      If false, the current texture mapping tag is returned only when MYON_SubDTextureCoordinateType::FromMapping = TextureCoordinateType().
  Returns:
    The current texture mapping tag.
  Remarks:
    The texture mapping tag should be applied only when this->TextureCoordinateType()
    is MYON_SubDTextureCoordinateType::FromMapping.
    SubD texture domains and coordinates are a mutable property.
    They can be changed by rendering applications as needed.
  */
  const MYON_MappingTag TextureMappingTag(
    bool bIgnoreTextureCoordinateType
    ) const;

  /*
  Description:
    Set the texture mapping tag.
  Remarks:
    The texture mapping tag should be applied only when this->TextureCoordinateType()
    is MYON_SubDTextureCoordinateType::FromMapping.

    Calling this->SetTextureMappingTag() does not change existing cached
    texture coordinates. At an appropriate time, call this->SetFragmentTextureCoordinates()
    to update texture coordinates on any cached fragments.

    SubD texture domains and coordinates are a mutable property.
    They can be changed by rendering applications as needed.
  */
  void SetTextureMappingTag(const class MYON_MappingTag&) const;

  /*
  Returns:
    True if setting texture coordinates requires a set MYON_TextureMapping mapping.
  Remarks:
    An explicit texture mapping is required when TextureCoordinateType() is
    MYON_SubDTextureCoordinateType::FromMapping and TextureMappingTag()
    is set and not MYON_MappingTag::SurfaceParameterMapping or an equivalent
    surface parameter mapping.
  */
  bool TextureMappingRequired() const;

  /*
  Parameters:
    texture_coordinate_type - [in]
    texture_mapping_tag - [in]
      If texture_coordinate_type is MYON_SubDTextureCoordinateType::Custom, then
      then texture_mapping_tag information is included in the hash.
      Otherwise, texture_mapping_tag is ignored.
  Returns:
    A hash that uniquely identifies the information in  TextureCoordinateType() and
    TextureMappingTag() that applies to the current subd.
  */
  static const MYON_SHA1_Hash TextureSettingsHash(
    MYON_SubDTextureCoordinateType texture_coordinate_type,
    const class MYON_MappingTag& texture_mapping_tag
  );

  /*
  Returns:
    MYON_SubD::TextureSettingsHash(this->TextureCoordinateType(),this->TextureMappingTag());
  Remarks:
    Comparing with this->FragmentTextureCoordinatesTextureSettingsHash() can tell you
    if the current fragment texture coordinates were calculated using the same settings.
  */
  const MYON_SHA1_Hash TextureSettingsHash() const;

   /*
  Description:
    If needed, set the fragment texture coordinates.
  Parameters:
    mapping - [in]
      If MYON_SubD::TextureMappingRequired() is true, then you must
      pass a mapping with a tag that matches MYON_SubDTextureMappingTag().
      Otherwise, mapping is ignored and you may pass MYON_TextureMapping::Unset.
    bLazy - [in]
      If true and the m_T[] values were set using the same
      mapping parameters, then no calculation is performed.
  Returns:
    True if successful.
  Remarks:
    SubD texture domains and coordinates are a mutable property.
    They can be changed by rendering applications as needed.
    Call SetTextureCoordinatesFromFaceDomains() to restore them to the default values.
  */
  bool SetFragmentTextureCoordinates(
    const class MYON_TextureMapping& mapping,
    bool bLazy
  ) const;

  /*
  Returns:
    The value of MYON_SubD::TextureSettingsHash(texture_coordinate_type,texture_mapping_tag)
    for the texture_coordinate_type and texture_mapping_tag used to set the current
    fragment texture coordinates. If no fragments exist or the coordinates are not set,
    then MYON_SHA1_Hash::EmptyContentHash is returned.
  */
  const MYON_SHA1_Hash FragmentTextureCoordinatesTextureSettingsHash() const;

  /*
  Description:
    If a change requires existing fragment texture coordinates to be recalculated,
    then call ClearFragmentTextureCoordinatesTextureSettingsHash().
  */
  void ClearFragmentTextureCoordinatesTextureSettingsHash() const;

private:
  /*
  Description:
    Unconditionally sets fragment texture coordinates when a mapping is not required or is not available.
  */
  bool Internal_SetFragmentTextureCoordinatesWithoutMapping() const;

  /*
  Description:
     Sets the value returned by FragmentTextureCoordinatesTextureSettingsHash()
  */
  void Internal_SetFragmentTextureCoordinatesTextureSettingsHash(MYON_SHA1_Hash hash) const;

  MYON_SubDTextureCoordinateType Internal_BestChoiceTextureCoordinateType(
    const class MYON_TextureMapping& available_mapping
  ) const;

public:
  /*
  Description:
    Use a callback to set the vertex colros in m_C[].
  Parameters:
    bLazySet - [in]
      If bLazySet is true and fragment_colors_settings_hash and the current 
      FragmentColorsSettingsHash() are equal, then nothing is changed.
    fragment_colors_settings_hash - [in]
      A that uniquely identifies the method and parameters being
      used to set the fragment vertex colors. In general this hash
      should depend on the value of this->GeometryContentSerialNumber(),
      color_callback, and all values in the callback_context that
      determine vertex colors.  Under no circumstances should this
      hash depend on this->RenderContentSerialNumber().
    fragment_colors_mapping_tag - [in]
      If not applicable, pass MYON_MappingTag::Unset.
      A mapping tag identifying what is setting the fragment colors.
      This is the only property that persists in SubD copies and saves in 3dm archives.
      Typically:
        m_mapping_id is an id you make up that identifies what is setting the colors (thickness, curvature, ...).
        m_mapping_type will be MYON_TextureMapping::TYPE::false_colors.
        m_mapping_crc is a field from the 1990s that the SHA1 hash handles better now and setting
          m_mapping_crc = MYON_CRC32(0, sizeof(fragment_colors_settings_hash), &fragment_colors_settings_hash)
          works well.
      works well.
      Typically, m_mapping_type = TYPE::false_colors.
    callback_context - [in]
      first parameter passed to color_callback()
    color_callback - [i]
      A callback function used to set the fragment vertex colors.
  */
  bool SetFragmentColorsFromCallback(
    bool bLazySet,
    MYON_SHA1_Hash fragment_colors_settings_hash,
    MYON_MappingTag fragment_colors_mapping_tag,
    MYON__UINT_PTR callback_context,
    const MYON_Color(*color_callback)(
      MYON__UINT_PTR callback_context,
      const MYON_MappingTag& mapping_tag,
      const MYON_SubD& subd,
      MYON_SubDComponentPtr cptr,
      const MYON_3dPoint& P,
      const MYON_3dVector& N,
      const MYON_3dPoint& T,
      const MYON_SurfaceCurvature& K
      )
    ) const;

  /*
  Description:
    Clear all fragment vertex colors
  Parameters:
    bClearFragmentColorsMappingTag - [in]
      When in doubt, pass true.
      If true, the mapping tag associated with the fragment vertex colors is unset as well.
  */
  void ClearFragmentColors(
    bool bClearFragmentColorsMappingTag
  );

  /*
    Returns:
      hash identifying the way the fragment vertex colors were set.
  */
  const MYON_SHA1_Hash FragmentColorsSettingsHash() const;

  /*
  Returns:
    The current fragment vertex colors mapping tag.
  */
  const MYON_MappingTag FragmentColorsMappingTag() const;

  /*
  Description:
    Set the fragment colors mapping tag.
  Remarks:
    Calling this->SetFragmentColorsMappingTag() does not change existing cached
    fragment vertex colors. At an appropriate time, call this->SetFragmentColorsFromCallback()
    to update fragment vertex colors on any cached fragments.

    SubD fragment vertex tag and colors are a mutable property.
    They can be changed by rendering applications as needed.
  */
  void SetFragmentColorsMappingTag(const class MYON_MappingTag&) const;

public:
  /*
  Description:
    Pretend this function and MYON_SubDimple do not exist.
  Returns:
    Something that you are pretending does not exist.
  Remarks:
    It is intentional that the definition of MYON_SubDimple class is not
    available in the opennurbs library interface (not in a header file).
    The size and design of MYON_SubDimple will change constantly.
    If you choose to hack and whack so you can dereference an
    MYON_SubDimple* pointer, then your code will crash unpredictably.
  */
  const class MYON_SubDimple* SubDimple() const;
  const class MYON_SubDLevel& ActiveLevel() const;
  unsigned int SubDimpleUseCount() const;

  void ShareDimple(const MYON_SubD&);
  void SwapDimple(MYON_SubD&);

  void ShareDimple(const class MYON_SubDMeshImpl&);
  void SwapDimple(class MYON_SubDMeshImpl& );

private:
  class MYON_SubDimple* SubDimple(bool bCreateIfNeeded);
  class MYON_SubDLevel const * ActiveLevelConstPointer() const;
  class MYON_SubDLevel* ActiveLevelPointer();

  void CopyHelper(const MYON_SubD&);

private:
  class MYON_SubDHeap* Internal_Heap() const;

public:

    /*
    Returns:
      True if every face has a nonzero PackId and a set PackRect.
    */
    bool FacesArePacked() const;

    /*
    Description:
      Validates the face packing.
      
      If a face pack contains more than one face, then all of the following are required
      for that face pack to be valid.
      - Every face in the pack is a quad.
      - The quads form a rectangular grid.
      - All faces in the rectangular grid are quads.
      - All interior edges in the rectangular grid are smooth.
      - All interior vertices in the rectangular grid are smooth and have 4 edge and 4 faces.
      - All edges on the sides of the rectangular grid are either creases or are attached to
      exactly one face in the rectangular grid.
      
    Parameters:
      bPermitWrapping - [in]
        If true, then the face pack is allowed to wrap.
        For example, if bPermitWrapping is true, then a SubD cylinder that is a regular quad grid
        can have a single face pack.
      bIfValidThenUpdateFacePackingTopologyHash - [in]
        When in doubt, pass false to test if all of the current face packing information is
        completely valid.

        If you are using an add/remove creases edit approach to modify an initially valid packing,
        then pass true. Otherwise pass false.

        If this parameter is true, the packing is valid, and this->FacePackingSubDTopologyHash()
        does not match this->SubDTopologyHash(), then this->FacePackingSubDTopologyHash() is updated
        to the current value of this->SubDTopologyHash().

        If this parameter is false and and this->FacePackingSubDTopologyHash()
        does not match this->SubDTopologyHash(), then the function returns false.
    Returns:
      True if FacesArePacked() is true, the quad grids meet all the conditions described above,
      this->FacePackingId() is not nil, and either this->FacePackingSubDTopologyHash() is equal to
      this->SubDTopologyHash() or bIfValidThenUpdateFacePackingTopologyHash is true.
    */
    bool FacePackingIsValid(
      bool bIfValidThenUpdateFacePackingTopologyHash
    ) const;

private:
    /*
    Returns:
      True if all of the following are satisfied.
      1. All quads are packed into rectangular grids.
      2. The pack id is used in those grids is not used by any other face.
      3. The rectangular grids do not have interior creases.
      4. The rectangular grids have 4 boundaries (no wrapping).
      5. Non quads have pack ids that are zero or not shared with a quad.
    */
    bool QuadPackingIsValid() const;

public:

    /// <summary>
    /// The fast and simple face packing uses topology, vertex tags, and edge tags to 
    /// group quads into rectangular grids. It does not perform geometric feature analysis.
    /// {C3D8DD54-F8C8-4455-BB0E-2A2F4988EC81}
    /// </summary>
    static const MYON_UUID FastAndSimpleFacePackingId;


    // ADD NEW PackFaces ids above this comment and below FastAndSimplePackFacesId.


    /// <summary>
    /// MYON_SubD::DefaultFacePackingId ideitifies the default face packing.
    /// Code that wants to use the built-in face packing that is currently
    /// the best option for general use, will specify MYON_SubD::DefaultFacePackingId.
    /// 
    /// Currently this id is MYON_SubD::FastAndSimpleFacePackingId. 
    /// In the future it may be changed to another id. Code that wants to
    /// always apply the same face packing will explicitly specify one of
    /// the built-in face pack ids defined above.
    /// </summary>
    static const MYON_UUID DefaultFacePackingId;

    static bool IsBuiltInFacePackingId(
      MYON_UUID candidate_id
    );

    /*
    Returns:
      An id that identifies the algorithm used to pack the faces in this subd.
    */
    const MYON_UUID FacePackingId() const;

    /*
    Returns:
      The value of MYON_SubDHash::Create(MYON_SubDHashType::TopologyAndEdgeCrease, *this) when the faces were packed.
    */
    const MYON_SubDHash FacePackingTopologyHash() const;

    /*
    Description:
      Sets the FacePackingTopologyHash() property to Empty.
      Experts may need to do this when modifying a face packing.
      After calling ClearFacePackingTopologyHashForExperts(), call
      FacePackingIsValid(true) to make sure the modified packing was
      actually valid and update the FacePackingTopologyHash().
    */
    void ClearFacePackingTopologyHashForExperts() const;

    /*
    Description:
      When a custom algorithm that is not built into MYON_SubD is used to pack the
      faces, this function must be called with an id that uniquely identifies the
      custom algorithm. The present SubD geometry will be used to set the value
      of FacePackingTopologyHash().
    Returns:
      True if faces are properly packed and custom_packing_id is not nil and unique.
      Otherwise the packing is reset to the default and false is returned.
    */
    bool SetFacePackingIdForExperts(
      MYON_UUID custom_packing_id
    );

    /*
    Description:
      Clear all face pack ids and related information.
    */
    void ClearFacePackIds();



private:
  friend class MYON_SubDRef;
#pragma MYON_PRAGMA_WARNING_PUSH
#pragma MYON_PRAGMA_WARNING_DISABLE_MSC( 4251 ) 
  // C4251: ... needs to have dll-interface to be used by clients of class ...
  // m_subdimple_sp is private and all code that manages m_subdimple_sp is explicitly implemented in the DLL.
private:
  std::shared_ptr<class MYON_SubDimple> m_subdimple_sp;
#pragma MYON_PRAGMA_WARNING_POP

public:
  // The MYON_SubD code increments MYON_SubD::ErrorCount every time something
  // unexpected happens. This is useful for debugging.
  static unsigned int ErrorCount;
}; 

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDRef
//
class MYON_CLASS MYON_SubDRef
{
public:
  static const MYON_SubDRef Empty;

  MYON_SubDRef() MYON_NOEXCEPT;
  ~MYON_SubDRef();
  MYON_SubDRef(const MYON_SubDRef& src) MYON_NOEXCEPT;
  MYON_SubDRef& operator=(const MYON_SubDRef& src);

#if defined(MYON_HAS_RVALUEREF)
  // rvalue copy constructor
  MYON_SubDRef( MYON_SubDRef&& ) MYON_NOEXCEPT;
  // rvalue assignment operator
  MYON_SubDRef& operator=( MYON_SubDRef&& );
#endif

  const class MYON_SubD& SubD() const;

  /*
  Returns:
    Number of references to the managed MYON_SubD, including the one by this MYON_SubDRef.
  */
  unsigned int ReferenceCount() const;

  /*
  Description:
    Allocates a new empty MYON_SubD and has this MYON_SubDRef reference it.
  */
  class MYON_SubD& NewSubD();

  /*
  Description:
    Allocates a new MYON_SubD and has this MYON_SubDRef reference it.
  Parameters:
    src - [in]
      The new MYON_SubD managed by this MYON_SubDRef will be a copy of src.SubD().
  Returns:
    A reference to the new MYON_SubD managed by this MYON_SubDRef.
  */
  class MYON_SubD& CopySubD(
    const MYON_SubDRef& src
    );

  /*
  Description:
    Allocates a new MYON_SubD and has this MYON_SubDRef reference it.
  Parameters:
    src - [in]
      The new MYON_SubD managed by this MYON_SubDRef will be a copy of src.
  Returns:
    A reference to the new MYON_SubD managed by this MYON_SubDRef.
  */
  class MYON_SubD& CopySubD(
    const MYON_SubD& src
    );

  /*
  Description:
    If ReferenceCount() > 1, then have this MYON_SubDRef reference a 
    new copy. Otherwise do nothing. The result being that this will
    be the unique reference to the MYON_SubD managed by this MYON_SubDRef.
  Returns:
    A reference to the MYON_SubD uniquely managed by this MYON_SubDRef.
  */
  class MYON_SubD& UniqueSubD();
  
  /*
  Description:
    Remove this reference to the managed MYON_SubD. 
    If this is the last reference, then the managed MYON_SubD is deleted.
  */
  void Clear();

public:
  class MYON_SubDVertexIterator VertexIterator() const;
  class MYON_SubDEdgeIterator EdgeIterator() const;
  class MYON_SubDFaceIterator FaceIterator() const;

  /*
  Description:
    Expert user function to have this MYON_SubDRef manage the lifetime of subd.
  Parameters:
    subd - [in/out]
      subd must point to an MYON_SubD that was constructed on the heap using
      an operator new call with a public MYON_SubD constructor.
  Returns:
    a pointer to the managed subd or nullptr subd in not valid.
  Example:
    MYON_SubD* subd = new MYON_SubD(...);
    MYON_SubDRef subr;
    MYON_SubD* managed_subd = subdr.SetSubD(subd);
    // subd = nullptr
    // managed_subd = pointer you can use
  */
  class MYON_SubD* SetSubDForExperts(
    class MYON_SubD*& subd
    );

  /*
  Description:
    Expert user function to have this MYON_SubDRef reference the
    contents of an existing MYON_SubD.
    Do not use if user data on the referenced subd needs to be accessed.
  Parameters:
    subd - [in]
      Any subd on the heap or the stack.
  Returns:
    true if successful.
  */
  static MYON_SubDRef CreateReferenceForExperts(
    const MYON_SubD& subd
    );

private:
 /*
  Description:
    Expert user function to have this MYON_SubDRef reference the
    contents of an existing MYON_SubD.
    Do not use if user data on the referenced subd needs to be accessed.
  Parameters:
    subd - [in]
      Any subd on the heap or the stack.
  */
  MYON_SubDRef(
    const class MYON_SubD& subd 
    );
  
private:
#pragma MYON_PRAGMA_WARNING_PUSH
#pragma MYON_PRAGMA_WARNING_DISABLE_MSC( 4251 ) 
  // C4251: ... needs to have dll-interface to be used by clients of class ...
  // m_subd_sp is private and all code that manages m_subd_sp is explicitly implemented in the DLL.
private:
  std::shared_ptr<class MYON_SubD> m_subd_sp;
#pragma MYON_PRAGMA_WARNING_POP
};


#if defined(MYON_DLL_TEMPLATE)
MYON_DLL_TEMPLATE template class MYON_CLASS MYON_SimpleArray<MYON_SubDComponentAndNumber>;
#endif


//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDComponentList
//
class MYON_CLASS MYON_SubDComponentList
{
public:
  MYON_SubDComponentList() = default;
  ~MYON_SubDComponentList() = default;
  MYON_SubDComponentList(const MYON_SubDComponentList&);
  MYON_SubDComponentList& operator=(const MYON_SubDComponentList&);

public:
  static const MYON_SubDComponentList Empty;

public:
  unsigned CreateFromMarkedComponents(const MYON_SubD& subd, bool bComponentInListMark);
  unsigned CreateFromMarkedVertices(const MYON_SubD& subd, bool bVertexInListMark);
  unsigned CreateFromMarkedEdges(const MYON_SubD& subd, bool bEdgeInListMark);
  unsigned CreateFromMarkedFaces(const MYON_SubD& subd, bool bFaceInListMark);

  unsigned CreateFromComponentList(const MYON_SubD& subd, const MYON_SimpleArray<MYON_COMPONENT_INDEX>& component_list);
  unsigned CreateFromComponentList(const MYON_SubD& subd, const MYON_SimpleArray<MYON_SubDComponentPtr>& component_list);

  unsigned CreateFromVertexIdList(const MYON_SubD& subd, const MYON_SimpleArray<unsigned>& free_vertex_ids);
  unsigned CreateFromVertexList(const MYON_SubD& subd, const MYON_SimpleArray<MYON_SubDVertexPtr>& free_vertices);
  unsigned CreateFromVertexList(const MYON_SubD& subd, const MYON_SimpleArray<const MYON_SubDVertex*>& free_vertices);

  void Destroy();

  /*
  Returns:
    Number of removed components.
  */
  unsigned int RemoveAllComponents();

  /*
  Returns:
    Number of removed components.
  */
  unsigned int RemoveAllVertices();

  /*
  Returns:
    Number of removed components.
  */
  unsigned int RemoveAllEdges();

  /*
  Returns:
    Number of removed components.
  */
  unsigned int RemoveAllFaces();

  /*
  Returns:
    SubD runtime serial number.
  */
  MYON__UINT64 SubDRuntimeSerialNumber() const;

  /*
  Returns:
    SubD content serial number when this list was created or the last
    time UpdateContentSerialNumbers() was run.
  */
  MYON__UINT64 SubDGeometryContentSerialNumber() const;

  /*
  Returns:
    SubD content serial number when this list was created or the last
    time UpdateContentSerialNumbers() was run.
  */
  MYON__UINT64 SubDRenderContentSerialNumber() const;

  unsigned int Count() const;

  /*
  operator[] returns MYON_SubDComponentPtr::Null when index is out of bounds.
  */
  const MYON_SubDComponentPtr operator[](int) const;
  const MYON_SubDComponentPtr operator[](unsigned int) const;
  const MYON_SubDComponentPtr operator[](MYON__INT64) const;
  const MYON_SubDComponentPtr operator[](MYON__UINT64) const;
#if defined(MYON_RUNTIME_APPLE)
  const MYON_SubDComponentPtr operator[](size_t) const;
#endif

  const MYON_SimpleArray< MYON_SubDComponentPtr >& ComponentList() const;

  const MYON_SubD& SubD() const;

  /*
  Description:
    Update the saved subd geometry and render content serial number to the current values 
    of SubD().GeometryContentSerialNumber() and SubD().RenderContentSerialNumber().
  */
  void UpdateContentSerialNumbers();

  /*
  Description:
    Change the component list to reference components in a different subd.
  Parameters:
    new_subd - [in]
      subd to replace current referenced subd
    bUpdateDeletedComponents - [in]
      false: current components that are deleted will be ignored.
      true: if the corresponding component in new_sub is not deleted, it
      will be added to the list.
  Returns:
    Number of components in list after updating.
  */
  unsigned int UpdateSubDForExperts(const MYON_SubD& subd, bool bUpdateDeletedComponents);


private:
  unsigned Internal_Create(
    const MYON_SubD& subd,
    bool bAddVertices,
    bool bAddEdges,
    bool bAddFaces,
    bool bComponentInListMark,
    unsigned marked_component_count
  );

  unsigned Internal_RemoveComponents(
    bool bRemoveVertices,
    bool bRemoveEdges,
    bool bRemoveFaces
  );

private:
  MYON__UINT64 m_subd_runtime_serial_number = 0;
  MYON__UINT64 m_subd_geometry_content_serial_number = 0;
  MYON__UINT64 m_subd_render_content_serial_number = 0;

  unsigned m_subd_vertex_count = 0;
  unsigned m_subd_edge_count = 0;
  unsigned m_subd_face_count = 0;
  unsigned m_reserved = 0;

private:
  MYON_SubD m_subd; // keeps subd dimple in scope while m_component_list[] is active
  MYON_SimpleArray< MYON_SubDComponentPtr > m_component_list;
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDComponentMarksClearAndRestore
//
class MYON_SubDComponentMarksClearAndRestore
{
public:
  // Constructor saves current component RuntimeMark() settings and then clears them.
  MYON_SubDComponentMarksClearAndRestore(
    const MYON_SubD& subd
  );

  // Destructor restores saved marks.
  ~MYON_SubDComponentMarksClearAndRestore();

  /*
  Description:
    Restore marks saved by the constructor.
  Parameters:
    bDisableFutureRestores - [in]
      If true, no additional restores, including by the destructor, will occur.
  */
  bool Restore(
    bool bDisableFutureRestores
  );

  // Call DisableRestore() to prevent the destructor from restoring saved marks.
  void DisableRestore();

  const MYON_SimpleArray< const class MYON_SubDComponentBase* >& ComponentList() const;

private:
  MYON_SubD m_subd;

  MYON_SimpleArray< const class MYON_SubDComponentBase* > m_component_list;

  bool m_bRestore = true;
  unsigned char m_reserved1 = 0;
  unsigned char m_reserved2 = 0;
  unsigned char m_reserved3 = 0;
  unsigned int m_reserved4 = 0;

private:
  MYON_SubDComponentMarksClearAndRestore(const MYON_SubDComponentMarksClearAndRestore&) = delete;
  MYON_SubDComponentMarksClearAndRestore& operator=(const MYON_SubDComponentMarksClearAndRestore&) = delete;
};


//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDSectorType
//
class MYON_CLASS MYON_SubDSectorType
{
public:
  MYON_SubDSectorType() = default;
  MYON_SubDSectorType(const MYON_SubDSectorType&) = default;
  MYON_SubDSectorType& operator=(const MYON_SubDSectorType&) = default;

  static const MYON_SubDSectorType Empty;

  bool IsValid() const;

  unsigned int SectorTypeHash() const;

  static int Compare(const MYON_SubDSectorType*, const MYON_SubDSectorType*);


  /////////////////////////////////////////////////////////////////////////////////////
  //
  // Sector Coefficients
  //
  /////////////////////////////////////////////////////////////////////////////////////
  //
  //   In the comment below, 
  //   F = number of faces in the sector,
  //   E = number of edges in the sector.
  //
  //   There are five valid sector configurations of edges and faces. In all
  //   configurations, the edges have one end at the center vertex and the
  //   faces have one corner at the center vertex.
  //
  //   SMOOTH
  //     1) The center vertex is smooth.
  //     2) F >= 2
  //     3) E = F
  //     4) Every edge is smooth.
  //     5) Every edge is an edge of two different faces in the sector.
  //
  //   DART
  //     1) The center vertex is a dart.
  //     2) F >= 2
  //     3) E = F
  //     4) One edge is a crease.
  //     5) The crease edge is an edge of two geometrically adjacent sector faces.
  //
  //   DART* (The same as "DART", but the crease edge has been duplicated.)
  //     1) The center vertex is a dart.
  //     2) F >= 2
  //     3) E = F+1
  //     4) Two edges are creases that have the same end locations.
  //     5) Each crease edge is an edge of a single face in the sector,
  //        these faces are different and are geometrically adjacent.
  //
  //   BOUNDED
  //     1) The center vertex is a crease or corner vertex.
  //     2) F >= 2
  //     3) E = F+1
  //     4) Two edges are crease edges that have different vertices at their ends.
  //     5) Each crease edge is an edge of a single face in the sector,
  //        these faces are different and not geometrically adjacent.
  //
  //   BOUNDED*
  //     1) The center vertex is a crease or corner vertex.
  //     2) F = 1
  //     3) E = 2
  //     4) The edges are crease edges that have different vertices at their ends.
  //     5) The edges a edges of the face.
  //
  /////////////////////////////////////////////////////////////////////////////////////
  //
  //   The sector coefficient is used when subdividing smooth edges in sectors
  //   with a DART, DART* or BOUNDED configuration. In these cases the 
  //   sector coefficient is a value strictly between 0.0 and 1.0 that depends on
  //     1) the center vertex tag (crease, corner or dart), 
  //     2) the value of F,
  //     3) and when the center vertex is a corner, the angle between 
  //        the boundary edges.  
  //
  //   The sector coefficient is ignored when dividing smooth edges in SMOOTH sectors.
  //   The sector coefficient is ignored when subdividing crease edges.
  //
  //   For a smooth edge in a sector with a DART, DART* or BOUNDED configuration,
  //   with w = sector coefficient, C = location of the center vertex
  //   and P = location of the smooth vertex at the other end
  //   of the smooth edge, the point
  //   
  //     Q = 3/4 * (w*C + (1-w)*P) 
  //
  //   is the contribution of C and P to the edge's subdivision point.
  //   
  //   When a smooth edge has smooth vertices at both ends located
  //   at A and B, the contribution of A and B to the edge's subdivision
  //   point is 
  //
  //     Q = 3/8 * (A + B) = 3/4 * (1/2*A + 1/2*B)
  //
  //   A crease edge's subdivision point is always the edge's midpoint.
  /*
  Description:
    Calculates sector coefficient value for the sector type
    identified by this MYON_SubDSectorType.
    The sector coefficient is a property of a smooth edge end
    that is attached to a dart, crease, or corner vertex.
    In all other cases the sector coefficient is ignored.
    The value of the sector coefficient is constant throughout subdivision.
    Every smooth edge in a sector has the same sector coefficient at the central vertex.
  Returns:
    w: 0.0 < w < 1.0
      w = sector coefficient value.
    MYON_SubDSectorType::IgnoredCornerSectorAngle  (= 0.0)
      This value is used to set sector angles when the actual value is not needed.
    MYON_SubDSectorType::ErrorSectorCoefficient
      This MYON_SubDSectorType is not valid and the calculation failed.
  */
  double SectorCoefficient() const;


  unsigned int FacetEdgeCount() const;

  MYON_SubDVertexTag VertexTag() const;


  unsigned int EdgeCount() const;

  unsigned int FaceCount() const;

  /*
  Returns:
   Number of points in the point ring.  
   For quad subds, this is 1 + FaceCount() + EdgeCount().
   For tri subds, this is 1 + EdgeCount().
  */
  unsigned int PointRingCount() const;

  /*
  Returns:
   1 + FaceCount() + EdgeCount()
  */
  unsigned int ComponentRingCount() const;

  /*
  Returns:
    If the sector vertex tag is MYON_SubDVertexTag::Corner,
    the angle between the corner crease boundary edges is
    returned.  
    Otherwise, MYON_SubDSectorType::ErrorCornerSectorAngle is returned.
  */
  double CornerSectorAngleRadians() const;

  /*
  Returns:
    a value >= 0 and <= MYON_SubDSectorType::MaximumCornerAngleIndex
  */
  unsigned int CornerSectorAngleIndex() const;

  /*
  Description:
    An angle index value of MYON_SubDSectorType::MaximumCornerAngleIndex indicates
    the angle is 2pi radians.
  */
  enum : unsigned int
  {
    MaximumCornerAngleIndex = 72
  };

  // MYON_SubDSectorType::MinimumCornerAngleRadians = (2.0*MYON_PI)/((double)(MYON_SubDSectorType::MaximumCornerAngleIndex));
  static const double MinimumCornerAngleRadians;

  // MYON_SubDSectorType::MaximumCornerAngleRadians = 2.0*MYON_PI - MYON_SubDSectorType::MinimumCornerAngleRadians;
  static const double MaximumCornerAngleRadians;

  /*
  Parameters:
    angle_radians - [in] (0.0 <= angle_radians <= 2*MYON_PI)
      The angle between the bounding crease edges
  Returns:
    If angle_radians is valid input, then the value angle_index is returned.
    The value angle_index is selected so that
    (0 < angle_index < MYON_SubDSectorType::MaximumCornerSectorIndex) and
    fabs(angle_index*angle_quantum - angle_radians) is as small as possible, where
    angle_quantum =  (2.0*MYON_PI)/MYON_SubDSectorType::MaximumCornerSectorIndex.
    Otherwise MYON_UNSET_UINT_INDEX is returned.
  */
  static unsigned int CornerAngleIndexFromCornerAngleRadians(
    double angle_radians
    );

  /*
  Convert and angle index into radians
  Parameters:
    corner_angle_index - [in]
      0 to MYON_SubDSectorType::MaximumCornerAngleIndex.
  Returns:
    If angle_index is valid, the corresponding angle in radians is returned.
      = (angle_index / ((double)MYON_SubDSectorType::MaximumCornerAngleIndex)) * MYON_2PI
    Otherwise MYON_UNSET_VALUE is returned.
  */
  static double AngleRadiansFromCornerAngleIndex(
    unsigned int corner_angle_index
    );

  /*
  Returns:
    True if this is a smooth interior vertex sector
  */
  bool IsSmoothSector() const;


  /*
  Returns:
    True if this is a dart interior vertex sector
  */
  bool IsDartSector() const;


  /*
  Returns:
    True if this is a crease vertex sector
  */
  bool IsCreaseSector() const;

  /*
  Returns:
    True if this is a corner vertex sector
  */
  bool IsCornerSector() const;

  /*
  Returns:
    True if this is a convex corner vertex sector (sector angle <= pi)
  */
  bool IsConvexCornerSector() const;

  /*
  Returns:
    True if this is a concave corner vertex sector (sector angle > pi)
  */
  bool IsConcaveCornerSector() const;

  /*
  Parameters:
    sector_boundary_edge0_ptr - [in]
    sector_boundary_edge1_ptr - [in]
      Crease edges that bound the sector containing the smooth edge.
      The edge direction must identify the corner vertex.
      corner vertex = sector_boundary_edge0_ptr.RelativeVertex(0) = sector_boundary_edge1_ptr.RelativeVertex(0)
  Returns:
    tagged end angle for a smooth edge that
    1) ends at a vertex tagged on MYON_SubDVertexTag::Corner
    2) has two adjacent faces.
    3) lies in a sector bounded by 2 distinct crease edges.
    The angle is convex and positive: 0 < angle <= Pi
  */
  static double CornerSectorAngleRadiansFromEdges(
    MYON_SubDEdgePtr sector_boundary_edge0_ptr,
    MYON_SubDEdgePtr sector_boundary_edge1_ptr
    );

  static bool IsValidCornerSectorAngleRadians(
    double corner_sector_angle_radians
    );

  static double ClampCornerSectorAngleRadians(
    double corner_sector_angle_radians
    );

  /*
  Returns:
    Number of subdivision points in a sector ring
    facet_type vertex_tag ring count
    tri        smooth     N+1
    tri        crease     N+2
    quad       smooth     2N+1
    quad       crease     2N+2
    (2 * valence + 1) for quad subds
    (valence + 1) for tri subds
  */
  static unsigned int SectorPointRingCountFromEdgeCount(
    MYON_SubDVertexTag vertex_tag,
    unsigned int sector_edge_count
    );

  static unsigned int SectorPointRingCountFromFaceCount(
    MYON_SubDVertexTag vertex_tag,
    unsigned int sector_face_count
    );

  static unsigned int SectorFaceCountFromEdgeCount(
    MYON_SubDVertexTag vertex_tag,
    unsigned int sector_edge_count
    );

  static unsigned int SectorEdgeCountFromFaceCount(
    MYON_SubDVertexTag vertex_tag,
    unsigned int sector_face_count
    );
  
  static unsigned int MinimumSectorEdgeCount(
    MYON_SubDVertexTag vertex_tag
    );
  
  static unsigned int MinimumSectorFaceCount(
    MYON_SubDVertexTag vertex_tag
    );

public:
  /*
  Description:
    The sector coefficient is a property of a smooth edge end
    that is attached to a dart, crease, or corner vertex.
    In all other cases the sector coefficient is ignored.
    The value of the sector coefficient is constant throughout subdivision.
    Every smooth edge in a sector has the same sector coefficient at the central vertex.
  Returns:
    MYON_SubDSectorType::IgnoredSectorCoefficient
  */
  static double SmoothSectorCoefficient();
  
  /*
  Parameters:
  face_type - [in]
  sector_face_count - [in]
    number of faces in the smooth sector.
  Returns:
    0: 
      failed to calculate coefficient
    MYON_SubDSectorType::UnsetSectorCoefficient:
      This typically happens when a SubD control net is being 
      created and a facet type is not specified.  
      The coefficients will be calculated at the first subdivision.
    0 < w < 1:
      1/2 + 1/3*cos(tagged end sector angle / number of faces) for quadrangle facets
  Remarks:
    This is a useful tool when calling AddEdge while a subdivision
    level is being constructed.
  */
  static double CreaseSectorCoefficient(
    unsigned int sector_face_count
    );

  static double DartSectorCoefficient(
    unsigned int sector_face_count
    );

  static double CornerSectorCoefficient(
    unsigned int sector_face_count,
    double corner_sector_angle_radians
    );

  /// <summary>
  /// Copy the sector coefficient that is currently set on the edge.
  /// The sector coefficient is a property of a smooth edge end
  /// that is attached to a dart, crease, or corner vertex.
  /// In all other cases the sector coefficient is ignored.
  /// The value of the sector coefficient is constant throughout subdivision
  /// and this function is used to copy edge sector coefficients during subdivision. 
  /// Every smooth edge in a sector has the same sector coefficient at the central vertex.
  /// </summary>
  /// <param name="edge"></param>
  /// <param name="vertex">
  /// The vertex identifies which end of the edge to query.
  /// </param>
  /// <returns>
  /// If the edge and vertex are not nullptr and attached to each other, 
  /// edge->IsSmooth() is true,
  /// and vertex->IsDartCreaseOrCorner() is true, 
  /// then the current value of edge->m_sector_coefficient[vertex index] is returned.
  /// If the edge and vertex are not nullptr and attached to each other
  /// and edge->IsCrease() is true or vertex->IsSmooth() is true, 
  /// then MYON_SubDSectorType::IgnoredSectorCoefficient is returned.
  /// In all other cases error_return_value is returned.
  /// </returns>
  static double CopyEdgeSectorCoefficient(
    const class MYON_SubDEdge* edge,
    const class MYON_SubDVertex* vertex,
    double error_return_value
  );

  // This value is is used to set sector angles when the
  // actual value is not needed. This occurs at both ends
  // of a creased edge and when the end of a smooth edge
  // is a smooth vertex.
  static const double IgnoredCornerSectorAngle; // = 0.0;

  // This value is used to set sector coefficients that could not be
  // correctly set because something in the calculation failed.  
  // It is typically used when an invalid component in SubD object
  // was needed to calculate the coefficient.
  static const double UnsetCornerSectorAngle; // = -8881.0;

  // This value is indicate a corner sector angle calculation failed.
  static const double ErrorCornerSectorAngle; // = -9991.0;


  // This value is used for smooth sector thetas
  static const double SmoothSectorTheta; // = 0.5*MYON_PI

  // This value is used to indicate a sector theta needs to be set
  static const double UnsetSectorTheta; // = -8882.0;

  // This value is used to indicate a sector theta calculation failed.
  static const double ErrorSectorTheta; // = -9992.0;


  // This value is is used to set edge sector coefficients when the
  // actual value is not needed. This occurs at both ends
  // of a creased edge and when the end of a smooth edge
  // is a smooth vertex.
  static const double IgnoredSectorCoefficient; // = 0.0;

  // This value is used to mark edge sector coefficients that need to be
  // set in the future when more information is available.
  // It is typically used when creating a subD control net
  // and the facet type is not known. Any value < 0.0 and not
  // equal to MYON_UNSET_VALUE would work. The fact that the actual
  // value is -999.0 has no other significance.
  static const double UnsetSectorCoefficient; // = -8883.0;

  // This value indicates an edge sector coefficient calculation failed.
  static const double ErrorSectorCoefficient; // = -9993.0;

  static bool IsValidSectorCoefficientValue(
    double coefficient_value,
    bool bAllowUnsetTaggedEndCoefficient
    );

  /*
  Returns:
    MYON_SubDSectorType::ErrorSectorCoefficient and calls debug breakpoint
  */
  static double SectorCoefficientCalculationError();

  
  /*
  Description:
    Create a MYON_SubDSectorType from a MYON_SubDSectorIterator.
  Parameters:
    subd_type - [in]
    vertex_tag - [in]
    sector_face_count - [in]
      Number of faces in the sector.
    corner_sector_angle_radians - [in]
      If vertex_tag is MYON_SubDVertexTag::Corner, this
      parameter is the angle between the crease edges
      that bound the corner.  
      If vertex_tag is not  MYON_SubDVertexTag::Corner,
      this parameter is ignored.
  Returns:
    An MYON_SubDSectorType for the case the input parameters 
    identify.
  */
  static MYON_SubDSectorType Create(
    MYON_SubDVertexTag vertex_tag,
    unsigned int sector_face_count,
    double corner_sector_angle_radians
    );


  /*
  Description:
    Create a MYON_SubDSectorType from a MYON_SubDSectorIterator.
  Parameters:
    subd_type - [in]
    sit - [in]
  Returns:
    An MYON_SubDSectorType for the sector identified by sit.
  */
  static MYON_SubDSectorType Create(
    const MYON_SubDSectorIterator& sit
    );

  /*
  Description:
    Create a MYON_SubDSectorType for the sector containing the face.
  Parameters:
    subd_type - [in]
    face - [in]
    face_vertex_index - [in]
      face->Vertex(face_vertex_index) will be the sector's
      center vertex.
  Returns:
    An MYON_SubDSectorType for the sector containing the face.
  */
  static MYON_SubDSectorType Create(
    const class MYON_SubDFace* face,
    unsigned int face_vertex_index
    );
  
  static MYON_SubDSectorType Create(
    const class MYON_SubDFace* face,
    const class MYON_SubDVertex* vertex
    );
  
  /*
  Description:
    Create a MYON_SubDSectorType for the sector containing the edge.
  Parameters:
    subd_type - [in]
    edge - [in]
    edge_vertex_index - [in]
      edge->Vertex(edge_vertex_index) will be the sector's
      center vertex.
  Returns:
    An MYON_SubDSectorType for the sector containing the edge.
  */
  static MYON_SubDSectorType Create(
    const class MYON_SubDEdge* edge,
    unsigned int edge_vertex_index
    );

  /*
  Description:
    Create a smooth MYON_SubDSectorType.
  Parameters:
    subd_type - [in]
    sector_face_count - [in]
      Number of faces in the sector.
  Returns:
    An MYON_SubDSectorType for the smooth sector case specified
    by the input parameters.
  */
  static MYON_SubDSectorType CreateSmoothSectorType(
    unsigned int sector_face_count
    );

  /*
  Description:
    Create a crease MYON_SubDSectorType.
  Parameters:
    subd_type - [in]
    sector_face_count - [in]
      Number of faces in the sector.
  Returns:
    An MYON_SubDSectorType for the crease sector case specified
    by the input parameters.
  */
  static MYON_SubDSectorType CreateCreaseSectorType(
    unsigned int sector_face_count
    );

  /*
  Description:
    Create a dart MYON_SubDSectorType.
  Parameters:
    subd_type - [in]
    sector_face_count - [in]
      Number of faces in the sector.
  Returns:
    An MYON_SubDSectorType for the dart sector case specified
    by the input parameters.
  */
  static MYON_SubDSectorType CreateDartSectorType(
    unsigned int sector_face_count
    );

  /*
  Description:
    Create a corner MYON_SubDSectorType.
  Parameters:
    subd_type - [in]
    sector_face_count - [in]
      Number of faces in the sector.
    corner_sector_angle_radians - [in]
      The angle between the crease edges that bound the corner.  
  Returns:
    An MYON_SubDSectorType for the corner sector case specified
    by the input parameters.
  */
  static MYON_SubDSectorType CreateCornerSectorType(
    unsigned int sector_face_count,
    double sector_corner_angle_radians
    );

  static int Compare(
    const MYON_SubDSectorType& a,
    const MYON_SubDSectorType& b
    );


  /*
  Description:
    Get the subdivision matrix for the default subdivison algorithms 
    used by MYON_SubD.

    The matrix coefficients are ordered so that the matrix acts on
    the left of the points returned by MYON_SubDSectorIterator::GetVertexRing().

    For an interior vertex (smooth or dart), the coefficients are ordered
    so that one iteration of subdivision is given by:
    S*Transpose(V, E[0], F[0], E[1], F[1], ..., E[N-1], F[N-1]).
    For a dart vertex, E[0] is the point at the end of the creased edge.


    For a boundary vertex (crease or corner), the coefficients are ordered
    so that one iteration of subdivision is given by:
    S*Transpose(V, E[0], F[0], E[1], F[1], ..., F[N-2], E[N-1]).

    N = edge valence = number of edges in the sector.
    E[i] = end of i-th edge radiating from V.
    F[i] = point on the quad that is opposite V.
    The edges and faces are ordered radially so that the face for F[i]
    lies between the edges for E[i] and E[(i+1)%N].

  Parameters:
    S - [out]
      subdivision matrix
      Matrix coefficient (i,j) = S[i][j]
      0 <= i < R, 0 <= j < R, R = MYON_SubDSectorType.PointRingCount()
    matrix_capacity - [in]
      S[] can store any RxR matrix with R <= matrix_capacity.

  Returns:
    R > 0: 
      R = PointRingCount() and S is the RxR subdivision matrix for the sector type.
    0: Invalid input
  */
  unsigned int GetSubdivisionMatrix(
    double** S,
    size_t matrix_capacity
    ) const;

  /*
  Parameters:
    S - [out]
      subdivision matrix. 
      Matrix coefficient (i,j) = S[i*R + j],
      0 <= i < R, 0 <= j < R, R = MYON_SubDSectorType.PointRingCount()
    S_capacity - [in]
      Number of elements in S[] array
  Returns:
     0: Invalid input.
    >0: Number of rows and columns in S.
        This number is always MYON_SubDSectorType.PointRingCount().
  */
  unsigned int GetSubdivisionMatrix(
    double* S,
    size_t S_capacity
    ) const;

  /*
  Description:
    All the subdivision matrices for the MYON_SubD built-in
    subdivision algorithms have eigenvalues (1, lambda1, lambda2, e4, ..., eR),
    where 1 > lambda1 >= lambda2 > |e4| >= ... >= |eR| > 0.

    The subdominant eigenvalue is lambda1 and, 
    with one exception, lambda1 = lambda2.
    The exception is described in the description of 
    MYON_SubDSectorType::SubdominantEigenvalueMulitiplicity().

  Returns:
    > 0.0: 
      The subdominant eigenvalue for the subdivision matrix.

    MYON_UNSET_VALUE:
      This MYON_SubDSectorType is not valid.
  */
  double SubdominantEigenvalue() const;

  /*
  Returns:
    0:
      The sector type is not set.

    2: 
      The subdominant eigenvalue has algebraic and geometric multiplicity = 2.
      This is the most common case.

    1: 
      The subdominant eigenvalue has algebraic and geometric multiplicity = 1.
      This occurs in Catmull-Clark subdivision at a crease vertex with 
      two crease edges and a single face.  The subdivision matrix for this
      case is
        S is a 4 x 4 matrix with rows =
           (3/4, 1/8, 0, 1/8), 
           (1/2, 1/2, 0, 0),
           (1/4, 1/4, 1/4, 1/4), 
           (1/2, 0, 0, 1/2).
        S has 4 real eigenvalues = (1, 1/2, 1/4, 1/4), all with
        geometric multiplicity = 1.
        The three eigenvectors are
           (1, 1, 1, 1), (0, -1, 0, 1), (0, 0, 1, 0).
  */
  unsigned int SubdominantEigenvalueMulitiplicity() const;

  /*
  Description:
    With one exception, which is described below,
    all the subdivision matrices for the MYON_SubD built-in
    subdivision algorithms have eigenvalues (1, lambda, lambda, e4, ..., eR),
    where lambda is real, 1 > lambda > |e4| >= ... >= |eR| > 0, and the
    geometric dimension of the lambda eigenspace is 2 (there are two
    linearly independent lambda eigenvectors). 

    The subdominant eigenvalue is lambda.  This function returns an
    orthogonal basis, (E1, E2), for the subdominant eigenspace.

    An eigenvector for the dominant eigen value 1 has is (1,1,...,1).
    The domainant eignevector is orthogonal to the subdominant eigenspace.
    
    Put another way,
    0 = E1[0] + ... + E1[R-1]
    0 = E2[0] + ... + E2[R-1]
    0 = E1[0]*E2[0] + ... + E1[R-1]*E2[R-1]

    Exceptional case:
      The Catmull-Clark subdivision matrix for a crease vertex with
      two crease edges and a single face is a special case.  
      In this exceptional, this function returns
        lambda = 1/2, E1 = {0,-1,0,-1} and E2 = {1, -2, -5, -2}.
      For more information about the exceptional case, see the description of
      MYON_SubDSectorType::SubdominantEigenvalueMulitiplicity().

  Parameters:
    E1_capacity - [in]
      Capacity of the E1[] array.
    E1 - [out]
    E2_capacity - [in]
      Capacity of the E2[] array.
    E2 - [out]
      When E1_capacity > 0 and E2_capacity > 0, two orthogonal eigenvectors
      spanning the subdivision matrix subdominant eigenspace are returned
      in E1[] and E2[].
      If one of E1_capacity or E2_capacity is > 0, then both must be > 0.

  Returns:
    MYON_UNSET_VALUE: Invalid input.
    e > 0.0 and e < 1.0:
      subdominant eigenvalue.
  */
  double GetSubdominantEigenvectors(
    double* E1,
    size_t E1_capacity,
    double* E2,
    size_t E2_capacity
    ) const;

  /*
  Parameters:
    LPev_capacity - [in]
      Capacity of the LPev[] array.
    LPev - [out]
      When LPev_capacity > 0, then the limit surface point evaluation coefficients are
      returned in LPev[].  Otherwise LPev is ignored.
    LT0ev_capacity - [in]
      Capacity of the LPev[] array.
    LT0ev - [out]
    LT1ev_capacity - [in]
      Capacity of the LPev[] array.
    LT1ev - [out]
      When LT0ev_capacity > 0 and LT1ev_capacity > 0, then the limit surface
      tangent coefficients are returned in LT0ev[] and LT1ev[]. Otherwise,
      LT0ev[] and LT1ev[] are ignored.
      If one of LT0ev_capacity or LT1ev_capacity is > 0, then both must be > 0.
  Returns:
     0: Invalid input.
    >0: Number of evaluation coefficients in the L*ev[] arrays.
        This number is always MYON_SubDSectorType.PointRingCount().
  */
  unsigned int GetSurfaceEvaluationCoefficients(
    double* LPev,
    size_t LPev_capacity,
    double* LT0ev,
    size_t LT0ev_capacity,
    double* LT1ev,
    size_t LT1ev_capacity
    ) const;

  // SurfaceNormalSign() is a debugging tool - slow and not useful in general
  double SurfaceNormalSign() const;

  bool SurfaceEvaluationCoefficientsAvailable() const;

  /*
  Parameters:
    eigenvalues_capacity - [in]
      Capacity of the eigenvalues[] array.
      Must be 0 or >= PointRingCount()
    eigenvalues - [out]
      If 0 = eigenvalues_capacity, eigenvalues must be nullptr.
      If eigenvalues_capacity > 0, is specifies the capactiy 
      of the eigenvalues[] array.
  Returns:
    R > 0: 
      A complete set of eigenvalues is available for this sector type.
      The eigenvalues are (1, lambda, lambda, e3, ..., eR), where
      1 > lambda > e3 >= ... >= eR > 0.
    0: 
      Invalid input or the eigenvalues for this sector typoe are not available.
  */
  unsigned int GetAllEigenvalues(
    double* eigenvalues,
    size_t eigenvalues_capacity
    );

  /////*
  ////Description:
  ////  The subdivision matrix for all cases is known.
  ////  A complete set of eigenvalues are available for some cases.
  ////Parameters:
  ////  facet_type - [in]
  ////  vertex_tag - [in]
  ////  sector_edge_count - [in]
  ////    The input parameters identify the subdivision case.
  ////Returns:
  ////  R > 0: Eigenvalues are known.  There subdivison matrix is R x R.
  ////  0: Eigenvalues for this case are not known.
  ////*/
  ////static unsigned int AllEigenvaluesAvailableKnown(
  ////  MYON_SubDVertexTag vertex_tag,
  ////  unsigned int sector_edge_count
  ////  );

  /*
  Description:
    Create a partial subdivison sector around vertex.
    The resulting MYON_SubD has an outer ring with smooth edges and vertices,
    which is not valid as a stand-alone subd.  This is typically used for
    testing.
  Parameters:
    radius - [in]
      The center vertex is located at (0,0,0),
      If radius > 0.0, then the end of the first edge is at (radius,0,0), 
      subsequent edges are in  a radial array and quad face points, if any, 
      are 2*radius from the origin.
    sector_angle_radians - [in]
      If radius > 0, 
      this->VertexTag() is MYON_SubDVertexTag::Crease,
      crease_sector_angle_radians > 0.0 and 
      crease_sector_angle_radians < 2.0*MYON_PI,
      then this will be the angle between the crease boundary edges.
      In all other cases, crease_sector_angle_radians is ignored.
    subd - [in]
      If subd is not null, the vertex ring is put in this
      subd.
  Returns:
    a pointer to the vertex ring
    nullptr is returned if the input is not valid.
  */
  MYON_SubD* SectorRingSubD(
    double radius,
    double sector_angle_radians,
    MYON_SubD* subd
    ) const;

private:
  MYON_SubDVertexTag m_vertex_tag = MYON_SubDVertexTag::Unset;
  unsigned char m_reserved1 = 0;
  unsigned short m_reserved2 = 0;
  unsigned int m_hash = 0; // SetHash() sets this field, SectorTypeHash() returns its value.
  unsigned int m_corner_sector_angle_index = 0; // >= 0 and <= MYON_SubDSectorType::MaximumCornerAngleIndex
  unsigned int m_sector_face_count = 0;
  double m_sector_coefficient = 0.0;
  double m_sector_theta = 0.0;
  double m_corner_sector_angle_radians = 0.0;  
  
private:
  void SetHash();

  /*
  Description:
    Calculates sector theta value for the sector type
    identified by this MYON_SubDSectorType.
  Returns:
    theta: 0.0 <= theta <= MYON_PI
      The sector theta value.
    MYON_SubDSectorType::ErrorSectorTheta
      This MYON_SubDSectorType is not valid and the calculation failed.
  */
  double SectorTheta() const;

  /*
  Parameters:
    sector_face_count - [in] >= 1
      Number of faces in the crease sector.
  Returns:
    theta: 0.0 < theta <= MYON_PI
      sector theta value for a crease sector with sector_face_count faces.
      theta = Pi / sector_face_count
    MYON_SubDSectorType::ErrorSectorTheta
      sector_face_count is not valid and the calculation failed.
  */
  static double CreaseSectorTheta(
    unsigned int sector_face_count
    );
  
  /*
  Parameters:
    sector_face_count - [in] >= 2
      Number of faces in the dart sector.
  Returns:
    theta: 0.0 < theta <= MYON_PI
      sector theta value for a dart sector with sector_face_count faces.
      theta = 2 Pi / sector_face_count
    MYON_SubDSectorType::ErrorSectorTheta
      sector_face_count is not valid and the calculation failed.
  */
  static double DartSectorTheta(
    unsigned int sector_face_count
    );

  /*
  Parameters:
    sector_face_count - [in] >= 2
      Number of faces in the corner sector.
      Note the function will accept sector_face_count = 1 for consistency with the
      limit surface evaluation code but there is no valid SubD that can connect
      a smooth edge to a corner vertex with sector face count == 1.
    corner_sector_angle_radians - [in] (0.0 <= corner_sector_angle_radians <= MYON_PI)
      The convex angle between the bounding crease edges
  Returns:
    theta: 0.0 < theta <= MYON_PI/2
      sector theta value for the corner sector.
      theta = index_snapped[Pi/36](clamped[0,Pi](corner_sector_angle_radians)) / sector_face_count
    MYON_SubDSectorType::ErrorSectorTheta
      sector_face_count or corner_sector_angle_radians were not valid
      and the calculation failed.
  */
  static double CornerSectorThetaFromCornerAngle(
    unsigned int sector_face_count,
    double corner_sector_angle_radians
    );

  /*
  Parameters:
    sector_theta - [in] 0 < sector_theta <= MYON_PI
      value from one of the sector theta functions.
      MYON_SubDEdge::SectorTheta()
      MYON_SubDEdge::SmoothSectorTheta()
      MYON_SubDEdge::CreaseSectorTheta()
      MYON_SubDEdge::CornerSectorTheta()
      MYON_SubDEdge::DartSectorTheta()
  Returns:
    0: 
      failed to calculate coefficient
    MYON_SubDSectorType::ErrorSectorCoefficient: 
      sector_theta is not valid.
    0 < w < 1:
      The returned value is 
      1/2 + 1/3*cos(sector_theta). (1/6 <= w <= 5/6)
  Remarks:
    This is a useful tool when calling AddEdge while a subdivision
    level is being constructed.
  */
  static double SectorCoefficientFromTheta(
    double sector_theta
    );
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDMeshFragment
//
//  Meshes of MYON_SubD limit surface are calculated in fragments.
//
class MYON_CLASS MYON_SubDMeshFragmentGrid
{
public:
  // No construction for performance reasons. 
  // use = MYON_SubDMeshFragmentGrid::Empty
  // QuadGridFromSideSegmentCount(...)
  // or QuadGridFromDisplayDensity(...) to initialize.
  //
  //MYON_SubDMeshFragmentGrid() = default;
  //~MYON_SubDMeshFragmentGrid() = default;
  //MYON_SubDMeshFragmentGrid(const MYON_SubDMeshFragmentGrid&) = default;
  //MYON_SubDMeshFragmentGrid& operator=(const MYON_SubDMeshFragmentGrid&) = default;


  static const MYON_SubDMeshFragmentGrid Empty;

  static const MYON_SubDMeshFragmentGrid OneQuadGrid;

  /*
  Description:
    Get mesh facet quads that index into a grid of points.
  Parameters:
    side_segment_count - [in]
      number quads in each row and column of the quad grid.
        side_segment_count >= 1
        side_segment_count <= MYON_SubDMesh::MaximumSideSegmentCount
        side_segment_count must be a power of 2

    mesh_density_reduction - [in]
      0: quad count = maximum quad count = (side_count x side_count)
      1: quad count = 1/4 maximum quad count
      1: quad count = 1/16 maximum quad count
      ...
      If 4^mesh_density_reduction > maximum quad count, then a single quad is returned.
  */
  static MYON_SubDMeshFragmentGrid QuadGridFromSideSegmentCount(
    unsigned int side_segment_count,
    unsigned int mesh_density_reduction
    );

  static MYON_SubDMeshFragmentGrid QuadGridFromDisplayDensity(
    unsigned int display_density,
    unsigned int mesh_density_reduction
    );

private:
  /*
  Description:
    Get mesh facet quads that index into a grid of points.
  Parameters:
    side_segment_count - [in]
      number quads in each row and column of the quad grid
      with the highest level of detail (level_of_detail = 0)
      side_count must be a power of 2
    level_of_detail - [in]
      Desired level of detail of the returned grid
        0: highest (side_count x side_count) quads
        1: 1/4 density (side_count x side_count)/4 quads
        2: 1/16 density (side_count x side_count)/16 quads
        ...
        side_count-2: 4 quads
        side_count-1: 1 quad
        >= side_count: 1 quad
  Returns:
    Number of quads in the grid.
  */
  static unsigned int SetQuads(
    unsigned int side_segment_count,
    unsigned int level_of_detail,
    unsigned int* quads,
    size_t quad_capacity,
    size_t quad_stride,
    unsigned int* sides,
    size_t side_capacity,
    size_t side_stride
    );

public:
  unsigned int SideSegmentCount() const;

  /*
  Returns:
    SideSegmentCount() + 1;
  */
  unsigned int SidePointCount() const;

  /*
  Description:
    The GridId() is persistent and unique based on the contents of the
    grid. It is intended to be used in render applications that store
    copies of MYON_SubDMeshFragmentGrid settings in things like
    vertex object buffers and want a reliable way to index into them.
    The Empty grid has id = 0;
  Returns:
    0:
      when the grid is empty
    32*n + 2*lod + t:
      t = 0 for quads and 1 for tris,
      (0 <= n <= 8) m_F_count = 2^(2n), 
      (0 <= lod <= 8) m_F_level_of_detail = lod
  Remarks:
    m_F_count is always
  */
  unsigned int GridId() const;


  unsigned int GridFaceCount() const;

  /*
  Returns:
    Total number of points in the grid = SidePointCount()*SidePointCount().
  */
  unsigned int GridPointCount() const;

  /*
  Parameters:
    grid_point_index - [in]
      0 <= grid_point_index < GridPointCount().
    grid_parameters = [out]
      g2dex = Grid2dexFromPointIndex(grid_point_index).
      grid_parameters[] = {g2dex.i/SideSegmentCount(), g2dex.j/SideSegmentCount()}
  Returns:
    True if grid_point_index was valid and grid_parameters was returned.
  Remarks:
    On a SubD quad face, 
    face->Vertex(0), face->Vertex(1), face->Vertex(2), face->Vertex(3)
    are the SubD quad corners in counter-clockwise order.
    When a grid comes from a SubD quad face, the associated grid parameters are:
    grid_parameters[] = (0,0) at face->Vertex(0),
    grid_parameters[] = (1,0) at face->Vertex(1),
    grid_parameters[] = (0,1) at face->Vertex(3),
    grid_parameters[] = (1,1) at face->Vertex(2),
  */
  bool GetGridParameters(
    unsigned int grid_point_index,
    double grid_parameters[2]
    ) const;

  /*
  Parameters:
    grid_point_index - [in]
      0 <= grid_point_index < GridPointCount().
  Returns:
    Grid (i,j) for that grid_point_index.  
    0 <= i < SidePointCount()
    0 <= j < SidePointCount()
  Remarks:
    On a SubD quad face,
    face->Vertex(0), face->Vertex(1), face->Vertex(2), face->Vertex(3)
    are the SubD quad corners in counter-clockwise order.

    Set 
    n = grid->GridPointCount()-1 and 
    k = grid->SideSegmentCount() = (grid->SidePointCount()-1).
    Note that (k+1)*(k+1) = (n+1) = GridPointCount().

    When a grid comes from a SubD quad face, the associated grid point indices are

    vertex, grid point index, grid point 2dex
    face->Vertex(0), 0, (0,0)
    face->Vertex(1), k, (k,0)
    face->Vertex(2), n-k, (k,k)
    face->Vertex(3), n, (0,k)
  */
  const MYON_2udex Grid2dexFromPointIndex(
    unsigned int grid_point_index
  )  const;

  /*
  Parameters:
    i - [in]
      0 <= i < SidePointCount()
    j - [in]
      0 <= j < SidePointCount()
  Returns:
    0 <= grid_point_index < GridPointCount().
  Remarks:
    On a SubD quad face,
    face->Vertex(0), face->Vertex(1), face->Vertex(2), face->Vertex(3)
    are the SubD quad corners in counter-clockwise order.

    Set
    n = grid->GridPointCount()-1 and
    k = grid->SideSegmentCount() = (grid->SidePointCount()-1).
    Note that (k+1)*(k+1) = (n+1) = GridPointCount().

    When a grid comes from a SubD quad face, the associated grid point indices are

    vertex, grid point index, grid point 2dex
    face->Vertex(0), 0, (0,0)
    face->Vertex(1), k, (k,0)
    face->Vertex(2), n-k, (k,k)
    face->Vertex(3), n, (0,k)
  */
  unsigned int PointIndexFromGrid2dex(
    unsigned int i,
    unsigned int j
  )  const;

  /*
  Returns:
    A number between 0 and 8 or MYON_UNSET_INT_INDEX.
    SideSegmentCount() = 2^DisplayDensity().
  */
  unsigned int DisplayDensity() const;

  MYON_DEPRECATED_MSG("Identical to DisplayDensityReduction(). Use DisplayDensityReduction() instead because its a better name.")
  unsigned int LevelOfDetail() const;

  /*
  Description
    Each grid set has grids of quads that index the identical set of points (normals, texture coordinates, ...).
    The grid in the set with the maximum number of quads has DisplayDensityReduction() = 0.
    Additional grids in the set reduce the number of quads by a factor of 4.
    The intedger on the left is the DisplayDensityReduction value. The value of the right
    is the number of quads where M = maximum quad count for that grid set. 
    All nonzero values of M are powers of 4 (4^n). The relationship between 
    P = number of points in the set and M (the maximum number of quads for the point set)
    is P = (M/2+1)*(M/2+1).
    
    DisplayDensityReduction(): number of quads
    0: M
    1: M/4
    2: M/16
    3: M/64
    4: M/256
    ...

  Returns:
    The display density reduction from the maximum display density for this grid set.
  */
  unsigned int DisplayDensityReduction() const;

private:
  unsigned char m_reserved;

public:
  unsigned char m_reserved1 = 0;
  unsigned char m_side_segment_count; // = 2^n for non-empty grids (0 <= n <= 8)
  
  // m_F_count = number of quads
  //           = m_side_segment_count*m_side_segment_count
  // (0,1,4,16,256,1024,4096) After 0, each permitted value is 4x previous value
  unsigned short m_F_count;

  // m_F_level_of_detail is poorly named. It should be named m_F_mesh_density_reduction.
  // 0 = no reduction (maximum level of detail)
  // 1 = number of quads reduced by 1/4
  // 2 = number of quads reduced by 1/16
  // ...
  unsigned short m_F_level_of_detail;

  unsigned short m_F_stride;
  const unsigned int* m_F;
  const unsigned int* m_S; // [4*m_side_segment_count + 1] indices that form the polyline boundary.
  const MYON_SubDMeshFragmentGrid* m_prev_level_of_detail; // nullptr or the previous level with 4 times the number of quads.
  const MYON_SubDMeshFragmentGrid* m_next_level_of_detail; // nullptr or the next level with 1/4 times the number of quads.
};

class MYON_CLASS MYON_SubDMeshFragment
{
public:
  // No construction for performance reasons. 
  // If you require initialization, use = MYON_SubDMeshFragment::Empty
  //
  //MYON_SubDMeshFragment() = default;
  //~MYON_SubDMeshFragment() = default;
  //MYON_SubDMeshFragment(const MYON_SubDMeshFragment&) = default;
  //MYON_SubDMeshFragment& operator=(const MYON_SubDMeshFragment&) = default;

  bool CopyFrom( const MYON_SubDMeshFragment& src_fragment );

  /*
  Parameters:
    src_fragment - [in]
      fragment to copy
    display_density - [in]
      The desired display density of the copy.

      If display_density = MYON_UNSET_UINT_INDEX, then this->m_P_capacity must
      be at least srf_fragment.m_P_count, all points are copied, and 
      this->m_grid = srf_fragment.m_grid.

      Otherwise, src_fragment must have enough points to provide 
      the specified denisity and this must have enough 
      point capacity to store the specified density.

    this - [out]
      This must have a point capacity large enough to accommodate the
      requested display density.

  */
  bool CopyFrom( 
    const MYON_SubDMeshFragment& src_fragment,
    unsigned int display_density
  );
  
  // Every field of MYON_SubDMeshFragment::Empty is zero.
  static const MYON_SubDMeshFragment Empty;

  enum : unsigned int
  {
    MaximumSideSegmentCount = (1U << MYON_SubDDisplayParameters::MaximumDensity)
  };
  
  /*
  Returns:
    side_segment_count  = 2^display_density
  */
  static unsigned int SideSegmentCountFromDisplayDensity(
    unsigned int display_density
    );
  
  /*
  Returns:
    base 2 log of side_segment_count.
  Remarks:
    side_segment_count = 2^display_density
  */
  static unsigned int DisplayDensityFromSideSegmentCount(
    unsigned int side_segment_count
    );

  /*
  Parameters:
    display_density - [in]
      >= 0
  Returns:
    total number of points in the limit mesh fragment.
  Remarks:
    The number of points is the same for quad or tri subdivision limit
    mesh fragments, even though one is a rectanglular collection of 
    quads and the other is a triangular collection of triangles.
  */
  static unsigned int PointCountFromDisplayDensity(
    unsigned int display_density
    );

  /*
  Parameters:
    display_density - [in]
      >= 0
  Returns:
    total number of faces in the limit mesh fragment.
  */
  static unsigned int FaceCountFromDisplayDensity(
    unsigned int display_density
    );

  /*
  Returns:
    true if side_segment_count is valid.
    Otherwise 0 is returned.
  */
  static bool SideSegmentCountIsValid(
    unsigned int side_segment_count
    );

  /*
  Returns:
    If side_segment_count is valid, then (side_segment_count+1) is returned.
    Otherwise 0 is returned.
  */
  static unsigned int SidePointCountFromSideCount(
    unsigned int side_segment_count
    );

  /*
  Returns:
    If side_segment_count is valid, then (side_segment_count+1)^2 is returned.
    Otherwise 0 is returned.
  */
  static unsigned int QuadGridPointCountFromSideCount(
    unsigned int side_segment_count
    );

  /*
  Returns:
    If side_segment_count is valid, then side_segment_count^2 is returned.
    Otherwise 0 is returned.
  */
  static unsigned int QuadGridQuadCountFromSideCount(
    unsigned int side_segment_count
    );

  /*
  Description:
    Copies location and optionally normal information from a side of src_fragment
    to a side of dst_fragment. This is often used for "sealing" adjacent fragments
    where the same edge is evaluated from different faces. All evaluations are as accurate
    as IEEE double precision arithmetic can produce. However there are often tiny differences
    (last few bits of the coordinate values differ) in vertext locations because we are 
    using finite precision arithmetic with operations in different orders. 
    In general, rendering, mesho topology, and edge analysis calculations perform poorly
    when tinity differences exist.

  Parameters:
    bTestNearEqual - [in]
      If true, then the copy is performed only when the points or normals are nearly
      identical. In this case if the discrepance between coordinates is too large,
      then this function returns false.
    bCopyNormals - [in]
      If true, then normal information is copied along with point information.
    src_fragment - [in] 
      source fragment
    i0, i1 - [in]
      indices for the src_fragment.m_S[] array.      
    dst_fragment - [in] 
      destination fragment
    j0, j1 - [in]
      indices for dst_fragment.m_S[] array.
      abs(j0-j1) = abs(i0-i1);
  Remarks:
    It is required that max(i0,i1) - min(i0,i1) = max(j0,j1) - min(j0,j1) and
    all indices are valid for the respective fragments.
    The src_fragment point/normal identified by src_fragment.m_S[i] 
    is copied to he dst_fragment point/normal identified by dst_fragment.m_S[j], 
    where i0->j0, i1->j1. 
  */
  static bool SealAdjacentSides(
    bool bTestNearEqual,
    bool bCopyNormals,
    const MYON_SubDMeshFragment& src_fragment,
    unsigned int i0,
    unsigned int i1,
    MYON_SubDMeshFragment& dst_fragment,
    unsigned int j0,
    unsigned int j1
  );

  /*
  Parameters:
    bTestNearEqual - [in]
      If true, then the copy is performed only when the points or normals are nearly
      identical. In this case if the discrepance between coordinates is too large,
      then this function returns false.
    src - [in]
    dst - [in/out]
      The 3d point (src[0],src[1],src2[2]) is copied to (dst[0],dst[1],dst[2]).
  Returns:
    True if a copy occurred.
  */
  static bool SealPoints(
    bool bTestNearEqual,
    const double* src, 
    double* dst
    );

  /*
  Parameters:
    bTestNearEqual - [in]
      If true, then the copy is performed only when the points or normals are nearly
      identical. In this case if the discrepance between coordinates is too large,
      then this function returns false.
    src - [in]
    dst - [in/out]
      The 3d point (src[0],src[1],src2[2]) is copied to (dst[0],dst[1],dst[2]).
  Returns:
    True if a copy occurred.
  */
  static bool SealNormals(
    bool bTestNearEqual,
    const double* src, 
    double* dst
    );

  /*
  Returns:
    Number of mesh quads in a full sized fragment with the specified mesh density.
  */
  static unsigned int FullFragmentMeshQuadCountFromDensity(
    unsigned int mesh_density
  );

  /*
  Returns:
    Number of mesh quads in a half sized fragment with the specified mesh density.
  */
  static unsigned int HalfFragmentMeshQuadCountFromDensity(
    unsigned int mesh_density
  );

  /*
  Returns:
    Number of mesh points in a full sized fragment with the specified mesh density.
  */
  static unsigned int FullFragmentMeshPointCountFromDensity(
    unsigned int mesh_density
  );

  /*
  Returns:
    Number of mesh points in a half sized fragment with the specified mesh density.
  */
  static unsigned int HalfFragmentMeshPointCountFromDensity(
    unsigned int mesh_density
  );

private:
  // This field overlaps with MYON_FixedSizePoolElement.m_next when a fixed size pool is managing the fragments.
  // When m_reserved != 0, the framgment is uninitialized. 
  MYON__UINT64 m_reserved;

public:
  const class MYON_SubDFace* m_face;

  // m_face_vertex_index[] stores the information needed for the Vertex()
  // and Edge() functions to work.
  //
  // If m_face is nullptr, then m_face_vertex_index[] has no meaning.
  // If m_face is not nullptr and a corner of the grid is on a face
  // vertex, then the corresponding m_face_vertex_index[] value
  // will be <= MYON_SubDFace::MaximumEdgeCount and m_face->Vertex(m_face_vertex_index[])
  // is the vertex.  Otherwise,  the corresponding m_face_vertex_index[] value
  // will be > MYON_SubDFace::MaximumEdgeCount.  
  // Catmull-Clark limit meshes:
  //   When the original SubD face is a quad, a full fragment is created and
  //   m_face_vertex_index[4] = {0,1,2,3}.
  //   When the original SuD face is an N-gon with N != 4, a partial fragment 
  //   is delivered and m_face_vertex_index[2] identifies the face vertex 
  //   for that fragment.  m_face_vertex_index[0,1,3] = a value > MYON_SubDFace::MaximumEdgeCount
  unsigned short m_face_vertex_index[4];

  const class MYON_SubDFace* SubDFace() const;

  const bool HasValidPointAndNormalGrid() const;

  /*
  Returns:
    True if the fragment covers the entire SubD face.
  */
  bool IsFullFaceFragment() const;

  /*
  Returns:
    True if the fragment covers a corner of the SubD face.
  */
  bool IsFaceCornerFragment() const;
  
  /*
  Returns:
    If IsFaceCornerFragment() is true, then the index of the face's vertex for the corner is returned.
    Otherwise, MYON_UNSET_UINT_INDEX is returned.
  */
  unsigned int FaceCornerIndex() const;

  /*
  Returns:
    Number of fragments that cover this face. 1 for quads and N>= 3 for faces with N sides when N != 4.
  */
  unsigned int FaceFragmentCount() const;

  /*
  Returns:
    First fragment for this->m_face.
  */
  const MYON_SubDMeshFragment* FirstFaceFragment() const;

  /*
  Returns:
    Last fragment for this->m_face.
  */
  const MYON_SubDMeshFragment* LastFaceFragment() const;

  /*
  Parameters:
    bReturnLastFromFirstFirst - in
      If false and this is the first fragment, then nullptr is returned.
      If true and this is the first fragment, then LastFaceFragment() is returned.
  Returns:
    Previous fragment for this->m_face.
  */
  const MYON_SubDMeshFragment* PreviousFaceFragment(
    bool bReturnLastFromFirstFirst
  ) const;

  /*
  Parameters:
    bReturnFirstFromLast - in
      If false and this is the last fragment, then nullptr is returned.
      If true and this is the last fragment, then FirstFaceFragment() is returned.
  Returns:
    Next fragment for this->m_face.
  */
  const MYON_SubDMeshFragment* NextFaceFragment(
    bool bReturnFirstFromLast
  ) const;

  /*
  Parameters:
    fragments_capacity - in
      Capacity of the fragments[] array.
    fragments[] - out
      Fragments are returned here.
  */
  unsigned int GetFaceFragments(
    const MYON_SubDMeshFragment** fragments,
    size_t fragments_capacity
    ) const;

  unsigned int GetFaceFragments(
    MYON_SimpleArray<const MYON_SubDMeshFragment*>& fragments
  ) const;

  /*
  Parameters:
    grid2dex_i - [in]
      0 <= grid2dex_i < m_grid.SidePointCount()
    grid2dex_j - [in]
      0 <= grid2dex_j < m_grid.SidePointCount()
  Returns:
  */
  const MYON_3dPoint VertexPoint(
    unsigned grid2dex_i,
    unsigned grid2dex_j
  ) const;
  const MYON_3dPoint VertexPoint(
    MYON_2udex grid2dex
  ) const;
  const MYON_3dPoint VertexPoint(
    unsigned grid_point_index
  ) const;

  /*
  Parameters:
    grid2dex_i - [in]
      0 <= grid2dex_i < m_grid.SidePointCount()
    grid2dex_j - [in]
      0 <= grid2dex_j < m_grid.SidePointCount()
  Returns:
  */
  const MYON_3dVector VertexNormal(
    unsigned grid2dex_i,
    unsigned grid2dex_j
  ) const;
  const MYON_3dVector VertexNormal(
    MYON_2udex grid2dex
  ) const;
  const MYON_3dVector VertexNormal(
    unsigned grid_point_index
  ) const;
  
  /*
  Parameters:
    grid2dex_i - [in]
      0 <= grid2dex_i < m_grid.SidePointCount()
    grid2dex_j - [in]
      0 <= grid2dex_j < m_grid.SidePointCount()
  Returns:
    The texture coordinates calculated by iterpolating the m_ctrlnetT[] values.
  */
  const MYON_3dPoint VertexTextureCoordinateFromCorners(
    unsigned grid2dex_i,
    unsigned grid2dex_j
  ) const;
  const MYON_3dPoint VertexTextureCoordinateFromCorners(
    MYON_2udex grid2dex
  ) const;
  const MYON_3dPoint VertexTextureCoordinateFromCorners(
    unsigned grid_point_index
  ) const;
  
  /*
  Description:
    Get the texture coordinate for the specified fragment grid point.
  Parameters:
    grid2dex_i - [in]
      0 <= grid2dex_i < m_grid.SidePointCount()
    grid2dex_j - [in]
      0 <= grid2dex_j < m_grid.SidePointCount()
  Returns:
    The texture coordinate for the specified fragment grid point.
    TextureCoordinateDimension() reports the number of coordinates to set.
    When it is 2, the z coordinate of the returned point is 0.0.
  */
  const MYON_3dPoint VertexTextureCoordinate(
    unsigned grid2dex_i,
    unsigned grid2dex_j
  ) const;
  const MYON_3dPoint VertexTextureCoordinate(
    MYON_2udex grid2dex
  ) const;
  const MYON_3dPoint VertexTextureCoordinate(
    unsigned grid_point_index
  ) const;

  /*
  Description:
    Set the texture coordinate for the specified fragment grid point.
  Parameters:
    grid2dex_i - [in]
      0 <= grid2dex_i < m_grid.SidePointCount()
    grid2dex_j - [in]
      0 <= grid2dex_j < m_grid.SidePointCount()
    unsigned texture_coordinate_dimension - [in]
      2 or 3
    texture_coordinate - [in]
      Texture coordinates are mutable and are often modified on an otherwise const fragment.
  Returns:
    True if input was valid and the texture coordinate was set.
  */
  bool SetVertexTextureCoordinate(
    unsigned grid2dex_i,
    unsigned grid2dex_j,
    MYON_3dPoint texture_coordinate
  ) const;
  bool SetVertexTextureCoordinate(
    MYON_2udex grid2dex,
    MYON_3dPoint texture_coordinate
  ) const;
  bool SetVertexTextureCoordinate(
    unsigned grid_point_index,
    MYON_3dPoint texture_coordinate
  ) const;

  bool TextureCoordinatesExist() const;
  void SetTextureCoordinatesExist(bool bTextureCoordinatesExist) const;

  /*
  Parameters:
    grid_side_index - [in]
      0 to 3 for quad grids.
      0 to 2 for tri grids
  Returns:
    The subd edge that is on the identified side of the grid.
  */
  const class MYON_SubDEdgePtr SubDEdgePtr(
    unsigned int grid_side_index
    ) const;
  const class MYON_SubDEdge* SubDEdge(
    unsigned int grid_side_index
    ) const;

  /*
  Parameters:
    grid_corner_index - [in]
      0, 1, 2, or 3
  Remarks:
    For partial fragments (IsFaceCornerFragment() = true), grid_corner_index = 2 is the only
    corner that corresponds to a SubD vertex. 
    For partial fragments (IsFaceCornerFragment() = true), grid_side_index = 1 and grid_side_index = 2
    correspond to half of original SuD edges.
  */
  const class MYON_SubDVertexPtr SubDVertexPtr(
    unsigned int grid_corner_index
    ) const;

  /*
  Parameters:
    grid_corner_index - [in]
      0, 1, 2, or 3
  Remarks:
    For partial fragments (IsFaceCornerFragment() = true), grid_corner_index = 2 is the only
    corner that corresponds to a SubD vertex. 
    For partial fragments (IsFaceCornerFragment() = true), grid_side_index = 1 and grid_side_index = 2
    correspond to half of original SuD edges.
  */
  const class MYON_SubDVertex* SubDVertex(
    unsigned int grid_corner_index
    ) const;


  /*
  Parameters:
    grid_corner_index - [in]
      0, 1, 2, or 3
  Returns:
    Limit surface location at the grid corner or MYON_3dPoint::NanPoint if the fragment is empty.
  Remarks:
    For partial fragments (IsFaceCornerFragment() = true), grid_corner_index = 2 is the only
    corner that corresponds to a SubD vertex. 
    For partial fragments (IsFaceCornerFragment() = true), grid_side_index = 1 and grid_side_index = 2
    correspond to half of original SuD edges.
  */
  const MYON_3dPoint CornerPoint(
    unsigned int grid_corner_index
    ) const;

  /*
  Parameters:
    grid_corner_index - [in]
      0, 1, 2, or 3
  Returns:
    Limit surface normal at the grid corner or MYON_3dPoint::NanPoint if the fragment is empty.
  Remarks:
    For partial fragments (IsFaceCornerFragment() = true), grid_corner_index = 2 is the only
    corner that corresponds to a SubD vertex. 
    For partial fragments (IsFaceCornerFragment() = true), grid_side_index = 1 and grid_side_index = 2
    correspond to half of original SuD edges.
  */
  const MYON_3dVector CornerNormal(
    unsigned int grid_corner_index
  ) const;

  /*
  Parameters:
    grid_corner_index - [in]
      grid side N is between corner index N and corner index (N+1)%4.
  Returns:
    Limit surface frame at the grid corner or MYON_3dPoint::NanPoint if the fragment is empty.
  Remarks:
    For partial fragments (IsFaceCornerFragment() = true), grid_corner_index = 2 is the only
    corner that corresponds to a SubD vertex. 
    For partial fragments (IsFaceCornerFragment() = true), grid_side_index = 1 and grid_side_index = 2
    correspond to half of original SuD edges.
  */
  const MYON_Plane CornerFrame(
    unsigned int grid_corner_index
  ) const;

  /*
  Parameters:
    grid_corner_index - [in]
      0, 1, 2, or 3
  Returns:
    vertex color at the grid corner or MYON_Color::UnsetColor if thre are not vertex colors.
  Remarks:
    For partial fragments (IsFaceCornerFragment() = true), grid_corner_index = 2 is the only
    corner that corresponds to a SubD vertex.
    For partial fragments (IsFaceCornerFragment() = true), grid_side_index = 1 and grid_side_index = 2
    correspond to half of original SuD edges.
  */
  const MYON_Color CornerColor(
    unsigned int grid_corner_index
  ) const;

  /*
  Parameters:
    grid_corner_index - [in]
      0, 1, 2, or 3
  Returns:
    vertex surface curvature at the grid corner or MYON_SurfaceCurvature::Nan if there are not vertex curvatures.
  Remarks:
    For partial fragments (IsFaceCornerFragment() = true), grid_corner_index = 2 is the only
    corner that corresponds to a SubD vertex.
    For partial fragments (IsFaceCornerFragment() = true), grid_side_index = 1 and grid_side_index = 2
    correspond to half of original SuD edges.
  */
  const MYON_SurfaceCurvature CornerCurvature(
    unsigned int grid_corner_index
  ) const;


  /*
  Parameters:
    grid_side_index - [in]
      grid side N is between corner index N and corner index (N+1)%4.
  Returns:
    Limit surface location at the midde of the grid side or MYON_3dPoint::NanPoint if the fragment is empty.
  Remarks:
    For partial fragments (IsFaceCornerFragment() = true), grid_corner_index = 2 is the only
    corner that corresponds to a SubD vertex. 
    For partial fragments (IsFaceCornerFragment() = true), grid_side_index = 1 and grid_side_index = 2
    correspond to half of original SuD edges.
  */
  const MYON_3dPoint SidePoint(
    unsigned int grid_side_index
  ) const;

  /*
  Parameters:
    grid_side_index - [in]
      grid side N is between corner index N and corner index (N+1)%4.
  Returns:
    Limit surface normal at the grid corner or MYON_3dPoint::NanPoint if the fragment is empty.
  Remarks:
    For partial fragments (IsFaceCornerFragment() = true), grid_corner_index = 2 is the only
    corner that corresponds to a SubD vertex. 
    For partial fragments (IsFaceCornerFragment() = true), grid_side_index = 1 and grid_side_index = 2
    correspond to half of original SuD edges.
  */
  const MYON_3dVector SideNormal(
    unsigned int grid_side_index
  ) const;

  /*
  Parameters:
    grid_side_index - [in]
      grid side N is between corner index N and corner index (N+1)%4.
  Returns:
    Limit surface frame at the grid corner or MYON_3dPoint::NanPoint if the fragment is empty.
  Remarks:
    For partial fragments (IsFaceCornerFragment() = true), grid_corner_index = 2 is the only
    corner that corresponds to a SubD vertex. 
    For partial fragments (IsFaceCornerFragment() = true), grid_side_index = 1 and grid_side_index = 2
    correspond to half of original SuD edges.
  */
  const MYON_Plane SideFrame(
    unsigned int grid_side_index
  ) const;

  /*
  Parameters:
    grid_side_index - [in]
      grid side N is between corner index N and corner index (N+1)%4.
  Returns:
    Limit surface location at the center of the grid side or MYON_3dPoint::NanPoint if the fragment is empty.
  Remarks:
    For partial fragments (IsFaceCornerFragment() = true), grid_corner_index = 2 is the only
    corner that corresponds to a SubD vertex. 
    For partial fragments (IsFaceCornerFragment() = true), grid_side_index = 1 and grid_side_index = 2
    correspond to half of original SuD edges.
  */
  const MYON_3dPoint CenterPoint(
  ) const;

  const MYON_3dVector CenterNormal() const;

  const MYON_Plane CenterFrame() const;

  const MYON_3dPoint CenterTextureCoordinate() const;

private:
  bool Internal_GetFrameHelper(
    unsigned int P_dex,
    unsigned int Q_dex,
    MYON_Plane& frame
  ) const;

public:

  /*
  Returns:
    Status of the face.
  */
  MYON_ComponentStatus Status() const;

  bool Transform(
    const MYON_Xform& xform
    );  

  MYON_SubDMeshFragment* m_next_fragment;
  MYON_SubDMeshFragment* m_prev_fragment;

  unsigned short m_face_fragment_count; // Number of fragments that will be delivered for this face.
  unsigned short m_face_fragment_index; // First fragment has index = 0. Last fragment has index = m_face_fragment_count-1.

  // The mesh fragment is a grid of quads.
  // There are m_side_count quad edges along each side of the tessellation,
  // There are a total of m_side_count X m_side_count quads, and
  // m_P_count = (m_side_count+1)*(m_side_count+1).

public:
  enum : unsigned
  {
    /// <summary>
    /// 64x64 grid of points
    /// </summary>
    MaximumVertexCount = 0x1000
  };

  /*
  Returns:
    Number of vertices in the mesh fragment grid.
    VertexCount() should be identical to m_grid.GridPointCount().
    VertexCapacity() should be >= VertexCount().
  */
  unsigned VertexCount() const;

  /*
  Description:
    Sets number of fragment vertices being used (number of elements being used in the m_P[], m_N[], and m_T[] arrays).
  Parameters:
    vertex_count - [in]
      A value no larger than MYON_SubDMeshFragment::MaximumVertexCount.
  */
  bool SetVertexCount(size_t vertex_count);

  /*
  Returns:
    Capactity for vertices in the mesh fragment grid.
    VertexCapacity() should be >= VertexCount().
    VertexCount() should be identical to m_grid.PointCount().
  */
  unsigned VertexCapacity() const;

  /*
  Description:
    Sets number of fragment vertices available (number of elements available in the m_P[], m_N[], and m_T[] arrays).
    The memory for the arrays is managed by something besides this MYON_SubDManagedMeshFragment instance.
  Parameters:
    vertex_capacity - [in]
      A value no larger than MYON_SubDMeshFragment::MaximumVertexCount.
  Returns:
    True if successful
  */
  bool SetUnmanagedVertexCapacity(size_t vertex_capacity);


  /*
  Description:
    Sets number of fragment vertices available (number of elements available in the m_P[], m_N[], and m_T[] arrays).
    The memory for the arrays is managed by something besides this MYON_SubDManagedMeshFragment instance.
  Parameters:
    vertex_capacity - [in]
      A value no larger than MYON_SubDMeshFragment::MaximumVertexCount.
  Returns:
    True if successful
  */
  bool ReserveManagedVertexCapacity(size_t vertex_capacity);

  /*
  Returns:
    True if the memory in the m_P[], m_N[], and m_T[] is managed by this MYON_SubDManagedMeshFragment instance.
  */
  bool ManagedArrays() const;

  bool DeleteManagedArrays();

  /*
  Returns:
    True if the memory in the m_P[], m_N[], and m_T[] is managed by something besides this MYON_SubDManagedMeshFragment instance.
  */
  bool UnmanagedArrays() const;

private:
  friend class MYON_SubDManagedMeshFragment;
  friend class MYON_SubDMeshImpl;
  friend class MYON_SubDHeap;

  // Number of grid vertices and capacity of arrays in certain conditions.
  enum
  {
    /// <summary>
    /// 3 double for the vertex point
    /// 3 double for the vertex normal
    /// 3 double for the vertex texture coordinate
    /// 1 double (4 bytes used for vertex color and 4 bytes currently not used)
    /// </summary>
    ManagedDoublesPerVertex = 10,

    /// <summary>
    /// Number of doubles between successive 3d points in m_P[]/m_N[]/m_T[].
    /// When managed arrays are not interlaced, this value must be 3.
    /// When managed arraays are interlaced, this value must be ManagedDoublesPerPoint.
    /// </summary>
    Managed_3d_stride = 3,

    /// <summary>
    /// Number of MYON_Color elements between successive colors in m_C[].
    /// When managed arrays are not interlaced, this value must be 1.
    /// When managed arraays are interlaced, this value must be ManagedDoublesPerPoint*sizeof(double)/sizeof(MYON_Color).
    /// </summary>
    Managed_color_stride = 1,
  };

  static bool Internal_ManagedArraysAreInterlaced();
  static size_t Internal_Managed3dArrayOffset(size_t vertex_capacity);

  /*
  Parameters:
    PNTC_array[] - [in]
      An array of ManagedDoublesPerVertex*vertex_capacity doubles.
  */
  void Internal_LayoutArrays(
    bool bManagedArray,
    double* PNTC_array,
    size_t vertex_capacity
  );

  enum : unsigned short
  {
    /// <summary>
    /// Vertex count = (m_vertex_count_etc & EtcMask)
    /// Vertex capacity = (m_vertex_capacity_etc & EtcMask)
    /// These are the counts and capacities of the m_P[]/m_N[]/m_T[]/m_C[] arrays the pointer
    /// is not nullpr and the corresponding m_P_stride/m_N_stride/m_T_stride/m_C_stride > 0.
    /// </summary>
    ValueMask = 0x1FFF, // maximum vertex count and capacity

    /// <summary>
    /// The use of the 3 bits in (m_vertex_count_etc & EtcMask) and 3 bits in (m_vertex_capacity_etc & EtcMask)
    /// are described in the enum values below.
    /// </summary>
    EtcMask = 0xE000,

    /// <summary>
    /// This bit is on m_vertex_count_etc.
    /// Set means 4 m_ctrlnetP[] points are set and the m_ctrlnetT[]/m_ctrlnetC[] 
    /// are set if m_T[]/m_C[] is set. 
    /// </summary>
    EtcControlNetQuadBit = 0x8000,

    /// <summary>
    /// This bit is on m_vertex_count_etc.
    /// Set means m_T[] contains set values.
    /// </summary>
    EtcTextureCoordinatesExistBit = 0x4000,
    
    /// <summary>
    /// This bit is on m_vertex_capacity_etc.
    /// Set means m_C[] contains set values.
    /// </summary>
    EtcColorsExistBit = 0x2000,

    /// <summary>
    /// This bit is on m_vertex_capacity_etc.
    /// Set means m_K[] contains set values.
    /// </summary>
    EtcCurvaturesExistBit = 0x4000,

    /// <summary>
    /// This bit is on m_vertex_capacity_etc.
    /// Set means the memory for the m_P[], m_N[], m_T[], and m_C[] arrays is managed by this class as a single
    /// allocation.
    /// m_P = new(std::nothrow) double[10*(point capacity)] and m_N, m_T, and m_C point into this allocation.
    /// </summary>
    EtcManagedArraysBit = 0x8000,
  };
  mutable unsigned short m_vertex_count_etc; // count value and 3 et cetera status bits
  mutable unsigned short m_vertex_capacity_etc; // capacity value and 3 et cetera status bits

  static void Internal_Set3dPointArrayToNan(double* a, size_t a_count, size_t a_stride);

private:
  // corners for control net display in grid order (counter-clockwise quad order must swap [2] and[3])
  double m_ctrlnetP[4][3];

  // Normal used for shading the control net display in grid order.
  double m_ctrlnetN[3];


public:
  /*
  Parameters:
    grid_corner_index - [in]
      grid side N is between corner index N and corner index (N+1)%4.
  Returns:
    Texture coordinate at that corner.
  Remarks:
    For partial fragments (IsFaceCornerFragment() = true), grid_corner_index = 2 is the only
    corner that corresponds to a SubD vertex.
    For partial fragments (IsFaceCornerFragment() = true), grid_side_index = 1 and grid_side_index = 2
    correspond to half of original SuD edges.
  */
  const MYON_3dPoint TextureCoordinateCorner(
    unsigned int grid_corner_index
  ) const;

private:
  // The fragment's current corner texture coordinates in grid order.
  // These can be from MYON_SubDFace pack rectangles, MYON_SubDFace custom texture points,
  // a texture mapping, or any other values rendering code deems desirable.
  // When a texture mapping evaluatior is not involved, they are typically
  // used to generate the fragment's texture coordinates by interpolation.
  //  m_ctrlnetT[0]
  //  m_ctrlnetT[1]
  //  m_ctrlnetT[2]
  //  m_ctrlnetT[3]
  mutable double m_ctrlnetT[4][3]; 

  // Corner principal curvatures in grid order.
  mutable MYON_SurfaceCurvature m_ctrlnetK[4];

  // Corner vertex colors in grid order.
  mutable MYON_Color m_ctrlnetC[4];

private:
  // When an MYON_SubDFace is a quad, there is a single MYON_SubDMeshFragment for the face
  // and MYON_SubDMeshFragment.m_pack_rect are the corners of the quad face's pack rect in
  // MYON_SubDMeshFragment gid order.
  // When an MYON_SubDFace is an N-gon with N !+ 4, there are N MYON_SubDMeshFragments for the face
  // and MYON_SubDMeshFragment.m_pack_rect are corners of a subrect of the face's pack rect
  // in MYON_SubDMeshFragment gid order.
  // The m_pack_rect[] points are in grid order. If the parent MYON_SubDFace pack rect is rotated,
  // then "lower/upper" and "left/right" are with the rotation applied and hence a lower/left coordinate
  // may be greater than an upper/right coordinate value.
  //  m_pack_rect[0] "lower left"
  //  m_pack_rect[1] "lower right"
  //  m_pack_rect[2] "upper left"
  //  m_pack_rect[3] "upper right"
  double m_pack_rect[4][2]; // NOT a mutable property

public:
  /*
  Parameters:
    grid_corner_index - [in]
      grid side N is between corner index N and corner index (N+1)%4.
  Returns:
    Pack rect corner point.
  Remarks:
    For partial fragments (IsFaceCornerFragment() = true), grid_corner_index = 2 is the only
    corner that corresponds to a SubD vertex.
    For partial fragments (IsFaceCornerFragment() = true), grid_side_index = 1 and grid_side_index = 2
    correspond to half of original SuD edges.
  */
  const MYON_2dPoint PackRectCorner(
    unsigned int grid_corner_index
  ) const;

  const MYON_2dPoint PackRectCenter() const;

  void SetPackRectCornersForExperts(
    bool bGridOrder,
    const MYON_2dPoint fragment_pack_rect_corners[4]
  );

  /*
  Description:
    This functions sets the pack rect corners on fragments used to render
    quad and 3-gon MYON_SubDFaces.
    When an MYON_SubDFace is a quad, it is rendered with one MYON_SubDMeshFragment.
    When an MYON_SubDFace is a 3-gon, it is rendered with three MYON_SubDMeshFragments.
  Remarks:
    It is critical that m_face_fragment_count and m_face_fragment_index be set
    correctly before calling this function.
    A MYON_SubDMeshFragment used to render a quad MYON_SubDFace has
    m_face_fragment_count = 1 and m_face_fragment_index = 0.
    A MYON_SubDMeshFragment used to render a 3-gon MYON_SubDFace has
    m_face_fragment_count = 3 and m_face_fragment_index = 0, 1 or 2.
  */
  void SetQuadOr3gonFaceFragmentPackRectCorners(
    bool bGridOrder,
    const MYON_2dPoint face_pack_rect_corners[4]
  );

  /*
  Description:
    This functions sets the pack rect corners on fragments used to render
    n-gon MYON_SubDFaces when n >= 5.
    When an MYON_SubDFace is an n-gon, it is rendered with n MYON_SubDMeshFragments.
  Parameters:
    bGridOrder = [in]
      Order for face_pack_rect_corners[].
    face_texture_coordinate_corners - [in]
      The points returned by MYON_SubDFace::PackRectCorner(bGridOrder, i)
    face_pack_rect_size - [in]
      The n-gon face's MYON_SubDFace::PackRectSize() value.
    ngon_grid_size - [in]
    ngon_sub_pack_rect_size - [in]
    ngon_sub_pack_rect_delta - [in]
      These three parameters are values from MYON_SubDFace::GetNgonSubPackRectSizeAndDelta().
      ngon_grid_size, ngon_sub_pack_rect_size, and ngon_sub_pack_rect_delta are identical
      for all the MYON_SubDMeshFragments used to render the n-gon face. The value of
      m_face_fragment_index determines which sub pack rect is assigned to each of
      the n MYON_SubDMeshFragments use to render the n-gon face.
  Remarks:
    It is critical that m_face_fragment_count and m_face_fragment_index be set
    correctly before calling this function.
    A MYON_SubDMeshFragment used to render an n-gon MYON_SubDFace has
    m_face_fragment_count = n and m_face_fragment_index = 0, ..., n-1.
  */
  void SetNgonFaceFragmentPackRectCorners(
    bool bGridOrder,
    const MYON_2dPoint face_pack_rect_corners[4],
    MYON_2dVector face_pack_rect_size,
    MYON_2udex ngon_grid_size,
    MYON_2dVector ngon_sub_pack_rect_size,
    MYON_2dVector ngon_sub_pack_rect_delta
  );

  /*
  Description:
    This functions gets the pack rect corners on fragments used to render 3-gon MYON_SubDFaces.
    When an MYON_SubDFace is an 3-gon, it is rendered with 3 MYON_SubDMeshFragments.
  Parameters:
    bFacePackRectGridOrder - [in]
      Order for face_pack_rect_corners[].
    face_pack_rect_corners - [in]
      The points returned by MYON_SubDFace::PackRectCorner(bGridOrder, i)
    fragment_index - [in]
      0 <= ngon_fragment_index < 3
    bFragmentPackRectGridOrder - [in]
      Order for face_pack_rect_corners[].
    fragment_pack_rect_corners - [out]
  */
  static bool Get3gonFaceFragmentPackRectCorners(
    bool bFaceGridOrder,
    const MYON_2dPoint face_pack_rect_corners[4],
    unsigned int fragment_index,
    bool bFragmentGridOrder,
    MYON_2dPoint fragment_pack_rect_corners[4]
  );

  /*
  Description:
    This functions gets the pack rect corners for fragments used to render
    n-gon MYON_SubDFaces when n >= 5.
    When an MYON_SubDFace is an n-gon, it is rendered with n MYON_SubDMeshFragments.
  Parameters:
    ngon_fragment_index - [in]
      0 <= ngon_fragment_index < ngon_edge_count
    bGridOrder - [in]
      Order for face_pack_rect_corners[].
    face_pack_rect_corners - [in]
      The points returned by MYON_SubDFace::PackRectCorner(bGridOrder, i)
    face_pack_rect_size - [in]
      The n-gon face's MYON_SubDFace::PackRectSize() value.
    ngon_grid_size - [in]
    ngon_sub_pack_rect_size - [in]
    ngon_sub_pack_rect_delta - [in]
      These three parameters are values from MYON_SubDFace::GetNgonSubPackRectSizeAndDelta().
      ngon_grid_size, ngon_sub_pack_rect_size, and ngon_sub_pack_rect_delta are identical
      for all the MYON_SubDMeshFragments used to render the n-gon face. The value of
      m_face_fragment_index determines which sub pack rect is assigned to each of
      the n MYON_SubDMeshFragments use to render the n-gon face.
    fragment_pack_rect_corners - [out]
  */
  static bool GetNgonFaceFragmentPackRectCorners(
    unsigned int ngon_edge_count,
    unsigned int ngon_fragment_index,
    bool bGridOrder,
    const MYON_2dPoint face_pack_rect_corners[4],
    MYON_2dVector face_pack_rect_size,
    MYON_2udex ngon_grid_size,
    MYON_2dVector ngon_sub_pack_rect_size,
    MYON_2dVector ngon_sub_pack_rect_delta,
    MYON_2dPoint fragment_pack_rect_corners[4]
  );

public:
  ///////////////////////////////////////////////////////////////////////////////////
  //
  // 3-dimensional grid vertex points
  //
  // Depending on the strides, m_P[], m_N[], and m_T[] can be separate or interlaced.
  //
  // The stride m_P_stride and memory m_P references is managed by some other class or function.
  // Never modify m_P_stride, m_P, or the values in m_P.
  // Use m_grid functions to get point indices and quad face indices.
  double* m_P; // surface points
  size_t m_P_stride; // stride between points for m_P[] as an array of 8 byte doubles (so 0 or >= 3)
  const double* PointArray(MYON_SubDComponentLocation subd_appearance)const;
  size_t PointArrayStride(MYON_SubDComponentLocation subd_appearance)const;
  unsigned PointArrayCount(MYON_SubDComponentLocation subd_appearance) const;

public:

  /*
  Description:
    Create a one quad fragment with 
    m_P = this->m_ctrlnetP[], m_P_stride = 3
    m_N = this->m_ctrlnetN, m_N_stride = 0
    m_T = this->m_ctrlnetT, m_T_stride = 3
    NOTE WELL: 
    1) "this" must remain in scope and unchanged while the returned value
    is used because the returned value references memory in thism_ctrlnetX[] arrays.
    2) The next/prev pointers on the returned fragment are copied from this,
    but the returned value is not reciprocally referenced by next/prev in the linked list.
  Returns:
    A control net quad fragment that can be used locally when the SubDAppearance
    is MYON_SubDComponentLocation::ControlNet.
    The points, normals, and texture_coordinates of the returned fragment
    are the control net quad points, normals, and texture coordinates
    of this fragment. m_grid is a single quad grid.
  */
  const MYON_SubDMeshFragment ControlNetQuadFragmentForExperts() const;

  /*
  Parameters:
    bGridOrder - [in]
      If true, then points are returned in fragment grid order.
      Otherwise, points are returned in counter-clockwise quad order.
    quad_points - [out]
    quad_normal - [out]
  */
  bool GetControlNetQuad(
    bool bGridOrder,
    MYON_3dPoint quad_points[4],
    MYON_3dVector& quad_normal
  ) const;

  const MYON_3dPoint ControlNetQuadPoint(
    bool bGridOrder,
    unsigned point_index
  ) const;

  /*
  Parameters:
    bGridOrder - [in]
      If true, then points are returned in fragment grid order.
      Otherwise, points are returned in counter-clockwise quad order.
    quad_points - [in]
  */
  void SetControlNetQuad(
    bool bGridOrder,
    const MYON_3dPoint quad_points[4],
    MYON_3dVector quad_normal
  );

  void UnsetControlNetQuad();

  const MYON_BoundingBox SurfaceBoundingBox() const;
  const MYON_BoundingBox ControlNetQuadBoundingBox() const;
  const MYON_BoundingBox BoundingBox(MYON_SubDComponentLocation subd_appearance) const;

public:
  /*
  Returns:
    If grid vertex points are available, then VertexCount() is returned.
    Otherwise 0 is returned.
  */
  unsigned int PointCount() const;
  unsigned int PointCapacity() const;

  ///////////////////////////////////////////////////////////////////////////////////
  //
  // 3-dimensional grid vertex surface normals
  //
  // The m_N[] and m_P[] arrays are parallel. 
  // Depending on the strides, m_P[], m_N[], and m_T[] can be separate or interlaced.
  //
  // If m_N is not nullptr and m_N_stride>0, then m_N[] can accommodate up to m_P_capacity normals.
  // If m_N is not nullptr and m_N_stride=0, then m_N[] can accommodate a single normal (flat shaded polygon face).
  // The stride m_N_stride and memory m_N references is managed by some other class or function.
  // Never modify m_N_stride, m_N, or the values in m_N.
  // Use m_grid functions to get normal indices and quad face indices.
  // Note well: m_N_stride can be 0 when the normal is constant (control net face normal for example).
  double* m_N; // surface normals
  size_t m_N_stride; // stride between normals for m_N[] as an array of 8 byte doubles (so 0 or >= 3)
  const double* NormalArray(MYON_SubDComponentLocation subd_appearance)const;
  size_t NormalArrayStride(MYON_SubDComponentLocation subd_appearance)const;
  unsigned NormalArrayCount(MYON_SubDComponentLocation subd_appearance) const;

public:
  /*
  Returns:
    If grid vertex surface normals are available, then VertexCount() is returned.
    Otherwise 0 is returned.
  */
  unsigned int NormalCount() const;
  unsigned int NormalCapacity() const;

  ///////////////////////////////////////////////////////////////////////////////////
  //
  // 3d vertex texture coordinates.
  //
  // Depending on the strides, m_P[], m_N[], and m_T[] can be separate or interlaced.
  //
  // If m_T is not nullptr and m_T_stride>0, then m_T[] can accommodate up to m_P_capacity textures coordinates.
  // Otherwise there are no texture coordinates.
  // Never modify m_T_stride, m_T.
  // Use m_grid functions to get texture indices and quad face indices.
  // Note well: m_T_stride can be 0 when the texture coordinate is constant (one color per face for example)
  mutable double* m_T;
  mutable size_t m_T_stride; // stride between texture points for m_T[] as an array of 8 byte doubles (so 0 or >= 3)

  const double* TextureCoordinateArray(MYON_SubDComponentLocation subd_appearance)const;
  size_t TextureCoordinateArrayStride(MYON_SubDComponentLocation subd_appearance)const;
  unsigned TextureCoordinateArrayCount(MYON_SubDComponentLocation subd_appearance) const;   
public:
  /*
  Returns:
    If grid texture coordinates are available, then VertexCount() is returned.
    Otherwise 0 is returned.
  */
  unsigned int TextureCoordinateCount() const;
  unsigned int TextureCoordinateCapacity() const;


  /*
  Description:
    Sets the values of each fragment's m_ctrlnetT[] member.
  Parameters:
    bSetTextureCoordinates
      If true, the texture coordinate corners are used ti set the fragment's m_T[]
      values after m_ctrlnetT[] is set.
  */
  void SetTextureCoordinateCornersForExperts(
    bool bGridOrder,
    const MYON_3dPoint fragment_texture_coordinate_corners[4],
    bool bSetTextureCoordinates
  ) const;

  /*
  Description:
    This functions sets the texture coordinates on fragments used to render
    quad and 3-gon MYON_SubDFaces.
    A quad MYON_SubDFace is rendered with one MYON_SubDMeshFragment.
    A 3-gon MYON_SubDFace is rendered with three MYON_SubDMeshFragments.
  Remarks:
    It is critical that m_face_fragment_count and m_face_fragment_index be set
    correctly before calling this function.
    A MYON_SubDMeshFragment used to render a quad MYON_SubDFace has
    m_face_fragment_count = 1 and m_face_fragment_index = 0.
    A MYON_SubDMeshFragment used to render a 3-gon MYON_SubDFace has
    m_face_fragment_count = 3 and m_face_fragment_index = 0, 1 or 2.
  */
  void SetQuadOr3gonFaceFragmentTextureCoordinateCorners(
    bool bGridOrder,
    const MYON_3dPoint face_texture_coordinate_corners[4],
    bool bSetTextureCoordinates
  ) const;

  /*
  Description:
    When an MYON_SubDFace is an n-gon with n >= 5, it is rendered with n MYON_SubDMeshFragments.
    This functions sets the texture coordinates on those fragments.    
  Parameters:
    bGridOrder = [in]
      Order for face_pack_rect_corners[].
    face_texture_coordinate_corners - [in]
      The points returned by MYON_SubDFace::TextureCoordinateCorner(bGridOrder, i, false)
    face_pack_rect_size - [in]
      The n-gon face's MYON_SubDFace::PackRectSize() value.
    ngon_grid_size - [in]
    ngon_sub_pack_rect_size - [in]
    ngon_sub_pack_rect_delta - [in]
      These three parameters are values from MYON_SubDFace::GetNgonSubPackRectSizeAndDelta().
      ngon_grid_size, ngon_sub_pack_rect_size, and ngon_sub_pack_rect_delta are identical
      for all the MYON_SubDMeshFragments used to render the n-gon face. The value of
      m_face_fragment_index determines which sub pack rect is assigned to each of
      the n MYON_SubDMeshFragments use to render the n-gon face.
  Remarks:
    It is critical that m_face_fragment_count and m_face_fragment_index be set
    correctly before calling this function.
    A MYON_SubDMeshFragment used to render an n-gon MYON_SubDFace has
    m_face_fragment_count = n and m_face_fragment_index = 0, ..., n-1.
  */
  void SetNgonFaceFragmentTextureCoordinateCorners(
    bool bGridOrder,
    const MYON_3dPoint face_texture_coordinate_corners[4],
    MYON_2dVector face_pack_rect_size,
    MYON_2udex ngon_grid_size,
    MYON_2dVector ngon_sub_pack_rect_size,
    MYON_2dVector ngon_sub_pack_rect_delta,
    bool bSetTextureCoordinates
  ) const;

  /*
  Description:
    Get the texture coordinate corners.
  Parameters:
    bGridOrder - [in]
      If true, then points are returned in fragment grid order.
      Otherwise, points are returned in counter-clockwise quad order.
    texture_coordinate_corners - [out]
  */
  bool GetTextureCoordinteCorners(
    bool bGridOrder,
    MYON_3dPoint texture_coordinate_corners[4]
  ) const;

  /*
  Description:
    Set the texture coordinates in m_T[] from the values in m_ctrlnetT[].
  */
  void SetTextureCoordinatesFromCorners() const;

  /*
  Description:
    Set the texture coordinates in m_T[] from the values in m_pack_rect[].
  */
  void SetPackedTextureCoordinates() const;

  /*
  Description:
    Set the this fragments texture coordinates in m_T[] to cover (0,1)x(0,1)
  */
  void SetUnpackedTextureCoordinates() const;

private:
  void Internal_SetTextureCoordinatesFromCorners(
    const double* corner0,
    const double* corner1,
    const double* corner2,
    const double* corner3,
    double default_coordinate_value,
    int corner_dim
  ) const;

private:
  ///////////////////////////////////////////////////////////////////////////////////
  //
  // Per vertex colors
  //
  // Depending on the strides, m_P[], m_N[], m_T[] and m_C[] can be separate or interlaced.
  //
  // If m_C is not nullptr and m_C_stride>0, then m_C[] can accommodate up to m_P_capacity 
  // elements.
  // Otherwise there is no per vertex false color.
  // Never modify m_C_stride, m_C.
  // Use m_grid functions to get color indices and quad face indices.
  //
  // m_C[] is rarely used and typically managed with ReserveManagedColorCapacity() / DeleteManagedColorCapacity().
  // NOTE WELL: 
  //   When m_C is interlaced with something containing doubles, m_C_stride must be 
  //   a multiple of 2 to keep the doubles 8 bytes aligned. 
  //   When m_C is not interlaced, m_C_stride is typically 1. If this is confusing,
  //   please learn more about algnment and interlacing before working on this code.
  mutable MYON_Color* m_C;
  mutable size_t m_C_stride; // stride for m_C[] as an array of 4 bytes MYON_Color elements (so 0 or >= 1).

public:
  /*
  Returns:
    If grid vertex colors are available, then VertexCount() is returned.
    Otherwise 0 is returned.
  */
  unsigned int ColorCount() const;
  unsigned int ColorCapacity() const;

  const MYON_Color* ColorArray(MYON_SubDComponentLocation subd_appearance)const;
  size_t ColorArrayStride(MYON_SubDComponentLocation subd_appearance)const;
  unsigned ColorArrayCount(MYON_SubDComponentLocation subd_appearance) const;

  /*
  Returns:
    True if vertex color values are set on this fragment.
  */
  bool ColorsExist() const;

  /*
  Parameters:
    bSetColorsExist - [in]
      True if vertex colors exist.
      False vertex colors do not exist or are no longer valid.
  */
  void SetColorsExist(bool bSetColorsExist) const;

  /*
  Description:
    Set the vertex colors in m_C[] to color.
  Parameters:
    color - [in]
  */
  bool SetColors(
    MYON_Color color
  ) const;

  /*
  Description:
    Set the vertex colors in m_C[] from a callback function.
  Parameters:
  */
  bool SetColorsFromCallback(
    const MYON_MappingTag& fragment_colors_mapping_tag,
    const MYON_SubD& subd,
    MYON__UINT_PTR callback_context,
    const MYON_Color(*color_callback)(
      MYON__UINT_PTR callback_context,
      const MYON_MappingTag& mapping_tag,
      const MYON_SubD& subd,
      MYON_SubDComponentPtr cptr,
      const MYON_3dPoint& P,
      const MYON_3dVector& N,
      const MYON_3dPoint& T,
      const MYON_SurfaceCurvature& K
      )
  )const;

private:
  ///////////////////////////////////////////////////////////////////////////////////
  //
  // Principal curvatures
  //
  // If m_K is not nullptr and m_K_stride>0, then m_K[] can accommodate up to m_P_capacity 
  // principal curvatures and sectional curvatures (four doubles, k1,k2,sx,sy).
  // Otherwise there are no principal curvatures or sectional curvatures.
  // (sx = sectional curvature in grid "x" direction, sy = sectional curvature in grid "y" direction.)
  // At exceptional points, the curvature values may be nan.
  // Never modify m_K_stride, m_K.
  // Use m_grid functions to get principal curvature indices and quad face indices.
  //
  // m_K[] is rarely used and typically managed with ReserveManagedCurvatureCapacity() / DeleteManagedCurvatureCapacity().
  // NOTE WELL:
  //   If m_K[] is interlacesd, the number of bytes between successive elements of m_K[] must be a multiple of 16
  //   because sizeof(m_K) = 16 AND m_K_stride is the stride between elements of m_K[]
  //   This is different than m_P_stride, m_N_stride, and m_T_stride, which count the number of doubles between successive 
  //   points/normals/texture points in m_P[]/m_N[]/m_T[].
  mutable MYON_SurfaceCurvature* m_K;
  size_t m_K_stride; // stride for m_K[] as an array of 16 byte MYON_SurfaceCurvature elements (so 0 or >= 1).

public:
  /*
  Returns:
    If grid vertex principal curvatures and sectional curvatures are available, then VertexCount() is returned.
    Otherwise 0 is returned.
  */
  unsigned int CurvatureCount() const;
  unsigned int CurvatureCapacity() const;

  const MYON_SurfaceCurvature* CurvatureArray(MYON_SubDComponentLocation subd_appearance)const;
  unsigned CurvatureArrayCount(MYON_SubDComponentLocation subd_appearance) const;

  /*
  Description:
    Adds managed curvature capacity to store vertex curvatures.
  */
  bool ReserveManagedCurvatureCapacity() const;

  /*
  Description:
    Deletes capacity added by ReserveManagedCurvatureCapacity().
  */
  void DeleteManagedCurvatureCapacity() const;


  /*
  Returns:
    True if vertex color values are set on this fragment.
  */
  bool CurvaturesExist() const;

  /*
  Parameters:
    bSetCurvaturesExist - [in]
      True if vertex curvatures exist.
      False vertex curvatures do not exist or are no longer valid.
  */
  void SetCurvaturesExist(bool bSetCurvaturesExist) const;

public:

  // Normalized grid parameters useful for appling a texture to the grid are available
  // from m_grid functions.
  
  // Information to resolve m_P[], m_N[], and m_T[] into a grid of NxN quads.
  MYON_SubDMeshFragmentGrid m_grid;

  const MYON_SubDMeshFragmentGrid& Grid(MYON_SubDComponentLocation subd_appearance) const;

  // 3d bounding box of grid vertex points.
  MYON_BoundingBox m_surface_bbox;
public:
  /*
  Parameters:
    display_density - [in]
      Determines grid size
    bCurvatureArray - [in]
      true to include room for the m_K[] array.
  Returns:
    Amount of memory needed for the fragment, the m_P[], m_N[], m_T[], m_C[], 
    and optionally the m_K[] arrays.
  */
  static size_t SizeofFragment(
    unsigned int display_density,
    bool bCurvatureArray
  );

  public:
    void Dump(MYON_TextLog& text_log) const;
};

class MYON_CLASS MYON_SubDManagedMeshFragment : public MYON_SubDMeshFragment
{
public:
  MYON_SubDManagedMeshFragment() MYON_NOEXCEPT;
  ~MYON_SubDManagedMeshFragment() MYON_NOEXCEPT;
  MYON_SubDManagedMeshFragment(const MYON_SubDManagedMeshFragment&) MYON_NOEXCEPT;
  MYON_SubDManagedMeshFragment& operator=(const MYON_SubDManagedMeshFragment&) MYON_NOEXCEPT;

  static MYON_SubDManagedMeshFragment Create(const MYON_SubDMeshFragment& src) MYON_NOEXCEPT;

#if defined(MYON_HAS_RVALUEREF)
  // rvalue copy constructor
  MYON_SubDManagedMeshFragment( MYON_SubDManagedMeshFragment&& ) MYON_NOEXCEPT;

  // rvalue assignment operator
  MYON_SubDManagedMeshFragment& operator=( MYON_SubDManagedMeshFragment&& ) MYON_NOEXCEPT;
#endif

  void Clear() MYON_NOEXCEPT;

  void Destroy() MYON_NOEXCEPT;

  bool ReserveCapacity(
    unsigned int mesh_density
    ) MYON_NOEXCEPT;

private:
  void CopyHelper(const MYON_SubDMeshFragment& src);
  size_t m_storage_capacity = 0;
  double* m_storage = nullptr;
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDMesh
//

/// <summary>
/// MYON_SubDMesh is used to store a high density traditional quad mesh
/// of a SubD surface or a mesh of a SubD control net. 
/// In general, is is better to use an MYON_SubDMeshFragmentIterator(subd)
/// that iterates the MYON_MeshFragments cached on the MYON_SubD. 
/// </summary>
class MYON_CLASS MYON_SubDMesh
{
public:
  static const MYON_SubDMesh Empty;

  /*
  Returns:
    A runtime number that changes if the limit mesh content changes.
    0: Empty limit mesh
  Remarks:
    This is a runtime number. It is not saved in archives and begins
    at 1 with each new runtime instance of the opennurbs library.
  */
  MYON__UINT64 ContentSerialNumber() const;



  /*
  Description:
  Parameters:
    destination_mesh - [in]
      If destination_mesh is not nullptr, then the returned mesh
      will be store here. Otherwise the returned mesh will be 
      allocated with a call to new MYON_Mesh().
  Returns:
    If this limit mesh is not empty, an MYON_Mesh representation of this limit mesh.
    Otherwise, nullptr.
  */
  MYON_Mesh* ToMesh(
    MYON_Mesh* destination_mesh
  ) const;


  /*
  Description:
  Parameters:
    mesh_density - [in]
      Larger numbers return denser meshes.
    destination_mesh - [in]
      If destination_mesh is not nullptr, then the returned mesh
      will be store here. Otherwise the returned mesh will be 
      allocated with a call to new MYON_Mesh().
  Returns:
    If this limit mesh is not empty, an MYON_Mesh representation of this limit mesh.
    Otherwise, nullptr.
  */
  MYON_Mesh* ToMesh(
    unsigned int mesh_density,
    MYON_Mesh* destination_mesh
  ) const;

  /*
  Description:
  Parameters:
    frit - [in]
      A fragment iterator from an MYON_SubDMesh or MYON_SubD.
    mesh_density - [in]
      Larger numbers return denser meshes.
      MinimumMeshDensity() <= mesh_density <= MaximumMeshDensity()
    destination_mesh - [in]
      If destination_mesh is not nullptr, then the returned mesh
      will be store here. Otherwise the returned mesh will be 
      allocated with a call to new MYON_Mesh().
  Returns:
    If this limit mesh is not empty, an MYON_Mesh representation of this limit mesh.
    Otherwise, nullptr.
  */
  static MYON_Mesh* ToMesh(
    class MYON_SubDMeshFragmentIterator& frit,
    unsigned int mesh_density,
    MYON_Mesh* destination_mesh
  );

  MYON_SubDMesh() = default;
  ~MYON_SubDMesh() = default;
  MYON_SubDMesh(const MYON_SubDMesh&) = default;
  MYON_SubDMesh& operator=(const MYON_SubDMesh&) = default;

  
#if defined(MYON_HAS_RVALUEREF)
  // rvalue copy constructor
  MYON_SubDMesh( MYON_SubDMesh&& ) MYON_NOEXCEPT;
  // rvalue assignment operator
  MYON_SubDMesh& operator=( MYON_SubDMesh&& );
#endif

  MYON_SubDMesh Copy() const;

  MYON_SubDMesh& CopyFrom(
    const MYON_SubDMesh& src
    );

  static void Swap(
    MYON_SubDMesh& a,
    MYON_SubDMesh& b
    );

  bool Transform( 
    const MYON_Xform& xform
    );

  MYON_DEPRECATED_MSG("AbsoluteSubDDisplayDensity")
  unsigned int DisplayDensity() const;

  /*
  Returns:
    The absolute subd display density used to create this display mesh.
  */
  unsigned int AbsoluteSubDDisplayDensity() const;
  
  MYON_SubDDisplayParameters DisplayParameters() const;

  unsigned int FragmentCount() const;
  const MYON_SubDMeshFragment* FirstFragment() const; // linked list of mesh fragments

  /*
  Parameters:
    face - [in]
  Returns:
    The first fragment for this face or nullptr if none is found.
    If the face is not a quad, then there will be multiple partial
    fragments for the face and this is the first on. The others
    follow using m_next_fragment.
  */
  const MYON_SubDMeshFragment* FaceFragment(
  const class MYON_SubDFace* face
  ) const;

  /*
  Description:
    Get the limit surface point location and normal for 
    the face's center from the limit mesh grid.
  Parameters:
    face - [in]
    P - [out]
      P = limit surface location or MYON_3dPoint::NanPoint if not available.
    N - [out]
      N = limit surface unit normal or MYON_3dVector::NanVector if not available.
  Returns:
    True if the point and normal were set from the limit mesh fragment.
    False if the limit mesh fragment was not found and nan values were returned.
  */
  bool GetFaceCenterPointAndNormal(
    const class MYON_SubDFace* face,
    double* P,
    double* N
  ) const;

  /*
  Description:
    Get the limit surface point location and normal for 
    the edge's midpoint from the limit mesh grid.
  Parameters:
    edge - [in]
    edge_face_index - [in]
      Index of the face to use for the normal. If the edge is a crease, then
      each attached face may have a different normal. Pass 0 when in doubt.
    P - [out]
      P = limit surface location or MYON_3dPoint::NanPoint if not available.
    N - [out]
      N = limit surface unit normal or MYON_3dVector::NanVector if not available.
  Returns:
    True if the point and normal were set from the limit mesh fragment.
    False if the limit mesh fragment was not found and nan values were returned.
  */
  bool GetEdgeCenterPointAndNormal(
    const class MYON_SubDEdge* edge,
    unsigned int edge_face_index,
    double* P,
    double* N
  ) const;
  
  /*
  Description:
    If the subd referenced by m_subd_ref changes, then call
    Update to update the limit mesh. 
  */
  bool Update(
    bool bShareUpdate
    );

  /*
  Description:
    The MYON__INT_PTRs in the tree are const MYON_SubDMeshFragment* pointers.
  */
  const MYON_RTree& FragmentTree() const;

  /*
  Description:
    Clears everything.
  */
  void Clear();

  /*
  Description:
    If the tree is not needed and memory resources are tight, then call ClearTree()
    to remove the RTree.
  */
  void ClearTree();

  bool IsEmpty() const;

  MYON_BoundingBox BoundingBox() const;

  bool GetTightBoundingBox(
    MYON_BoundingBox& bbox,
    bool bGrowBox,
    const MYON_Xform* xform
    ) const;

  MYON_SubDRef SubDRef() const;
  MYON_SubD SubD() const;

public:
  /*
  Description:
    Pretend this function and MYON_SubDMeshImpl do not exist.
  Returns:
    Something that you are pretending does not exist.
  Remarks:
    It is intentional that the definition of MYON_SubDMeshImpl class is not
    available in the opennurbs library interface (not in a header file).
    The size and design of MYON_SubDMeshImpl will change constantly.
    If you choose to hack and whack so you can dereference an
    MYON_SubDMeshImpl* pointer, then your code will crash unpredictably.
  */
  class MYON_SubDMeshImpl* SubLimple() const;
  unsigned int SubLimpleUseCount() const;

private:
#pragma MYON_PRAGMA_WARNING_PUSH
#pragma MYON_PRAGMA_WARNING_DISABLE_MSC( 4251 ) 
  friend class MYON_SubDMeshImpl;
  // C4251: ... needs to have dll-interface to be used by clients of class ...
  // m_impl_sp is private and all code that manages m_impl_sp is explicitly implemented in the DLL.
private:
  std::shared_ptr< class MYON_SubDMeshImpl > m_impl_sp;
#pragma MYON_PRAGMA_WARNING_POP

};


class MYON_CLASS MYON_SubDSectorSurfacePoint
{
public:
  // For performance reasons, the default the data members are
  // not initialized by the default constructor
  // Use = MYON_SubDSectorSurfacePoint::Unset when initialization is required
  static const MYON_SubDSectorSurfacePoint Unset; // all doubles are MYON_UNSET_VALUE, all pointer are nullptr
  static const MYON_SubDSectorSurfacePoint Nan;   // all doubles are MYON_DBL_QNAN, all pointer are nullptr
  static const MYON_SubDSectorSurfacePoint Zero;  // all doubles are 0.0, all pointer are nullptr
  
  /*
  Returns:
    true if m_limitP[0] is MYON_UNSET_VALUE.
    false otherwise.
  */
  bool IsUnset() const;

  /*
  Returns:
    true if m_limitP[0] is a nan (like MYON_DBL_QNAN).
    false otherwise.
  */
  bool IsNan() const;

  /*
  Returns:
    true if all coordinates are zero.
    false otherwise.
  */
  bool IsZero() const;

  /*
  Returns:
    true if all coordinates are valid doubles and the tangents and normal have at least
    one nonzero coordinate.
    false otherwise.
  */
  bool IsSet(
    bool bUndefinedNormalIsPossible
  ) const;

  bool Transform(
    const MYON_Xform& xform
    );

  /// <summary>
  /// Get a limit surface point.
  /// </summary>
  /// <returns>Limit surface point.</returns>
  const MYON_3dPoint Point() const;

  /// <summary>
  /// Get a limit surface normal.
  /// </summary>
  /// <returns>Limit surface normal vector.</returns>
  const MYON_3dVector Normal() const;

  /// <summary>
  /// Get a limit surface tangent vector.
  /// </summary>
  /// <param name="tangent_index">0 or 1</param>
  /// <returns>Limit surface tangent vector.</returns>
  const MYON_3dVector Tangent(int tangent_index) const;

  // limit surface point, tangents and normal
  double m_limitP[3];  // point
  double m_limitT1[3]; // first unit tangent
  double m_limitT2[3]; // second unit tangent
  double m_limitN[3];  // unit normal (same direction as m_limitT1 x m_limitT2)

  // When an MYON_SubDVertex has a single sector, (MYON_SubDVertex.IsSingleSectorVertex() is true),
  // these pointers are both null.
  // When an MYON_SubDVertex has a multiple sectors, 
  // m_sector_face is the "first" face in the sector and
  // m_next_sector_limit_point is used to create a linked list.
  // (The "first" face in a sector is the one MYON_SubDSectorIterator.IncrementToCrease(-1) returns.)
  const class MYON_SubDSectorSurfacePoint* m_next_sector_limit_point; // nullptr for vertices with one sector
  const class MYON_SubDFace* m_sector_face;                         // nullptr for vertices with one sector
};

////////////////////////////////////////////////////////////////////////////
//
// MYON_SubDComponentBase
//
class MYON_CLASS MYON_SubDComponentBase
{
public:
  static const MYON_SubDComponentBase Unset;
  
  /*
  Returns:
    True if this component is active in its parent subd or other
    relevant context.
  Remarks:
    When a component is in use, IsActive() = true. 
    If was used and then deleted, IsActive() is false.
  */
  bool IsActive() const;
    
public:
  MYON_SubDComponentBase() = default;
  ~MYON_SubDComponentBase() = default;
  MYON_SubDComponentBase(const MYON_SubDComponentBase&) = default;
  MYON_SubDComponentBase& operator=(const MYON_SubDComponentBase&) = default;

public:
  static int CompareId(
    const MYON_SubDComponentBase* lhs,
    const MYON_SubDComponentBase* rhs
  );


public:
  // The audience for this comment is anybody who wants to change the data
  // fields in MYON_SubDComponentBase. Everyone else should ignore this comment.
  //   MYON_SubD components come from MYON_FixedSizePool and MYON_SubD code
  //   uses MYON_FixedSizePool.ReturnElement.  The first sizeof(void*) bytes
  //   must be a data field that is not referenced in returned elements.
  //   Since a returned element cannot have a "next level vertex",
  //   m_subd_point1 is a good data member to put first.

  // m_subd_point1 points to the next level's vertex when this component
  // has been subdivided using an algorithm like Catmull-Clark or Loop-Warren.
  const class MYON_SubDVertex* m_subd_point1 = nullptr;

public:
  // The audience for this comment is anybody who wants to change the data
  // fields in MYON_SubDComponentBase. Everyone else should ignore this comment.
  //   It is critical that the offset of m_id in MYON_SubDComponentBase be >= sizeof(void*).
  //   MYON_SubD components come from MYON_FixedSizePool and MYON_SubD code
  //   use MYON_FixedSizePool.ElementFromId and MYON_FixedSizePool.ReturnElement.
  //   Once assigned, m_id is never changed and that allows MYON_SubD component
  //   indices to work.

  // Id assigned to this component. NEVER MODIFY THE m_id VALUE.  
  // It is assigned by allocators and used to find the component 
  // from an MYON_COMPONENT_INDEX.
  unsigned int m_id = 0;

private:
  // The m_archive_id must be immediately after the m_id field.
  // A value of MYON_UNSET_UINT_INDEX indicate the component was
  // in use and then deleted. See MYON_SubDHeap Return...() functions
  // for more details.
  mutable unsigned int m_archive_id = 0;  
  
public:

  /*
  Returns:
    Subdivision level (0 to 255)
  */
  unsigned const SubdivisionLevel() const;

  /*
  Parameters:
    level - [in]
      Subdivision level (0 to 255)
  */
  void SetSubdivisionLevel(unsigned level);

  
public:
  const MYON_ComponentStatus Status() const;


public:
  mutable MYON_ComponentStatus m_status = MYON_ComponentStatus::NoneSet;

  /*
  Returns:
    The current value of the component mark ( m_status->RuntimeMark() ).
  Remarks:
    SubD components have a mutable runtime  mark that can be used
    in any context where a single thread cares about the marks.
    Any code can use Mark() at any time. You cannot assume that
    other functions including const will not change its value.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specific state.
  */
  bool Mark() const;

  /*
  Description:
    Clears (sets to false) the value of the component mark.
  Remarks:
    SubD components have a mutable runtime  mark that can be used
    in any context where a single thread cares about the marks.
    Any code can use Mark() at any time. You cannot assume that
    other functions including const will not change its value.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specific state.
  Returns:
    Input value of Mark().
  */
  bool ClearMark() const;

  /*
  Description:
    Sets (sets to true) the value of the component mark.
  Remarks:
    SubD components have a mutable runtime  mark that can be used
    in any context where a single thread cares about the marks.
    Any code can use Mark() at any time. You cannot assume that
    other functions including const will not change its value.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specific state.
  Returns:
    Input value of Mark().
  */
  bool SetMark() const;

  /*
  Description:
    Sets the value of the component mark to bMark.
  Parameter:
    bMark - [in]
  Remarks:
    SubD components have a mutable runtime  mark that can be used 
    in any context where a single thread cares about the marks.
    Any code can use Mark() at any time. You cannot assume that
    other functions including const will not change its value.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specific state.
  Returns:
    Input value of Mark().
  */
  bool SetMark(
    bool bMark
  ) const;

  /*
  Returns:
    The current value of the component mark bits ( m_status->MarkBits() ).
  Remarks:
    Mark() and MarkBits() are independent.

    SubD components have a mutable runtime mark bits that can be used
    in any context where a single thread cares about the mark bits value.
    Any code can use MarkBits() at any time. You cannot assume that
    other functions including const will not change their value.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specific state. 

    MarkBits() is used in more complex calculations where the single true/false
    provided by Mark() is not sufficient. Typically MarkBits() is used when
    a collecection of components needs to be partitioned into more than two 
    sets or when the value of Mark() cannot be changed.
  */
  MYON__UINT8 MarkBits() const;

  /*
  Returns:
    Set the component mark bits ( m_status->SetMarkBits( mark_bits ) ).
  Remarks:
    Mark() and MarkBits() are independent.

    SubD components have a mutable runtime mark bits that can be used
    in any context where a single thread cares about the mark bits value.
    Any code can use MarkBits() at any time. You cannot assume that
    other functions including const will not change their value.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specific state.

    MarkBits() is used in more complex calculations where the single true/false
    provided by Mark() is not sufficient. Typically MarkBits() is used when
    a collecection of components needs to be partitioned into more than two
    sets or when the value of Mark() cannot be changed.
  Returns:
    Input value of MarkBits().
  */
  MYON__UINT8 SetMarkBits(
    MYON__UINT8 mark_bits
  ) const;

  /*
  Returns:
    Set the component mark bits to 0 ( m_status->SetMarkBits( 0 ) ).
  Remarks:
    Mark() and MarkBits() are independent.

    SubD components have a mutable runtime mark bits that can be used
    in any context where a single thread cares about the mark bits value.
    Any code can use MarkBits() at any time. You cannot assume that
    other functions including const will not change their value.
    It is widely used in many calculations to keep track of sets of
    components that are in a certain context specific state.

    MarkBits() is used in more complex calculations where the single true/false
    provided by Mark() is not sufficient. Typically MarkBits() is used when
    a collecection of components needs to be partitioned into more than two
    sets or when the value of Mark() cannot be changed.
  Returns:
    Input value of MarkBits().
  */
  MYON__UINT8 ClearMarkBits() const;


public:
  
  //////////////////////////////////////////////////////////////
  //
  // Saved subdivision point
  //
  /*
  Description:
    Set the saved subdivision point.
  Parameters:
    subdivision_point - [in]
      includes displacement if it exists
  Returns:
    true if successful
  */
  bool SetSavedSubdivisionPoint(
    const double subdivision_point[3]
    ) const;

  bool GetSavedSubdivisionPoint(
    double subdivision_point[3]
    ) const;

  /*
  Description:
    Clears saved subdivision and limit surface information for this component.
    Attached components are not modified.
  */
  void ClearSavedSubdivisionPoint() const;

  /*
  Returns:
    Saved subdivision point. If a point has not been saved,
    MYON_3dPoint::NanPoint is returned.
  */
  const MYON_3dPoint SavedSubdivisionPoint() const;

  /*
  Returns:
    True if the subdivision point is cached.
  */
  bool SavedSubdivisionPointIsSet() const;

  MYON_DEPRECATED_MSG("Does nothing. Returns false.")
  bool SetSubdivisionDisplacement(const double*);

  MYON_DEPRECATED_MSG("Does nothing. Returns false.")
  bool GetSubdivisionDisplacement(double*) const;
  
  MYON_DEPRECATED_MSG("Does nothing. Returns nans.")
  const MYON_3dVector SubdivisionDisplacement() const;

  MYON_DEPRECATED_MSG("Does nothing. Returns false.")
  bool SubdivisionDisplacementIsNonzero() const;

  MYON_DEPRECATED_MSG("Does nothing.")
  void ClearSubdivisionDisplacement() const;

protected:
  friend class MYON_Internal_SubDFaceMeshFragmentAccumulator;
  friend class MYON_SubDHeap;
  enum SavedPointsFlags : unsigned char
  {
    // if ( 0 != (m_saved_points_flags & SubdivisionPointBit), then m_cache_subd_P is set.
    SubdivisionPointBit = 0x40,

    // if ( 0 != (m_saved_points_flags & SurfacePointBit), then MYON_SubDVertex.m_limit* values are set.
    // MYON_SubDVertex: Set means one or more sector limit surface points are saved in MYON_SubDVertex.m_limit_point.
    // MYON_SubDEdge: Set means the limit surface NURBS curve control points are cached.
    // MYON_SubDFace: Set means limit surface mesh fragments are saved in MYON_SubDFace.m_surface_mesh_fragments.
    // Unset means any information in the fields identified above is invalid and must be recalculated.
    SurfacePointBit = 0x80,

    // SubdivisionPointBit | SurfacePointBit
    CachedPointMask = 0xC0
  };

  enum ModifiedFlags : unsigned char
  {
    // if ( 0 != (m_saved_points_flags & Modified1Bit), then the component has been modified and
    // cached subdivision information needs to be recalculated.
    Modified1Bit = 0x01,

    // if ( 0 != (m_saved_points_flags & Modified2Bit), then the component is adjacent to
    // a modified component and cached subdivision information needs to be recalculated.
    Modified2Bit = 0x02,

    // ModifiedFlagsMask = Modified1Bit | Modified2Bit
    // if ( 0 != (m_saved_points_flags & ModifiedFlagsMask), then any cached subdivision information
    // on that component needs to be recalculated.
    ModifiedFlagsMask = 0x03
  };

  // m_saved_points_flags is a bit field based on MYON_SubDComponentBase::SavePointsFlags values.
  // GetSurfacePoint( bUseSavedSurfacePoint=true ) can change the value of m_saved_points_flags
  void Internal_SetSavedSurfacePointFlag(bool bSavedSurfacePointFlag) const;
  void Internal_SetModified1Flag() const;
  void Internal_SetModified2Flag() const;

  /*
  Returns:
    True if Modified1Bit or Modified2Bit is set.
  */
  bool Internal_Modified1IsSet() const;

  /*
  Returns:
    True if Modified1Bit or Modified2Bit is set.
  */
  bool Internal_Modified1or2IsSet() const;

  void Internal_ClearModifiedFlags() const;
  mutable unsigned char m_saved_points_flags = 0U;

  unsigned char m_level = 0U;
public:

  // All the faces with the same nonzero value of m_group_id are in the same "group".
  // SDK interface on MYON_SubD will be added after we get a better idea of how this
  // feature will be used.
  mutable unsigned int m_group_id = 0U;

protected:
  /*
  Description:
    Clears the flags indicating the saved subdivision point and surface point information 
    is current.
  */
  void Internal_ClearSubdivisionPointAndSurfacePointFlags() const;

  /*
  Description:
    Clears the flag indicating the saved subdivision point is current.
  */
  void Internal_ClearSubdivisionPointFlag() const;

  /*
  Description:
    Clears the flag indicating the saved surface point information is current.
  */
  void Internal_ClearSurfacePointFlag() const;

  bool Internal_SubdivisionPointFlag() const;

  bool Internal_SurfacePointFlag() const;

  void Internal_TransformComponentBase(bool bTransformationSavedSubdivisionPoint, const class MYON_Xform& xform);
  // GetSubdivisionPoint( bUseSavedSubdivisionPoint=true ) can change the value of m_cache_subd_P
  mutable double m_saved_subd_point1[3]; // saved subdivision point

private:
  // Reserved for future use for attributes that apply to allSubD components (MYON_SubDVertex, MYON_SubDEdge, and MYON_SubDFace).
  MYON__UINT64 m_reserved8bytes1;
  MYON__UINT64 m_reserved8bytes2;
  MYON__UINT64 m_reserved8bytes3;

public:
  /*
  Description:
    Pretend ArchiveId() and SetArchiveId() do not exist.
  Returns:
    The ArchiveId is a value set and used by MYON_BinaryArchive Read() and Write()
    functions and copy constructors and operator=(). 
    A public interface is supplied because it is not practical to use friends.
  Remarks:
    A value of MYON_UNSET_UINT_INDEX indicates the component is not in use.
  */
  unsigned int ArchiveId() const
  {
    return m_archive_id;
  }

  void SetArchiveId(
    unsigned int archive_id
    ) const
  {
    // m_archive_id is mutable
    if ( MYON_UNSET_UINT_INDEX != archive_id )
      m_archive_id = archive_id; 
  }

protected:
  void CopyBaseFrom(
    const MYON_SubDComponentBase* src,
    bool bCopySymmetrySetNext
    );

public:
  /*
  Returns:
    True if this component is marked as a primary motif component.
  Remarks:
    You must use MYON_SubD SymmetrySet member functions to get symmetry set contents.
  */
  bool IsSymmetrySetPrimaryMotif() const;

  /*
  Returns:
    True if this component is marked being in a symmetry set.
  Remarks:
    You must use MYON_SubD SymmetrySet member functions to get symmetry set contents.
  */
  bool InSymmetrySet() const;

private:
  // Symmetry sets are a linked loops of components order so that symmetry(component) = next component.
  // There is exactly one motif in each symmetry set.
  // The motif component is marked with 1 == m_symmetry_set_next.ComponentDirection(). 
  // The next component in the symmetry set loop is m_symmetry_set_next.Vertex()/Edge()/Face().
  // When a symmetry set is a singleton (fixed component in the symmetry), this = m_symmetry_set_next
  // and m_symmetry_set_next.ComponentDirection()= 1.
  // The only safe way query, set, and clear symmetry set information is to use
  // MYON_SubD.*SymmetrySet*() functions. Any other technique is not supported and will cause crashes.
  friend class MYON_SubDArchiveIdMap;
  MYON_SubDComponentPtr m_symmetry_set_next;
};

////////////////////////////////////////////////////////////////////////////
//
// MYON_SubDVertexEdgeProperties
//
class MYON_CLASS MYON_SubDVertexEdgeProperties
{
public:
  MYON_SubDVertexEdgeProperties() = default;
  ~MYON_SubDVertexEdgeProperties() = default;
  MYON_SubDVertexEdgeProperties(const MYON_SubDVertexEdgeProperties&) = default;
  MYON_SubDVertexEdgeProperties& operator=(const MYON_SubDVertexEdgeProperties&) = default;

public:
  static const MYON_SubDVertexEdgeProperties Zero; // all member values are zero.

  /*
  Returns:
    True if there are no null edges and there are two edges with a single face and all remaining edges have two faces.
  Remarks:
    Tags are ignored.
  */
  bool HasInteriorVertexTopology() const;

  /*
  Returns:
    True if there are no null edges, exactly two boundary edges, and any other edges have two faces.
  Remarks:
    Tags are ignored.
  */
  bool HasBoundaryVertexTopology() const;

  /*
  Returns:
    HasInteriorVertexTopology() || HasBoundaryVertexTopology().
  */
  bool HasManifoldVertexTopology() const;

  /*
  Returns:
    True if there are no null edges and there is an edge with zero faces or an edge with three or more faces.
  Remarks:
    Tags are ignored.
  */
  bool HasNonmanifoldVertexTopology() const;

  /*
  Returns:
    Number of edges.
  */
  unsigned EdgeCount() const;

public:
  // Number of null edges
  unsigned short m_null_edge_count = 0;


  /////////////////////////////////////////////////////
  //
  // Vertex attached component counts
  //

  // vertex->m_edge_count 
  unsigned short m_edge_count = 0;

  // vertex->m_face_count 
  unsigned short m_face_count = 0;

  /////////////////////////////////////////////////////
  //
  // Edge tag counts
  //

  // Number of edges tags MYON_SubDEdgeTag::Unset
  unsigned short m_unset_edge_count = 0;

  // Number of edges tags MYON_SubDEdgeTag::Smooth or MYON_SubDEdgeTag::SmoothX
  unsigned short m_smooth_edge_count = 0;

  // Number of edges tags MYON_SubDEdgeTag::Crease
  unsigned short m_crease_edge_count = 0;

  /////////////////////////////////////////////////////
  //
  // Edge topology counts
  //

  // Number of wire edges (0 attached faces)
  unsigned short m_wire_edge_count = 0;

  // Number of boundary edges (1 attached face)
  unsigned short m_boundary_edge_count = 0;

  // Number of interior edges (2 attached faces)
  unsigned short m_interior_edge_count = 0;

  // Number of nonmanifold edges (3 or more attached faces)
  unsigned short m_nonmanifold_edge_count = 0;


  /////////////////////////////////////////////////////
  //
  // Edge face counts
  //

  // Minimum value of attached edges's m_face_count.
  unsigned short m_min_edge_face_count = 0;

  // Maximum value of attached edges's m_face_count.
  unsigned short m_max_edge_face_count = 0;
};

////////////////////////////////////////////////////////////////////////////
//
// MYON_SubDVertex
//
class MYON_CLASS MYON_SubDVertex : public MYON_SubDComponentBase
{
private:
  friend class MYON_SubDArchiveIdMap;
  friend class MYON_SubDEdge;
  friend class MYON_SubDFace;

public:
  MYON_SubDVertex() = default;
  ~MYON_SubDVertex() = default;
  MYON_SubDVertex(const MYON_SubDVertex&) = default;
  MYON_SubDVertex& operator=(const MYON_SubDVertex&) = default;

public:
  unsigned int VertexId() const;

public:
  /*
  Description:
    Clears saved subdivision and limit surface information for this vertex.
    Attached edges and faces are not modified.
  */
  void ClearSavedSubdivisionPoints() const;  


  /*
  Description:
    Clears saved subdivision and limit surface information for this vertex.
  Parameters:
    bClearNeighborhood - [in]
      If true, all edges and faces attached to this vertex are also cleared.
  */
  void ClearSavedSubdivisionPoints(
    bool bClearNeighborhood
  ) const;

public:
  static const MYON_SubDVertex Empty;  

  bool Write (
    class MYON_BinaryArchive& archive
    ) const;

  static bool Read (
    class MYON_BinaryArchive& archive,
    class MYON_SubD& subd,
    class MYON_SubDVertex*& vertex
    );

  const MYON_wString ToString(
    bool bIncludeControlNetPoint,
    bool bIncludeTopology
  ) const;

  /*
  Parameters:
    bIncludeEdges - [in]
      If true, then attached edges are included.
    bIncludeFaces - [in]
      If true, then attached faces are included.
  Returns:
    A MYON_ComponentStatusLogicalOr() of this vertex's status and the 
    specified attached components.
  See Also:
    MYON_SubDComponentBase::Status()
  */
  const MYON_ComponentStatus NeighborhoodStatusLogicalOr(
    bool bIncludeEdges,
    bool bIncludeFaces
  ) const;

  /*
  Description:
    Apply a tranxfomration matrix to vertex geometry information.
  Parameters:
    bTransformationSavedSubdivisionPoint - [in]
      If the transformation is being applied to every vertex, edge and 
      face in every level of a subdivision object, and the transformation
      is an isometry (rotation, translation, ...), a uniform scale, or a 
      composition of these types, then set 
      bTransformationSavedSubdivisionPoint = true to apply the
      transformation to saved subdivision and saved limit point information.
      In all other cases, set bTransformationSavedSubdivisionPoint = false
      and any saved subdivision points or saved limit points will be
      deleted.  When in doubt, pass false.

    xform - [in]
  */
  bool Transform(
    bool bTransformationSavedSubdivisionPoint,
    const class MYON_Xform& xform
    );

  bool SetControlNetPoint(
    MYON_3dPoint control_net_point,
    bool bClearNeighborhoodCache
  );


  MYON_BoundingBox ControlNetBoundingBox() const;


public:
  const MYON_COMPONENT_INDEX ComponentIndex() const;
  const MYON_SubDComponentPtr ComponentPtr() const;

public:
  // m_prev_vertex, m_next_vertex must be the first data members of MYON_SubDVertex
  const class MYON_SubDVertex* m_prev_vertex = nullptr; // linked list of vertices on this level
  const class MYON_SubDVertex* m_next_vertex = nullptr; // linked list of vertices on this level

public:
  MYON_SubDVertexTag m_vertex_tag = MYON_SubDVertexTag::Unset;

  
private:
  unsigned char  m_reserved1 = 0;
private:
  unsigned short  m_reserved2 = 0;
private:
  unsigned int m_reserved3 = 0;

public:
  unsigned short m_edge_count = 0;
  unsigned short m_face_count = 0;

  unsigned short m_edge_capacity = 0;
  unsigned short m_face_capacity = 0;

public:
  // Array of m_edge_count edges.
  //   m_edge[i].EdgeDirection() indicates which edge end is located at this vertex
  //   If m_edge_capacity > 0, m_edge_capacity is the number of elements that
  //   may be used in m_edges[].
  class MYON_SubDEdgePtr* m_edges = nullptr;

  // Array of m_face_count faces.
  //   If m_face_capacity > 0, m_face_capacity is the number of elements that
  //   may be used in m_faces[].
  const class MYON_SubDFace** m_faces = nullptr;

  /*
  Description:
    Expert user tool to remove an edge from the vertex's m_edges[] array.
  Remarks:
    Does not modify the edge. If the vertex is referenced by the edge,
    then the vertex must be removed from edge's m_vertex[] array.
  */
  bool RemoveEdgeFromArray(
    const class MYON_SubDEdge* f
    );

  /*
  Description:
    Expert user tool to remove a face from the vertex's m_faces[] array.
  Remarks:
    Does not modify the face or any edges. The faces edges must be updated accordingly.
  */
  bool RemoveFaceFromArray(
    const class MYON_SubDFace* f
    );

  /*
  Parameters:
    bApplyInputTagBias - [in]
      If bApplyInputTagBias is true, the current tag value is used
      to choose between possible output results. When in doubt, pass false.
    bReturnBestGuessWhenInvalid
      If bReturnBestGuessWhenInvalid is true and the topology and current edges
      tags do not meet the conditions for a valid vertex, then a best guess for 
      a vertex tag is returned. Otherwise MYON_SubDVertexTag::Unset is returned.
      When in doubt pass false and check for unset before using.
  Returns:
    The suggested value for this vertices tag based on its current tag value and
    its current edges. Assumes the vertex and edge topology are correct and the 
    edge tags are correctly set.
  */
  MYON_SubDVertexTag SuggestedVertexTag(
    bool bApplyInputTagBias,
    bool bReturnBestGuessWhenInvalid
  ) const;

public:
  double m_P[3]; // vertex control net location
  
  ///<summary>
  /// The SubD vertex control net point.
  ///</summary>
  const MYON_3dPoint ControlNetPoint() const;

private:
  // Cached limit point and limit normal
  // GetSurfacePoint( bUseSavedSurfacePoint=true ) can change the value of m_limit_point.
  // If the limit point is set and vertex has a single sector, then 
  // m_limit_point.m_sector_face = nullptr and m_limit_point.m_next_sector_limit_point = nullptr.
  // If the limit point is set and vertex has a multiple sectors, then 
  // m_limit_point.m_sector_face = first face in the sector.
  // If multiple limit points are set, then they are in a linked list
  // traversed using the MYON_SubDSectorSurfacePoint.m_next_sector_limit_point.
  // The second and any additional limit points are managed by a fixed size pool.
  // Calling ClearSurfacePoint() will return these to the pool.
  mutable MYON_SubDSectorSurfacePoint m_limit_point = MYON_SubDSectorSurfacePoint::Unset;

public:
  enum : unsigned int
  {
    MaximumEdgeCount = 0xFFF0U,
    MaximumFaceCount = 0xFFF0U
  };

  static int CompareUnorderedEdges(
    const MYON_SubDVertex* a,
    const MYON_SubDVertex* b
    );

  static int CompareUnorderedFaces(
    const MYON_SubDVertex* a,
    const MYON_SubDVertex* b
    );

  static int CompareUnorderedEdgesAndFaces(
    const MYON_SubDVertex* a,
    const MYON_SubDVertex* b
    );

  ///*
  //Description:
  //  Sort the m_edges[] and m_faces[] arrays so radial groups are together.
  //  After the sorting is completed, m_vertex_edge_order is set to recorded
  //  the current sorting state and its value is returned.  
  //  The sorting is done unconditionally.
  //*/
  //MYON_SubD::VertexEdgeOrder SortEdges();

  unsigned int EdgeCount(
    MYON_SubDEdgeTag edge_tag
    ) const;

  unsigned int EdgeCount() const;

  const class MYON_SubDEdge* Edge(
    unsigned int i
    ) const;

  const MYON_SubDEdgePtr EdgePtr(
    unsigned int i
    ) const;

  MYON__UINT_PTR EdgeDirection(
    unsigned int i
    ) const;

  unsigned int EdgeArrayIndex(
    const MYON_SubDEdge* edge
    ) const;

  /*
  Description:
    Expert user tool to replace reference to old_edge with a reference to new_edge.
  Parameters:
    old_edge = [in]      
      Cannot be nullptr.
    new_edge = [in]
      If new_edge is Null, old_edge is simply removed.
  Returns:
    If the replacement was successful, then the m_edges[] array index where old_edge/new_edge replacement occurred is returned.
    Otherwise MYON_UNSET_UINT_INDEX is returned.
  Remarks:
    No modifications are made to old_edge or new_edge.
  */
  unsigned int ReplaceEdgeInArray(
    const MYON_SubDEdge* old_edge,
    const MYON_SubDEdgePtr new_edge
    );

  unsigned int FaceCount() const;

  const class MYON_SubDFace* Face(
    unsigned int i
    ) const;

  unsigned int FaceArrayIndex(
    const MYON_SubDFace* face
  ) const;

  /*
  Description:
    Expert user tool to replace reference to old_face with a reference to new_face.
  Parameters:
    old_face = [in]      
      Cannot be nullptr.
    new_face = [in]
      If new_face is nullptr, old_face is simply removed.
  Returns:
    If the replacement was successful, then the m_faces[] array index where old_face/new_face replacement occurred is returned.
    Otherwise MYON_UNSET_UINT_INDEX is returned.
  Remarks:
    No modifications are made to old_face or new_face.
  */
  unsigned int ReplaceFaceInArray(
    const MYON_SubDFace* old_face,
    const MYON_SubDFace* new_face
    );

  /*
  Returns:
    True if m_vertex_tag is MYON_SubDVertexTag::Smooth.
  */
  bool IsSmooth() const;

  /// <summary>
  /// Sharp vertices are smooth, crease or dart vertices attached to 
  /// at least one sharp edge. Note that when an edge has variable
  /// sharpness it can have zero sharpness at one end
  /// and nonzero shaprness at the other end.
  /// The bEndCheck parameter controls what type of sharpness query
  /// is performed.
  /// Note that the vertex subdivision point is affected by attached
  /// sharp edges when IsSharp(true) is is true (MYON_Vertex::VertexSharpness() &gt; 0). 
  /// The vertex limit surface point is affected by edge sharpenss 
  /// when IsSharp(false) is true.
  /// See MYON_SubDEdge::IsSharp() for more information about sharp edges.
  /// </summary>
  /// <param name="bEndCheck">
  /// When bEndCheck is false, the check looks for edges with any nonzero sharpness.
  /// When bEndCheck is true, the check looks for edges with nonzero sharpness at this vertex.
  /// </param>
  /// <returns>
  /// True if this vertex is smooth, dart, or crease and is attached 
  /// to at least one sharp edge.
  ///</returns>
  bool IsSharp( bool bEndCheck ) const;

  /// <summary>
  /// Sharp vertices are smooth, crease or dart vertices attached 
  /// to at least one sharp edge with nonzero end sharpness at the vertex.
  /// See MYON_SubDEdge::IsSharp() for more information about sharp edges.
  /// </summary>
  /// <returns>
  /// If the vertex is smooth and and two or more attached edges have positive end sharpness
  /// at this vertex, then the maximum edge end sharpness at this vertex is returned.
  /// If the vertex is a dart or crease and and one or more attached edges have positive end sharpness
  /// at this vertex, then the maximum edge end sharpness at this vertex is returned.
  /// Otherwise 0.0 is returned.
  ///< / returns>
  double VertexSharpness() const;

  /// <summary>
  /// Sharp vertices are smooth, crease or dart vertices attached to 
  /// at least one sharp edge. Note that the end sharpness at a vertex
  /// can be zero.
  /// See MYON_SubDEdge::IsSharp() for more information about sharp edges.
  /// </summary>
  /// <param name="bEndCheck">
  /// When bEndCheck is false, the check looks for edges with any nonzero sharpness.
  /// When bEndCheck is true, the check looks for edges with nonzero sharpness at this vertex.
  /// </param>
  /// <returns>Number of sharp edges attached to this vertex.</returns>
  unsigned int SharpEdgeCount( bool bEndCheck ) const;

  /// <summary>
  /// Get the range of sharpness values assigned to sharp edges 
  /// and return the number of sharp edges.
  /// Sharp vertices are smooth, crease or dart vertices attached to at least one sharp edge
  /// with nonzero sharpness at the vertex. Note that the end sharpness at a vertex
  /// can be zero.
  /// See MYON_SubDEdge::IsSharp() for more information about sharp edges.
  /// </summary>
  /// <param name="bEndCheck">
  /// When bEndCheck is false, the check looks for edges with any nonzero sharpness.
  /// When bEndCheck is true, the check looks for edges with nonzero sharpness at this vertex.
  /// </param>
  /// <param name="sharpness_range">
  /// The range of sharpness values is returned here.
  /// If bEndCheck is true, the minimum and maximum of nonzero attached_edge->EndSharpness() at this vertex is returned in sharpness_range.
  /// If bEndCheck is false, the minimum and maximum of nonzero attached_edge->MaximumEndSharpness() is returned in sharpness_range.
  /// If no sharp edges are attached, then (0,0) is returned.
  /// </param>
  /// <returns>
  /// If bEndCheck is true, the number of edges with nonzero sharpness at this vertex is returned.
  /// If bEndCheck is false, the number of edges attached to this vertex with nonzero sharpness is returned.
  /// </returns>
  unsigned int SharpEdgeCount(bool bEndCheck, MYON_Interval& sharpness_range) const;

  /// <summary>
  /// Sharp vertices are smooth, crease or dart vertices attached to at least one sharp edge
  /// with nonzero sharpness at the vertex.
  /// See MYON_SubDEdge::IsSharp() for more information about sharp edges.
  /// </summary>
  /// <param name="sharp_subdivision_point">If the returned sharpness is &gt; 0,
  /// then the sharp subdivision point is returned. 
  /// When the returned sharpness is &gt; 0 and &lt; 1, 
  /// the final subdivision point is a weighted average of 
  /// sharp_subdivision_point and the ordinary subdivision point.
  /// When the returned sharpness is &gt;= 1, the sharp subdivision point is used
  /// in place of the ordinary subdivision point.
  /// </param>
  /// <returns>
  /// If the vertex is smooth and and two or more attached edges have positive end sharpness
  /// at this vertex, then the maximum edge end sharpness at this vertex is returned.
  /// If the vertex is a dart or crease and and one or more attached edges have positive end sharpness
  /// at this vertex, then the maximum edge end sharpness at this vertex is returned.
  /// Otherwise 0.0 is returned.
  ///< / returns>
  double GetSharpSubdivisionPoint(MYON_3dPoint& sharp_subdivision_point) const;

  /*
  Returns:
    True if m_vertex_tag is MYON_SubDVertexTag::Crease.
  */
  bool IsCrease() const;

  /*
  Returns:
    True if m_vertex_tag is MYON_SubDVertexTag::Corner.
  */
  bool IsCorner() const;

  /*
  Returns:
    True if m_vertex_tag is MYON_SubDVertexTag::Dart.
  */
  bool IsDart() const;

  /*
  Returns:
    True if m_vertex_tag is MYON_SubDVertexTag::Smooth or MYON_SubDVertexTag::Crease.
  */
  bool IsSmoothOrCrease() const;

  /*
  Returns:
    True if m_vertex_tag is MYON_SubDVertexTag::Crease or MYON_SubDVertexTag::Corner.
  */
  bool IsCreaseOrCorner() const;

  /*
  Returns:
    True if m_vertex_tag is MYON_SubDVertexTag::Dart, MYON_SubDVertexTag::Crease or MYON_SubDVertexTag::Corner.
  */
  bool IsDartOrCreaseOrCorner() const;


  /*
  Returns:
    True if m_vertex_tag is MYON_SubDVertexTag::Dart or MYON_SubDVertexTag::Crease
  */
  bool IsDartOrCrease() const;

  /*
  Returns:
    True if m_vertex_tag is MYON_SubDVertexTag::Dart or MYON_SubDVertexTag::Corner
  */
  bool IsDartOrCorner() const;

  /*
  Returns:
    True if m_vertex_tag is MYON_SubDVertexTag::Smooth or MYON_SubDVertexTag::Dart.
  */
  bool IsSmoothOrDart() const;
  /*
  Returns:
    True if m_vertex_tag is MYON_SubDVertexTag::Smooth or MYON_SubDVertexTag::Dart or MYON_SubDVertexTag::Crease.
  */
  bool IsSmoothOrDartOrCrease() const;

  const MYON_SubDVertexEdgeProperties EdgeProperties() const;

  /*
  Parameters:
    eptr0 - [out]
    eptr1 - [out]
      If a vertex has exactly two attached edges, each of which has a single attached face,
      then these edges are returned in the order the appear in the vertex's edge list. 
      (RelativeVertex(0) = this vertex). Otherwise the parameters are set to null.
  Returns:
    True if the vertex has exactly two attached edges, each of which has a single attached face.
    False otherwise.
  */
  bool GetBoundaryVertexEdges(
    MYON_SubDEdgePtr* eptr0,
    MYON_SubDEdgePtr* eptr1
  ) const;

  /*
  Parameters:
    vei0 - [out]
    vei1 - [out]
      If a vertex has exactly two attached edges, each of which has a single attached face,
      then the indices of those edges in the vertex's edge list are returned. 
      Otherwise MYON_UNSET_UINT_INDEX is returned.
  Returns:
    True if the vertex has exactly two attached edges, each of which has a single attached face.
    False otherwise.
  */
  bool GetBoundaryVertexEdgeIndices(
    unsigned* vei0,
    unsigned* vei1
  ) const;

  /*
  Description:
    A "standard" vertex is one where the standard subdivsion matrix for that vertex
    can be used to calculate the subdivision point. 
    This function is designed to be useful for testing and debugging code when
    a certain situation is expected to exist.  It is not used for evaluation
    because it is too slow.

  Returns:
    True if the subdivison point of the vertex can be calculated using the standard
    subdivion matrix for the vertex type and face count.

  Remarks:
    If the vertex is tagged and has multiple sectors, like an interior
    crease or corner vertex, then this function will return false.
    In this situation, it is possible that the vertex, as the center of a
    sector, is standard.
  */
  bool IsStandard() const;

  /*
  Description:
    A single sector vertex is a smooth vertex, a dart vertex, or a manifold boundary
    vertex.
  Returns:
    True if the vertex has a single sector and, consequently, a single
    limit surface normal.

  */
  bool IsSingleSectorVertex() const;

  /*
  Description:
    A manifold boundary vertex is a crease or corner vertex with 2 creased edges, 
    each attached to a single face, and all other edges are smooth edges attached 
    to two faces. There is a single sector at a manifold boundary vertex.
  Returns:
    True if the vertex has a single sector and, consequently, a single
    limit surface normal.

  */
  bool IsManifoldBoundaryVertex() const;

  /*
  Description:
    A vertex has interior vertex topology if
    EdgeCount() >= 2,
    EdgeCount() == FaceCount(),
    and every attached edge has two attached faces.
  Returns:
    True if the vertex has interior vertex toplology.
  Remarks:
    Tags are ignored. This property is often used during construction
    and modification when tags are not set.
  */
  bool HasInteriorVertexTopology() const;

  /*
  Description:
    A vertex has boundary vertex topology if
    EdgeCount() >= 2,
    EdgeCount() == 1+FaceCount(),
    two attached edges are attached to one face,
    the remaining edges are attached to two faces.
  Returns:
    True if the vertex has interior vertex toplology.
  Remarks:
    Tags are ignored. This property is often used during construction
    and modification when tags are not set.
  */
  bool HasBoundaryVertexTopology() const;

  /*
  Returns:
    If this vertex has two boundary edges, they are returned in the pair with
    BoundaryEdgePair().First().EdgePtr().RelativeVetex(0) and
    BoundaryEdgePair().Second().EdgePtr().RelativeVetex(0)
    equal to this vertex.
    Otherwise MYON_SubDComponentPtrPair::Null is returned.
  */
  const MYON_SubDComponentPtrPair BoundaryEdgePair() const;

  /*
  Returns:
    If this vertex has two creased edges, they are returned in the pair.
    Otherwise MYON_SubDComponentPtrPair::Null is returned.
  */
  const MYON_SubDComponentPtrPair CreasedEdgePair(
    bool bInteriorEdgesOnly
  ) const;


  /*
  Returns:
    If this vertex has one creased edge, it is returned.
    Otherwise MYON_SubDEdgePtr::Null is returned.
  */
  const MYON_SubDEdgePtr CreasedEdge(
    bool bInteriorEdgesOnly
  ) const;

  /*
  Description:
    Count creases with specified topology.
  Parameters:
    bCountInteriorCreases - [in]
      Count includes crease edges with 2 faces.
    bCountBoundaryCreases - [in]
      Count includes crease edges with 1 face.
    bCountNonmanifoldCreases - [in]
      Count includes crease edges with 3 or more faces.
    bCountWireCreases - [in]
      Count includes crease edges with 0 faces.
  Returns:
    Number of creased edges with the specified topology.
  */
  const unsigned int CreasedEdgeCount(
    bool bCountInteriorCreases,
    bool bCountBoundaryCreases,
    bool bCountNonmanifoldCreases,
    bool bCountWireCreases
    ) const;

  /*
  Returns:
    Number of creased edges.
  */
  const unsigned int CreasedEdgeCount() const;

  /*
  Parameters:
    bUseSavedSubdivisionPoint - [in]
      If there is a saved subdivision point and bUseSavedSubdivisionPoint
      is true, then the saved value is returned.
    subdivision_point - [out]
      The SubD vertex Catmull-Clark subdivision point is returned here.
  Returns:
    true if successful
  */
  bool GetSubdivisionPoint(
    double subdivision_point[3]
    ) const;

  ///<summary>
  /// The SubD vertex Catmull-Clark subdivision point.
  ///</summary>
  const MYON_3dPoint SubdivisionPoint() const;

  /*
  Description:
    Evaluates the Catmull-Clark subdivision point ignoring all cached information.
    This function is typically used in testing and debugging code and 
    in ordinary cases, it is faster and better to call SubdivisionPoint() 
    or GetSubdivisionPoint().
  Parameters:
    subdivision_point - [out]
      The vertex Catmull-Clark subdivision point is returned here.
  */
  bool EvaluateCatmullClarkSubdivisionPoint(
    double subdivision_point[3]
  ) const;

  /*
  Parameters:
   sector_face - [in]
      A face in the sector of interest
    limit_point - [out]
  Returns:
    true if successful
  */
  bool GetSurfacePoint(
    const MYON_SubDFace* sector_face,
    class MYON_SubDSectorSurfacePoint& limit_point
    ) const;

  bool GetSurfacePoint(
    const MYON_SubDFace* sector_face,
    bool bUndefinedNormalIsPossible,
    class MYON_SubDSectorSurfacePoint& limit_point
    ) const;

  /*
  Description:
    If there is a saved limit surface point, then its location is returned in surface_point[].
  Parameters:
    surface_point - [out]
  Returns:
    True if a saved limit surface point is returned.
    False if there is no saved limit surface point. The input value of surface_point[] is not changed.
  */
  bool GetSavedSurfacePoint(
    double surface_point[3]
  ) const;

  bool GetSurfacePoint(
    double surface_point[3]
  ) const;

  /*
  Returns:
    The SubD surface point.
  */
  const MYON_3dPoint SurfacePoint() const;

  /*
  Parameters:
    sector_face - [in]
      When the vertex is a crease or corner vertex, different sectors typically
      have different normals and you must specify a face to determine the sector.
    bUndefinedNormalPossible - [in]
      If the SubD control net is degenerate around the vertex, the normal may
      be zero. Pass true if you will accept a zero normal vector. Otherwise
      MYON_3dVector::NanVector is returned when a non-zero normal cannot be calculated.
  Returns:
    The SubD surface normal.
  */
  const MYON_3dVector SurfaceNormal(
    const MYON_SubDFace* sector_face,
    bool bUndefinedNormalPossible
  ) const;

  const MYON_SubDSectorSurfacePoint& SectorSurfacePointForExperts() const;

  /*
  Parameters:
    subd_appearance - [in]
      If MYON_SubDComponentLocation::ControlNet, then this->ControlNetPoint() is returned.
      If MYON_SubDComponentLocation::Surface, then this->SurfacePoint() is returned.
      Otherwise MYON_3dPoint::NanPoint is returned.
  */
  const MYON_3dPoint Point(MYON_SubDComponentLocation subd_appearance) const;


  /*
  Description:
    Save limit surface point and limit normal for future use.
  Parameters:
    bUndefinedNormalIsPossible - [in]
      true if an undefined normal might occur
    surface_point - [in]
  Returns:
    true if successful
  Remarks:
    ClearSavedSubdivisionPoints() clears any saved limit points.
  */
  bool SetSavedSurfacePoint(
    bool bUndefinedNormalIsPossible,
    const MYON_SubDSectorSurfacePoint surface_point
    ) const;

  /*
  Description:
    In general, after you modify a vertex you should call VertexModifiedNotification().

    This is an expert user function that clears any saved limit point evaluations for this vertex.
    No saved subdivision points are cleared.
    No modifications are made to attached edges or faces.
  Remarks:
    In general, you should call VertexModifiedNotification() after you modify a vertex.
    Compare with ClearSavedSubdivisionPoints() which clears any subdivision point
    limit point evaluations saved on this vertex.
  */
  void ClearSavedSurfacePoints() const;
   
  /*
  Returns:
    true
      The vertex limit point and at least one sector normal are saved for Catmull-Clark quad subdivision.
  */
  bool SurfacePointIsSet() const;


  const MYON_Plane VertexFrame(
    MYON_SubDComponentLocation subd_appearance
  ) const;


  /*
  Description:
    Call this function if the vertex is modified. 
    It will clear saved subdivision information on the vertex and neighboring faces and edges that needs to be recalculated.
  */
  void VertexModifiedNofification() const;
  
  /*
  Returns:
    Number of edges attached to this vertex with Edge().m_status.RuntimeMark() = true;
  */
  unsigned int MarkedEdgeCount() const;

  /*
  Description:
    Clears all marks on edges.
  Returns:
    true if all edges are not null.
    false if any edges are null.
  */
  bool ClearEdgeMarks() const;

  /*
  Returns:
    Number of faces attached to this vertex with Face().m_status.RuntimeMark() = true;
  */
  unsigned int MarkedFaceCount() const;


  /*
  Description:
    Clears all marks on faces.
  Returns:
    true if all faces are not null.
    false if any faces are null.
  */
  bool ClearFaceMarks() const;

  /*
  Returns:
    Minimum number of edges for any face attached to this vertex.
  */
  unsigned int MinimumFaceEdgeCount() const;

  /*
  Returns:
    Maximum number of edges for any face attached to this vertex.
  */
  unsigned int MaximumFaceEdgeCount() const;


  /*
  Returns:
    Minimum number of faces for any edge attached to this vertex.
  */
  unsigned int MinimumEdgeFaceCount() const;

  /*
  Returns:
    Maximum number of faces for any edge attached to this vertex.
  */
  unsigned int MaximumEdgeFaceCount() const;

  /*
  Description:
    Expert user tool to unset MYON_SubEdge.m_sector_coefficent[] values for
    edges attached to this vertex.
  Parameters:
    relative_edge_end_dex - [in]
      0: unset vertex edge sector coefficient at the end where the edges
      attaches to this vertex.
      1: unset vertex edge sector coefficient at the end where the edges
      attaches to the other vertex.
      2: unset vertex edge sector coefficients at both ends of the edge.
  */
  void UnsetSectorCoefficientsForExperts(
    unsigned int relative_edge_end_dex
  ) const;

private:
  static bool Internal_GetCatmullClarkSubdivisionPoint(
    const class MYON_SubDVertex* vertex, // smooth or dart
    double vertex_point[3]
    );

  static unsigned int Internal_GetFacePointSum(
    const MYON_SubDFace* face,
    const MYON_SubDVertex* vertex,
    double* facePsum // sum of points that are not immediately adjacent to vertex
    );


  /*
  Description:
    Used for smooth and dart vertices when there are faces 
    that use the vertex have different numbers of sides. 
    This typically happen when a quad subd control net is
    being subdivided for the first time.
  Parameters:
    vertex - [in]
    vertex_point - [out]
  Returns:
    true if successful
  */
  static bool Internal_GetGeneralQuadSubdivisionPoint(
    const class MYON_SubDVertex* vertex,
    double vertex_point[3]
    );

private:
  void CopyFrom(
    const MYON_SubDVertex* src,
    bool bCopyEdgeArray,
    bool bCopyFaceArray,
    bool bCopySurfacePointList,
    bool bCopySymmetrySetNext
    );
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDEdge
//
class MYON_CLASS MYON_SubDEdge : public MYON_SubDComponentBase
{
private:
  friend class MYON_Internal_SubDFaceMeshFragmentAccumulator;
  friend class MYON_SubDHeap;
  friend class MYON_SubDArchiveIdMap;
  friend class MYON_SubDVertex;
  friend class MYON_SubDFace;

public:
  MYON_SubDEdge() = default;
  ~MYON_SubDEdge() = default;
  MYON_SubDEdge(const MYON_SubDEdge&) = default;
  MYON_SubDEdge& operator=(const MYON_SubDEdge&) = default;

public:
  unsigned int EdgeId() const;

public:
  /*
  Description:
    Clears saved subdivision and limit surface information for this edge.
    Attached vertices and faces are not modified.
  */
  void ClearSavedSubdivisionPoints() const;  

  /*
  Description:
    Clears saved subdivision and limit surface information for this edge.
  Parameters:
    bClearNeighborhood - [in]
      If true, all vertices and faces attached to this edge are also cleared.
  */
  void ClearSavedSubdivisionPoints(
    bool bClearNeighborhood
  ) const;

public:
  static const MYON_SubDEdge Empty;
  
  bool Write (
    class MYON_BinaryArchive& archive
    ) const;

  static bool Read (
    class MYON_BinaryArchive& archive,
    class MYON_SubD& subd,
    class MYON_SubDEdge*& edge
    );
  
  /*
  Parameters:
    bIncludeVertices - [in]
      If true, then attached vertices are included.
    bIncludeFaces - [in]
      If true, then attached faces are included.
  Returns:
    A MYON_ComponentStatusLogicalOr() of this vertex's status and the 
    specified attached components.
  See Also:
    MYON_SubDComponentBase::Status()
  */
  const MYON_ComponentStatus NeighborhoodStatusLogicalOr(
    bool bIncludeVertices,
    bool bIncludeFaces
  ) const;

  /*
  Description:
    Apply a tranxfomration matrix to vertex geometry information.

  Parameters:
    bTransformationSavedSubdivisionPoint - [in]
      If the transformation is being applied to every vertex, edge and 
      face in every level of a subdivision object, and the transformation
      is an isometry (rotation, translation, ...), a uniform scale, or a 
      composition of these types, then set 
      bTransformationSavedSubdivisionPoint = true to apply the
      transformation to saved subdivision and saved limit point information.
      In all other cases, set bTransformationSavedSubdivisionPoint = false
      and any saved subdivision points or saved limit points will be
      deleted.  When in doubt, pass false.

    xform - [in]
  */
  bool Transform(
    bool bTransformationSavedSubdivisionPoint,
    const class MYON_Xform& xform
    );

  const MYON_BoundingBox ControlNetBoundingBox() const;

  const MYON_Plane CenterFrame(
    MYON_SubDComponentLocation subd_appearance
  ) const;



  /*
  Description:
    Call this function if the edge is modified and it will clear any
    cached subdivision information that needs to be recalculated.
  */
  void EdgeModifiedNofification() const;

  /*
  Description:
    Expert user tool to unset sector coefficients.
  */
  void UnsetSectorCoefficientsForExperts() const;

  /*
  Description:
    Expert user tool to set mutable sector coefficients.
  Returns:
    True if values were modified.
  */
  bool UpdateEdgeSectorCoefficientsForExperts(
    bool bUnsetEdgeSectorCoefficientsOnly
    ) const;

public:
  const MYON_COMPONENT_INDEX ComponentIndex() const;
  const MYON_SubDComponentPtr ComponentPtr() const;

public:
  // m_prev_edge, m_next_edge must be the first data members of MYON_SubDEdge
  const class MYON_SubDEdge* m_prev_edge = nullptr; // linked list of edges on this level
  const class MYON_SubDEdge* m_next_edge = nullptr; // linked list of edges on this level

public:
  // When checking the edge tag, it is important to consider what
  // should happen in the MYON_SubDEdgeTag::SmoothX case.  It is strongly
  // suggested that you use the member functions MYON_SubDEdge::IsSmooth()
  // and MYON_SubDEdge::IsCrease() instead of comparing m_edge_tag to 
  // MYON_SubDEdgeTag values.
  MYON_SubDEdgeTag m_edge_tag = MYON_SubDEdgeTag::Unset;

private:
  unsigned char m_reserved1 = 0;
  unsigned short m_reserved2 = 0;

public:
  // Array of m_face_count faces.
  //
  //  The first two are in m_face2[0] and m_face2[1].
  //  When m_face_count > 3, the third and additional faces
  //  are in m_facex[0], ..., m_facex[m_face_count-3];
  //
  //  The value of MYON_SubDFacePtr.FaceDirection() is 0 if the
  //  edge's natural orientation from m_vertex[0] to m_vertex[1]
  //  agrees with the face's boundary orientation.
  //
  //  The value of MYON_SubDFacePtr.FaceDirection() is 1 if the
  //  edge's natural orientation from m_vertex[0] to m_vertex[1]
  //  is opposited the face's boundary orientation.
  enum : unsigned int
  {
    MaximumFaceCount = 0xFFF0U
  };
  unsigned short m_face_count = 0;
  unsigned short m_facex_capacity = 0;
  MYON_SubDFacePtr m_face2[2] = {};
  MYON_SubDFacePtr* m_facex = nullptr;

  // m_vertex[0] = vertex at the start of the edge.
  // m_vertex[1] = vertex at the end of the edge.
  const class MYON_SubDVertex* m_vertex[2] = {};

  // NOTE:
  // The sector coefficient is a property of a smooth edge end 
  // that is constant throughout subdivision. It exists at ends of
  // smooth edges that are attached to dart, crease, or corner vertices.
  // In all other cases the sector coefficient is ignored.
  // In particular crease edges and the ends of smooth edges 
  // attached to smooth vertices do not have a sector coefficient.
  // 
  // The MYON_SubDSectorType class provides three static functions that
  // calculate sector coefficients:
  // MYON_SubDSectorType::DartSectorCoefficient()
  // MYON_SubDSectorType::CreaseSectorCoefficient()
  // MYON_SubDSectorType::CornerSectorCoefficient()
  // 
  // If the value of vertex->m_vertex_tag is not MYON_SubDVertexTag::Smooth,
  // then that vertex is "tagged". 
  //
  // If the value of m_edge_tag is MYON_SubDEdgeTag::Crease,
  // then m_sector_coefficient[] should be {0,0}.  
  // In any case m_sector_coefficient[] values are ignored and the
  // midpoint of the edge is the location of the edge.s subdivision point.
  // The edge's subdivision vertex will be tagged as MYON_SubDVertexTag::Crease
  // and both subdivision edges will be tagged as MYON_SubDEdgeTag::Crease.
  //
  // If the value of m_edge_tag is MYON_SubDEdgeTag::Smooth
  // and neither end vertex is tagged, then m_sector_coefficient[] should be {0,0}.  
  // In any case m_sector_coefficient[] values are ignored on smooth edges
  // with smooth vertices at both ends.
  // The edge's subdivision vertex will be tagged as MYON_SubDVertexTag::Smooth
  // and both subdivision edges will be tagged as MYON_SubDEdgeTag::Smooth.
  //
  // If the value of m_edge_tag is MYON_SubDEdgeTag::Smooth and
  // exactly one end vertex is tagged, then the m_sector_coefficient[]
  // value for the tagged end is calculated by MYON_SubDSectorType::SectorCoefficient().
  // tagged_coefficient*tagged_vertex + (1.0 - tagged_coefficient)*untagged_vertex
  // is used when combining the edge ends.
  // The edge's subdivision vertex will be tagged as MYON_SubDVertexTag::Smooth
  // and both subdivision edges will be tagged as MYON_SubDEdgeTag::Smooth.
  //
  // If the value of m_edge_tag is MYON_SubDEdgeTag::SmoothX, then the edge
  // must have exactly two neighboring faces,
  // both vertices must be tagged and the m_sector_coefficient[]
  // values are calculated by MYON_SubDSectorType::SectorCoefficient().
  // When the edge is subdivided, the midpoint of the edge is the 
  // location of the edge.s subdivision point.
  // The edge's subdivision vertex will be tagged as MYON_SubDVertexTag::Smooth
  // and both subdivision edges will be tagged as MYON_SubDEdgeTag::Smooth.
  //
  // If the value of m_edge_tag is MYON_SubDEdgeTag::Smooth
  // and both end vertices are tagged, that is a severe error
  // condition and the edge is subdivided at its midpoint.
  //
  // If the value of m_edge_tag is MYON_SubDEdgeTag::SmoothX
  // and both end vertices are not tagged, that is a severe error
  // condition and the edge is subdivided at its midpoint.
  //
  // m_sector_coefficient[tagged_end] = 1/2 + 1/3*cos(theta_k)
  // where "theta_k" is the tagged end's "theta_k", which depends on the vertex tag (dart/crease/corner), 
  // the number of faces k in the sector, and the control net crease angle alpha when the tagged end is a corner.
  // Dart:   theta_k = 2 * Pi / k
  // Crease: theta_k = Pi / k
  // Corner: theta_k = alpha / k
  //
  // The name "sector coefficient" is used because the value is a property of the
  // vertex's sector (every smooth edge inside a vertex sector has the same value at the tagged vertex).
  // The sector coefficient does not change which a subdivision is applied.
  // 
  // ------------------------------
  // 2022-02-09, Pierre C, RH-67377
  // ------------------------------
  // Wrong values in 3dm files created by OpenNURBS 7.16 and before, fixed in 7.17.
  // 
  // Since 2015-02-20,
  // MYON_SubDSectorType::CornerSectorThetaFromCornerAngle(unsigned int sector_face_count, double corner_sector_angle_radians)
  // had been incorrectly computing theta as:
  // corner_sector_theta = corner_sector_angle_radians / (2 * sector_face_count)
  // instead of:
  // corner_sector_theta = corner_sector_angle_radians / sector_face_count.
  // 
  // This is fixed as of Rhino 7.17, but 3dm files created before this version saved the incorrect value.
  // 
  // This changes the coefficients saved in the SubD edge, and in the 3dm files.
  // Rhino recomputes all m_sector_coefficients cached in the SubD edges when
  // adding a SubD to a document, so all values are now correct. This changes 
  // SubD subdivision, meshing, limit surface, and limit point editing results, 
  // for SubDs that have a smooth edge connected to a corner vertex.
  // 
  // If you are using this value from a SubD in a 3dm file created by Rhino 7.16 or earlier,
  // without adding the SubD to a Rhino document using CRhinoSubDObject::SetSubD(MYON_SubDRef subd_ref),
  // you need to recompute all sector coefficients using subd->UpdateAllTagsAndSectorCoefficients(true);
  mutable double m_sector_coefficient[2] = {};

private:
  // For a smooth edge, m_sharpness is the edge's sharpness.
  // Edge sharpenss has no meaning for edges with a crease tag.
  // MYON_SubDEdge::Sharpness() comments for details. 
  MYON_SubDEdgeSharpness m_sharpness = MYON_SubDEdgeSharpness::Zero;

public:

  /// <summary>
  /// Gets the edge's sharp subdivision point and returns the average of the edge's sharpnesses.
  /// The final subdivision point is (sharpness &gt;= 1.0) ? sharp_subdivision_point : (1.0-sharpness)(smooth subdivsion point)+sharpness*sharp_subdivision_point.
  /// See MYON_SubDEdge::IsSharp() for more information about sharp edges.
  /// </summary>
  /// <param name="sharp_subdivision_point">If the returned sharpness is &gt; 0,
  /// then the sharp subdivision point is returned. 
  /// When the returned sharpness is &gt; 0 and &lt; 1, 
  /// the final subdivision point is a weighted average of 
  /// sharp_subdivision_point and the ordinary smooth subdivision point.
  /// When the returned sharpness is &gt;= 1, the sharp subdivision point is used
  /// in place of the smooth subdivision point.
  /// </param>
  /// <returns>
  /// If the edge is sharp, a value &gt; 0 and &lt; MYON_SubDEdgeSharpness::Maximum is returned.
  /// If the edge is smooth with zero sharpness, 0 is returned.
  /// If the edge is a crease, 0 is returned.
  /// </returns>
  double GetSharpSubdivisionPoint(
    MYON_3dPoint& sharp_subdivision_point
  ) const;

private:
  // Cached limit curve
  // GetEdgeSurfaceCurveControlPoints( bUseSavedSurfacePoint=true ) can change the value of m_limit_curve.
  // If 0 != MYON_SUBD_CACHE_LIMIT_FLAG(m_saved_points_flags), then
  // m_limit_curve is the edge's limit surface curve.
  // The memory is managed by the parent MYON_SubD. 
  // If 0 == MYON_SUBD_CACHE_LIMIT_FLAG(m_saved_points_flags),
  // then any information in m_limit_mesh_fragments is dirty
  // and should not be used.
  // ClearSavedSubdivisionPoints() zeros
  // the appropriate bit of m_saved_points_flags.

  mutable class MYON_SubDEdgeSurfaceCurve* m_limit_curve = nullptr;

public:

  /*
  Returns:
    Number of distinct non-nullptr vertices.
    If the edge is valid, this will be 2.
  */
  unsigned int VertexCount() const;

  /*
  Parameters:
    evi - [in]
       0 or 1
  Returns:
    If evi is 0 or 1 and m_vertex[evi] is not nullptr, then m_vertex[evi]->m_id is returned. Otherwise 0 i returned.
  */
  unsigned int VertexId(
    unsigned evi
  ) const;

  /*
  Parameters:
    evi - [in]
       0 or 1
  Returns:
    If evi is 0 or 1, then m_vertex[evi] is returned. Otherwise nullptr is returned.
  */
  const class MYON_SubDVertex* Vertex(
    unsigned evi
  ) const;

  unsigned int VertexArrayIndex(
    const class MYON_SubDVertex* v
  ) const;


  /*
  Description:
    Return the vertex at the other end of the edge.
  Parameters:
    vertex - [in]
      A vertex referenced in the edge's m_vertex[] array.
  Returns:
    If vertex is not nullptr and exactly one of m_vertex[] is equal to vertex,
    then the other m_vertex[] pointer is returned.
    In any other case, nullptr is returned.
  See Also:
    MYON_SubDEdge.NeighborFace()
  */
  const class MYON_SubDVertex* OtherEndVertex(
    const class MYON_SubDVertex* vertex
  ) const;


  unsigned int FaceCount() const;

  /*
  Parameters:
    bRequireSameFaceOrientation - [in]
      If true, the attached faces must use the edge with opposite directions (oriented manifold).
  Returns:
    True if the edge has two distinct faces.
  */
  bool HasInteriorEdgeTopology(
    bool bRequireOppositeFaceDirections
  ) const;

  /*
  Parameters:
    bRequireSameFaceOrientation - [in]
      If true, the attached faces must use the edge with opposite directions (oriented manifold).
  Returns:
    True if the edge has two distinct faces.
  */
  bool HasBoundaryEdgeTopology() const;

  const MYON_SubDFacePtr FacePtr(
    unsigned int i
    ) const;

  const MYON_SubDFacePtr FacePtrFromFace(
    const class MYON_SubDFace* f
    ) const;

  const class MYON_SubDFace* Face(
    unsigned int i
    ) const;

  MYON__UINT_PTR FaceDirection(
    unsigned int i
    ) const;

  unsigned int FaceArrayIndex(
    const class MYON_SubDFace* f
    ) const;

  /*
  Description:
    Expert user tool to remove a face from the edges's face array.
  Remarks:
    Does not modify the face. If the edge is referenced in the face's edge array,
    then the edge must be removed from the face's edge array.
  */
  bool RemoveFaceFromArray(
    const MYON_SubDFace* f
    );

  /*
  Description:
    Expert user tool to remove a face from the edges's face array.
  Remarks:
    Does not modify the face.  If the edge is referenced in the face's edge array,
    then the edge must be removed from the face's edge array.
  */
  bool RemoveFaceFromArray(
    unsigned int i,
    MYON_SubDFacePtr& removed_face
    );

  /*
  Description:
    Expert user tool to add a face from the edges's face array.
  Remarks:
    Does not modify the face.  If the edge is not referenced in the face's edge array,
    then the edge must be inserted in the correct location in the faces array.
    If you are creating a non-manifold SubD, you must first reserve m_facex[]
    capacity by calling MYON_SubD::GrowEdgeFaceArray().
  */
  bool AddFaceToArray(
    MYON_SubDFacePtr face_ptr
  );

  /*
  Description:
    Expert user tool to replace reference to old_face with a reference to new_face.
    Existing orientation is copied.  No changes are made to old_face and new_face and
    their edge references must be updated accordingly.
  Parameters:
    old_face = [in]
      Cannot be nullptr.
    new_face = [in]
      If new_face is nullptr, old_face is simply removed.
  Returns:
    If the replacement was successful, then the m_faces[] array index where old_face/new_face replacement occurred is returned.
    Otherwise MYON_UNSET_UINT_INDEX is returned.
  Remarks:
    No modifications are made to old_face or new_face.
  */
  unsigned int ReplaceFaceInArray(
    const MYON_SubDFace* old_face,
    const MYON_SubDFace* new_face
  );

  /*
  Parameters:
    vertex0 - [in]
    vertex1 - [in]
    bIgnoreOrientation - [in]
      If false, then the returned edge must have vertices (vertex0, vertex1).
      If true, then the returned edge may have vertices (vertex0, vertex1) or (vertex1, vertex0).
  Returns:
    If an edge connects the input vertices, it is returned. Otherwise nullptr is returned.
  */
  static const MYON_SubDEdge* FromVertices(
    const MYON_SubDVertex* vertex0,
    const MYON_SubDVertex* vertex1,
    bool bIgnoreOrientation
  );

  static const MYON_SubDEdgePtr FromVertices(
    const MYON_SubDVertex* vertex0,
    const MYON_SubDVertex* vertex1
  );

  /*
  Parameters:
    i - [in]
      0 or 1.
  Returns:
    If i is 0 or 1 and vertex[i] is set, then vertex[i]->ControlNetPoint() is returned. 
    Otherwise MYON_3dPoint::NanPoint is returned.
  */
  const MYON_3dPoint ControlNetPoint( unsigned int i ) const;

  const MYON_Line ControlNetLine() const;

  /*
  Returns:
    If vertices are set, then the vector from m_vertex[0]->ControlNetPoint() 
    to m_vertex[1]->ControlNetPoint() is returned.
    Otherwise MYON_3dVector::NanVector is returned.
  */
  const MYON_3dVector ControlNetDirection() const;

  /*
  Returns:
    If vertices are set and v is an end of the edge,
    then the vector from v to OtherEndVertex(v) is returned.
    Otherwise MYON_3dVector::NanVector is returned.
  */
  const MYON_3dVector ControlNetDirectionFrom(
    const MYON_SubDVertex* v
  ) const;

  /*
  Description:
    Return the neighboring face.
  Parameters:
    face - [in]
      A face referenced in the edge's m_face2[] array.
    bStopAtCrease - [in]
      If true and if m_edge_tag = MYON_SubDEdgeTag::Crease, 
      then nullptr is returned.
  Returns:
    If the m_face_count = 2,
    m_edge_tag is smooth or x or passes the crease tag test,
    one of m_face2[0,1] points a face, then
    the neighboring face is returned.
    In any other case, nullptr is returned.
  */
  const MYON_SubDFace* NeighborFace(
    const MYON_SubDFace* face,
    bool bStopAtCrease
    ) const;

  /*
  Description:
    Return the neighboring face.
  Parameters:
    face - [in]
      A face referenced in the edge's m_face2[] array.
    bStopAtCrease - [in]
      If true and if m_edge_tag = MYON_SubDEdgeTag::Crease, 
      then nullptr is returned.
  Returns:
    If the m_face_count = 2,
    m_edge_tag is smooth or x or passes the crease tag test,
    one of m_face2[0,1] points a face, then
    the neighboring face is returned.
    In any other case, MYON_SubDFacePtr::Null is returned.
  */
  const MYON_SubDFacePtr NeighborFacePtr(
    const MYON_SubDFace* face,
    bool bStopAtCrease
    ) const;

  /*
  Parameters:
    edge_vertex_index - [in]
      0 or 1 identifying which end of this edge to check.
    i - [in]
      Index of the face in this edge's face array.
  Returns:
    The edge adjacent to this edge in this->Face(i).
    The orientation is with respect to this->Face(i).
  */
  const MYON_SubDEdgePtr AdjacentEdgePtr(
    unsigned int edge_vertex_index,
    unsigned int i
    ) const;

  const MYON_SubDEdge* AdjacentEdge(
    unsigned int edge_vertex_index,
    unsigned int i
    ) const;
  /*
  Returns:
    True if m_edge_tag is MYON_SubDEdgeTag::Smooth or MYON_SubDEdgeTag::SmoothX.
    False in all other cases.
  */
  bool IsSmooth() const;
  
  /*
  Returns:
    True if m_edge_tag is MYON_SubDEdgeTag::Smooth.
  Remarks:
    Expert user function.
    This is used in rare cases when level 0 edges tagged as MYON_SubDEdgeTag::SmoothX
    need special handling in low level evaluation code. Typical SDK level functions
    and anything related to runtime user interface should call IsSmooth().
  */
  bool IsSmoothNotX() const;
  
  /*
  Returns:
    True if m_edge_tag is MYON_SubDEdgeTag::SmoothX.
  Remarks:
    Expert user function.
    This is used in rare cases when level 0 edges tagged as MYON_SubDEdgeTag::SmoothX
    need special handling in low level evaluation code. Typical SDK level functions
    and anything related to runtime user interface should call IsSmooth().
    An edge tagged as "X" can occur at level 0. It is subdivided as a smooth
    vertex and both of its end vertices are tagged as MYON_SubDVertexTag::Crease, 
    MYON_SubDVertexTag::Corner, or MYON_SubDVertexTag::Dart. 
    This tag cannot appear at level N with N >= 1.
  */
  bool IsSmoothX() const;

  /// <summary>
  /// Sharp edges are a blend between smooth edges and crease edges. 
  /// The limit surface has a continuous normal along a sharp edge.
  /// A sharp edge has a smooth tag, 
  /// has sharpness &gt; 0 at at least one end, 
  /// and has sharpness &lt; MYON_SubDEdgeSharpness::Maximum at at least one end.
  /// Sharpness has no meaning for edges with crease tags.
  /// Both sharpness values are zero for an ordinary smooth edge.
  /// Edge sharpness steadily decreases during subdivision and becomes zero after at most MYON_SubDEdgeSharpness::Maximum subdivisions.
  /// </summary>
  /// <returns>True if the edge is tagged as smooth, h and at least one end with sharpness &gt; 0 and &lt; MYON_SubDEdgeSharpness::Maximum.</returns>
  bool IsSharp() const;

  /// <summary>
  /// Determine if an edge is smooth and is not sharp.
  /// </summary>
  /// <returns>(true == IsSmooth() and false == IsSharp())</returns>
  bool IsSmoothNotSharp() const;

  /// <summary>
  /// An expert user function to determine if an edge tag in MYON_SubDEdgeTag::Smooth and is not sharp.
  /// </summary>
  /// <returns>(true == IsSmoothNotX() and false == IsSharp())</returns>
  bool IsSmoothNotXNotSharp() const;

  /// <summary>
  /// Get the edge's sharpness.
  /// See MYON_SubDEdge::IsSharp() for more information about sharp edges.
  /// </summary>
  /// <returns>
  /// If the edge is sharp, the interval[0] value is the sharpness at the start of the edge and 
  /// the interval[1] value is the sharpness at the end of the edge.
  /// If the edge is smooth or a crease, (0,0) is returned.
  /// Otherwise, (0,0) is returned.
  /// </returns>
  const MYON_SubDEdgeSharpness Sharpness() const;

  /// <summary>
  /// Get the edge's sharpness at the end with the specified vertex.
  /// See MYON_SubDEdge::IsSharp() for more information about sharp edges.
  /// </summary>
  /// <param name="v">Vertex at an end of the edge</param>
  /// <returns>
  /// If the edge is sharp, the sharpness at the end with the specified vertex is returned.
  /// If the edge is smooth or a crease, 0 is returned.
  /// Otherwise, 0.0 is returned.
  /// </returns>
  /// <returns>The sharpness at the end of the edge with the specified vertex.</returns>
  double EndSharpness(
    const class MYON_SubDVertex* v
  ) const;

  /// <summary>
  /// Get the edge's sharpness at the end with the specified vertex.
  /// See MYON_SubDEdge::IsSharp() for more information about sharp edges.
  /// </summary>
  /// <param name="evi">End index (0=start or 1=end).</param>
  /// <returns>
  /// If the edge is sharp, the sharpness at the end with the specified by evi is returned.
  /// If the edge is smooth or a crease, 0 is returned.
  /// Otherwise, 0.0 is returned.
  /// </returns>
  /// <returns>The sharpness at the end of the edge specified by evi.</returns>
  double EndSharpness(
    unsigned evi
  ) const;


  /// <summary>
  /// Get the edge sharpenss values for the subdivided edge at the specified end of this edge.
  /// </summary>
  /// <param name="evi">Selects the subdivided edge (0 for the left subdivided edge, 1 for the right subdivided edge) </param>
  /// <param name="bReverseSharpness">Pass true if this edge and the subdividied edge will have opposite orientations. </param>
  /// <returns>Edge sharpness for the subdivided edge.</returns>
  const MYON_SubDEdgeSharpness SubdivideSharpness(
    unsigned evi,
    bool bReverseSharpness
  ) const;

  /// <summary>
  /// Get the edge sharpenss values for the subdivided edge at the specified end of this edge.
  /// </summary>
  /// <param name="end_vertex">One of this edge's vertices used to select the subdivided edge.</param>
  /// <param name="bReverseSharpness">Pass true if this edge and the subdividied edge will have opposite orientations. </param>
  /// <returns>Edge sharpness for the subdivided edge.</returns>
  const MYON_SubDEdgeSharpness SubdivideSharpness(
    const class MYON_SubDVertex* end_vertex,
    bool bReverseSharpness
  ) const;

  /// <summary>
  /// This tool is for expert users and internal use. 
  /// A collection of MYON_SubD::SetEdgeSharpness() functions provide the easiest way to
  /// set and change edge sharpness.
  /// Set the edge sharpness values to (sharpness[0],sharpness[1]).
  /// The interval values must be &gt;= 0 and &lt;= MYON_SubDEdgeSharpness::Maximum.
  /// See MYON_SubDEdge::IsSharp() for more information about sharp edges.
  /// </summary>
  /// <param name="sharpness">End sharpenss values.</param>
  void SetSharpnessForExperts(
    MYON_SubDEdgeSharpness sharpness
  );

  /// <summary>
  /// This tool is for expert users and internal use. 
  /// Use MYON_SubD::ClearEdgeSharpness() to remove all sharp edges from a SubD.
  /// Sets the edge sharpness to MYON_EdgeSharpness::Zero.
  /// </summary>
  /// <returns>True if there were changes to the edge.</returns>
  bool ClearSharpnessForExperts();

  /*
  Returns:
    True if m_edge_tag is MYON_SubDEdgeTag::Crease.
  */
  bool IsCrease() const;

  /*
  Returns:
    True if m_edge_tag is MYON_SubDEdgeTag::Crease and both of its end vertices 
    are tagged as MYON_SubDVertexTag::Crease, or MYON_SubDVertexTag::Corner. 
  */
  bool IsHardCrease() const;

  /*
  Returns:
    True if m_edge_tag is MYON_SubDEdgeTag::Crease and at least one of its end vertices 
    are tagged as MYON_SubDVertexTag::Dart.
  */
  bool IsDartCrease() const;

  /*
  Returns:
    0: end vertices are not tagged as darts
    1: one end vertex is tagged as a dart.
    2: both end vertices are tagged as a darts.
  */
  unsigned int DartCount() const;

  /*
  Returns:
    bitwise or of applicable MYON_ComponentAttributes::EdgeAttributes values.
  Remarks:
    MYON_ComponentAttributes::EdgeAttributes has subsets of mutually exclusive
    edge attributes.  If the edge is valid, then exactly one bit from each 
    mutually exclusive set of properties will be set.
    If an edge is not valie, then all bits for a set may be clear. 
    For example, if the edge has nullptr values in m_vertex[] 
    or the vertex control point locations are unset or nan, 
    then neither the MYON_ComponentAttributes::EdgeAttributes::Open bit
    nor MYON_ComponentAttributes::EdgeAttributes::Closed bit will be set.
  */
  unsigned int EdgeAttributes() const;

  /*
  Parameters:
    subdivision_point - [out]
  Returns:
    true if successful
  */
  bool GetSubdivisionPoint(
    double subdivision_point[3]
    ) const;

  /// <summary>
  /// Get the SubD edge Catmull-Clark subdivision point.
  /// </summary>
  /// <returns>Catmull-Clark edge subdivision point.</returns>
  const MYON_3dPoint SubdivisionPoint() const;

  /*
  Description:
    Evaluates the Catmull-Clark subdivision point ignoring all cached information.
    This function is typically used in testing and debugging code and
    in ordinary cases, it is faster and better to call SubdivisionPoint()
    or GetSubdivisionPoint().
  Parameters:
    subdivision_point - [out]
      The edge Catmull-Clark subdivision point is returned here.
  */
  bool EvaluateCatmullClarkSubdivisionPoint(
    double subdivision_point[3]
  ) const;

  const MYON_3dPoint ControlNetCenterPoint() const;
  const MYON_3dVector ControlNetCenterNormal(
    unsigned int edge_face_index
  ) const;

  bool EdgeSurfaceCurveIsSet() const;



  /*
  Parameters:
    edge_vertex_index - [in]
      0 or 1
    edge_ptr0 - [out]
    edge_ptr1 - [out]
      Crease edges that bound the sector containing this edge.
      The direction value of the edge pointer identifies the end
      of the sector boundary edge this->at m_vertex[edge_vertex_index].
  Returns:
    Number of faces in the sector.
  */
  unsigned int GetSectorBoundaryEdges(
    unsigned int edge_vertex_index,
    MYON_SubDEdgePtr* edge_ptr0,
    MYON_SubDEdgePtr* edge_ptr1
    ) const;

  /*
  Returns:
    Set bTagged[i] = m_vertex[0]->IsDartOrCreaseOrCorner().
    0: bTagged[0] is true and bTagged[1] is false.
    1: bTagged[0] is false and bTagged[1] is true.
    2: bTagged[0] and Tagged[1] are both true.
    3: bTagged[0] and Tagged[1] are both false.
  */
  unsigned int TaggedEndIndex() const;

  /*
  Returns:
    Number of end vertices with Vertex().m_status.RuntimeMark() = true;
  */
  unsigned int MarkedVertexCount() const;

  /*
  Returns:
    Number of faces attached to this edge with Face().m_status.RuntimeMark() = true;
  */
  unsigned int MarkedFaceCount() const;

private:
  static unsigned int GetFacePointSum(
    const MYON_SubDFace* face,
    const MYON_SubDEdge* edge,
    double* facePsum // sum of face vertex points not on the edge
    );

private:
  void CopyFrom(
    const MYON_SubDEdge* src,
    bool bReverseEdge,
    bool bCopyVertexArray,
    bool bCopyFaceArray,
    bool bCopySymmetrySetNext
    );
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDFace
//
class MYON_CLASS MYON_SubDFace : public MYON_SubDComponentBase
{
private:
  friend class MYON_Internal_SubDFaceMeshFragmentAccumulator;
  friend class MYON_SubDHeap;
  friend class MYON_SubDArchiveIdMap;
  friend class MYON_SubDVertex;
  friend class MYON_SubDEdge;

public:
  MYON_SubDFace() = default;
  ~MYON_SubDFace() = default;
  MYON_SubDFace(const MYON_SubDFace&) = default;
  MYON_SubDFace& operator=(const MYON_SubDFace&) = default;

public:
  unsigned int FaceId() const;

public:
  /*
  Description:
    Clears saved subdivision and limit surface information for this face.
    Attached edges and vertices are not modified.
  */
  void ClearSavedSubdivisionPoints() const;

  /*
  Description:
    Clears saved subdivision and limit surface information for this face.
  Parameters:
    bClearNeighborhood - [in]
      If true, all vertices, edges and face attached to this face are also cleared.
  */
  void ClearSavedSubdivisionPoints(
    bool bClearNeighborhood
  ) const;

public:
  static const MYON_SubDFace Empty;

  bool Write(
    class MYON_BinaryArchive& archive
  ) const;

  static bool Read(
    class MYON_BinaryArchive& archive,
    class MYON_SubD& subd,
    class MYON_SubDFace*& face
  );


  /*
  Parameters:
    bIncludeVertices - [in]
      If true, then attached vertices are included.
    bIncludeEdges - [in]
      If true, then attached edges are included.
  Returns:
    A MYON_ComponentStatusLogicalOr() of this vertex's status and the
    specified attached components.
  See Also:
    MYON_SubDComponentBase::Status()
  */
  const MYON_ComponentStatus NeighborhoodStatusLogicalOr(
    bool bIncludeVertices,
    bool bIncludeEdges
  ) const;

  /*
  Description:
    Apply a tranxfomration matrix to vertex geometry information.

  Parameters:
    bTransformationSavedSubdivisionPoint - [in]
      If the transformation is being applied to every vertex, edge and
      face in every level of a subdivision object, and the transformation
      is an isometry (rotation, translation, ...), a uniform scale, or a
      composition of these types, then set
      bTransformationSavedSubdivisionPoint = true to apply the
      transformation to saved subdivision and saved limit point information.
      In all other cases, set bTransformationSavedSubdivisionPoint = false
      and any saved subdivision points or saved limit points will be
      deleted.  When in doubt, pass false.

    xform - [in]
  */
  bool Transform(
    bool bTransformationSavedSubdivisionPoint,
    const class MYON_Xform& xform
  );

  const MYON_BoundingBox ControlNetBoundingBox() const;

  /*
  Parameters:
    vertex_list - [in]
      vertices in face boundary.
      vertex_list[0] can be any vertex in the face boundary and vertex_list[] can traverse the
      boundary in order or reversed.
  Return:
    If there is a face whose boundary vertex list is face_vertices[], then that face is returned and 
    MYON_SubDFacePtr.FaceDirection() indicates the orientation of face_vertices[].
    Otherwise MYON_SubDFacePtr::Null is returned.
  */
  static const MYON_SubDFacePtr FromVertices(
    const MYON_SimpleArray< const MYON_SubDVertex* >& vertex_list
  );
  static const MYON_SubDFacePtr FromVertices(
    const MYON_SubDVertex*const* vertex_list,
    size_t face_vertices_count
  );


  const MYON_COMPONENT_INDEX ComponentIndex() const;
  const MYON_SubDComponentPtr ComponentPtr() const;

  /*
  Description:
    Call this function if the face is modified and it will clear any
    cached subdivision information that needs to be recalculated.
  */
  void FaceModifiedNofification() const;

public:
  // m_prev_face, m_next_face must be the first data members of MYON_SubDFace
  const class MYON_SubDFace* m_prev_face = nullptr;  // linked list of faces on this level
  const class MYON_SubDFace* m_next_face = nullptr;  // linked list of faces on this level

private:
  unsigned int m_reserved = 0;

private:
  // If non zero, m_pack_id identifies the packed group of faces this faces belongs to.
  // Faces that are not quads are never in grouped with other faces.
  // When possible, quads are packed into groups that form larger rectangular regions.
  // MYON_SubD::Packfaces() creates the packing information.
  // Packing information is saved in 3dm files.
  unsigned int m_pack_id = 0;

  // Location of this face's pack rectin normalized coordinates
  // Each face is assigned a unique rectangle in the unit square (0,1)x(0,1). 
  // Groups of quad faces packed together.
  // If faces are added to an existing subd, the pack rects for the entire subd must be recalculated.
  // If faces in a quad group are removed from a subd, the pack rects for the entire subd must be recalculated.
  double m_pack_rect_origin[2] = {MYON_DBL_QNAN};
  double m_pack_rect_size[2] = {MYON_DBL_QNAN};
  unsigned int m_packed_rect_u = 0;
  unsigned int m_packed_rect_v = 0;

  enum PackStatusBits : unsigned char
  {
    ZeroPackStatusBits = 0,

    /////////////////////////////////////////////////////////////////////
    //
    // Pack rectangle status
    //

    // 4 ways the pack rectangle can be rotated for a face when packed texture coordinates are 
    // calculated from the packing rectangle.
    // These enum values must be 0,1,2,3 because these values are used in arithmetic formulae.
    // Rotation is used to keep pac rect coordinates continuous in quad packs containing faces
    // with inconsistent topological orientations, which is the most common situation.
    PackingRotate0 = 0,
    PackingRotate90 = 1,
    PackingRotate180 = 2,
    PackingRotate270 = 3,

    // mask for the bits used to encode the packing rotation
    PackingRotateMask = 3,

    // bit used to determine if the values in m_packed_coordinate_origin[] and m_packed_coordinate_delta[]
    // are set to valid values.
    PackRectSet = 0x04U,

    PackingBitsMask = 0x07U,
    NotPackingBitsMask = 0xF8U,

    // add other non-mutable bits here
  };

  // NOT mutable on purpose
  unsigned char m_pack_status_bits = 0;

private:
  enum TextureStatusBits : unsigned char
  {
    ZeroTextureStatusBits = 0,

    // TexturePoints bits
    TexturePointsSet = 0x01U, // set when face has set custom texture coordinates
    TexturePointsBitsMask = 0x01U,
    NotTexturePointsBitsMask = 0xFEU,

    // add other mutable bits here
  };
  // mutable because rendering code frequently modifis const objects in the Rhino.
  mutable unsigned char m_texture_status_bits = 0;

public:
  /*
  Returns:
    0: unset pack id.
    > 0: set pack id.
  Remarks:
    Faces that share the same PackId() must be neighboring quad faces that form a larger
    rectangular grid of quads. Single faces that are not quads and isolated quads cannot
    share a pack id with other faces.
  */
  unsigned int PackId() const;

  /*
  Returns:
    true if the pack rect is set.
  Remarks:
    Faces that share the same PackId() must be neighboring quad faces that form a larger
    rectangular grid of quads. Single faces that are not quads and isolated quads cannot
    share a pack id with other faces.
  */
  bool PackRectIsSet() const;

  /*
  Returns
    Lower left coordinate of this face's pack rect in normalized pack rect coordinates.
  */
  const MYON_2dPoint PackRectOrigin() const;

  /*
  Returns
    Size of this face's pack rect in normalized pack rect coordinates.
  */
  const MYON_2dVector PackRectSize() const;

  /*
  Returns:
    0, 90, 180, or 270
  Remarks:
    This rotation is used to keep pack rect coordinates continuous in quad packs 
    containing faces with inconsistent topological orientations, which is the most
    common situation.
  */
  unsigned PackRectRotationDegrees() const;

  /*
  Returns:
    0, 0.5*MYON_PI, MYON_PI, or 1.5*MYON_PI
  Remarks:
    This rotation is used to keep pack rect coordinates continuous in quad packs
    containing faces with inconsistent topological orientations, which is the most
    common situation.
  */
  double PackRectRotationRadians() const;

  /*
  Parameters:
    bGridOrder - [in]
      false: counter clockwise quad order.
      true: fragment grid order
    corner_index - [in]
      0, 1, 2, or 3
  Returns:
    Specified pack rectangle corner coordinates.
    The pack rect is the (x0,x0+dx) x (y0,y0+dy) rectangle inside the unit square (0,1)x(0,1)
    where (x0,y0) = PackRectOrigin() and (dx,dy) = PackRectSize().
  */
  const MYON_2dPoint PackRectCorner(
    bool bGridOrder,
    int corner_index
  ) const;

  /*
  Parameters:
    bGridOrder - [in]
      false: counter clockwise quad order.
      true: fragment grid order
    mesh_fragment_pack_rect_corners - [out]
      Pack rectangle corners for the specified mesh fragment are returned here.
      face_pack_rect_corners[i] = this->PackRectCorner(bGridOrder,i);
  Returns:
    True if the input is valid and the face's pack rectangle corner coordinates were returned.
    False otherwise.
  Remarks:
    Compare with MYON_SubDFace.GetMeshFragmentPackRectCorners().
  */
  bool GetFacePackRectCorners(
    bool bGridOrder,
    MYON_2dPoint face_pack_rect_corners[4]
  ) const;

  /*
  Parameters:
    bGridOrder - [in]
      false: counter clockwise quad order.
      true: fragment grid order
    fragment_index - [in]
      If the face is a quad (EdgeCount() = 4), then fragment_index must be zero.
      If the face is a n-gon (EdgeCount() = n and n != 4),
      then 0 <= fragment_index < EdgeCount().
    mesh_fragment_pack_rect_corners - [out]
      Pack rectangle corners for the specified mesh fragment are returned here.
      If the face is a quad, mesh_fragment_pack_rect_corners[] are the pack rect texture points for
      the quad corners in the order specified by bGridOrder and standard linear interpolation
      between the 4 corners will produce reasonable texture mapping coordinates.
      If the face is an n-gon, mesh_fragment_pack_rect_corners[0] is the pack rect texture point
      at face->Vertex(fragment_index). For n-gons, the n fragments are assigned non-overlapping rectangular
      subsetsof the face's pack rect. Interpolating between corner values will not produce good texture
      mapping coordinates. Pack rects are create useful and optimal texture mapping results when the SubD
      is rendered from its mesh fragmantes.
  Returns:
    True if the input is valid and the pack rectangle corner coordinates for the specified mesh fragment were returned.
    False otherwise.
  Remarks:
    A quad face (EdgeCount()=4) is rendered using 1 MYON_SubDMeshFragment.
    An n-gon face (EdgeCount()=n and n != 4) is rendered using n MYON_SubDMeshFragments. These n fragments correspond
    the SubD quads the n-gon face would generate with one level of Catmull-Clark subdivision.
  */
  bool GetMeshFragmentPackRectCorners(
    bool bGridOrder,
    unsigned int fragment_index,
    MYON_2dPoint mesh_fragment_pack_rect_corners[4]
  ) const;

  /*
  Description:
    Sets PackId() to zero.
  Remarks:
    Does not change the values of MYON_SubDFace::PackRectOrigin(), MYON_SubDFace::PackRectSize(), 
    MYON_SubDFace::PackRectRotationDegrees(), or MYON_SubDFace::TextureCoordinateType()
    Use MYON_SubDFace::ClearPackRect() to clear the pack rectangle.
  */
  void ClearPackId();

  /*
  Description:
    Clears the pack rectangle.
  Remarks:
    Does not change the value of MYON_SubDFace::PackId() or MYON_SubDFace::TextureCoordinateType()
    Use MYON_SubDFace::ClearPackId() to clear the pack id.
  */
  void ClearPackRect();

  /*
  Parameters:
    pack_rect_origin - [in]
      Lower left corner.
      Valid origins have (0 <= origin.x < 1) and (0 <= origin.y < 1)
    pack_rect_size - [in]
      vector from lower left corner to upper right corner.
      Valid deltas have (0 < delta.x, 0 < delta.y, (origin.x+delta.x) <= 1) and (origin.y+delta.y) <= 1.
    packing_rotation_degrees - [in]
      Valid packing_rotation_degrees are a multiple of 90.
  Returns:
    True if the input parameters define a valid pack rectangle.
  */
  static bool IsValidPackRect(
    MYON_2dPoint pack_rect_origin,
    MYON_2dVector pack_rect_size,
    int packing_rotation_degrees
  );

  /*
  Description:  
    The MYON_SubD::PackFaces() function uses this function to set the value of MYON_SubDFace::PackId().
    Unless you are an expert and doing something very carefully and very fancy, to not call this function. 
    You must also set the pack rectangle correctly.
  Remarks:
    Faces that share the same PackId() must be neighboring quad faces that form a larger
    rectangular grid of quads. Single faces that are not quads and isolated quads cannot 
    share a pack id with other faces.
  */
  void SetPackIdForExperts(
    unsigned int pack_id
  );

  /*
  Description:
    The MYON_SubD::PackFaces() function uses this function to set the face's pack rectangle
    (MYON_SubDFace::PackRectOrigin(), MYON_SubDFace::PackRectSize(), MYON_SubDFace::PackRectRotationDegrees()).
    Unless you are an expert and doing something very carefully and very fancy, to not call this function.
    The lower left corner will be origin, the upper right corner will be delta.
    You must also set the pack id correctly.
  Parameters:
    pack_rect_origin - [in]
      Lower left corner.
      Valid origins have (0 <= origin.x < 1) and (0 <= origin.y < 1)
    pack_rect_size - [in]
      vector from lower left corner to upper right corner.
      Valid deltas have (0 < delta.x, 0 < delta.y, (origin.x+delta.x) <= 1) and (origin.y+delta.y) <= 1.
    packing_rotation_degrees - [in]
      Valid packing_rotation_degrees are a multiple of 90.
  Return:
    True if input is valid and the pack rectangle was set.
    False if the input was not vaie and the pack rectangle coordinates were set to nan.
  */
  bool SetPackRectForExperts(
    MYON_2dPoint pack_rect_origin,
    MYON_2dVector pack_rect_size,
    int packing_rotation_degrees
  );

  /*
  Description:
    Calculate how a packing rectangle assigned to an MYON_SubDFace will
    be subdivided into n sub packing rectangles for an ngon when n >= 5.
  Parameters:
    ngon_edge_count - [in]
      >= 5.
    ngon_face_pack_rect_size - [in]
      ngon_face_pack_rect_size.x > 0 and ngon_face_pack_rect_size.y > 0
      The width and height of the sizeof the ngon's entire packing rectangle.
      This is typically MYON_SubD_Face::PackRectSize() once that property is 
      coorectly set.
    ngon_sub_pack_rect_size - [out]
      The size of sub pack rects.
      If input is not valid, then MYON_2dVector::ZeroVector is returned.
    ngon_sub_pack_rect_delta - [out]
      The delta from one sub pack rect to the next.
      If input is not valid, then MYON_2dVector::ZeroVector is returned.
  Returns:
    When the input is valid, MYON_2udex(i,j) is returned and specifies the face's packing 
    rectangle should be divided into i X j sub packing rectangles.
    Otherwise, MYON_2udex(0,0) is returned and 
  */
  static const MYON_2udex GetNgonSubPackRectSizeAndDelta(
    unsigned int ngon_edge_count,
    MYON_2dVector ngon_face_pack_rect_size,
    MYON_2dVector& ngon_sub_pack_rect_size,
    MYON_2dVector& ngon_sub_pack_rect_delta
  );

  /*
  Parameters:
    pack_rect_distance_in_pixels - [in]
      A (normalized pack rect distance) * MYON_SubD::TextureImageSuggestedMinimumSize
  Returns:
    Suggested gap between adjacent pack rects for a texture images
    with width and height >= MYON_SubD::TextureImageSuggestedMinimumSize.
    This value will be 0.0 when pack_rect_distance_in_pixels is too
    small to permit a gap of 1 or more pixels without visibly
    adverse effects in a texture map clarity.
  */
  static double PackRectGapInPixels(
    double pack_rect_distance_in_pixels
  );


private:
  // The application specifies a base MYON_Material used to render the subd this face belongs to.
  // If m_material_channel_index > 0 AND face_material_id = base.MaterialChannelIdFromIndex(m_material_channel_index)
  // is not nil, then face_material_id identifies an override rendering material for this face.
  // Otherwise base will be used to render this face.
  mutable unsigned short m_material_channel_index = 0;

public:
  /*
  Description:
    Set the per face rendering material channel index.

  Parameters:
    material_channel_index - [in]
      A value between 0 and MYON_Material::MaximumMaterialChannelIndex, inclusive.
      This value is typically 0 or the value returned from MYON_Material::MaterialChannelIndexFromId().

  Remarks:
    If base_material is the MYON_Material assigned to render this subd and
    MYON_UUID face_material_id = base_material.MaterialChannelIdFromIndex( material_channel_index )
    is not nil, then face_material_id identifies an override rendering material for this face.
    Otherwise base_material is used to reneder this face.
  */
  void SetMaterialChannelIndex(int material_channel_index) const;

  /*
  Description:
    Remove the per face rendering material channel index.
    The face will use the material assigned to the subd object.
  */
  void ClearMaterialChannelIndex() const;

  /*
  Returns:
    This face's rendering material channel index.

    Remarks:
    If base_material is the MYON_Material assigned to render this subd, MaterialChannelIndex() > 0,
    and MYON_UUID face_material_id = base_material.MaterialChannelIdFromIndex( face.MaterialChannelIndex() )
    is not nil, then face_material_id identifies an override rendering material for this face.
    Otherwise base_material is used to reneder this face.
  */
  int MaterialChannelIndex() const;

private:
  // The application specifies a base MYON_Material used to render the subd this face belongs to.
  // If m_material_channel_index > 0 AND face_material_id = base.MaterialChannelIdFromIndex(m_material_channel_index)
  // is not nil, then face_material_id identifies an override rendering material for this face.
  // Otherwise base will be used to render this face.
  mutable MYON_Color m_per_face_color = MYON_Color::UnsetColor;

public:

  /*
  Description:
    Set per face color.

  Parameters:
    color - [in]
  */
  void SetPerFaceColor(
    MYON_Color color
    ) const;

  /*
  Description:
    Remove per face color setting. The face will use the color assigned to the subd object.
  */
  void ClearPerFaceColor() const;

  /*
  Returns:
    Per face color. A value of MYON_Color::UnsetColor indicates the face uses the color assigned to the subd object.
  */
  const MYON_Color PerFaceColor() const;

public:
  unsigned int m_level_zero_face_id = 0;   // id of level zero face

public:
  // Array of m_edge_count edges that form the boundary of the face.
  // The edges are in ordered to form a continuous loop.
  //
  //  The first four are in m_edge4[0], ..., m_edge4[3].
  //  When m_edge_count > 4, the fifth and additional edges
  //  are in m_edgex[0], ..., m_edgex[m_edge_count-5];
  //
  //  The value of MYON_SubDEdgePtr.EdgeDirection() is 0 if the
  //  edge's natural orientation from m_vertex[0] to m_vertex[1]
  //  agrees with the face's boundary orientation.
  //
  //  The value of MYON_SubDEdgePtr.EdgeDirection() is 1 if the
  //  edge's natural orientation from m_vertex[0] to m_vertex[1]
  //  is opposited the face's boundary orientation.
  enum : unsigned int
  {
    MaximumEdgeCount = 0xFFF0U
  };
  unsigned short m_edge_count = 0;
  unsigned short m_edgex_capacity = 0;

  MYON_SubDEdgePtr m_edge4[4];
  MYON_SubDEdgePtr* m_edgex = nullptr;

  /*
  Returns:
    A linked list of limit mesh fragments that cover this face.
    Nullptr if none are currently available.

    If the face is a quad, there is a single fragment. If the
    face is an n-gon with 4 != n, then the list has n fragments.
    The i-th fragment covers the subdivision quad with its 
    corner at Vertex(i).

  Remarks:
    Use the returned fragments immediately or make a 
    copies you manage for later use.

    Use MYON_SubD.UpdateSurfaceMeshCache(false) or MYON_SubD.UpdateSurfaceMeshCache(true)
    to create the face fragments.

    MYON_SubDFace.ClearSavedSubdivisionPoints() removes any saved
    fragments and is called when the face or nearby components
    are modified.
  */
  const class MYON_SubDMeshFragment* MeshFragments() const;


  const MYON_3dPoint ControlNetCenterPoint() const;

  const MYON_3dVector ControlNetCenterNormal() const;

  const MYON_Plane ControlNetCenterFrame() const;

  bool IsConvex() const;

  bool IsNotConvex() const;

  bool IsPlanar(double planar_tolerance = MYON_ZERO_TOLERANCE) const;

  bool IsNotPlanar(double planar_tolerance = MYON_ZERO_TOLERANCE) const;

public:
  /*
  Returns:
    Number of texture points that can be set on this face.
  Remarks:
    To allocate texture point storage, call MYON_SubD.AddFaceTexturePointCapacity(this).
  */
  unsigned int TexturePointsCapacity() const;

  /*
  Returns:
    If texture points are set, then true is returned.
    Otherwise, false is returned.
  */
  bool TexturePointsAreSet() const;

  /*
  Description:
    Set the texture point at the specified face vertex.
  Parameters:
    i - [in]
      0 <= 0 < this->EdgeCount()
  Parameters:
    texture_point - [in]
  Returns:
    If this->TexturePointsCanBeSet() is true and i is a valid index,
    then the texture point is set and true is returned.
    Otherwise, false is returned.
  Remarks:
    To allocate texture point storage, call MYON_SubD.AddFaceTexturePointStorage(this).
    Texture points are a mutable property on MYON_SubDFace.
  */
  bool SetTexturePoint(
    unsigned i,
    MYON_3dPoint texture_point
  ) const;

  /*
  Description:
    Remove all texture points.
  Remarks:
    Texture points are a mutable property on MYON_SubDFace.
  */
  void ClearTexturePoints() const;

  /*
  Description:
    Get the texture point at the specified face vertex.
  Parameters:
    i - [in]
      0 <= 0 < this->EdgeCount()
  Returns:
    If texture
  */
  const MYON_3dPoint TexturePoint(
    unsigned int i
  ) const;

  const MYON_3dPoint TextureCenterPoint() const;

private:
  // If m_texture_points is not nullptr, it has capacity 4 + m_edgex_capacity.
  // Custom texture coordinates are stored in m_texture_points.
  // When texture coordinates are packed or come from a mapping,
  // m_texture_points is not used. Typically m_texture_points
  // is used when an MYON_SubD is created from an MYON_Mesh and the mesh
  // has custom texture coordinates. Here "custom" means
  // not from a mapping.  
  // https://mcneel.myjetbrains.com/youtrack/issue/RH-59465
  mutable MYON_3dPoint* m_texture_points = nullptr;

private:
  // If 0 != MYON_SUBD_CACHE_LIMIT_FLAG(m_saved_points_flags), then
  // m_mesh_fragments is a linked list of (4==m_edge_count?1:m_edge_count)
  // fragments available from MeshFragments() and managed by the parent MYON_SubD. 
  // If 0 == MYON_SUBD_CACHE_LIMIT_FLAG(m_saved_points_flags), then any information
  // in m_limit_mesh_fragments is dirty and should not be used.
  // ClearSavedSubdivisionPoints() zeros the appropriate bit of m_saved_points_flags.

  // Mesh fragment(s) for this face
  mutable class MYON_SubDMeshFragment* m_mesh_fragments = nullptr;

private:

public:
  /*
  Description:
    Returns the number of edges and (number of vertices) in the face's boundary.
  Remarks:
    Boundaries that vist the same vertex or same edge multiple times are not permitted.
    So the number of vertices and number of edges is always the same.
  */
  unsigned int EdgeCount() const;

  /*
  Description:
    Rapidly verifies that:
    1. EdgeCount() >= 3
    2. Every edge is not null and has 2 non-null vertices.
    3. The end vertex of and edge is identical to the start vertex of the next edge.
    4. Every edge has FaceCount() >= 1.
    5. Every vertex has EdgeCount() >= 2 and FaceCount() >= 1.
    6. The first 4 edges are unique.
    6. The first 4 vertices are unique.
  Returns:
    True if the 5 conditions above are true.
  Remarks:
    The face can still be invalid, but if HasValidEdges() returns true,
    it is save to deference pointers returned by the face's Edge() and Vertex() functions.
  */
  bool HasEdges() const;

  const MYON_SubDEdgePtr EdgePtr(
    unsigned int i
    ) const;

  const MYON_SubDEdgePtr EdgePtrFromEdge(
    const class MYON_SubDEdge* e
    ) const;

  bool EdgeMark(
    unsigned int i,
    bool bMissingEgeReturnValue
  ) const;

  MYON__UINT8 EdgeMarkBits(
    unsigned int i,
    MYON__UINT8 missing_edge_return_value
  ) const;


  const class MYON_SubDVertex* Vertex(
    unsigned int i
    ) const;

  const MYON_3dPoint ControlNetPoint(
    unsigned int i
  ) const;

  unsigned int VertexIndex(
    const MYON_SubDVertex* vertex
    ) const;

  bool VertexMark(
    unsigned int i,
    bool bMissingVertexReturnValue
  ) const;

  MYON__UINT8 VertexMarkBits(
    unsigned int i,
    MYON__UINT8 missing_vertex_return_value
  ) const;

  /*
  Returns;
    If the vertex is in this face's boundary, pair of face boundary edges at the vertex is returned 
    with face boundary orientations, that is vertex = pair.First().EdgePtr().RelativeVertex(1)
    and vertex = pair.Second().EdgePtr().RelativeVertex(0). Otherwise, MYON_SubDComponentPtrPair::Null
    is returned.
  */
  const MYON_SubDComponentPtrPair VertexEdgePair(
    const MYON_SubDVertex* vertex
  ) const;

  /*
  Returns;
    If the vertex is in this face's boundary, pair of face boundary edges at the vertex is returned
    with face boundary orientations, that is vertex = pair.First().EdgePtr().RelativeVertex(1)
    and vertex = pair.Second().EdgePtr().RelativeVertex(0). Otherwise, MYON_SubDComponentPtrPair::Null
    is returned.
  */
  const MYON_SubDComponentPtrPair VertexEdgePair(
    unsigned vertex_index
  ) const;

  const class MYON_SubDEdge* Edge(
    unsigned int i
    ) const;

  MYON__UINT_PTR EdgeDirection(
    unsigned int i
    ) const;

  /*
  Returns:
    If e is part of the face's boundary, then the index of the edge is returned.
    Otherwise, MYON_UNSET_UINT_INDEX is returned.
  */
  unsigned int EdgeArrayIndex(
    const MYON_SubDEdge* e
    ) const;

  /*
  Description:
    Expert user tool to remove an edge from the face's edge array.
  Remarks:
    Does not modify the edge.  If the face is referenced in the edge's face array,
    then the face must be removed from the edge's face array.
  */
  bool RemoveEdgeFromArray(
    const MYON_SubDEdge* e
    );

  /*
  Description:
    Expert user tool to remove an edge from the face's edge array.
  Remarks:
    Does not modify the edge.  If the face is referenced in the edge's face array,
    then the face must be removed from the edge's face array.
  */
  bool RemoveEdgeFromArray(
    unsigned int i,
    MYON_SubDEdgePtr& removed_edge
    );

  /*
  Description:
    Expert user tool to replace one edge with another in the face's edge array.
  Parameters:
    edge_to_remove - [in]
    edge_to_insert - [in]
     The inserted edge is assigned the same boundary orientation as the
     removed edge.
  Remarks:
    Does not modify the edge. The corresponding reference to this face must
    be removed from the first edge and added to the second edge.
  */
  bool ReplaceEdgeInArray(
    unsigned int fei0,
    MYON_SubDEdge* edge_to_remove,
    MYON_SubDEdge* edge_to_insert
  );

  /*
  Description:
    Expert user tool to replace one edge with another in the face's edge array.
  Parameters:
    edge_to_remove - [in]
    edgeptr_to_insert - [in]
     The inserted edge is assigned the same boundary orientation specified
     in edgeptr_to_insert.
  Remarks:
    Does not modify the edge. The corresponding reference to this face must
    be removed from the first edge and added to the second edge.
  */
  bool ReplaceEdgeInArray(
    unsigned int fei0,
    MYON_SubDEdge* edge_to_remove,
    MYON_SubDEdgePtr edgeptr_to_insert
  );

  /*
  Description:
    Rotates the edge array so that Edge(fei0) becomes the first edge in the edge array.
  */
  bool RotateEdgeArray(
    unsigned int fei0
  );

  const MYON_SubDEdge* PrevEdge(
    const MYON_SubDEdge* edge
    ) const;

  const MYON_SubDEdge* NextEdge(
    const MYON_SubDEdge* edge
    ) const;

  unsigned int PrevEdgeArrayIndex(
    unsigned int edge_array_index
    ) const;

  unsigned int NextEdgeArrayIndex(
    unsigned int edge_array_index
    ) const;

  /*
  Description:
    If the face is a quad, get the opposite corner vertex.
  Parameters:
    vertex - [in]
      a vertex on this face.
  Returns:
    If the face is a quad and vertex is a vertex of the face, then
    the vertex on the opposite corner is returned.
    Otherwise, nullptr is returned.
  */
  const MYON_SubDVertex* QuadOppositeVertex(
    const MYON_SubDVertex* vertex
    ) const;

  /*
  Description:
    If the face is a quad, get  the opposite side edge.
  Parameters:
    edge - [in]
      an edge on this face.
  Returns:
    If the face is a quad and edge is an edge of the face, then
    the edge on the opposite side is returned.
    Otherwise, nullptr is returned.
  */
  const MYON_SubDEdge* QuadOppositeEdge(
    const MYON_SubDEdge* edge
    ) const;

  /*
  Parameters:
    subdivision_point - [out]
      The average of the face vertex locations.
  Returns:
    true if successful
  */
  bool GetSubdivisionPoint(
    double subdivision_point[3]
    ) const;

  ///<summary>
  /// The SubD face Catmull-Clark subdivision point.
  ///</summary>
  const MYON_3dPoint SubdivisionPoint() const;
  
  /*
  Description:
    Evaluates the Catmull-Clark subdivision point ignoring all cached information.
    This function is typically used in testing and debugging code and
    in ordinary cases, it is faster and better to call SubdivisionPoint()
    or GetSubdivisionPoint().
  Parameters:
    subdivision_point - [out]
      The Catmull-Clark face subdivision point is returned here.
  */
  bool EvaluateCatmullClarkSubdivisionPoint(
    double subdivision_point[3]
  ) const;


  /*
  Description:
    Reverse the order and orientation of the edges that form
    the boundary of this face.
  */
  bool ReverseEdgeList();

  /*
  Description:
    Get the bicubic b-spline control points for the limit surface.
    The corresponding knots are uniform.
  Parameters:
    vertex - [in]
    limit_surface_cv_stride0 - [int]
    limit_surface_cv_stride1 - [out]
    limit_surface_cv - [out]
      control points for a cubic spline surface
        CV[i][j][k] = limit_surface_cv[i*limit_bspline_cv_stride0 + j*limit_bspline_cv_stride1 + k]
        0 <= i < 4, 0 <= j < 4, 0 <= k < 3
  Returns:
    true if successful
    false if the limit surface for this face is not a cubic surface
  Remarks:
    The knots for the bicubic b-spline surface are uniform.
  */
  bool GetQuadSurface(
    double* limit_surface_cv,
    size_t limit_surface_cv_stride0,
    size_t limit_surface_cv_stride1
    ) const;

  bool GetQuadSurface(
    class MYON_NurbsSurface& limit_surface
    ) const;

  bool GetQuadSurface(
    class MYON_BezierSurface& limit_surface
    ) const;

  /*
  Returns:
    Number of edges in the face's boundary with Edge().m_status.RuntimeMark() = true;
  */
  unsigned int MarkedEdgeCount() const;

  /*
  Returns:
    Number of vertices in the face's boundary with Vertex().m_status.RuntimeMark() = true;
  */
  unsigned int MarkedVertexCount() const;

  /*
  Description:
    Set the mark of every vertex attached to this face.
  Returns:
    Number of marks changed.
  */
  unsigned int SetVertexMarks(
    bool bMark
    ) const;

  /*
  Description:
    Set the mark of every boundary edge attached to this face.
  Returns:
    Number of marks changed.
  */
  unsigned int SetEdgeMarks(
    bool bMark
    ) const;

  /*
  Description:
    Get the face's boundary.
  Parameters:
    face_edge_array - [out]
      The boundary of the face is returned in canonical counter-clockwise order.
  Returns:
    Number of edges in the face's boundary.
  */
  unsigned int GetEdgeArray(
    MYON_SimpleArray< MYON_SubDEdgePtr >& face_edge_array
  ) const;

private:
  void CopyFrom(
    const MYON_SubDFace* src,
    bool bCopyEdgeArray,
    bool bCopySymmetrySetNext
    );
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDVertexArray
//

/*
Description:
  Avoid using MYON_SubDVertexArray. MYON_SubDVertexIterator is more efficient and a better choice.
*/
class MYON_CLASS MYON_SubDVertexArray
{
public:
  MYON_SubDVertexArray(
    const MYON_SubD& subd
    );
  MYON_SubDVertexArray() = default;
  MYON_SubDVertexArray(const MYON_SubDVertexArray&) = default;
  MYON_SubDVertexArray& operator=(const MYON_SubDVertexArray&) = default;

#if defined(MYON_HAS_RVALUEREF)
  // rvalue copy constructor
  MYON_SubDVertexArray(MYON_SubDVertexArray&&) MYON_NOEXCEPT;

  // rvalue copy operator-=
  MYON_SubDVertexArray& operator=(MYON_SubDVertexArray&&);
#endif

  const MYON_SubD& SubD() const
  {
    return m_subd;
  }

  unsigned int VertexCount() const
  {
    return m_vertex_count;
  }

  const class MYON_SubDVertex* operator[](unsigned int i) const
  {
    return (i < m_vertex_count) ? m_a[i] : nullptr;
  }
  
private:
  MYON_SubD m_subd;
  const class MYON_SubDVertex*const* m_a = nullptr;
  unsigned int m_vertex_count = 0;

#pragma MYON_PRAGMA_WARNING_PUSH
#pragma MYON_PRAGMA_WARNING_DISABLE_MSC( 4251 )
  // C4251: ... needs to have dll-interface to be used by clients of class ...
  // m_sp is private and all code that manages m_sp is explicitly implemented in the DLL.
private:
  std::shared_ptr< const class MYON_SubDVertex* > m_sp;
#pragma MYON_PRAGMA_WARNING_POP
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDEdgeArray
//

/*
Description:
  Avoid using MYON_SubDEdgeArray. MYON_SubDEdgeIterator is more efficient and a better choice.
*/
class MYON_CLASS MYON_SubDEdgeArray
{
public:
  MYON_SubDEdgeArray(
    const MYON_SubD& subd
    );
  MYON_SubDEdgeArray() = default;
  MYON_SubDEdgeArray(const MYON_SubDEdgeArray&) = default;
  MYON_SubDEdgeArray& operator=(const MYON_SubDEdgeArray&) = default;

#if defined(MYON_HAS_RVALUEREF)
  // rvalue copy constructor
  MYON_SubDEdgeArray(MYON_SubDEdgeArray&&) MYON_NOEXCEPT;

  // rvalue copy operator-=
  MYON_SubDEdgeArray& operator=(MYON_SubDEdgeArray&&);
#endif

  const MYON_SubD& SubD() const
  {
    return m_subd;
  }

  unsigned int EdgeCount() const
  {
    return m_edge_count;
  }

  const class MYON_SubDEdge* operator[](unsigned int i) const
  {
    return (i < m_edge_count) ? m_a[i] : nullptr;
  }

private:
  MYON_SubD m_subd;
  const class MYON_SubDEdge*const* m_a = nullptr;
  unsigned int m_edge_count = 0;

#pragma MYON_PRAGMA_WARNING_PUSH
#pragma MYON_PRAGMA_WARNING_DISABLE_MSC( 4251 )
  // C4251: ... needs to have dll-interface to be used by clients of class ...
  // m_sp is private and all code that manages m_sp is explicitly implemented in the DLL.
private:
  std::shared_ptr< const class MYON_SubDEdge* > m_sp;
#pragma MYON_PRAGMA_WARNING_POP
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDFaceArray
//

/*
Description:
  Avoid using MYON_SubDFaceArray. MYON_SubDFaceIterator is more efficient and a better choice.
*/
class MYON_CLASS MYON_SubDFaceArray
{
public:
  MYON_SubDFaceArray(
    const MYON_SubD& subd
    );
  MYON_SubDFaceArray() = default;
  MYON_SubDFaceArray(const MYON_SubDFaceArray&) = default;
  MYON_SubDFaceArray& operator=(const MYON_SubDFaceArray&) = default;

#if defined(MYON_HAS_RVALUEREF)
  // rvalue copy constructor
  MYON_SubDFaceArray(MYON_SubDFaceArray&&) MYON_NOEXCEPT;

  // rvalue copy operator-=
  MYON_SubDFaceArray& operator=(MYON_SubDFaceArray&&);
#endif

  const MYON_SubD& SubD() const
  {
    return m_subd;
  }

  unsigned int FaceCount() const
  {
    return m_face_count;
  }

  const class MYON_SubDFace* operator[](unsigned int i) const
  {
    return (i < m_face_count) ? m_a[i] : nullptr;
  }

private:
  MYON_SubD m_subd;
  const class MYON_SubDFace*const* m_a = nullptr;
  unsigned int m_face_count = 0;
  
#pragma MYON_PRAGMA_WARNING_PUSH
#pragma MYON_PRAGMA_WARNING_DISABLE_MSC( 4251 )
  // C4251: ... needs to have dll-interface to be used by clients of class ...
  // m_sp is private and all code that manages m_sp is explicitly implemented in the DLL.
private:
  std::shared_ptr< const class MYON_SubDFace* > m_sp;
#pragma MYON_PRAGMA_WARNING_POP
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDVertexIterator
//
class MYON_CLASS MYON_SubDVertexIterator
{
public:
  // The best way to get an MYON_SubDVertexIterator is so use the MYON_SubD member function
  // MYON_SubDVertexIterator vit = subd.VertexIterator();
  MYON_SubDVertexIterator(
    const class MYON_SubD& subd
    );

  // The best way to get an MYON_SubDVertexIterator is so use the MYON_SubD member function
  // MYON_SubDVertexIterator vit = subd.VertexIterator();
  MYON_SubDVertexIterator(
    const class MYON_SubDRef& subd_ref
    );

  // Construct and iterator that iterates over a single vertex.
  MYON_SubDVertexIterator(
    const class MYON_SubD& subd,
    const class MYON_SubDVertex& vertex
    );

  // Construct and iterator that iterates over a single vertex.
  MYON_SubDVertexIterator(
    const class MYON_SubDRef& subd_ref,
    const class MYON_SubDVertex& vertex
    );

  // Construct and iterator that iterates over the vertices of an edge.
  MYON_SubDVertexIterator(
    const class MYON_SubD& subd,
    const class MYON_SubDEdge& edge
    );

  // Construct and iterator that iterates over the vertices of an edge.
  MYON_SubDVertexIterator(
    const class MYON_SubDRef& subd_ref,
    const class MYON_SubDEdge& edge
    );

  // Construct and iterator that iterates over the vertices of a face.
  MYON_SubDVertexIterator(
    const class MYON_SubD& subd,
    const class MYON_SubDFace& face
    );

  // Construct and iterator that iterates over the vertices of a face.
  MYON_SubDVertexIterator(
    const class MYON_SubDRef& subd_ref,
    const class MYON_SubDFace& face
    );

  MYON_SubDVertexIterator() = default;
  MYON_SubDVertexIterator(const MYON_SubDVertexIterator&) = default;
  MYON_SubDVertexIterator& operator=(const MYON_SubDVertexIterator&) = default;

#if defined(MYON_HAS_RVALUEREF)
  // rvalue copy constructor
  MYON_SubDVertexIterator( MYON_SubDVertexIterator&& ) MYON_NOEXCEPT;
  // rvalue assignment operator
  MYON_SubDVertexIterator& operator=( MYON_SubDVertexIterator&& );
#endif

  /*
  Returns:
  The subD object for this iterator.
  */
  const class MYON_SubD& SubD() const
  {
    return m_subd_ref.SubD();
  }

  const class MYON_SubDRef& SubDRef() const
  {
    return m_subd_ref;
  }

  /*
  Description:
    Increment the iterator.
  Returns:
    Current vertex.
  Remarks:
    operator++ and NextVertex() behave differently.
  */
  const class MYON_SubDVertex* operator++()
  {
    const class MYON_SubDVertex* v = m_v_current;
    NextVertex();
    return v;
  }

  /*
  Return:
    Number of vertices this iterator will iterate through.
  */
  unsigned int VertexCount() const
  {
    return m_vertex_count;
  }

  /*
  Return:
    Iterator index of the current vertex.
  */
  unsigned int CurrentVertexIndex() const
  {
    return m_vertex_index;
  }

  /*
  Description:
  Set the iterator to the beginning of the vertex list.
  Returns:
  First vertex in the list.
  */
  const class MYON_SubDVertex* FirstVertex()
  {
    m_vertex_index = 0;
    return (m_v_current = m_v_first);
  }

  /*
  Description:
    Increment the iterator.
  Returns:
    Next vertex.
  Remarks:
    operator++ and NextVertex() behave differently.
  */
  const class MYON_SubDVertex* NextVertex()
  {
    m_vertex_index++;
    if (m_vertex_index < m_vertex_count)
    {
      if (0 == m_component_ptr.m_ptr)
      {
        if (nullptr != m_v_current)
          m_v_current = m_v_current->m_next_vertex;
      }
      else
      {
        const MYON_SubDEdge* edge = m_component_ptr.Edge();
        if (nullptr != edge)
        {
          m_v_current = edge->Vertex(m_vertex_index);
        }
        else
        {
          const MYON_SubDFace* face = m_component_ptr.Face();
          if (nullptr != face)
            m_v_current = face->Vertex(m_vertex_index);
          else
            m_v_current = nullptr;
        }
      }
    }
    else
    {
      m_vertex_index = m_vertex_count;
      m_v_current = nullptr;
    }
    return m_v_current;
  }

  /*
  Returns:
  Current vertex;
  */
  const class MYON_SubDVertex* CurrentVertex() const
  {
    return m_v_current;
  }

  /*
  Description:
  Set the iterator to the end of the vertex list.
  Returns:
  Last vertex in the list.
  */
  const class MYON_SubDVertex* LastVertex()
  {
    m_vertex_index = (m_vertex_count > 0) ? (m_vertex_count - 1) : 0;
    return (m_v_current = m_v_last);
  }

private:
  void Internal_Init(
    const MYON_SubDRef& subd_ref,
    unsigned int vertex_count,
    const MYON_SubDVertex* first,
    const MYON_SubDVertex* last,
    MYON_SubDComponentPtr component_ptr
  );
  MYON_SubDRef m_subd_ref;
  const MYON_SubDVertex* m_v_first = nullptr;
  const MYON_SubDVertex* m_v_last = nullptr;
  const MYON_SubDVertex* m_v_current = nullptr;
  unsigned int m_vertex_index = 0;
  unsigned int m_vertex_count = 0;
  MYON_SubDComponentPtr m_component_ptr = MYON_SubDComponentPtr::Null;
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDVertexIdIterator
//

class MYON_CLASS MYON_SubDVertexIdIterator : private MYON_FixedSizePoolIterator
{
public:
  MYON_SubDVertexIdIterator() = default;
  ~MYON_SubDVertexIdIterator() = default;
  MYON_SubDVertexIdIterator(const MYON_SubDVertexIdIterator&) = default;
  MYON_SubDVertexIdIterator& operator=(const MYON_SubDVertexIdIterator&) = default;

public:
  MYON_SubDVertexIdIterator(const MYON_SubDRef& subd_ref);
  MYON_SubDVertexIdIterator(const MYON_SubD& subd);

public:
  /*
  Description:
    In general, you want to use a MYON_SubDVertexIterator to loop through SubD vertices.
    This is a special tool for unusual sitiations when it is necessary to
    iteratate through every vertex on every level of a SubD in order
    of increasing m_id value. 
  Returns:
    The vertex with the smallest id.
  */
  const MYON_SubDVertex* FirstVertex();

  const MYON_SubDVertex* FirstVertexOnLevel(unsigned int level_index);


  /*
  Description:
    In general, you want to use a MYON_SubDVertexIterator to loop through SubD vertices.
    This is a special tool for unusual sitiations when it is necessary to
    iteratate through every vertex on every level of a SubD in order
    of increasing m_id value. 
  Returns:
    The vertex in order of increasing id.
  */
  const MYON_SubDVertex* NextVertex();

  const MYON_SubDVertex* NextVertexOnLevel(unsigned int level_index);

  /*
  Returns:
    The most recently returned vertex from a call to FirstVertex() or NextVertex().
  */
  const MYON_SubDVertex* CurrentVertex() const;

private:
  friend class MYON_SubDHeap;
  MYON_SubDRef m_subd_ref;
  void Internal_Init();
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDEdgeIterator
//
class MYON_CLASS MYON_SubDEdgeIterator
{
public:
  // The best way to get an MYON_SubDEdgeIterator is so use the MYON_SubD member function
  // MYON_SubDEdgeIterator eit = subd.EdgeIterator();
  MYON_SubDEdgeIterator(
    const class MYON_SubD& subd
    );

  // The best way to get an MYON_SubDEdgeIterator is so use the MYON_SubD member function
  // MYON_SubDEdgeIterator eit = subd.EdgeIterator();
  MYON_SubDEdgeIterator(
    const class MYON_SubDRef& subd_ref
    );

  // Construct and iterator that iterates over a single edge.
  MYON_SubDEdgeIterator(
    const class MYON_SubD& subd,
    const class MYON_SubDEdge& edge
    );

  // Construct and iterator that iterates over a single edge.
  MYON_SubDEdgeIterator(
    const class MYON_SubDRef& subd_ref,
    const class MYON_SubDEdge& edge
    );

  // Construct and iterator that iterates over the edges of a vertex.
  MYON_SubDEdgeIterator(
    const class MYON_SubD& subd,
    const class MYON_SubDVertex& vertex
    );

  // Construct and iterator that iterates over the edges of a vertex.
  MYON_SubDEdgeIterator(
    const class MYON_SubDRef& subd_ref,
    const class MYON_SubDVertex& vertex
    );

  // Construct and iterator that iterates over the edges of a face.
  MYON_SubDEdgeIterator(
    const class MYON_SubD& subd,
    const class MYON_SubDFace& face
    );

  // Construct and iterator that iterates over the edges of a face.
  MYON_SubDEdgeIterator(
    const class MYON_SubDRef& subd_ref,
    const class MYON_SubDFace& face
    );

  MYON_SubDEdgeIterator() = default;
  MYON_SubDEdgeIterator(const MYON_SubDEdgeIterator&) = default;
  MYON_SubDEdgeIterator& operator=(const MYON_SubDEdgeIterator&) = default;

#if defined(MYON_HAS_RVALUEREF)
  // rvalue copy constructor
  MYON_SubDEdgeIterator( MYON_SubDEdgeIterator&& ) MYON_NOEXCEPT;
  // rvalue assignment operator
  MYON_SubDEdgeIterator& operator=( MYON_SubDEdgeIterator&& );
#endif

  /*
  Returns:
  The subD object for this iterator.
  */
  const class MYON_SubD& SubD() const
  {
    return m_subd_ref.SubD();
  }

  const class MYON_SubDRef& SubDRef() const
  {
    return m_subd_ref;
  }

  /*
  Description:
    Increment the iterator.
  Returns:
    Current edge.
  Remarks:
    operator++ and NextEdge() behave differently.
  */
  const class MYON_SubDEdge* operator++()
  {
    const class MYON_SubDEdge* e = m_e_current;
    NextEdge();
    return e;
  }

  /*
  Return:
    Number of edges this iterator will iterate through.
  */
  unsigned int EdgeCount() const
  {
    return m_edge_count;
  }

  /*
  Return:
    Iterator index of the current edge.
  */
  unsigned int CurrentEdgeIndex() const
  {
    return m_edge_index;
  }

  /*
  Description:
    Set the iterator to the beginning of the edge list.
  Returns:
    First edge in the list.
  */
  const class MYON_SubDEdge* FirstEdge()
  {
    m_edge_index = 0;
    return m_e_current = m_e_first;
  }

  /*
  Description:
    Increment the iterator.
  Returns:
    Next edge.
  Remarks:
    operator++ and NextEdge() behave differently.
  */
  const class MYON_SubDEdge* NextEdge()
  {
    m_edge_index++;
    if (m_edge_index < m_edge_count)
    {
      if (0 == m_component_ptr.m_ptr)
      {
        if (nullptr != m_e_current)
          m_e_current = m_e_current->m_next_edge;
      }
      else
      {
        const MYON_SubDVertex* vertex = m_component_ptr.Vertex();
        if (nullptr != vertex)
        {
          m_e_current = vertex->Edge(m_edge_index);
        }
        else
        {
          const MYON_SubDFace* face = m_component_ptr.Face();
          if (nullptr != face)
            m_e_current = face->Edge(m_edge_index);
          else
            m_e_current = nullptr;
        }
      }
    }
    else
    {
      m_edge_index = m_edge_count;
      m_e_current = nullptr;
    }
    return m_e_current;
  }

  /*
  Returns:
  Current edge;
  */
  const class MYON_SubDEdge* CurrentEdge() const
  {
    return m_e_current;
  }

  /*
  Description:
  Set the iterator to the end of the edge list.
  Returns:
  Last edge in the list.
  */
  const class MYON_SubDEdge* LastEdge()
  {
    m_edge_index = (m_edge_count > 0) ? (m_edge_count - 1) : 0;
    return m_e_current = m_e_last;
  }

private:
  void Internal_Init(
    const MYON_SubDRef& subd_ref,
    unsigned int edge_count,
    const MYON_SubDEdge* first,
    const MYON_SubDEdge* last,
    MYON_SubDComponentPtr component_ptr
  );
  MYON_SubDRef m_subd_ref;
  const MYON_SubDEdge* m_e_first = nullptr;
  const MYON_SubDEdge* m_e_last = nullptr;
  const MYON_SubDEdge* m_e_current = nullptr;
  unsigned int m_edge_index = 0;
  unsigned int m_edge_count = 0;
  MYON_SubDComponentPtr m_component_ptr = MYON_SubDComponentPtr::Null;
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDEdgeIdIterator
//

class MYON_CLASS MYON_SubDEdgeIdIterator : private MYON_FixedSizePoolIterator
{
public:
  MYON_SubDEdgeIdIterator() = default;
  ~MYON_SubDEdgeIdIterator() = default;
  MYON_SubDEdgeIdIterator(const MYON_SubDEdgeIdIterator&) = default;
  MYON_SubDEdgeIdIterator& operator=(const MYON_SubDEdgeIdIterator&) = default;

public:
  MYON_SubDEdgeIdIterator(const MYON_SubDRef& subd_ref);
  MYON_SubDEdgeIdIterator(const MYON_SubD& subd);

public:
  /*
  Description:
    In general, you want to use a MYON_SubDEdgeIterator to loop through SubD edges.
    This is a special tool for unusual sitiations when it is necessary to
    iteratate through every edge on every level of a SubD in order
    of increasing m_id value. 
  Returns:
    The edge with the smallest id.
  */
  const MYON_SubDEdge* FirstEdge();

  const MYON_SubDEdge* FirstEdgeOnLevel(unsigned int level_index);


  /*
  Description:
    In general, you want to use a MYON_SubDEdgeIterator to loop through SubD edges.
    This is a special tool for unusual sitiations when it is necessary to
    iteratate through every edge on every level of a SubD in order
    of increasing m_id value. 
  Returns:
    The edge in order of increasing id.
  */
  const MYON_SubDEdge* NextEdge();

  const MYON_SubDEdge* NextEdgeOnLevel(unsigned int level_index);

  /*
  Returns:
    The most recently returned edge from a call to FirstEdge() or NextEdge().
  */
  const MYON_SubDEdge* CurrentEdge() const;

private:
  friend class MYON_SubDHeap;
  MYON_SubDRef m_subd_ref;
  void Internal_Init();
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDFaceIterator
//
class MYON_CLASS MYON_SubDFaceIterator
{
public:
  // The best way to get an MYON_SubDFaceIterator is so use the MYON_SubD member function
  // MYON_SubDFaceIterator fit = subd.FaceIterator();
  MYON_SubDFaceIterator(
    const class MYON_SubD& subd
    );

  // The best way to get an MYON_SubDFaceIterator is so use the MYON_SubD member function
  // MYON_SubDFaceIterator fit = subd.FaceIterator();
  MYON_SubDFaceIterator(
    const class MYON_SubDRef& subd_ref
    );

  // Construct and iterator that iterates over the single face.
  MYON_SubDFaceIterator(
    const class MYON_SubD& subd,
    const class MYON_SubDFace& face
    );

  // Construct and iterator that iterates over the single face.
  MYON_SubDFaceIterator(
    const class MYON_SubDRef& subd_ref,
    const class MYON_SubDFace& face
    );

  // Construct and iterator that iterates over the faces of a vertex.
  MYON_SubDFaceIterator(
    const class MYON_SubD& subd,
    const class MYON_SubDVertex& vertex
    );

  // Construct and iterator that iterates over the faces of a vertex.
  MYON_SubDFaceIterator(
    const class MYON_SubDRef& subd_ref,
    const class MYON_SubDVertex& vertex
    );

  // Construct and iterator that iterates over the faces of an edge.
  MYON_SubDFaceIterator(
    const class MYON_SubD& subd,
    const class MYON_SubDEdge& edge
    );

  // Construct and iterator that iterates over the faces of an edge.
  MYON_SubDFaceIterator(
    const class MYON_SubDRef& subd_ref,
    const class MYON_SubDEdge& edge
    );

  MYON_SubDFaceIterator() = default;
  MYON_SubDFaceIterator(const MYON_SubDFaceIterator&) = default;
  MYON_SubDFaceIterator& operator=(const MYON_SubDFaceIterator&) = default;

#if defined(MYON_HAS_RVALUEREF)
  // rvalue copy constructor
  MYON_SubDFaceIterator( MYON_SubDFaceIterator&& ) MYON_NOEXCEPT;
  // rvalue assignment operator
  MYON_SubDFaceIterator& operator=( MYON_SubDFaceIterator&& );
#endif

  /*
  Returns:
  The subD object for this iterator.
  */
  const class MYON_SubD& SubD() const
  {
    return m_subd_ref.SubD();
  }

  const class MYON_SubDRef& SubDRef() const
  {
    return m_subd_ref;
  }

  /*
  Description:
    Returns the current face and increment the iterator.
  Returns:
    Current face.
  Remarks:
    operator++ and NextFace() behave differently.
  */
  const class MYON_SubDFace* operator++()
  {
    const class MYON_SubDFace* f = m_face_current;
    NextFace();
    return f;
  }

  /*
  Return:
    Number of faces this iterator will iterate through.
  */
  unsigned int FaceCount() const
  {
    return m_face_count;
  }

  /*
  Return:
    Iterator index of the current face.
  */
  unsigned int CurrentFaceIndex() const
  {
    return m_face_index;
  }


  /*
  Description:
    Set the iterator to the beginning of the face list.
  Returns:
    First face in the list.
  */
  const class MYON_SubDFace* FirstFace()
  {
    m_face_index = 0;
    return (m_face_current = m_face_first);
  }

  /*
  Description:
    Returns the next face and increments the iterator.
  Returns:
    Next face.
  Remarks:
    operator++ and NextFace() behave differently.
  */
  const class MYON_SubDFace* NextFace()
  {
    m_face_index++;
    if (m_face_index < m_face_count)
    {
      if (0 == m_component_ptr.m_ptr)
      {
        if (nullptr != m_face_current)
          m_face_current = m_face_current->m_next_face;
      }
      else
      {
        const MYON_SubDVertex* vertex = m_component_ptr.Vertex();
        if (nullptr != vertex)
        {
          m_face_current = vertex->Face(m_face_index);
        }
        else
        {
          const MYON_SubDEdge* edge = m_component_ptr.Edge();
          if (nullptr != edge)
            m_face_current = edge->Face(m_face_index);
          else
            m_face_current = nullptr;
        }
      }
    }
    else
    {
      m_face_index = m_face_count;
      m_face_current = nullptr;
    }
    return m_face_current;
  }

  /*
  Returns:
    Current face;
  */
  const class MYON_SubDFace* CurrentFace() const
  {
    return m_face_current;
  }

  /*
  Description:
    Set the iterator to the end of the face list.
  Returns:
    Last face in the list.
  */
  const class MYON_SubDFace* LastFace()
  {
    m_face_index = (m_face_count > 0) ? (m_face_count - 1) : 0;
    return (m_face_current = m_face_last);
  }


private:
  void Internal_Init(
    const MYON_SubDRef& subd_ref,
    unsigned int face_count,
    const MYON_SubDFace* first,
    const MYON_SubDFace* last,
    MYON_SubDComponentPtr component_ptr
  );
  MYON_SubDRef m_subd_ref;
  const MYON_SubDFace* m_face_first = nullptr;
  const MYON_SubDFace* m_face_last = nullptr;
  const MYON_SubDFace* m_face_current = nullptr;
  unsigned int m_face_index = 0;
  unsigned int m_face_count = 0;
  MYON_SubDComponentPtr m_component_ptr = MYON_SubDComponentPtr::Null;
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDFaceIdIterator
//

class MYON_CLASS MYON_SubDFaceIdIterator : private MYON_FixedSizePoolIterator
{
public:
  MYON_SubDFaceIdIterator() = default;
  ~MYON_SubDFaceIdIterator() = default;
  MYON_SubDFaceIdIterator(const MYON_SubDFaceIdIterator&) = default;
  MYON_SubDFaceIdIterator& operator=(const MYON_SubDFaceIdIterator&) = default;

public:
  MYON_SubDFaceIdIterator(const MYON_SubDRef& subd_ref);
  MYON_SubDFaceIdIterator(const MYON_SubD& subd);

public:
  /*
  Description:
    In general, you want to use a MYON_SubDFaceIterator to loop through SubD faces.
    This is a special tool for unusual sitiations when it is necessary to
    iteratate through every face on every level of a SubD in order
    of increasing m_id value. 
  Returns:
    The face with the smallest id.
  */
  const MYON_SubDFace* FirstFace();
  
  const MYON_SubDFace* FirstFaceOnLevel(unsigned int level_index);

  /*
  Description:
    In general, you want to use a MYON_SubDFaceIterator to loop through SubD faces.
    This is a special tool for unusual sitiations when it is necessary to
    iteratate through every face on every level of a SubD in order
    of increasing m_id value. 
  Returns:
    The face in order of increasing id.
  */
  const MYON_SubDFace* NextFace();

  const MYON_SubDFace* NextFaceOnLevel(unsigned int level_index);

  /*
  Returns:
    The most recently returned face from a call to FirstFace() or NextFace().
  */
  const MYON_SubDFace* CurrentFace() const;

private:
  friend class MYON_SubDHeap;
  MYON_SubDRef m_subd_ref;
  void Internal_Init();
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDComponentIterator
//
class MYON_CLASS MYON_SubDComponentIterator
{
public:
  static const MYON_SubDComponentIterator Empty;

  // The MYON_SubD member function
  //   MYON_SubDComponentIterator MYON_SubD::ComponentIterator(subd_level_index)
  //   is the best way to get a component iterator for a subd level.
  MYON_SubDComponentIterator(
    const class MYON_SubD& subd
    );
  MYON_SubDComponentIterator(
    const class MYON_SubDRef& subd_ref
    );

  MYON_SubDComponentIterator() = default;
  MYON_SubDComponentIterator(const MYON_SubDComponentIterator&) = default;
  MYON_SubDComponentIterator& operator=(const MYON_SubDComponentIterator&) = default;

#if defined(MYON_HAS_RVALUEREF)
  // rvalue copy constructor
  MYON_SubDComponentIterator( MYON_SubDComponentIterator&& ) MYON_NOEXCEPT;
  // rvalue assignment operator
  MYON_SubDComponentIterator& operator=( MYON_SubDComponentIterator&& );
#endif

  /*
  Returns:
  The subD object for this iterator.
  */
  const class MYON_SubD& SubD() const
  {
    return m_subd_ref.SubD();
  }

  const class MYON_SubDRef& SubDRef() const
  {
    return m_subd_ref;
  }

  /*
  Returns:
  The subD level for this iterator.
  */
  unsigned int SubDLevel() const
  {
    return m_subd_level;
  }

  /*
  Description:
    Returns the current component and increment the iterator.
  Returns:
    Current component.
  Remarks:
    operator++ and NextComponent() behave differently.
  */
  const class MYON_SubDComponentPtr operator++()
  {
    const class MYON_SubDComponentPtr cptr = m_cptr_current;
    NextComponent();
    return cptr;
  }

  /*
  Description:
    Set the iterator to the beginning of the component list.
  Returns:
    First component in the list.
  */
  const class MYON_SubDComponentPtr FirstComponent();

  /*
  Description:
    Returns the next component and increments the iterator.
  Returns:
    Next component.
  Remarks:
    operator++ and NextComponent() behave differently.
  */
  const class MYON_SubDComponentPtr NextComponent();  

  /*
  Returns:
    Current component;
  */
  const class MYON_SubDComponentPtr CurrentComponent() const
  {
    return m_cptr_current;
  }

  /*
  Description:
    Set the iterator to the end of the component list.
  Returns:
    Last component in the list.
  */
  const class MYON_SubDComponentPtr LastComponent();

private:
  MYON_SubDRef m_subd_ref;
  unsigned int m_subd_level = 0;
  const MYON_SubDVertex* m_vertex_first = nullptr;
  const MYON_SubDVertex* m_vertex_last = nullptr;
  const MYON_SubDEdge* m_edge_first = nullptr;
  const MYON_SubDEdge* m_edge_last = nullptr;
  const MYON_SubDFace* m_face_first = nullptr;
  const MYON_SubDFace* m_face_last = nullptr;
  MYON_SubDComponentPtr m_cptr_current = MYON_SubDComponentPtr::Null;
};


//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDMeshFragmentIterator
//

class MYON_CLASS MYON_SubDMeshFragmentIterator
{
public:
  MYON_SubDMeshFragmentIterator() = default;
  ~MYON_SubDMeshFragmentIterator() = default;
  MYON_SubDMeshFragmentIterator(const MYON_SubDMeshFragmentIterator&);
  MYON_SubDMeshFragmentIterator& operator=(const MYON_SubDMeshFragmentIterator&);

public:
  MYON_SubDMeshFragmentIterator(const class MYON_SubDMesh limit_mesh);
  MYON_SubDMeshFragmentIterator(MYON_SubDRef& subd_ref);
  MYON_SubDMeshFragmentIterator(const MYON_SubD& subd);
  MYON_SubDMeshFragmentIterator(const MYON_SubDFaceIterator& fit);

public:
  const MYON_SubDMeshFragment* FirstFragment();
  const MYON_SubDMeshFragment* NextFragment();
  const MYON_SubDMeshFragment* CurrentFragment();

  bool IsEmpty() const;

  const MYON_SubD& SubD() const;

  /*
  Returns
    MYON_SubDComponentLocation::Surface or MYON_SubDComponentLocation::ControlNet
  */
  MYON_SubDComponentLocation SubDAppearance() const;
  
  /*
  Parameters:
    subd_appearance_override - [in]
      MYON_SubDComponentLocation::Unset - appearance controlled by SubD().SubDAppearance().
      MYON_SubDComponentLocation::Surface
      MYON_SubDComponentLocation::ControlNet
  Returns
    MYON_SubDComponentLocation::Surface or MYON_SubDComponentLocation::ControlNet
  */
  void SetSubDAppearanceOverride(MYON_SubDComponentLocation subd_appearance_override);


  /*
  Returns:
    Density setting used to create the fragments and the maximum mesh density
    these fragments can deliver.
  Remarks:
    0: a single mesh quad per SubD quad 
    1: 4 mesh quads per SubD quad (n mesh quads per SubD n-gon when n != 4)
    d: 4^d mesh quads per SubD quad (n*(4^(d-1)) mesh quads per SubD n-gon when n != 4)
  */
  unsigned int MaximumMeshDensity() const;

  /*
  Returns:
    Minimum mesh density that can be extracted from these fragments.
  Remarks:
    0 = all fragments are full sized.
    1 = there are half sixed fragments because the SubD has n-gons with n != 4.
  */
  unsigned int MinimumMeshDensity() const;

  enum : unsigned int
  {
    /// <summary>
    /// Rhino uses this value in MeshDensityFromMaxMeshQuadCount() to se the default level of detail.
    /// The enum will always exist but the value can change without breaking the SDK. 
    /// You code must assume this value will change with each service release of Rhino.
    /// </summary>
    DefaultMaximumMeshQuadCount = 262144
  };

  /*
  Parameters:
    candidate_mesh_density - [in]
  Returns:
    If candidate_mesh_density > MeshDensity(), then MeshDensity() is returned.
    If candidate_mesh_density < MinimumMeshDensity(), then MinimumMeshDensity() is returned.
    Otherwise, candidate_mesh_density is returned.
  */
  unsigned int ClampMeshDensity(
    unsigned int candidate_mesh_density
  ) const;

  /*
  Returns:
    Total number of fragments.
  */
  unsigned int FragmentCount() const;

  /*
  Returns:
    Total number of full sized fragments. 
  Remarks:
    A full sized fragment covers an entire quad face.
  */
  unsigned int FullSizeFragmentCount() const;

  /*
  Returns:
    Total number of half sized fragments. 
  Remarks:
    A half sized fragment covers the corner of an N-gon and are used when N != 4.
  */
  unsigned int HalfSizeFragmentCount() const;


  /*
  Parameters:
    full_size_fragment_count - [out]
      Number of full sized fragments. These cover an entire quad face.
    half_size_fragment_count - [out]
      Number of half sized fragments. These cover a corner of an N-gon
      and are used when N != 4.
  Returns:
    Total number of fragments.
  */
  unsigned int GetFragmentCounts(
    unsigned int& full_size_fragment_count,
    unsigned int& half_size_fragment_count
  ) const;

  /*
  Parameters:
    mesh_density - [in]
      MinimumMeshDensity() <= mesh_density <= MeshDensity()
  Returns:
    Total number of mesh quads in at the specified mesh density.
  */
  unsigned int TotalQuadCount(unsigned int mesh_density) const;

  /*
  Parameters:
    mesh_density - [in]
      MinimumMeshDensity() <= mesh_density <= MeshDensity()
  Returns:
    Total number of mesh points delivered at the specified mesh density.
  */
  unsigned int TotalPointCount(unsigned int mesh_density) const;

  /*
  Returns:
    Total number of mesh quads delivered at MaximumMeshDensity().
  */
  unsigned int MaximumDensityQuadCount() const;

  /*
  Parameters:
    mesh_density - [in]
      MinimumMeshDensity() <= mesh_density <= MeshDensity()
  Returns:
    Total number of mesh points delivered at MaximumMeshDensity().
  */
  unsigned int MaximumDensityPointCount() const;

  const MYON_BoundingBox SurfaceBoundingBox() const;
  const MYON_BoundingBox ControlNetQuadBoundingBox() const;
  /*
  Returns:
    If this->SubDAppearance() is MYON_SubDComponentLocation::ControlNet, then this->ControlNetQuadBoundingBox() is returned.
    Otherwise this->SurfaceBoundingBox() returned.
  */
  const MYON_BoundingBox BoundingBox() const;

private:
  void Internal_CopyFrom(const MYON_SubDMeshFragmentIterator& src);

  MYON_SubDMesh m_limit_mesh;
  MYON_SubD m_subd;
  MYON_SubDFaceIterator m_fit;
  const MYON_SubDMeshFragment* m_current_fragment = nullptr;
  bool m_bFromFaceFragments = false;
  mutable bool m_bHaveCounts = false;

  // used to override the appearance of SubD().SubDApperance().
  MYON_SubDComponentLocation m_subd_appearance_override = MYON_SubDComponentLocation::Unset;

private:
  // Rendering density = MaximumMeshDensity() - m_density_reduction.
  unsigned char m_reserved_density_reduction = 0; // 0 = none

private:
  mutable unsigned int m_maximum_mesh_density = 0; // See MaximumMeshDensity() comment

  // full sized fragment count (for quads)
  mutable unsigned int m_full_size_fragment_count = 0;

  // half sized fragment count (for n-gons with n != 4)
  mutable unsigned int m_half_size_fragment_count = 0;
};


//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDSectorIterator
//

class MYON_CLASS MYON_SubDSectorIterator
{
public:
  static const MYON_SubDSectorIterator Empty;

  MYON_SubDSectorIterator() = default;
  ~MYON_SubDSectorIterator() = default;
  MYON_SubDSectorIterator(const MYON_SubDSectorIterator&) = default;
  MYON_SubDSectorIterator& operator=(const MYON_SubDSectorIterator&) = default;


  /*
  Parameters:
    center_vertex - [in]
      The vertex to iterated around.
      center_vertex->Face(0) is used to select the sector.
  Returns:
    If input is valid, a pointer to the center vertex is returned.
    Otherwise, nullptr is returned.
  */
  const MYON_SubDVertex* Initialize(
    const MYON_SubDVertex* center_vertex
    );

  /*
  Parameters:
    initial_face - [in]
    iterator_orientation - [in]
      0: (more common) 
        "next" means counter-clockwise with respect to the orientation of initial_face
      1: (less common)
        "next" means clockwise with respect to the orientation of initial_face
    center_vertex - [in]
      The vertex on initial_face that will be iterated around.
  Returns:
    If input is valid, a pointer to the center vertex is returned.
    Otherwise, nullptr is returned.
  */
  const MYON_SubDVertex* Initialize(
    const MYON_SubDFace* initial_face,
    MYON__UINT_PTR iterator_orientation,
    const MYON_SubDVertex* center_vertex
    );

  /*
  Parameters:
    initial_face - [in]
    iterator_orientation - [in]
      0: (more common) 
        "next" means counter-clockwise with respect to the orientation of initial_face
      1: (less common)
        "next" means clockwise with respect to the orientation of initial_face
    face_vertex_index - [in]
      initial_face->Vertex(face_vertex_index) is the center vertex
      that will be iterated around.
  Returns:
    If input is valid, a pointer to the center vertex is returned.
    Otherwise, nullptr is returned.
  */
  const MYON_SubDVertex* Initialize(
    const MYON_SubDFace* initial_face,
    MYON__UINT_PTR iterator_orientation,
    unsigned int face_vertex_index
    );

  bool InitializeToCurrentFace();

  void Initialize();

  /*
  Description:
  The current ring index reports the total increment from the
  start to the current state.  It can be positive or negative.
  */
  int CurrentRingIndex() const;

  const MYON_SubDVertex* CenterVertex() const;

  const MYON_SubDFace* InitialFace() const;

  unsigned int InitialFaceCenterVertexIndex() const;

  const MYON_SubDFace* CurrentFace() const;

  unsigned int CurrentFaceDirection() const;

  MYON_SubDFacePtr CurrentFacePtr() const;

  unsigned int CurrentFaceCenterVertexIndex() const;


  /*
  Description:

                CurrentEdge(1)
                    |
                    |
         NextFace() | CurrentFace()
                    |
                    |
                    *------------- CurrentEdge(0)
                        PrevFace()

    The asterisk (*) marks the center vertex.
    The diagram is With respect to the initial iterator orientation.

  Parameters:
    face_side_index - [in]
      CurrentEdge(0) = edge on the clockwise (PrevFace) side of the current face
      CurrentEdge(1) = edge on the counterclockwise (NextFace) side of the current face
      The directions "counterclockwise" and "clockwise" are with respect to the
      initial iterator orientation.

   Returns:
    The requested edge. The edge pointer is oriented such that 
    RelativeVertex(0) is the center vertex.
  */
  MYON_SubDEdgePtr CurrentEdgePtr(
    unsigned int face_side_index
    ) const;

  /*
  Description:

                CurrentEdge(1)
                    |
                    |
         NextFace() | CurrentFace()
                    |
                    |
                    *------------- CurrentEdge(0)
                        PrevFace()

    The asterisk (*) marks the center vertex.
    The diagram is With respect to the initial iterator orientation.

  Parameters:
    face_side_index - [in]
      CurrentEdge(0) = edge on the clockwise (PrevFace) side of the current face
      CurrentEdge(1) = edge on the counterclockwise (NextFace) side of the current face
      The directions "counterclockwise" and "clockwise" are with respect to the
      initial iterator orientation.
  Returns:
    The requested edge or nullptr if the iterator is not initialized, 
    has terminated, or is not valid. 
    When the sector iterator is initialized and valid, sit.CenterVertex() = CurrentEdge(*).RelativeVertex(0).
  */
  const MYON_SubDEdge* CurrentEdge(
    unsigned int face_side_index
    ) const;

  MYON__UINT_PTR CurrentEdgeDirection(
    unsigned int face_side_index
    ) const;

  /*
  Returns:
    The vertex on CurrentEdge(face_side_index) that is opposite
    the central vertex.
  */
  const MYON_SubDVertex* CurrentEdgeRingVertex(
    unsigned int face_side_index
    ) const;

  enum class StopAt : unsigned char
  {
    ///<summary>
    /// Sector iteration will terminate when the edge being crossed does not have two faces.
    ///</summary>
    Boundary = 0,

    ///<summary>
    /// Sector iteration will terminate when the edge being crossed does not have two faces
    /// or that edge is tagged as MYON_SubDEdgeTag::Crease.
    ///</summary>
    AnyCrease = 1,

    ///<summary>
    /// Sector iteration will terminate when the edge being crossed does not have two faces
    /// or that edge is tagged as MYON_SubDEdgeTag::Crease and has no dart vertices.
    ///</summary>
    HardCrease = 2,
  };

  /*
  Description:
    Advance the "current" face to the "next" face in the ring
    by crossing CurrentEdge(1).  
    
    If the current face is not reversed (1 == CurrentFaceDirection),
    then this rotation is counter-clockwise with respect to 
    the current face's orientation.
    
    If the current face is reversed (1 == CurrentFaceDirection),
    then this rotation is clockwise with respect to 
    the current face's orientation.
  Parameters:
    stop_at - [in]
      Determines that type of CurrentEdge(1) will terminate iteration.
  Returns:
    When the input CurrentEdge(1) has exactly two faces and
    is not tagged as a crease or bStopAtCrease is false, the 
    face on the other side of CurrentEdge(1) becomes the new
    current face and a pointer to this face is returned.
  Remarks:
    Identical to calling IncrementFace(+1,bStopAtCrease);
  */
  const MYON_SubDFace* NextFace(
    MYON_SubDSectorIterator::StopAt stop_at
    );

  /*
  Description:
    Advance the "current" face to the "previous" face in the ring
    by crossing CurrentEdge(0).  
    
    If the current face is not reversed (0 == CurrentFaceDirection),
    then this rotation is clockwise with respect to 
    the current face's orientation.
    
    If the current face is reversed (1 == CurrentFaceDirection),
    then this rotation is counter-clockwise with respect to 
    the current face's orientation.
  Parameters:
    stop_at - [in]
      Determines that type of CurrentEdge(0) will terminate iteration.
  Returns:
    When the input CurrentEdge(0) has exactly two faces and
    is not tagged as a crease or bStopAtCrease is false, the 
    face on the other side of CurrentEdge(0) becomes the new
    current face and a pointer to this face is returned.
    In all other cases, nullptr is returned
  Remarks:
    Identical to calling IncrementFace(-1,bStopAtCrease);
  */
  const MYON_SubDFace* PrevFace(
    MYON_SubDSectorIterator::StopAt stop_at
    );

  /*
  Description:
    Advance the "current" face by "hopping across" the edge
    CurrentEdge((increment_direction>0) ? 1 : 0).
    
    If the current face is not reversed (0 == CurrentFaceDirection),
    then increment_direction>0 rotates counter-clockwise with respect to 
    the current face's orientation.
    
    If the current face is reversed (1 == CurrentFaceDirection),
    then increment_direction>0 rotates clockwise with respect to 
    the current face's orientation.
  Parameters:
    increment_direction - [in]
      > 0 advance the "current" face to next face
      <= 0 advance the "current" face to previous face
    stop_at - [in]
      Determines that type of CurrentEdge((increment_direction>0) ? 1 : 0) will terminate iteration.
      If true and the input value of CurrentEdge((increment_direction>0) ? 1 : 0) 
      When iteration terminates at a crease, 
      CurrentFace() becomes nullptr
      CurrentEdge((increment_direction>0) ? 1 : 0) becomes nullptr
      CurrentEdge((increment_direction>0) ? 0 : 1) points at the crease
      and nullptr returned.
  Returns:
    nullptr if iteration terminates.
  */
  const MYON_SubDFace* IncrementFace(
    int increment_direction,
    MYON_SubDSectorIterator::StopAt stop_at
    );

  /*
  Description:
    Increment the iterator until it reaches a face with 
    a crease 
  Parameters:
    increment_direction - [in]
    > 0 advance next until CurrentEdge(1) is a crease.
    <= 0 advance previous until CurrentEdge(0) is a crease.
  Returns:
    nullptr - the sector has no creases.
    not nullptr - incremented to a crease
  */
  const MYON_SubDFace* IncrementToCrease(
    int increment_direction
    );

  /*
  Description:
    Reset iterator to initial face.
  */
  const MYON_SubDFace* FirstFace();

  bool IsValid() const;

private:
  const MYON_SubDVertex* m_center_vertex = nullptr;
  const MYON_SubDFace* m_initial_face = nullptr;
  const MYON_SubDFace* m_current_face = nullptr;

  // m_current_eptr[0].Edge() = "prev" side edge
  // m_current_eptr[1].Edge() = "next" side edge
  // center vertex = m_current_eptr[i].RelativeVertex(0)
  MYON_SubDEdgePtr m_current_eptr[2] = {}; // default = { MYON_SubDEdgePtr::Null, MYON_SubDEdgePtr::Null };

  unsigned int m_initial_fvi = 0;
  unsigned int m_current_fvi = 0;
  unsigned int m_current_fei[2] = {};  // default = { 0, 0 }; // "prev" and "next"

  // m_initial_face_dir 
  // 0: "next" means clockwise with respect to the initial face's orientation.
  // 1: "next" means counter-clockwise with respect to the initial face's orientation.
  unsigned int m_initial_face_dir = 0;

  // m_current_face_dir 
  // 0: "next" means clockwise with respect to the initial face's orientation.
  // 1: "next" means counter-clockwise with respect to the initial face's orientation.
  // When the subd faces around the center vertex are consistently oriented, 
  // m_current_face_dir is always equal to m_initial_face_dir.
  unsigned int m_current_face_dir = 0;

  int m_current_ring_index = 0;
};


//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDFaceEdgeIterator
//
class MYON_CLASS MYON_SubDFaceEdgeIterator
{
public:
  MYON_SubDFaceEdgeIterator();

  /*
  Description:
    Construct an iterator for going around the edges on a face.
  Parameters:
    face - [in]
    first_edge - [in]
      starting edge for the iterator or nullptr to start at face->Edge(0).
  */
  MYON_SubDFaceEdgeIterator(
    const MYON_SubDFace* face
    );

  MYON_SubDFaceEdgeIterator(
    const MYON_SubDFace* face,
    const MYON_SubDEdge* first_edge
    );


  /*
  Description:
    Initialize an iterator for going around the edges on a face.
  Parameters:
    face - [in]
    first_edge - [in]
      starting edge for the iterator or nullptr to start at face->Edge(0).
  */
  unsigned int Initialize(
    const MYON_SubDFace* face
    );

  unsigned int Initialize(
    const MYON_SubDFace* face,
    const MYON_SubDEdge* first_edge
    );

  unsigned int EdgeCount() const;

  /*
  Returns:
    Resets the iterator and returns the first edge.
  */
  const MYON_SubDEdge* FirstEdge();

  /*
  Description:
    Increments the iterator and returns the edge.
  */
  const MYON_SubDEdge* NextEdge();

  /*
  Description:
    Decrements the iterator and returns the edge.
  */
  const MYON_SubDEdge* PrevEdge();


  /*
  Parameters:
    bReturnNullAtFirstEdge = [in]
      If true and the next edge would be FirstEdge(), the nullptr is returned
      and CurrentEdge() is not changed.
  Description:
    Increments the iterator and returns the edge.
  */
  const MYON_SubDEdge* NextEdge(
    bool bReturnNullAtFirstEdge
  );

  /*
  Parameters:
    bReturnNullAtLastEdge = [in]
      If true and the previous edge would be FirstEdge(), the nullptr is returned
      and CurrentEdge() is not changed.
  Description:
    Decrements the iterator and returns the edge.
  */
  const MYON_SubDEdge* PrevEdge(
    bool bReturnNullAtFirstEdge
  );

  /*
  Returns:
  Current edge.
  */
  const MYON_SubDEdge* CurrentEdge() const;

  unsigned int FirstEdgeIndex() const;

  unsigned int CurrentEdgeIndex() const;

private:
  const MYON_SubDFace* m_face;
  unsigned int m_edge_count;
  unsigned int m_edge_index0;
  unsigned int m_edge_index;
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDFromMeshParameters
//
class MYON_CLASS MYON_SubDFromMeshParameters
{
public:

  // Default construction is identical to MYON_SubDFromMeshParameters::Smooth.
  MYON_SubDFromMeshParameters() = default;
  ~MYON_SubDFromMeshParameters() = default;
  MYON_SubDFromMeshParameters(const MYON_SubDFromMeshParameters&) = default;
  MYON_SubDFromMeshParameters& operator=(const MYON_SubDFromMeshParameters&) = default;

  ///////////////////////////////////////////////////////////////////////////////////////
  //
  // Crease options
  //

  // No interior creases and no corners.
  static const MYON_SubDFromMeshParameters Smooth;


  // Create an interior sub-D crease along all input mesh double edges.
  static const MYON_SubDFromMeshParameters InteriorCreases;

  // Look for convex corners at sub-D vertices with 2 edges
  // that have an included angle <= 90 degrees.
  static const MYON_SubDFromMeshParameters ConvexCornersAndInteriorCreases;

  ///////////////////////////////////////////////////////////////////////////////////////
  //
  // Custom interior crease options
  //
#pragma region RH_C_SHARED_ENUM [MYON_SubDFromMeshParameters::InteriorCreaseOption] [Rhino.Geometry.SubDCreationOptions.InteriorCreaseOption] [nested:byte]
  /// <summary>
  /// Specifies the test used to determine when an interior mesh edge generates an interior SubD creased edge.
  /// </summary>
  enum class InteriorCreaseOption : unsigned char
  {
    ///<summary>The interior crease test is not defined.</summary>
    Unset = 0,

    ///<summary>No interior creases.</summary>
    None = 1,

    ///<summary>An interior mesh double edge will create an interior SubD creased edge.
    /// An interior mesh double edge occurs when the sides of two mesh faces have
    /// have distinct vertex indices and identical vertex locations.
    ///</summary>
    AtMeshDoubleEdge = 2,
  };
#pragma endregion

  static MYON_SubDFromMeshParameters::InteriorCreaseOption InteriorCreaseOptionFromUnsigned(
    unsigned int interior_crease_option_as_unsigned
    );

  /*
  Parameters:
    interior_crease_option - [in]
  */
  void SetInteriorCreaseOption(
    MYON_SubDFromMeshParameters::InteriorCreaseOption interior_crease_option
    );

  /*
  Returns:
    The interior crease option.
  */
  MYON_SubDFromMeshParameters::InteriorCreaseOption GetInteriorCreaseOption() const;

  /*
  Description:
    Copy all settings that control how interior crease edges are created.
  Parameters:
    source_options - [in]
  Returns:
    The currently selected interior crease option.
  */
  MYON_SubDFromMeshParameters::InteriorCreaseOption CopyInteriorCreaseTest(
    MYON_SubDFromMeshParameters source_options
    );


  ///////////////////////////////////////////////////////////////////////////////////////
  //
  // Convex corner options
  //
#pragma region RH_C_SHARED_ENUM [MYON_SubDFromMeshParameters::ConvexCornerOption] [Rhino.Geometry.SubDCreationOptions.ConvexCornerOption] [nested:byte]
  /// <summary>
  /// Defines how convex corners are treated.
  /// </summary>
  enum class ConvexCornerOption : unsigned char
  {
    ///<summary>The option is not set.</summary>
    Unset = 0,

    ///<summary>No convex coners.</summary>
    None = 1,

    ///<summary>A convex subd corner will appear at input mesh boundary vertices 
    /// where the corner angle &lt;= MaximumConvexCornerAngleRadians() and
    /// the number of edges the end at the vertex is &lt;= MaximumConvexCornerEdgeCount().
    ///</summary>
    AtMeshCorner = 2
  };
#pragma endregion

  static MYON_SubDFromMeshParameters::ConvexCornerOption ConvexCornerOptionFromUnsigned(
    unsigned int convex_corner_option_as_unsigned
    );

  /*
  Parameters:
    convex_corner_option - [in]
  */
  void SetConvexCornerOption(
    MYON_SubDFromMeshParameters::ConvexCornerOption convex_corner_option
    );

  /*
  Returns:
    The currently selected convex corner option.
  */
  MYON_SubDFromMeshParameters::ConvexCornerOption GetConvexCornerOption() const;

  /*
  Description:
    If ConvexCornerTest() ConvexCornerOption::at_mesh_corner, then an
    input mesh boundary vertex becomes a sub-D corner when the number of
    edges that end at the vertex is <= MaximumConvexCornerEdgeCount() edges
    and the corner angle is <= MaximumConvexCornerAngleRadians().
  Parameters:
    maximum_convex_corner_edge_count - [in]
  */
  void SetMaximumConvexCornerEdgeCount(
    unsigned int maximum_convex_corner_edge_count
    );

  /*
  Description:
    If ConvexCornerTest() ConvexCornerOption::at_mesh_corner, then an
    input mesh boundary vertex becomes a sub-D corner when the number of
    edges that end at the vertex is <= MaximumConvexCornerEdgeCount() edges
    and the corner angle is <= MaximumConvexCornerAngleRadians().
  Returns:
    The maximum number of edges at a convex corner vertex.
  */
  unsigned int MaximumConvexCornerEdgeCount() const;

  /*
  Description:
    If ConvexCornerTest() ConvexCornerOption::at_mesh_corner, then an
    input mesh boundary vertex becomes a sub-D corner when the number of
    edges that end at the vertex is <= MaximumConvexCornerEdgeCount() edges
    and the corner angle is <= MaximumConvexCornerAngleRadians().
  Parameters:
    maximum_convex_corner_angle_radians - [in]
      > 0.0 and < MYON_PI
  */
  void SetMaximumConvexCornerAngleRadians(
    double maximum_convex_corner_angle_radians
    );

  /*
  Description:
    If ConvexCornerTest() ConvexCornerOption::at_mesh_corner, then an
    input mesh boundary vertex becomes a sub-D corner when the number of
    edges that end at the vertex is <= MaximumConvexCornerEdgeCount() edges
    and the corner angle is <= MaximumConvexCornerAngleRadians().
  Returns:
    The maximum corner angle.
  */
  double MaximumConvexCornerAngleRadians() const;
  
  /*
  Description:
    Copy all settings that control how convex corner vertices are created.
  Parameters:
    source_options - [in]
  Returns:
    The currently selected convex corner option.
  */
  MYON_SubDFromMeshParameters::ConvexCornerOption CopyConvexCornerTest(
    MYON_SubDFromMeshParameters source_parameters
    );


  ///////////////////////////////////////////////////////////////////////////////////////
  //
  // Concave corner options
  //
#pragma region RH_C_SHARED_ENUM [MYON_SubDFromMeshParameters::ConcaveCornerOption] [Rhino.Geometry.SubDCreationOptions.ConcaveCornerOption] [nested:byte]
  /// <summary>
  /// Defines how concave corners are treated.
  /// </summary>
  enum class ConcaveCornerOption : unsigned char
  {
    ///<summary>The option is not set.</summary>
    Unset = 0,

    ///<summary>No concave coners. In general, this is the best choice.</summary>
    None = 1,

    ///<summary>A concave subd corner will appear at input mesh boundary vertices 
    /// where the corner angle &gt;= MinimumConcaveCornerAngleRadians() and
    /// the number of edges the end at the vertex is &gt;= MinimumConcaveCornerEdgeCount().
    ///</summary>
    AtMeshCorner = 2
  };
#pragma endregion

  static MYON_SubDFromMeshParameters::ConcaveCornerOption ConcaveCornerOptionFromUnsigned(
    unsigned int concave_corner_option_as_unsigned
  );

  /*
  Parameters:
    concave_corner_option - [in]
  */
  void SetConcaveCornerOption(
    MYON_SubDFromMeshParameters::ConcaveCornerOption concave_corner_option
  );

  /*
  Returns:
    The currently selected concave corner option.
  */
  MYON_SubDFromMeshParameters::ConcaveCornerOption GetConcaveCornerOption() const;

  /*
  Description:
    If ConcaveCornerTest() = ConcaveCornerOption::AtMeshConcaveCorner, then an
    input mesh boundary vertex becomes a sub-D corner when the number of
    edges that end at the vertex is >= MinimumConcaveCornerEdgeCount() edges
    and the corner angle is >= MinimumConcaveCornerAngleRadians().
  Parameters:
    minimum_concave_corner_edge_count - [in]
  */
  void SetMinimumConcaveCornerEdgeCount(
    unsigned int minimum_concave_corner_edge_count
  );

  /*
  Description:
    If ConcaveCornerTest() ConcaveCornerOption::AtMeshConcaveCorner, then an
    input mesh boundary vertex becomes a sub-D corner when the number of
    edges that end at the vertex is >= MinimumConcaveCornerEdgeCount() edges
    and the corner angle is >= MinimumConcaveCornerAngleRadians().
  Returns:
    The minimum number of edges at a concave corner vertex.
  */
  unsigned int MinimumConcaveCornerEdgeCount() const;

  /*
  Description:
    If ConcaveCornerTest() ConcaveCornerOption::AtMeshConcaveCorner, then an
    input mesh boundary vertex becomes a sub-D corner when the number of
    edges that end at the vertex is >= MinimumConcaveCornerEdgeCount() edges
    and the corner angle is >= MinimumConcaveCornerAngleRadians().
  Parameters:
    minimum_concave_corner_angle_radians - [in]
      > MYON_PI and <= MYON_2PI
  */
  void SetMinimumConcaveCornerAngleRadians(
    double minimum_concave_corner_angle_radians
  );

  /*
  Description:
    If ConcaveCornerTest() ConcaveCornerOption::AtMeshConcaveCorner, then an
    input mesh boundary vertex becomes a sub-D corner when the number of
    edges that end at the vertex is >= MinimumConcaveCornerEdgeCount() edges
    and the corner angle is >= MinimumConcaveCornerAngleRadians().
  Returns:
    The minimum corner angle.
  */
  double MinimumConcaveCornerAngleRadians() const;

  /*
  Description:
    Copy all settings that control concave corner vertices are created.
  Parameters:
    source_options - [in]
  Returns:
    The currently selected concave corner option.
  */
  MYON_SubDFromMeshParameters::ConcaveCornerOption CopyConcaveCornerTest(
    MYON_SubDFromMeshParameters source_parameters
  );

  /*
  Returns:
    false - In MYON_SubD::CreateFromMesh(), input mesh vertex locations will be used to set subd vertex control net locations.
    true - In MYON_SubD::CreateFromMesh(), input mesh vertex locations will be used to set subd vertex limit surface locations.
  Remark:
    Interpolation computation is available in Rhino, Rhino compute, Rhino Inside,
    Grasshopper. Interpolation computation is not available in the opennurbs IO toolkit.
  */
  bool InterpolateMeshVertices() const;

  /*
  Parameters:
    false
      In MYON_SubD::CreateFromMesh(), input mesh vertex locations will be used to set subd vertex control net locations.
      This is the fastest and most robust way to create a subd from a mesh.
    true
      In MYON_SubD::CreateFromMesh(), input mesh vertex locations will be used to set subd vertex limit surface locations.
      If there are a large number of vertices, this option can require lots of computation.
  Remark:
    Interpolation computation is available in Rhino, Rhino compute, Rhino Inside,
    Grasshopper. Interpolation computation is not available in the opennurbs IO toolkit.
  */
  void SetInterpolateMeshVertices(
    bool bInterpolateMeshVertices
  );


  ///////////////////////////////////////////////////////////////////////////////////////
  //
  // Texture coordinates
  //
#pragma region RH_C_SHARED_ENUM [MYON_SubDFromMeshParameters::TextureCoordinatesOption] [Rhino.Geometry.SubDCreationOptions.TextureCoordinateOption] [nested:byte]
  /// <summary>
  /// Specifies how texture coordinate information is transferred from the mesh to the SubD.
  /// </summary>
  enum class TextureCoordinatesOption : unsigned char
  {
    ///<summary>The option is not set.</summary>
    Unset = 0,

    ///<summary>No texture coordianate information is transferred from the mesh.</summary>
    None = 1,

    ///<summary>
    /// If the mesh has a mapping, then TextureCoordinatesOption::CopyMapping is used.
    /// Otherwise if the mesh has texture coordinates, then TextureCoordinatesOption::CopyCoordinates is used.
    /// Otherwise TextureCoordinatesOption::Packed is used.
    ///</summary>
    Automatic = 2,

    ///<summary>
    /// No texture coordianate information is transferred from the mesh.
    /// The SubD faces are packed.
    ///</summary>
    Packed = 3,

    ///<summary>Texture coordinates mapping information is copied from the mesh.
    /// Generally this is the best choice because common mappings, like planar, 
    /// will appear as most people expect on the SubD.
    ///</summary>
    CopyMapping = 4,

    ///<summary>
    /// If a mesh has custom texture coordinates, the custom texture coordinates 
    /// are transferred to the SubD. This requires more memory resources, 
    /// slows subdivision evaluation, often produces unattractive
    /// results on n-gons, and distorts the texture when comes from a common mapping
    /// technique, like planar. This option may be useful when the mesh contains only 
    /// triangles and quads and the custom texture coordinates are of high quality.
    ///</summary>
    CopyCoordinates = 5,
  };
#pragma endregion


  /*
  Description:
    Set the texture coordinates option.
  Parameters:
    texture_coordinates_opton - [in]
  */
  void SetTextureCoordinatesOption(
    MYON_SubDFromMeshParameters::TextureCoordinatesOption texture_coordinates_opton
  );

  /*
  Returns:
    The texture coorindates option.
  */
  MYON_SubDFromMeshParameters::TextureCoordinatesOption GetTextureCoordinatesOption() const;

  static MYON_SubDFromMeshParameters::TextureCoordinatesOption TextureCoordinatesOptionFromUnsigned(
    unsigned int texture_coordinates_opton_as_unsigned
  );


  /*
  Returns:
    true - In MYON_SubD::CreateFromMesh(), colinear boundary edges belonging to the same face are merged into a single edge.
    false - In MYON_SubD::CreateFromMesh(), each mesh boundary edge becomes a SubD boundary edge.
  */
  bool MergeColinearBoundaryEdges() const;

  /*
  Parameters:
    bMergeColinearBoundaryEdges - [in]
      true 
        In MYON_SubD::CreateFromMesh(), colinear boundary edges belonging to the same face are merged into a single edge.
        In general, this is the best choice and leads to the highest quality of SubD object.
      false
        In MYON_SubD::CreateFromMesh(), each mesh boundary edge becomes a SubD boundary edge.
        Use this when the boundary topology of the mesh and SubD should be identical.
  */
  void SetMergeColinearBoundaryEdges(
    bool bMergeColinearBoundaryEdges
  );

  /*
  Returns:
    true - In MYON_SubD::CreateFromMesh(), colinear interior edges between two faces are merged into a single edge.
    false - In MYON_SubD::CreateFromMesh(), each mesh interior edge becomes a SubD interior edge.
  */
  bool MergeColinearInteriorEdges() const;

  /*
  Parameters:
  bMergeColinearInteriorEdges - [in]
    true
      In MYON_SubD::CreateFromMesh(), colinear interior edges between two faces are merged into a single edge.
      In general, this is the best choice and leads to the highest quality of SubD object.
    false
      In MYON_SubD::CreateFromMesh(), each mesh interior edge becomes a SubD interior edge.
      Use this when the interior topology of the mesh and SubD should be identical.
  */
  void SetMergeColinearInteriorEdges(
    bool bMergeColinearInteriorEdges
  );

  
private:
  enum : unsigned char
  {
    MergeColinearBoundaryEdgesMask = 1,
    MergeColinearInteriorEdgesMask = 2
  };
  unsigned char m_merge_edges_bits = 0; // clear bit (0) = "true, set bit (1) = "false"

  bool m_bInterpolateMeshVertices = false;

  MYON_SubDFromMeshParameters::InteriorCreaseOption m_interior_crease_option = MYON_SubDFromMeshParameters::InteriorCreaseOption::None;
  MYON_SubDFromMeshParameters::ConvexCornerOption m_convex_corner_option = MYON_SubDFromMeshParameters::ConvexCornerOption::None;
  MYON_SubDFromMeshParameters::ConcaveCornerOption m_concave_corner_option = MYON_SubDFromMeshParameters::ConcaveCornerOption::None;
  MYON_SubDFromMeshParameters::TextureCoordinatesOption m_texture_coordinates_option = MYON_SubDFromMeshParameters::TextureCoordinatesOption::None;

  unsigned short m_maximum_convex_corner_edge_count = 2U;

  unsigned short m_minimum_concave_corner_edge_count = 4U;

  unsigned short m_reserved2 = 0;
  unsigned int m_reserved3 = 0;

  double m_reserved4 = 0.0;

  double m_maximum_convex_corner_angle_radians = 120.0 * MYON_DEGREES_TO_RADIANS; // 120 degrees
  double m_minimum_concave_corner_angle_radians = 240.0 * MYON_DEGREES_TO_RADIANS; // 240 degrees
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDComponentRef
//
//  Used when an MYON_SubD vertex, edge or face needs to be sent around as
//  a piece of MYON_Geometry.
//
class MYON_CLASS MYON_SubDComponentRef : public MYON_Geometry
{
  MYON_OBJECT_DECLARE(MYON_SubDComponentRef);
public:
  static const MYON_SubDComponentRef Empty;

  MYON_SubDComponentRef() = default;
  ~MYON_SubDComponentRef() = default;
  MYON_SubDComponentRef(const MYON_SubDComponentRef&) MYON_NOEXCEPT;
  MYON_SubDComponentRef& operator=(const MYON_SubDComponentRef&);

#if defined(MYON_HAS_RVALUEREF)
  // rvalue copy constructor
  MYON_SubDComponentRef( MYON_SubDComponentRef&& ) MYON_NOEXCEPT;

  // The rvalue assignment operator calls MYON_Object::operator=(MYON_Object&&)
  // which could throw exceptions.  See the implementation of
  // MYON_Object::operator=(MYON_Object&&) for details.
  MYON_SubDComponentRef& operator=( MYON_SubDComponentRef&& );
#endif
  
  /*
  Parameters:
    subd_ref - [in]
    component_index - [in]
    component_location - [in]
  */
  static const MYON_SubDComponentRef Create(
    const MYON_SubDRef& subd_ref,
    MYON_COMPONENT_INDEX component_index,
    MYON_SubDComponentLocation component_location,
    MYON__UINT_PTR reference_id
    );

  static const MYON_SubDComponentRef Create(
    const MYON_SubDRef& subd_ref,
    MYON_SubDComponentPtr component_ptr,
    MYON_SubDComponentLocation component_location,
    MYON__UINT_PTR reference_id
    );

  static const MYON_SubDComponentRef Create(
    const MYON_SubDRef& subd_ref,
    const class MYON_SubDVertex* vertex,
    MYON_SubDComponentLocation component_location,
    MYON__UINT_PTR reference_id
    );

  static const MYON_SubDComponentRef Create(
    const MYON_SubDRef& subd_ref,
    const class MYON_SubDEdge* edge,
    MYON_SubDComponentLocation component_location,
    MYON__UINT_PTR reference_id
    );

  static const MYON_SubDComponentRef Create(
    const MYON_SubDRef& subd_ref,
    const class MYON_SubDFace* face,
    MYON_SubDComponentLocation component_location,
    MYON__UINT_PTR reference_id
    );

  void Clear();

  MYON_SubDRef SubDRef() const;

  const class MYON_SubD& SubD() const;

  MYON_COMPONENT_INDEX ComponentIndex() const override;

  MYON_SubDComponentPtr ComponentPtr() const;

  MYON_SubDComponentLocation ComponentLocation() const;

  /*
  Description:
    The interpretation of this value depends on the context.
    When the context is an MYON_SubDComponentRef created by
    CRhinoGetObject, ReferenceId() is the parent CRhinoSubDObject's
    runtime serial number.
  */
  MYON__UINT_PTR ReferenceId() const;

  void SetReferenceId(
    MYON__UINT_PTR reference_id
  );

  const class MYON_SubDVertex* Vertex() const;

  const class MYON_SubDEdge* Edge() const;

  const class MYON_SubDFace* Face() const;

  static int Compare(const MYON_SubDComponentRef* lhs, const MYON_SubDComponentRef* rhs);
  static int Compare2(const MYON_SubDComponentRef* const* lhs, const MYON_SubDComponentRef* const* rhs);
  
private:
  MYON_SubDRef m_subd_ref;
  MYON_SubDComponentPtr m_component_ptr = MYON_SubDComponentPtr::Null;
  MYON_COMPONENT_INDEX m_component_index = MYON_COMPONENT_INDEX::UnsetComponentIndex;
  MYON_SubDComponentLocation m_component_location =  MYON_SubDComponentLocation::Unset;
  MYON__UINT_PTR m_reference_id = 0;

public:
  // overrides of virtual MYON_Object functions
  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;
  void Dump( MYON_TextLog& ) const override;
  unsigned int SizeOf() const override;
  MYON::object_type ObjectType() const override;

  // overrides of virtual MYON_Geometry functions
  int Dimension() const override;

  // virtual MYON_Geometry GetBBox override		
  bool GetBBox( double* boxmin, double* boxmax, bool bGrowBox = false ) const override;
};

class MYON_CLASS MYON_SubDComponentRefList
{
public:
  MYON_SubDComponentRefList() = default;
  ~MYON_SubDComponentRefList();
  MYON_SubDComponentRefList(const MYON_SubDComponentRefList& src);
  MYON_SubDComponentRefList& operator=(const MYON_SubDComponentRefList& src);

  /*
  Description:
    Append a MYON_SubDComponentRef to the identified component.
  Returns;
    A pointer to the MYON_SubDComponentRef managed by this class or
    nullptr if the input is not valid.
  */
  const MYON_SubDComponentRef& Append(
    const MYON_SubDRef& subd_ref,
    MYON_COMPONENT_INDEX ci,
    MYON_SubDComponentLocation component_location,
    MYON__UINT_PTR reference_id
  );

  const MYON_SubDComponentRef& Append(
    const MYON_SubDRef& subd_ref,
    MYON_SubDComponentPtr component_ptr,
    MYON_SubDComponentLocation component_location,
    MYON__UINT_PTR reference_id
  );

  /*
  Description:
    Appends a copy of src_ref and returns a pointer to the MYON_SubDComponentRef
    managed by this class.
  */
  const MYON_SubDComponentRef& Append(const MYON_SubDComponentRef& src_ref);
  const MYON_SubDComponentRef& Append(const MYON_SubDComponentRef* src_ref);

  /*
  Description:
    Expert user function to transfer management of an MYON_SubDComponentRef on the heap
    to this class.
  */
  const MYON_SubDComponentRef& AppendForExperts(MYON_SubDComponentRef*& ref);

  /*
  Description:
    Expert user function to append a MYON_SubDComponentRef to the identified component.
    The expert user is responsible for insuring subd exists fot the lifetime of this
    class.
  */
  const MYON_SubDComponentRef& AppendForExperts(
    const MYON_SubD& subd,
    MYON_COMPONENT_INDEX ci,
    MYON_SubDComponentLocation component_location,
    MYON__UINT_PTR reference_id
  );

  const MYON_SubDComponentRef& AppendForExperts(
    const MYON_SubD& subd,
    MYON_SubDComponentPtr component_ptr,
    MYON_SubDComponentLocation component_location,
    MYON__UINT_PTR reference_id
  );

  /*
  Description:
    Sort by MYON_SubDComponentRef::Compare2() and remove duplicates and empty elements.
  Returns:
    Length of clean list.
  */
  int Clean();


  /*
  Returns:
    Number of refs in the list.
  */
  int Count() const;
  
  void Remove(int i);

  /*
  Description:
    Transfers the ref to an expert user who is responsible for properly managing it.
  */
  MYON_SubDComponentRef* TransferForExperts(int i);

  const MYON_SubDComponentRef& operator[](int) const;

  /*
  Returns:
    If the list is clean, the number of subd objects in the list. Multiple components can belong to the same SubD.
    If the list is not clean, 0.
  Remarks:
    Use Clean() to get a clean list.
  */
  int SubDCount() const;

  /* 
  Returns:
    Number of vertices.
  */
  int VertexCount() const;

  /* 
  Returns:
    Number of vertices with the specified tag.
  */
  int VertexCount(MYON_SubDVertexTag vertex_tag) const;

  /* 
  Returns:
    Number of edges.
  */
  int EdgeCount() const;

  /* 
  Returns:
    Number of edges with the specified tag.
  */
  int EdgeCount(MYON_SubDEdgeTag edge_tag) const;


  /* 
  Returns:
    Number of faces.
  */
  int FaceCount() const;

  /*
  Returns:
    Number of components.
  */
  int ComponentCount() const;

private:
#pragma MYON_PRAGMA_WARNING_PUSH
#pragma MYON_PRAGMA_WARNING_DISABLE_MSC( 4251 ) 
  // C4251: ... needs to have dll-interface to be used by clients of class ...
  // m_subdimple_sp is private and all code that manages m_subdimple_sp is explicitly implemented in the DLL.
  MYON_SimpleArray< class MYON_SubDComponentRef* > m_list;
#pragma MYON_PRAGMA_WARNING_POP

  int m_subd_count = 0;
  int m_subd_vertex_smooth_count = 0;
  int m_subd_vertex_dart_count = 0;
  int m_subd_vertex_crease_count = 0;
  int m_subd_vertex_corner_count = 0;
  int m_subd_edge_smooth_count = 0;
  int m_subd_edge_crease_count = 0;
  int m_subd_face_count = 0;

  bool m_bIsClean = false; // true if m_list is clean.

  unsigned char m_reserved1 = 0;
  unsigned short m_reserved2 = 0;
  unsigned int m_reserved3 = 0;
  MYON__UINT_PTR m_reserved4 = 0;

private:
  bool Internal_UpdateCount(const MYON_SubDComponentRef& r, int delta);
  void Internal_Destroy();
  void Internal_CopyFrom(const MYON_SubDComponentRefList& src);
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDComponentPoint
//
//  Used in selection tests to return a point and parameters on a component.
//
class MYON_CLASS MYON_SubDComponentPoint
{
public:
  static const MYON_SubDComponentPoint Unset;

  MYON_SubDComponentPoint() = default;
  ~MYON_SubDComponentPoint() = default;
  MYON_SubDComponentPoint(const MYON_SubDComponentPoint&) = default;
  MYON_SubDComponentPoint& operator=(const MYON_SubDComponentPoint&) = default;

  /*
  Description:
    Dictionary compare of component type, component pointer, and component direction (partial).
    This function is useful for sorting arrays of MYON_SubDComponentPoint values remove duplicates.
  */
  static int CompareComponentAndDirection(
    const MYON_SubDComponentPoint* a,
    const MYON_SubDComponentPoint* b
    );

  /*
  Description:
    Compare the pick events to determine the component the user was most likely aiming at.

  Parameters:
    pick_type - [in]
      When pick_type is MYON_PickType::PointPick, several biases may be applied to favor vertices and edges.

    vertex_depth_bias - [in]
      When in doubt pass 0.0.
      A positive value increases vertex bias in some situations; otherwise vertex_depth_bias is ignored.
      When pick_type is MYON_PickType::PointPick and either
      an edge and a vertex of that edge or a face and a vertex of that face are being compared,
      then then vertex_depth_bias is added to the vertex hit depth before comparing depths.
      When the pick is happening in a perspective view, it is important to choose a vertex_depth_bias
      appropriate for the depth in the view frustum.

    edge_depth_bias - [in]
      When in doubt pass 0.0.
      A positive value increases edge bias in some situations; otherwise vertex_depth_bias is ignored.
      When pick_type is MYON_PickType::PointPick and a face and an edge of that face are being compared,
      then then edge_depth_bias is added to the edge hit depth before comparing depths.
      When the pick is happening in a perspective view, it is important to choose an edge_depth_bias
      appropriate for the depth in the view frustum.

    A - [in]
    B - [in]
  Returns:
    A pick point for the component the user was most likely aiming at
    with distance and depth settings that will lead to conistent improvement
    if more than two points are being compared.
  */
  static const MYON_SubDComponentPoint BestPickPoint(
    MYON_PickType pick_type,
    double vertex_depth_bias,
    double edge_depth_bias,
    const MYON_SubDComponentPoint& A,
    const MYON_SubDComponentPoint& B
  );
  
  // m_component_ptr will be face, edge or vertex
  MYON_SubDComponentPtr m_component_ptr = MYON_SubDComponentPtr::Null;

  //// If the point is on a a face that does not have the ordinary number of 
  //// edges for the subdivision type, then m_face_corner_index identifies the
  //// subfragment corner.
  //unsigned int m_face_corner_index = MYON_UNSET_UINT_INDEX;

  //// If m_level_index is MYON_UNSET_UINT_INDEX, the point is on the limit surface.
  //// Otherwise the point is on the control net at the specified level.
  //unsigned int m_level_index = MYON_UNSET_UINT_INDEX;

  MYON_PickPoint m_pick_point = MYON_PickPoint::Unset;
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubDMatrix
//
class MYON_CLASS MYON_SubDMatrix
{
public:
  MYON_SubDMatrix() = default;

  static const MYON_SubDMatrix Empty;

  /*
  Description:
    Precise evaluation of cos(a) and cos(a) where a = i/n pi.
    These values are required for high qualitiy limit surface evaluation.
  Parameters:
    j - [in]
    n - [in]
    cos_theta - [out]
      cos(j/n pi)
    sin_theta - [out]
      sin(j/n pi)
  */
  static bool EvaluateCosAndSin(
    unsigned int j,
    unsigned int n,
    double* cos_theta,
    double* sin_theta
    );

  bool IsValid() const;

  bool IsValidPointRing(
    const double* point_ring,
    size_t point_ring_count,
    size_t point_ring_stride
    ) const;

  bool EvaluateSubdivisionPoint(
    unsigned int component_index,
    const double* point_ring,
    size_t point_ring_count,
    size_t point_ring_stride,
    double subd_point[3]
    ) const;

  bool EvaluateSurfacePoint(
    const double* point_ring,
    size_t point_ring_count,
    size_t point_ring_stride,
    bool bUndefinedNormalIsPossible,
    double limit_point[3],
    double limit_tangent1[3],
    double limit_tangent2[3],
    double limit_normal[3]
    ) const;

  bool EvaluateSurfacePoint(
    const double* point_ring,
    size_t point_ring_count,
    size_t point_ring_stride,
    bool bUndefinedNormalIsPossible,
    class MYON_SubDSectorSurfacePoint& limit_point
    ) const;

  /*
  Description:
    Get the subdivision matrix information for the case specified
    by the input parameters.  This information is retrieved from
    a cache.  In some cases, it will be calculated the first time
    it is needed.  
  Parameters:
    facet_type - [in]
    vertex_tag - [in]
    valence - [in]
      The input parameters identify the subdivision case.
  Remarks:
    Every member function of MYON_SubDMatrix, including this one
    is thread safe.
  */
  static const MYON_SubDMatrix& FromCache(
    MYON_SubDSectorType sector_type
    );

  MYON_SubDSectorType m_sector_type;

  unsigned int m_R = 0; // the matrix m_S is m_R x m_R (m_R = m_sector_type.PointRingCount())

  // The term "standard vertex ring points" is used below. 
  //
  // Let "C" be an interior vertex (m_vertex_tag is smooth or dart), 
  // (E[0], ...., E[N-1]) is a radially sorted list of its edges, 
  // (F[0], ..., F[N-1]) is a radially sorted list of its faces (F[i] is between E[i] and E[(i+1)%N]),
  // (P[0], ..., P[N-1]) are the vertices on the edges opposide Cn (P[i] is on E[i].
  // 
  // C is a "standard" smooth vertex if 
  // all edges E[i] are smooth with sharpness = 0,
  // and all faces f[i] are quads.
  // 
  // C is a "standard" dart vertex if
  // edge E[0] is a crease,
  // edges E[1], ..., E[N-1] are smooth with sharpness = 0,  
  // and all faces F[i] are quads.
  //
  // If If "C" is a boundary vertex (m_vertex_tag is crease or corner), the conditions
  // listed above are satisfied except 
  // E[0] and E[N-1] are crease edges, 
  // E[1], ..., E[N-2] are smooth edges with sharpness = 0, 
  // and faces f[0], ..., F[N-2] are quads.
  // 
  // If C is a standard interior smooth or dart vertex,
  // then the "standard vertex ring" is the list of 2*N+1 points 
  // (C, P[0], Q[0], ...., P[N-1], Q[N-1]), 
  // where Q[I] is the vertex of quad F[i] diagonally across from C.
  //
  // If C is a standard boundary vertex,
  // then the "standard vertex ring" is the list of 2*N points 
  // (C, P[0], Q[0], ...., P[N-1]).

  // m_S = R x R subdivision matrix
  // If (vertexR[0], ..., vertexR[R-1]) is a list of standard vertex ring points,
  // then then the location of the subdivided ring points
  // (vertexR1[0], ..., vertexR1[R-1]) can be calculated from m_S.
  // vertexR1[i] = m_S[i][0]*vertexR[0] + ... + m_S[i][R-1]*vertexR[R-1]
  const double* const* m_S = nullptr;

  // m_LP[] = limit point evaluation vector.
  // The array m_LP[] has m_R elements.
  // If (vertexR[0], ..., vertexR[R-1]) is a list of standard vertex ring points,
  // then Limit point = m_LP[0]*vertexR[0] + ... + m_LP[R-1]*vertexR[R-1].
  // m_LP is the eigenvector of Transpose(m_S) with eigenvalue = 1.
  // Length(m_LP) = 1.
  const double* m_LP = nullptr;

  // m_L1 and m_L2 = tangent space evaluation vectors.
  // The arrays m_L1[] and m_L2[] have m_R elements.
  // If (vertexR[0], ..., vertexR[R-1]) is a list of standard vertex ring points,
  // then the two vectors 
  // V1 = m_L1[0]*vertexR[0] + ... + m_L1[R-1]*vertexR[R-1].
  // V2 = m_L2[0]*vertexR[0] + ... + m_L2[R-1]*vertexR[R-1].
  // span the tangent plane and 
  // N = V1 x V2 is perpendicular to the limit tangent plane.
  // In general and almost always in practice, V1 and V2 are not unit vectors
  // and it is best to normalize V1 and V2 before taking the cross product.
  // m_L1 and m_L2 are subdominant eigenvectors of Transpose(m_S).
  // When the subdominant eigenvalue has geometric multiplicity 2, 
  // m_L1 and m_L2 span the same space as m_E1 and m_E2. 
  // The values stored in m_L1 and m_L2 are chosen to provide accurate
  // evaluation. In come common cases m_L1 and m_L2 are equal to m_E1 and m_E2,
  // but not in all cases.
  const double* m_L1 = nullptr;
  const double* m_L2 = nullptr;

  /*
  Description:
    Set the values in this MYON_SubDMatrix so the information 
    can be used to evaluate the case identified by the input
    parameters.
  Parameters:
    facet_type - [in]
    vertex_tag - [in]
    sector_edge_count - [in]
      The input parameters identify the subdivision case.
  Returns:
    R > 0: Success.  The matrix is R x R.
    0: Failure.
  */
  unsigned int SetFromSectorType(
    MYON_SubDSectorType sector_type
    );

  /*
  Returns:
    MYON_UNSET_VALUE - serious error
    >= 0:
      Maximum value of numbers that should be zero in and ideal world.  
      When the matrices, eigenvalues and eigenvectors are correctly calculated, 
      this returned value is in the range from 1e-16 to 5e-13 as valence goes 
      from 3 to 100.
      For valences < 100, if a value larger than 1.0e-12 occurs, there is a bug in the code.
  */
  double TestMatrix() const;

  /*
  Description:
    Run evaluation tests on this subdivision case.
  Returns:
    >= 0.0: Test passed. Maximum deviation found in any test is returned.
    MYON_UNSET_VALUE: Test failed.
  */
  double TestEvaluation() const;

  /*
  Description:
    Run evaluation tests on a range of subdivision cases.
  Parameters:
    sector_type - [in]
      If MYON_SubDSectorType::Empty, then all supported sector types types are tested.
    minimum_sector_face_count - [in]
      If 0, then testing begins at MYON_SubDSectorType::MinimumSectorFaceCount(vertex_tag)
      when testing vertex_tag types
    maximum_sector_face_count - [in]
      If 0, then testing stops at MYON_SubD::maximum_evaluation_valence.
    text_log - [out]
      If not nullptr, then a brief written report is printed for each test case.
  Returns:
    >= 0.0: Test passed.  Maximum deviation found in any test is returned.
    MYON_UNSET_VALUE: Test failed.    
  */
  static double TestEvaluation(
    MYON_SubDSectorType sector_type,
    unsigned int minimum_sector_face_count,
    unsigned int maximum_sector_face_count,
    MYON_TextLog* text_log
    );

  double TestComponentRing(
    const MYON_SubDComponentPtr* component_ring,
    size_t component_ring_count
    ) const;

  /*
  Description:
    Test cached subdivision matrix on sector identified by sit.
  Parameters:
    subd_type - [in]
    subd_recursion_count - [in]
      number of times to subdivide
    sit - [in]
      vertex to subdivide
    component_ring - [out]
    subd_points - [out]
    limit_point - [out]
    limit_tangent0 - [out]
    limit_tangent1 - [out]
    limit_normal - [out]
  */
  static double TestEvaluation(
    const unsigned int subd_recursion_count,
    MYON_SubDSectorIterator sit,
    MYON_SimpleArray<MYON_SubDComponentPtr>& component_ring,
    MYON_SimpleArray< MYON_3dPoint >& subd_points,
    class MYON_SubDSectorSurfacePoint& limit_point
    );

private:
  unsigned int m__max_R = 0;
  MYON_Matrix m__S; // m_S matrix memory
  MYON_SimpleArray<double> m__buffer; // m_LP, m_L1, m_L2, m_E1, m_E2 memory  
};

//////////////////////////////////////////////////////////////////////////
//
// MYON_SubD_FixedSizeHeap
//

/*
Description:
  A MYON_SubD_FixedSizeHeap is used to manage heap used for a local subdivision.
*/
class MYON_CLASS MYON_SubD_FixedSizeHeap
{
private:
  static unsigned int m__sn_factory;

public:
  // The serial number is used for debugging purposes.
  const unsigned int m_sn = ++m__sn_factory;

public:
  MYON_SubD_FixedSizeHeap() = default;
  ~MYON_SubD_FixedSizeHeap();

  /*
  Description:
    Reserve enough room to for a subdivision of a vertex sector.
  Parameters:
    sector_edge_count - [in]
      Number of edges in the sector.
  */
  bool ReserveSubDWorkspace(
    unsigned int sector_edge_count
    );

  /*
  Description:
    Reserve enough room for a local subdivide the neighborhood of center_face.
  Parameters:
    center_face0 - [in]
  */
  bool ReserveSubDWorkspace(
    const MYON_SubDFace* center_face0
    );

private:
  bool Internal_ReserveSubDWorkspace(
    size_t vertex_capacity,
    size_t face_capacity,
    size_t array_capacity,
    bool bEnableHashTable
    );

  bool Internal_ReserveSubDWorkspace_HashTable();

public:

  /*
  Description:
    Reset this MYON_SubD_FixedSizeHeap so it can be used again.
  */
  void Reset();

  /*
  Description:
    Deallocate all reserved heap.
  */
  void Destroy();

  bool InUse() const;

   MYON_SubDVertex* AllocateVertex(
    const double vertexP[3],
    unsigned int edge_capacity
    );

  /*
  Description:
    Allocate a vertex located at the vertex0 subdivision point. 
    
  Parameters:
    vertex0 - [in]
    edge_capacity - [in]
      If the returned vertex will be interior in a local subdivision,
      then pass vertex0->EdgeCount().
      If the returned vertex will be an outer ring vertex in a local subdivision,
      then pass 3.
  */
  MYON_SubDVertex* AllocateVertex(
    const MYON_SubDVertex* vertex0,
    unsigned int edge_capacity
    );

  /*
  Description:
    Allocate a vertex located at the edge0 subdivision point. 
    The vertex will have an edge and face capacity of 4.
  Parameters:
    edge0 - [in]
  */
  MYON_SubDVertex* AllocateVertex(
    const MYON_SubDEdge* edge0
    );

  /*
  Description:
    Find or allocate a vertex located at the edge0 subdivision point. 
    The vertex will have an edge and face capacity of 4.
  Parameters:
    edge0 - [in]
  Remarks: 
    In order for FindOrAllocateVertex() to find a vertex, that vertex must 
    have been created  by an earlier call to FindOrAllocateVertex().
    Typically, AllocateVertex(edge0) is used for center face boundary edges
    and FindOrAllocateVertex(edge0) is used for ring edges.
  */
  MYON_SubDVertex* FindOrAllocateVertex(
    const MYON_SubDEdge* edge0
    );

  /*
  Description:
    Allocate a vertex located at the edge0 subdivision point.
    The vertex will have an edge and face capacity of 4.
  Parameters:
    edge0 - [in]
  */
  MYON_SubDVertex* AllocateEdgeSubdivisionVertex(
    bool bUseFindOrAllocate,
    const MYON_SubDEdge * edge0
  );

  /*
  Description:
    Find or allocate a vertex and the face subdivision point. The vertex will have an
    edge and face capacity of face0->EdgeCount().
  Parameters:
    face0 - [in]
      outer face in a local subdivision situation
  Remarks: 
    In order for FindOrAllocateVertex() to find a vertex, that vertex must 
    have been created  by an earlier call to FindOrAllocateVertex().
    Typically, AllocateVertex(edge0) is used for the center face and
    and FindOrAllocateVertex(edge0) is used for ring faces.
  */
  MYON_SubDVertex* FindOrAllocateVertex(
    const MYON_SubDFace* face0
    );


  /*
  Description:
    Allocate a vertex located at the sector_face0 subdivision point. 
    The vertex will have an edge and face capacity of 3.
  Parameters:
    sector_face0 - [in]
      A face in a vertex sector.
  */
  MYON_SubDVertex* AllocateSectorFaceVertex(
    const MYON_SubDFace* sector_face0
    );


  /*
  Parameters:
    v0 - [in]
    v0_sector_coefficient - [in]
      If v0 null or MYON_SubDVertexTag::Smooth == v0->m_vertex_tag, and v1 is null or tagged,
      then m_sector_coefficient[0] is set to v0_sector_coefficient.
      In all other cases the value of v0_sector_coefficient is ignored and m_sector_coefficient[0]
      is set to MYON_SubDSectorType::IgnoredSectorCoefficient.
    v1 - [in]
    v1_sector_coefficient - [in]
      If v1 null or MYON_SubDVertexTag::Smooth == v1->m_vertex_tag, and v0 is null or tagged,
      then m_sector_coefficient[1] is set to v1_sector_coefficient.
      In all other cases the value of v1_sector_coefficient is ignored and m_sector_coefficient[1]
      is set to MYON_SubDSectorType::IgnoredSectorCoefficient.
  Returns:
    An edge.
    The vertex parameter information is used to set the MYON_SubDEdge.m_vertex[] 
    and MYON_SubDEdge.m_sector_coefficient[] values.
    If v0 and v1 are not null and both are tagged, then MYON_SubDEdge.m_edge_tag is 
    set to MYON_SubDEdgeTag::Crease.  
    In all other cases, MYON_SubDEdge.m_edge_tag is set to MYON_SubDEdgeTag::Smooth.
    If v0 or v1 is not null, then MYON_SubDEdge.m_level is set to the
    maximum of v0->m_level or v1->m_level.
  */
  const MYON_SubDEdgePtr AllocateEdge(
    MYON_SubDVertex* v0,
    double v0_sector_coefficient,
    MYON_SubDVertex* v1,
    double v1_sector_coefficient
    );

  /*
  Description:
    NOTE WELL: 
    In order for FindOrAllocateEdge() to find an edge, that edge must have been created 
    by an earlier call to FindOrAllocateEdge().
  */
  const MYON_SubDEdgePtr FindOrAllocateEdge(
    MYON_SubDVertex* v0,
    double v0_sector_coefficient,
    MYON_SubDVertex* v1,
    double v1_sector_coefficient
    );

  const MYON_SubDEdgePtr AllocateEdge(
    bool bUseFindOrAllocatEdge,
    MYON_SubDVertex* v0,
    double v0_sector_coefficient,
    MYON_SubDVertex* v1,
    double v1_sector_coefficient
  );



  private:
  /*
  Returns:
    A face with all field values zero (same values as MYON_SubDEdge::Face), except MYON_SubDFace.m_id.
  */
  MYON_SubDFace* Internal_AllocateFace(
    unsigned int zero_face_id,
    unsigned int parent_face_id
    );
  public:

  MYON_SubDFace* AllocateQuad(
    unsigned int zero_face_id,
    unsigned int parent_face_id,
    const MYON_SubDEdgePtr eptrs[4]
    );

  MYON_SubDFace* AllocateQuad(
    unsigned int zero_face_id,
    unsigned int parent_face_id,
    MYON_SubDEdgePtr e0,
    MYON_SubDEdgePtr e1,
    MYON_SubDEdgePtr e2,
    MYON_SubDEdgePtr e3
    );

  /*
  Parameters:
    capacity - [in]
      desired array size
    bZeroMemory - [in]
      If true, all array element values are zero.
      If false, array element values are undefined.
  Returns:
    An array of capacity MYON__UINT_PTR pointers.
  */
  MYON__UINT_PTR* AllocatePtrArray(
    unsigned int capacity,
    bool bZeroMemory
    );

  /*
  Description:
    Return the most recent array obtained from AllocatePtrArray().
    so it can be reused.  
  Returns:
    True: 
      Success.
    False: 
      Failure. The array was not the most recent array obtained
      from AllocatePtrArray().
  */
  bool ReturnPtrArray(
    void* p,
    unsigned int capacity
    );

private:
  MYON_SubDVertex* m_v = nullptr;
  unsigned int m_v_capacity = 0;
  unsigned int m_v_index = 0;

  MYON_SubDEdge* m_e = nullptr;
  unsigned int m_e_capacity = 0;
  unsigned int m_e_index = 0;

  MYON_SubDFace* m_f = nullptr;
  unsigned int m_f_capacity = 0;
  unsigned int m_f_index = 0;

  MYON__UINT_PTR* m_p = nullptr;
  unsigned int m_p_capacity = 0;
  unsigned int m_p_index = 0;

private:
  // Used to find level 1 subdivision vertex from level 0 component
  class MYON_SubD_FixedSizeHeap_ComponentPairHashElement** m_hash_table = nullptr;
  class MYON_SubD_FixedSizeHeap_ComponentPairHashElement* m_hash_elements = nullptr;
  unsigned int m_h_capacity = 0; // m_hash_table[] capacity.
  unsigned int m_h_count = 0; // 0xFFFFFFFFU means hash is disabled
  enum : unsigned int
  {
    DisabledHashCount = 0xFFFFFFFFU
  };

  bool Internal_HashEnabled() const;
  unsigned int Internal_Hash(MYON_SubDComponentPtr component0);
  class MYON_SubDVertex* Internal_HashFindVertex1(unsigned int hash, MYON_SubDComponentPtr component0);
  void Internal_HashAddPair(unsigned int hash, MYON_SubDComponentPtr component0, class MYON_SubDVertex* vertex1);

private:
  // copies not supported
  MYON_SubD_FixedSizeHeap(const MYON_SubD_FixedSizeHeap&) = delete;
  MYON_SubD_FixedSizeHeap& operator=(const MYON_SubD_FixedSizeHeap&) = delete;
};

class MYON_CLASS MYON_SubDEdgeChain
{
public:
  MYON_SubDEdgeChain() = default;
  ~MYON_SubDEdgeChain() = default;
  MYON_SubDEdgeChain(const MYON_SubDEdgeChain&) = default;
  MYON_SubDEdgeChain& operator=(const MYON_SubDEdgeChain&) = default;

public:
  static const MYON_SubDEdgeChain Empty;

public:

  /////////////////////////////////////////////////////////
  //
  // Edge chain tools
  //

  /*
  Description:
    Unconditionally sort edges into chains.

  Parameters:
    unsorted_edges - [in]
      If unsorted_edges[] contains three or more edges that share a common vertex,
      no edge chain will pass through that vertex.
      To sort an array in place, pass the same array as both parameters.

    sorted_edges - [out]
      The sorted_edges[] has the edges grouped into edge chains.
      A value of MYON_SubDEdgePtr::Null is at the end of every chain
      including the last chain.

      In an edge chain subsequent edges share a common vertex; i.e.
      sorted_edges[i].RelativeVertex(1) == sorted_edges[i+1].RelativeVertex(0).
  Returns:
    Number of chains in edge_chains[].
  Remarks:
    This version of SortEdgesIntoEdgeChains() uses MarkBits() on the edges
    and vertices in unsorted_edges[]. The output values of MarkBits()
    on these components are zero.
    Multiple threads may not simultaneously use any SubD tools on that rely
    on markbits on the same MYON_SubD.
  */
  static unsigned int SortEdgesIntoEdgeChains(
    const MYON_SimpleArray< MYON_SubDEdgePtr >& unsorted_edges,
    MYON_SimpleArray< MYON_SubDEdgePtr >& sorted_edges
  );

  /*
  Description:
    Unconditionally sort edges into chains.

  Parameters:
    unsorted_edges - [in]
      If unsorted_edges[] contains three or more edges that share a common vertex,
      no edge chain will pass through that vertex.

    sorted_edges - [out]
      The sorted_edges[] has the edges grouped into edge chains.
      A value of MYON_SubDEdgePtr::Null is at the end of every chain
      including the last chain.

      In an edge chain subsequent edges share a common vertex; i.e.
      sorted_edges[i].RelativeVertex(1) == sorted_edges[i+1].RelativeVertex(0).
  Returns:
    Number of chains in edge_chains[].
  Remarks:
    This version of SortEdgesIntoEdgeChains() uses MarkBits() on the edges
    and vertices in unsorted_edges[]. The output values of MarkBits()
    on these components are zero.
    Multiple threads may not simultaneously use any SubD tools on that rely
    on markbits on the same MYON_SubD.
  */
  static unsigned int SortEdgesIntoEdgeChains(
    const MYON_SimpleArray< const MYON_SubDEdge* >& unsorted_edges,
    MYON_SimpleArray< MYON_SubDEdgePtr >& sorted_edges
  );

  /*
  Description:
    Unconditionally sort edges into chains.

  Parameters:
    unsorted_edges - [in]
      If unsorted_edges[] contains three or more edges that share a common vertex,
      no edge chain will pass through that vertex.

    sorted_edges - [out]
      The sorted_edges[] has the edges grouped into edge chains.
      A value of MYON_SubDEdgePtr::Null is at the end of every chain
      including the last chain.

      In an edge chain subsequent edges share a common vertex; i.e.
      sorted_edges[i].RelativeVertex(1) == sorted_edges[i+1].RelativeVertex(0).
  Returns:
    Number of chains in edge_chains[].
  Remarks:
    This version of SortEdgesIntoEdgeChains() uses MarkBits() on the edges
    and vertices in unsorted_edges[]. The output value of MarkBits()
    on these components is zero.
    Multiple threads may not simultaneously use any SubD tools on that rely
    on markbits on the same MYON_SubD.
  */
  static unsigned int SortEdgesIntoEdgeChains(
    const MYON_SimpleArray< MYON_SubDComponentPtr >& unsorted_edges,
    MYON_SimpleArray< MYON_SubDEdgePtr >& sorted_edges
  );

  /*
  Description:
    Unconditionally sort edges into chains.

  Parameters:
    unsorted_edges - [in]
      If unsorted_edges[] contains three or more edges that share a common vertex,
      no edge chain will pass through that vertex.

    sorted_edges - [out]
      The sorted_edges[] has the edges grouped into edge chains.
      A value of MYON_SubDEdgePtr::Null is at the end of every chain
      including the last chain.

      In an edge chain subsequent edges share a common vertex; i.e.
      sorted_edges[i].RelativeVertex(1) == sorted_edges[i+1].RelativeVertex(0).
  Returns:
    Number of chains in edge_chains[].
  Remarks:
    This version of SortEdgesIntoEdgeChains() uses MarkBits() on the edges
    and vertices in unsorted_edges[]. The output value of MarkBits()
    on these components is zero.
    Multiple threads may not simultaneously use any SubD tools on that rely
    on markbits on the same MYON_SubD.
  */
  static unsigned int SortEdgesIntoEdgeChains(
    const MYON_SubD& subd,
    const MYON_SimpleArray< MYON_COMPONENT_INDEX >& unsorted_edges,
    MYON_SimpleArray< MYON_SubDEdgePtr >& sorted_edges
  );

  /*
  Description:
    Determine if the array of MYON_SubDEdgePtrs in edges[] can be sorted
    into a single edge chain.
  Parameters:
    edges - [in]
      Set of edges to test.
    bIsClosed - [out]
      If true is returned and edges[] sorts into a closed edge chain,
      then bIsClosed is true. Otherwise bIsClosed is false.
    bIsSorted - [out]
      If true is returned and edges[] is currently sorted into an edge chain,
      then bIsSorted is true. Otherwise bIsSorted is false.
  Returns:
    If the array of edges[] can be sorted into a single edge chain
    with no self intersections, then true is returned. Otherwise false
    is returned.
  Remarks:
    This test usesthe MarkBits() values on the edges and vertices and
    restores the values to the input state.
    Multiple threads may not simultaneously use any SubD tools on that rely
    on markbits on the same MYON_SubD.
  */
  static bool IsSingleEdgeChain(
    const MYON_SimpleArray<MYON_SubDEdgePtr>& edges,
    bool& bIsClosed,
    bool& bIsSorted
  );

  /*
  Description:
    Determine if the array of MYON_SubDEdgePtrs in edges[] can be sorted
    into a single edge chain.
  Parameters:
    edges - [in]
      Set of edges to test.
  Returns:
    If the array of edges[] can be sorted into a single edge chain
    with no self intersections, then true is returned. Otherwise false
    is returned.
  Remarks:
    This test usesthe MarkBits() values on the edges and vertices and
    restores the values to the input state.
    Multiple threads may not simultaneously use any SubD tools on that rely
    on markbits on the same MYON_SubD.
  */
  static bool IsSingleEdgeChain(
    const MYON_SimpleArray<MYON_SubDEdgePtr>& edges
  );

  /*
  Description:
    Sort edges into a chains

  Parameters:
    unsorted_edges - [in]
      To sort an array in place, pass the same array as both parameters.
      If unsorted_edges[] contains three or more edges that share a common vertex,
      then all of the edges that share that vertex are ignored.
      The edges can be from one or more SubDs.

    unsigned int minimum_chain_length - [in]
      minimum number of edges to consider for a chain.

    edge_chains - [out]
      The edge_chains[] has the edges grouped into edge chains.

      In an edge chain subsequent edges share a common vertex; i.e.
      edge_chains[i].RelativeVertex(1) == edge_chains[i+1].RelativeVertex(0).

      When edge_chains[i].RelativeVertex(1) != edge_chains[i+1].RelativeVertex(0),
      a chain ends at edge_chains[i] and another begins at edge_chains[i+1].

      Reasonable effort is made to keep the first edge in every chain with the
      same orientation as the input edge from edge_chains[]. However, this is not
      possible in every case, for example if the input is two edges sharing the
      same starting vertex.

      NB: Reasonable effort is made to keep the corner vertices on the exterior
      of the chains, however some chains will have corners in their interior,
      especially closed chains.

    bIgnoreCorners - [in]
      If true the chains are not split at corner vertices

  Returns:
    Number of chains in edge_chains[].

  Remarks:
    When the orientation of the input edges is not intentionally set,
    the versions of SortEdgesIntoEdgeChains() above without a minimum_chain_length
    variable are a better choice.
  */
  static unsigned int SortEdgesIntoEdgeChains(
    const MYON_SimpleArray< MYON_SubDEdgePtr >& unsorted_edges,
    unsigned int minimum_chain_length,
    MYON_SimpleArray< MYON_SubDEdgePtr >& edge_chains,
    const bool bIgnoreCorners = false
  );

  /*
  Description:
    Sort edges into a chains

  Parameters:
    unsorted_edges - [in]
      To sort an array in place, pass the same array as both parameters.
      If unsorted_edges[] contains three or more edges that share a common vertex,
      then all of the edges that share that vertex are ignored.
      The edges can be from one or more SubDs.

    unsigned int minimum_chain_length - [in]
      minimum number of edges to consider for a chain.

    edge_chains - [out]
      The edge_chains[] has the edges grouped into edge chains.

      In an edge chain subsequent edges share a common vertex; i.e.
      edge_chains[i].RelativeVertex(1) == edge_chains[i+1].RelativeVertex(0).

      When edge_chains[i].RelativeVertex(1) != edge_chains[i+1].RelativeVertex(0),
      a chain ends at edge_chains[i] and another begins at edge_chains[i+1].

      The first edge in every chain has the same orientation as the input edge
      from edge_chains[].

    bIgnoreCorners - [in]
      If true the chains are not split at corner vertices

  Returns:
    Number of chains in edge_chains[].

  Remarks:
    When the orientation of the input edges is not intentionally set,
    the versions of SortEdgesIntoEdgeChains() above without a minimum_chain_length
    variable are a better choice.
  */
  static unsigned int SortEdgesIntoEdgeChains(
    const MYON_SimpleArray< const MYON_SubDEdge* >& unsorted_edges,
    unsigned int minimum_chain_length,
    MYON_SimpleArray< MYON_SubDEdgePtr >& edge_chains,
    const bool bIgnoreCorners = false
  );

  /*
  Description:
    Sort edges into a chains

  Parameters:
    unsorted_edges - [in]
      To sort an array in place, pass the same array as both parameters.
      If unsorted_edges[] contains three or more edges that share a common vertex,
      then all of the edges that share that vertex are ignored.
      The edges can be from one or more SubDs.

    unsigned int minimum_chain_length - [in]
      minimum number of edges to consider for a chain.

    edge_chains - [out]
      The edge_chains[] has the edges grouped into edge chains.

      In an edge chain subsequent edges share a common vertex; i.e.
      edge_chains[i].RelativeVertex(1) == edge_chains[i+1].RelativeVertex(0).

      When edge_chains[i].RelativeVertex(1) != edge_chains[i+1].RelativeVertex(0),
      a chain ends at edge_chains[i] and another begins at edge_chains[i+1].

      The first edge in every chain has the same orientation as the input edge
      from edge_chains[].

  Returns:
    Number of chains in edge_chains[].

  Remarks:
    When the orientation of the input edges is not intentionally set,
    the versions of SortEdgesIntoEdgeChains() above without a minimum_chain_length
    variable are a better choice.
  */
  static unsigned int SortEdgesIntoEdgeChains(
    const MYON_SimpleArray< MYON_SubDComponentPtr >& unsorted_edges,
    unsigned int minimum_chain_length,
    MYON_SimpleArray< MYON_SubDEdgePtr >& sorted_edges
  );

  /*
  Description:
    Sort edges into a chains

  Parameters:
    unsorted_edges - [in]
      To sort an array in place, pass the same array as both parameters.
      If unsorted_edges[] contains three or more edges that share a common vertex,
      then all of the edges that share that vertex are ignored.
      The edges can be from one or more SubDs.

    unsigned int minimum_chain_length - [in]
      minimum number of edges to consider for a chain.

    edge_chains - [out]
      The edge_chains[] has the edges grouped into edge chains.

      In an edge chain subsequent edges share a common vertex; i.e.
      edge_chains[i].RelativeVertex(1) == edge_chains[i+1].RelativeVertex(0).

      When edge_chains[i].RelativeVertex(1) != edge_chains[i+1].RelativeVertex(0),
      a chain ends at edge_chains[i] and another begins at edge_chains[i+1].

      The first edge in every chain has the same orientation as the input edge
      from edge_chains[].

  Returns:
    Number of chains in edge_chains[].

  Remarks:
    When the orientation of the input edges is not intentionally set,
    the versions of SortEdgesIntoEdgeChains() above without a minimum_chain_length
    variable are a better choice.
  */
  static unsigned int SortEdgesIntoEdgeChains(
    const MYON_SubD& subd,
    const MYON_SimpleArray< MYON_COMPONENT_INDEX >& unsorted_edges,
    unsigned int minimum_chain_length,
    MYON_SimpleArray< MYON_SubDEdgePtr >& edge_chains
  );

  /*
  Description:
    Orient edges[] into edge chains preserving the order of edges[].
  Returns:
    Number of chains in edge_chains[].
  */
  static unsigned int OrientEdgesIntoEdgeChains(
    const MYON_SubD& subd,
    const MYON_SimpleArray< MYON_COMPONENT_INDEX >& edges,
    MYON_SimpleArray< MYON_SubDEdgePtr >& edge_chains
  );

  /*
  Description:
    Orient edges[] into edge chains preserving the order of edges[].
  Returns:
    Number of chains in edge_chains[].
  */
  static unsigned int OrientEdgesIntoEdgeChains(
    const MYON_SimpleArray< const MYON_SubDEdge* >& edges,
    MYON_SimpleArray< MYON_SubDEdgePtr >& edge_chains
  );

  /*
  Description:
    Orient edges[] into edge chains preserving the order of edges[].
  Returns:
    Number of chains in edge_chains[].
  */
  static unsigned int OrientEdgesIntoEdgeChains(
    const MYON_SimpleArray< MYON_SubDComponentPtr >& edges,
    MYON_SimpleArray< MYON_SubDEdgePtr >& edge_chains
  );

  /*
  Returns:
    A SHA1 hash of the edge and vertex ids. 
    Useful for determining when two edge chains from different
    subds (one generally a modified copy) involve the same
    edges and vertices.
  */
  const MYON_SHA1_Hash Hash() const;


  /////////////////////////////////////////////////////////
  //
  // Edge chain tools
  //

  /*
  Description:
    Get the neighboring link in an edge chain.
  Parameters:
    starting_edge - [in]
    search_direction - [in]
      MYON_ChainDirection::Previous or MYON_ChainDirection::Next.
      The search direction is relative to this->EdgeDirection().
    chain_type - [in]
      Determines what edge/vertex tag conditions must be satisfied by the neighbor.
  Returns:
    The next or previous edge in the chain if it exists.
    Otherwise, nullptr is returned.
  Remarks:
    When multiple edges are candidates, there is a bias to preserve smooth/crease and a bias to
    preserve face count. If the biases don't reduce the list of candidates to one or bStopAtBreak is true
    and a creaase/smooth break is encountered, then MYON_SubDEdgePtr::Null is returned.
  */
  static const MYON_SubDEdgePtr EdgeChainNeighbor(
    MYON_SubDEdgePtr starting_edge,
    MYON_ChainDirection search_direction,
    MYON_SubD::ChainType chain_type
  );

  /*
  Description:
    Get the neighboring link in an edge chain.
  Parameters:
    starting_edge - [in]
    search_direction - [in]
      MYON_ChainDirection::Previous or MYON_ChainDirection::Next.
      The search direction is relative to this->EdgeDirection().
    chain_type - [in]
      Determines what edge/vertex tag conditions must be satisfied by the neighbor.
    bEnableStatusCheck - [in]
    status_pass - [in]
    status_fail - [in]
      If bEnableStatusFilter is false, then no status checks are performed.
      If bEnableStatusFilter is true, MYON_ComponentStatus::StatusCheck(candidate_edge->m_status,status_pass,status_fail)
      must be true for candidate_edge to be returned.
  Returns:
    The next or previous edge in the chain if it exists.
    Otherwise, nullptr is returned.
  Remarks:
    When multiple edges are candidates, there is a bias to preserve smooth/crease and a bias to
    preserve face count. If the biases don't reduce the list of candidates to one or bStopAtBreak is true
    and a creaase/smooth break is encountered, then MYON_SubDEdgePtr::Null is returned.
  */
  static const MYON_SubDEdgePtr EdgeChainNeighbor(
    MYON_SubDEdgePtr starting_edge,
    MYON_ChainDirection search_direction,
    MYON_SubD::ChainType chain_type,
    bool bEnableStatusCheck,
    MYON_ComponentStatus status_pass,
    MYON_ComponentStatus status_fail
  );

  /*
  Description:
    Reverse the orientation of the elements and the order
    of the elements in the edge_chain[] array.
  Parameters:
    edge_chain - [in/out]
  */
  static void ReverseEdgeChain(
    MYON_SimpleArray< MYON_SubDEdgePtr >& edge_chain
  );

  /*
  Description:
    Reverse the orientation of the elements and the order
    of the elements in the edge_chain[] array.
  Parameters:
    edge_count - [in]
      Number of elements in edge_chain[]
    edge_chain - [in/out]
  */
  static void ReverseEdgeChain(
    MYON_SubDEdgePtr* edge_chain,
    size_t edge_count
    );
  
  /*
  Description:
    Checks that all edge and vertex pointers are not nullptr 
    and that adjacent edges in the list share the same vertex.
  Parameters:
    edge_chain - [in]
      Edge chain to test
    bCheckForDuplicateEdges - [in]
      If bCheckForDuplicateEdges true, then false is returned if
      there are duplicate edges or duplicate interior vertices.
  Returns:
    True if edge_chain[] is valid.
  */
  static bool IsValidEdgeChain(
    const MYON_SimpleArray< MYON_SubDEdgePtr >& edge_chain,
    bool bCheckForDuplicateEdges
  );

  /*
  Description:
    Checks that all edge and vertex pointers are not nullptr 
    and that adjacent edges in the list share the same vertex.
  Parameters:
    edge_count - [in]
      Number of elements in edge_chain[] to test.
    edge_chain - [in]
      Edge chain to test
    bCheckForDuplicateEdges - [in]
      If bCheckForDuplicateEdges true, then false is returned if
      there are duplicate edges or duplicate interior vertices.
  Returns:
    True if edge_chain[] is valid.
  */
  static bool IsValidEdgeChain(
    const MYON_SubDEdgePtr* edge_chain,
    size_t edge_count,
    bool bCheckForDuplicateEdges
  );


  /*
  Description:
    Get the edges and faces on a specified side of the edge chain.
  Parameters:
    relative_side - [in]
      0: Get edges and faces on the MYON_SubDEdgePtr::RelativeFace(0) side (left).
      1: Get edges and faces on the MYON_SubDEdgePtr::RelativeFace(1) side (right).
    
    side_components - [out]
      side_components[] is a sequence of made of vertices, edges, and faces that
      record the edges and faces that are on the specified side of the edge chain.
      
      When a vertex is in side_components[], MYON_SubDComponentPtr.Vertex() is not nullptr
      and the vertex is between two conscutive edges in the edge chain.
      
      When an edge is in side_components[], MYON_SubDComponentPtr.EdgePtr() is not nullptr,
      the edge is on the specified side of the edge chain (not in the edge chain),
      and MYON_SubDComponentPtr.EdgePtr().RelativeVertex(0) is a vertex on the edge chain.
      
      When a face is in side_components[], MYON_SubDComponentPtr.Face() is not nullptr,
      then at least one vertex of f is part of the edge chain and f is on the specified
      side of the edge chain.

      If a vertex v is side_components[], then it is preceded and followed by the same
      face (...f,v,f,...), there are consecutive edges in the edge chain (...e0,e1,...),
      and e0 and e1 are consecutive edges in f's boundary.

      If ...eptr0,f,eptr1,... is in side_components[], 
      v0 = eptr0.RelativeVertex(0),
      v1 = eptr0.RelativeVertex(0), 
      and v0 == v1,
      then eptr0 and eptr1 are consecutive edges in the boundary of f 
      and v0==v1 is a vertex in the edge chain.

      If ...eptr0,f,eptr1,... is in side_components[], 
      v0 = eptr0.RelativeVertex(0),
      v1 = eptr0.RelativeVertex(0), 
      and v0 != v1,
      then there is an edge c from v0 to v1 that is in the edge chain and
      eptr0,c,eptr1 are consecutive edges in the boundary of f.

      If ...eptr0,f,v,... is in side_components[] and v0 = eptr0.RelativeVertex(0),
      and then v0 != v, then there is an edge c from v0 to v that is in the edge chain,
      and eptr0,c are consecutive edges in the boundary of f.

      If ...v,f,eptr1,... is in side_components[] and v1 = eptr1.RelativeVertex(0),
      and then v != v1, then there is an edge c from v to v1 that is in the edge chain,
      and c,eptr1 consecutive edges in the boundary of f.

  Remarks:
    If the SubD is not an oriented manifold along the entire side of the chain,
    then there may be gaps in chain_side[]. When there are face fans at a chain
    vertex, there will be faces that do not have an edge on the chain.
  */
  bool GetSideComponents(
    unsigned relative_side,
    MYON_SimpleArray<MYON_SubDComponentPtr>& side_components
  ) const;

  /*
  See above.
  */
  static bool GetSideComponents(
    const MYON_SimpleArray<MYON_SubDEdgePtr>& edge_chain,
    unsigned relative_side,
    MYON_SimpleArray<MYON_SubDComponentPtr>& side_components
  );

public:
  /*
  Returns:
    Current EdgeChain
  */
  const MYON_SimpleArray<MYON_SubDEdgePtr>& EdgeChain() const;


  const MYON_SubD& SubD() const;
  const MYON_SubDRef SubDRef() const;

  bool InChain(
    const MYON_SubDEdgePtr edge_ptr
  ) const;

  bool InChain(
    const MYON_SubDEdge* edge
  ) const;

  bool InChain(
    const MYON_SubDVertex* vertex
  ) const;

  /*
  Returns:
    True if the edge chain has 3 or more edges that form a closed loop.
  */
  bool IsClosedLoop() const;

  /*
  Parameters:
    bStrictlyConvex - [in]
      If true, then a colinear pair of edges is not considered convex.
  Returns:
    True if the edge chain has 3 or more edges that forma a convex loop.
  */
  bool IsConvexLoop(
    bool bStrictlyConvex
  ) const;


  /*
  Parameters:
    persistent_subd_id - [in]
      If this edge chain needs to persist in a 3dm archive, then persistent_subd_id
      should identify the subd in its current context.
      If that context is an MYONX_Model, then the persistent id is the MYON_ModelGeometryComponent.Id().
      If that context is a CRhinoDoc, then the persistent id is CRhinoObject.ModelObjectId().
      If the edge chain is being used in a runtime context and will not be saved in a 3dm archive
      or otherwise serialized, then persistent_subd_id can be MYON_nil_uuid.

  */
  unsigned int BeginEdgeChain(
    MYON_UUID persistent_subd_id,
    MYON_SubDRef subd_ref,
    const MYON_SubDEdge* initial_edge
  );

  /*
  Parameters:
    persistent_subd_id - [in]
      If this edge chain needs to persist in a 3dm archive, then persistent_subd_id
      should identify the subd in its current context.
      If that context is an MYONX_Model, then the persistent id is the MYON_ModelGeometryComponent.Id().
      If that context is a CRhinoDoc, then the persistent id is CRhinoObject.ModelObjectId().
      If the edge chain is being used in a runtime context and will not be saved in a 3dm archive
      or otherwise serialized, then persistent_subd_id can be MYON_nil_uuid.

  */
  unsigned int BeginEdgeChain(
    MYON_UUID persistent_subd_id,
    MYON_SubDRef subd_ref,
    const MYON_SimpleArray<const MYON_SubDEdge* >& initial_edge_chain
  );

  /*
  Parameters:
    persistent_subd_id - [in]
      If this edge chain needs to persist in a 3dm archive, then persistent_subd_id
      should identify the subd in its current context.
      If that context is an MYONX_Model, then the persistent id is the MYON_ModelGeometryComponent.Id().
      If that context is a CRhinoDoc, then the persistent id is CRhinoObject.ModelObjectId().
      If the edge chain is being used in a runtime context and will not be saved in a 3dm archive
      or otherwise serialized, then persistent_subd_id can be MYON_nil_uuid.

  */
  unsigned int BeginEdgeChain(
    MYON_UUID persistent_subd_id,
    MYON_SubDRef subd_ref,
    const MYON_SubDEdge*const* initial_edge_chain,
    size_t edge_count
    );

  /*
  Parameters:
    persistent_subd_id - [in]
      If this edge chain needs to persist in a 3dm archive, then persistent_subd_id
      should identify the subd in its current context.
      If that context is an MYONX_Model, then the persistent id is the MYON_ModelGeometryComponent.Id().
      If that context is a CRhinoDoc, then the persistent id is CRhinoObject.ModelObjectId().
      If the edge chain is being used in a runtime context and will not be saved in a 3dm archive
      or otherwise serialized, then persistent_subd_id can be MYON_nil_uuid.

  */
  unsigned int BeginEdgeChain(
    MYON_UUID persistent_subd_id,
    MYON_SubDRef subd_ref,
    MYON_SubDEdgePtr initial_edge
  );

  /*
  Parameters:
    persistent_subd_id - [in]
      If this edge chain needs to persist in a 3dm archive, then persistent_subd_id
      should identify the subd in its current context.
      If that context is an MYONX_Model, then the persistent id is the MYON_ModelGeometryComponent.Id().
      If that context is a CRhinoDoc, then the persistent id is CRhinoObject.ModelObjectId().
      If the edge chain is being used in a runtime context and will not be saved in a 3dm archive
      or otherwise serialized, then persistent_subd_id can be MYON_nil_uuid.

  */
  unsigned int BeginEdgeChain(
    MYON_UUID persistent_subd_id,
    MYON_SubDRef subd_ref,
    const MYON_SimpleArray<MYON_SubDEdgePtr>& initial_edge_chain
  );

  /*
  Parameters:
    persistent_subd_id - [in]
      If this edge chain needs to persist in a 3dm archive, then persistent_subd_id
      should identify the subd in its current context.
      If that context is an MYONX_Model, then the persistent id is the MYON_ModelGeometryComponent.Id().
      If that context is a CRhinoDoc, then the persistent id is CRhinoObject.ModelObjectId().
      If the edge chain is being used in a runtime context and will not be saved in a 3dm archive
      or otherwise serialized, then persistent_subd_id can be MYON_nil_uuid.

  */
  unsigned int BeginEdgeChain(
    MYON_UUID persistent_subd_id,
    MYON_SubDRef subd_ref,
    const MYON_SubDEdgePtr* initial_edge_chain,
    size_t edge_count
    );

  unsigned int EdgeCount() const;

  void SetStatusCheck(
    bool bEnableStatusCheck,
    MYON_ComponentStatus status_check_pass,
    MYON_ComponentStatus status_check_fail
  );

  bool StatusCheckEnabled() const;

  const MYON_ComponentStatus StatusCheckPass() const;

  const MYON_ComponentStatus StatusCheckFail() const;

  bool StatusCheck(
    const MYON_SubDEdge* edge
  ) const;

  void Reverse();

  const MYON_SubDEdgePtr FirstEdgePtr() const;
  const MYON_SubDEdgePtr LastEdgePtr() const;
  const MYON_SubDEdgePtr EdgePtr(int edge_index) const;

  const MYON_3dPoint FirstControlNetPoint() const;
  const MYON_3dPoint LastControlNetPoint() const;
  const MYON_3dPoint ControlNetPoint(int vertex_index) const;

  const MYON_SubDEdge* FirstEdge() const;
  const MYON_SubDEdge* LastEdge() const;
  const MYON_SubDEdge* Edge(int edge_index) const;

  const MYON_SubDVertex* FirstVertex() const;
  const MYON_SubDVertex* LastVertex() const;
  const MYON_SubDVertex* Vertex(int vertex_index) const;

  unsigned int AddOneNeighbor(
    MYON_ChainDirection direction,
    MYON_SubD::ChainType chain_type
    );

  unsigned int AddAllNeighbors(
    MYON_ChainDirection direction,
    MYON_SubD::ChainType chain_type
    );

  unsigned int AddEdge(
    const MYON_SubDEdge* edge
    );

  unsigned int RemoveEdges(
    const MYON_SubDEdge* first_edge,
    const MYON_SubDEdge* last_edge
  );

  void ClearEdgeChain();

  /*
  Returns:
    The persistent id of the parent subd object.
    If the context is an MYONX_Model, then the persistent id is the MYON_ModelGeometryComponent.Id().
    If the context is a CRhinoDoc, then the persistent id is CRhinoObject.ModelObjectId().
  */
  const MYON_UUID PersistentSubDId() const;

  bool HasPersistentEdgeIds() const;

  bool HasRuntimeEdgePtrs() const;

  bool SetPersistentEdgeIdsFromRuntimeEdgePtrs() const;
    
  /*
  Description:
    In situations where this edge chain is being read from a 3dm archive,
    or a similar serialization context, this function uses the saved edge
    id information to initialize the runtime MYON_SubDEdgePtr information.
  Parameters:
    persistent_subd_id - [in]
      This id is passed to insure it matches the saved persistent_subd_id. 
      The source of the id depends on the context of the model managing the subd.
      If that context is an MYONX_Model, then the persistent id is the MYON_ModelGeometryComponent.Id().
      If that context is a CRhinoDoc, then the persistent id is CRhinoObject.ModelObjectId().
  Returns:
    True if successful.
    False if not successful.
  */
  bool SetRuntimeEdgePtrsFromPersistentSubD(
    MYON_UUID persistent_subd_id,
    MYON_SubDRef persistent_subd_ref
  );

  bool Write(class MYON_BinaryArchive& archive) const;
  bool Read(class MYON_BinaryArchive& archive);
  void Dump(class MYON_TextLog& text_log) const;

private:
  MYON_SubDRef m_subd_ref;
  MYON_SimpleArray<MYON_SubDEdgePtr> m_edge_chain;


  // If m_persistent_subd_id, then the id identifies the parent subd in the model.
  // In an MYONX_Model, this is the MYON_ModelGeometryComponent.Id() value
  // of the corresponding MYON_SubD in the MYONX_Model.
  // In Rhino, this is the CRhinoObject.ModelObjectId() value
  // of the corresponding CRhinoSubDObject in the CRhinoDoc.
  MYON_UUID m_persistent_subd_id = MYON_nil_uuid;

  // If m_persistent_subd_id is set and m_persistent_edge_id[] is not empty,
  // m_persistent_edge_id[] is a list of edge ids and orientations.
  // unsigned id = m_persistent_edge_id[] & 0xFFFFFFFF.
  // reversed orientation = (0 != (m_persistent_edge_id[] & 0x8000000000000000)).
  // The persistent id information is saved in 3dm archives. When that archive is read,
  // the SetRuntimeEdgePtrsFromPersistentSubD() can be used to set the runtime edge chain values.
  // These mutable fields are set by Write and used by Read.
  mutable MYON_SimpleArray<unsigned int> m_persistent_edge_id;
  mutable MYON_SimpleArray<MYON__UINT8> m_persistent_edge_orientation; // 0 = not reversed, 1 = reversed.

  MYON_UniqueTester m_unique_tester;
  MYON_ComponentStatus m_status_check_pass = MYON_ComponentStatus::NoneSet;
  MYON_ComponentStatus m_status_check_fail = MYON_ComponentStatus::Selected;
  bool m_bEnableStatusCheck = false;
};

class MYON_CLASS MYON_SubDComponentFilter
{
public:
  MYON_SubDComponentFilter() = default;
  ~MYON_SubDComponentFilter() = default;
  MYON_SubDComponentFilter(const MYON_SubDComponentFilter&) = default;
  MYON_SubDComponentFilter& operator=(const MYON_SubDComponentFilter&) = default;

public:

  ///<summary>
  /// No filters are set and all components are accepted.
  ///</summary>
  static const MYON_SubDComponentFilter Unset;

  ///<summary>
  /// Only vertices are accepted.
  ///</summary>
  static const MYON_SubDComponentFilter OnlyVertices;

  ///<summary>
  /// Only edges are accepted.
  ///</summary>
  static const MYON_SubDComponentFilter OnlyEdges;

  ///<summary>
  /// Only faces are accepted.
  ///</summary>
  static const MYON_SubDComponentFilter OnlyFaces;

  /*
  Parameters:
    bAcceptVertices - [in]
      If true, all vertices are accepted. Otherwise, all vertices are rejected.
    bAcceptEdges - [in]
      If true, all edges are accepted. Otherwise all edges are rejected.
    bAcceptFaces - [in]
      If true, all faces are accepted. Otherwise all faces are rejected.
  */
  static const MYON_SubDComponentFilter Create(
    bool bAcceptVertices,
    bool bAcceptEdges,
    bool bAcceptFaces
  );

public:
  ///<summary>
  /// Topology filters.
  ///</summary>
  enum class Topology : unsigned char
  {
    ///<summary>
    /// No topology filter.
    ///</summary>
    Unset = 0,

    ///<summary>
    /// A boundary vertex has a single sector bounded by two boundary edges.
    /// A boundary edge has a single face.
    /// A boundary face has at least one boundary edge.
    ///</summary>
    Boundary = 1,

    ///<summary>
    /// An interior vertex has the same number of edges and faces and all edges are interior.
    /// An interior edge has two faces.
    /// An interior face has all interior edges.
    ///</summary>
    Interior = 2,

    ///<summary>
    /// A nonmanifold vertex is a vertex that is neither boundary nor interior.
    /// A nonmanifold edge is an edge that is neither boundary nor interior.
    /// A nonmanifold face is a face that is neither boundary nor interior.
    ///</summary>
    Nonmanifold = 4,

    ///<summary>
    /// A component that is either boundary or interior.
    ///</summary>
    BoundaryOrInterior = 3,

    ///<summary>
    /// A component that is either boundary or nonmanifold.
    ///</summary>
    BoundaryOrNonmanifold = 5,

    ///<summary>
    /// A component that is either interior or nonmanifold
    ///</summary>
    InteriorOrNonmanifold = 6
  };

  bool AcceptComponent(
    const class MYON_Geometry* geometry
  ) const;

  bool AcceptComponent(
    MYON_COMPONENT_INDEX component_index,
    const class MYON_Geometry* geometry
  ) const;

  bool AcceptComponent(
    const class MYON_SubDComponentRef* cref
  ) const;

  /*
  Returns:
    True if the filter accepts the component. False otherwise.
  */
  bool AcceptComponent(MYON_SubDComponentPtr cptr) const;

  /*
  Returns:
    True if the filter accepts the vertex. False otherwise.
  */
  bool AcceptVertex(MYON_SubDVertexPtr vptr) const;

  /*
  Returns:
    True if the filter accepts the edge. False otherwise.
  */
  bool AcceptEdge(MYON_SubDEdgePtr eptr) const;

  /*
  Returns:
    True if the filter accepts the face. False otherwise.
  */
  bool AcceptFace(MYON_SubDFacePtr fptr) const;

  /*
  Returns:
    True if the filter accepts the vertex. False otherwise.
  */
  bool AcceptVertex(const MYON_SubDVertex* v) const;

  /*
  Returns:
    True if the filter accepts the edge. False otherwise.
  */
  bool AcceptEdge(const MYON_SubDEdge* e) const;
  
  /*
  Returns:
    True if the filter accepts the face. False otherwise.
  */
  bool AcceptFace(const MYON_SubDFace* f) const;  

  void SetAcceptVertices(bool bAcceptVertices);

  bool AcceptVertices() const;

  void SetAcceptEdges(bool bAcceptEdges);

  bool AcceptEdges() const;

  void SetAcceptFaces(bool bAcceptFaces);

  bool AcceptFaces() const;

  void SetVertexTopologyFilter(MYON_SubDComponentFilter::Topology vertex_topology_filter);

  void ClearVertexTopologyFilter();

  MYON_SubDComponentFilter::Topology VertexTopologyFilter() const;

  void SetEdgeTopologyFilter(MYON_SubDComponentFilter::Topology edge_topology_filter);

  MYON_SubDComponentFilter::Topology EdgeTopologyFilter() const;

  void ClearEdgeTopologyFilter();

  void SetFaceTopologyFilter(MYON_SubDComponentFilter::Topology face_topology_filter);

  MYON_SubDComponentFilter::Topology FaceTopologyFilter() const;

  void ClearFaceTopologyFilter();

  bool AcceptVertexTag(MYON_SubDVertexTag vertex_tag) const;

  void AddAcceptedVertexTag(MYON_SubDVertexTag vertex_tag);

  void ClearVertexTagFilter();

  bool AcceptEdgeTag(MYON_SubDEdgeTag edge_tag) const;

  void AddAcceptedEdgeTag(MYON_SubDEdgeTag edge_tag);

  void ClearEdgeTagFilter();

  bool AcceptFaceEdgeCount(
    unsigned face_edge_count
  ) const;

  void SetFaceEdgeCountFilter(
    unsigned minimum_face_edge_count,
    unsigned maximum_face_edge_count
  );
  
  void ClearFaceEdgeCountFilter();

private:
  bool m_bRejectVertices = false;
  MYON_SubDComponentFilter::Topology m_vertex_topology_filter = MYON_SubDComponentFilter::Topology::Unset;
  MYON_SubDVertexTag m_vertex_tag_filter[4] = {};

  bool m_bRejectEdges = false;
  MYON_SubDComponentFilter::Topology m_edge_topology_filter = MYON_SubDComponentFilter::Topology::Unset;
  MYON_SubDEdgeTag m_edge_tag_filter[2] = {};

  bool m_bRejectFaces = false;
  MYON_SubDComponentFilter::Topology m_face_topology_filter = MYON_SubDComponentFilter::Topology::Unset;
  unsigned m_minimum_face_edge_count = 0U;
  unsigned m_maximum_face_edge_count = 0U;
};





class MYON_SubDRTree : public MYON_RTree
{
private:
  MYON_SubDRTree(const MYON_SubDRTree&) = delete;
  MYON_SubDRTree& operator=(const MYON_SubDRTree&) = delete;

public:
  MYON_SubDRTree() = default;
  ~MYON_SubDRTree() = default;

public:

  bool CreateSubDVertexRTree(
    const MYON_SubD& subd,
    MYON_SubDComponentLocation vertex_location
  );

  bool CreateSubDEmptyRTree(
    const MYON_SubD& subd
  );

  bool AddVertex(
    const MYON_SubDVertex* v,
    MYON_SubDComponentLocation vertex_location
  );

  const MYON_SubDVertex* FindVertexAtPoint(
    const MYON_3dPoint P,
    const double distance_tolerance
  ) const;

  const MYON_SubDVertex* FindMarkedVertexAtPoint(
    const MYON_3dPoint P,
    const double distance_tolerance
  ) const;

  const MYON_SubDVertex* FindUnmarkedVertexAtPoint(
    const MYON_3dPoint P,
    const double distance_tolerance
  ) const;

  const MYON_SubDVertex* FindVertex(
    const class MYON_SubDRTreeVertexFinder& vertex_finder,
    const double distance_tolerance
  ) const;

  const MYON_SubD& SubD() const;

  /*
  Description:
    CLears the reference to the SubD and removes all RTree nodes.
  */
  void Clear();

private:
  // Shares contents with the referenced subd.
  // Used to increment the reference count on the MYON_SubDimple (not a real copy). 
  // This is used to insure the vertex pointers in the rtree nodes are valid.
  MYON_SubD m_subd;
};

class MYON_SubDRTreeVertexFinder
{
public:
  MYON_SubDRTreeVertexFinder() = default;
  ~MYON_SubDRTreeVertexFinder() = default;
  MYON_SubDRTreeVertexFinder(const MYON_SubDRTreeVertexFinder&) = default;
  MYON_SubDRTreeVertexFinder& operator=(const MYON_SubDRTreeVertexFinder&) = default;

  static const MYON_SubDRTreeVertexFinder Unset;

public:
  static const MYON_SubDRTreeVertexFinder Create(const MYON_3dPoint P);

  /*
  Parameters:
    bMarkFilter - [in]
      Vertices with Mark = bMarkFilter are eligible to be found.
      Vertices with Mark != bMarkFilter are ignored.
  */
  static const MYON_SubDRTreeVertexFinder Create(const MYON_3dPoint P, bool bMarkFilter);

  enum class MarkBitsFilter : unsigned char
  {
    None = 0,
    Equal = 1,
    NotEqual = 2
  };

  static const MYON_SubDRTreeVertexFinder Create(
    const MYON_3dPoint P, 
    MYON_SubDRTreeVertexFinder::MarkBitsFilter mark_bits_filter,
    MYON__UINT8 mark_bits
  );


public:
  MYON_3dPoint m_P = MYON_3dPoint::NanPoint;
  double m_distance = MYON_DBL_QNAN;
  const MYON_SubDVertex* m_v = nullptr;

  // When m_bMarkFilterEnabled is true, then vertices with Mark() == m_bMarkFilter are eligible
  // to be found and vertices with Mark() != m_bMarkFilter are ignored.
  bool m_bMarkFilterEnabled = false;
  bool m_bMarkFilter = false;

public:
  MYON_SubDRTreeVertexFinder::MarkBitsFilter m_mark_bits_filter = MYON_SubDRTreeVertexFinder::MarkBitsFilter::None;
  MYON__UINT8 m_mark_bits = 0;

private:
  unsigned int m_reserved2 = 0;

public:

  static bool Callback(void* a_context, MYON__INT_PTR a_id);
 
};

#if defined(MYON_COMPILING_OPENNURBS)
/*
The MYON_SubDAsUserData class is used to attach a subd to it proxy mesh
when writing V6 files in commercial rhino.
*/
class MYON_SubDMeshProxyUserData : public MYON_UserData
{
public:
  /*
  Returns:
    A pointer to a mesh that now manages subd.
  */
  static MYON_Mesh* MeshProxyFromSubD(
    const MYON_SubD* subd
  );

  /*
  Returns:
    A pointer to a subd and deletes mesh.
  */
  static MYON_SubD* SubDFromMeshProxy(
    const MYON_Mesh* mesh
  );

  /*
  Returns:
    A pointer to a subd and deletes mesh.
  */
  static bool IsSubDMeshProxy(
    const MYON_Mesh* mesh
  );

  static const MYON_SubDDisplayParameters MeshProxyDisplayParameters();

private:
  MYON_OBJECT_DECLARE(MYON_SubDMeshProxyUserData);

public:
  MYON_SubDMeshProxyUserData();
  ~MYON_SubDMeshProxyUserData();
  MYON_SubDMeshProxyUserData(const MYON_SubDMeshProxyUserData&);
  MYON_SubDMeshProxyUserData& operator=(const MYON_SubDMeshProxyUserData&);
  
private:
  // MYON_Object overrides
  bool Write(MYON_BinaryArchive& archive) const override;
  bool Read(MYON_BinaryArchive& archive) override;
  bool IsValid( class MYON_TextLog* text_log = nullptr ) const override;

  bool ParentMeshValid() const;

private:
  // MYON_UserData overrides
  bool GetDescription(MYON_wString& description) override;
  bool WriteToArchive(
    const class MYON_BinaryArchive& archive,
    const class MYON_Object* parent_object
  ) const override;
 
private:
  // The subd
  mutable MYON_SubD* m_subd = nullptr;

private:
  // information used to see if the parent mesh has been modified.
  mutable unsigned int m_mesh_face_count = 0;
  mutable unsigned int m_mesh_vertex_count = 0;
  mutable MYON_SHA1_Hash m_mesh_face_array_sha1 = MYON_SHA1_Hash::EmptyContentHash;
  mutable MYON_SHA1_Hash m_mesh_vertex_array_sha1 = MYON_SHA1_Hash::EmptyContentHash;

private:
  void Internal_CopyFrom(const MYON_SubDMeshProxyUserData& src);
  void Internal_Destroy();
  static const bool Internal_MeshHasFaces(const MYON_Mesh* mesh);
  static const MYON_SHA1_Hash Internal_FaceSHA1(const MYON_Mesh* mesh);
  static const MYON_SHA1_Hash Internal_VertexSHA1(const MYON_Mesh* mesh);
};
#endif

#endif

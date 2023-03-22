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

#include "opennurbs.h"

#if !defined(MYON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// MYON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, MYON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error MYON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

MYON_Symmetry::Type MYON_Symmetry::SymmetryTypeFromUnsigned(unsigned int symmetry_type_as_unsigned)
{
  switch (symmetry_type_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Symmetry::Type::Unset);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Symmetry::Type::Reflect);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Symmetry::Type::Rotate);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Symmetry::Type::ReflectAndRotate);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Symmetry::Type::Inversion);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Symmetry::Type::Cyclic);
  }

  MYON_ERROR("Invalid type_as_unsigned parameter");
  return MYON_Symmetry::Type::Unset;
}

const MYON_wString MYON_Symmetry::SymmetryTypeToString(MYON_Symmetry::Type symmetry_type)
{
  const wchar_t* s;
  switch (symmetry_type)
  {
  case MYON_Symmetry::Type::Unset:
    s = L"Unset";
    break;
  case MYON_Symmetry::Type::Reflect:
    s = L"Reflect";
    break;
  break;  case MYON_Symmetry::Type::Rotate:
    s = L"Rotate";
    break;
  case MYON_Symmetry::Type::ReflectAndRotate:
    s = L"ReflectAndRotate";
    break;
  case MYON_Symmetry::Type::Inversion:
    s = L"Inversion";
    break;
  case MYON_Symmetry::Type::Cyclic:
    s = L"Cyclic";
    break;
  default:
    s = nullptr;
    break;
  }
  return MYON_wString(s);
}

MYON_Symmetry::Region MYON_Symmetry::SymmetryRegionFromUnsigned(unsigned int region_as_unsigned)
{
  switch (region_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Symmetry::Region::Unset);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Symmetry::Region::AboveReflectionPlane);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Symmetry::Region::OnRotationAxis);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Symmetry::Region::OnReflectionPlane);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Symmetry::Region::CrossesReflectionPlane);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Symmetry::Region::BelowReflectionPlane);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Symmetry::Region::OffRotationAxis);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Symmetry::Region::OnReflectionHalfPlane);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Symmetry::Region::OnSupplementalHalfPlane);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Symmetry::Region::OnRotateAndReflectHalfPlane);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Symmetry::Region::InsideRotateAndReflect);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Symmetry::Region::OutsideRotateAndReflect);
  }

  MYON_ERROR("Invalid region_as_unsigned parameter");
  return MYON_Symmetry::Region::Unset;
}


const MYON_wString SymmetryRegionToString(MYON_Symmetry::Region r)
{
  switch (r)
  {
  case MYON_Symmetry::Region::Unset:
    return MYON_wString(L"Unset");
    break;
  case MYON_Symmetry::Region::AboveReflectionPlane:
    return MYON_wString(L"AboveReflectionPlane");
    break;
  case MYON_Symmetry::Region::OnRotationAxis:
    return MYON_wString(L"OnRotationAxis");
    break;
  case MYON_Symmetry::Region::OffRotationAxis:
    return MYON_wString(L"OffRotationAxis");
    break;
  case MYON_Symmetry::Region::OnReflectionPlane:
    return MYON_wString(L"OnReflectionPlane");
    break;
  case MYON_Symmetry::Region::CrossesReflectionPlane:
    return MYON_wString(L"CrossesReflectionPlane");
    break;
  case MYON_Symmetry::Region::BelowReflectionPlane:
    return MYON_wString(L"BelowReflectionPlane");
    break;
  case MYON_Symmetry::Region::OnReflectionHalfPlane:
    return MYON_wString(L"OnReflectionHalfPlane");
    break;
  case MYON_Symmetry::Region::OnSupplementalHalfPlane:
    return MYON_wString(L"OnSupplementalHalfPlane");
    break;
  case MYON_Symmetry::Region::OnRotateAndReflectHalfPlane:
    return MYON_wString(L"OnRotateAndReflectHalfPlane");
    break;
  case MYON_Symmetry::Region::InsideRotateAndReflect:
    return MYON_wString(L"InsideRotateAndReflect");
    break;
  case MYON_Symmetry::Region::OutsideRotateAndReflect:
    return MYON_wString(L"OutsideRotateAndReflect");
    break;
  }
  return MYON_wString::EmptyString;
}

MYON_Symmetry::Region MYON_Symmetry::PointRegion(MYON_3dPoint point, bool bUseCleanupTolerance) const
{
  for(;;)
  {
    if (false == point.IsValid())
      break;

    const double tol = bUseCleanupTolerance ? CleanupTolerance() : MYON_Symmetry::ZeroTolerance;
    if (false == MYON_IsValid(tol))
      break;

    double h[2];
    switch (m_type)
    {
    case MYON_Symmetry::Type::Reflect:
      h[0] = ReflectionPlane().ValueAt(point);
      if (h[0] < -tol)
        return MYON_Symmetry::Region::BelowReflectionPlane;
      if (h[0] <= tol)
        return MYON_Symmetry::Region::OnReflectionPlane;
      if (h[0] > tol)
        return MYON_Symmetry::Region::AboveReflectionPlane;

      // ... else nans
      break;

    case MYON_Symmetry::Type::Rotate:
    {
      const MYON_Line axis = RotationAxis();
      const MYON_3dPoint A = axis.ClosestPointTo(point);
      const double d = fabs((point - A).MaximumCoordinate());
      if (d <= tol)
        return MYON_Symmetry::Region::OnRotationAxis;
      if (d > tol)
        return MYON_Symmetry::Region::OffRotationAxis;

      // ... else nans
    }
    break;

    case MYON_Symmetry::Type::ReflectAndRotate:
    {
      const MYON_Line axis = RotationAxis();
      const MYON_3dPoint A = axis.ClosestPointTo(point);
      const double d = fabs((point - A).MaximumCoordinate());
      if (d <= tol)
        return MYON_Symmetry::Region::OnRotationAxis;

      const MYON_PlaneEquation M = ReflectAndRotatePlane(0); // reflection plane
      const MYON_PlaneEquation S = ReflectAndRotatePlane(1); // supplental plane
      h[0] = M.ValueAt(point);
      h[1] = S.ValueAt(point);
      if (h[0] > tol && h[1] > tol)
        return MYON_Symmetry::Region::InsideRotateAndReflect;
      if (fabs(h[0]) <= tol && fabs(h[0]) <= fabs(h[1]))
      {
        return (h[1] >= 0.0) ? MYON_Symmetry::Region::OnReflectionHalfPlane : MYON_Symmetry::Region::OnRotateAndReflectHalfPlane;
      }
      if (fabs(h[1]) <= tol)
      {
        return (h[0] >= 0.0) ? MYON_Symmetry::Region::OnSupplementalHalfPlane : MYON_Symmetry::Region::OnRotateAndReflectHalfPlane;
      }

      if (h[0] == h[0] && h[1] == h[1])
      {
        const MYON_Xform R = this->RotationTransformation();
        const unsigned Rcount = this->RotationCount();
        for (unsigned i = 0; i < Rcount; ++i)
        {
          point = R * point;
          h[0] = S.ValueAt(point);
          if (fabs(h[0]) <= tol)
            return MYON_Symmetry::Region::OnRotateAndReflectHalfPlane;
          h[0] = M.ValueAt(point);
          if (fabs(h[0]) <= tol)
            return MYON_Symmetry::Region::OnRotateAndReflectHalfPlane;
        }
        return MYON_Symmetry::Region::OutsideRotateAndReflect;
      }
      

      // ... else nans
    }
    break;

    default:
      break;
    }
  }

  // When the point is not valid, the symmetry is not set, or an evaluaton produces nans, then return MYON_Symmetry::Region::Unset.
  return MYON_Symmetry::Region::Unset; 
}

bool MYON_Symmetry::SymmetryRegionHasFixedPoints(MYON_Symmetry::Region symmetry_region)
{
  return
    MYON_Symmetry::Region::OnRotationAxis == symmetry_region
    || MYON_Symmetry::Region::OnReflectionPlane == symmetry_region
    || MYON_Symmetry::Region::OnReflectionHalfPlane == symmetry_region
    || MYON_Symmetry::Region::OnSupplementalHalfPlane == symmetry_region
    || MYON_Symmetry::Region::OnRotateAndReflectHalfPlane == symmetry_region
    ;
}

MYON_Symmetry::Coordinates MYON_Symmetry::SymmetryCoordinatesFromUnsigned(unsigned int symmetry_coordinates_as_unsigned)
{
  switch (symmetry_coordinates_as_unsigned)
  {
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Symmetry::Coordinates::Unset);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Symmetry::Coordinates::Object);
    MYON_ENUM_FROM_UNSIGNED_CASE(MYON_Symmetry::Coordinates::World);
  }

  MYON_ERROR("Invalid symmetry_coordinates_as_unsigned parameter");
  return MYON_Symmetry::Coordinates::Unset;
}

const MYON_wString MYON_Symmetry::SymmetryCoordinatesToString(MYON_Symmetry::Coordinates symmetry_coordinates)
{
  const wchar_t* s;
  switch (symmetry_coordinates)
  {
  case MYON_Symmetry::Coordinates::Unset:
    s = L"Unset";
    break;
  case MYON_Symmetry::Coordinates::Object:
    s = L"Object";
    break;
  case MYON_Symmetry::Coordinates::World:
    s = L"World";
    break;
  default:
    s = nullptr;
    break;
  }
  return MYON_wString(s);
}

bool MYON_Symmetry::Write(MYON_BinaryArchive& archive) const
{
  if (false == archive.BeginWrite3dmAnonymousChunk(4))
    return false;

  bool rc = false;
  for (;;)
  {
    const MYON_Symmetry::Type symmetry_type = IsSet() ? SymmetryType() : MYON_Symmetry::Type::Unset;
    const unsigned char utype = static_cast<unsigned char>(symmetry_type);
    if (false == archive.WriteChar(utype))
      break;
    if (MYON_Symmetry::Type::Unset ==  symmetry_type)
    {
      rc = true;
      break;
    }
    if (false == archive.WriteInt(m_inversion_order))
      break;
    if (false == archive.WriteInt(m_cyclic_order))
      break;
    if (false == archive.WriteUuid(m_id))
      break;

    if (archive.BeginWrite3dmAnonymousChunk(3))
    {
      switch (m_type)
      {
      case MYON_Symmetry::Type::Unset:
        break;
      case MYON_Symmetry::Type::Reflect:
        rc = archive.WritePlaneEquation(m_fixed_plane);
        break;
      case MYON_Symmetry::Type::Rotate:
        // The plane is written but not read so the files saved afeter June 15, 2021 
        // can be read by code compiled before June 15, 2021.
        rc = archive.WriteLine(m_rotation_axis) && archive.WritePlaneEquation(MYON_PlaneEquation::NanPlaneEquation);
        break;
      case MYON_Symmetry::Type::ReflectAndRotate:
        rc = archive.WritePlaneEquation(m_fixed_plane) && archive.WriteLine(m_rotation_axis);
        break;
      case MYON_Symmetry::Type::Inversion:
        // fixed plane added for chunk version >= 2
        rc = archive.WriteXform(m_inversion_transform) && archive.WritePlaneEquation(m_fixed_plane);
        break;
      case MYON_Symmetry::Type::Cyclic:
        // fixed plane added for chunk version >= 2
        rc = archive.WriteXform(m_cyclic_transform) && archive.WritePlaneEquation(m_fixed_plane);
        break;
      default:
        MYON_ERROR("You added a new enum value but failed to update archive IO code.");
        break;
      }
      if (false == archive.EndWrite3dmChunk())
        rc = false;
    }

    // MYON_Symmetry::Coordinates added Dec 16, 2019 chunk version 2
    const MYON_Symmetry::Coordinates symmetry_coordinates = IsSet() ? SymmetryCoordinates() : MYON_Symmetry::Coordinates::Unset;
    const unsigned char ucoordinates = static_cast<unsigned char>(symmetry_coordinates);
    if (false == archive.WriteChar(ucoordinates))
      break;

    // m_symmetric_object_content_serial_number added Feb 11, 2020 chunk version 3
    if ( false == archive.WriteBigInt(this->m_symmetric_object_content_serial_number) )
      break;

    // m_symmetric_object_topology_hash added March 29, 2021 chunk version 4
    if (false == this->m_symmetric_object_topology_hash.Write(archive))
      break;
    // m_symmetric_object_geometry_hash added March 29, 2021 chunk version 4
    if (false == this->m_symmetric_object_geometry_hash.Write(archive))
      break;

    rc = true;
    break;
  }
  if (false == archive.EndWrite3dmChunk())
    rc = false;
  return rc;
}

bool MYON_Symmetry::IsValidCyclicTranformation(
  MYON_Xform transformation,
  unsigned transformation_order
)
{
  if (transformation_order < 2)
    return false;
  if (transformation_order > MYON_Symmetry::MaximumOrder)
    return false;
  if (false == transformation.IsValid())
    return false;

  // In the comments below, 
  //   I = identity transformation,
  //   n = transformation_order, 
  //   T = transformation.
  // 
  // 
  // Verify I = T^n, I != T^i when i<n, and calculate some fixed points F[].
  MYON_Xform x = transformation;
  MYON_Xform y = transformation;
  MYON_Xform A = MYON_Xform::IdentityTransformation;
  MYON_Xform B = MYON_Xform::IdentityTransformation;
  for (unsigned j = 1; j < transformation_order; ++j)
  {
    // Here, x = T^j, y = T^j and 1 <= j < n
    if (x.IsIdentity(MYON_Symmetry::ZeroTolerance))
      return false; // transformation_order is too small or transformation is unstable
    if (y.IsIdentity(MYON_Symmetry::ZeroTolerance))
      return false; // transformation_order is too small or transformation is unstable
    A = A + x;
    B = B + y;
    x = x * transformation; // x = transformation^(j+1)
    y = transformation * y; // y = transformation^(j+1)
  }
  if (false == x.IsIdentity(MYON_Symmetry::ZeroTolerance))
    return false; // transformation_order is too large or transformation is unstable
  if (false == y.IsIdentity(MYON_Symmetry::ZeroTolerance))
    return false; // transformation_order is too large or transformation is unstable

  const MYON_Xform ZA = A - A * transformation;
  if (false == ZA.IsZero4x4(MYON_Symmetry::ZeroTolerance))
    return false;

  const MYON_Xform ZB = B - transformation * B;
  if (false == ZB.IsZero4x4(MYON_Symmetry::ZeroTolerance))
    return false;

  return true;
}

bool MYON_Symmetry::IsValidFixedPlane(MYON_Xform transformation, MYON_PlaneEquation fixed_plane)
{
  if (false == fixed_plane.IsSet())
    return false;

  if (false == transformation.IsValid())
    return false;

  // Get a scattering of points on the plane.
  const MYON_3dPoint P[]{
    fixed_plane.ClosestPointTo(MYON_3dPoint(0,0,0)),
    fixed_plane.ClosestPointTo(MYON_3dPoint(1,0,0)),
    fixed_plane.ClosestPointTo(MYON_3dPoint(0,1,0)),
    fixed_plane.ClosestPointTo(MYON_3dPoint(0,0,1)),
    fixed_plane.ClosestPointTo(MYON_3dPoint(1,1,0)),
    fixed_plane.ClosestPointTo(MYON_3dPoint(0,1,1)),
    fixed_plane.ClosestPointTo(MYON_3dPoint(1,0,1)),
    fixed_plane.ClosestPointTo(MYON_3dPoint(1,1,1))
  };
  const size_t Pcount = sizeof(P) / sizeof(P[0]);

  // Test both fixed_plane and a unitized version
  const MYON_PlaneEquation e = fixed_plane.UnitizedPlaneEquation();
  double maxd = 0.0;
  for (size_t i = 0; i < Pcount && maxd <= MYON_Symmetry::ZeroTolerance; ++i)
  {
    const double d0 = fabs(fixed_plane.ValueAt(P[i]));
    if (d0 > maxd)
      maxd = d0;
    else if (d0 != d0)
      maxd = MYON_DBL_QNAN;

    const double d1 = fabs(e.ValueAt(P[i]));
    if (d1 > maxd)
      maxd = d1;
    else if (d1 != d1)
      maxd = MYON_DBL_QNAN;

    const MYON_3dPoint Q = transformation * P[i];
    const double d2 = (P[i] - Q).MaximumCoordinate();
    if (d2 > maxd)
      maxd = d2;
    else if (d2 != d2)
      maxd = MYON_DBL_QNAN;
  }

  return maxd <= MYON_Symmetry::ZeroTolerance;

}

bool MYON_Symmetry::IsValidReflectionTranformationAndFixedPlane(MYON_Xform reflection, MYON_PlaneEquation reflection_plane)
{
  return MYON_Symmetry::IsValidCyclicTranformation(reflection, 2) && MYON_Symmetry::IsValidFixedPlane(reflection, reflection_plane);
}

bool MYON_Symmetry::IsMotifBoundarySubDVertex(const class MYON_SubDVertex* v, bool bUseCleanupTolerance) const
{
  if (nullptr == v)
    return false;
  if (false == v->IsCreaseOrCorner())
    return false;

  const MYON_Symmetry::Type symmetry_type = this->SymmetryType();
  if (false == v->HasBoundaryVertexTopology())
  {
    switch (symmetry_type)
    {
    case MYON_Symmetry::Type::Reflect:
      return false; // easy case.
      break;

    case MYON_Symmetry::Type::Rotate:
      if (v->HasInteriorVertexTopology())
        return false;
      if (MYON_SubDVertexTag::Corner != v->m_vertex_tag)
        return false;
      // We have to keep nonmanifold corners - RH-63789
      break;

    case MYON_Symmetry::Type::ReflectAndRotate:
      return false; 

    default:
      return false;
      break;
    }
  }

  const MYON_3dPoint P = v->ControlNetPoint();
  const double tol = bUseCleanupTolerance ? this->CleanupTolerance() : MYON_Symmetry::ZeroTolerance;
  
  switch (symmetry_type)
  {
  case MYON_Symmetry::Type::Reflect:
  {
    const double d = fabs(this->ReflectionPlane().ValueAt(P));
    return (d <= tol);
  }
  break;

  case MYON_Symmetry::Type::Rotate:
    // All boundary vertices must be eligable for joining.
    // The pinwheel motifs in RH-63376 shows why.
    return true;
    break;

  case MYON_Symmetry::Type::ReflectAndRotate:
    // The ReflectAndRotatePlane(2) test makes sure we are in the correct half space
    // where the ReflectAndRotate reflection plane is active.
    if ( this->ReflectAndRotatePlane(2).ValueAt(P) >= -tol )
    {
      // The d0 test is correct for certain
      const double d0 = fabs(this->ReflectAndRotatePlane(0).ValueAt(P));
      if (d0 <= tol)
        return true;

      // The d1 test is probably correct
      const double d1 = fabs(this->ReflectAndRotatePlane(1).ValueAt(P));
      if (d1 <= tol)
        return true;
    }
    break;

  default:
    break;
  }

  return false;
}

bool MYON_Symmetry::IsFixedSubDComponent(
  const class MYON_SubDComponentPtr& subd_component,
  bool bUseCleanupTolerance
) const
{
  const unsigned motif_count = this->MotifCount();
  if (motif_count < 2 || motif_count > MYON_Symmetry::MaximumOrder)
    return false;

  const MYON_SubDVertex* v = nullptr;
  const MYON_SubDEdge* e = nullptr;
  const MYON_SubDFace* f = nullptr;
  MYON_3dPoint C = MYON_3dPoint::NanPoint;

  switch (subd_component.ComponentType())
  {
  case MYON_SubDComponentPtr::Type::Vertex:
    v = subd_component.Vertex();
    if (nullptr != v)
      C = v->ControlNetPoint();
    break;

  case MYON_SubDComponentPtr::Type::Edge:
    e = subd_component.Edge();
    if (nullptr != e)
      C = e->ControlNetCenterPoint();
    break;

  case MYON_SubDComponentPtr::Type::Face:
    f = subd_component.Face();
    if (nullptr != f)
      C = f->ControlNetCenterPoint();
    break;

  default:
    break;
  }

  if (false == C.IsValid())
    return false;

  const double dtol = bUseCleanupTolerance ? this->CleanupTolerance() : MYON_Symmetry::ZeroTolerance;
  // quick test of center point
  double d = C.DistanceTo(this->MotifTransformation(1) * C);
  if (false == (d <= dtol))
    return false;

  MYON_SimpleArray<MYON_3dPoint> P(nullptr != f ? f->EdgeCount() : 2U);
  if (nullptr != f)
  {
    for (unsigned short fvi = 0; fvi < f->m_edge_count; ++fvi)
      P.Append(f->ControlNetPoint(fvi));
  }
  else if (nullptr != e)
  {
    P.Append(e->ControlNetPoint(0));
    P.Append(e->ControlNetPoint(1));
  }
  else if (nullptr != v)
  {
    P.Append(C);
  }

  const unsigned Pcount = P.UnsignedCount();
  MYON_SimpleArray<bool> hit_array(Pcount);
  hit_array.SetCount(Pcount);
  bool* bHit = hit_array.Array();

  for (unsigned j = 1; j < motif_count; ++j)
  {
    const MYON_Xform& motif_xform = this->MotifTransformation(j);
    hit_array.Zero();
    unsigned hit_count = 0;
    for (unsigned i = 0; i < Pcount; ++i)
    {
      const MYON_3dPoint Q = motif_xform * P[i];
      for (unsigned k = 0; k < Pcount; ++k)
      {
        if (false == bHit[k])
        {
          d = Q.DistanceTo(P[k]);
          if (d <= dtol)
          {
            bHit[k] = true;
            ++hit_count;
          }
        }
      }
    }
    if (hit_count != Pcount)
      return false;
  }

  return true;
}

bool MYON_Symmetry::IsValidRotationAxis(
  MYON_Line rotation_axis,
  unsigned int rotation_count
)
{
  for (;;)
  {
    if (rotation_count < 2)
      break;
    if (rotation_count > MYON_Symmetry::MaximumOrder)
      break;
    if (false == rotation_axis.IsValid())
      break;
    if (false == (rotation_axis.Length() > MYON_Symmetry::ZeroTolerance))
      break;

    // Make sure a unitized axis can be reliably used for evaluations.
    const MYON_Line unit_axis(rotation_axis.from, rotation_axis.from + rotation_axis.Tangent());
    if (false == unit_axis.IsValid())
      break;
    const double d0 = unit_axis.DistanceTo(rotation_axis.to);
    if (false == (d0 <= MYON_Symmetry::ZeroTolerance))
      break;
    const double d1 = rotation_axis.DistanceTo(unit_axis.to);
    if (false == (d1 <= MYON_Symmetry::ZeroTolerance))
      break;

    return true;
  }

  return false;
}

bool MYON_Symmetry::IsValidRotationAxisAndFixedPlane(
  MYON_Line rotation_axis,
  unsigned int rotation_count,
  MYON_PlaneEquation fixed_plane
)
{
  for (;;)
  {
    if (rotation_count < 2)
      return false;
    if (rotation_count > MYON_Symmetry::MaximumOrder)
      return false;
    if (false == rotation_axis.IsValid())
      break;
    if (false == (rotation_axis.Length() > MYON_Symmetry::ZeroTolerance))
      break;

    if (false == fixed_plane.IsSet())
      break;
    
    // Test both fixed_plane and a unitized version just in case
    // fixed_plane has a very short (x,y,z) part. 
    // The idea of all symmetry validation is to prohibit getting
    // started with any sort of garbage input.
    const MYON_PlaneEquation e = fixed_plane.UnitizedPlaneEquation();
    const double h[] =
    {
      fixed_plane.ValueAt(rotation_axis.from),
      fixed_plane.ValueAt(rotation_axis.to),
      e.ValueAt(rotation_axis.from),
      e.ValueAt(rotation_axis.to)
    };
    
    double maxd = 0.0;
    for (size_t i = 0; i < sizeof(h)/sizeof(h[0]) && maxd <= MYON_Symmetry::ZeroTolerance; ++i)
    {
      const double d = fabs(h[i]);
      if (d > maxd)
        maxd = d;
      else if (d != d)
        maxd = MYON_DBL_QNAN;
    }
    if (maxd <= MYON_Symmetry::ZeroTolerance)
      return true;

    // The rotation axis must lie in the fixed plane.
    break;
  }
  return false;
}




static bool Internal_CreateAndValidateFixedPlane(
  MYON_3dPoint P,
  MYON_3dVector N,
  MYON_PlaneEquation& fixed_plane,
  const double zero_tolerance,
  const size_t Fcount,
  const MYON_3dPoint* F
)
{
  for (;;)
  {
    // create
    if (false == N.IsUnitVector())
      N = N.UnitVector();
    if (false == fixed_plane.Create(P, N))
      break;
    if (false == fixed_plane.IsValid())
      break;

    // validate
    double d = 0.0;
    for (size_t i = 0; i < Fcount && d <= zero_tolerance; ++i)
      d = fabs(fixed_plane.ValueAt(F[i]));

    if (d <= zero_tolerance)
      return true; // no nans and all F[] close enough to fixed_plane.

    break;
  }

  // P and N do not define a valid fixed plane
  fixed_plane = MYON_PlaneEquation::NanPlaneEquation;
  return false;
}

static bool Internal_InventSymmetryFixedPlane(const double zero_tolerance, MYON_Xform xform, unsigned xform_order, MYON_PlaneEquation& fixed_plane)
{
  for(;;)
  {
    if (xform_order < 2)
      break;
    if (false == xform.IsNotIdentity())
      break;

    // Set F[] = some fixed points of the xform.
    MYON_3dPoint P[] = { MYON_3dPoint(0,0,0), MYON_3dPoint(1,0,0), MYON_3dPoint(0,1,0), MYON_3dPoint(0,0,1) };
    MYON_3dPoint F[sizeof(P) / sizeof(P[0])];
    const size_t Pcount = sizeof(P) / sizeof(P[0]);
    for (size_t i = 0; i < Pcount; ++i)
      F[i] = P[i];
    MYON_Xform x(xform);
    for (unsigned j = 1; j < xform_order; ++j)
    {
      for (size_t i = 0; i < Pcount; ++i)
        F[i] += x * P[i];
      x = x * xform;
    }
    if (false == x.IsIdentity(zero_tolerance))
      break;

    // validate fixed points
    double fdist = 0.0;
    for (size_t i = 0; i < Pcount; ++i)
    {
      F[i] = F[i] / ((double)xform_order);
      double d = F[i].DistanceTo(xform * F[i]);
      if (d > fdist)
        fdist = d;
      else if (d != d)
      {
        fdist = MYON_DBL_QNAN;
        break;
      }
    }
    if (false == (fdist <= zero_tolerance))
      break; // xform doesn't have fixed points.

    size_t i0 = 0;
    size_t j0 = 0;
    double maxd = 0.0;
    for (size_t i = 1; i < Pcount; ++i)
    {
      const double d = F[0].DistanceTo(F[i]);
      if (d >= maxd)
      {
        maxd = d;
        j0 = i0;
        i0 = i;
      }
      else if (maxd != maxd)
      {
        maxd = MYON_DBL_QNAN;
        break;
      }
    }
    if (maxd != maxd)
      break; // nan

    if (maxd <= zero_tolerance)
    {
      // xform has a fixed point (like DiagonalTransformation(neg,neg,neg)).
      if (Internal_CreateAndValidateFixedPlane(F[0], MYON_3dVector::ZAxis, fixed_plane,zero_tolerance,Pcount,F) )
        return true;
      break;
    }

    if (0 == i0)
      break;

    const MYON_Line fixed_line(F[0], F[i0]);
    if (false == fixed_line.IsValid())
      break;
    const MYON_3dVector T = fixed_line.Tangent();

    // fixed_line is in the fixed plane
    maxd = 0.0;
    for (size_t i = 1; i < Pcount; ++i)
    {
      const MYON_3dPoint Q = fixed_line.ClosestPointTo(F[i]);
      const double d = F[i].DistanceTo(Q);
      if (d > maxd)
        maxd = d;
      else if (d != d)
      {
        maxd = MYON_DBL_QNAN;
        break;
      }
    }
    if (maxd != maxd)
      break; // nan


    if (maxd <= zero_tolerance)
    {
      // xform has a fixed line (like a rotation)
      if ( Internal_CreateAndValidateFixedPlane(F[0], T.Perpendicular(MYON_3dVector::NanVector), fixed_plane, zero_tolerance, Pcount, F) )
        return true;
      break;
    }

    if (0 == j0 || i0 == j0)
      break;

    const MYON_3dVector Y = F[j0] - F[0];
    const MYON_3dVector N = MYON_3dVector::CrossProduct(T, Y).UnitVector();
    if (Internal_CreateAndValidateFixedPlane(F[0], N, fixed_plane, zero_tolerance, Pcount, F))
      return true;

    break;
  }

  fixed_plane = MYON_PlaneEquation::NanPlaneEquation;
  return false;
}

////static bool Internal_InventRotationFixedPlane(const double zero_tolerance, const MYON_Line rotation_axis, unsigned cyclic_order, MYON_PlaneEquation& fixed_plane)
////{
////  const MYON_3dPoint F[2] = { rotation_axis.from, rotation_axis.to };
////  const MYON_3dVector N = rotation_axis.Tangent().Perpendicular(MYON_3dVector::NanVector);
////  return Internal_CreateAndValidateFixedPlane(F[0], N, fixed_plane, zero_tolerance, 2, F);
////}

bool MYON_Symmetry::Read(MYON_BinaryArchive& archive)
{
  *this = MYON_Symmetry::Unset;
  int chunk_version = 0;
  if (false == archive.BeginRead3dmAnonymousChunk(&chunk_version))
    return false;

  MYON_Symmetry::Type symmetry_type = MYON_Symmetry::Type::Unset;
  unsigned int inversion_order = 0;
  unsigned int cyclic_order = 0;
  MYON_UUID symmetry_id = MYON_nil_uuid;
  MYON_Xform inversion_transform = MYON_Xform::Nan;
  MYON_Xform cyclic_transform = MYON_Xform::Nan;
  MYON_PlaneEquation fixed_plane = MYON_PlaneEquation::NanPlaneEquation;
  MYON_Line rotation_axis = MYON_Line::NanLine;

  bool rc = false;
  for (;;)
  {
    if (chunk_version <= 0)
      break;

    unsigned char utype = 0;
    if (false == archive.ReadChar(&utype))
      break;

    bool bNewRotatePrototype = false;
    if (113 == utype)
    {
      // There was a period of time in June 2021 when MYON_Symmetry::Type::NewRotate = 113
      // was used to prototype a replacement for an earlier version of rotate
      // that did not work well.  The protyped replacement worked much better
      // and became the standard rotate on June 15, 2021.
      bNewRotatePrototype = true;
      utype = static_cast<unsigned char>(MYON_Symmetry::Type::Rotate);
    }
    symmetry_type = MYON_Symmetry::SymmetryTypeFromUnsigned(utype);
    if (MYON_Symmetry::Type::Unset == symmetry_type)
    {
      rc = true;
      break;
    }

    if (false == archive.ReadInt(&inversion_order))
      break;
    if (false == archive.ReadInt(&cyclic_order))
      break;
    if (false == archive.ReadUuid(symmetry_id))
      break;

    // Before June 1, 2021: Chunk verson 2 for rotations that had a rotation plane.
    // June 1, 2021: Chunk version 3 for symmetry_type=113 prototyping rotations with no rotation plane
    // Future: Chunk version 4 for final rotations not using a rotation plane.
    int inner_chunk_version = 0;
    if (false == archive.BeginRead3dmAnonymousChunk(&inner_chunk_version))
      break;

    MYON_Symmetry symmetry;

    MYON_Symmetry::Coordinates symmetry_coordinates = MYON_Symmetry::Coordinates::Object;

    switch (symmetry_type)
    {
    case MYON_Symmetry::Type::Unset:
      break;

    case MYON_Symmetry::Type::Reflect:
      rc = archive.ReadPlaneEquation(fixed_plane);
      if (rc)
        symmetry = MYON_Symmetry::CreateReflectSymmetry(fixed_plane, symmetry_coordinates);
      break;

    case MYON_Symmetry::Type::Rotate:
      rc = archive.ReadLine(rotation_axis);
      if (inner_chunk_version >= 2 && false == bNewRotatePrototype)
        rc = archive.ReadPlaneEquation(fixed_plane);
      if (rc)
      {
        // the fixed plane is intentionally ignored.
        symmetry = MYON_Symmetry::CreateRotateSymmetry(rotation_axis, cyclic_order, symmetry_coordinates);
      }
      break;

    case MYON_Symmetry::Type::ReflectAndRotate:
      rc = archive.ReadPlaneEquation(fixed_plane) && archive.ReadLine(rotation_axis);
      if (rc)
        symmetry = MYON_Symmetry::CreateReflectAndRotateSymmetry(fixed_plane, rotation_axis, cyclic_order, symmetry_coordinates);
      break;

    case MYON_Symmetry::Type::Inversion:
      rc = archive.ReadXform(inversion_transform);
      if (inner_chunk_version >= 2)
        rc = archive.ReadPlaneEquation(fixed_plane);
      else
        rc = Internal_InventSymmetryFixedPlane(MYON_Symmetry::ZeroTolerance, inversion_transform, 2, fixed_plane);
      if (rc)
        symmetry = MYON_Symmetry::Internal_CreateInversionSymmetry(symmetry_id, inversion_transform, fixed_plane, symmetry_coordinates);
      break;

    case MYON_Symmetry::Type::Cyclic:
      rc = archive.ReadXform(cyclic_transform);
      if (inner_chunk_version >= 2)
        rc = archive.ReadPlaneEquation(fixed_plane);
      else
        rc = Internal_InventSymmetryFixedPlane(MYON_Symmetry::ZeroTolerance, cyclic_transform, cyclic_order, fixed_plane);
      if (rc)
        symmetry = MYON_Symmetry::Internal_CreateCyclicSymmetry(symmetry_id, cyclic_transform, cyclic_order, fixed_plane, symmetry_coordinates);
      break;

    default:
      // Old code reading a file containing a future type.
      symmetry_type = MYON_Symmetry::Type::Unset;
      rc = true; // means no media reading error
      break;
    }

    if (
      rc
      && MYON_Symmetry::Type::Unset != symmetry_type
      && symmetry.SymmetryType() == symmetry_type
      && symmetry.InversionOrder() == inversion_order
      && symmetry.CyclicOrder() == cyclic_order
      && symmetry.SymmetryId() == symmetry_id
      )
    {
      *this = symmetry;
    }

    if (false == archive.EndRead3dmChunk())
      rc = false;

    if (chunk_version < 2)
      break;

    unsigned char ucoordinates = 0;
    rc = archive.ReadChar(&ucoordinates);
    if (false == rc)
      break;
    symmetry_coordinates = MYON_Symmetry::SymmetryCoordinatesFromUnsigned(ucoordinates);
    if (MYON_Symmetry::Coordinates::Unset != symmetry_coordinates && m_coordinates != symmetry_coordinates)
      m_coordinates = symmetry_coordinates;
    
    if (chunk_version < 3)
      break;

    MYON__UINT64 symmetric_object_content_serial_number = 0;
    rc = archive.ReadBigInt(&symmetric_object_content_serial_number);
    if (rc)
      this->m_symmetric_object_content_serial_number = symmetric_object_content_serial_number;

    if (chunk_version < 4)
      break;

    // m_symmetric_object_topology_hash added March 29, 2021 chunk version 4
    rc = rc && this->m_symmetric_object_topology_hash.Read(archive);

    // m_symmetric_object_geometry_hash added March 29, 2021 chunk version 4
    rc = rc && this->m_symmetric_object_geometry_hash.Read(archive);

    break;
  }

  if (false == archive.EndRead3dmChunk())
    rc = false;

  return rc;
}

void MYON_PlaneEquation::Dump(class MYON_TextLog& text_log) const
{
  // print -0 as 0.
  double c[4] = { (0.0==x) ? 0.0 : x,(0.0 == y) ? 0.0 : y,(0.0 == z) ? 0.0 : z,(0.0 == d) ? 0.0 : d };
  for (int i = 0; i < 3; ++i)
  {
    if (false == (0.0 != c[i] && 0.0 == c[(i + 1) % 3] && 0.0 == c[(i + 2) % 3]) )
      continue;
    const char* coord = (0 == i) ? "x" : ((1 == i) ? "y" : "z");
    if (0.0 == c[3])
      text_log.Print(L"%s = 0", coord);
    else if (1.0 == c[i])
      text_log.Print(L"%s = %g", coord, -c[3]);
    else
      text_log.Print(L"%g*%s = %g", c[i] , coord, -c[3]);
    return;
  }

  // general case
  text_log.Print(L"%g*x + %g*y + %g*z + %g = 0", c[0], c[1], c[2], c[3]);
}

void MYON_Symmetry::Dump(MYON_TextLog& text_log) const
{
  ToText(true, text_log);
}

void MYON_Symmetry::ToText(
  bool bIncludeSymmetricObject,
  class MYON_TextLog& text_log
) const
{
  const MYON_wString type = MYON_Symmetry::SymmetryTypeToString(m_type);
  const MYON_wString coordinates = MYON_Symmetry::SymmetryCoordinatesToString(m_coordinates);
  text_log.Print(L"%ls %ls symmetry\n",static_cast<const wchar_t*>(type), static_cast<const wchar_t*>(coordinates));
  if (IsUnset())
    return;

  text_log.Print(L"Motif count: %u\n", MotifCount());

  switch (m_type)
  {
  case MYON_Symmetry::Type::Unset:
    break;

  case MYON_Symmetry::Type::Reflect:
  {
    text_log.Print(L"plane: ");
    ReflectionPlane().Dump(text_log);
    text_log.PrintNewLine();
  }
  break;

  case MYON_Symmetry::Type::Rotate:
  {
    text_log.Print(L"rotation count: %u (%g degrees)\n", RotationCount(), RotationAngleDegrees());
    const MYON_Line axis = RotationAxis();
    text_log.Print(L"axis: ");
    text_log.Print(axis.from);
    text_log.Print(L", ");
    text_log.Print(axis.to);
    text_log.PrintNewLine();
  }
  break;

  case MYON_Symmetry::Type::ReflectAndRotate:
  {
    text_log.Print(L"plane: ");
    ReflectionPlane().Dump(text_log);
    text_log.PrintNewLine();
    text_log.Print(L"rotation count: %u (%g degrees)\n", RotationCount(), RotationAngleDegrees());
    const MYON_Line axis = RotationAxis();
    text_log.Print(L"axis: ");
    text_log.Print(axis.from);
    text_log.Print(L", ");
    text_log.Print(axis.to);
    text_log.PrintNewLine();
  }
  break;

  case MYON_Symmetry::Type::Inversion:
  {
    const MYON_Line line = RotationAxis();
    text_log.Print(InversionTransformation());
    text_log.PrintNewLine();
  }
  break;

  case MYON_Symmetry::Type::Cyclic:
  {
    const MYON_Line line = RotationAxis();
    text_log.Print(CyclicTransformation());
    text_log.PrintNewLine();
  }
  break;

  default:
    break;
  }

  if (bIncludeSymmetricObject && 0 != m_symmetric_object_content_serial_number)
  {
    text_log.Print("Symmetric object hashes:\n");
    const MYON_TextLogIndent indent1(text_log);
    text_log.Print(L"content serial number: %u\n", m_symmetric_object_content_serial_number);
    text_log.PrintString(MYON_wString(L"topology hash: ") + this->m_symmetric_object_topology_hash.ToStringEx(true));
    text_log.PrintNewLine();
    text_log.PrintString(MYON_wString(L"geometry hash: ") + this->m_symmetric_object_geometry_hash.ToStringEx(true));
    text_log.PrintNewLine();
  }
}

const MYON_Symmetry MYON_Symmetry::TransformConditionally(const MYON_Xform& xform) const
{
  return
    (MYON_Symmetry::Coordinates::Object == SymmetryCoordinates())
    ? MYON_Symmetry::TransformUnconditionally(xform)
    : MYON_Symmetry(*this);
}

const MYON_Symmetry MYON_Symmetry::TransformUnconditionally(const MYON_Xform& xform) const
{
  switch (m_type)
  {
  case MYON_Symmetry::Type::Unset:
    break;

  case MYON_Symmetry::Type::Reflect:
  {
    if (false == m_fixed_plane.IsValid())
      break;
    MYON_PlaneEquation e = m_fixed_plane;
    e.Transform(xform);
    if (false == e.IsValid())
      break;
    return MYON_Symmetry::CreateReflectSymmetry(e, m_coordinates);
  }
  break;


  case MYON_Symmetry::Type::Rotate:
  {
    if (false == m_rotation_axis.IsValid())
      break;
    MYON_Line a = m_rotation_axis;
    a.Transform(xform);
    if (false == a.IsValid())
      break;
    return MYON_Symmetry::CreateRotateSymmetry(a, RotationCount(), m_coordinates);
  }
  break;

  case MYON_Symmetry::Type::ReflectAndRotate:
  {
    if (false == m_fixed_plane.IsValid())
      break;
    if (false == m_rotation_axis.IsValid())
      break;
    MYON_PlaneEquation e = m_fixed_plane;
    e.Transform(xform);
    if (false == e.IsValid())
      break;
    MYON_Line a = m_rotation_axis;
    a.Transform(xform);
    if (false == a.IsValid())
      break;
    return MYON_Symmetry::CreateReflectAndRotateSymmetry(e, a, RotationCount(), m_coordinates);
  }
  break;

  case MYON_Symmetry::Type::Inversion:
  {
    const MYON_Xform xform_inverse = xform.Inverse();
    const MYON_Xform inversion_xform = xform * InversionTransformation()*xform_inverse;
    if (false == m_fixed_plane.IsValid())
      break;
    MYON_PlaneEquation e = m_fixed_plane;
    e.Transform(xform);
    if (false == e.IsValid())
      break;
    return MYON_Symmetry::Internal_CreateInversionSymmetry(SymmetryId(), inversion_xform, e, m_coordinates);
  }
  break;

  case MYON_Symmetry::Type::Cyclic:
  {
    const MYON_Xform xform_inverse = xform.Inverse();
    const MYON_Xform cyclic_xform = xform * CyclicTransformation()*xform_inverse;
    if (false == m_fixed_plane.IsValid())
      break;
    MYON_PlaneEquation e = m_fixed_plane;
    e.Transform(xform);
    if (false == e.IsValid())
      break;
    return MYON_Symmetry::Internal_CreateCyclicSymmetry(SymmetryId(), cyclic_xform, CyclicOrder(), e, m_coordinates);
  }
  break;

  default:
    break;
  }
  return MYON_Symmetry::Unset;
}

static bool Internal_SamePlane(const MYON_Symmetry* lhs, const MYON_Symmetry* rhs, double zero_tolerance)
{
  const MYON_PlaneEquation lhs_e = lhs->ReflectionPlane().UnitizedPlaneEquation();
  const MYON_PlaneEquation rhs_e = rhs->ReflectionPlane().UnitizedPlaneEquation();
  return
    fabs(lhs_e.x - rhs_e.x) <= zero_tolerance
    && fabs(lhs_e.y - rhs_e.y) <= zero_tolerance
    && fabs(lhs_e.z - rhs_e.z) <= zero_tolerance
    && fabs(lhs_e.d - rhs_e.d) <= zero_tolerance
    ;
}

static bool Internal_SameRotation(const MYON_Symmetry* lhs, const MYON_Symmetry* rhs, double zero_tolerance)
{
  const MYON_Line lhs_l = lhs->RotationAxis();
  const MYON_Line rhs_l = rhs->RotationAxis();
  if (
    lhs_l.DistanceTo(rhs_l.from) <= zero_tolerance
    && lhs_l.DistanceTo(rhs_l.to) <= zero_tolerance
    && rhs_l.DistanceTo(lhs_l.from) <= zero_tolerance
    && rhs_l.DistanceTo(lhs_l.to) <= zero_tolerance
    )
  {
    const MYON_3dVector lhs_t = lhs->RotationAxis().Tangent();
    const MYON_3dVector rhs_t = lhs->RotationAxis().Tangent();
    const double lhs_a = lhs->RotationAngleRadians();
    const double rhs_a = ((lhs_t * rhs_t < 0.0) ? -1.0 : 1.0) * rhs->RotationAngleRadians();
    if (fabs(lhs_a - rhs_a) <= zero_tolerance)
    {
      // a point 1 unit from the common axis will rotate within zero tolrance
      return true;
    }
  }
  return false;
}

static bool Internal_SameTransformation(const MYON_Xform lhs_x, const MYON_Xform rhs_x, double zero_tolerance)
{
  return (lhs_x * rhs_x.Inverse()).IsIdentity(zero_tolerance) && (rhs_x * lhs_x.Inverse()).IsIdentity(zero_tolerance);
}

static bool Internal_SameTransformation(const MYON_Symmetry* lhs, const MYON_Symmetry* rhs, double zero_tolerance)
{
  MYON_Xform lhs_x;
  MYON_Xform rhs_x;
  if (lhs->InversionOrder() != rhs->InversionOrder())
    return false;
  if (lhs->CyclicOrder() != rhs->CyclicOrder())
    return false;
  if (lhs->InversionOrder() > 1 && false == Internal_SameTransformation(lhs->InversionTransformation(), rhs->InversionTransformation(), zero_tolerance))
    return false;
  if (lhs->CyclicOrder() > 1 && false == Internal_SameTransformation(lhs->CyclicTransformation(), rhs->CyclicTransformation(), zero_tolerance))
    return false;
  return true;
}


int MYON_Symmetry::CompareSymmetryTransformation(const MYON_Symmetry* lhs, const MYON_Symmetry* rhs, double zero_tolerance)
{
  for (;;)
  {
    const MYON_Symmetry::Type lhs_type = (nullptr != lhs) ? lhs->SymmetryType() : MYON_Symmetry::Type::Unset;
    const MYON_Symmetry::Type rhs_type = (nullptr != rhs) ? rhs->SymmetryType() : MYON_Symmetry::Type::Unset;
    if (lhs_type != rhs_type)
      break;

    if (MYON_Symmetry::Type::Unset == lhs_type)
      return 0; // both are unset

    if (false == (zero_tolerance >= 0.0 && zero_tolerance < MYON_UNSET_POSITIVE_FLOAT))
      zero_tolerance = MYON_Symmetry::ZeroTolerance;

    switch (lhs_type)
    {
    case MYON_Symmetry::Type::Unset:
      break;

    case MYON_Symmetry::Type::Reflect:
      if (Internal_SamePlane(lhs, rhs, zero_tolerance))
        return 0;
      break;

    case MYON_Symmetry::Type::Rotate:
      if (Internal_SameRotation(lhs, rhs, zero_tolerance))
        return 0;
      break;

    case MYON_Symmetry::Type::ReflectAndRotate:
      if (Internal_SamePlane(lhs, rhs, zero_tolerance) && Internal_SameRotation(lhs, rhs, zero_tolerance))
        return 0;
      break;

    case MYON_Symmetry::Type::Inversion:
    case MYON_Symmetry::Type::Cyclic:
      if (Internal_SameTransformation(lhs, rhs, zero_tolerance))
        return 0;

    default:
      break;
    }
  }

  return MYON_Symmetry::Compare(lhs, rhs);
}


const MYON_PlaneEquation MYON_Symmetry::Internal_UnitizePlaneEquationParameter(MYON_PlaneEquation e)
{
  for (;;)
  {
    if (false == e.IsSet())
      break;
    if (e.IsUnitized())
      return e;
    const MYON_PlaneEquation u = e.UnitizedPlaneEquation();
    if (u.IsUnitized())
      return u;
    break;
  }
  return MYON_PlaneEquation::NanPlaneEquation;
}

const MYON_Symmetry MYON_Symmetry::Internal_CreateInversionSymmetry(
  MYON_UUID symmetry_id, 
  MYON_Xform inversion_transform, 
  MYON_PlaneEquation fixed_plane,
  MYON_Symmetry::Coordinates symmetry_coordinates
)
{
  for (;;)
  {
    const double det = inversion_transform.Determinant();
    if (false == (det < 0.0))
      break;
    if (false == MYON_Symmetry::IsValidCyclicTranformation(inversion_transform, 2))
      break;

    if (false == (MYON_nil_uuid == symmetry_id) )
    {
      // prohibit using built-in ids
      if (MYON_Symmetry::ReflectId == symmetry_id)
        break;
      if (MYON_Symmetry::RotateId == symmetry_id)
        break;
      if (MYON_Symmetry::ReflectAndRotateId == symmetry_id)
        break;
    }

    MYON_Symmetry symmetry;
    symmetry.m_type = MYON_Symmetry::Type::Inversion;
    symmetry.m_coordinates = symmetry_coordinates;
    symmetry.m_inversion_order = 2;
    symmetry.m_cyclic_order = 1;
    symmetry.m_id = symmetry_id;
    symmetry.m_inversion_transform = inversion_transform;
    symmetry.m_fixed_plane = fixed_plane;
    symmetry.m_cyclic_transform = MYON_Xform::IdentityTransformation;
    return symmetry;
  }

  return MYON_Symmetry::Unset;
}

const MYON_Symmetry MYON_Symmetry::Internal_CreateCyclicSymmetry(
  MYON_UUID symmetry_id,
  MYON_Xform cyclic_transform,
  unsigned int cyclic_order,
  MYON_Line fixed_line,
  MYON_Symmetry::Coordinates symmetry_coordinates
)
{
  for (;;)
  {
    if (false == fixed_line.IsValid())
      break;

    const MYON_Line unit_axis(fixed_line.from, fixed_line.from + fixed_line.Tangent());
    if (false == unit_axis.IsValid())
      break;

    if (false == MYON_Symmetry::IsValidCyclicTranformation(cyclic_transform, cyclic_order))
      break;

    const double det = cyclic_transform.Determinant();
    if (2 == cyclic_order || 1 == (cyclic_order % 2))
    {
      if (false == (det > 0.0))
        break;
    }
    else
    {
      if (false == (det != 0.0))
        break;
    }

    if (false == (MYON_nil_uuid == symmetry_id))
    {
      // prohibit using built-in ids
      if (MYON_Symmetry::ReflectId == symmetry_id)
        break;
      if (MYON_Symmetry::RotateId == symmetry_id)
        break;
      if (MYON_Symmetry::ReflectAndRotateId == symmetry_id)
        break;
    }


    // Make sure cyclic_transform fixes the axis.
    const MYON_3dPoint P[3] = { fixed_line.from, fixed_line.to, unit_axis.to };
    const unsigned Pcount = (unsigned)(sizeof(P) / sizeof(P[0]));
    MYON_Xform T = cyclic_transform;
    for (unsigned int i = 0; i < cyclic_order; ++i)
    {
      for (unsigned j = 0; j < Pcount; ++j)
      {
        const MYON_3dPoint Q = T * P[j];
        const double d = P[j].DistanceTo(Q);
        if (false == (d <= MYON_Symmetry::ZeroTolerance))
          return MYON_Symmetry::Unset;
      }
      T = T * cyclic_transform;
    }

    MYON_Symmetry symmetry;
    symmetry.m_type = MYON_Symmetry::Type::Cyclic;
    symmetry.m_coordinates = symmetry_coordinates;
    symmetry.m_inversion_order = 1;
    symmetry.m_cyclic_order = cyclic_order;
    symmetry.m_id = symmetry_id;
    symmetry.m_inversion_transform = MYON_Xform::IdentityTransformation;
    symmetry.m_cyclic_transform = cyclic_transform;
    symmetry.m_fixed_plane = MYON_PlaneEquation::NanPlaneEquation;
    return symmetry;
  }

  return MYON_Symmetry::Unset;
}

const MYON_Symmetry MYON_Symmetry::Internal_CreateCyclicSymmetry(
  MYON_UUID symmetry_id, 
  MYON_Xform cyclic_transform,
  unsigned int cyclic_order,
  MYON_PlaneEquation zero_plane,
  MYON_Symmetry::Coordinates symmetry_coordinates
)
{
  for (;;)
  {
    zero_plane = MYON_Symmetry::Internal_UnitizePlaneEquationParameter(zero_plane);
    if (false == zero_plane.IsSet())
      break;

    if (false == MYON_Symmetry::IsValidCyclicTranformation(cyclic_transform, cyclic_order))
      break;

    const double det = cyclic_transform.Determinant();
    if (2 == cyclic_order || 1 == (cyclic_order % 2))
    {
      if (false == (det > 0.0))
        break;
    }
    else
    {
      if (false == (det != 0.0))
        break;
    }

    if (false == (MYON_nil_uuid == symmetry_id))
    {
      // prohibit using built-in ids
      if (MYON_Symmetry::ReflectId == symmetry_id)
        break;
      if (MYON_Symmetry::RotateId == symmetry_id)
        break;
      if (MYON_Symmetry::ReflectAndRotateId == symmetry_id)
        break;
    }

    MYON_Symmetry symmetry;
    symmetry.m_type = MYON_Symmetry::Type::Cyclic;
    symmetry.m_coordinates = symmetry_coordinates;
    symmetry.m_inversion_order = 1;
    symmetry.m_cyclic_order = cyclic_order;
    symmetry.m_id = symmetry_id;
    symmetry.m_inversion_transform = MYON_Xform::IdentityTransformation;
    symmetry.m_cyclic_transform = cyclic_transform;
    symmetry.m_fixed_plane = zero_plane;
    return symmetry;
  }

  return MYON_Symmetry::Unset;
}

const MYON_Symmetry MYON_Symmetry::CreateReflectSymmetry(
  MYON_PlaneEquation reflection_plane, 
  MYON_Symmetry::Coordinates symmetry_coordinates
)
{
  for (;;)
  {
    reflection_plane = MYON_Symmetry::Internal_UnitizePlaneEquationParameter(reflection_plane);
    if (false == reflection_plane.IsSet())
      break;
    MYON_PlaneEquation e = reflection_plane.UnitizedPlaneEquation();
    if (false == e.IsSet())
      break;
    const MYON_Xform xform(MYON_Xform::MirrorTransformation(reflection_plane));
    if (false == MYON_Symmetry::IsValidReflectionTranformationAndFixedPlane(xform, reflection_plane))
      break;
    MYON_Symmetry symmetry = MYON_Symmetry::Internal_CreateInversionSymmetry(MYON_nil_uuid, xform, reflection_plane, symmetry_coordinates);
    if (MYON_Symmetry::Type::Inversion != symmetry.m_type)
      break;
    symmetry.m_type = MYON_Symmetry::Type::Reflect;
    symmetry.m_id = MYON_Symmetry::ReflectId;
    return symmetry;
  }
  return MYON_Symmetry::Unset;
}

const MYON_Symmetry MYON_Symmetry::CreateRotateSymmetry(
  MYON_Line rotation_axis,
  unsigned int rotation_count,
  MYON_Symmetry::Coordinates symmetry_coordinates
)
{
  for (;;)
  {
    if (false == MYON_Symmetry::IsValidRotationAxis(rotation_axis, rotation_count))
      break;
    const MYON_Xform R = Internal_RotationXform(rotation_axis, 1, rotation_count);
    MYON_Symmetry symmetry = MYON_Symmetry::Internal_CreateCyclicSymmetry(MYON_nil_uuid, R, rotation_count, rotation_axis, symmetry_coordinates);
    if (MYON_Symmetry::Type::Cyclic != symmetry.m_type)
      break;
    symmetry.m_type = MYON_Symmetry::Type::Rotate;
    symmetry.m_id = MYON_Symmetry::RotateId;
    symmetry.m_rotation_axis = rotation_axis;
    return symmetry;
  }
  return MYON_Symmetry::Unset;
}

const MYON_Symmetry MYON_Symmetry::CreateReflectAndRotateSymmetry(
  MYON_PlaneEquation reflection_plane,
  MYON_Line rotation_axis, 
  unsigned int rotation_count,
  MYON_Symmetry::Coordinates symmetry_coordinates
)
{
  for (;;)
  {
    reflection_plane = MYON_Symmetry::Internal_UnitizePlaneEquationParameter(reflection_plane);

    const MYON_Symmetry reflection = CreateReflectSymmetry(reflection_plane, symmetry_coordinates);
    if (MYON_Symmetry::Type::Reflect != reflection.SymmetryType())
      break;

    const MYON_Symmetry rotation = CreateRotateSymmetry(rotation_axis,rotation_count, symmetry_coordinates);
    if (MYON_Symmetry::Type::Rotate != rotation.SymmetryType())
      break;

    MYON_Symmetry symmetry;
    symmetry.m_type = MYON_Symmetry::Type::ReflectAndRotate;
    symmetry.m_coordinates = symmetry_coordinates;
    symmetry.m_inversion_order = reflection.m_inversion_order;
    symmetry.m_cyclic_order = rotation.m_cyclic_order;
    symmetry.m_id = MYON_Symmetry::ReflectAndRotateId;
    symmetry.m_inversion_transform = reflection.m_inversion_transform;
    symmetry.m_cyclic_transform = rotation.m_cyclic_transform;
    symmetry.m_fixed_plane = reflection.m_fixed_plane;
    symmetry.m_rotation_axis = rotation.m_rotation_axis;
    return symmetry;
  }
  return MYON_Symmetry::Unset;
}

int MYON_Symmetry::Internal_CompareDouble(const double* lhs, const double* rhs, size_t count)
{
  if (lhs == rhs)
    return 0;
  if (nullptr == lhs)
    return 1;
  if (nullptr == rhs)
    return -1;
  for (size_t i = 0; i < count; ++i)
  {
    const double x = lhs[i];
    const double y = rhs[i];
    if (x < y)
      return -1;
    if (x > y)
      return 1;
    const bool xok = (x == x) ? true : false;
    const bool yok = (y == y) ? true : false;
    if (xok == yok)
      continue;
    if (false == xok)
      return 1; // lhs is a nan
    if (false == yok)
      return -1; // rhs is a nan
  }
  return 0;
}

int MYON_Symmetry::Compare(const MYON_Symmetry* lhs, const MYON_Symmetry* rhs)
{
  if (lhs == rhs)
    return 0;

  // sort nulls to end
  if (nullptr == lhs)
    return 1;
  if (nullptr == rhs)
    return -1;

  if (static_cast<unsigned char>(lhs->m_type) < static_cast<unsigned char>(rhs->m_type))
    return -1;
  if (static_cast<unsigned char>(lhs->m_type) > static_cast<unsigned char>(rhs->m_type))
    return 1;
  if (MYON_Symmetry::Type::Unset == lhs->m_type)
    return 0;

  if (static_cast<unsigned char>(lhs->m_coordinates) < static_cast<unsigned char>(rhs->m_coordinates))
    return -1;
  if (static_cast<unsigned char>(lhs->m_coordinates) > static_cast<unsigned char>(rhs->m_coordinates))
    return 1;


  if (lhs->m_inversion_order < rhs->m_inversion_order)
    return -1;
  if (lhs->m_inversion_order > rhs->m_inversion_order)
    return 1;

  if (lhs->m_cyclic_order < rhs->m_cyclic_order)
    return -1;
  if (lhs->m_cyclic_order > rhs->m_cyclic_order)
    return 1;

  if (0U == lhs->m_inversion_order || 0U == lhs->m_cyclic_order)
    return 0;
   
  if (lhs->Internal_RequiresFixedPlane() || rhs->Internal_RequiresFixedPlane())
  {
    const int rc = MYON_Symmetry::Internal_CompareDouble(&lhs->m_fixed_plane.x, &rhs->m_fixed_plane.x, 4);
    if (0 != rc)
      return rc;
  }

  if (lhs->Internal_RequiresRotationAxis() || rhs->Internal_RequiresRotationAxis())
  {
    const int rc = MYON_Symmetry::Internal_CompareDouble(&lhs->m_rotation_axis.from.x, &rhs->m_rotation_axis.from.x, 6);
    if (0 != rc)
      return rc;
  }

  if (lhs->Internal_IsStandardType() && rhs->Internal_IsStandardType())
    return 0;

  if (lhs->m_inversion_order > 1)
  {
    const int rc = MYON_Symmetry::Internal_CompareDouble(&lhs->m_inversion_transform.m_xform[0][0], &rhs->m_inversion_transform.m_xform[0][0], 16);
    if (0 != rc)
      return rc;
  }

  if (lhs->m_cyclic_order > 1)
  {
    const int rc = MYON_Symmetry::Internal_CompareDouble(&lhs->m_inversion_transform.m_xform[0][0], &rhs->m_inversion_transform.m_xform[0][0], 16);
    if (0 != rc)
      return rc;
  }

  // Do NOT compare the symmetric object settings in this function.

  return 0;
}

MYON_Symmetry::Type MYON_Symmetry::SymmetryType() const
{
  return m_type;
}

bool MYON_Symmetry::IsRotate() const
{
  return MYON_Symmetry::Type::Rotate == m_type;
}

bool MYON_Symmetry::IsReflect() const
{
  return MYON_Symmetry::Type::Reflect == m_type;
}

bool MYON_Symmetry::IsReflectAndRotate() const
{
  return MYON_Symmetry::Type::ReflectAndRotate == m_type;
}

MYON_Symmetry::Coordinates MYON_Symmetry::SymmetryCoordinates() const
{
  return m_coordinates;
}

const MYON_UUID MYON_Symmetry::SymmetryId() const
{
  return m_id;
}

void MYON_Symmetry::Clear()
{
  *this = MYON_Symmetry::Unset;
}

bool MYON_Symmetry::IsSet() const
{
  return 
    MYON_Symmetry::Type::Unset != m_type
    && (1 == m_inversion_order || 2 == m_inversion_order)
    && m_cyclic_order >= 1
    && MotifCount() >= 2
    ;
}

bool MYON_Symmetry::IsUnset() const
{
  return (false == IsSet());
}

unsigned int MYON_Symmetry::MotifCount() const
{
  return InversionOrder()*CyclicOrder();
}


unsigned int MYON_Symmetry::InversionOrder() const
{
  return m_inversion_order;
}


unsigned int MYON_Symmetry::CyclicOrder() const
{
  return m_cyclic_order;
}

const MYON_Xform MYON_Symmetry::InversionTransformation() const
{
  return IsSet() ? m_inversion_transform : MYON_Xform::Nan;
}

const MYON_Xform MYON_Symmetry::CyclicTransformation() const
{
  return IsSet() ? m_cyclic_transform : MYON_Xform::Nan;
}

const MYON_SHA1_Hash MYON_Symmetry::Hash() const
{
  for(;;)
  {
    if (false == IsSet())
      break;
    
    MYON_SHA1 sha1;

    const unsigned char t = static_cast<unsigned char>(m_type);
    sha1.AccumulateBytes(&t, sizeof(t));

    const unsigned char c = static_cast<unsigned char>(m_coordinates);
    sha1.AccumulateBytes(&c, sizeof(c));

    sha1.AccumulateInteger32(InversionOrder());
    sha1.AccumulateInteger32(CyclicOrder());

    if ( this->Internal_RequiresFixedPlane())
      sha1.AccumulateDoubleArray(4, &m_fixed_plane.x);

    if ( this->Internal_RequiresRotationAxis() )
      sha1.AccumulateDoubleArray(6, &m_rotation_axis.from.x);

    if ( false == this->Internal_IsStandardType() )
    {
      if (InversionOrder() > 1)
        sha1.AccumulateDoubleArray(16, &m_inversion_transform.m_xform[0][0]);
      if (CyclicOrder() > 1)
        sha1.AccumulateDoubleArray(16, &m_cyclic_transform.m_xform[0][0]);
    }
    return sha1.Hash();
  }

  return MYON_SHA1_Hash::EmptyContentHash;
}

const MYON_PlaneEquation MYON_Symmetry::ReflectionPlane() const
{
  return (MYON_Symmetry::Type::Reflect == m_type || MYON_Symmetry::Type::ReflectAndRotate == m_type)
    ? FixedPlane()
    : MYON_PlaneEquation::NanPlaneEquation;
}

const MYON_PlaneEquation MYON_Symmetry::ReflectAndRotatePlane(
  unsigned int plane_index
) const
{
  if ( (IsSet() && MYON_Symmetry::Type::ReflectAndRotate == m_type) || (plane_index >= 0 && plane_index <= 2))
  {
    const MYON_PlaneEquation fixed_plane = FixedPlane();
    if (0 == plane_index)
      return fixed_plane;

    // 2nd plane
    MYON_Xform R;
    const double a = (1 == plane_index) ? -0.5 * RotationAngleRadians() : 0.5 * MYON_PI;
    R.Rotation(a, RotationAxisTangent(), RotationAxisPoint());
    MYON_PlaneEquation e = fixed_plane;
    e.Transform(R);
    return (1 == plane_index) ? e.NegatedPlaneEquation() : e;
  }

  return MYON_PlaneEquation::NanPlaneEquation;
}

bool MYON_Symmetry::OnReflectAndRotateFixedPlane( MYON_3dPoint P, bool bUseCleanupTolerance) const
{
  if (MYON_Symmetry::Type::ReflectAndRotate != m_type)
    return false;
  if (false == P.IsValid())
    return false;
  const double tol = this->Tolerance(bUseCleanupTolerance);
  
  const MYON_PlaneEquation M = this->ReflectAndRotatePlane(0);
  double h = M.ValueAt(P);
  if (fabs(h) <= tol)
    return true;
  
  const MYON_PlaneEquation S = this->ReflectAndRotatePlane(1);
  h = S.ValueAt(P);
  if (fabs(h) <= tol)
    return true;

  const unsigned Rcount = this->RotationCount();
  if (Rcount > 2)
  {
    const MYON_Xform R = this->RotationTransformation();
    for (unsigned i = 1; i < Rcount; ++i)
    {
      P = R * P;
      h = S.ValueAt(P);
      if (fabs(h) <= tol)
        return true;
      h = M.ValueAt(P);
      if (fabs(h) <= tol)
        return true;
    }
  }

  return false;
}

const MYON_Xform MYON_Symmetry::ReflectionTransformation() const
{
  return (MYON_Symmetry::Type::Reflect == m_type || MYON_Symmetry::Type::ReflectAndRotate == m_type)
    ? MotifTransformation(1)
    : MYON_Xform::Nan;
}

const MYON_PlaneEquation MYON_Symmetry::FixedPlane() const
{
  return (MYON_Symmetry::Type::Unset != m_type)
    ? m_fixed_plane
    : MYON_PlaneEquation::NanPlaneEquation;
}

bool MYON_Symmetry::Internal_RequiresRotationAxis() const
{
  return (MYON_Symmetry::Type::Rotate == m_type || MYON_Symmetry::Type::ReflectAndRotate == m_type);
}

bool MYON_Symmetry::Internal_RequiresFixedPlane() const
{
  return (MYON_Symmetry::Type::Reflect == m_type || MYON_Symmetry::Type::ReflectAndRotate == m_type);
}

bool MYON_Symmetry::Internal_IsStandardType() const
{
  return (MYON_Symmetry::Type::Reflect == m_type || MYON_Symmetry::Type::Rotate == m_type || MYON_Symmetry::Type::ReflectAndRotate == m_type);
}

const MYON_Line MYON_Symmetry::RotationAxis() const
{
  return (Internal_RequiresRotationAxis())
    ? m_rotation_axis
    : MYON_Line::NanLine;
}

const MYON_3dPoint MYON_Symmetry::RotationAxisPoint() const
{
  return (Internal_RequiresRotationAxis())
    ? m_rotation_axis.from
    : MYON_3dPoint::NanPoint;
}

const MYON_3dVector MYON_Symmetry::RotationAxisDirection() const
{
  return (Internal_RequiresRotationAxis())
    ? m_rotation_axis.Direction()
    : MYON_3dVector::NanVector;
}

const MYON_3dVector MYON_Symmetry::RotationAxisTangent() const
{
  return (Internal_RequiresRotationAxis())
    ? m_rotation_axis.Tangent()
    : MYON_3dVector::NanVector;
}

unsigned int MYON_Symmetry::RotationCount() const
{
  return (Internal_RequiresRotationAxis())
    ? m_cyclic_order
    : 0U;
}

double MYON_Symmetry::RotationAngleDegrees() const
{
  return (Internal_RequiresRotationAxis())
    ? (360.0 / ((double)RotationCount()))
    : MYON_DBL_QNAN;
}

double MYON_Symmetry::RotationAngleRadians() const
{
  return (Internal_RequiresRotationAxis())
    ? ((2.0*MYON_PI) / ((double)RotationCount()))
    : MYON_DBL_QNAN;
}


const MYON_Xform MYON_Symmetry::RotationTransformation() const
{
  if ( MYON_Symmetry::Type::Rotate == m_type )
  {
    return MotifTransformation(1);
  }
  if (MYON_Symmetry::Type::ReflectAndRotate == m_type)
  {
    return MotifTransformation(2);
  }
  return MYON_Xform::Nan;
}

const MYON_Xform MYON_Symmetry::Internal_RotationXform(
  int rotation_index, 
  int rotation_count
) const
{
  if (rotation_index < 0 || rotation_index >= rotation_count)
    return MYON_Xform::Nan;
  if (0 == rotation_index)
    return MYON_Xform::IdentityTransformation;
  if (1 == rotation_index)
    return m_cyclic_transform;

  return MYON_Symmetry::Internal_RotationXform(m_rotation_axis, rotation_index, rotation_count);
}

const MYON_Xform MYON_Symmetry::Internal_RotationXform(
  MYON_Line rotation_axis,
  int rotation_index,
  int rotation_count
)
{
  if (rotation_index < 0 || rotation_index >= rotation_count)
    return MYON_Xform::Nan;
  if (0 == rotation_index)
    return MYON_Xform::IdentityTransformation;

  // calculate from trig functions for maximum precision
  double sin_sign = 1.0;
  if (2 * rotation_index > rotation_count)
  {
    rotation_index = rotation_count - rotation_index;
    sin_sign = -1.0;
  }

  double cos_angle = MYON_DBL_QNAN;
  double sin_angle = MYON_DBL_QNAN;

  if (2 * rotation_index == rotation_count)
  {
    // angle = pi
    sin_angle = 0.0;
    cos_angle = -1.0;
  }
  else if (3 * rotation_index == rotation_count)
  {
    // angle = 2pi/3
    sin_angle = 0.5*sqrt(3.0);
    cos_angle = -0.5;
  }
  else if (4 * rotation_index == rotation_count)
  {
    // angle = pi/2
    sin_angle = 1.0;
    cos_angle = 0.0;
  }
  else if (6 * rotation_index == rotation_count)
  {
    // angle = pi/3
    sin_angle = 0.5*sqrt(3.0);
    cos_angle = 0.5;
  }
  else if (8 * rotation_index == rotation_count)
  {
    // angle = pi/4
    sin_angle = cos_angle = 1.0 / sqrt(2.0);
  }
  else if (12 * rotation_index == rotation_count)
  {
    // angle = pi/3
    sin_angle = 0.5;
    cos_angle = 0.5*sqrt(3.0);
  }
  else
  {
    const double a = (rotation_index*(2.0*MYON_PI)) / ((double)rotation_count);
    sin_angle = sin(a);
    cos_angle = cos(a);
  }

  MYON_Xform r;
  r.Rotation(sin_sign*sin_angle, cos_angle, rotation_axis.Direction(), rotation_axis.from);
  return r;
}

const MYON_Xform MYON_Symmetry::MotifTransformation(
  int index
) const
{
  const int count = MotifCount();
  if ( count <= 1)
    return MYON_Xform::Nan;

  // convert index to be >= 0
  index = ((index % count) + count) % count;
  
  MYON_Xform x = MYON_Xform::Nan;
  switch (m_type)
  {
  case MYON_Symmetry::Type::Unset:
    break;

  case MYON_Symmetry::Type::Reflect:
    x = (0 == index)
      ? MYON_Xform::IdentityTransformation
      : m_inversion_transform;
    break;

  case MYON_Symmetry::Type::Rotate:
    x = Internal_RotationXform(index, count);
    break;

  case MYON_Symmetry::Type::ReflectAndRotate:
    if (0 == index)
      x = MYON_Xform::IdentityTransformation;
    else if (1 == index)
      x = m_inversion_transform;
    else if (2 == index)
      x = m_cyclic_transform;
    else if ( index > 2 )
      x = Internal_ReflectAndRotateTransformation((unsigned)index);
    break;

  case MYON_Symmetry::Type::Inversion:
    x = (0 == index)
      ? MYON_Xform::IdentityTransformation
      : m_inversion_transform;
    break;

  case MYON_Symmetry::Type::Cyclic:
    if (0 == index)
    {
      x = MYON_Xform::IdentityTransformation;
    }
    else if (1 == index)
    {
      x = m_cyclic_transform;
    }
    else if (index >= 2)
    {
      x = m_cyclic_transform * m_cyclic_transform;
      for (int i = 2; i < index; i++)
        x = m_cyclic_transform * x;
    }
    break;

  default:
    break;
  }
  return x;
}

unsigned MYON_Symmetry::GetMotifTransformations(
  bool bIncludeIdentity,
  MYON_SimpleArray<MYON_Xform>& motif_transformations
) const
{
  motif_transformations.SetCount(0);
  const unsigned motif_count = MotifCount();
  if (motif_count <= 0)
    return 0;
  motif_transformations.Reserve(bIncludeIdentity ? motif_count : (motif_count - 1));
  for (unsigned i = bIncludeIdentity ? 0 : 1; i < motif_count; ++i)
  {
    const MYON_Xform x = this->MotifTransformation(i);
    motif_transformations.Append(x);
  }
  return motif_count;
}

const MYON_Xform MYON_Symmetry::Internal_ReflectAndRotateTransformation(unsigned index) const
{
  MYON_Xform r = Internal_RotationXform(index / 2, m_cyclic_order);
  if (1 == index % 2)
    r = r * m_inversion_transform;
  return r;
}

const MYON_SHA1_Hash MYON_Symmetry::SymmetryHash() const
{
  MYON_SHA1 sha1;
  sha1.AccumulateBytes(&m_type, sizeof(m_type));
  sha1.AccumulateBytes(&m_coordinates, sizeof(m_coordinates));
  sha1.AccumulateUnsigned8(m_inversion_order);
  sha1.AccumulateInteger32(m_cyclic_order);
  sha1.AccumulateId(m_id);
  sha1.AccumulateDoubleArray(16, &m_inversion_transform.m_xform[0][0]);
  sha1.AccumulateDoubleArray(16, &m_cyclic_transform.m_xform[0][0]);
  sha1.AccumulateDoubleArray(4,&m_fixed_plane.x);
  sha1.Accumulate3dPoint(m_rotation_axis.from);
  sha1.Accumulate3dPoint(m_rotation_axis.to);
  return sha1.Hash();
}

void MYON_Symmetry::ClearSymmetricObject() const
{
  m_symmetric_object_content_serial_number = 0U;
  m_symmetric_object_geometry_hash = MYON_SHA1_Hash::ZeroDigest;
  m_symmetric_object_topology_hash = MYON_SHA1_Hash::ZeroDigest;
}

void MYON_Symmetry::SetCleanupTolerance(
  double cleanup_tolerance
)
{
  if (cleanup_tolerance > MYON_Symmetry::ZeroTolerance && cleanup_tolerance < MYON_UNSET_POSITIVE_VALUE)
    m_cleanup_tolerance = cleanup_tolerance;
  else
    m_cleanup_tolerance = 0.0; // See MYON_Symmetry::CleanupTolerance().
}


double MYON_Symmetry::CleanupTolerance() const
{
  // The default constructor sets m_cleanup_tolerance = 0.0.
  // Handling m_cleanup_tolerance this way insures that MYON_Symmetry::CleanupTolerance()
  // will always return MYON_Symmetry::ZeroTolerance (which may chnage), even with class definitions 
  // read from old archives.
  return (m_cleanup_tolerance >= MYON_Symmetry::ZeroTolerance) ? m_cleanup_tolerance : MYON_Symmetry::ZeroTolerance;
}

double MYON_Symmetry::Tolerance(bool bUseCleanupTolerance) const
{
  return bUseCleanupTolerance ? CleanupTolerance() : MYON_Symmetry::ZeroTolerance;
}

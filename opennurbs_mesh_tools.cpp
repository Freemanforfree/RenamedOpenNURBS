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


/////////////////////////////////////////////////////////////////////////////
// SwapMeshEdge()
//

bool MYON_Mesh::SwapEdge_Helper( int topei, bool bTestOnly )
{
  MYON_Mesh& mesh = *this;
  const MYON_MeshTopology& top = mesh.Topology();  
  const int F_count = mesh.FaceCount();
  const int V_count = mesh.VertexCount();
  const int topv_count = top.m_topv.Count();
  //const int tope_count = top.m_tope.Count();

  if ( topei < 0 || topei >= top.m_tope.Count() )
  {
    return false;
  }

  if ( top.m_topf.Count() != F_count )
    return false;

  const MYON_MeshTopologyEdge& tope = top.m_tope[topei];

  if (    tope.m_topf_count != 2 
       || tope.m_topvi[0] == tope.m_topvi[1] 
       || tope.m_topvi[0] < 0
       || tope.m_topvi[1] < 0
       || tope.m_topvi[0] >= topv_count
       || tope.m_topvi[1] >= topv_count )
  {
    return false;
  }

  int fi0 = tope.m_topfi[0];
  int fi1 = tope.m_topfi[1];
  if ( fi0 < 0 || fi0 >= F_count || fi1 < 0 || fi1 >= F_count || fi0 == fi1 )
    return false;

  const MYON_MeshFace& f0 = mesh.m_F[fi0];
  const MYON_MeshFace& f1 = mesh.m_F[fi1];
  if ( !f0.IsValid(V_count) )
    return false;
  if ( !f1.IsValid(V_count) )
    return false;
  if ( !f0.IsTriangle() )
    return false;
  if ( !f1.IsTriangle() )
    return false;

  const MYON_MeshTopologyFace& topf0 = top.m_topf[fi0];
  const MYON_MeshTopologyFace& topf1 = top.m_topf[fi1];

  int fei0;
  for ( fei0 = 0; fei0 < 3; fei0++ )
  {
    if ( topf0.m_topei[fei0] == topei )
      break;
  }
  if ( fei0 >= 3 )
    return false;

  int f0_vi0 = f0.vi[(fei0+3)%4];
  int f0_vi1 = f0.vi[fei0];
  int f0_vi2 = f0.vi[(fei0+1)%3];

  int fei1;
  for ( fei1 = 0; fei1 < 3; fei1++ )
  {
    if ( topf1.m_topei[fei1] == topei )
      break;
  }
  if ( fei1 >= 3 )
    return false;

  int f1_vi0 = f1.vi[(fei1+3)%4];
  int f1_vi1 = f1.vi[fei1];
  int f1_vi2 = f1.vi[(fei1+1)%3];

  if ( topf0.m_reve[fei0] == topf1.m_reve[fei1] )
    return false;
  if ( f0_vi0 != f1_vi1 )
    return false;
  if ( f0_vi1 != f1_vi0 )
    return false;

  const MYON_MeshTopologyVertex& topv0 = top.m_topv[tope.m_topvi[0]];
  const MYON_MeshTopologyVertex& topv1 = top.m_topv[tope.m_topvi[1]];
  if ( topv0.m_v_count < 1 || topv1.m_v_count < 1 )
  {
    return false;
  }
  if ( topv0.m_vi[0] < 0 || topv0.m_vi[0] >= V_count )
  {
    return false;
  }
  if ( topv1.m_vi[0] < 0 || topv1.m_vi[0] >= V_count )
  {
    return false;
  }

  if ( bTestOnly )
    return true;


  MYON_MeshFace newf0;
  MYON_MeshFace newf1;
  newf0.vi[0] = f0_vi1;
  newf0.vi[1] = f0_vi2;
  newf0.vi[2] = f1_vi2;
  newf0.vi[3] = newf0.vi[2];

  newf1.vi[0] = f1_vi1;
  newf1.vi[1] = f1_vi2;
  newf1.vi[2] = f0_vi2;
  newf1.vi[3] = newf1.vi[2];

  mesh.m_F[fi0] = newf0;
  mesh.m_F[fi1] = newf1;

  mesh.DestroyTopology();
  mesh.DestroyPartition();

  return true;
}

bool MYON_Mesh::IsSwappableEdge(int topei )
{
  return const_cast<MYON_Mesh*>(this)->SwapEdge_Helper( topei, true );
}

bool MYON_Mesh::SwapEdge( int topei )
{
  return SwapEdge_Helper( topei, false );
}

/////////////////////////////////////////////////////////////////////////////
// CollapseMeshEdge()
//

// DO NOT PUT THIS CLASS IN A HEADER FILE
class MYON__MESHEDGE
{
public:
  // MYON_Mesh m_V[] indices
  int vi0;
  int vi1; // always > vi0
  
  // MYON_MeshTopology m_topvi[] indices
  int topvi0;
  int topvi1;

  // MYON_Mesh m_V[] index of new vertex
  int newvi;

  // location of vertex that will replace the edge
  MYON_3dPoint  newV;
  MYON_3fVector newN;
  MYON_2fPoint  newT;
};

// DO NOT PUT THIS CLASS IN A HEADER FILE
class MYON__NEWVI
{
public:
  // MYON_Mesh m_V[] indices
  int oldvi;
  int newvi;
};

typedef int (*QSORTCMPFUNC)(const void*,const void*);

static int CompareMESHEDGE( const MYON__MESHEDGE* a, const MYON__MESHEDGE* b )
{
  // sort based on "vi0" and vi1" values (vi0 is always < vi1)
  int i = (a->vi0 - b->vi0);
  if ( 0 == i )
  {
    i = a->vi1 - b->vi1;
  }
  return i;
}

static int CompareNEWVI( const MYON__NEWVI* a, const MYON__NEWVI* b )
{
  // sort based on "oldvi" value
  return a->oldvi - b->oldvi;
}

static int CompareInt( const void* a, const void* b )
{
  return ( *((int*)a) - *((int*)b) );
}

static void RemoveFaceOrVertexFromNgon(MYON_Mesh& mesh, const unsigned int faceidx, const int* vertidx = nullptr)
{
  unsigned int NgonIdx = mesh.NgonIndexFromFaceIndex(faceidx);
  const MYON_MeshNgon* pOldNgon = nullptr;
  pOldNgon = mesh.Ngon(NgonIdx);

  if (nullptr != pOldNgon)
  {
    // Make a copy of the ngon we need to update.
    MYON_MeshNgon NewNgon;
    NewNgon.m_Vcount = pOldNgon->m_Vcount;
    NewNgon.m_Fcount = pOldNgon->m_Fcount;

    NewNgon.m_vi = (unsigned int*)onmalloc((NewNgon.m_Vcount+ NewNgon.m_Fcount)*sizeof(unsigned int));
    memcpy(NewNgon.m_vi, pOldNgon->m_vi, NewNgon.m_Vcount*sizeof(unsigned int));
      
    NewNgon.m_fi = &NewNgon.m_vi[NewNgon.m_Vcount];
    memcpy(NewNgon.m_fi, pOldNgon->m_fi, NewNgon.m_Fcount*sizeof(unsigned int));

    if (nullptr != NewNgon.m_vi && nullptr != NewNgon.m_fi)
    {
      unsigned int ct = 0;
      if (nullptr == vertidx)
      {
        while (NewNgon.m_Fcount > ct)
        {
          // find the face index in the ngon's list of faces
          if (faceidx == NewNgon.m_fi[ct])
            break;
          ct++;
        }

        if (NewNgon.m_Fcount > ct)
        {
          // if ct isn't bogus and is not at the end already, shift the indexes
          if (NewNgon.m_Fcount - 1 > ct)
            memcpy(&NewNgon.m_fi[ct], &NewNgon.m_fi[ct + 1], (NewNgon.m_Fcount - (ct + 1))*sizeof(NewNgon.m_fi[0]));

          NewNgon.m_Fcount--;
        }
      }
      else
      {
        unsigned int  idx = MYON_UNSET_UINT_INDEX;
        while (NewNgon.m_Vcount > ct)
        {
          idx = *vertidx;
          // find the vertex index in the ngon's list of vertexes
          if (idx == NewNgon.m_vi[ct])
            break;
          ct++;
        }

        if (NewNgon.m_Vcount > ct)
        {
          // if vi_idx isn't bogus and is not at the end already, shift the indexes
          if (NewNgon.m_Vcount - 1 > ct)
            memcpy(&NewNgon.m_vi[ct], &NewNgon.m_vi[ct + 1], (NewNgon.m_Vcount - (ct + 1))*sizeof(NewNgon.m_vi[0]));

          NewNgon.m_Vcount--;
        }
      }

      mesh.ModifyNgon(NgonIdx, &NewNgon);
    }

    if (nullptr != NewNgon.m_vi)
      onfree(NewNgon.m_vi);
  }
}

bool MYON_Mesh::CollapseEdge( int topei )
{
  MYON_Mesh& mesh = *this;

  MYON__MESHEDGE me;
  memset(&me,0,sizeof(me));
  const MYON_MeshTopology& top = mesh.Topology();  
  const int F_count = mesh.FaceCount();
  const int V_count = mesh.VertexCount();
  const int topv_count = top.m_topv.Count();
  //const int tope_count = top.m_tope.Count();

  if ( topei < 0 || topei >= top.m_tope.Count() )
  {
    return false;
  }

  const MYON_MeshTopologyEdge& tope = top.m_tope[topei];

  if (    tope.m_topf_count < 1 
       || tope.m_topvi[0] == tope.m_topvi[1] 
       || tope.m_topvi[0] < 0
       || tope.m_topvi[1] < 0
       || tope.m_topvi[0] >= topv_count
       || tope.m_topvi[1] >= topv_count )
  {
    return false;
  }

  const MYON_MeshTopologyVertex& topv0 = top.m_topv[tope.m_topvi[0]];
  const MYON_MeshTopologyVertex& topv1 = top.m_topv[tope.m_topvi[1]];
  if ( topv0.m_v_count < 1 || topv1.m_v_count < 1 )
  {
    return false;
  }
  if ( topv0.m_vi[0] < 0 || topv0.m_vi[0] >= V_count )
  {
    return false;
  }
  if ( topv1.m_vi[0] < 0 || topv1.m_vi[0] >= V_count )
  {
    return false;
  }
  
  // create a MYON__MESHEDGE for each face (usually one or two) that uses the edge
  MYON__MESHEDGE* me_list = (MYON__MESHEDGE*)alloca(tope.m_topf_count*sizeof(me_list[0]));
  int me_list_count = 0;
  int efi;
  for ( efi = 0; efi < tope.m_topf_count; efi++ )
  {
    int fi = tope.m_topfi[efi];
    if ( fi < 0 || fi >= F_count )
      continue;
    const MYON_MeshFace& f = mesh.m_F[fi];
    if ( !f.IsValid(V_count) )
      continue;
    me.vi1 = f.vi[3];
    me.topvi1 = top.m_topv_map[me.vi1];
    int fvi;
    for ( fvi = 0; fvi < 4; fvi++ )
    {
      me.vi0 = me.vi1;
      me.topvi0 = me.topvi1;
      me.vi1 = f.vi[fvi];
      me.topvi1 = top.m_topv_map[me.vi1];
      if ( me.vi0 != me.vi1 )
      {
        if (    (me.topvi0 == tope.m_topvi[0] && me.topvi1 == tope.m_topvi[1])
             || (me.topvi0 == tope.m_topvi[1] && me.topvi1 == tope.m_topvi[0]) )
        {
          if ( me.vi0 > me.vi1 )
          {
            int i = me.vi0; me.vi0 = me.vi1; me.vi1 = i;
            i = me.topvi0; me.topvi0 = me.topvi1; me.topvi1 = i;
          }
          me_list[me_list_count++] = me;
          break;
        }
      }
    }
  }

  if (me_list_count<1)
  {
    return false;
  }

  // Sort me_list[] so edges using same vertices are adjacent
  // to each other in the list.  This is needed so that non-manifold
  // crease edges will be properly collapsed.
  MYON_qsort(me_list,me_list_count,sizeof(me_list[0]),(QSORTCMPFUNC)CompareMESHEDGE);

  // create new vertex or vertices that edge will be
  // collapsed to.
  mesh.m_C.Destroy();
  mesh.m_K.Destroy();
  
  int mei;
  bool bHasVertexNormals = mesh.HasVertexNormals();
  bool bHasTextureCoordinates = mesh.HasTextureCoordinates();
  bool bHasFaceNormals = mesh.HasFaceNormals();
  if ( topv0.m_v_count == 1 || topv1.m_v_count == 1 )
  {
    // a single new vertex
    MYON_Line Vline(MYON_3dPoint::Origin,MYON_3dPoint::Origin);
    MYON_Line Tline(MYON_3dPoint::Origin,MYON_3dPoint::Origin);
    MYON_3dVector N0(0,0,0);
    MYON_3dVector N1(0,0,0);
    MYON_3dPoint P;
    int vi, tvi, cnt;

    int newvi = topv0.m_vi[0];

    cnt = 0;
    for ( tvi = 0; tvi < topv0.m_v_count; tvi++ )
    {
      vi = topv0.m_vi[tvi];
      if ( vi < 0 || vi > V_count )
        continue;
      if ( vi < newvi )
        newvi = vi;
      cnt++;
      P = mesh.Vertex(vi);
      Vline.from += P;
      if ( bHasVertexNormals )
      {
        N0 += MYON_3dVector(mesh.m_N[vi]);
      }
      if ( bHasTextureCoordinates )
      {
        P = mesh.m_T[vi];
        Tline.from += P;
      }
    }

    if (cnt > 1)
    {
      double s = 1.0/((double)cnt);
      Vline.from.x *= s;
      Vline.from.y *= s;
      Vline.from.z *= s;
      Tline.from.x *= s;
      Tline.from.y *= s;
      Tline.from.z *= s;
      N0 = s*N0;
    }

    cnt = 0;
    for (tvi = 0; tvi < topv1.m_v_count; tvi++)
    {
      vi = topv1.m_vi[tvi];
      if (vi < 0 || vi > V_count)
        continue;
      if (vi < newvi)
        newvi = vi;
      cnt++;
      P = mesh.Vertex(vi);
      Vline.to += P;
      if ( bHasVertexNormals )
      {
        N1 += MYON_3dVector(mesh.m_N[vi]);
      }
      if ( bHasTextureCoordinates )
      {
        P = mesh.m_T[vi];
        Tline.to += P;
      }
    }

    if (cnt > 1)
    {
      double s = 1.0/((double)cnt);
      Vline.to.x *= s;
      Vline.to.y *= s;
      Vline.to.z *= s;
      Tline.to.x *= s;
      Tline.to.y *= s;
      Tline.to.z *= s;
      N1 = s*N1;
    }

    MYON_3fPoint newV(Vline.PointAt(0.5));
    MYON_3fVector newN(MYON_3fVector::ZeroVector);
    MYON_2fPoint newT(MYON_2fPoint::Origin);
    if ( bHasVertexNormals )
    {
      N0.Unitize();
      N1.Unitize();
      MYON_3dVector N = N0+N1;
      if ( !N.Unitize() )
      {
        N = (topv0.m_v_count == 1) ? mesh.m_N[topv0.m_vi[0]] :mesh.m_N[topv1.m_vi[0]];
      }
      newN = N;
    }
    if ( bHasTextureCoordinates )
    {
      newT = Tline.PointAt(0.5);
    }
    for ( mei = 0; mei < me_list_count; mei++ )
    {
      me_list[mei].newvi = newvi;
      me_list[mei].newV = newV;
      me_list[mei].newN = newN;
      me_list[mei].newT = newT;
    }
  }
  else
  {
    // collapsing a "crease" edge - attempt to preserve
    // the crease.
    memset(&me,0,sizeof(me));
    me.vi0 = -1;
    me.vi1 = -1;
    for ( mei = 0; mei < me_list_count; mei++ )
    {
      // cook up new vertex
      me_list[mei].newvi = mesh.VertexCount();
      me = me_list[mei];
      MYON_Line line;
      line.from = mesh.Vertex(me.vi0);
      line.to   = mesh.Vertex(me.vi1);
      me.newV = line.PointAt(0.5);
      if ( bHasVertexNormals )
      {
        MYON_3dVector N0(mesh.m_N[me.vi0]);
        MYON_3dVector N1(mesh.m_N[me.vi1]);
        MYON_3dVector N = N0 + N1;
        if ( !N.Unitize() )
          N = N0;
        me.newN = N;
      }
      if ( bHasTextureCoordinates )
      {
        line.from = mesh.m_T[me.vi0];
        line.to   = mesh.m_T[me.vi1];
        me.newT = line.PointAt(0.5);
      }
      me.newvi = (me.vi0 < me.vi1) ? me.vi0 : me.vi1;

      me_list[mei].newvi = me.newvi;
      me_list[mei].newV = me.newV;
      me_list[mei].newN = me.newN;
      me_list[mei].newT = me.newT;
    }
  }

  // We are done averaging old mesh values.
  // Change values in mesh m_V[], m_N[] and m_T[] arrays.
  for ( mei = 0; mei < me_list_count; mei++ )
  {
    // We need to set the vertex location of all mesh vertices associated with the toplogical
    // edge or we risk "tearing apart" vertexes at corners where there are more than 2 mesh vertexes
    // associated with the topological vertex.  Don't to this for anything but the location.
    int i;
    for (i = 0; top.m_topv[me_list[mei].topvi0].m_v_count > i; i++)
      mesh.SetVertex(top.m_topv[me_list[mei].topvi0].m_vi[i], me_list[mei].newV);
    for (i = 0; top.m_topv[me_list[mei].topvi1].m_v_count > i; i++)
      mesh.SetVertex(top.m_topv[me_list[mei].topvi1].m_vi[i], me_list[mei].newV);
  
    if ( bHasVertexNormals )
    {
      mesh.m_N[me_list[mei].vi0] = me_list[mei].newN;
      mesh.m_N[me_list[mei].vi1] = me_list[mei].newN;
    }
    if ( bHasTextureCoordinates )
    {
      mesh.m_T[me_list[mei].vi0] = me_list[mei].newT;
      mesh.m_T[me_list[mei].vi1] = me_list[mei].newT;
    }
  }

  // make a map of old to new
  int old2new_map_count = 0;
  MYON__NEWVI* old2new_map = (MYON__NEWVI*)alloca(2*me_list_count*sizeof(old2new_map[0]));

  for ( mei = 0; mei < me_list_count; mei++ )
  {
    old2new_map[old2new_map_count].oldvi = me_list[mei].vi0;
    old2new_map[old2new_map_count].newvi = me_list[mei].newvi;
    old2new_map_count++;
    old2new_map[old2new_map_count].oldvi = me_list[mei].vi1;
    old2new_map[old2new_map_count].newvi = me_list[mei].newvi;
    old2new_map_count++;
  }

  // sort old2new_map[] so we can use a fast bsearch() call
  // to update faces.
  MYON_qsort(old2new_map,old2new_map_count,sizeof(old2new_map[0]),(QSORTCMPFUNC)CompareNEWVI);

  // count faces that use the vertices that are being changed
  int bad_fi_count = 0;
  int topv_end, vei, fi, fvi23, fvi;
  MYON__NEWVI nvi;

  for ( topv_end = 0; topv_end < 2; topv_end++ )
  {
    const MYON_MeshTopologyVertex& topv = (topv_end) ? topv1 : topv0;
    for ( vei = 0; vei < topv.m_tope_count; vei++ )
    {
      topei = topv.m_topei[vei];
      if ( topei < 0 && topei >= top.m_tope.Count() )
        continue;
      bad_fi_count += top.m_tope[topei].m_topf_count;
    }
  }
  int* bad_fi = (int*)alloca(bad_fi_count*sizeof(*bad_fi));
  bad_fi_count = 0;

  // Go through all the faces that use the vertices at the
  // ends of the edge and update the vi[] values to use the
  // new vertices.
  for ( topv_end = 0; topv_end < 2; topv_end++ )
  {
    const MYON_MeshTopologyVertex& topv = (topv_end) ? topv1 : topv0;
    for ( vei = 0; vei < topv.m_tope_count; vei++ )
    {
      topei = topv.m_topei[vei];
      if ( topei < 0 && topei >= top.m_tope.Count() )
        continue;
      const MYON_MeshTopologyEdge& e = top.m_tope[topei];
      for ( efi = 0; efi < e.m_topf_count; efi++ )
      {
        fi = e.m_topfi[efi];
        if ( fi < 0 || fi >= F_count )
          continue;
        bool bChangedFace = false;
        MYON_MeshFace& f = mesh.m_F[fi];
        for ( fvi = 0; fvi < 4; fvi++ )
        {
          nvi.oldvi = f.vi[fvi];
          MYON__NEWVI* p = (MYON__NEWVI*)bsearch(&nvi,old2new_map,old2new_map_count,sizeof(old2new_map[0]),(QSORTCMPFUNC)CompareNEWVI);
          if ( 0 != p && p->oldvi != p->newvi)
          {
            RemoveFaceOrVertexFromNgon(mesh, fi, &p->oldvi);
            f.vi[fvi] = p->newvi;
            bChangedFace = true;
          }
        }
        if ( bChangedFace )
        {
          if ( !f.IsValid(V_count) )
          {
            if ( f.vi[3] == f.vi[0] )
            {
              f.vi[0] = f.vi[1];
              f.vi[1] = f.vi[2];
              f.vi[2] = f.vi[3];
            }
            else if ( f.vi[0] == f.vi[1] )
            {
              fvi23 = f.vi[0];
              f.vi[0] = f.vi[2];
              f.vi[1] = f.vi[3];
              f.vi[2] = fvi23;
              f.vi[3] = fvi23;
            }
            else if ( f.vi[1] == f.vi[2] )
            {
              fvi23 = f.vi[1];
              f.vi[1] = f.vi[0];
              f.vi[0] = f.vi[3];
              f.vi[2] = fvi23;
              f.vi[3] = fvi23;
            }
            if ( f.vi[0] == f.vi[1] || f.vi[1] == f.vi[2] || f.vi[2] == f.vi[0] || f.vi[2] != f.vi[3] )
            {
              bad_fi[bad_fi_count++] = fi;
            }
          }
          if ( bHasFaceNormals )
          {
            // invalid faces are removed below
            MYON_3fVector a, b, n;
            a = mesh.Vertex(f.vi[2]) - mesh.Vertex(f.vi[0]);
            b = mesh.Vertex(f.vi[3]) - mesh.Vertex(f.vi[1]);
            n = MYON_CrossProduct( a, b );
            n.Unitize();
            mesh.m_FN[fi] = n;
          }
        }
      }
    }
  }

  if ( bad_fi_count > 0 )
  {
    // remove collapsed faces from mesh
    MYON_qsort(bad_fi,bad_fi_count,sizeof(bad_fi[0]),CompareInt);
    int bfi = 1;
    int dest_fi = bad_fi[0];
    unsigned int* face_index_map = nullptr;
    MYON_SimpleArray<unsigned int> face_index_map_array;
    if (mesh.HasNgons())
    {
      face_index_map_array.Reserve(mesh.m_F.UnsignedCount());
      face_index_map = face_index_map_array.Array();
      for (fi = 0; fi < dest_fi; fi++)
        face_index_map[fi] = fi;
      for (fi = dest_fi; fi < mesh.m_F.Count(); fi++)
        face_index_map[fi] = MYON_UNSET_UINT_INDEX;
    }

    for ( fi = dest_fi+1; fi < F_count && bfi < bad_fi_count; fi++ )
    {
      if ( fi == bad_fi[bfi] )
      {
        bfi++;
        if (nullptr != face_index_map)
          face_index_map[fi] = MYON_UNSET_UINT_INDEX;
      }
      else
      {
        // remove collapsed faces from ngons
        if (nullptr != face_index_map)
          face_index_map[fi] = dest_fi;
        mesh.m_F[dest_fi++] = mesh.m_F[fi];
      }
    }
    while (fi<F_count)
    {
      if (nullptr != face_index_map)
        face_index_map[fi] = dest_fi;
      mesh.m_F[dest_fi++] = mesh.m_F[fi++];
    }
    mesh.m_F.SetCount(dest_fi);
    const unsigned int new_mesh_F_count = mesh.m_F.UnsignedCount();


    bool bUpdateNgonMap = false;
    for ( unsigned int ni = 0; ni < mesh.m_Ngon.UnsignedCount(); ni++ )
    {
      MYON_MeshNgon* ngon = m_Ngon[ni];
      if (nullptr == ngon)
        continue;
      unsigned int new_ngon_F_count = 0;
      for (unsigned int nfi = 0; nfi < ngon->m_Fcount; nfi++)
      {
        unsigned int new_fi = face_index_map[ngon->m_fi[nfi]];
        if (new_fi >= new_mesh_F_count)
          continue;

        ngon->m_fi[new_ngon_F_count++] = new_fi;
      }

      MYON_MeshNgon* new_ngon;
      if (0 == new_ngon_F_count)
      {
        new_ngon = nullptr;
      }
      else if (new_ngon_F_count != ngon->m_Fcount)
      {
        new_ngon = m_NgonAllocator.AllocateNgon(ngon->m_Vcount, new_ngon_F_count);
        // copy old ngon vertex list to new ngon
        for (unsigned int nvi_for_loop = 0; nvi_for_loop < new_ngon->m_Vcount; nvi_for_loop++)
          new_ngon->m_vi[nvi_for_loop] = ngon->m_vi[nvi_for_loop];
        // update new ngon face list
        for (unsigned int nfi = 0; nfi < new_ngon->m_Fcount; nfi++)
          new_ngon->m_fi[nfi] = ngon->m_fi[nfi];
      }
      else
      {
        continue;
      }
      bUpdateNgonMap = true;
      m_Ngon[ni] = new_ngon;
      mesh.m_NgonAllocator.DeallocateNgon(ngon);
    }
    if (bUpdateNgonMap)
    {
      mesh.m_NgonMap.Destroy();
      mesh.CreateNgonMap();
    }

    if ( bHasFaceNormals )
    {
      bfi = 1;
      dest_fi = bad_fi[0];
      for ( fi = dest_fi+1; fi < F_count && bfi < bad_fi_count; fi++ )
      {
        if ( fi == bad_fi[bfi] )
        {
          bfi++;
        }
        else
        {
          mesh.m_FN[dest_fi++] = mesh.m_FN[fi];
        }
      }
      while (fi<F_count)
      {
        mesh.m_FN[dest_fi++] = mesh.m_FN[fi++];
      }
      mesh.m_FN.SetCount(dest_fi);
    }
  }

  mesh.Compact();
  mesh.DestroyTopology();
  mesh.DestroyPartition();

  return true;
}


//static int NewIndex( int old_index, int missing_index_count, const int* missing_index_list )
//{
//  // tool to calculate new indices when some elements are deleted from a list
//  int delta;
//  for ( delta = 0; delta < missing_index_count; delta++ )
//  {
//    if ( old_index < missing_index_list[delta] )
//      break;
//  }
//  return old_index - delta;
//}

bool MYON_Mesh::DeleteFace( int meshfi )
{
  // Do NOT add a call Compact() in this function.
  // Compact() is slow and this function may be called
  // many times in sequence.  
  // It is the callers responsibility to call Compact()
  // when it is needed.
  bool rc = false;

  if ( meshfi >= 0 && meshfi < m_F.Count() )
  {
    if ( m_top.m_topf.Count() > 0 )
    {
      DestroyTopology();
    }
    DestroyPartition();
    DestroyTree();
    if ( m_FN.Count() == m_F.Count() )
    {
      m_FN.Remove(meshfi);
    }
    m_F.Remove(meshfi);

    // 6 Mar 2010 S. Baer
    // Invalidate the cached IsClosed flag. This forces the mesh to
    // recompute IsClosed the next time it is called
    SetClosed(-1);

    rc = true;
  }
  return rc;
}

MYON_Mesh* MYON_ControlPolygonMesh(
  const MYON_NurbsSurface& nurbs_surface,
  bool bCleanMesh,
  MYON_Mesh* input_mesh
)
{
  int u0 = 0;
  int u1 = nurbs_surface.CVCount(0);

  int v0 = 0;
  int v1 = nurbs_surface.CVCount(1);

  if ( 0 == nurbs_surface.m_cv || !nurbs_surface.IsValid() )
  {
    MYON_ERROR("MYON_ControlPolygonMesh - surface is not valid");
    return nullptr;
  }

  MYON_SimpleArray<double> gu(u1);
  MYON_SimpleArray<double> gv(v1);
  gu.SetCount(u1);
  gv.SetCount(v1);
  nurbs_surface.GetGrevilleAbcissae(0,gu.Array());
  nurbs_surface.GetGrevilleAbcissae(1,gv.Array());

  MYON_Interval d0 = nurbs_surface.Domain(0);
  MYON_Interval d1 = nurbs_surface.Domain(1);

  bool bPeriodic0 = nurbs_surface.IsPeriodic(0)?true:false;
  bool bIsClosed0 = bPeriodic0 ? true : (nurbs_surface.IsClosed(0)?true:false);
  bool bPeriodic1 = nurbs_surface.IsPeriodic(1)?true:false;
  bool bIsClosed1 = bPeriodic1 ? true : (nurbs_surface.IsClosed(1)?true:false);

  if ( bPeriodic0 )
  {
    u1 -= (nurbs_surface.Degree(0) - 1);
    while ( u1 < nurbs_surface.CVCount(0) && gu[u0] < d0[0] && gu[u1-1] <= d0[1] )
    {
      u0++;
      u1++;
    }
    d0.Set(gu[u0],gu[u1-1]);
  }

  if ( bPeriodic1 )
  {
    v1 -= (nurbs_surface.Degree(1) - 1);
    while ( v1 < nurbs_surface.CVCount(1) && gv[v0] < d1[0] && gv[v1-1] <= d1[1] )
    {
      v0++;
      v1++;
    }
    d1.Set(gv[v0],gv[v1-1]);
  }


  MYON_Mesh* mesh = (0 == input_mesh) ? new MYON_Mesh() : input_mesh;

  int vertex_count = (u1-u0)*(v1-v0);
  int face_count = (u1-u0-1)*(v1-v0-1);

  MYON_3dPointArray& dpv = mesh->DoublePrecisionVertices();
  dpv.Reserve(vertex_count);
  mesh->m_N.Reserve(vertex_count);
  mesh->m_T.Reserve(vertex_count);
  mesh->m_S.Reserve(vertex_count);
  mesh->m_F.Reserve(face_count);
  mesh->m_srf_domain[0] = d0;
  mesh->m_srf_domain[1] = d1;

  MYON_3dPoint V;
  MYON_3dVector N;
  MYON_2dPoint S;
  MYON_2dPoint T;

  int hint[2] = {0,0};
  int i, j;
  int k = -1;
  for ( j = v0; j < v1; j++)
  {
    S.y = gv[j];
    T.y = d1.NormalizedParameterAt(S.y);
    for ( i = u0; i < u1; i++)
    {
      nurbs_surface.GetCV( i, j, V);
      S.x = gu[i];
      T.x = d0.NormalizedParameterAt(S.x);
      nurbs_surface.EvNormal(gu[i],gv[j],N,0,hint);
      dpv.AppendNew() = V;
      mesh->m_N.AppendNew() = N;
      mesh->m_S.AppendNew() = S;
      mesh->m_T.AppendNew() = T;
      if ( i > u0 && j > v0 )
      {
        MYON_MeshFace& f = mesh->m_F.AppendNew();
        f.vi[0] = k++;
        f.vi[1] = k;
        f.vi[2] = dpv.Count()-1;
        f.vi[3] = f.vi[2]-1;
      }
    }
    k++;
  }
  
  mesh->UpdateSinglePrecisionVertices();

  u1 -= u0;
  v1 -= v0;

  // make sure closed seams are spot on
  if ( bIsClosed0 )
  {
    i = 0;
    for ( j = 0; j < v1; j++ )
    {
      k = i + (u1-1);
      mesh->SetVertex(k, mesh->Vertex(i));
      if ( bPeriodic0 )
      {
        mesh->m_N[k] = mesh->m_N[i];
      }
      i = k+1;
      // do NOT synch texture coordinates
    }
  }

  if ( bIsClosed1 )
  {
    for ( i = 0, k = u1*(v1-1); i < u1; i++, k++ )
    {
      mesh->SetVertex(k, mesh->Vertex(i));
      if ( bPeriodic1 )
      {
        mesh->m_N[k] = mesh->m_N[i];
      }
      // do NOT synch texture coordinates
    }
  }

  // make sure singular ends are spot on
  i=0;
  for ( k = 0; k < 4; k++ )
  {
    if ( nurbs_surface.IsSingular(k) )
    {
      switch(k)
      {
      case 0: // 0 = south
        i = 0;
        j = 1;
        k = u1;
        break;

      case 1: // 1 = east
        i = u1-1;
        j = u1;
        k = u1*v1;
        break;

      case 2: // 2 = north
        i = u1*(v1-1);
        j = 1;
        k = u1*v1;
        break;

      case 3: // 3 = west
        i = 0;
        j = u1;
        k = u1*(v1-1)+1;
        break;
      }
      V = mesh->Vertex(i);
      for ( i = i+j; i < k; i += j )
      {
        mesh->SetVertex(i, V);
      }
    }
  }

  if ( bCleanMesh )
  {
    // Clean up triangles etc.
    MYON_3dPoint P[4];
    MYON_SimpleArray<int> badfi(32);
    for ( i = 0; i < mesh->m_F.Count(); i++ )
    {
      MYON_MeshFace& f = mesh->m_F[i];
      P[0] = mesh->Vertex(f.vi[0]);
      P[1] = mesh->Vertex(f.vi[1]);
      P[2] = mesh->Vertex(f.vi[2]);
      P[3] = mesh->Vertex(f.vi[3]);
      if ( P[0] == P[1] )
      {
        f.vi[1] = f.vi[2];
        f.vi[2] = f.vi[3];
        P[1] = P[2];
        P[2] = P[3];
      }
      if ( P[1] == P[2] )
      {
        f.vi[2] = f.vi[3];
        P[2] = P[3];
      }
      if ( P[2] == P[3] )
      {
        f.vi[2] = f.vi[3];
        P[2] = P[3];
      }
      if ( P[3] == P[0] )
      {
        f.vi[0] = f.vi[1];
        f.vi[1] = f.vi[2];
        f.vi[2] = f.vi[3];
        P[0] = P[1];
        P[1] = P[2];
        P[2] = P[3];
      }
      if (    f.vi[0] == f.vi[1] 
           || f.vi[1] == f.vi[2] 
           || f.vi[3] == f.vi[0] 
           || P[0] == P[2] || P[1] == P[3] )
      {
        badfi.Append(i);
      }
    }
    
    if ( badfi.Count() > 0 )
    {
      if ( badfi.Count() == mesh->m_F.Count() )
      {
        if ( input_mesh )
        {
          mesh->Destroy();
        }
        else
        {
          delete mesh;
        }
        mesh = 0;
      }
      else
      {
        // remove bad faces
        i = badfi[0];
        j = i+1;
        k = 1;
        for ( j = i+1; j < mesh->m_F.Count(); j++ )
        {
          if ( k < badfi.Count() && j == badfi[k] )
          {
            k++;
          }
          else
          {
            mesh->m_F[i++] = mesh->m_F[j];
          }
        }
        mesh->m_F.SetCount(i);
      }

      // 29 May 2008: Mikko, TRR 34687:
      // Added crash protection. At this point mesh is nullptr if it contained all bad faces.
      if ( mesh)
        mesh->CullUnusedVertices();
    }
  }

  return mesh;
}

///////////////////////////////////////////////////////////////////////
//
// mesh components
static bool IsUnweldedEdge(int edgeidx, const MYON_MeshTopology& Top)
{

  const MYON_MeshTopologyEdge& edge = Top.m_tope[edgeidx];
  if (1 == edge.m_topf_count)
    return true;

  if (1 == Top.m_topv[edge.m_topvi[0]].m_v_count || 1 == Top.m_topv[edge.m_topvi[1]].m_v_count)
  {
    //Both ends of the edge have to have more than one mesh vertex or they are for sure welded.
    //However having more than 1 vertex at both ends does not necessarily mean it is unwelded.
    return false; 
  }

  MYON_3dPoint ptA = Top.m_mesh->Vertex(Top.m_topv[edge.m_topvi[0]].m_vi[0]);
  MYON_3dPoint ptB = Top.m_mesh->Vertex(Top.m_topv[edge.m_topvi[1]].m_vi[0]);
  MYON_SimpleArray<int> ptAindexes(Top.m_topv[edge.m_topvi[0]].m_v_count);
  MYON_SimpleArray<int> ptBindexes(Top.m_topv[edge.m_topvi[1]].m_v_count);

  int i, ict = edge.m_topf_count;
  int j, jct;
  int k, kct;
  for (i=0; ict>i; i++)
  {
    const MYON_MeshFace& face = Top.m_mesh->m_F[edge.m_topfi[i]];
    jct = face.IsQuad()?4:3;
    for (j=0; jct>j; j++)
    {
      if (ptA == Top.m_mesh->Vertex(face.vi[j]))
      {
        if (0 == ptAindexes.Count())
        {
          ptAindexes.Append(face.vi[j]);
          continue;
        }
        else
        {
          kct = ptAindexes.Count();
          for (k=0; kct>k; k++)
          {
            if (ptAindexes[k] == face.vi[j])
              return false;
          }
          ptAindexes.Append(face.vi[j]);
        }
      }
      else if (ptB == Top.m_mesh->Vertex(face.vi[j]))
      {
        if (0 == ptBindexes.Count())
        {
          ptBindexes.Append(face.vi[j]);
          continue;
        }
        else
        {
          kct = ptBindexes.Count();
          for (k=0; kct>k; k++)
          {
            if (ptBindexes[k] == face.vi[j])
              return false;
          }
          ptBindexes.Append(face.vi[j]);
        }
      }
    }
  }

  return true;
}

static void FindAdjacentFaces(const MYON_MeshTopology& Top, 
                              MYON_SimpleArray<int>& FacesToCheck, 
                              const MYON_SimpleArray<int>& SortedFaceArray,
                              MYON_SimpleArray<int>& DupFaceArray,
                              bool bUseVertexConnections, 
                              bool bTopologicalConnections)
{
  int fi, vi, ei, facecount = FacesToCheck.Count(), totalcount = SortedFaceArray.Count();
  DupFaceArray.Zero();
  MYON_SimpleArray<int> OldFacesToCheck = FacesToCheck;

  FacesToCheck.Empty();

  for (fi=0;fi<facecount;fi++)
  {
    if (totalcount > OldFacesToCheck[fi])
    {
      if (0 == SortedFaceArray[OldFacesToCheck[fi]])
      {
        FacesToCheck.Append(OldFacesToCheck[fi]);
        DupFaceArray[OldFacesToCheck[fi]] = 1;
      }

      if (false == bUseVertexConnections)
      {
        int j;
        const MYON_MeshTopologyFace& face = Top.m_topf[OldFacesToCheck[fi]];
        for(ei=0;ei<(face.IsQuad()?4:3);ei++)
        {
          const MYON_MeshTopologyEdge& edge = Top.m_tope[face.m_topei[ei]];

          if (1 == edge.m_topf_count || (false == bTopologicalConnections && true == IsUnweldedEdge(face.m_topei[ei], Top)))
            continue;

          for(j=0;j<edge.m_topf_count;j++)
          {  
            if (0 == SortedFaceArray[edge.m_topfi[j]] && 1 != DupFaceArray[edge.m_topfi[j]])
            {
              FacesToCheck.Append(edge.m_topfi[j]);
              DupFaceArray[edge.m_topfi[j]] = 1;
            }
          }
        }
      }
      else
      {
        int j, k, m;
        MYON_3dPoint Pt;
        const MYON_MeshFace& face = Top.m_mesh->m_F[OldFacesToCheck[fi]];
        for(vi=0;vi<(face.IsQuad()?4:3);vi++)
        {
          const MYON_MeshTopologyVertex& vertex = Top.m_topv[Top.m_topv_map[face.vi[vi]]];
          for (j=0; vertex.m_tope_count>j; j++)
          {
            const MYON_MeshTopologyEdge& edge = Top.m_tope[vertex.m_topei[j]];
            for (k=0; edge.m_topf_count>k; k++)
            {
              if (true == bTopologicalConnections)
              {
                if (0 == SortedFaceArray[edge.m_topfi[k]] && 1 != DupFaceArray[edge.m_topfi[k]])
                {
                  FacesToCheck.Append(edge.m_topfi[k]);
                  DupFaceArray[edge.m_topfi[k]] = 1;
                }
              }
              else
              {
                Pt = Top.m_mesh->Vertex(vertex.m_vi[0]);
                const MYON_MeshFace& thisface = Top.m_mesh->m_F[edge.m_topfi[k]];
                for (m=0; m<(thisface.IsQuad()?4:3);m++)
                {
                  if (Pt != Top.m_mesh->Vertex(thisface.vi[m]))
                    continue;

                  if (face.vi[vi] == thisface.vi[m] && 0 == SortedFaceArray[edge.m_topfi[k]] && 1 != DupFaceArray[edge.m_topfi[k]])
                  {
                    //Faces share vertex 
                    FacesToCheck.Append(edge.m_topfi[k]);
                    DupFaceArray[edge.m_topfi[k]] = 1;
                    break;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}


int MYON_Mesh::GetConnectedComponents( bool bUseVertexConnections, 
                                     bool bTopologicalConnections, 
                                     MYON_SimpleArray<int>& facet_component_labels
                                   ) const
{
  int i, facecount = m_F.Count(), meshidx = 0;

  //This array will act as an associative array to m_F since MYON_MeshFace do not have something
  //like m_trim_user.i on a MYON_BrepTrim.  It will have the indice of the final mesh the face 
  //belongs to.
  if (facecount != facet_component_labels.Count())
  {
    facet_component_labels.Reserve(facecount);
    facet_component_labels.SetCount(facecount);
  }

  //initialize to 0
  facet_component_labels.MemSet(0);

  MYON_SimpleArray<int> DupFaceArray(facecount);
  DupFaceArray.SetCount(facecount);

  const MYON_MeshTopology& Top = Topology();
  if (!Top.IsValid())
    return 0;

  MYON_SimpleArray<int> FacesToCheck;
  FacesToCheck.Reserve(64);
  i = 0;
  while (i < facecount)
  {
    meshidx++;

    FacesToCheck.Append(i);

    while(0 != FacesToCheck.Count())
    {
      //Figure out which faces are connected to each other
      FindAdjacentFaces(Top, FacesToCheck, facet_component_labels, DupFaceArray, bUseVertexConnections, bTopologicalConnections);
      int j;
      for (j=0;j<FacesToCheck.Count();j++)
        facet_component_labels[FacesToCheck[j]] = meshidx;
    }

    for(;i<facecount;i++)
    {
      if (0 == facet_component_labels[i])
        break;
    }
  }
  
  return meshidx;
}

int MYON_Mesh::GetConnectedComponents( bool bUseVertexConnections, 
                                     bool bTopologicalConnections, 
                                     MYON_SimpleArray<MYON_Mesh*>* components
                                   ) const
{ 
  int i, j, k, kct, facecount = m_F.Count();
  MYON_SimpleArray<int> SortedFaceArray(facecount);
  SortedFaceArray.SetCount(facecount);

  int compct = GetConnectedComponents(bUseVertexConnections, bTopologicalConnections, SortedFaceArray);
  if (0 == compct || 0 == components)
    return compct;

  bool bHasFaceNormals = HasFaceNormals();
  bool bHasPrincipalCurvatures = HasPrincipalCurvatures();
  bool bHasSurfaceParameters = HasSurfaceParameters();
  bool bHasTextureCoordinates = HasTextureCoordinates();
  bool bHasVertexColors = HasVertexColors();
  bool bHasVertexNormals = HasVertexNormals();

  MYON_MeshFace newface;
  newface.vi[0] = -1; newface.vi[1] = -1; newface.vi[2] = -1; newface.vi[3] = -1; 

  MYON_SimpleArray<int> vertidxarray(VertexCount());
  vertidxarray.SetCount(VertexCount());

  MYON_SimpleArray<MYON_MeshNgon> ngons;
  MYON_SimpleArray<unsigned int> ngonfacecount;
  unsigned int ngonct = this->NgonUnsignedCount();
  MYON_SimpleArray<unsigned int> ngonfacevertexarray;
  if (0 < ngonct)
  {
    // if the original mesh had ngons setup an array of new ngons
    // to be filled in as the face/vertex remap occurs
    ngons.Reserve(ngonct);

    // this array will hold the current index for the face of the 
    // new ngon
    ngonfacecount.Reserve(ngonct);
    ngonfacecount.SetCount(ngonct);
    ngonfacecount.MemSet(0);

    unsigned int j_local, ct;
    // first figure out how much space we'll need for 
    // face and vertex arrays
    ct = 0;
    for (j_local = 0; ngonct > j_local; j_local++)
    {
      const MYON_MeshNgon* ngon = this->Ngon(j_local);
      if (nullptr == ngon)
        continue;
      ct += ngon->m_Vcount + ngon->m_Fcount;
    }

    ngonfacevertexarray.Reserve(ct);
    ngonfacevertexarray.SetCount(ct);
    unsigned int* tmp = ngonfacevertexarray.Array();
    //initialize to unset, not zero, so we can check for ngons
    //that were not part of this mark and prevent adding them to 
    //output
    memset(tmp, MYON_UNSET_UINT_INDEX, ct*sizeof(tmp[0]));
    ct = 0;
    for (j_local = 0; ngonct > j_local; j_local++)
    {
      const MYON_MeshNgon* ngon = this->Ngon(j_local);
      if (nullptr == ngon)
        continue;

      MYON_MeshNgon& new_ngon = ngons.AppendNew();
      new_ngon.m_Vcount = ngon->m_Vcount;
      new_ngon.m_Fcount = ngon->m_Fcount;
      new_ngon.m_vi = &ngonfacevertexarray[ct];
      ct += new_ngon.m_Vcount;
      new_ngon.m_fi = &ngonfacevertexarray[ct];
      ct += new_ngon.m_Fcount;
    }
  }

  const MYON_3dPoint* pMesh_D = 0;
  if ( HasDoublePrecisionVertices() )
  {
    if ( VertexCount() > 0 && HasSynchronizedDoubleAndSinglePrecisionVertices() )
    {
      pMesh_D = DoublePrecisionVertices().Array();
    }
  }

  MYON_3dPointArray bogus_D;

  for (i=1;compct>=i;i++)
  {
    kct = vertidxarray.Count();
    for (k=0; kct>k; k++)
      vertidxarray[k] = -1;

    MYON_Mesh* pNewMesh = new MYON_Mesh();
    if (0 == pNewMesh)
      continue;

    MYON_3dPointArray& pNewMesh_D = ( 0 != pMesh_D )
                       ? pNewMesh->DoublePrecisionVertices()
                       : bogus_D;

	  for (j=0;j<facecount;j++)
    {
      if ( i == SortedFaceArray[j] )
      {
        const MYON_MeshFace& face = m_F[j];
        kct = face.IsTriangle()?3:4; 
        for (k=0; k<kct; k++)
        {
          if (-1 != vertidxarray[face.vi[k]])
          {
            newface.vi[k] = vertidxarray[face.vi[k]];
            continue;
          }

          int newvi;
          if ( 0 != pMesh_D )
          {
            newvi = pNewMesh_D.Count();
            pNewMesh_D.Append(pMesh_D[face.vi[k]]);
          }
          else
          {
            newvi = pNewMesh->VertexCount();
            pNewMesh->m_V.AppendNew() = MYON_3fPoint(Vertex(face.vi[k]));
          }

          newface.vi[k] = vertidxarray[face.vi[k]] = newvi;

          if (true == bHasPrincipalCurvatures)
            pNewMesh->m_K.Append(m_K[face.vi[k]]);

          if (true == bHasSurfaceParameters)
            pNewMesh->m_S.Append(m_S[face.vi[k]]);

          if (true == bHasTextureCoordinates)
            pNewMesh->m_T.Append(m_T[face.vi[k]]);

          if (true == bHasVertexColors)
            pNewMesh->m_C.Append(m_C[face.vi[k]]);

          if (true == bHasVertexNormals)
            pNewMesh->m_N.Append(m_N[face.vi[k]]);
        }

        if (3 == kct)
          newface.vi[3] = newface.vi[2];

        if (0 < ngonct)
        {
          // Add new face indexes to the new ngons
          unsigned int idx = this->NgonIndexFromFaceIndex(j);
          if (MYON_UNSET_UINT_INDEX != idx)
            ngons[idx].m_fi[ngonfacecount[idx]++] = pNewMesh->FaceCount();
        }

        pNewMesh->m_F.Append(newface);
        if (true == bHasFaceNormals)
          pNewMesh->m_FN.Append(m_FN[j]);
      }
    }

    if (0 < ngonct)
    {
      unsigned int j_local, k_local;
      for (j_local = 0; ngonct > j_local; j_local++)
      {
        const MYON_MeshNgon* oldngon = this->Ngon(j_local);
        if (nullptr == oldngon)
          continue;

        MYON_MeshNgon& newngon = ngons[j_local];
        if (oldngon->m_Vcount != newngon.m_Vcount)
          continue; //should never happen

        for (k_local = 0; newngon.m_Vcount > k_local; k_local++)
          newngon.m_vi[k_local] = vertidxarray[oldngon->m_vi[k_local]];
      }

      bool bIsValid;
      for (j_local = 0; ngonct > j_local; j_local++)
      {
        bIsValid = true;
        MYON_MeshNgon& ngon = ngons[j_local];

        // if the faces/ngons were not marked then the 
        // output ngon will have not been set, do not
        // add it to the output mesh
        for (k_local = 0; ngon.m_Vcount > k_local && bIsValid; k_local++)
        {
          if (MYON_UNSET_UINT_INDEX == ngon.m_vi[k_local])
            bIsValid = false;
        }

        for (k_local = 0; ngon.m_Fcount > k_local && bIsValid; k_local++)
        {
          if (MYON_UNSET_UINT_INDEX == ngon.m_fi[k_local])
            bIsValid = false;
        }

        if (bIsValid)
          pNewMesh->AddNgon(ngons[j_local].m_Vcount, ngons[j_local].m_vi, ngons[j_local].m_Fcount, ngons[j_local].m_fi);
      }

      pNewMesh->CreateNgonMap();
    }

    if ( 0 != pMesh_D )
      pNewMesh->UpdateSinglePrecisionVertices();

    pNewMesh->Compact();

    if (0 < pNewMesh->m_F.Count())
      components->Append(pNewMesh);
    else
    {
      delete pNewMesh;
      pNewMesh = 0;
    }
  }

  return compct;
}

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
//  Definitions of MYON_GL() functions that demonstrate how to
//  use GL to display OpenNURBS objects.			
//				
////////////////////////////////////////////////////////////////

#include "opennurbs.h"

#if defined(MYON_COMPILER_MSC)

// Tested compilers:
//   Microsoft Developer Studio 6.0
//   Microsoft Visual Studio 2005
//   Support for other Windows compilers is not available.

// Windows Open GL files require windows.h to be included before the
// Open GL header files.
#pragma MYON_PRAGMA_WARNING_PUSH
#include <windows.h>
#include <GL/gl.h>      // Open GL basic definitions
#include <GL/glu.h>     // Open GL utilities (for GL NURBS stuff)
#pragma MYON_PRAGMA_WARNING_POP

#elif defined(MYON_COMPILER_CLANG)

// Tested compilers:
//   Apple Xcode 2.4.1
//   Support for other Apple compilers is not available.
#include <GLUT/glut.h>   // Open GL auxiliary functions

#else

// Unsupported compiler:
//   Support for other compilers is not available
#include <GL/gl.h>      // Open GL basic definitions
#include <GL/glu.h>     // Open GL utilities (for GL NURBS stuff)

#endif


#if !defined(OPENNURBS_GL_INC_)
#define OPENNURBS_GL_INC_


// Use MYON_GL( const MYON_Point, ...) to render single points.
void MYON_GL( 
      const MYON_Point&
      );

// Use MYON_GL( const MYON_PointCloud, ...) to render Rhino point sets.
void MYON_GL( 
      const MYON_PointCloud&
      );

// Use MYON_GL( const MYON_Mesh&, ...) to render OpenNURBS meshes.
void MYON_GL( 
      const MYON_Mesh&
      );

// Use MYON_GL( const MYON_Brep&, ...) to render OpenNURBS b-reps.
void MYON_GL( 
      const MYON_Brep&,
      GLUnurbsObj*
      );

// must be bracketed by calls to glBegin(GL_POINTS) / glEnd()
void MYON_GL(
      const MYON_3dPoint&
      );

void MYON_GL( 
      const MYON_Curve&,   // 
      GLUnurbsObj*,      // created with gluNewNurbsRenderer
      GLenum = 0,        // type of curve (if 0, type is automatically set)
      double[][4] = nullptr // optional transformation applied to curve
      );

// must be bracketed by calls to gluBeginSurface( nobj )/gluEndSurface( nobj )
void MYON_GL( 
      const MYON_Surface&,   // 
      GLUnurbsObj*         // created with gluNewNurbsRenderer
      );

// Use MYON_GL( const MYON_NurbsCurve&,...) in place of
// gluNurbsCurve().  See your system's gluNurbsCurve() documentation
// for details.  In particular, for 3d curves the call to 
// MYON_GL( const MYON_NurbsCurve&, nobj,...) should appear inside
// of a gluBeginCurve( nobj )/gluEndCurve( nobj ) pair.
// Generally, the GL "type" should be set using the formula
//   MYON_NurbsCurve:IsRational()
//   ? GL_MAP1_VERTEX_4 
//   : GL_MAP1_VERTEX_3;
void MYON_GL( 
      const MYON_NurbsCurve&, // 
      GLUnurbsObj*,         // created with gluNewNurbsRenderer
      GLenum = 0,           // type of curve (if 0, type is automatically set)
      int = 1,              // bPermitKnotScaling - If true, curve knots may
                            // be rescaled to avoid knot vectors GL cannot handle.
      double* = nullptr,       // knot_scale[2] - If not nullptr and bPermitKnotScaling,
                            // the scaling applied to the knot vector is
                            // returned here.
      double[][4] = nullptr    // optional transformation applied to curve
      );

void MYON_GL( // low level NURBS curve renderer
      int, int, int, int, // dim, is_rat, cv_count, order
      const double*,      // knot_vector[]
      int,                // cv_stride
      const double*,      // cv
      GLUnurbsObj*,       // created with gluNewNurbsRenderer
      GLenum = 0,         // type of curve (if 0, type is automatically set)
      int = 1,            // bPermitKnotScaling - If true, curve knots may
                          // be rescaled to avoid knot vectors GL cannot handle.
      double* = nullptr,     // knot_scale[2] - If not nullptr and bPermitKnotScaling,
                          // the scaling applied to the knot vector is
                          // returned here.
      double[][4] = nullptr  // optional transformation applied to curve
      );


// Use MYON_GL( const MYON_NurbsSurface&,...) in place of
// gluNurbsSurface().  See your system's gluNurbsSurface() documentation
// for details.  In particular, the call to 
// MYON_GL( const MYON_NurbsSurface&, nobj, ...) should appear inside
// of a gluBeginSurface( nobj )/gluEndSurface( nobj ) pair.
// Generally, the GL "type" should be set using the formula
//   MYON_NurbsSurface:IsRational()
//   ? GL_MAP2_VERTEX_4 
//   : GL_MAP2_VERTEX_3;
void MYON_GL( 
      const MYON_NurbsSurface&, // 
      GLUnurbsObj*,              // created with gluNewNurbsRenderer
      GLenum = 0,                // type of surface 
                                 // (if 0, type is automatically set)
      int = 1,           // bPermitKnotScaling - If true, surface knots may
                         // be rescaled to avoid knot vectors GL cannot handle.
      double* = nullptr,    // knot_scale0[2] - If not nullptr and bPermitKnotScaling,
                         // the scaling applied to the first parameter is
                         // returned here.
      double* = nullptr     // knot_scale0[2] - If not nullptr and bPermitKnotScaling,
                         // the scaling applied to the second parameter is
                         // returned here.
      );


// Use MYON_GL( const MYON_BrepFace&, nobj ) to render
// the trimmed NURBS surface that defines a MYON_Brep face's geometry.
// The call to MYON_GL( const MYON_BrepFace&, nobj ) should
// appear inside of a gluBeginSurface( nobj )/gluEndSurface( nobj )
// pair.
void MYON_GL( 
      const MYON_BrepFace&, // 
      GLUnurbsObj*       // created with gluNewNurbsRenderer
      );

// Use MYON_GL( const MYON_Color ...) to set GL color to OpenNURBS color
void MYON_GL( const MYON_Color&,
              GLfloat[4] 
              );
void MYON_GL( const MYON_Color&,
              double, // alpha
              GLfloat[4] 
              );

// Use MYON_GL( const MYON_Material ...) to set GL material to OpenNURBS material
void MYON_GL( 
      const MYON_Material&
      );

void MYON_GL( 
      const MYON_Material* // pass nullptr to get OpenNURBS's default material
      );

// Use MYON_GL( const MYON_Light, ...) to add OpenNURBS spotlights to
// GL lighting model
void MYON_GL( 
      const MYON_Light*, // pass nullptr to disable the light
      GLenum                  // GL_LIGHTi where 0 <= i <= GL_MAX_LIGHTS
                              // See glLight*() documentation for details
      );
void MYON_GL( 
      const MYON_Light&,
      GLenum                  // GL_LIGHTi where 0 <= i <= GL_MAX_LIGHTS
                              // See glLight*() documentation for details
      );

//////////////////////////////////////////////////////////////////////////
// Use MYON_GL( MYON_Viewport& ... ) to set the GL projections to match
// those used in the OpenNURBS viewport.

////////////
//
// Use MYON_GL( MYON_Viewport&, in, int, int, int ) to specify the size of the
// GL window and loads the GL projection matrix (camera to clip 
// transformation). If the aspect ratio of the GL window and 
// MYON_Viewport's frustum do not match, the viewport's frustum is 
// adjusted to get things back to 1:1.  
//
// For systems where the upper left corner of a window has 
// coordinates (0,0) use:
//   port_left   = 0
//   port_right  = width-1
//   port_bottom = height-1
//   port_top    = 0
void MYON_GL( MYON_Viewport&,
            int, int, // port_left, port_right (port_left != port_right)
            int, int  // port_bottom, port_top (port_bottom != port_top)
            );

////////////
//
// Use MYON_GL( MYON_Viewport& ) to load the GL model view matrix (world to 
// camera transformation).
void MYON_GL( const MYON_Viewport& );

// Use MYON_GL( order, cv_count, knot, bPermitScaling, glknot )
// to create knot vectors suitable for GL NURBS rendering.
void MYON_GL( 
          const int,     // order, MYON_NurbsCurve... order
          const int,     // cv_count, MYON_NurbsCurve... cv count
          const double*, // knot, MYON_NurbsCurve... knot vector
          GLfloat*,      // glknot[] - GL knot vector
          int = 0,       // bPermitScaling - true if re-scaling is allowed
          double* = nullptr // scale[2] - If not nullptr and bPermitScaling is true,
                         // then the scaling parameters are returned here.
                         // ( glknot = (knot = scale[0])*scale[1] )
          );

#endif

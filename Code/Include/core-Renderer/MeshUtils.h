/// @file   core-Renderer/MeshUtils.h
/// @brief  a set of triangle mesh related utilities
#pragma once

#include "core\Array.h"
#include "core-Renderer\Face.h"
#include "core-Renderer\LitVertex.h"


///////////////////////////////////////////////////////////////////////////////

class MeshUtils
{
public:
   /**
    * Calculates texture-space tangent vectors. 
    *
    * @param faces
    * @param inOutVertices
    */
   static void calculateVertexTangents( const Array< Face >& faces, Array< LitVertex >& inOutVertices );

   /**
    * Calculates vertex normals for a mesh, making it look flat.
    * The condition here is that no face shares vertices with other faces.
    *
    * @param faces
    * @param inOutVertices
    */
   static void calculateFlatMeshNormals( const Array< Face >& faces, Array< LitVertex >& inOutVertices );
};

///////////////////////////////////////////////////////////////////////////////

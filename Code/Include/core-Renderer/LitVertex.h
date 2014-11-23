/// @file   core-Renderer\LitVertex.h
/// @brief  geometry vertex definition
#pragma once

#include "core\Vector.h"
#include "core\types.h"
#include <iostream>


///////////////////////////////////////////////////////////////////////////////

class InStream;
class OutStream;

///////////////////////////////////////////////////////////////////////////////

/**
 * GeometryComponent vertex definition.
 */
struct LitVertex
{
   DECLARE_ALLOCATOR( LitVertex, AM_DEFAULT );

   TVector<4>   m_coords;
   TVector<3>   m_normal;
   TVector<3>   m_tangent;
   TVector<2>   m_textureCoords;

   /**
    * Constructor for a simple mesh vertex.
    */
   LitVertex( float x, float y, float z, float nx, float ny, float nz, float tx, float ty, float tz, float u, float v )
   {
      m_coords.v[0] = x; m_coords.v[1] = y; m_coords.v[2] = z; m_coords.v[3] = 1;
      m_normal.v[0] = nx; m_normal.v[1] = ny; m_normal.v[2] = nz;
      m_tangent.v[0] = tx; m_tangent.v[1] = ty; m_tangent.v[2] = tz;
      m_textureCoords.v[0] = u; m_textureCoords.v[1] = v;
   }

   LitVertex()
   {
      m_coords.setUniform( 0.0f );
      m_normal.setUniform( 0.0f );
      m_tangent.setUniform( 0.0f );
      m_textureCoords.setUniform( 0.0f );
   }

   friend OutStream& operator<<( OutStream& stream, const LitVertex& vtx );
   friend InStream& operator>>( InStream& stream, LitVertex& vtx );
};

///////////////////////////////////////////////////////////////////////////////

struct VertexWeight
{
   DECLARE_ALLOCATOR( VertexWeight, AM_DEFAULT );

   TVector<4>          m_weights;
   TVector<4>          m_indices;

   VertexWeight()
   {
      m_weights.setUniform( 0.0f );
      m_indices.setUniform( -1.0f );
   }
   
   friend OutStream& operator<<( OutStream& stream, const VertexWeight& weight );
   friend InStream& operator>>( InStream& stream, VertexWeight& weight );
};

///////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<( std::ostream& stream, const LitVertex& vertex );
std::ostream& operator<<( std::ostream& stream, const VertexWeight& vertex );

///////////////////////////////////////////////////////////////////////////////
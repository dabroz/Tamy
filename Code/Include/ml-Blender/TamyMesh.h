/// @file   ml-Blender/TamyMesh.h
/// @brief  Tamy mesh export-related proxies
#pragma once


///////////////////////////////////////////////////////////////////////////////

struct LitVertex;
struct Face;
struct VertexWeight;

///////////////////////////////////////////////////////////////////////////////

struct TamyMesh
{
   const char*    name;

   LitVertex*     verticesList;
   Face*          facesList;
   VertexWeight*  vertexWeightsList;
   int*           materialsList;

   int            facesCount;
   int            verticesCount;
   int            materialsCount;

   bool           hasVertexWeights;
};

///////////////////////////////////////////////////////////////////////////////

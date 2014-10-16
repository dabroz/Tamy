#include "core-Renderer/MeshUtils.h"
#include "core/Math.h"


///////////////////////////////////////////////////////////////////////////////

void MeshUtils::calculateVertexTangents( const Array< Face >& faces, Array< LitVertex >& inOutVertices )
{
    /**
    * I used the algorithm from this page: http://www.terathon.com/code/tangent.html.
    */

   uint vertexCount = inOutVertices.size();
   Vector* tan1 = new Vector[vertexCount * 2];
   Vector* tan2 = tan1 + vertexCount;
   memset( tan1, 0, vertexCount * sizeof(Vector) * 2 );

   Vector sdir, tdir;
   uint trianglesCount = faces.size();
   for ( uint i = 0; i < trianglesCount; ++i )
   {
      const Face& tri = faces[i];
      uint i1 = tri.idx[0];
      uint i2 = tri.idx[1];
      uint i3 = tri.idx[2];

      const TVector<4>& v1 = inOutVertices[i1].m_coords;
      const TVector<4>& v2 = inOutVertices[i2].m_coords;
      const TVector<4>& v3 = inOutVertices[i3].m_coords;

      const TVector<2>& w1 = inOutVertices[i1].m_textureCoords;
      const TVector<2>& w2 = inOutVertices[i2].m_textureCoords;
      const TVector<2>& w3 = inOutVertices[i3].m_textureCoords;

      float x1 = v2[0] - v1[0];
      float x2 = v3[0] - v1[0];
      float y1 = v2[1] - v1[1];
      float y2 = v3[1] - v1[1];
      float z1 = v2[2] - v1[2];
      float z2 = v3[2] - v1[2];

      float s1 = w2[0] - w1[0];
      float s2 = w3[0] - w1[0];
      float t1 = w2[1] - w1[1];
      float t2 = w3[1] - w1[1];

      float r = (s1 * t2 - s2 * t1);
      if ( r != 0.0f )
      { 
         r = 1.0f / r;

         sdir.set( (t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r );
         tdir.set( (s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r );

         tan1[i1].add( sdir );
         tan1[i2].add( sdir );
         tan1[i3].add( sdir );

         tan2[i1].add( tdir );
         tan2[i2].add( tdir );
         tan2[i3].add( tdir );
      }
   }

   Vector vertexNorm, vertexTangent, normTanCross;
   for ( uint i = 0; i < vertexCount; ++i )
   {
      vertexNorm.load( inOutVertices[i].m_normal );
      const Vector& t = tan1[i];

      // Gram-Schmidt orthogonalize
      FastFloat normTan1Dot;
      normTan1Dot.setNeg( vertexNorm.dot( t ) );
      vertexTangent.setMulAdd( vertexNorm, normTan1Dot, t );
      vertexTangent.normalize();

      // Calculate handedness
      normTanCross.setCross( vertexNorm, t );
      FastFloat handedness;
      handedness.setSign( normTanCross.dot( tan2[i] ) );
      handedness.reciprocal();
      vertexTangent.mul( handedness );

      // store the results
      vertexTangent.store( inOutVertices[i].m_tangent );
   }

   delete[] tan1;
}

///////////////////////////////////////////////////////////////////////////////

void MeshUtils::calculateFlatMeshNormals( const Array< Face >& faces, Array< LitVertex >& inOutVertices )
{
   Plane trianglePlane;

   uint facesCount = faces.size();
   Vector v1, v2, v3, triangleNormal;
   for ( uint faceIdx = 0; faceIdx < facesCount; ++faceIdx )
   {
      const Face& face = faces[faceIdx];

      LitVertex& lv1 = inOutVertices[ face.idx[0] ];
      LitVertex& lv2 = inOutVertices[ face.idx[1] ];
      LitVertex& lv3 = inOutVertices[ face.idx[2] ];

      v1.load( lv1.m_coords );
      v2.load( lv2.m_coords );
      v3.load( lv3.m_coords );

      trianglePlane.setFromPoints( v1, v2, v3 );
      trianglePlane.getNormal( triangleNormal );

      triangleNormal.store( lv1.m_normal );
      triangleNormal.store( lv2.m_normal );
      triangleNormal.store( lv3.m_normal );
   }
}

///////////////////////////////////////////////////////////////////////////////

#include "DebugGeometryBuilder.h"
#include "core-Renderer\TriangleMesh.h"
#include "core-Renderer\MeshUtils.h"


///////////////////////////////////////////////////////////////////////////////

TriangleMesh* DebugGeometryBuilder::createBox( const Vector& min, const Vector& max )
{
   Array<LitVertex> vertices;
   Array<Face> faces;
   vertices.resize( 24 );
   faces.resize( 12 );

   Vector tmpVec;
   {
      // add the lines:
      // bottom face
      tmpVec.set( min[0], min[1], max[2] ); tmpVec.store( vertices[0].m_coords );
      tmpVec.set( min[0], min[1], min[2] ); tmpVec.store( vertices[1].m_coords );
      tmpVec.set( max[0], min[1], max[2] ); tmpVec.store( vertices[2].m_coords );
      tmpVec.set( max[0], min[1], min[2] ); tmpVec.store( vertices[3].m_coords );
      faces[0].idx[0] = 0; faces[0].idx[1] = 1; faces[0].idx[2] = 2;
      faces[1].idx[0] = 1; faces[1].idx[1] = 3; faces[1].idx[2] = 2;

      // top face
      tmpVec.set( min[0], max[1], max[2] ); tmpVec.store( vertices[4].m_coords );
      tmpVec.set( min[0], max[1], min[2] ); tmpVec.store( vertices[5].m_coords );
      tmpVec.set( max[0], max[1], max[2] ); tmpVec.store( vertices[6].m_coords );
      tmpVec.set( max[0], max[1], min[2] ); tmpVec.store( vertices[7].m_coords );
      faces[2].idx[0] = 5; faces[2].idx[1] = 4; faces[2].idx[2] = 7;
      faces[3].idx[0] = 4; faces[3].idx[1] = 6; faces[3].idx[2] = 7;

      // right face
      tmpVec.set( min[0], min[1], min[2] ); tmpVec.store( vertices[8].m_coords );
      tmpVec.set( max[0], min[1], min[2] ); tmpVec.store( vertices[9].m_coords );
      tmpVec.set( min[0], max[1], min[2] ); tmpVec.store( vertices[10].m_coords );
      tmpVec.set( max[0], max[1], min[2] ); tmpVec.store( vertices[11].m_coords );
      faces[4].idx[0] = 8; faces[4].idx[1] = 10; faces[4].idx[2] = 9;
      faces[5].idx[0] = 10; faces[5].idx[1] = 11; faces[5].idx[2] = 9;

      // left face
      tmpVec.set( min[0], min[1], max[2] ); tmpVec.store( vertices[12].m_coords );
      tmpVec.set( min[0], min[1], min[2] ); tmpVec.store( vertices[13].m_coords );
      tmpVec.set( min[0], max[1], max[2] ); tmpVec.store( vertices[14].m_coords );
      tmpVec.set( min[0], max[1], min[2] ); tmpVec.store( vertices[15].m_coords );
      faces[6].idx[0] = 12; faces[6].idx[1] = 14; faces[6].idx[2] = 13;
      faces[7].idx[0] = 14; faces[7].idx[1] = 15; faces[7].idx[2] = 13;

      // front face
      tmpVec.set( min[0], min[1], max[2] ); tmpVec.store( vertices[16].m_coords );
      tmpVec.set( max[0], min[1], max[2] ); tmpVec.store( vertices[17].m_coords );
      tmpVec.set( min[0], max[1], max[2] ); tmpVec.store( vertices[18].m_coords );
      tmpVec.set( max[0], max[1], max[2] ); tmpVec.store( vertices[19].m_coords );
      faces[8].idx[0] = 17; faces[8].idx[1] = 19; faces[8].idx[2] = 16;
      faces[9].idx[0] = 19; faces[9].idx[1] = 18; faces[9].idx[2] = 16;

      // back face
      tmpVec.set( max[0], min[1], max[2] ); tmpVec.store( vertices[20].m_coords );
      tmpVec.set( max[0], min[1], min[2] ); tmpVec.store( vertices[21].m_coords );
      tmpVec.set( max[0], max[1], max[2] ); tmpVec.store( vertices[22].m_coords );
      tmpVec.set( max[0], max[1], min[2] ); tmpVec.store( vertices[23].m_coords );
      faces[10].idx[0] = 21; faces[10].idx[1] = 23; faces[10].idx[2] = 20;
      faces[11].idx[0] = 23; faces[11].idx[1] = 22; faces[11].idx[2] = 20;
   }

   MeshUtils::calculateFlatMeshNormals( faces, vertices );

   TriangleMesh* mesh = new TriangleMesh( FilePath(), vertices, faces );
   return mesh;
}

///////////////////////////////////////////////////////////////////////////////

TriangleMesh* DebugGeometryBuilder::createArrow( float size, const Vector& start, const Vector& end )
{
   Vector dir;
   dir.setSub( end, start );
   dir.normalize();

   FastFloat simdSize; simdSize.setFromFloat( size );

   // the line
   Array<LitVertex> vertices;
   Array<Face> faces;
   addCuboid( simdSize, start, end, vertices, faces );

   // the cone tip
   FastFloat tipSize; tipSize.setFromFloat( 1.3f * size );
   Vector tipEnd;
   tipEnd.setMulAdd( dir, tipSize, end );
   addCone( tipSize, end, tipEnd, vertices, faces );

   MeshUtils::calculateFlatMeshNormals( faces, vertices );

   TriangleMesh* mesh = new TriangleMesh( FilePath(), vertices, faces );
   return mesh;
}

///////////////////////////////////////////////////////////////////////////////

TriangleMesh* DebugGeometryBuilder::createDiamond( float size, const Vector& start, const Vector& end )
{
   Array<LitVertex> vertices;
   Array<Face> faces;

   FastFloat tipSize; tipSize.setFromFloat( size );
   addCone( tipSize, start, end, vertices, faces );

   MeshUtils::calculateFlatMeshNormals( faces, vertices );

   TriangleMesh* mesh = new TriangleMesh( FilePath(), vertices, faces );
   return mesh;

}

///////////////////////////////////////////////////////////////////////////////

TriangleMesh* DebugGeometryBuilder::createBoxHeadedArrow( float size, const Vector& start, const Vector& end )
{
   FastFloat simdSize; simdSize.setFromFloat( size );

   Vector dir;
   dir.setSub( end, start );
   dir.normalize();

   // the line
   Array<LitVertex> vertices;
   Array<Face> faces;
   addCuboid( simdSize, start, end, vertices, faces );
   
   // and the box-shaped tip
   FastFloat tipSize; tipSize.setFromFloat( 6 * size );
   Vector tipEnd;
   tipEnd.setMulAdd( dir, tipSize, end );
   addCuboid( tipSize, end, tipEnd, vertices, faces );
   
   MeshUtils::calculateFlatMeshNormals( faces, vertices );

   TriangleMesh* mesh = new TriangleMesh( FilePath(), vertices, faces );
   return mesh;
}

///////////////////////////////////////////////////////////////////////////////

void DebugGeometryBuilder::addCone( const FastFloat& baseSize, const Vector& start, const Vector& end, Array< LitVertex >& outVertices, Array< Face >& outFaces )
{
   // calculate additional vectors needed for cuboid construction
   Vector dir;
   dir.setSub( end, start );
   dir.normalize();

   Vector perpVec1, perpVec2;
   VectorUtil::calculatePerpendicularVector( dir, perpVec1 );
   perpVec1.normalize();
   perpVec2.setCross( dir, perpVec1 );

   perpVec1.mul( baseSize );
   perpVec2.mul( baseSize );

   // calculate the vertices and outFaces
   uint firstVtxIdx = outVertices.size();
   uint firstFaceIdx = outFaces.size();

   for( uint i = 0; i < 16; ++i )
   {
      outVertices.push_back( LitVertex() );
   }
   for( uint i = 0; i < 6; ++i )
   {
      outFaces.push_back( Face() );
   }

   Vector baseVertex[4];
   Vector tipVertex;
   {
      perpVec1.mul( Float_6 );
      perpVec2.mul( Float_6 );

      FastFloat tipSizeMultiplier; tipSizeMultiplier.setFromFloat( 12 );
      tipSizeMultiplier.mul( baseSize );
      tipVertex.setMulAdd( dir, tipSizeMultiplier, end );

      baseVertex[0].setAdd( start, perpVec1 ); baseVertex[0].sub( perpVec2 );
      baseVertex[1].setSub( start, perpVec1 ); baseVertex[1].sub( perpVec2 );
      baseVertex[2].setAdd( start, perpVec1 ); baseVertex[2].add( perpVec2 );
      baseVertex[3].setSub( start, perpVec1 ); baseVertex[3].add( perpVec2 );

      // cone bottom
      baseVertex[0].store( outVertices[firstVtxIdx + 0].m_coords );
      baseVertex[1].store( outVertices[firstVtxIdx + 1].m_coords );
      baseVertex[2].store( outVertices[firstVtxIdx + 2].m_coords );
      baseVertex[3].store( outVertices[firstVtxIdx + 3].m_coords );
      outFaces[firstFaceIdx + 0].idx[0] = firstVtxIdx + 0; outFaces[firstFaceIdx + 0].idx[1] = firstVtxIdx + 1; outFaces[firstFaceIdx + 0].idx[2] = firstVtxIdx + 2;
      outFaces[firstFaceIdx + 1].idx[0] = firstVtxIdx + 1; outFaces[firstFaceIdx + 1].idx[1] = firstVtxIdx + 3; outFaces[firstFaceIdx + 1].idx[2] = firstVtxIdx + 2;

      // cone side 1
      baseVertex[0].store( outVertices[firstVtxIdx + 4].m_coords );
      tipVertex.store( outVertices[firstVtxIdx + 5].m_coords );
      baseVertex[1].store( outVertices[firstVtxIdx + 6].m_coords );
      outFaces[firstFaceIdx + 2].idx[0] = firstVtxIdx + 4;   outFaces[firstFaceIdx + 2].idx[1] = firstVtxIdx + 5;   outFaces[firstFaceIdx + 2].idx[2] = firstVtxIdx + 6;

      // cone side 2
      baseVertex[1].store( outVertices[firstVtxIdx + 7].m_coords );
      tipVertex.store( outVertices[firstVtxIdx + 8].m_coords );
      baseVertex[3].store( outVertices[firstVtxIdx + 9].m_coords );
      outFaces[firstFaceIdx + 3].idx[0] = firstVtxIdx + 7;   outFaces[firstFaceIdx + 3].idx[1] = firstVtxIdx + 8;   outFaces[firstFaceIdx + 3].idx[2] = firstVtxIdx + 9;

      // cone side 3
      baseVertex[3].store( outVertices[firstVtxIdx + 10].m_coords );
      tipVertex.store( outVertices[firstVtxIdx + 11].m_coords );
      baseVertex[2].store( outVertices[firstVtxIdx + 12].m_coords );
      outFaces[firstFaceIdx + 4].idx[0] = firstVtxIdx + 10;   outFaces[firstFaceIdx + 4].idx[1] = firstVtxIdx + 11;   outFaces[firstFaceIdx + 4].idx[2] = firstVtxIdx + 12;

      // cone side 4
      baseVertex[2].store( outVertices[firstVtxIdx + 13].m_coords );
      tipVertex.store( outVertices[firstVtxIdx + 14].m_coords );
      baseVertex[0].store( outVertices[firstVtxIdx + 15].m_coords );
      outFaces[firstFaceIdx + 5].idx[0] = firstVtxIdx + 13;   outFaces[firstFaceIdx + 5].idx[1] = firstVtxIdx + 14;   outFaces[firstFaceIdx + 5].idx[2] = firstVtxIdx + 15;
   }
}

///////////////////////////////////////////////////////////////////////////////

void DebugGeometryBuilder::addCuboid( const FastFloat& size, const Vector& start, const Vector& end, Array< LitVertex >& outVertices, Array< Face >& outFaces )
{
   // calculate additional vectors needed for cuboid construction
   Vector dir;
   dir.setSub( end, start );
   dir.normalize();

   Vector perpVec1, perpVec2;
   VectorUtil::calculatePerpendicularVector( dir, perpVec1 );
   perpVec1.normalize();
   perpVec2.setCross( dir, perpVec1 );

   perpVec1.mul( size );
   perpVec2.mul( size );


   // draw the cuboid
   uint firstVtxIdx = outVertices.size();
   uint firstFaceIdx = outFaces.size();

   for( uint i = 0; i < 24; ++i )
   {
      outVertices.push_back( LitVertex() );
   }
   for( uint i = 0; i < 12; ++i )
   {
      outFaces.push_back( Face() );
   }
   Vector tmpVec;
   {
      // add the lines:
      // bottom face
      tmpVec.setAdd( start, perpVec1 ); tmpVec.sub( perpVec2 ); tmpVec.store( outVertices[firstVtxIdx + 0].m_coords );
      tmpVec.setSub( start, perpVec1 ); tmpVec.sub( perpVec2 ); tmpVec.store( outVertices[firstVtxIdx + 1].m_coords );
      tmpVec.setAdd( start, perpVec1 ); tmpVec.add( perpVec2 ); tmpVec.store( outVertices[firstVtxIdx + 2].m_coords );
      tmpVec.setSub( start, perpVec1 ); tmpVec.add( perpVec2 ); tmpVec.store( outVertices[firstVtxIdx + 3].m_coords );
      outFaces[firstFaceIdx + 0].idx[0] = firstVtxIdx + 0; outFaces[firstFaceIdx + 0].idx[1] = firstVtxIdx + 1; outFaces[firstFaceIdx + 0].idx[2] = firstVtxIdx + 2;
      outFaces[firstFaceIdx + 1].idx[0] = firstVtxIdx + 1; outFaces[firstFaceIdx + 1].idx[1] = firstVtxIdx + 3; outFaces[firstFaceIdx + 1].idx[2] = firstVtxIdx + 2;

      // top face
      tmpVec.setAdd( end, perpVec1 ); tmpVec.sub( perpVec2 ); tmpVec.store( outVertices[firstVtxIdx + 4].m_coords );
      tmpVec.setSub( end, perpVec1 ); tmpVec.sub( perpVec2 ); tmpVec.store( outVertices[firstVtxIdx + 5].m_coords );
      tmpVec.setAdd( end, perpVec1 ); tmpVec.add( perpVec2 ); tmpVec.store( outVertices[firstVtxIdx + 6].m_coords );
      tmpVec.setSub( end, perpVec1 ); tmpVec.add( perpVec2 ); tmpVec.store( outVertices[firstVtxIdx + 7].m_coords );
      outFaces[firstFaceIdx + 2].idx[0] = firstVtxIdx + 5; outFaces[firstFaceIdx + 2].idx[1] = firstVtxIdx + 4; outFaces[firstFaceIdx + 2].idx[2] = firstVtxIdx + 7;
      outFaces[firstFaceIdx + 3].idx[0] = firstVtxIdx + 4; outFaces[firstFaceIdx + 3].idx[1] = firstVtxIdx + 6; outFaces[firstFaceIdx + 3].idx[2] = firstVtxIdx + 7;

      // right face
      tmpVec.setSub( start, perpVec1 ); tmpVec.sub( perpVec2 ); tmpVec.store( outVertices[firstVtxIdx + 8].m_coords );
      tmpVec.setSub( start, perpVec1 ); tmpVec.add( perpVec2 ); tmpVec.store( outVertices[firstVtxIdx + 9].m_coords );
      tmpVec.setSub( end, perpVec1 ); tmpVec.sub( perpVec2 ); tmpVec.store( outVertices[firstVtxIdx + 10].m_coords );
      tmpVec.setSub( end, perpVec1 ); tmpVec.add( perpVec2 ); tmpVec.store( outVertices[firstVtxIdx + 11].m_coords );
      outFaces[firstFaceIdx + 4].idx[0] = firstVtxIdx + 8; outFaces[firstFaceIdx + 4].idx[1] = firstVtxIdx + 10; outFaces[firstFaceIdx + 4].idx[2] = firstVtxIdx + 9;
      outFaces[firstFaceIdx + 5].idx[0] = firstVtxIdx + 10; outFaces[firstFaceIdx + 5].idx[1] = firstVtxIdx + 11; outFaces[firstFaceIdx + 5].idx[2] = firstVtxIdx + 9;

      // left face
      tmpVec.setAdd( start, perpVec1 ); tmpVec.sub( perpVec2 ); tmpVec.store( outVertices[firstVtxIdx + 12].m_coords );
      tmpVec.setSub( start, perpVec1 ); tmpVec.sub( perpVec2 ); tmpVec.store( outVertices[firstVtxIdx + 13].m_coords );
      tmpVec.setAdd( end, perpVec1 ); tmpVec.sub( perpVec2 ); tmpVec.store( outVertices[firstVtxIdx + 14].m_coords );
      tmpVec.setSub( end, perpVec1 ); tmpVec.sub( perpVec2 ); tmpVec.store( outVertices[firstVtxIdx + 15].m_coords );
      outFaces[firstFaceIdx + 6].idx[0] = firstVtxIdx + 12; outFaces[firstFaceIdx + 6].idx[1] = firstVtxIdx + 14; outFaces[firstFaceIdx + 6].idx[2] = firstVtxIdx + 13;
      outFaces[firstFaceIdx + 7].idx[0] = firstVtxIdx + 14; outFaces[firstFaceIdx + 7].idx[1] = firstVtxIdx + 15; outFaces[firstFaceIdx + 7].idx[2] = firstVtxIdx + 13;

      // front face
      tmpVec.setAdd( start, perpVec1 ); tmpVec.sub( perpVec2 ); tmpVec.store( outVertices[firstVtxIdx + 16].m_coords );
      tmpVec.setAdd( start, perpVec1 ); tmpVec.add( perpVec2 ); tmpVec.store( outVertices[firstVtxIdx + 17].m_coords );
      tmpVec.setAdd( end, perpVec1 ); tmpVec.sub( perpVec2 ); tmpVec.store( outVertices[firstVtxIdx + 18].m_coords );
      tmpVec.setAdd( end, perpVec1 ); tmpVec.add( perpVec2 ); tmpVec.store( outVertices[firstVtxIdx + 19].m_coords );
      outFaces[firstFaceIdx + 8].idx[0] = firstVtxIdx + 17; outFaces[firstFaceIdx + 8].idx[1] = firstVtxIdx + 19; outFaces[firstFaceIdx + 8].idx[2] = firstVtxIdx + 16;
      outFaces[firstFaceIdx + 9].idx[0] = firstVtxIdx + 19; outFaces[firstFaceIdx + 9].idx[1] = firstVtxIdx + 18; outFaces[firstFaceIdx + 9].idx[2] = firstVtxIdx + 16;

      // back face
      tmpVec.setAdd( start, perpVec1 ); tmpVec.add( perpVec2 ); tmpVec.store( outVertices[firstVtxIdx + 20].m_coords );
      tmpVec.setSub( start, perpVec1 ); tmpVec.add( perpVec2 ); tmpVec.store( outVertices[firstVtxIdx + 21].m_coords );
      tmpVec.setAdd( end, perpVec1 ); tmpVec.add( perpVec2 ); tmpVec.store( outVertices[firstVtxIdx + 22].m_coords );
      tmpVec.setSub( end, perpVec1 ); tmpVec.add( perpVec2 ); tmpVec.store( outVertices[firstVtxIdx + 23].m_coords );
      outFaces[firstFaceIdx + 10].idx[0] = firstVtxIdx + 21; outFaces[firstFaceIdx + 10].idx[1] = firstVtxIdx + 23; outFaces[firstFaceIdx + 10].idx[2] = firstVtxIdx + 20;
      outFaces[firstFaceIdx + 11].idx[0] = firstVtxIdx + 23; outFaces[firstFaceIdx + 11].idx[1] = firstVtxIdx + 22; outFaces[firstFaceIdx + 11].idx[2] = firstVtxIdx + 20;
   }
}

///////////////////////////////////////////////////////////////////////////////

TriangleMesh* DebugGeometryBuilder::createTorus( float innerRadius, float outerRadius, const Matrix& transform, int segmentsCount, int segmentVerticesCount )
{
   const Vector& origin = transform.position();
   const Vector& mainAxis = transform.forwardVec();
   const Vector& sideAxis = transform.sideVec();
   Vector circumferenceAxis = transform.upVec();

   FastFloat circumferenceWidth; circumferenceWidth.setFromFloat( outerRadius - innerRadius );
   FastFloat radius; radius.setFromFloat( innerRadius + outerRadius - innerRadius );

   // calculate torus vertices
   const uint verticesCount = segmentsCount * segmentVerticesCount;
   Array<LitVertex> vertices( verticesCount );
   vertices.resizeWithoutInitializing( verticesCount );
   {
      FastFloat dMainAngle; dMainAngle.setFromFloat( DEG2RAD( 360.0f / (float)segmentsCount ) );
      FastFloat dSegmentAngle; dSegmentAngle.setFromFloat( DEG2RAD( 360.0f / (float)segmentVerticesCount ) );
      FastFloat mainAngle = Float_0;

      Quaternion mainRot, circumferenceRot;
      Vector vtxPos, posOnCircumference, radiusDisplacement, radiusVec, torusVtx;
      radiusVec.setMul( sideAxis, radius );
      uint vtxIdx = 0;
      for ( uint segmentIdx = 0; segmentIdx < segmentsCount; ++segmentIdx, mainAngle.add( dMainAngle ) )
      {
         mainRot.setAxisAngle( mainAxis, mainAngle );
         mainRot.transform( radiusVec, radiusDisplacement );

         FastFloat segmentAngle = Float_0;
         for ( uint segVtxIdx = 0; segVtxIdx < segmentVerticesCount; ++segVtxIdx, segmentAngle.add( dSegmentAngle ) )
         {
            circumferenceRot.setAxisAngle( circumferenceAxis, segmentAngle );
            Quaternion rotQ;
            rotQ.setMul( circumferenceRot, mainRot );

            // first - create a point on the circumference of the toruses' segment
            vtxPos.setMul( sideAxis, circumferenceWidth );
            rotQ.transform( vtxPos, posOnCircumference );

            // and displace the circumference point so that it ends up in its final position on the toruses' circumference
            torusVtx.setAdd( radiusDisplacement, posOnCircumference );
            torusVtx.add( origin );

            // store the vertex
            torusVtx.store( vertices[vtxIdx].m_coords );
            ++vtxIdx;
         }
      }
   }

   // set torus indices
   const uint facesCount = verticesCount * 2;
   Array<Face> faces( facesCount );
   faces.resizeWithoutInitializing( facesCount );
   {
      uint faceIdx = 0;
      for ( uint segmentIdx = 0; segmentIdx < segmentsCount; ++segmentIdx )
      {
         uint currSegmentFirstVtx = segmentIdx * segmentVerticesCount;
         uint nextSegmentFirstVtx = ( currSegmentFirstVtx + segmentVerticesCount ) % verticesCount;

         for ( uint segVtxIdx = 0; segVtxIdx < segmentVerticesCount; ++segVtxIdx )
         {
            uint skipOffset = 0;
            if ( segVtxIdx == 0 )
            {
               skipOffset = segmentVerticesCount;
            }

            faces[faceIdx].idx[0] = currSegmentFirstVtx + segVtxIdx - 1 + skipOffset;
            faces[faceIdx].idx[1] = currSegmentFirstVtx + segVtxIdx;
            faces[faceIdx].idx[2] = nextSegmentFirstVtx + segVtxIdx - 1 + skipOffset;
            ++faceIdx;

            faces[faceIdx].idx[0] = currSegmentFirstVtx + segVtxIdx;
            faces[faceIdx].idx[1] = nextSegmentFirstVtx + segVtxIdx;
            faces[faceIdx].idx[2] = nextSegmentFirstVtx + segVtxIdx - 1 + skipOffset;
            ++faceIdx;
         }
      }
   }

   TriangleMesh* mesh = new TriangleMesh( FilePath(), vertices, faces );
   return mesh;
}

///////////////////////////////////////////////////////////////////////////////

TriangleMesh* DebugGeometryBuilder::createBone()
{
   Array<LitVertex> vertices( 48 );
   vertices.resizeWithoutInitializing( 48 );
   {
      Vector tmpVec;

      float y[2][3] = { { 0.0f, 0.05f, 0.1f }, { 0.1f, 0.3f, 1.0f } };
      float r[2] = { 0.05f, 0.1f };     

      for ( int i = 0; i < 2; ++i )
      {
         int startIdx = i * 24;
         float y0 = y[i][0];
         float y1 = y[i][1];
         float y2 = y[i][2];
         float r0 = r[i];

         tmpVec.set(  0.0f,  y0,  0.0f ); tmpVec.store( vertices[startIdx + 0].m_coords );
         tmpVec.set(  0.0f,  y1,   -r0 ); tmpVec.store( vertices[startIdx + 1].m_coords );
         tmpVec.set(    r0,  y1,  0.0f ); tmpVec.store( vertices[startIdx + 2].m_coords );

         tmpVec.set(  0.0f,  y0,  0.0f ); tmpVec.store( vertices[startIdx + 3].m_coords );
         tmpVec.set(  -r0,   y1,  0.0f ); tmpVec.store( vertices[startIdx + 4].m_coords );
         tmpVec.set(  0.0f,  y1,   -r0 ); tmpVec.store( vertices[startIdx + 5].m_coords );

         tmpVec.set(  0.0f,  y0,  0.0f ); tmpVec.store( vertices[startIdx + 6].m_coords );
         tmpVec.set(  0.0f,  y1,    r0 ); tmpVec.store( vertices[startIdx + 7].m_coords );
         tmpVec.set(   -r0,  y1,  0.0f ); tmpVec.store( vertices[startIdx + 8].m_coords );

         tmpVec.set(  0.0f,  y0,  0.0f ); tmpVec.store( vertices[startIdx + 9].m_coords );
         tmpVec.set(    r0,  y1,  0.0f ); tmpVec.store( vertices[startIdx + 10].m_coords );
         tmpVec.set(  0.0f,  y1,    r0 ); tmpVec.store( vertices[startIdx + 11].m_coords );

         tmpVec.set(    r0,  y1,  0.0f ); tmpVec.store( vertices[startIdx + 12].m_coords );
         tmpVec.set(  0.0f,  y1,   -r0 ); tmpVec.store( vertices[startIdx + 13].m_coords );
         tmpVec.set(  0.0f,  y2,  0.0f ); tmpVec.store( vertices[startIdx + 14].m_coords );

         tmpVec.set(  0.0f,  y1,   -r0 ); tmpVec.store( vertices[startIdx + 15].m_coords );
         tmpVec.set(   -r0,  y1,  0.0f ); tmpVec.store( vertices[startIdx + 16].m_coords );
         tmpVec.set(  0.0f,  y2,  0.0f ); tmpVec.store( vertices[startIdx + 17].m_coords );

         tmpVec.set(   -r0,  y1,  0.0f ); tmpVec.store( vertices[startIdx + 18].m_coords );
         tmpVec.set(  0.0f,  y1,    r0 ); tmpVec.store( vertices[startIdx + 19].m_coords );
         tmpVec.set(  0.0f,  y2,  0.0f ); tmpVec.store( vertices[startIdx + 20].m_coords );

         tmpVec.set(  0.0f,  y1,    r0 ); tmpVec.store( vertices[startIdx + 21].m_coords );
         tmpVec.set(    r0,  y1,  0.0f ); tmpVec.store( vertices[startIdx + 22].m_coords );
         tmpVec.set(  0.0f,  y2,  0.0f ); tmpVec.store( vertices[startIdx + 23].m_coords );
      }
   }

   Array<Face> faces( 16 );
   faces.resizeWithoutInitializing( 16 );
   uint vtxIdx = 0;
   for ( uint i = 0; i < 16; ++i )
   {
      Face& face = faces[i];
      face.idx[0] = vtxIdx++;
      face.idx[1] = vtxIdx++;
      face.idx[2] = vtxIdx++;
   }

   MeshUtils::calculateFlatMeshNormals( faces, vertices );

   TriangleMesh* mesh = new TriangleMesh( FilePath(), vertices, faces );
   return mesh;
}

///////////////////////////////////////////////////////////////////////////////

TriangleMesh* DebugGeometryBuilder::createPlane( float width, float height, float horizTextureSize, float vertTextureSize )
{
   Array<LitVertex> vertices( 4 );
   vertices.resizeWithoutInitializing( 4 );
   {
      const float hw = width * 0.5f;
      const float hh = height * 0.5f;

      const float tx = width / horizTextureSize;
      const float ty = height/ vertTextureSize;

      Vector tmpVec;
      tmpVec.set( -hw,  hh, 0.0f ); tmpVec.store( vertices[0].m_coords );
      tmpVec.set(  hw,  hh, 0.0f ); tmpVec.store( vertices[1].m_coords );
      tmpVec.set( -hw, -hh, 0.0f ); tmpVec.store( vertices[2].m_coords );
      tmpVec.set(  hw, -hh, 0.0f ); tmpVec.store( vertices[3].m_coords );

      tmpVec.set( -tx,  ty, 0.0f ); tmpVec.store( vertices[0].m_textureCoords );
      tmpVec.set(  tx,  ty, 0.0f ); tmpVec.store( vertices[1].m_textureCoords );
      tmpVec.set( -tx, -ty, 0.0f ); tmpVec.store( vertices[2].m_textureCoords );
      tmpVec.set(  tx, -ty, 0.0f ); tmpVec.store( vertices[3].m_textureCoords );
   }

   Array<Face> faces( 2 );
   faces.resizeWithoutInitializing( 2 );
   {
      faces[0].idx[0] = 0;
      faces[0].idx[1] = 2;
      faces[0].idx[2] = 1;

      faces[1].idx[0] = 2;
      faces[1].idx[1] = 3;
      faces[1].idx[2] = 1;
   }

   MeshUtils::calculateFlatMeshNormals( faces, vertices );
   TriangleMesh* mesh = new TriangleMesh( FilePath(), vertices, faces );
   return mesh;
}

///////////////////////////////////////////////////////////////////////////////

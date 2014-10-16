#include "ext-2DGameLevel\GL2DLineSegmentsShape.h"
#include "ext-2DGameLevel\GL2DCollisionAlgorithms.h"
#include "ext-2DGameLevel\GL2DDirectedLineSegment.h"
#include "core\Assert.h"
#include "core\Transform.h"
#include "core\LocalList.h"
#include "core\IntersectionResult.h"
#include "core\PolygonMesh2D.h"
#include "core\MeshAdjacencyMap.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( GL2DLineSegmentsShape );
   PARENT( GL2DShape );
   PROPERTY( Array< GL2DDirectedLineSegment >, m_segments );
   PROPERTY( GL2DSegmentsTreeNode*, m_rootNode );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

GL2DLineSegmentsShape::GL2DLineSegmentsShape()
: m_rootNode( NULL )
{
   m_rootNode = new GL2DSegmentsTreeNode( );
}

///////////////////////////////////////////////////////////////////////////////

GL2DLineSegmentsShape::GL2DLineSegmentsShape( const GL2DLineSegmentsShape& rhs )
   : GL2DShape( rhs )
   , m_segments( rhs.m_segments )
{
   if ( rhs.m_rootNode )
   {
      m_rootNode = rhs.m_rootNode->getVirtualRTTI( ).instantiate( *rhs.m_rootNode );
   }
}

///////////////////////////////////////////////////////////////////////////////

GL2DLineSegmentsShape::~GL2DLineSegmentsShape( )
{
   if ( m_rootNode )
   {
      m_rootNode->removeReference();
      m_rootNode = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DLineSegmentsShape::addRect( const Transform& transform, const Vector& halfExtents )
{
   // create corners of the rectangle
   Vector corners[4];
   corners[0].setMul( halfExtents, Float_Minus1 );
   corners[2] = halfExtents;
   corners[1].setSelect< VectorComparison::MASK_Y >( corners[0], corners[2] );
   corners[3].setSelect< VectorComparison::MASK_X >( corners[0], corners[2] );

   for ( byte i = 0; i < 4; ++i )
   {
      transform.transform( corners[i], corners[i] );
   }

   // create line segments
   addSegment( corners[0], corners[3] );
   addSegment( corners[3], corners[2] );
   addSegment( corners[2], corners[1] );
   addSegment( corners[1], corners[0] );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DLineSegmentsShape::addSegment( const Vector& start, const Vector& end )
{
   m_segments.push_back( GL2DDirectedLineSegment( start, end ) );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DLineSegmentsShape::setFromPolygonSoup( const Vector* arrVertices, const uint verticesCount, const word* arrIndices, const uint trianglesCount )
{
   // clear the previous implementation
   m_segments.clear();
   m_rootNode->removeReference();
   m_rootNode = new GL2DSegmentsTreeNode();

   // build the new one
   PolygonMergeRules mergeRules;
   mergeRules.m_angularDifference.setFromFloat( DEG2RAD( 60.0f ) );
   mergeRules.m_vertexWeldDistance = Float_1e_1;

   PolygonMesh2D polygon2D( Vector_OZ );
   polygon2D.addTriangleSoup( arrVertices, ( word* ) arrIndices, trianglesCount, mergeRules );

   MeshAdjacencyMap adjacencyMap;
   polygon2D.generateAdjacencyMap( adjacencyMap );

   List< Vector > boundaryEdge;
   polygon2D.getBoundaryEdge( adjacencyMap, boundaryEdge );

   for ( List< Vector >::iterator it = boundaryEdge.begin(); !it.isEnd(); )
   {
      const Vector& startVtx = *it; ++it;
      const Vector& endVtx = *it; ++it;
      addSegment( startVtx, endVtx );
   }

   build();
}

///////////////////////////////////////////////////////////////////////////////

void GL2DLineSegmentsShape::build()
{
   // clean up the old representation
   {
      if ( m_rootNode )
      {
         m_rootNode->removeReference();
         m_rootNode = NULL;
      }
   }

   // create the root node
   {
      m_rootNode = new GL2DSegmentsTreeNode();

      // calculate the bounding box around the geometry
      const uint segmentsCount = m_segments.size( );
      for ( uint segIdx = 0; segIdx < segmentsCount; ++segIdx )
      {
         const GL2DDirectedLineSegment& seg = m_segments[segIdx];

         m_rootNode->m_bounds.include( seg.m_start );
         m_rootNode->m_bounds.include( seg.m_end );
      }
   }
   

   // analyze all segments that were added to the shape and create a tree of nodes
   {
      LocalList< GL2DSegmentsTreeNode* > nodes;

      AxisAlignedBox addedSegmentBounds;
      const uint segmentsCount = m_segments.size();
      for ( uint segIdx = 0; segIdx < segmentsCount; ++segIdx )
      {
         const GL2DDirectedLineSegment& seg = m_segments[segIdx];

         addedSegmentBounds.include( seg.m_start );
         addedSegmentBounds.include( seg.m_end );

         nodes.pushBack( m_rootNode );
         while ( !nodes.empty() )
         {
            GL2DSegmentsTreeNode* analyzedNode = nodes.front();
            nodes.popFront();

            if ( !addedSegmentBounds.testIntersection( analyzedNode->m_bounds ) )
            {
               continue;
            }

            // is this the leaf segment, or does it have children?
            if ( analyzedNode->m_count == 0 )
            {
               // it's got children - analyze them
               for ( GL2DSegmentsTreeNode* childNode = analyzedNode->m_child; childNode != NULL; childNode = childNode->m_sibling )
               {
                  nodes.pushBack( childNode );
               }
               continue;
            }


            if ( analyzedNode->m_count >= GL2D_MAX_SEGMENTS_PER_NODE && analyzedNode->calcArea() > GL2D_MIN_ALLOWED_NODE_AREA )
            {
               // this child is full and it's still large enough so that we can subdivide it
               analyzedNode->subdivide();

               // add the children for analysis
               for ( GL2DSegmentsTreeNode* childNode = analyzedNode->m_child; childNode != NULL; childNode = childNode->m_sibling )
               {
                  nodes.pushBack( childNode );
               }
               continue;
            }

            // it's a leaf node that can incorporate more segments
            ++analyzedNode->m_count;
         }
      }
   }

   // We used node counters to indicate when a node needs to be split, so we need to reset them now.
   // While we're at it - collect all leaf nodes. We'll use them in the next step to populate the tree with segments
   LocalList< GL2DSegmentsTreeNode* > leafNodes;
   {
      LocalList< GL2DSegmentsTreeNode* > nodes;
      nodes.pushBack( m_rootNode );
      
      while ( !nodes.empty( ) )
      {
         GL2DSegmentsTreeNode* analyzedNode = nodes.front( );
         nodes.popFront( );

         // is this the leaf segment, or does it have children?
         for ( GL2DSegmentsTreeNode* childNode = analyzedNode->m_child; childNode != NULL; childNode = childNode->m_sibling )
         {
            nodes.pushBack( childNode );
         }
         
         analyzedNode->m_count = 0;
         if ( analyzedNode->m_child == NULL )
         {
            leafNodes.pushBack( analyzedNode );
         }
      }
   }

   // this is almost the final pass - we need to see how many segments each node will hold so that we can
   // distribute start indices correctly
   {
      AxisAlignedBox addedSegmentBounds;
      const uint segmentsCount = m_segments.size();
      for ( uint segIdx = 0; segIdx < segmentsCount; ++segIdx )
      {
         const GL2DDirectedLineSegment& seg = m_segments[segIdx];

         addedSegmentBounds.include( seg.m_start );
         addedSegmentBounds.include( seg.m_end );

         for ( LocalList< GL2DSegmentsTreeNode* >::iterator it = leafNodes.begin(); !it.isEnd(); ++it )
         {
            GL2DSegmentsTreeNode* node = *it;
            if ( addedSegmentBounds.testIntersection( node->m_bounds ) )
            {
               ++node->m_count;
            }
         }
      }
   }

   // assign start indices
   int entriesCount = 0;
   for ( LocalList< GL2DSegmentsTreeNode* >::iterator it = leafNodes.begin(); !it.isEnd(); ++it )
   {
      GL2DSegmentsTreeNode* segment = *it;
      segment->m_startIdx = entriesCount;
      entriesCount += segment->m_count;
   }

   // And finally - sort the segments. For every leaf node, run through all segments and see which one overlaps with it ( just as in 
   // one of the previous step) and assign it to the sorted segments array at the correct spot 
   {
      Array< GL2DDirectedLineSegment > sortedSegments( entriesCount );
      sortedSegments.resizeWithoutInitializing( entriesCount );

      uint insertionIdx = 0;

      AxisAlignedBox addedSegmentBounds;
      const uint segmentsCount = m_segments.size();
      for ( LocalList< GL2DSegmentsTreeNode* >::iterator it = leafNodes.begin(); !it.isEnd(); ++it )
      {
         GL2DSegmentsTreeNode* node = *it;

         for ( uint segIdx = 0; segIdx < segmentsCount; ++segIdx )
         {
            const GL2DDirectedLineSegment& seg = m_segments[segIdx];
            addedSegmentBounds.include( seg.m_start );
            addedSegmentBounds.include( seg.m_end );

            if ( addedSegmentBounds.testIntersection( node->m_bounds ) )
            {
               ASSERT( insertionIdx < node->m_startIdx + node->m_count );
               sortedSegments[insertionIdx] = seg;
               ++insertionIdx;
            }
         }
      }

      // and swap the arrays
      m_segments = sortedSegments;
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DLineSegmentsShape::getSegments( const Sphere& boundingSphere, Array< const GL2DDirectedLineSegment* >& outSegments ) const
{
   LocalList< GL2DSegmentsTreeNode* > nodes;
   nodes.pushBack( m_rootNode );

   IntersectionResult tmpIntersectionResult;
   while ( !nodes.empty() )
   {
      GL2DSegmentsTreeNode* analyzedNode = nodes.front();
      nodes.popFront();

      if ( !boundingSphere.testIntersection( analyzedNode->m_bounds, tmpIntersectionResult ) )
      {
         continue;
      }

      // is this the leaf segment, or does it have children?
      if ( analyzedNode->m_child != NULL )
      {
         // it's got children - analyze them
         for ( GL2DSegmentsTreeNode* childNode = analyzedNode->m_child; childNode != NULL; childNode = childNode->m_sibling )
         {
            nodes.pushBack( childNode );
         }
      }
      else
      {
         // it's a leaf that we're interested in - collect the segments it contains
         uint lastSegIdx = analyzedNode->m_startIdx + analyzedNode->m_count;
         for ( uint i = analyzedNode->m_startIdx; i < lastSegIdx; ++i )
         {
            const GL2DDirectedLineSegment* seg = &m_segments[i];
            outSegments.push_back( seg );
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

uint GL2DLineSegmentsShape::detectNarrowphaseCollision( const GL2DShape* otherShape, const Matrix& thisTransform, const Matrix& otherBodyTransform, IntersectionResult* outArrResults ) const
{
   return otherShape->collideAgainstStaticGeometry( this, otherBodyTransform, thisTransform, outArrResults );
}

///////////////////////////////////////////////////////////////////////////////

uint GL2DLineSegmentsShape::collideAgainstStaticGeometry( const GL2DLineSegmentsShape* otherShape, const Matrix& thisTransform, const Matrix& otherBodyTransform, IntersectionResult* outArrResults ) const
{
   ASSERT_MSG( false, "This call should never take place - static geometry can't be tested against other static geometry" );
   return false;
}

///////////////////////////////////////////////////////////////////////////////

uint GL2DLineSegmentsShape::collideAgainstCircle( const GL2DCircleShape* otherShape, const Matrix& thisTransform, const Matrix& otherBodyTransform, IntersectionResult* outArrResults ) const
{
   return GL2DCollisionAlgorithms::circleVsLineSegments( *otherShape, *this, otherBodyTransform, outArrResults );
}

///////////////////////////////////////////////////////////////////////////////

uint GL2DLineSegmentsShape::collideAgainstBox( const GL2DBoxShape* otherShape, const Matrix& thisTransform, const Matrix& otherBodyTransform, IntersectionResult* outArrResults ) const
{
   // only circular shapes collide with box shapes, so we automatically ignore this type of collision
   return false;
}

///////////////////////////////////////////////////////////////////////////////

bool GL2DLineSegmentsShape::rayCast( const Ray& ray, const Matrix& transform, RaycastResult& outResult ) const
{
   return false;
}

///////////////////////////////////////////////////////////////////////////////

void GL2DLineSegmentsShape::getBoundingSphere( Sphere& outSphere ) const
{
   outSphere.fromAxisAlignedBox( m_rootNode->m_bounds );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( GL2DSegmentsTreeNode );
   PARENT( ReflectionObject );
   PROPERTY( uint, m_startIdx );
   PROPERTY( uint, m_count );
   PROPERTY( AxisAlignedBox, m_bounds );
   PROPERTY( GL2DSegmentsTreeNode*, m_child );
   PROPERTY( GL2DSegmentsTreeNode*, m_sibling );
END_OBJECT( );

///////////////////////////////////////////////////////////////////////////////

GL2DSegmentsTreeNode::GL2DSegmentsTreeNode( )
   : m_startIdx( 0 )
   , m_count( 0 )
   , m_child( NULL )
   , m_sibling( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

GL2DSegmentsTreeNode::GL2DSegmentsTreeNode( const GL2DSegmentsTreeNode& rhs )
   : ReflectionObject( rhs )
   , m_startIdx( rhs.m_startIdx )
   , m_count( rhs.m_count )
   , m_bounds( rhs.m_bounds )
   , m_child( NULL )
   , m_sibling( NULL )
{
   if ( rhs.m_child )
   {
      m_child = rhs.m_child->getVirtualRTTI( ).instantiate( *rhs.m_child );
   }

   if ( rhs.m_sibling )
   {
      m_sibling = rhs.m_sibling->getVirtualRTTI( ).instantiate( *rhs.m_sibling );
   }
}

///////////////////////////////////////////////////////////////////////////////

GL2DSegmentsTreeNode::~GL2DSegmentsTreeNode( )
{
   if ( m_sibling )
   {
      m_sibling->removeReference( );
      m_sibling = NULL;
   }

   if ( m_child )
   {
      m_child->removeReference();
      m_child = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

const FastFloat GL2DSegmentsTreeNode::calcArea( ) const
{
   Vector extents;
   m_bounds.getExtents( extents );
   
   extents.setSelect< VectorComparison::MASK_XY >( extents, Vector_ZERO );
   const FastFloat area = extents.dot( extents );
   return area;
}

///////////////////////////////////////////////////////////////////////////////

void GL2DSegmentsTreeNode::subdivide()
{
   ASSERT( m_child == NULL );
   
   // create new nodes
   const uint newCount = m_count / 4;

   AxisAlignedBox* bounds[4];
   GL2DSegmentsTreeNode** ptrNode = &m_child;
   for ( int i = 0; i < 4; ++i )
   {
      GL2DSegmentsTreeNode* childNode = new GL2DSegmentsTreeNode();
      childNode->m_count = newCount;
      *ptrNode = childNode;
      ptrNode = &childNode->m_sibling;

      bounds[i] = &childNode->m_bounds;
   }

   // calculate bounding boxes
   Vector divisionCenter;
   m_bounds.getCenter( divisionCenter );

   /*
   Division scheme:

        2  |  3
      -----------
        0  |  1
   */

   bounds[0]->min = m_bounds.min;
   bounds[0]->max = divisionCenter;

   bounds[1]->min.setSelect< VectorComparison::MASK_X >( divisionCenter, m_bounds.min );
   bounds[1]->max.setSelect< VectorComparison::MASK_Y >( divisionCenter, m_bounds.max );

   bounds[2]->min.setSelect< VectorComparison::MASK_Y >( divisionCenter, m_bounds.min );
   bounds[2]->max.setSelect< VectorComparison::MASK_X >( divisionCenter, m_bounds.max );

   bounds[3]->min = divisionCenter;
   bounds[3]->max = m_bounds.max;
}

///////////////////////////////////////////////////////////////////////////////

#include "core-Renderer\StaticGeometryTree.h"

// math
#include "core\Matrix.h"
#include "core\AxisAlignedBox.h"

// resources
#include "core\FilePath.h"
#include "core\ResourcesManager.h"

// scene
#include "core-MVC\Entity.h"
#include "core-MVC\EntityUtils.h"
#include "core-MVC\Prefab.h"
#include "core-Renderer\GeometryComponent.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\TriangleMesh.h"

// utils
#include "core\LocalList.h"


///////////////////////////////////////////////////////////////////////////////

#define MAX_VERTICES_PER_NODE    0xffff      // we can only have as many vertices per node, 'cause we're using the 16-bit face indices

///////////////////////////////////////////////////////////////////////////////

StaticGeometryTree::MaterialEntry::MaterialEntry( uint entryIdx, Material* material, const AxisAlignedBox& bounds )
: m_entryIdx( entryIdx )
, m_root( new StaticGeometryNode( bounds ) )
, m_material( material )
{}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

StaticGeometryTree::StaticGeometryTree( const AxisAlignedBox& sceneBounds )
   : m_sceneBounds( sceneBounds )
{
}

///////////////////////////////////////////////////////////////////////////////

StaticGeometryTree::~StaticGeometryTree()
{
   for ( List< MaterialEntry* >::iterator it = m_materialEntries.begin(); !it.isEnd(); ++it )
   {
      MaterialEntry* entry = *it;

      // delete the tree
      LocalList< StaticGeometryNode* > nodes;
      nodes.pushBack( entry->m_root );

      while ( !nodes.empty() )
      {
         StaticGeometryNode* node = nodes.front();
         nodes.popFront();

         const uint childrenCount = node->m_children.size();
         for ( uint i = 0; i < childrenCount; ++i )
         {
            nodes.pushBack( node->m_children[i] );
         }

         delete node;
      }

      // delete the entry
      delete entry;
   }
   m_materialEntries.clear();
}

///////////////////////////////////////////////////////////////////////////////

void StaticGeometryTree::add( const Prefab* prefab, const Matrix& transform )
{
   if ( !prefab )
   {
      return;
   }

   List< const GeometryComponent* > components;
   EntityUtils::collectNodesByType< GeometryComponent >( prefab->getEntity(), components );

   for ( List< const GeometryComponent* >::iterator it = components.begin(); !it.isEnd(); ++it )
   {
      add( *it, transform );
   }
}

///////////////////////////////////////////////////////////////////////////////

void StaticGeometryTree::add( const GeometryComponent* geometry, const Matrix& transform )
{
   // find an entry with the same material
   MaterialEntry* expandedEntry = NULL;
   Material* mat = geometry->getRenderState()->m_material;
   for ( List< MaterialEntry* >::iterator it = m_materialEntries.begin(); !it.isEnd(); ++it )
   {
      MaterialEntry* entry = *it;
      if ( entry->m_material == mat )
      {
         expandedEntry = entry;
         break;
      }
   }

   if ( !expandedEntry )
   {
      // we need to create a new entry
      expandedEntry = new MaterialEntry( m_materialEntries.size(), mat, m_sceneBounds );
      m_materialEntries.pushBack( expandedEntry );
   }

   // find a node that can accommodate the geometry
   StaticGeometryNode* node = expandedEntry->m_root;
   uint depth = 0;
   while ( node != NULL )
   {
      node = node->add( geometry, transform );
      ++depth;
   }
}

///////////////////////////////////////////////////////////////////////////////

Entity* StaticGeometryTree::build( const FilePath& geometryDir, const char* entityName )
{
   Entity* assembledEntity = new Entity( entityName );

   for ( List< MaterialEntry* >::iterator it = m_materialEntries.begin(); !it.isEnd(); ++it )
   {
      MaterialEntry* entry = *it;
      buildSingleMaterial( geometryDir, entityName, entry, assembledEntity );
   }

   return assembledEntity;
}

///////////////////////////////////////////////////////////////////////////////

void StaticGeometryTree::buildSingleMaterial( const FilePath& geometryDir, const char* entityName, MaterialEntry* entry, Entity* rootEntity )
{
   LocalList< StaticGeometryNode* > nodes;
   nodes.pushBack( entry->m_root );

   uint nodeIdx = 0;
   while ( !nodes.empty() )
   {
      StaticGeometryNode* node = nodes.front();
      nodes.popFront();

      // define a unique file path for the mesh
      char tmpFilePath[1024];
      sprintf_s( tmpFilePath, "%s/%s_chunk%d_%d.%s", geometryDir.c_str(), entityName, entry->m_entryIdx, nodeIdx, TriangleMesh::getExtension() );
      ++nodeIdx;

      GeometryComponent* newComp = node->build( FilePath( tmpFilePath ), entry->m_material );
      if ( newComp )
      {
         rootEntity->addChild( newComp );
      }

      const uint childrenCount = node->m_children.size();
      for ( uint i = 0; i < childrenCount; ++i )
      {
         nodes.pushBack( node->m_children[i] );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

StaticGeometryNode::StaticGeometryNode( const AxisAlignedBox& nodeBounds )
   : m_nodeBounds( nodeBounds )
   , m_currVerticesCount( 0 )
   , m_isLeafNode( true )
{
}

///////////////////////////////////////////////////////////////////////////////

StaticGeometryNode::~StaticGeometryNode()
{
   for ( List< Entry* >::iterator it = m_geometry.begin(); !it.isEnd(); ++it )
   {
      Entry* entry = *it;
      delete entry;
   }
   m_geometry.clear();
}

///////////////////////////////////////////////////////////////////////////////

StaticGeometryNode* StaticGeometryNode::add( const GeometryComponent* geometry, const Matrix& transform )
{
   TriangleMesh* mesh = static_cast< TriangleMesh* >( geometry->getMesh() );
   const uint verticesCount = mesh->getVertices().size();

   if ( ( m_currVerticesCount + verticesCount ) > MAX_VERTICES_PER_NODE )
   {
      // subdivide the node
      subdivide();
   }

   if ( !m_isLeafNode )
   {
      // this is a composite node that doesn't store any geometry - look for a leaf node
      // that can accommodate the node
      AxisAlignedBox geometryBoundsWorldSpace;
      mesh->getBoundingVolume().transform( transform, geometryBoundsWorldSpace );
      StaticGeometryNode* childNode = findNodeFor( geometryBoundsWorldSpace );
      return childNode;
   }

   // if we got this far, it means that this is the node to add our geometry to
   addEntry( new Entry( geometry, transform ) );
   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

void StaticGeometryNode::subdivide()
{
   if ( !m_isLeafNode )
   {
      ASSERT( false );  // this should never take place - the logics in StaticGeometryNode::add method
                        // should prevent a call to this method from being made on a node that's a composite node
      return;
   }

   // create the children nodes
   m_children.resize( 4, NULL );

   // initialize them, splitting the area of this node into 4 equal parts
   Vector centerPoint;
   m_nodeBounds.getCenter( centerPoint );

   Vector p1, p2, p3, p4;
   p1.setSelect< VectorComparison::MASK_YZW >( m_nodeBounds.min, centerPoint );
   p2.setSelect< VectorComparison::MASK_XZW >( m_nodeBounds.max, centerPoint );
   p3.setSelect< VectorComparison::MASK_XZW >( m_nodeBounds.min, centerPoint );
   p4.setSelect< VectorComparison::MASK_YZW >( m_nodeBounds.max, centerPoint );

   m_children[0] = new StaticGeometryNode( AxisAlignedBox( m_nodeBounds.min, centerPoint ) );
   m_children[1] = new StaticGeometryNode( AxisAlignedBox( p1, p2 ) );
   m_children[2] = new StaticGeometryNode( AxisAlignedBox( centerPoint, m_nodeBounds.max ) );
   m_children[3] = new StaticGeometryNode( AxisAlignedBox( p3, p4 ) );

   // adjust the Z coords
   for ( byte i = 0; i < 4; ++i )
   {
      m_children[i]->m_nodeBounds.min[2] = 0.f;
      m_children[i]->m_nodeBounds.max[2] = 1.f;
   }

   // this is no longer a leaf node
   m_isLeafNode = false;
   m_currVerticesCount = 0;

   // distribute the geometry amongst the children
   AxisAlignedBox geometryBoundsWorldSpace;
   for ( List< Entry* >::iterator it = m_geometry.begin(); !it.isEnd(); ++it )
   {
      Entry* entry = *it;

      TriangleMesh* mesh = static_cast< TriangleMesh* >( entry->m_geometry->getMesh() );
      mesh->getBoundingVolume().transform( entry->m_transform, geometryBoundsWorldSpace );

      StaticGeometryNode* childNode = findNodeFor( geometryBoundsWorldSpace );
      childNode->addEntry( entry );
   }
   m_geometry.clear();
}

///////////////////////////////////////////////////////////////////////////////

void StaticGeometryNode::addEntry( Entry* entry )
{
   ASSERT( entry );
   ASSERT( m_isLeafNode ); // the call can only be made on a node that's a leaf node

   m_geometry.pushBack( entry );

   // update the vertices counter
   TriangleMesh* mesh = static_cast< TriangleMesh* >( entry->m_geometry->getMesh() );
   m_currVerticesCount += mesh->getVertices().size();

   ASSERT_MSG( m_currVerticesCount < MAX_VERTICES_PER_NODE, "Maximum number of vertices per node has been exceeded" );
}

///////////////////////////////////////////////////////////////////////////////

StaticGeometryNode* StaticGeometryNode::findNodeFor( const AxisAlignedBox& boundsWorldSpace )
{
   if ( m_isLeafNode )
   {
      ASSERT( false );  // this should never take place - the logics in StaticGeometryNode::add method
                        // should prevent a call to this method from being made on a node that's a leaf node
      return this;
   }

   const byte childrenCount = (byte)m_children.size();
   AxisAlignedBox unionOfBounds;
   float largestArea = 0.f;
   StaticGeometryNode* bestCandidate = NULL;
   for ( byte i = 0; i < childrenCount; ++i )
   {
      StaticGeometryNode* child = m_children[i];

      child->m_nodeBounds.intersection( boundsWorldSpace, unionOfBounds );
      float boundsArea = unionOfBounds.calcArea();
      if ( largestArea < boundsArea )
      {
         largestArea = boundsArea;
         bestCandidate = child;
      }
   }

   return bestCandidate;
}

///////////////////////////////////////////////////////////////////////////////

GeometryComponent* StaticGeometryNode::build( const FilePath& meshFilePath, Material* material )
{
   if ( !m_isLeafNode )
   {
      // this is a composite node, so there's no geometry present here
      return NULL;
   }

   // create a new component
   GeometryComponent* assembledGeometryComp = new GeometryComponent( "GeometryComponent" );

   // create a mesh
   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
   TriangleMesh* assembledTriangleMesh = resMgr.create< TriangleMesh >( meshFilePath, false );
   assembledTriangleMesh->clear(); // clear the mesh in case it already contained something

   // set the material it will be using
   assembledGeometryComp->accessRenderState()->setMaterial( material );

   // start merging the geometry together 
   for ( List< Entry* >::iterator it = m_geometry.begin(); !it.isEnd(); ++it )
   {
      Entry* entry = *it;

      TriangleMesh* chunkMesh = static_cast< TriangleMesh* >( entry->m_geometry->getMesh() );
      assembledTriangleMesh->merge( *chunkMesh, entry->m_transform );
   }

   // Embed the mesh in the component. We need to do it after the mesh is assembled, so that
   // the geometry bounds could be updated correctly
   assembledGeometryComp->setMesh( *assembledTriangleMesh );

   return assembledGeometryComp;
}

///////////////////////////////////////////////////////////////////////////////

#include "core-Renderer\RenderTree.h"
#include "core-Renderer\GeometryComponent.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\Renderer.h"
#include "core\ContinuousMemoryPool.h"


///////////////////////////////////////////////////////////////////////////////

GeometryNode::GeometryNode( GeometryNode*& nextNode, GeometryComponent& geometry )
   : m_next( nextNode )
   , m_geometry( geometry )
{
   nextNode = this;
}

///////////////////////////////////////////////////////////////////////////////

GeometryNode::~GeometryNode()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

StateTreeNode::StateTreeNode( const RenderState& state ) 
   : m_sibling( NULL )
   , m_state( &state ) 
   , m_geometryNode( NULL )
{
}


///////////////////////////////////////////////////////////////////////////////

StateTreeNode::~StateTreeNode()
{
}

///////////////////////////////////////////////////////////////////////////////

bool StateTreeNode::compareState( const RenderState* state )
{
   return RenderState::isEqual( *m_state, *state );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

RenderTree::RenderTree( Renderer& renderer, const Array< GeometryComponent* >& visibleElems )
   : m_renderer( renderer )
   , m_root( NULL )
{
   if ( visibleElems.empty() )
   {
      return;
   }

   // the first element
   {
      GeometryComponent* geometry = visibleElems[0];
      const RenderState* checkedState = geometry->getRenderState();
      m_root = new ( renderer.m_renderTreeMemPool ) StateTreeNode( *checkedState );
      new ( renderer.m_renderTreeMemPool ) GeometryNode( m_root->m_geometryNode, *geometry );
   }

   // subsequent elements
   uint elemsCount = visibleElems.size();
   for ( uint i = 1; i < elemsCount; ++i )
   {
      GeometryComponent* geometry = visibleElems[i];
      const RenderState* checkedState = geometry->getRenderState();
      // at this point the states are already sorted

      // states span into the tree depth - so each branch is a unique state,
      // and its leaves contain geometry to render

      // find the child node for the state of the added element
      StateTreeNode* currNode = m_root;

      // check the siblings and look for a similar state
      StateTreeNode* prevNode = currNode;
      while ( currNode && currNode->compareState( checkedState ) == false )
      {
         prevNode = currNode;
         currNode = currNode->m_sibling;
      }

      if ( currNode == NULL )
      {
         // there is no node with a matching state - create a new one
         prevNode->m_sibling = new ( renderer.m_renderTreeMemPool ) StateTreeNode( *checkedState );
         currNode = prevNode->m_sibling;
      }

      // there are no more states to check - add the geometry
      new ( renderer.m_renderTreeMemPool ) GeometryNode( currNode->m_geometryNode, *geometry );
   }
}

///////////////////////////////////////////////////////////////////////////////

RenderTree::~RenderTree()
{
   for ( StateTreeNode* stateNode = m_root; stateNode != NULL; )
   {
      for ( GeometryNode* geomNode = stateNode->m_geometryNode; geomNode != NULL; )
      {
         GeometryNode* gnToRemove = geomNode;
         geomNode = geomNode->m_next;
         delete gnToRemove;
      }

      StateTreeNode* snToRemove = stateNode;
      stateNode = stateNode->m_sibling;
      delete snToRemove;
   }
}

///////////////////////////////////////////////////////////////////////////////

void RenderTree::render( RenderProfileId profileIdx )
{
   for ( StateTreeNode* stateNode = m_root; stateNode != NULL; stateNode = stateNode->m_sibling )
   {
      for ( GeometryNode* geomNode = stateNode->m_geometryNode; geomNode != NULL; geomNode = geomNode->m_next )
      {
         geomNode->m_geometry.render( m_renderer, profileIdx );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

/// @file   core-Renderer/RenderTree.h
/// @brief  render tree definition
#pragma once

#include "core\TSContinuousMemoryPool.h"
#include "core\MemoryRouter.h"
#include "core\Array.h"


///////////////////////////////////////////////////////////////////////////////

class GeometryComponent;
class RenderState;
class Renderer;

///////////////////////////////////////////////////////////////////////////////

/**
 * A geometry node in the scene render tree - this is the tree's leaf.
 */
struct GeometryNode
{
   DECLARE_ALLOCATOR( GeometryNode, AM_DEFAULT );

   GeometryNode*           m_next;
   GeometryComponent&      m_geometry;

   /**
    * Constructor.
    *
    * @param nextNode      the node can have siblings, but only ones containing geometry rendered with the same render state
    * @param geometry      rendered geometry
    */
   GeometryNode( GeometryNode*& nextNode, GeometryComponent& geometry );
   ~GeometryNode();
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A state node in the scene render tree - this is the tree's composite node.
 */
struct StateTreeNode
{
   DECLARE_ALLOCATOR( StateTreeNode, AM_DEFAULT );

   StateTreeNode*          m_sibling;

   const RenderState*      m_state;
   GeometryNode*           m_geometryNode;

   /**
    * Constructor.
    *
    * @param state         described render state
    */
   StateTreeNode( const RenderState& state );
   ~StateTreeNode();

   /**
    * Compares another render state with the one contained in this node.
    *
    * @param state      other render state
    */ 
   bool compareState( const RenderState* state );
};

///////////////////////////////////////////////////////////////////////////////

class RenderTree
{
   DECLARE_ALLOCATOR( RenderTree, AM_DEFAULT );

public:
   Renderer&            m_renderer;
   StateTreeNode*       m_root;

public:
   /**
    * Constructor.
    *
    * @param renderer
    * @param visibleElems
    */
   RenderTree( Renderer& renderer, const Array< GeometryComponent* >& visibleElems );
   ~RenderTree();

   /**
    * Renders the tree using the specified material profile.
    *
    * @param profileIdx
    */
   void render( enum RenderProfileId profileIdx );
};

///////////////////////////////////////////////////////////////////////////////

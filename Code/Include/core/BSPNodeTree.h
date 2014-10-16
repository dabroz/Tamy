/// @file   core\BSPNodeTree.h
/// @brief  a BSP tree that stores polygons in its nodes
#ifndef _BSP_NODE_TREE_H
#define _BSP_NODE_TREE_H

#include "core\MemoryRouter.h"
#include "core\Array.h"
#include "core\Plane.h"


///////////////////////////////////////////////////////////////////////////////

struct Vector;

///////////////////////////////////////////////////////////////////////////////

/**
 * This BSP tree stores split polygons in its nodes, and thus is perfect
 * for implementing renderables that require sorted geometry (like transparent
 * geometry for example).
 */
template <class TPolygon>
class BSPNodeTree
{
   DECLARE_ALLOCATOR( BSPNodeTree, AM_ALIGNED_16 );

private:
   struct Node
   {
      DECLARE_ALLOCATOR( Node, AM_ALIGNED_16 );

      Node* back;
      Node* front;
      Plane splitPlane;
      Array<TPolygon*> geometry;

      Node();
      ~Node();
   };

private:
   Node* m_root;

public:
   /**
    * Constructor.
    *
    * @param geometry   geometry we want to build the tree from.
    */
   BSPNodeTree( const Array<TPolygon*>& geometry );
   ~BSPNodeTree();

   /**
    * Queries the tree for polygons it contains. Those polygons will
    * be returned sorted with respect to the query position.
    *
    * @param pos           position with respect to which the polygons
    *                      should be sorted
    * @param results       placeholder for results
    * @param checkedNode   internal parameter specifying from which node
    *                      should the query start. Better left unspecified
    *                      if you want to get polygons from the whole tree.
    */
   void query( const Vector& pos, Array<TPolygon*>& results, Node* checkedNode = NULL );

private:
   void createSubtree( Node* currNode, const Array<TPolygon*>& geometry );

   void classifyPolygonAgainstPlane( const Plane& splittingPlane, TPolygon* classifiedPoly, Node* currNode, Array<TPolygon*>& outBackPolys, Array<TPolygon*>& outFrontPolys );
};

///////////////////////////////////////////////////////////////////////////////

#include "core\BSPNodeTree.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _BSP_NODE_TREE_H

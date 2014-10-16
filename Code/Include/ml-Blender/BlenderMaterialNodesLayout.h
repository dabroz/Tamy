/// @file   ml-Blender\BlenderMaterialNodesLayout.h
/// @brief  utility that manages the location of nodes, spreading them evenly across the canvas
#pragma once

#include "core\MemoryRouter.h"
#include "core\List.h"
#include "core\Vector.h"


///////////////////////////////////////////////////////////////////////////////

class MaterialNode;

///////////////////////////////////////////////////////////////////////////////

class BlenderMaterialNodesLayout
{
   DECLARE_ALLOCATOR( BlenderMaterialNodesLayout, AM_DEFAULT );

public:
   virtual ~BlenderMaterialNodesLayout() {}

   /**
    * Arranges the nodes.
    */
   void arrangeNodes();

   /**
   * Arranges the nodes from the specified position onwards.
   *
   * @param inOutPosition
   */
   virtual void calculate( Vector& inOutPosition ) = 0;
};

///////////////////////////////////////////////////////////////////////////////

class BMNCompositeItem : public BlenderMaterialNodesLayout
{
   DECLARE_ALLOCATOR( BMNCompositeItem, AM_DEFAULT );

protected:
   List< BlenderMaterialNodesLayout* >          m_childLayouts;

public:
   virtual ~BMNCompositeItem();

   /**
    * Adds a child layout to the layout.
    */
   void add( BlenderMaterialNodesLayout* layout );

   /**
    * Adds a new node to the layout.
    *
    * @param node
    */
   void add( MaterialNode* node );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Spreads the nodes horizontally.
 */
class BMNHorizLayout : public BMNCompositeItem
{
   DECLARE_ALLOCATOR( BMNHorizLayout, AM_DEFAULT );

public:
   // -------------------------------------------------------------------------
   // BlenderMaterialNodesLayout implementation
   // -------------------------------------------------------------------------
   void calculate( Vector& inOutPosition );
};

///////////////////////////////////////////////////////////////////////////////

/**
* Spreads the nodes vertically.
*/
class BMNVertLayout : public BMNCompositeItem
{
   DECLARE_ALLOCATOR( BMNVertLayout, AM_DEFAULT );

public:
   // -------------------------------------------------------------------------
   // BlenderMaterialNodesLayout implementation
   // -------------------------------------------------------------------------
   void calculate( Vector& inOutPosition );
};

///////////////////////////////////////////////////////////////////////////////

/**
* Spreads the nodes vertically.
*/
class BMNNodeItem : public BlenderMaterialNodesLayout
{
   DECLARE_ALLOCATOR( BMNNodeItem, AM_DEFAULT );

private:
   MaterialNode*           m_node;

public:
   /**
    * Constructor.
    *
    * @param node
    */
   BMNNodeItem( MaterialNode* node );
   ~BMNNodeItem();

   // -------------------------------------------------------------------------
   // BlenderMaterialNodesLayout implementation
   // -------------------------------------------------------------------------
   void calculate( Vector& inOutPosition );
};

///////////////////////////////////////////////////////////////////////////////
/// @file   core-AI/BlendTreeComposite.h
/// @brief  a composite blend tree node
#pragma once

#include "core-AI\BlendTreeNode.h"
#include "core\Array.h"
#include "core\RuntimeData.h"


///////////////////////////////////////////////////////////////////////////////

class BlendTree;
class BlendTreePlayerListener;

///////////////////////////////////////////////////////////////////////////////

/**
 * A composite blend tree node.
 */
class BlendTreeComposite : public BlendTreeNode
{
   DECLARE_ALLOCATOR( BlendTreeComposite, AM_DEFAULT );
   DECLARE_CLASS();

protected:
   // static data
   Array< BlendTreeNode* >                               m_nodes;

   // runtime data
   TRuntimeVar< Array< const BlendTreeNode* >* >         m_activeNodes;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   BlendTreeComposite( const char* name = "BlendTreeComposite" );
   virtual ~BlendTreeComposite();

   /**
    * Adds a new child node to the tree.
    */
   void add( BlendTreeNode* node );

   /**
    * Inserts a node at the specified position.
    *
    * @param index
    * @param node
    */
   void insert( int index, BlendTreeNode* node );

   /**
    * Removes a child node from the tree.
    */
   void remove( BlendTreeNode* node );

   /**
    * Clears the contents of this node.
    */
   void clear();

   // ----------------------------------------------------------------------
   // Runtime nodes management
   // ----------------------------------------------------------------------

   /**
    * Activates a child node.
    *
    * @param player
    * @param activatedChild
    */
   void activateChildNode( BlendTreePlayer* player, const BlendTreeNode* activatedChild ) const;

   /**
    * Deactivates a child node.
    *
    * @param player
    * @param activatedChild
    */
   void deactivateChildNode( BlendTreePlayer* player, const BlendTreeNode* deactivatedChild ) const;

   // ----------------------------------------------------------------------
   // BehTreeNode implementation
   // ----------------------------------------------------------------------
   void onDeactivateNode( BlendTreePlayer* player ) const;
   void pullStructure( BlendTreeListener* listener );
   void pullRuntimeStructure( BlendTreePlayer* player, BlendTreePlayerListener* listener ) const;
   void onHostTreeSet( BlendTree* tree );
   void initializeLayout( BlendTreePlayer* player ) const;
   void deinitializeLayout( BlendTreePlayer* player ) const;
   void updateLogic( BlendTreePlayer* player ) const;
   void generateTreeSyncProfile( BlendTreePlayer* player, BlendTreeSyncProfile& outSyncData ) const;
   void synchronizeNodeToTree( BlendTreePlayer* player, const BlendTreeSyncProfile& syncData ) const;
   void samplePose( BlendTreePlayer* player, float timeDelta ) const;

protected:
   /**
    * This notification is called when a node is about to be removed.
    *
    * @param node
    * @param idx        index the node used to be stored under
    */
   virtual void onNodeRemoved( BlendTreeNode* node, uint idx ) {}

   /**
    * This notification is called when a new node is inserted.
    *
    * @param node
    * @param idx
    */
   virtual void onNodeInserted( BlendTreeNode* node, uint idx ) {}
};

///////////////////////////////////////////////////////////////////////////////

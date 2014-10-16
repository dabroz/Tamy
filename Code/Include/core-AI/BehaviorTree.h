/// @file   core-AI/BehaviorTree.h
/// @brief  behavior tree resource
#pragma once

#include "core\Resource.h"
#include "core\FilePath.h"
#include "core-AI\BehTreeComposite.h"
#include "core\List.h"
#include "core\Array.h"


///////////////////////////////////////////////////////////////////////////////

class BehaviorTreeListener;
class BehaviorTreeVariable;
class BehaviorTreeRunner;

///////////////////////////////////////////////////////////////////////////////

class BehaviorTree : public Resource
{
   DECLARE_ALLOCATOR( BehaviorTree, AM_DEFAULT );
   DECLARE_RESOURCE();

public:
   // runtime data
   List< BehaviorTreeListener* >       m_listeners;

private:
   // static data
   BehTreeComposite*                   m_root;
   Array< BehaviorTreeVariable* >      m_variables;

public:
   /**
    * Constructor.
    *
    * @param path
    */
   BehaviorTree( const FilePath& path = FilePath() );
   ~BehaviorTree();

   /**
    * Returns the root node of the tree.
    */
   inline BehTreeComposite& getRoot() { return *m_root; }

   // -------------------------------------------------------------------------
   // Control data
   // -------------------------------------------------------------------------
   /**
    * Adds a new variable to the tree.
    *
    * @param btVariable
    */
   void addVariable( BehaviorTreeVariable* btVariable );

   /**
    * Removes a variable from the tree.
    *
    * @param btVariable
    */
   void removeVariable( BehaviorTreeVariable* btVariable );

   // -------------------------------------------------------------------------
   // Listeners management
   // -------------------------------------------------------------------------
   /**
    * Attaches a new listener.
    *
    * @param listener
    */
   void attachListener( BehaviorTreeListener* listener );

   /**
    * Detaches a listener.
    *
    * @param listener
    */
   void detachListener( BehaviorTreeListener* listener );

   /**
    * Pulls the tree structure, feeding it to the listener.
    *
    * @param listener
    */
   void pullStructure( BehaviorTreeListener* listener );

   // -------------------------------------------------------------------------
   // Runtime
   // -------------------------------------------------------------------------
   /**
    * Initializes a runtime layout of the tree.
    *
    * @param player
    */
   void createLayout( BehaviorTreeRunner* player );

   /**
    * Deinitializes a runtime layout of the tree.
    *
    * @param player
    */
   void destroyLayout( BehaviorTreeRunner* player );

   // -------------------------------------------------------------------------
   // Resource implementation
   // -------------------------------------------------------------------------
   void replaceContents( Resource& rhs );
   void onResourceLoaded( ResourcesManager& resMgr );

private:
   void indexVariables( uint startIdx );
};

///////////////////////////////////////////////////////////////////////////////

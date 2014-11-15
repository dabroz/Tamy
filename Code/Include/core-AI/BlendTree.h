/// @file   core-AI/BlendTree.h
/// @brief  a blend tree resource
#ifndef _BLEND_TREE_H
#define _BLEND_TREE_H

#include "core\Resource.h"
#include "core\FilePath.h"
#include "core\List.h"
#include "core\Array.h"


///////////////////////////////////////////////////////////////////////////////

class BlendTreeListener;
class BlendTreeStateMachine;
class BlendTreeEvent;
class BlendTreeVariable;
class BlendTreePlayer;
class Skeleton;

///////////////////////////////////////////////////////////////////////////////

/**
 * A blend tree resource.
 */
class BlendTree : public Resource
{
   DECLARE_ALLOCATOR( BlendTree, AM_DEFAULT );
   DECLARE_RESOURCE();

public:
   // runtime data
   List< BlendTreeListener* >          m_listeners;

   // -------------------------------------------------------------------------
   // static data
   // -------------------------------------------------------------------------
   // a skeleton this blend tree is capable of animating
   Skeleton*                           m_skeleton;

   // events & variables defined for this blend tree
   Array< BlendTreeEvent* >            m_events;
   Array< BlendTreeVariable* >         m_variables;

private:
   // static data
   BlendTreeStateMachine*              m_root;

public:
   /**
    * Constructor.
    *
    * @param path
    */
   BlendTree( const FilePath& path = FilePath() );
   ~BlendTree();

   /**
    * Returns the root node of the tree.
    */
   inline BlendTreeStateMachine& getRoot() { return *m_root; }

   /**
    * Sets a skeleton this blend tree is capable of animating.
    *
    * @param skeleton.
    */
   void setSkeleton( Skeleton* skeleton );

   // -------------------------------------------------------------------------
   // Control data
   // -------------------------------------------------------------------------
   /**
    * Adds a new event to the tree.
    *
    * @param event
    */
   void addEvent( BlendTreeEvent* event );

   /**
    * Removes an event from the tree.
    *
    * @param event
    */
   void removeEvent( BlendTreeEvent* event );

   /**
    * Adds a new variable to the tree.
    *
    * @param btVariable
    */
   void addVariable( BlendTreeVariable* btVariable );

   /**
    * Removes a variable from the tree.
    *
    * @param btVariable
    */
   void removeVariable( BlendTreeVariable* btVariable );

   /**
    * Returns a variable with the specified name.
    *
    * @param name
    */
   template< typename VarType >
   const VarType* getVariable( const std::string& name ) const;

   // -------------------------------------------------------------------------
   // Listeners management
   // -------------------------------------------------------------------------
   /**
    * Attaches a new listener.
    *
    * @param listener
    */
   void attachBlendTreeListener( BlendTreeListener* listener );

   /**
    * Detaches a listener.
    *
    * @param listener
    */
   void detachBlendTreeListener( BlendTreeListener* listener );

   /**
    * Pulls the tree structure, feeding it to the listener.
    *
    * @param listener
    */
   void pullStructure( BlendTreeListener* listener );

   // -------------------------------------------------------------------------
   // Runtime context
   // -------------------------------------------------------------------------

   /**
    * Initializes a runtime context of this tree.
    *
    * @param player
    */
   void initializeLayout( BlendTreePlayer* player ) const;
   
   /**
    * Deinitializes a runtime context of this tree.
    *
    * @param player
    */
   void deinitializeLayout( BlendTreePlayer* player ) const;

   // -------------------------------------------------------------------------
   // Resource implementation
   // -------------------------------------------------------------------------
   void replaceContents( Resource& rhs );
   void onResourceLoaded( ResourcesManager& resMgr );

private:
   void indexEvents( uint startIdx );
   void indexVariables( uint startIdx );
};

///////////////////////////////////////////////////////////////////////////////

#include "core-AI\BlendTree.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _BLEND_TREE_H

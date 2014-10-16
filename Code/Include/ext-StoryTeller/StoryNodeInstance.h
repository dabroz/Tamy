/// @file   ext-StoryTeller/StoryNodeInstance.h
/// @brief  a runtime instance of a story item
#pragma once

#include "core-MVC\Entity.h"
#include "core\ReflectionObjectChangeListener.h"


///////////////////////////////////////////////////////////////////////////////

class Prefab;
class StoryNode;
class BehaviorTreeRunner;
struct StoryBehTreeContext;
class BehaviorTree;
class StoryPlayer;

///////////////////////////////////////////////////////////////////////////////

class StoryNodeInstance : public Entity, public ReflectionObjectChangeListener
{
   DECLARE_ALLOCATOR( StoryNodeInstance, AM_ALIGNED_16 );
   DECLARE_CLASS();

private:
   StoryNode*              m_hostNode;

   StoryBehTreeContext*    m_context;
   BehaviorTreeRunner*     m_behTreePlayer;

public:
   /**
    * Copy constructor.
    *
    * @param rhs
    */
   StoryNodeInstance( const StoryNodeInstance& rhs );

   /**
    * Constructor.
    *
    * @param prefab        prefab we want to embed
    * @param node
    * @param name          entity name
    */
   StoryNodeInstance( Prefab* prefab, StoryNode* node, const char* name = "" );
   ~StoryNodeInstance();

   /**
    * Returns a pointer to the represented story node.
    */
   inline StoryNode* getStoryNode() const { return m_hostNode; }

   /**
    * Initializes logics execution context.
    */
   void initializeContext( StoryPlayer& player );

   /**
    * Deinitializes logic execution context.
    */
   void deinitializeContext( StoryPlayer& player );

   /**
    * Updates node's logic.
    */
   void updateLogic();

   // -------------------------------------------------------------------------
   // ReflectionObjectChangeListener implementation
   // -------------------------------------------------------------------------
   void onObservedPropertyChanged( ReflectionProperty& property );
   void onObservedObjectDeleted( ReflectionObject* deletedObject );
};

///////////////////////////////////////////////////////////////////////////////

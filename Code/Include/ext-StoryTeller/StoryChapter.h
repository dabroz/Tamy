/// @file   ext-StoryTeller/StoryChapter.h
/// @brief  a composite story tree node
#pragma once

#include "ext-StoryTeller\StoryNode.h"
#include "ext-StoryTeller\StoryAction.h"
#include "core\Array.h"
#include "core\List.h"
#include "core\GraphBuilder.h"
#include "core\GraphBuilderTransaction.h"
#include "core\RuntimeData.h"
#include <vector>


///////////////////////////////////////////////////////////////////////////////

class Model;
class Entity;
class StoryChapter;
typedef GraphBuilder< StoryAction >              StoryChapterGraphBuilder;
typedef GraphBuilderTransaction< StoryAction >   StoryGraphTransaction;

///////////////////////////////////////////////////////////////////////////////

/**
 * A composite story tree node.
 */
class StoryChapter : public StoryNode, public StoryChapterGraphBuilder
{
   DECLARE_ALLOCATOR( StoryChapter, AM_DEFAULT );
   DECLARE_CLASS();

public:
   Array< StoryNode* >                                m_storyNodes;
   std::vector< StoryAction* >                        m_actions;

   Prefab*                                            m_environment;

public:
   /**
    * Constructor.
    */
   StoryChapter();
   virtual ~StoryChapter();

   /**
    * Adds a new child node to the tree.
    */
   void add( StoryNode* node );

   /**
    * Inserts a node at the specified position.
    *
    * @param index
    * @param node
    */
   void insert( int index, StoryNode* node );

   /**
    * Removes a child node from the tree.
    */
   void remove( StoryNode* node );

   /**
    * Clears the contents of this node.
    */
   void clear();

   /**
    * Returns the graph builder's instance used to construct this chapter's story graph.
    */
   inline StoryChapterGraphBuilder& graphBuilder() { return *this; }

   // ----------------------------------------------------------------------
   // StoryNode implementation
   // ----------------------------------------------------------------------
   void pullStructure( StoryListener* listener );
   void onHostStorySet( Story* story );
   Prefab* getRepresentationPrefab() { return m_environment; }
   StoryNodeInstance* instantiate();
   BehaviorTree* getLogics() const { return NULL; }

   // ----------------------------------------------------------------------
   // ReflectionObject implementation
   // ----------------------------------------------------------------------
   void onObjectLoaded();
};

///////////////////////////////////////////////////////////////////////////////

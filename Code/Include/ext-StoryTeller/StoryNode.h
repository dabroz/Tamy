/// @file   ext-StoryTeller\StoryNode.h
/// @brief  base story node
#pragma once

#include "core\ReflectionObject.h"


///////////////////////////////////////////////////////////////////////////////

class Story;
class StoryPlayer;
class StoryListener;
class StoryNodeInstance;
class Prefab;
class BehaviorTree;

///////////////////////////////////////////////////////////////////////////////

/**
 * Base story node.
 */
class StoryNode : public ReflectionObject
{
   DECLARE_CLASS();
   DECLARE_ALLOCATOR( StoryNode, AM_DEFAULT );

public:
   enum Result
   {
      IN_PROGRESS,
      FINISHED,
      FAILED,
   };

protected:
   Story*                     m_story;
   std::string                m_name;

public:
   virtual ~StoryNode() {}

   /**
    * Sets the instance of the tree that owns this node.
    *
    * @param story
    */
   void setHostStory( Story* story );

   /**
    * Gives the node a new name
    *
    * @param nodeName
    */
   void setName( const char* nodeName );

   /**
    * Returns the node's name.
    */
   inline const std::string& getName() const { return m_name; }

   /**
    * Returns a model that will be used to represent the node in the scene.
    */
   virtual Prefab* getRepresentationPrefab() = 0;

   /**
    * Called in order to push the information about the story structure to the specified listener.
    *
    * @param listener
    */
   virtual void pullStructure( StoryListener* listener ) = 0;

   /**
    * Returns a pointer to node's logics implementation.
    */
   virtual BehaviorTree* getLogics() const = 0;

   // -------------------------------------------------------------------------
   // Instantiation
   // -------------------------------------------------------------------------
   /**
    * Instantiates the node in the specified scene.
    *
    * @param context
    */
   virtual StoryNodeInstance* instantiate() = 0;

   // -------------------------------------------------------------------------
   // ReflectionObject implementation
   // -------------------------------------------------------------------------
   void onPropertyChanged( ReflectionProperty& property );

protected:
   /**
    * Constructor.
    */
   StoryNode();

   /**
    * Called when a new host story instance is set on the node.
    *
    * @param tree
    */
   virtual void onHostStorySet( Story* story ) = 0;

};

///////////////////////////////////////////////////////////////////////////////

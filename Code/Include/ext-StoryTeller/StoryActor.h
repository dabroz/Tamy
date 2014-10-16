/// @file   ext-StoryTeller/StoryActor.h
/// @brief  an actor taking part in a story
#pragma once

#include "ext-StoryTeller\StoryNode.h"
#include "core\FilePath.h"
#include "core\Matrix.h"
#include "core\ReflectionObjectChangeListener.h"


///////////////////////////////////////////////////////////////////////////////

class Story;
class BehaviorTree;

///////////////////////////////////////////////////////////////////////////////

/**
 * An actor taking part in a story.
 */
class StoryActor : public StoryNode, public ReflectionObjectChangeListener
{
   DECLARE_ALLOCATOR( StoryActor, AM_DEFAULT );
   DECLARE_CLASS();

private:
   // static data
   FilePath                         m_thumbnailPath;
   Matrix                           m_transform;

   Prefab*                          m_actorPrefab;
   BehaviorTree*                    m_behaviorTree;

public:
   /**
    * Constructor.
    */
   StoryActor();
   ~StoryActor();

   /**
    * Sets a new world transform for the item.
    */
   void setTransform( const Matrix& transform );

   /**
    * Returns the world transform of the item.
    */
   inline const Matrix& getTransform() const { return m_transform; }

   /**
    * Returns a behavior tree of this actor.
    */
   inline BehaviorTree* getBehaviorTree() const { return m_behaviorTree; }

   // ----------------------------------------------------------------------
   // Characteristics
   // ----------------------------------------------------------------------

   /**
    * Returns actor's name.
    */
   inline const std::string& getName() const { return m_name; }

   /**
    * Returns the path to item's thumbnail.
    */
   inline const FilePath& getThumbnailPath() const { return m_thumbnailPath; }

   // -------------------------------------------------------------------------
   // StoryNode implementation
   // -------------------------------------------------------------------------
   void pullStructure( StoryListener* listener );
   void onHostStorySet( Story* story );
   Prefab* getRepresentationPrefab() { return m_actorPrefab; }
   StoryNodeInstance* instantiate();
   BehaviorTree* getLogics() const { return m_behaviorTree; }

   // -------------------------------------------------------------------------
   // ReflectionObjectChangeListener implementation
   // -------------------------------------------------------------------------
   void onObservedPropertyChanged( ReflectionProperty& property );
   void onObservedObjectDeleted( ReflectionObject* deletedObject );
};

///////////////////////////////////////////////////////////////////////////////

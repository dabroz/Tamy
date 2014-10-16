#include "ext-StoryTeller\StoryActor.h"
#include "ext-StoryTeller\Story.h"
#include "ext-StoryTeller\StoryNodeInstance.h"
#include "core-AI\BehaviorTreeRunner.h"
#include "core-AI\BehaviorTree.h"
#include "core-MVC\Prefab.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( StoryActor );
   PARENT( StoryNode );
   PROPERTY_EDIT( "Thumbnail", FilePath, m_thumbnailPath );
   PROPERTY_EDIT( "Transform", Matrix, m_transform );
   PROPERTY_EDIT( "Actor prefab", Prefab*, m_actorPrefab );
   PROPERTY_EDIT( "Behavior tree", BehaviorTree*, m_behaviorTree );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

StoryActor::StoryActor()
   : m_actorPrefab( NULL )
   , m_behaviorTree( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

StoryActor::~StoryActor()
{
   // resources will be removed by the resources manager
   m_actorPrefab = NULL;
   m_behaviorTree = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void StoryActor::pullStructure( StoryListener* listener )
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

void StoryActor::onHostStorySet( Story* story )
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

StoryNodeInstance* StoryActor::instantiate()
{
   if ( !m_actorPrefab )
   {
      return NULL;
   }

   StoryNodeInstance* actorModelEntity = new StoryNodeInstance( m_actorPrefab, this, getName().c_str() );

   // set the local transform and commit it to the global transform - children components,
   // such as the GL2DBody, will refer to that one when controlling the position of the body
   actorModelEntity->setLocalMtx( m_transform );
   actorModelEntity->updateTransforms();

   actorModelEntity->attachListener( *this );

   return actorModelEntity;
}

///////////////////////////////////////////////////////////////////////////////

void StoryActor::onObservedPropertyChanged( ReflectionProperty& property )
{
   if ( property.getName() == "m_localMtx" )
   {
      // item instance was moved - copy its transform
      Matrix* itemLocalMtx = (Matrix*)property.edit();
      if ( m_transform != *itemLocalMtx )
      {
         m_transform = *itemLocalMtx;

         ReflectionProperty* property = getProperty( "m_transform" );
         notifyPropertyChange( *property );
         delete property;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void StoryActor::onObservedObjectDeleted( ReflectionObject* deletedObject )
{
}

///////////////////////////////////////////////////////////////////////////////

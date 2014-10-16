#include "ext-StoryTeller\StoryNodeInstance.h"
#include "ext-StoryTeller\StoryNode.h"
#include "ext-StoryTeller\StoryBehTreeContext.h"
#include "ext-StoryTeller\StoryPlayer.h"
#include "core-AI\BehaviorTreeRunner.h"
#include "core\ReflectionProperty.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( StoryNodeInstance );
   PARENT( Entity );
   PROPERTY( StoryNode*, m_hostNode );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

StoryNodeInstance::StoryNodeInstance( const StoryNodeInstance& rhs )
   : Entity( rhs )
   , m_hostNode( rhs.m_hostNode )
   , m_behTreePlayer( NULL )
   , m_context( NULL )
{
   m_hostNode->addReference();
   m_hostNode->attachListener( *this );
}

///////////////////////////////////////////////////////////////////////////////

StoryNodeInstance::StoryNodeInstance( Prefab* prefab, StoryNode* node, const char* name )
   : Entity( name )
   , m_hostNode( node )
   , m_behTreePlayer( NULL )
   , m_context( NULL )
{
   setPrefab( prefab );

   m_hostNode->addReference();
   m_hostNode->attachListener( *this );
}

///////////////////////////////////////////////////////////////////////////////

StoryNodeInstance::~StoryNodeInstance()
{
   if ( m_hostNode )
   {
      m_hostNode->detachListener( *this );
      m_hostNode->removeReference();
      m_hostNode = NULL;
   }

   delete m_behTreePlayer;
   m_behTreePlayer = NULL;

   delete m_context;
   m_context = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void StoryNodeInstance::onObservedPropertyChanged( ReflectionProperty& property )
{
   if ( m_hostNode && property.getName() == "m_transform" )
   {
      // item instance was moved - copy its transform
      Matrix* itemLocalMtx = (Matrix*)property.edit();
      if ( getLocalMtx() != *itemLocalMtx )
      {
         setLocalMtx( *itemLocalMtx );

         ReflectionProperty* property = getProperty( "m_localMtx" );
         notifyPropertyChange( *property );
         delete property;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void StoryNodeInstance::onObservedObjectDeleted( ReflectionObject* deletedObject )
{
   m_hostNode = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void StoryNodeInstance::initializeContext( StoryPlayer& player )
{
   BehaviorTree* behTree = m_hostNode->getLogics();
   if ( behTree )
   {
      m_context = new StoryBehTreeContext( *m_hostNode, this, player );

      m_behTreePlayer = new BehaviorTreeRunner( *behTree, m_context );
   }
}

///////////////////////////////////////////////////////////////////////////////

void StoryNodeInstance::deinitializeContext( StoryPlayer& player )
{
   if ( m_behTreePlayer )
   {
      delete m_behTreePlayer;
      m_behTreePlayer = NULL;
   }

   if ( m_context )
   {
      delete m_context;
      m_context = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void StoryNodeInstance::updateLogic()
{
   if ( m_behTreePlayer )
   {
      m_behTreePlayer->execute();
   }
}

///////////////////////////////////////////////////////////////////////////////


#include "ext-StoryTeller\BTAInputToWorldPos.h"
#include "ext-StoryTeller\StoryBehTreeContext.h"
#include "ext-StoryTeller\StoryNodeInstance.h"
#include "ext-StoryTeller\StoryPlayer.h"
#include "core-AppFlow\UserInputController.h"
#include "core-AI\BehaviorTreeRunner.h"
#include "core-AI\BehTreeVariable.h"
#include "core-Renderer\Renderer.h"
#include <windows.h>

// queries
#include "core-Renderer\RQEntitySelection.h"
#include "core-Renderer\RQWorldPosition.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BTAInputToWorldPos );
   PARENT( BehTreeAction );
   PROPERTY_EDIT_REFERENCE( "World pos", BehTreeVarVector*, m_worldPos );
   PROPERTY_EDIT_REFERENCE( "Selected object", BehTreeVarReflectionObject*, m_selectedObject );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BTAInputToWorldPos::BTAInputToWorldPos()
   : m_worldPos( NULL )
   , m_selectedObject( NULL )
{
   registerReferences();
}

///////////////////////////////////////////////////////////////////////////////

BTAInputToWorldPos::~BTAInputToWorldPos()
{
}

///////////////////////////////////////////////////////////////////////////////

void BTAInputToWorldPos::createLayout( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data();
   data.registerVar( m_posQuery );
}

///////////////////////////////////////////////////////////////////////////////

void BTAInputToWorldPos::initialize( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data();

   PlayerSceneQuery* query = new PlayerSceneQuery();
   data[m_posQuery] = query;
}

///////////////////////////////////////////////////////////////////////////////

void BTAInputToWorldPos::deinitialize( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data();
   PlayerSceneQuery* query = data[m_posQuery];
   data[m_posQuery] = NULL;

   query->removeReference();
}

///////////////////////////////////////////////////////////////////////////////

BehTreeNode::Result BTAInputToWorldPos::execute( BehaviorTreeRunner& runner ) const
{     
   RuntimeDataBuffer& data = runner.data();

   PlayerSceneQuery* query = data[m_posQuery];
   StoryBehTreeContext* context = (StoryBehTreeContext*)runner.getContext();

   switch( query->m_state )
   {
   case PlayerSceneQuery::READY:
      {
         if ( context->m_player.userInputController()->isKeyPressed( VK_LBUTTON ) )
         {
            query->issue( *context );
         }

         break;
      }

   case PlayerSceneQuery::WAITING:
      {
         // keep on waiting
         break;
      }

   case PlayerSceneQuery::RESULT_AVAILABLE:
      {
         // send the actor somewhere
         query->commitResults( runner, *this );
         break;
      }
   }

   return BehTreeNode::IN_PROGRESS;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_OBJECT( PlayerSceneQuery );
   PARENT( ReflectionObject );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

PlayerSceneQuery::PlayerSceneQuery()
   : m_state( READY )
{
}

///////////////////////////////////////////////////////////////////////////////

void PlayerSceneQuery::issue( StoryBehTreeContext& context )
{
   Point mousePos = context.m_player.userInputController()->getMousePos();

   Renderer* renderer = context.m_player.renderer();
   new ( renderer->query() ) RQSelectEntity( mousePos, this );
   new ( renderer->query() ) RQWorldPosition( mousePos, this );

   // reset cached data in anticipation of the new one
   m_selectedNodeInstance = NULL;
   m_clickedWorldPos.setZero();

   // change state
   m_state = WAITING;
}

///////////////////////////////////////////////////////////////////////////////

void PlayerSceneQuery::commitResults( BehaviorTreeRunner& runner, const BTAInputToWorldPos& hostAction )
{
   hostAction.m_selectedObject->setRuntime( &runner, m_selectedNodeInstance );

   // we're moving in the XY plane only
   Vector clickedWorldPos = m_clickedWorldPos;
   clickedWorldPos[2] = 0.0f;
   hostAction.m_worldPos->setRuntime( &runner, clickedWorldPos );

   // change state
   m_state = READY;
}

///////////////////////////////////////////////////////////////////////////////

void PlayerSceneQuery::onEntitiesSelected( const List< Entity* >& entities )
{
   // determine which story node instance was clicked
   if ( entities.empty() )
   {
      m_selectedNodeInstance = NULL;
   }
   else
   {
      SceneNode* checkedEntity = entities.front();

      // get the top level StoryNodeInstance
      m_selectedNodeInstance = NULL;
      while( !m_selectedNodeInstance && checkedEntity )
      {
         if ( checkedEntity->isA< StoryNodeInstance >() )
         {
            m_selectedNodeInstance = static_cast< StoryNodeInstance* >( checkedEntity );
         }
         else
         {
            // find a parent of the entity that's a node
            checkedEntity = checkedEntity->getParent();
         }
      }
   }

   // change state
   m_state = RESULT_AVAILABLE;
}

///////////////////////////////////////////////////////////////////////////////

void PlayerSceneQuery::onWorldPositionClicked( const Vector& worldPos )
{
   // memorize clicked world position
   m_clickedWorldPos = worldPos;

   // change state
   m_state = RESULT_AVAILABLE;
}

///////////////////////////////////////////////////////////////////////////////

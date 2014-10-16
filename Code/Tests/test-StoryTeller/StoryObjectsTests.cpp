#include "core-TestFramework\TestFramework.h"

#include "core-MVC\Model.h"
#include "core-MVC\Prefab.h"
#include "core-MVC\Entity.h"
#include "core-MVC\EntityUtils.h"

#include "ext-StoryTeller\Story.h"
#include "ext-StoryTeller\StoryChapter.h"
#include "ext-StoryTeller\StoryAction.h"
#include "ext-StoryTeller\StoryCondition.h"
#include "ext-StoryTeller\StoryActionSockets.h"
#include "ext-StoryTeller\StoryPlayer.h"
#include "ext-StoryTeller\StoryItem.h"

#include "ext-StoryTeller\SASpawnItem.h"
#include "ext-StoryTeller\SAActivateChapter.h"
#include "ext-StoryTeller\SACondition.h"
#include "ext-StoryTeller\SAPause.h"


///////////////////////////////////////////////////////////////////////////////

namespace 
{
   struct StoryActionMock : public StoryAction
   {
      static int        s_nextIdx;

      int&              m_executionOrder;
      SAVoidOutput*     m_out;

      StoryActionMock( int& executionOrder )
         : m_executionOrder( executionOrder )
      {
         defineInput( new SAVoidInput( "In" ) );

         m_out = new SAVoidOutput( "Out" );
         defineOutput( m_out );
      }

      bool execute( StoryPlayer& runner, List< StoryAction* >& outActionsToActivate ) const
      {
         m_executionOrder = s_nextIdx++;
         m_out->collectConnectedNodes( outActionsToActivate );

         return true;
      }
   };

   int StoryActionMock::s_nextIdx = 0;

   // -------------------------------------------------------------------------

   struct SCMock : public StoryCondition
   {
      bool&       m_conditionFlag;

      SCMock( bool& conditionFlag )
         : m_conditionFlag( conditionFlag )
      {}

      bool evaluate( StoryPlayer& runner ) const
      {
         return m_conditionFlag;
      }
   };
}

///////////////////////////////////////////////////////////////////////////////

TEST( StoryPlayer, chapterGraphExecution )
{
   Story story;

   StoryChapter& chapter = story.getRoot();
   StoryChapterGraphBuilder& graphBuilder = chapter.graphBuilder();
   int executionOrder = -1;
   StoryGraphTransaction transaction( graphBuilder );
   {
      StoryAction* startAction = graphBuilder.getNodes()[0];
      StoryActionMock* newAction = new StoryActionMock( executionOrder );
      transaction.addNode( newAction );
      transaction.connectNodes( startAction, "Out", newAction, "In" );
   }
   transaction.commit();

   StoryPlayer player( story );

   CPPUNIT_ASSERT_EQUAL( true, player.execute() );
   CPPUNIT_ASSERT_EQUAL( -1, executionOrder );

   CPPUNIT_ASSERT_EQUAL( true, player.execute() );
   CPPUNIT_ASSERT_EQUAL( 0, executionOrder );

   // once it's been executed and the story's finished, we're not executing it over again
   CPPUNIT_ASSERT_EQUAL( false, player.execute() );
   CPPUNIT_ASSERT_EQUAL( 0, executionOrder );
}

///////////////////////////////////////////////////////////////////////////////

TEST( StoryPlayer, instantiatingItems )
{
   Story story;

   Vector chestPosition;
   chestPosition.set( 1, 2, 3 );

   StoryChapter& rootChapter = story.getRoot();

   // create an item
   StoryItem* chest = new StoryItem();
   Prefab* itemPrefab = new Prefab();
   {
      Matrix chestTransform;
      chestTransform.setPosition<3>( chestPosition );
      chest->setName( "Chest" );
      chest->setTransform( chestTransform );
      chest->setItemPrefab( itemPrefab );

      rootChapter.add( chest ); 
   }

   // create a story graph
   StoryChapterGraphBuilder& graphBuilder = rootChapter.graphBuilder();
   StoryGraphTransaction transaction( graphBuilder );
   {
      StoryAction* startAction = graphBuilder.getNodes()[0];
      StoryAction* itemSpawnAction = new SASpawnItem( chest );
      transaction.addNode( itemSpawnAction );
      transaction.connectNodes( startAction, "Out", itemSpawnAction, "In" );
   }
   transaction.commit();
   
   StoryActionMock::s_nextIdx = 0;
   StoryPlayer player( story );
   Model& gameWorld = player.gameWorld();

   CPPUNIT_ASSERT( NULL == EntityUtils::findNodeByName( gameWorld.getRoot(), "Chest" ) );
   
   bool wasChestSpawnedAtOneTime = false;
   Vector spawnedChestPosition;
   while( player.execute() ) 
   {
      Entity* spawnedChest = static_cast< Entity* >( EntityUtils::findNodeByName( gameWorld.getRoot(), "Chest" ) );

      if ( spawnedChest )
      {
         wasChestSpawnedAtOneTime = true;
         spawnedChest->getPosition( spawnedChestPosition );
      }
   }

   CPPUNIT_ASSERT( wasChestSpawnedAtOneTime );
   COMPARE_VEC( chestPosition, spawnedChestPosition );

   // cleanup 
   itemPrefab->removeReference();
}

///////////////////////////////////////////////////////////////////////////////

TEST( StoryPlayer, activatingChapter )
{
   Story story;

   StoryChapter& chapter = story.getRoot();
   StoryChapter* subChapter = new StoryChapter();
   chapter.add( subChapter );

   int executionOrder[] = { -1, -1, -1 };

   {
      StoryChapterGraphBuilder& rootGraphBuilder = chapter.graphBuilder();
      StoryGraphTransaction rootTransaction( rootGraphBuilder );

      StoryAction* startAction = chapter.getNodes()[0];
      StoryAction* rootAction1 = new StoryActionMock( executionOrder[0] );
      StoryAction* chapterAction = new SAActivateChapter( subChapter );
      StoryAction* rootAction2 = new StoryActionMock( executionOrder[2] );

      rootTransaction.addNode( rootAction1 );
      rootTransaction.addNode( chapterAction );
      rootTransaction.addNode( rootAction2 );
      rootTransaction.connectNodes( startAction, "Out", rootAction1, "In" );
      rootTransaction.connectNodes( rootAction1, "Out", chapterAction, "In" );
      rootTransaction.connectNodes( chapterAction, "Out", rootAction2, "In" );

      rootTransaction.commit();
   }

   {
      StoryChapterGraphBuilder& embeddedGraphBuilder = subChapter->graphBuilder();
      StoryGraphTransaction embeddedTransaction( embeddedGraphBuilder );

      StoryAction* startAction = subChapter->getNodes()[0];
      StoryAction* embeddedAction1 = new StoryActionMock( executionOrder[1] );

      embeddedTransaction.addNode( embeddedAction1 );
      embeddedTransaction.connectNodes( startAction, "Out", embeddedAction1, "In" );

      embeddedTransaction.commit();
   }

   StoryActionMock::s_nextIdx = 0;
   StoryPlayer player( story );

   // we're starting the execution ( start node is being evaluated )
   CPPUNIT_ASSERT_EQUAL( true, player.execute() );
   CPPUNIT_ASSERT_EQUAL( -1, executionOrder[0] );
   CPPUNIT_ASSERT_EQUAL( -1, executionOrder[1] );
   CPPUNIT_ASSERT_EQUAL( -1, executionOrder[2] );

   // now we're evaluating rootAction1
   CPPUNIT_ASSERT_EQUAL( true, player.execute() );
   CPPUNIT_ASSERT_EQUAL( 0, executionOrder[0] );
   CPPUNIT_ASSERT_EQUAL( -1, executionOrder[1] );
   CPPUNIT_ASSERT_EQUAL( -1, executionOrder[2] );

   // now we're evaluating chapterAction
   CPPUNIT_ASSERT_EQUAL( true, player.execute() );
   CPPUNIT_ASSERT_EQUAL( 0, executionOrder[0] );
   CPPUNIT_ASSERT_EQUAL( -1, executionOrder[1] );
   CPPUNIT_ASSERT_EQUAL( -1, executionOrder[2] );

   // now we're at embeddedAction1
   CPPUNIT_ASSERT_EQUAL( true, player.execute() );
   CPPUNIT_ASSERT_EQUAL( 0, executionOrder[0] );
   CPPUNIT_ASSERT_EQUAL( 1, executionOrder[1] );
   CPPUNIT_ASSERT_EQUAL( -1, executionOrder[2] );

   // and now we're out of the chapter and evaluating rootAction2
   CPPUNIT_ASSERT_EQUAL( true, player.execute() );
   CPPUNIT_ASSERT_EQUAL( 0, executionOrder[0] );
   CPPUNIT_ASSERT_EQUAL( 1, executionOrder[1] );
   CPPUNIT_ASSERT_EQUAL( 2, executionOrder[2] );

   // and we're done !
   CPPUNIT_ASSERT_EQUAL( false, player.execute() );
   CPPUNIT_ASSERT_EQUAL( 0, executionOrder[0] );
   CPPUNIT_ASSERT_EQUAL( 1, executionOrder[1] );
   CPPUNIT_ASSERT_EQUAL( 2, executionOrder[2] );
}

///////////////////////////////////////////////////////////////////////////////

TEST( StoryPlayer, conditions )
{
   Story story;

   StoryChapter& chapter = story.getRoot();
   StoryChapter* subChapter = new StoryChapter();
   chapter.add( subChapter );

   int executionOrder[] = { -1, -1 };
   bool conditionFlag = true;

   {
      StoryChapterGraphBuilder& rootGraphBuilder = chapter.graphBuilder();
      StoryGraphTransaction rootTransaction( rootGraphBuilder );

      StoryAction* startAction = chapter.getNodes()[0];
      StoryAction* conditionAction = new SACondition( new SCMock( conditionFlag ) );
      StoryAction* trueAction = new StoryActionMock( executionOrder[0] );
      StoryAction* falseAction = new StoryActionMock( executionOrder[1] );

      rootTransaction.addNode( conditionAction );
      rootTransaction.addNode( trueAction );
      rootTransaction.addNode( falseAction );
      rootTransaction.connectNodes( startAction, "Out", conditionAction, "In" );
      rootTransaction.connectNodes( conditionAction, "True", trueAction, "In" );
      rootTransaction.connectNodes( conditionAction, "False", falseAction, "In" );
      
      // create a loop between the true action and the condition
      rootTransaction.connectNodes( trueAction, "Out", conditionAction, "In" );

      rootTransaction.commit();
   }


   StoryActionMock::s_nextIdx = 0;
   StoryPlayer player( story );

   // we're starting the execution ( start node is being evaluated )
   CPPUNIT_ASSERT_EQUAL( true, player.execute() );
   CPPUNIT_ASSERT_EQUAL( -1, executionOrder[0] );
   CPPUNIT_ASSERT_EQUAL( -1, executionOrder[1] );

   // condition is being evaluated for the first time, and it lets
   // the signal through the 'True' socket
   conditionFlag = true;
   CPPUNIT_ASSERT_EQUAL( true, player.execute() );
   CPPUNIT_ASSERT_EQUAL( -1, executionOrder[0] );
   CPPUNIT_ASSERT_EQUAL( -1, executionOrder[1] );

   // 'true' action is being executed
   CPPUNIT_ASSERT_EQUAL( true, player.execute() );
   CPPUNIT_ASSERT_EQUAL( 0, executionOrder[0] );
   CPPUNIT_ASSERT_EQUAL( -1, executionOrder[1] );

   // back to evaluating the condition, but this time, we're choosing the 'False' output
   conditionFlag = false;
   CPPUNIT_ASSERT_EQUAL( true, player.execute() );
   CPPUNIT_ASSERT_EQUAL( 0, executionOrder[0] );
   CPPUNIT_ASSERT_EQUAL( -1, executionOrder[1] );

   // the story's finished, but we're leaving one more frame there just to let the user enjoy
   // the fruits of the last frame before we tear the wold down
   CPPUNIT_ASSERT_EQUAL( true, player.execute() );
   CPPUNIT_ASSERT_EQUAL( 0, executionOrder[0] );
   CPPUNIT_ASSERT_EQUAL( 1, executionOrder[1] );

   // 'false' action is being executed and the graph comes to an end
   CPPUNIT_ASSERT_EQUAL( false, player.execute() );
   CPPUNIT_ASSERT_EQUAL( 0, executionOrder[0] );
   CPPUNIT_ASSERT_EQUAL( 1, executionOrder[1] );
}

///////////////////////////////////////////////////////////////////////////////

TEST( StoryPlayer, pauses )
{
   Story story;

   StoryChapter& chapter = story.getRoot();
   StoryChapter* subChapter = new StoryChapter();
   chapter.add( subChapter );

   int executionOrder = -1;
   bool conditionFlag = false;

   {
      StoryChapterGraphBuilder& rootGraphBuilder = chapter.graphBuilder();
      StoryGraphTransaction rootTransaction( rootGraphBuilder );

      StoryAction* startAction = chapter.getNodes()[0];
      StoryAction* pauseAction = new SAPause( new SCMock( conditionFlag ) );
      StoryAction* action = new StoryActionMock( executionOrder );

      rootTransaction.addNode( pauseAction );
      rootTransaction.addNode( action );
      rootTransaction.connectNodes( startAction, "Out", pauseAction, "In" );
      rootTransaction.connectNodes( pauseAction, "Out", action, "In" );

      rootTransaction.commit();
   }


   StoryActionMock::s_nextIdx = 0;
   StoryPlayer player( story );

   // we're starting the execution ( start node is being evaluated )
   CPPUNIT_ASSERT_EQUAL( true, player.execute() );
   CPPUNIT_ASSERT_EQUAL( -1, executionOrder );

   // pause is being evaluated, but we're blocking it - the condition keeps returning false, 
   // so the pause will stall
   conditionFlag = false;
   CPPUNIT_ASSERT_EQUAL( true, player.execute() );
   CPPUNIT_ASSERT_EQUAL( -1, executionOrder );

   CPPUNIT_ASSERT_EQUAL( true, player.execute() );
   CPPUNIT_ASSERT_EQUAL( -1, executionOrder );

   // now the condition returns true and the pause lets the signal through
   conditionFlag = true;
   CPPUNIT_ASSERT_EQUAL( true, player.execute() );
   CPPUNIT_ASSERT_EQUAL( -1, executionOrder );

   // signal reaches our mock action and the graph execution comes to an end, 
   // but we're leaving one more frame there just to let the user enjoy
   // the fruits of the last frame before we tear the wold down
   CPPUNIT_ASSERT_EQUAL( true, player.execute() );
   CPPUNIT_ASSERT_EQUAL( 0, executionOrder );

   // and now the player is deinitialized and the story's over
   CPPUNIT_ASSERT_EQUAL( false, player.execute() );
   CPPUNIT_ASSERT_EQUAL( 0, executionOrder );
}

///////////////////////////////////////////////////////////////////////////////

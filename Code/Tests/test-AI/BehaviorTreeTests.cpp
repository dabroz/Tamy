#include "core-TestFramework\TestFramework.h"
#include "core-AI\BehaviorTree.h"
#include "core-AI\BehaviorTreeRunner.h"
#include "core-AI\BehTreeNode.h"
#include "core-AI\BehTreeAction.h"
#include "core-AI\BehTreeRepeater.h"
#include "core-AI\BehTreeParallel.h"
#include "core-AI\BehTreeSelector.h"
#include "core-AI\BehTreeCondition.h"


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{

   struct MockAction : public BehTreeAction
   {
      DECLARE_ALLOCATOR( MockAction, AM_DEFAULT );

      static int     s_executionSequence;
      int&           m_executionsSequence;
      int&           m_initsCount;
      int&           m_deinitsCount;

      int            m_iterationsCount;
      mutable int    m_remainingIterations;


      MockAction( int& executionsSequence, int& initsCount, int& deinititsCount ) 
         : m_executionsSequence( executionsSequence )
         , m_initsCount( initsCount )
         , m_deinitsCount( deinititsCount )
         , m_iterationsCount( 1 )
      {}

      static void restartSequence()
      {
         s_executionSequence = 1;
      }

      // ----------------------------------------------------------------------
      // BehTreeNode implementation
      // ----------------------------------------------------------------------

      void initialize( BehaviorTreeRunner& runner ) const
      {
         BehTreeAction::initialize( runner );

         m_remainingIterations = m_iterationsCount;
         ++m_initsCount;
      }

      void deinitialize( BehaviorTreeRunner& runner ) const
      {
         BehTreeAction::deinitialize( runner );
         ++m_deinitsCount;
      }

      Result execute( BehaviorTreeRunner& runner ) const
      {
         m_executionsSequence = s_executionSequence;
         ++s_executionSequence;

         --m_remainingIterations;
         return m_remainingIterations <= 0 ? FINISHED : IN_PROGRESS;
      }
   };

   // -------------------------------------------------------------------------

   struct MockCondition : public BehTreeCondition
   {
      DECLARE_ALLOCATOR( MockCondition, AM_DEFAULT );

      bool                 m_flag;

      MockCondition( bool flag )
         : m_flag( flag )
      {
      }

      // ----------------------------------------------------------------------
      // BehTreeCondition implementation
      // ----------------------------------------------------------------------
      int evaluate( BehaviorTreeRunner& runner, const BehTreeSelector& hostSelector ) const override
      {
         return m_flag ? 0 : 1;
      }
   };

   int MockAction::s_executionSequence = 1;
  
} // namespace anonymous


///////////////////////////////////////////////////////////////////////////////

TEST( BehaviorTree, singleAction )
{
   int executionSequence = 0;
   int initsCount = 0;
   int deinitsCount = 0;

   MockAction::restartSequence();

   BehaviorTree tree;
   tree.getRoot().add( new MockAction( executionSequence, initsCount, deinitsCount ) );

   // execute it twice - the action should only be executed once
   BehaviorTreeRunner treeRunner( tree );

   CPPUNIT_ASSERT_EQUAL( 0, executionSequence );
   CPPUNIT_ASSERT_EQUAL( 1, initsCount );
   CPPUNIT_ASSERT_EQUAL( 0, deinitsCount );

   treeRunner.execute();
   CPPUNIT_ASSERT_EQUAL( 1, executionSequence );
   CPPUNIT_ASSERT_EQUAL( 1, initsCount );
   CPPUNIT_ASSERT_EQUAL( 1, deinitsCount );

   treeRunner.execute();
   CPPUNIT_ASSERT_EQUAL( 1, executionSequence );
   CPPUNIT_ASSERT_EQUAL( 1, initsCount );
   CPPUNIT_ASSERT_EQUAL( 1, deinitsCount );
}

///////////////////////////////////////////////////////////////////////////////

TEST( BehaviorTree, sequence )
{
   int executionSequence[3];
   int initsCount[3];
   int deinitsCount[3];
   memset( executionSequence, 0, sizeof( int ) * 3 );
   memset( initsCount, 0, sizeof( int ) * 3 );
   memset( deinitsCount, 0, sizeof( int ) * 3 );

   MockAction::restartSequence();

   BehaviorTree tree;
   tree.getRoot().add( new MockAction( executionSequence[0], initsCount[0], deinitsCount[0] ) );
   tree.getRoot().add( new MockAction( executionSequence[1], initsCount[1], deinitsCount[1] ) );
   tree.getRoot().add( new MockAction( executionSequence[2], initsCount[2], deinitsCount[2] ) );

   // execute it twice - the action should only be executed once
   BehaviorTreeRunner treeRunner( tree );

   CPPUNIT_ASSERT_EQUAL( 0, executionSequence[0] );
   CPPUNIT_ASSERT_EQUAL( 0, executionSequence[1] );
   CPPUNIT_ASSERT_EQUAL( 0, executionSequence[2] );
   CPPUNIT_ASSERT_EQUAL( 1, initsCount[0] );
   CPPUNIT_ASSERT_EQUAL( 0, initsCount[1] );
   CPPUNIT_ASSERT_EQUAL( 0, initsCount[2] );
   CPPUNIT_ASSERT_EQUAL( 0, deinitsCount[0] );
   CPPUNIT_ASSERT_EQUAL( 0, deinitsCount[1] );
   CPPUNIT_ASSERT_EQUAL( 0, deinitsCount[2] );

   treeRunner.execute();
   CPPUNIT_ASSERT_EQUAL( 1, executionSequence[0] );
   CPPUNIT_ASSERT_EQUAL( 0, executionSequence[1] );
   CPPUNIT_ASSERT_EQUAL( 0, executionSequence[2] );
   CPPUNIT_ASSERT_EQUAL( 1, initsCount[0] );
   CPPUNIT_ASSERT_EQUAL( 1, initsCount[1] );
   CPPUNIT_ASSERT_EQUAL( 0, initsCount[2] );
   CPPUNIT_ASSERT_EQUAL( 1, deinitsCount[0] );
   CPPUNIT_ASSERT_EQUAL( 0, deinitsCount[1] );
   CPPUNIT_ASSERT_EQUAL( 0, deinitsCount[2] );

   treeRunner.execute();
   CPPUNIT_ASSERT_EQUAL( 1, executionSequence[0] );
   CPPUNIT_ASSERT_EQUAL( 2, executionSequence[1] );
   CPPUNIT_ASSERT_EQUAL( 0, executionSequence[2] );  
   CPPUNIT_ASSERT_EQUAL( 1, initsCount[0] );
   CPPUNIT_ASSERT_EQUAL( 1, initsCount[1] );
   CPPUNIT_ASSERT_EQUAL( 1, initsCount[2] );
   CPPUNIT_ASSERT_EQUAL( 1, deinitsCount[0] );
   CPPUNIT_ASSERT_EQUAL( 1, deinitsCount[1] );
   CPPUNIT_ASSERT_EQUAL( 0, deinitsCount[2] );

   treeRunner.execute();
   CPPUNIT_ASSERT_EQUAL( 1, executionSequence[0] );
   CPPUNIT_ASSERT_EQUAL( 2, executionSequence[1] );
   CPPUNIT_ASSERT_EQUAL( 3, executionSequence[2] );
   CPPUNIT_ASSERT_EQUAL( 1, initsCount[0] );
   CPPUNIT_ASSERT_EQUAL( 1, initsCount[1] );
   CPPUNIT_ASSERT_EQUAL( 1, initsCount[2] );
   CPPUNIT_ASSERT_EQUAL( 1, deinitsCount[0] );
   CPPUNIT_ASSERT_EQUAL( 1, deinitsCount[1] );
   CPPUNIT_ASSERT_EQUAL( 1, deinitsCount[2] );

   treeRunner.execute();
   CPPUNIT_ASSERT_EQUAL( 1, executionSequence[0] );
   CPPUNIT_ASSERT_EQUAL( 2, executionSequence[1] );
   CPPUNIT_ASSERT_EQUAL( 3, executionSequence[2] );
   CPPUNIT_ASSERT_EQUAL( 1, initsCount[0] );
   CPPUNIT_ASSERT_EQUAL( 1, initsCount[1] );
   CPPUNIT_ASSERT_EQUAL( 1, initsCount[2] );
   CPPUNIT_ASSERT_EQUAL( 1, deinitsCount[0] );
   CPPUNIT_ASSERT_EQUAL( 1, deinitsCount[1] );
   CPPUNIT_ASSERT_EQUAL( 1, deinitsCount[2] );
}

///////////////////////////////////////////////////////////////////////////////

TEST( BehaviorTree, parallel )
{
   int executionSequence[3];
   int initsCount[3];
   int deinitsCount[3];
   memset( executionSequence, 0, sizeof( int ) * 3 );
   memset( initsCount, 0, sizeof( int ) * 3 );
   memset( deinitsCount, 0, sizeof( int ) * 3 );

   MockAction::restartSequence();

   BehaviorTree tree;
   BehTreeParallel* parallel = new BehTreeParallel();
   tree.getRoot().add( parallel );

   parallel->add( new MockAction( executionSequence[0], initsCount[0], deinitsCount[0] ) );
   parallel->add( new MockAction( executionSequence[1], initsCount[1], deinitsCount[1] ) );
   parallel->add( new MockAction( executionSequence[2], initsCount[2], deinitsCount[2] ) );

   // execute it twice - the action should only be executed once
   BehaviorTreeRunner treeRunner( tree );
   CPPUNIT_ASSERT_EQUAL( 0, executionSequence[0] );
   CPPUNIT_ASSERT_EQUAL( 0, executionSequence[1] );
   CPPUNIT_ASSERT_EQUAL( 0, executionSequence[2] );
   CPPUNIT_ASSERT_EQUAL( 1, initsCount[0] );
   CPPUNIT_ASSERT_EQUAL( 1, initsCount[1] );
   CPPUNIT_ASSERT_EQUAL( 1, initsCount[2] );
   CPPUNIT_ASSERT_EQUAL( 0, deinitsCount[0] );
   CPPUNIT_ASSERT_EQUAL( 0, deinitsCount[1] );
   CPPUNIT_ASSERT_EQUAL( 0, deinitsCount[2] );

   // all three actions are executed during this single execution step
   treeRunner.execute();
   CPPUNIT_ASSERT_EQUAL( 1, executionSequence[0] );
   CPPUNIT_ASSERT_EQUAL( 2, executionSequence[1] );
   CPPUNIT_ASSERT_EQUAL( 3, executionSequence[2] );
   CPPUNIT_ASSERT_EQUAL( 1, initsCount[0] );
   CPPUNIT_ASSERT_EQUAL( 1, initsCount[1] );
   CPPUNIT_ASSERT_EQUAL( 1, initsCount[2] );
   CPPUNIT_ASSERT_EQUAL( 1, deinitsCount[0] );
   CPPUNIT_ASSERT_EQUAL( 1, deinitsCount[1] );
   CPPUNIT_ASSERT_EQUAL( 1, deinitsCount[2] );
}

///////////////////////////////////////////////////////////////////////////////

TEST( BehaviorTree, repeater )
{
   int executionSequence = 0;
   int initsCount = 0;
   int deinitsCount = 0;

   MockAction::restartSequence();

   BehTreeRepeater* repeater = new BehTreeRepeater();
   repeater->setDecoratedNode( new MockAction( executionSequence, initsCount, deinitsCount ) );
   BehaviorTree tree;
   tree.getRoot().add( repeater );

   // execute it twice - the action should only be executed once
   BehaviorTreeRunner treeRunner( tree );

   CPPUNIT_ASSERT_EQUAL( 0, executionSequence );
   CPPUNIT_ASSERT_EQUAL( 1, initsCount );
   CPPUNIT_ASSERT_EQUAL( 0, deinitsCount );

   treeRunner.execute();
   CPPUNIT_ASSERT_EQUAL( 1, executionSequence );
   CPPUNIT_ASSERT_EQUAL( 2, initsCount );
   CPPUNIT_ASSERT_EQUAL( 1, deinitsCount );

   treeRunner.execute();
   CPPUNIT_ASSERT_EQUAL( 2, executionSequence );
   CPPUNIT_ASSERT_EQUAL( 3, initsCount );
   CPPUNIT_ASSERT_EQUAL( 2, deinitsCount );

   treeRunner.execute();
   CPPUNIT_ASSERT_EQUAL( 3, executionSequence );
   CPPUNIT_ASSERT_EQUAL( 4, initsCount );
   CPPUNIT_ASSERT_EQUAL( 3, deinitsCount );
}

///////////////////////////////////////////////////////////////////////////////

TEST( BehaviorTree, selector )
{
   int executionSequence = 0;
   int initsCount = 0;
   int deinitsCount = 0;

   MockCondition* condition = new MockCondition( false );
   MockAction* action = new MockAction( executionSequence, initsCount, deinitsCount );
   BehTreeSelector* selector = new BehTreeSelector();
   selector->add( action );
   selector->setCondition( condition );

   BehaviorTree tree;
   tree.getRoot().add( selector );

   // execute it twice - the action should only be executed once
   BehaviorTreeRunner treeRunner( tree );

   {
      MockAction::restartSequence();

      // the condition evaluates false - the child won't be executed
      condition->m_flag = false;
      treeRunner.execute();
      CPPUNIT_ASSERT_EQUAL( 0, executionSequence );
      CPPUNIT_ASSERT_EQUAL( 0, initsCount );
      CPPUNIT_ASSERT_EQUAL( 0, deinitsCount );

      // the condition evaluates true - the child becomes active, runs, and gets deactivated
      condition->m_flag = true;
      treeRunner.restart();
      treeRunner.execute();
      CPPUNIT_ASSERT_EQUAL( 1, executionSequence );
      CPPUNIT_ASSERT_EQUAL( 1, initsCount );
      CPPUNIT_ASSERT_EQUAL( 1, deinitsCount );
   }


   {
      // now we make the child run a little while longer
      action->m_iterationsCount = 10;
      executionSequence = 0;
      initsCount = 0;
      deinitsCount = 0;
      MockAction::restartSequence();
      treeRunner.restart();

      // again, the condition evaluates false, so the action will not be executed
      condition->m_flag = false;
      treeRunner.execute();
      CPPUNIT_ASSERT_EQUAL( 0, executionSequence );
      CPPUNIT_ASSERT_EQUAL( 0, initsCount );
      CPPUNIT_ASSERT_EQUAL( 0, deinitsCount );

      // now the condition evaluates true, and the action starts being executed
      condition->m_flag = true;
      treeRunner.restart();
      treeRunner.execute();
      CPPUNIT_ASSERT_EQUAL( 1, executionSequence );
      CPPUNIT_ASSERT_EQUAL( 1, initsCount );
      CPPUNIT_ASSERT_EQUAL( 0, deinitsCount );
      CPPUNIT_ASSERT_EQUAL( 9, action->m_remainingIterations );

      // it keeps on running ( we've got 9 iterations left at this point )
      treeRunner.execute();
      CPPUNIT_ASSERT_EQUAL( 2, executionSequence );
      CPPUNIT_ASSERT_EQUAL( 1, initsCount );
      CPPUNIT_ASSERT_EQUAL( 0, deinitsCount );
      CPPUNIT_ASSERT_EQUAL( 8, action->m_remainingIterations );

      // now, still 8 iterations to go, the condition evaluates false, which shuts the whole system down
      condition->m_flag = false;
      treeRunner.execute();
      CPPUNIT_ASSERT_EQUAL( 2, executionSequence );
      CPPUNIT_ASSERT_EQUAL( 1, initsCount );
      CPPUNIT_ASSERT_EQUAL( 1, deinitsCount );
      CPPUNIT_ASSERT_EQUAL( 8, action->m_remainingIterations );

      // but as soon as the condition evaluates true again, the action gets reinitialized
      condition->m_flag = true;
      treeRunner.restart();
      treeRunner.execute();
      CPPUNIT_ASSERT_EQUAL( 3, executionSequence );
      CPPUNIT_ASSERT_EQUAL( 2, initsCount );
      CPPUNIT_ASSERT_EQUAL( 1, deinitsCount );
      CPPUNIT_ASSERT_EQUAL( 9, action->m_remainingIterations );      // notice the counter started back from 10, and after evaluating this tick, it went down to 9

   }
}

///////////////////////////////////////////////////////////////////////////////

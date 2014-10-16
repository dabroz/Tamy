#include "core-AI\BehaviorTreeRunner.h"
#include "core-AI\BehaviorTree.h"
#include "core-AI\BehTreeNode.h"
#include "core-AI\BehTreeComposite.h"
#include "core-AI\BehTreeVariable.h"
#include "core\RuntimeData.h"


///////////////////////////////////////////////////////////////////////////////

BehaviorTreeRunner::BehaviorTreeRunner( BehaviorTree& tree, void* context )
   : m_tree( tree )
   , m_root( &tree.getRoot() )
   , m_runtimeData( new RuntimeDataBuffer() )
   , m_context( context )
{
   // create a layout of runtime variables
   m_tree.createLayout( this );

   // initialize the tree
   m_root->initialize( *this );
}

///////////////////////////////////////////////////////////////////////////////

BehaviorTreeRunner::~BehaviorTreeRunner()
{
   // deinitialize the runtime context
   if ( m_root )
   {
      m_root->deinitialize( *this );
   }

   m_tree.destroyLayout( this );

   delete m_runtimeData;
   m_runtimeData = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeRunner::restart()
{
   // deinitialize the previous runtime context
   if ( m_root )
   {
      m_root->deinitialize( *this );
   }

   m_root = &m_tree.getRoot();

   // initialize the tree
   m_root->initialize( *this );
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeRunner::execute()
{
   if ( !m_root )
   {
      return;
   }

   BehTreeNode::Result result = m_root->execute( *this );
   if ( result != BehTreeNode::IN_PROGRESS )
   {
      // deinitialize the runtime context
      m_root->deinitialize( *this );

      m_root = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeRunner::onNodeAdded( BehTreeNode* parentNode, int insertionIdx, BehTreeNode* node )
{
   node->createLayout( *this );
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeRunner::onNodeRemoved( BehTreeNode* parentNode, BehTreeNode* node )
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeRunner::onNodeChanged( BehTreeNode* node )
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeRunner::onVariableAdded( BehaviorTreeVariable* var )
{
   var->initializeLayout( this );
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeRunner::onVariableRemoved( BehaviorTreeVariable* var )
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

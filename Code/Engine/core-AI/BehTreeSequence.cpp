#include "core-AI\BehTreeSequence.h"
#include "core-AI\BehaviorTreeRunner.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BehTreeSequence );
   PARENT( BehTreeComposite );
   PROPERTY_EDIT( "Never fail", bool, m_neverFail );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BehTreeSequence::BehTreeSequence()
   : m_neverFail( false )
{}

///////////////////////////////////////////////////////////////////////////////

void BehTreeSequence::createLayout( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data();
   data.registerVar( m_activeNodeIdx );

   BehTreeComposite::createLayout( runner );
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeSequence::initialize( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data();
   data[m_activeNodeIdx] = 0;

   if ( !m_nodes.empty() )
   {
      BehTreeNode* node = m_nodes[0];
      node->initialize( runner );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeSequence::deinitialize( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data();
   uint activeNode = data[m_activeNodeIdx];

   // deinitialize the active node
   if ( !m_nodes.empty() && activeNode < m_nodes.size() )
   {
      BehTreeNode* node = m_nodes[activeNode];
      node->deinitialize( runner );
   }
}

///////////////////////////////////////////////////////////////////////////////

BehTreeNode::Result BehTreeSequence::execute( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data();
   uint activeNodeIdx = data[ m_activeNodeIdx ];

   if ( m_nodes.empty() )
   {
      return FINISHED;
   }

   BehTreeNode* node = m_nodes[activeNodeIdx];
   BehTreeNode::Result result = node->execute( runner );

   if ( result != IN_PROGRESS )
   {
      // deactivate a node that's finished
      node->deinitialize( runner );
   }

   switch( result )
   {
   case FINISHED:
      {
         ++activeNodeIdx;
         data[ m_activeNodeIdx ] = activeNodeIdx;

         if ( activeNodeIdx == m_nodes.size() )
         {
            return FINISHED;
         }
         else
         {
            // activate the next node
            BehTreeNode* node = m_nodes[activeNodeIdx];
            node->initialize( runner );

            return IN_PROGRESS;
         }
      }

   case FAILED:
      {
      return m_neverFail ? FINISHED : FAILED;
      }
   };

   return IN_PROGRESS;
}

///////////////////////////////////////////////////////////////////////////////

#include "core-AI\BehTreeParallel.h"
#include "core-AI\BehaviorTreeRunner.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BehTreeParallel );
   PARENT( BehTreeComposite );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

void BehTreeParallel::createLayout( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data();
   data.registerVar( m_finishedNodes );

   BehTreeComposite::createLayout( runner );
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeParallel::initialize( BehaviorTreeRunner& runner ) const
{
   // initialize runtime data
   RuntimeDataBuffer& data = runner.data();

   uint count = m_nodes.size();
   bool* finishedNodes = new bool[ count ];
   memset( finishedNodes, false, sizeof( bool ) * count );

   data[m_finishedNodes] = finishedNodes;


   // initialize the children
   count = m_nodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      BehTreeNode* node = m_nodes[i];
      node->initialize( runner );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeParallel::deinitialize( BehaviorTreeRunner& runner ) const
{
   // deinitialize the children
   uint count = m_nodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      BehTreeNode* node = m_nodes[i];
      node->deinitialize( runner );
   }

   // delete runtime data
   RuntimeDataBuffer& data = runner.data();

   bool* finishedNodes = data[m_finishedNodes];
   delete [] finishedNodes;
   data[m_finishedNodes] = NULL;
}

///////////////////////////////////////////////////////////////////////////////

BehTreeNode::Result BehTreeParallel::execute( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data();
   
   bool* finishedNodes = data[m_finishedNodes];

   // Start by assuming that the node will be finished by the end of this method.
   // Now two things might happen:
   //   - a child might fail, in which case we'll EXIT IMMEDIATELY FROM THE LOOP, without bothering
   //     about the value of this variable
   //   - one of the children can return IN_PROGRESS, in which case we'll alter the value of this variable
   //     to IN_PROGRESS. Because the value can only be changed to IN_PROGRESS, even if subsequent children
   //     return FINISHED, the value of this variable will remain IN_PROGRESS.
   BehTreeNode::Result result = FINISHED;

   uint count = m_nodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( finishedNodes[i] )
      {
         continue;
      }

      BehTreeNode* node = m_nodes[i];
      BehTreeNode::Result childResult = node->execute( runner );

      switch( childResult )
      {
      case FINISHED:
         {
            // mark this node as finished and proceed with remaining nodes
            finishedNodes[i] = true;
            break;
         }

      case FAILED:
         {
            // the entire node fails immediately
            return FAILED;                         // <-- we're exiting here without evaluating the remaining nodes
            break;
         }

      case IN_PROGRESS:
         {
            // Keep on working, and change the exit status to IN_PROGRESS.
            result = IN_PROGRESS;
            break;
         }
      }
   }

   return result;
}

///////////////////////////////////////////////////////////////////////////////

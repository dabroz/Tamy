#include "core-AI\BlendTreeComposite.h"
#include "core-AI\BlendTree.h"
#include "core-AI\BlendTreeListener.h"
#include "core-AI\BlendTreePlayer.h"
#include "core-AI\BlendTreePlayerListener.h"
#include "core-AI\BlendTreeSyncProfile.h"
#include "core\List.h"
#include "core\Algorithms.h"
#include "core\Log.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_OBJECT( BlendTreeComposite );
   PARENT( BlendTreeNode );
   PROPERTY( Array< BlendTreeNode* >,  m_nodes );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BlendTreeComposite::BlendTreeComposite( const char* name )
   : BlendTreeNode( name )
{
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeComposite::~BlendTreeComposite()
{
   uint count = m_nodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      BlendTreeNode* node = m_nodes[i];
      if ( node )
      {
         node->removeReference();
      }
   }
   m_nodes.clear();
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeComposite::add( BlendTreeNode* node )
{
   if ( !node )
   {
      return;
   }

   // look for duplicates
   uint count = m_nodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      BlendTreeNode* checkedNode = m_nodes[i];
      if ( checkedNode == node )
      {
         return;
      }
   }

   uint insertionPos = m_nodes.size();
   m_nodes.push_back( node );

   // notify implementation
   onNodeInserted( node, insertionPos );

   if ( m_tree )
   {
      node->setHostTree( m_tree );

      // notify listeners
      for ( List< BlendTreeListener* >::iterator it = m_tree->m_listeners.begin(); !it.isEnd(); ++it )
      {
         BlendTreeListener* listener = *it;
         listener->onNodeAdded( this, insertionPos, node );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeComposite::insert( int index, BlendTreeNode* node )
{
   if ( !node )
   {
      return;
   }

   // look for duplicates
   uint count = m_nodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      BlendTreeNode* checkedNode = m_nodes[i];
      if ( checkedNode == node )
      {
         return;
      }
   }

   index = clamp<int>( index, 0, m_nodes.size() );
   m_nodes.insert( index, node );
   node->setHostTree( m_tree );

   // notify implementation
   onNodeInserted( node, index );

   // notify listeners
   if ( m_tree )
   {
      for ( List< BlendTreeListener* >::iterator it = m_tree->m_listeners.begin(); !it.isEnd(); ++it )
      {
         BlendTreeListener* listener = *it;
         listener->onNodeAdded( this, index, node );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeComposite::remove( BlendTreeNode* node )
{
   if ( !node )
   {
      return;
   }

   uint count = m_nodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      BlendTreeNode* checkedNode = m_nodes[i];
      if ( checkedNode == node )
      {
         // found it

         // notify the implementation
         onNodeRemoved( node, i );

         // first - notify listeners
         if ( m_tree )
         {
            for ( List< BlendTreeListener* >::iterator it = m_tree->m_listeners.begin(); !it.isEnd(); ++it )
            {
               BlendTreeListener* listener = *it;
               listener->onNodeRemoved( this, node );
            }
         }

         // next - remove the node
         checkedNode->setHostTree( NULL );
         checkedNode->removeReference();
         m_nodes.remove( i );
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeComposite::clear()
{
   uint count = m_nodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      BlendTreeNode* node = m_nodes[i];

      // first - notify listeners
      for ( List< BlendTreeListener* >::iterator it = m_tree->m_listeners.begin(); !it.isEnd(); ++it )
      {
         BlendTreeListener* listener = *it;
         listener->onNodeRemoved( this, node );
      }

      // next - remove the node
      node->removeReference();
   }

   m_nodes.clear();
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeComposite::pullStructure( BlendTreeListener* listener )
{
   uint count = m_nodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      BlendTreeNode* childNode = m_nodes[i];
      listener->onNodeAdded( this, i, childNode );
      childNode->pullStructure( listener );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeComposite::onHostTreeSet( BlendTree* tree )
{
   uint count = m_nodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      BlendTreeNode* childNode = m_nodes[i];
      childNode->setHostTree( tree );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeComposite::activateChildNode( BlendTreePlayer* player, const BlendTreeNode* activatedChild ) const
{
   RuntimeDataBuffer& data = player->data();
   Array< const BlendTreeNode* >* activeNodesArr = data[m_activeNodes];
   activeNodesArr->push_back( activatedChild );

   activatedChild->activateNode( player );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeComposite::deactivateChildNode( BlendTreePlayer* player, const BlendTreeNode* deactivatedChild ) const
{
   RuntimeDataBuffer& data = player->data();
   Array< const BlendTreeNode* >* activeNodesArr = data[m_activeNodes];
   uint count = activeNodesArr->size();
   for ( uint i = 0; i < count; ++i )
   {
      const BlendTreeNode* child = (*activeNodesArr)[i];
      if ( child == deactivatedChild )
      {
         deactivatedChild->deactivateNode( player );

         activeNodesArr->remove( i );
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeComposite::onDeactivateNode( BlendTreePlayer* player ) const
{
   // deactivate all children
   RuntimeDataBuffer& data = player->data();
   Array< const BlendTreeNode* >* activeNodesArr = data[m_activeNodes];
   uint count = activeNodesArr->size();
   for ( uint i = 0; i < count; ++i )
   {
      const BlendTreeNode* child = ( *activeNodesArr )[i];
      child->deactivateNode( player );
   }

   activeNodesArr->clear();
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeComposite::initializeLayout( BlendTreePlayer* player ) const
{
   BlendTreeNode::initializeLayout( player );

   // create a runtime array for storing references to currently active nodes
   RuntimeDataBuffer& data = player->data();
   data.registerVar( m_activeNodes );
   data[m_activeNodes] = new Array< const BlendTreeNode* >( 2 );

   // pass the call down the hierarchy
   uint count = m_nodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      BlendTreeNode* childNode = m_nodes[i];
      childNode->initializeLayout( player );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeComposite::deinitializeLayout( BlendTreePlayer* player ) const
{
   // deinitialize the hierarchy below first
   uint count = m_nodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      BlendTreeNode* childNode = m_nodes[i];
      childNode->deinitializeLayout( player );
   }

   // delete the runtime array for storing references to currently active nodes
   RuntimeDataBuffer& data = player->data();
   Array< const BlendTreeNode* >* activeNodesArr = data[m_activeNodes];
   delete activeNodesArr;
   data.unregisterVar( m_activeNodes );

   BlendTreeNode::deinitializeLayout( player );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeComposite::updateLogic( BlendTreePlayer* player ) const
{
   if ( m_nodes.empty() )
   {
      // nothing to blend
      return;
   }

   BlendTreeNode::updateLogic( player );

   // pass the call down the hierarchy
   RuntimeDataBuffer& data = player->data();
   Array< const BlendTreeNode* >* activeNodesArr = data[m_activeNodes];
   const uint count = activeNodesArr->size();

   for ( uint i = 0; i < count; ++i )
   {
      const BlendTreeNode* child = (*activeNodesArr)[i];
      child->updateLogic( player );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeComposite::generateTreeSyncProfile( BlendTreePlayer* player, BlendTreeSyncProfile& outSyncData ) const
{
   if ( m_nodes.empty() )
   {
      // nothing to blend
      return;
   }

   // pass the call down the hierarchy first ( we want to start from the leaves and propagate the call up the hierarchy towards the tree root )
   RuntimeDataBuffer& data = player->data();
   Array< const BlendTreeNode* >* activeNodesArr = data[m_activeNodes];
   uint count = activeNodesArr->size();
   for ( uint i = 0; i < count; ++i )
   {
      const BlendTreeNode* child = (*activeNodesArr)[i];
      child->generateTreeSyncProfile( player, outSyncData );
   }

   BlendTreeNode::generateTreeSyncProfile( player, outSyncData );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeComposite::synchronizeNodeToTree( BlendTreePlayer* player, const BlendTreeSyncProfile& syncData ) const
{
   if ( m_nodes.empty() )
   {
      // nothing to blend
      return;
   }

   // pass the call down the hierarchy first ( we want to start from the leaves and propagate the call up the hierarchy towards the tree root )
   RuntimeDataBuffer& data = player->data();
   Array< const BlendTreeNode* >* activeNodesArr = data[m_activeNodes];
   uint count = activeNodesArr->size();
   for ( uint i = 0; i < count; ++i )
   {
      const BlendTreeNode* child = (*activeNodesArr)[i];
      child->synchronizeNodeToTree( player, syncData );
   }

   BlendTreeNode::synchronizeNodeToTree( player, syncData );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeComposite::samplePose( BlendTreePlayer* player, float timeDelta ) const
{
   if ( m_nodes.empty() )
   {
      // nothing to blend
      return;
   }

   // pass the call down the hierarchy first - pose sampling takes place from the leaves, rootwards
   RuntimeDataBuffer& data = player->data();
   Array< const BlendTreeNode* >* activeNodesArr = data[m_activeNodes];
   uint count = activeNodesArr->size();
   for ( uint i = 0; i < count; ++i )
   {
      const BlendTreeNode* child = (*activeNodesArr)[i];
      child->samplePose( player, timeDelta );
   }

   BlendTreeNode::samplePose( player, timeDelta );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeComposite::pullRuntimeStructure( BlendTreePlayer* player, BlendTreePlayerListener* listener ) const
{
   // inform about the state of this composite and its children
   listener->onNodeStateChanged( player, this );

   uint count = m_nodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      BlendTreeNode* child = m_nodes[i];
      child->pullRuntimeStructure( player, listener );
   }
}

///////////////////////////////////////////////////////////////////////////////

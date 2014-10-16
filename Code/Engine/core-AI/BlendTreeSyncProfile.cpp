#include "core-AI\BlendTreeSyncProfile.h"
#include "core-AI\BlendTreeNode.h"
#include "core\Log.h"


///////////////////////////////////////////////////////////////////////////////

BlendTreeSyncProfile::BlendTreeSyncProfile()
   : m_eventsCount( 0 )
{}

///////////////////////////////////////////////////////////////////////////////

int BlendTreeSyncProfile::submit( BlendTreeEvent* syncPoint, float progress )
{
   // check if this event has already been registered
   for ( uint i = 0; i < m_eventsCount; ++i )
   {
      if ( m_btEvent[i] == syncPoint )
      {
         // already there
         return i;
      }
   }

   // check if we have enough room to accommodate one more sync channel
   if ( m_eventsCount >= MAX_SIMULTANEOUS_SYNCHRONIZATION_POINTS )
   {
      LOG( "Maximum number of simultaneous synchronization points has been exceeded" );
      return -1;
   }
   
   int eventIdx = m_eventsCount;
   ++m_eventsCount;

   m_btEvent[eventIdx] = syncPoint;
   m_progress[eventIdx] = progress;

   return eventIdx;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeSyncProfile::reset()
{
   m_eventsCount = 0;
}

///////////////////////////////////////////////////////////////////////////////

int BlendTreeSyncProfile::getSyncPointIdx( BlendTreeEvent* syncPoint ) const
{
   for ( uint i = 0; i < m_eventsCount; ++i )
   {
      if ( m_btEvent[i] == syncPoint )
      {
         return i;
      }
   }

   // sync point not registered with the profile
   return -1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BlendTreeNodeSyncProfile::BlendTreeNodeSyncProfile( const BlendTreeNode* parentNode )
   : m_parentNode( parentNode )
   , m_nodesCount( 0 )
{
   reset();
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeNodeSyncProfile::reset()
{
   m_nodesCount = 0;
   memset( m_nodeSyncData, NULL, sizeof( const BlendTreeNodeSyncProfile* ) * MAX_SIMULTANEOUS_SYNCHRONIZATION_POINTS );
   memset( m_timeRemaining, 0, sizeof( float ) * MAX_SIMULTANEOUS_SYNCHRONIZATION_POINTS );
   memset( m_totalBlendWeight, 0, sizeof( float ) * MAX_SIMULTANEOUS_SYNCHRONIZATION_POINTS );
   memset( m_syncPointIdx, -1, sizeof( char ) * MAX_SIMULTANEOUS_SYNCHRONIZATION_POINTS );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeNodeSyncProfile::setSyncPoint( int idx, float timeRemaining )
{
   if ( idx < 0 )
   {
      // We're trying to register a node to a sync event that doesn't exist in the tree profile.
      // This might happen if we have for instance overflown the number of simultaneously active
      // sync point, or if this is the first frame a given node is active
      // and it didn't have a chance to submit its info to the tree sync profile yet.
      return;
   }

   m_timeRemaining[idx] = timeRemaining;
   m_totalBlendWeight[idx] = 1.0f;
   m_nodeSyncData[idx] = NULL;
   if ( m_syncPointIdx[m_nodesCount] < 0 )
   {
      m_syncPointIdx[m_nodesCount] = idx;
      m_contributingNodePtrs[m_nodesCount] = NULL;
   }
   ++m_nodesCount;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeNodeSyncProfile::mergeWith( const BlendTreeNodeSyncProfile* rhs, float blendWeight )
{
   m_nodeSyncData[m_nodesCount] = rhs;
   
   for ( byte i = 0; i < MAX_SIMULTANEOUS_SYNCHRONIZATION_POINTS; ++i )
   {
      if ( rhs->m_timeRemaining[i] > 0.0f )
      {
         m_timeRemaining[i] += rhs->m_timeRemaining[i] * blendWeight;
         m_totalBlendWeight[i] += blendWeight;

         m_syncPointIdx[m_nodesCount] = m_syncPointIdx[m_nodesCount] < 0 ? i : m_syncPointIdx[m_nodesCount];
      }
   }

   // memorize which node contributed the specified data, so that we can later on adjust its velocity
   m_contributingNodePtrs[m_nodesCount] = rhs->m_parentNode;

   ++m_nodesCount;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeNodeSyncProfile::commit()
{
   for ( byte syncPointIdx = 0; syncPointIdx < MAX_SIMULTANEOUS_SYNCHRONIZATION_POINTS; ++syncPointIdx )
   {
      if ( m_totalBlendWeight[syncPointIdx] > 0.0f )
      {
         m_timeRemaining[syncPointIdx] /= m_totalBlendWeight[syncPointIdx];
      }
   }

   for ( uint nodeIdx = 0; nodeIdx < m_nodesCount; ++nodeIdx )
   {
      const BlendTreeNodeSyncProfile* rhs = m_nodeSyncData[nodeIdx];
      float newPlaybackSpeed = 1.0f;
      if ( rhs )
      {
         int syncPointIdx = m_syncPointIdx[nodeIdx];
         if ( syncPointIdx >= 0 && m_timeRemaining[syncPointIdx] > 0.0f )
         {
            newPlaybackSpeed = rhs->m_timeRemaining[syncPointIdx] / m_timeRemaining[syncPointIdx];
         }
      }

      m_playbackSpeed[nodeIdx] = newPlaybackSpeed;
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeNodeSyncProfile::applyVelocityChanges( BlendTreePlayer* player )
{
   for ( uint nodeIdx = 0; nodeIdx < m_nodesCount; ++nodeIdx )
   {
      const BlendTreeNode* contributingNode = m_contributingNodePtrs[nodeIdx];
      if ( contributingNode )
      {
         contributingNode->setPlaybackSpeed( player, m_playbackSpeed[nodeIdx] );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

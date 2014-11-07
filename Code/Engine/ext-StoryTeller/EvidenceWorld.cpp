#include "ext-StoryTeller\EvidenceWorld.h"
#include "ext-StoryTeller\Evidence.h"
#include "ext-StoryTeller\EvidenceHolder.h"
#include "ext-StoryTeller\RenderedEvidence.h"
#include "core\GraphUtils.h"


///////////////////////////////////////////////////////////////////////////////

EvidenceWorld::EvidenceWorld( Evidence& goal, Graph< Evidence* >& evidenceGraph )
   : m_goal( goal )
   , m_evidenceGraph( evidenceGraph )
   , m_context( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

EvidenceWorld::~EvidenceWorld()
{
   const uint evidenceHoldersCount = m_evidenceHolders.size();
   for ( uint i = 0; i < evidenceHoldersCount; ++i )
   {
      delete m_evidenceHolders[i];
   }

   for ( List< Creator* >::iterator it = m_creators.begin(); !it.isEnd(); ++it )
   {
      delete *it;
   }

   m_context = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void EvidenceWorld::createEvidenceHoldersAndDistributeEvidence()
{
   // tie each piece of evidence to an EvidenceHolder
   const uint nodesCount = m_evidenceGraph.getNodesCount();
   for ( uint i = 0; i < nodesCount; ++i )
   {
      Evidence* pieceOfEvidence = m_evidenceGraph.getNode( i );

      EvidenceHolder* newEvidenceHolder = NULL;
      const uint evidenceHoldersCount = m_evidenceHolders.size();
      for ( uint j = 0; j < evidenceHoldersCount; ++j )
      {
         EvidenceHolder* evidenceHolder = m_evidenceHolders[j];
         if ( canHolderStoreEvidence( evidenceHolder, pieceOfEvidence ) )
         {
            newEvidenceHolder = evidenceHolder;
            break;
         }
      }

      if ( !newEvidenceHolder )
      {
         // we need to create a new EvidenceHolder
         const uint requiredLeadsCapacity = pieceOfEvidence->getLeadsCount();

         Array< Creator* > applicableCreators( m_creators.size() );
         for ( List< Creator* >::iterator it = m_creators.begin(); !it.isEnd(); ++it )
         {
            Creator* creator = *it;
            if ( creator->m_maxRevealedLeads >= requiredLeadsCapacity )
            {
               applicableCreators.push_back( creator );
            }
         }

         ASSERT_MSG( !applicableCreators.empty(), "No holder type capable of handling the specified piece of evidence was registered" );

         if ( !applicableCreators.empty() )
         {
            const uint randCreatorIdx = rand() % applicableCreators.size();
            newEvidenceHolder = applicableCreators[randCreatorIdx]->create();
            m_evidenceHolders.push_back( newEvidenceHolder );
         }
      }

      // assign the holder that piece of evidence
      if ( newEvidenceHolder )
      {
         newEvidenceHolder->addPieceOfEvidence( *pieceOfEvidence );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

bool EvidenceWorld::canHolderStoreEvidence( EvidenceHolder* evidenceHolder, Evidence* pieceOfEvidence ) const
{
   Array< int > connectedNodeIndices;
   m_evidenceGraph.traceIncomingEdges( pieceOfEvidence->getIdx(), connectedNodeIndices );
   const uint requiredLeadsCapacity = connectedNodeIndices.size();

   if ( evidenceHolder->getNumLeadsToGiveUp() < requiredLeadsCapacity )
   {
      return false;
   }

   // make sure the holder's not gonna incriminate himself by
   // passing along a lead to an evidence that he himself is holding
   for ( uint j = 0; j < requiredLeadsCapacity; ++j )
   {
      Evidence* leadEvidence = m_evidenceGraph.getNode( connectedNodeIndices[j] );
      EvidenceHolder* leadEvidenceHolder = findEvidenceHolder( *leadEvidence );
      if ( leadEvidenceHolder == evidenceHolder )
      {
         return false;
      }
   }

   return true;
}

///////////////////////////////////////////////////////////////////////////////

void EvidenceWorld::compile()
{
   Graph< Evidence*> radialGraph;
   GraphSearchInfo< Evidence* > searchParams;
   searchParams.m_start = m_goal.getIdx();
   searchParams.m_costFunc = &EvidenceWorld::edgeCostFunc;
   GraphUtils< Evidence* >::dijkstra( m_evidenceGraph, searchParams, radialGraph );

   // find an index of the goal node in the radial graph
   int remappedGoalIdx = radialGraph.findNodeIdx( &m_goal );
   if ( remappedGoalIdx < 0 )
   {
      ASSERT_MSG( remappedGoalIdx < 0, "The goal node was not included in the simplified graph version" );
      return;
   }

   // assigning the nodes weights depending on how many parents do they have
   ClearanceAssigner clearanceAssigner( radialGraph );
   GraphUtils< Evidence* >::bfs( radialGraph, remappedGoalIdx, clearanceAssigner );
}

///////////////////////////////////////////////////////////////////////////////

RenderedEvidence* EvidenceWorld::findRenderedEvidence( Evidence& evidenceNode ) const
{
   const uint evidenceHoldersCount = m_evidenceHolders.size();
   for ( uint i = 0; i < evidenceHoldersCount; ++i )
   {
      EvidenceHolder* evidenceHolder = m_evidenceHolders[i];
      const uint numEvidencePieces = evidenceHolder->getNumEvidencePieces();
      for ( uint j = 0; j < numEvidencePieces; ++j )
      {
         RenderedEvidence& renderedEvidence = evidenceHolder->getPieceOfEvidence( j );
         if ( &renderedEvidence.getEvidenceNode() == &evidenceNode )
         {
            return &renderedEvidence;
         }
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

EvidenceHolder* EvidenceWorld::findEvidenceHolder( Evidence& evidenceNode ) const
{
   const uint evidenceHoldersCount = m_evidenceHolders.size();
   for ( uint i = 0; i < evidenceHoldersCount; ++i )
   {
      EvidenceHolder* evidenceHolder = m_evidenceHolders[i];
      const uint numEvidencePieces = evidenceHolder->getNumEvidencePieces();
      for ( uint j = 0; j < numEvidencePieces; ++j )
      {
         RenderedEvidence& renderedEvidence = evidenceHolder->getPieceOfEvidence( j );
         if ( &renderedEvidence.getEvidenceNode() == &evidenceNode )
         {
            return evidenceHolder;
         }
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

void EvidenceWorld::collectLeads( const Evidence& evidenceNode, Array< RenderedEvidence* >& outLeads ) const
{
   Array< Evidence* > leads;
   evidenceNode.getLeads( leads );

   const uint count = leads.size();
   for ( uint i = 0; i < count; ++i )
   {
      RenderedEvidence* renderedEvidence = findRenderedEvidence( *leads[i] );
      if ( renderedEvidence != NULL )
      {
         outLeads.push_back( renderedEvidence );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

int EvidenceWorld::edgeCostFunc( const Graph< Evidence* >& graph, int startVtx, int endVtx )
{
   return 1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

EvidenceWorld::ClearanceAssigner::ClearanceAssigner( const Graph< Evidence* >& evidenceGraph )
   : m_evidenceGraph( evidenceGraph )
{}

///////////////////////////////////////////////////////////////////////////////

void EvidenceWorld::ClearanceAssigner::operator()( Evidence* pieceOfEvidence )
{
   int pieceOfEvidenceIdx = m_evidenceGraph.findNodeIdx( pieceOfEvidence );
   if ( pieceOfEvidenceIdx < 0 )
   {
      return;
   }

   // the pieceOfEvidence hasn't been included in this version of the graph
   const Array< GraphEdge* >& connections = m_evidenceGraph.getEdges( pieceOfEvidenceIdx );

   const uint count = connections.size();
   if ( count > 0 )
   {
      float partialClearanceVal = pieceOfEvidence->getClearanceValue() / ( float ) count;

      for ( uint i = 0; i < count; ++i )
      {
         const int precedingEvidenceIdx = connections[i]->m_endNodeIdx;
         Evidence* precedingEvidence = m_evidenceGraph.getNode( precedingEvidenceIdx );
         precedingEvidence->setClearanceValue( partialClearanceVal );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

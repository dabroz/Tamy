#include "ext-StoryTeller\Investigator.h"
#include "ext-StoryTeller\EvidenceWorld.h"
#include "ext-StoryTeller\Evidence.h"


///////////////////////////////////////////////////////////////////////////////

Investigator::Investigator( EvidenceWorld& world )
   : m_world( world )
   , m_goal( world.getGoal() )
{
   const Graph< Evidence* >& storyGraph = world.getEvidenceGraph();

   const uint count = storyGraph.getNodesCount();
   for ( uint i = 0; i < count; ++i )
   {
      m_evidenceGraph.addNode( storyGraph.getNode( i ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

bool Investigator::investigate( Evidence& pieceOfEvidence )
{
   const uint pieceOfEvidenceIdx = pieceOfEvidence.getIdx();
   const Array< GraphEdge* >& existingEvidenceEdges = m_evidenceGraph.getEdges( pieceOfEvidenceIdx );

   Array<int> newEvidence;
   const bool newEvidenceRevealed = pieceOfEvidence.giveUpConnection( existingEvidenceEdges, newEvidence );
   if ( newEvidenceRevealed )
   {
      const uint count = newEvidence.size();
      for ( uint i = 0; i < count; ++i )
      {
         // update the connections graph
         m_evidenceGraph.connect( newEvidence[i], pieceOfEvidenceIdx );
      }
   }

   return newEvidenceRevealed;
}

///////////////////////////////////////////////////////////////////////////////

bool Investigator::isGoalDiscovered() const
{
   float collectedClearance = 0.0f;
   const Array< GraphEdge* >& connections = m_evidenceGraph.getEdges( m_goal.getIdx() );
   const uint count = connections.size();
   for ( uint i = 0; i < count; ++i )
   {
      const int precedingEvidenceIdx = connections[i]->m_endNodeIdx;
      Evidence* precedingEvidence = m_evidenceGraph.getNode( precedingEvidenceIdx );

      collectedClearance += precedingEvidence->getClearanceValue();
   }

   return collectedClearance >= m_goal.getClearanceValue();
}

///////////////////////////////////////////////////////////////////////////////

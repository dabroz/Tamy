#include "ext-StoryTeller\Evidence.h"
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

Evidence::Evidence()
   : m_clearanceValue( 1.0f )
   , m_evidenceGraph( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

Evidence::~Evidence()
{
}

///////////////////////////////////////////////////////////////////////////////

void Evidence::addToGraph( Graph< Evidence* >& evidenceGraph )
{
   if ( &evidenceGraph == m_evidenceGraph )
   {
      return;
   }

   ASSERT_MSG( m_evidenceGraph == NULL, "Can't add the same piece of evidence to 2 different evidence graphs" );
   m_evidenceGraph = &evidenceGraph;
   m_nodeIdx = evidenceGraph.addNode( this );
}

///////////////////////////////////////////////////////////////////////////////

void Evidence::getLeads( Array< Evidence* >& outLeads ) const
{
   Array< int > revealedEvidence;
   m_evidenceGraph->traceIncomingEdges( m_nodeIdx, revealedEvidence );

   const uint count = revealedEvidence.size();
   outLeads.resize( count );
   for ( uint i = 0; i < count; ++i )
   {
      outLeads[i] = m_evidenceGraph->getNode( revealedEvidence[i] );
   }
}

///////////////////////////////////////////////////////////////////////////////

uint Evidence::getLeadsCount() const
{
   Array< int > revealedEvidence;
   m_evidenceGraph->traceIncomingEdges( m_nodeIdx, revealedEvidence );

   return revealedEvidence.size();
}

///////////////////////////////////////////////////////////////////////////////

bool Evidence::giveUpConnection( const Array< GraphEdge* >& existingEvidenceEdges, Array< int >& outRevealedEvidence ) const
{
   // sum up the revealed clearances - if they are greater or equal than the required clearance value,
   // then we can reveal the information
   float collectedClearanceValue = 0.0f;

   const Array<GraphEdge*>& evidenceOnThisNode = m_evidenceGraph->getEdges( m_nodeIdx );
   if ( evidenceOnThisNode.empty() )
   {
      // this pieceOfEvidence is a pawn - there's no evidence on him, so he can give up all the information
      // voluntarily
      collectedClearanceValue = m_clearanceValue;
   }
   else
   {
      // we need to count the clearance level and see what information we can hand
      const uint count = existingEvidenceEdges.size();
      for ( uint i = 0; i < count; ++i )
      {
         const int pieceOfEvidenceIdx = existingEvidenceEdges[i]->m_endNodeIdx;
         Evidence* pieceOfEvidence = m_evidenceGraph->getNode( pieceOfEvidenceIdx );
         collectedClearanceValue += pieceOfEvidence->getClearanceValue();
      }
   }

   if ( collectedClearanceValue >= m_clearanceValue )
   {
      // give up the connections
      m_evidenceGraph->traceIncomingEdges( m_nodeIdx, outRevealedEvidence );

      return true;
   }
   else
   {
      return false;
   }
}

///////////////////////////////////////////////////////////////////////////////

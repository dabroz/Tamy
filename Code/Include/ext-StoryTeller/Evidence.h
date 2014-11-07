/// @file   ext-StoryTeller\Evidence.h
/// @brief  a node in the evidence graph
#pragma once

#include "core\MemoryRouter.h"
#include "core\Graph.h"


///////////////////////////////////////////////////////////////////////////////

class Evidence
{
   DECLARE_ALLOCATOR( Evidence, AM_DEFAULT );

private:
   uint                          m_nodeIdx;
   Graph< Evidence* >*           m_evidenceGraph;

   float                         m_clearanceValue;

public:
   /**
    * Constructor.
    */
   Evidence();
   ~Evidence();

   /**
    * A clearance value specifies the value of prerequisite evidence that needs to be uncovered
    * before this piece of evidence can be considered revealed.
    *
    * @param value
    */
   inline void setClearanceValue( float value )
   {
      m_clearanceValue = value;
   }

   /**
    * Returns the clearance value for this piece of evidence.
    */
   inline float getClearanceValue() const
   {
      return m_clearanceValue;
   }

   /**
    * Adds this piece of evidence to the specified graph.
    *
    * @param evidenceGraph
    */
   void addToGraph( Graph< Evidence* >& evidenceGraph );

   /**
    * Returns the node id assigned to the node by its parent graph.
    */
   inline uint getIdx() const
   {
      return m_nodeIdx;
   }

   /**
    * Collects and returns the other pieces of evidence this piece of evidence points to.
    */
   void getLeads( Array< Evidence* >& outLeads ) const;

   /**
    * Returns a number of leads this piece of evidence points to.
    */
   uint getLeadsCount() const;

   /**
    * Presents the evidence node with the collected connections pointing to it ( outbound connections ),
    * expecting it to give up its connections ( inbound connections ).
    *
    * @param existingEvidenceEdges
    * @param outRevealedEvidence
    */
   bool giveUpConnection( const Array< GraphEdge* >& existingEvidenceEdges, Array< int >& outRevealedEvidence ) const;
};

///////////////////////////////////////////////////////////////////////////////

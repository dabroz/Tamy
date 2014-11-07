/// @file   ext-StoryTeller\Investigator.h
/// @brief  the investigator inspects the evidence graph, tracking the evidence to the goal
#pragma once

#include "core\Graph.h"


///////////////////////////////////////////////////////////////////////////////

class EvidenceWorld;
class Evidence;

///////////////////////////////////////////////////////////////////////////////

class Investigator
{
   DECLARE_ALLOCATOR( Investigator, AM_DEFAULT );

private:
   EvidenceWorld&                m_world;
   Graph< Evidence* >         m_evidenceGraph;
   Evidence&                  m_goal;

public:
   /**
    * Constructor.
    *
    * @param world
    */
   Investigator( EvidenceWorld& world );

   /**
    * Investigates the specified piece of evidence.
    */
   bool investigate( Evidence& pieceOfEvidence );

   /**
    * Tells if the inspector has discovered the goal.
    */
   bool isGoalDiscovered() const;

   /**
    * Returns the instance of EvidenceWorld the investigator is operating on.
    */
   inline EvidenceWorld& getWorld() const
   {
      return m_world;
   }
};

///////////////////////////////////////////////////////////////////////////////

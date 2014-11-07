/// @file   ext-StoryTeller\RenderedEvidence.h
/// @brief  rendered evidence instance
#pragma once

#include "core\MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

class EvidenceHolder;
class Evidence;
class Investigator;

///////////////////////////////////////////////////////////////////////////////

class RenderedEvidence
{
   DECLARE_ALLOCATOR( RenderedEvidence, AM_DEFAULT );

protected:
   EvidenceHolder*         m_owner;

private:
   Evidence*               m_evidenceNode;

public:
   /**
    * Constructor.
    */
   RenderedEvidence();
   virtual ~RenderedEvidence();

   /**
    * Initializes the rendering.
    */
   void init( Evidence& evidenceNode, EvidenceHolder* owner );

   /**
    * Returns the evidence node this rendering represents.
    */
   inline const Evidence& getEvidenceNode() const {
      return *m_evidenceNode;
   }

   /**
    * Returns the holder of this piece of evidence.
    */
   inline const EvidenceHolder& getOwner() const {
      return *m_owner;
   }

   /**
    * Investigates this piece of evidence.
    *
    * @param investigator
    */
   bool investigate( Investigator& investigator );

protected:
   /**
    * The investigator will call this method when he asks about this piece of evidence.
    *
    * @param investigates
    */
   virtual void onAsked( Investigator& investigator ) {}

   /**
    * The investigator will call this method when the leading evidence is not to be turned up
    * during this interrogation.
    *
    * @param investigator
    */
   virtual void onAnswerDenied( Investigator& investigator ) {}

   /**
    * The investigator will call this method when 'giveUpConnections' returns true.
    *
    * It's where we implement the logic related to revealing of this piece of evidence.
    *
    * @param investigator
    * @param leads
    */
   virtual void onRevealed( Investigator& investigator, const EvidenceHolder& interrogated ) {}

};

///////////////////////////////////////////////////////////////////////////////

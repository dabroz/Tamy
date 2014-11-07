#include "ext-StoryTeller\RenderedEvidence.h"
#include "ext-StoryTeller\Investigator.h"
#include "ext-StoryTeller\EvidenceWorld.h"


///////////////////////////////////////////////////////////////////////////////

RenderedEvidence::RenderedEvidence()
   : m_owner( NULL )
   , m_evidenceNode( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

RenderedEvidence::~RenderedEvidence()
{
}

///////////////////////////////////////////////////////////////////////////////

void RenderedEvidence::init( Evidence& evidenceNode, EvidenceHolder* owner )
{
   m_evidenceNode = &evidenceNode;
   m_owner = owner;
}

///////////////////////////////////////////////////////////////////////////////

bool RenderedEvidence::investigate( Investigator& investigator )
{
   onAsked( investigator );

   bool interrogationSuccessfull = investigator.investigate( *m_evidenceNode );
   if ( interrogationSuccessfull )
   {
      EvidenceWorld& world = investigator.getWorld();
      Array< RenderedEvidence* > leads;
      world.collectLeads( getEvidenceNode(), leads );

      const uint count = leads.size();
      for ( uint i = 0; i < count; ++i )
      {
         RenderedEvidence* newLead = leads[i];
         newLead->onRevealed( investigator, getOwner() );
      }
   }
   else
   {
      onAnswerDenied( investigator );
   }

   return interrogationSuccessfull;
}

///////////////////////////////////////////////////////////////////////////////

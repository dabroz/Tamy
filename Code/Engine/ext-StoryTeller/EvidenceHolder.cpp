#include "ext-StoryTeller\EvidenceHolder.h"
#include "ext-StoryTeller\Evidence.h"
#include "ext-StoryTeller\RenderedEvidence.h"
#include "core\Assert.h"
#include "core\Algorithms.h"


///////////////////////////////////////////////////////////////////////////////

EvidenceHolder::EvidenceHolder()
   : m_name( "<<no name>>" )
   , m_numLeadsToGiveUp( 0 )
{}

///////////////////////////////////////////////////////////////////////////////

EvidenceHolder::~EvidenceHolder()
{
   const uint count = m_renderedEvidence.size();
   for ( uint i = 0; i < count; ++i )
   {
      RenderedEvidence* evidence = m_renderedEvidence[i];
      delete evidence;
   }
   m_renderedEvidence.clear();

   const uint creatorsCount = m_creators.size();
   for ( uint i = 0; i < creatorsCount; ++i )
   {
      delete m_creators[i];
   }
}

///////////////////////////////////////////////////////////////////////////////

void EvidenceHolder::addPieceOfEvidence( Evidence& pieceOfEvidence )
{
   if ( m_numLeadsToGiveUp <= 0 )
   {
      ASSERT_MSG( false, "This holder has all leads it can give up already taken" );
      return;
   }

   if ( m_creators.empty() )
   {
      ASSERT_MSG( !m_creators.empty(), "You failed to initialize the m_evidenceFactory" );
      return;
   }

   m_numLeadsToGiveUp =  m_numLeadsToGiveUp - pieceOfEvidence.getLeadsCount();

   const uint randCreatorIdx = rand() % m_creators.size();
   RenderedEvidence* renderedEvidence = m_creators[randCreatorIdx]->create();

   renderedEvidence->init( pieceOfEvidence, this );
   m_renderedEvidence.push_back( renderedEvidence );
}

///////////////////////////////////////////////////////////////////////////////

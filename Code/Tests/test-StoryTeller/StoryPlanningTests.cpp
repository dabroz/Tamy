#include "core-TestFramework\TestFramework.h"
#include "core\Log.h"

// core
#include "core\MemoryRouter.h"
#include "core\Graph.h"
#include "core\GraphUtils.h"
#include "core\UniqueIdGenerator.h"

#include "ext-StoryTeller\RenderedEvidence.h"
#include "ext-StoryTeller\EvidenceHolder.h"
#include "ext-StoryTeller\EvidenceWorld.h"
#include "ext-StoryTeller\Evidence.h"
#include "ext-StoryTeller\Investigator.h"


///////////////////////////////////////////////////////////////////////////////

namespace
{

   class Article : public RenderedEvidence
   {
      DECLARE_ALLOCATOR( Article, AM_DEFAULT );

   public:
      void onAsked( Investigator& investigator ) override
      {
         EvidenceWorld& world = investigator.getWorld();
         std::string& story = world.getContext<std::string>();

         char tmpStr[512];
         sprintf_s( tmpStr, "The article mentioned %s. I asked him about it.\n", m_owner->getName().c_str() );
         story += tmpStr;
      }

      void onAnswerDenied( Investigator& investigator ) override
      {
         EvidenceWorld& world = investigator.getWorld();
         std::string& story = world.getContext<std::string>();

         char tmpStr[512];
         sprintf_s( tmpStr, "- One small article doesn't mean a thing - said %s.\n", m_owner->getName().c_str() );
         story += tmpStr;
      }

      void onRevealed( Investigator& investigator, const EvidenceHolder& interrogated ) override
      {
         EvidenceWorld& world = investigator.getWorld();
         std::string& story = world.getContext<std::string>();

         char tmpStr[512];
         sprintf_s( tmpStr, "%s pointed me towards and article, that according to him, could be found at %s.\n", interrogated.getName().c_str(), m_owner->getName().c_str() );
         story += tmpStr;
      }
   };

   class Picture : public RenderedEvidence
   {
      DECLARE_ALLOCATOR( Picture, AM_DEFAULT );

   public:
      void onAsked( Investigator& investigator ) override
      {
         EvidenceWorld& world = investigator.getWorld();
         std::string& story = world.getContext<std::string>();

         char tmpStr[512];
         sprintf_s( tmpStr, "I checked the picture at the %s\n", m_owner->getName().c_str() );
         story += tmpStr;
      }

      void onAnswerDenied( Investigator& investigator ) override
      {
         EvidenceWorld& world = investigator.getWorld();
         std::string& story = world.getContext<std::string>();

         char tmpStr[512];
         sprintf_s( tmpStr, "There was something on the picture, but I couldn't quite make out what it was.\n ");
         story += tmpStr;
      }

      void onRevealed( Investigator& investigator, const EvidenceHolder& interrogated ) override
      {
         EvidenceWorld& world = investigator.getWorld();
         std::string& story = world.getContext<std::string>();

         char tmpStr[512];
         sprintf_s( tmpStr, "%s told me about a picture, that I can find at %s\n", interrogated.getName().c_str(), m_owner->getName().c_str() );
         story += tmpStr;
      }
   };

   class Memory : public RenderedEvidence
   {
      DECLARE_ALLOCATOR( Memory, AM_DEFAULT );

   public:
      void onAsked( Investigator& investigator ) override
      {
         EvidenceWorld& world = investigator.getWorld();
         std::string& story = world.getContext<std::string>();

         char tmpStr[512];
         sprintf_s( tmpStr, "I asked %s if he remembered anything\n", m_owner->getName().c_str() );
         story += tmpStr;
      }

      void onAnswerDenied( Investigator& investigator ) override
      {
         EvidenceWorld& world = investigator.getWorld();
         std::string& story = world.getContext<std::string>();

         char tmpStr[512];
         sprintf_s( tmpStr, "%s didn't give me any answers, just shook me off\n", m_owner->getName().c_str() );
         story += tmpStr;
      }

      void onRevealed( Investigator& investigator, const EvidenceHolder& interrogated ) override
      {
         EvidenceWorld& world = investigator.getWorld();
         std::string& story = world.getContext<std::string>();

         char tmpStr[512];
         sprintf_s( tmpStr, "%s told me that %s will definitely know this.\n", interrogated.getName().c_str(), m_owner->getName().c_str() );
         story += tmpStr;
      }
   };

   class FacialExpression : public RenderedEvidence
   {
      DECLARE_ALLOCATOR( FacialExpression, AM_DEFAULT );

   public:
      void onAsked( Investigator& investigator ) override
      {
         EvidenceWorld& world = investigator.getWorld();
         std::string& story = world.getContext<std::string>();

         char tmpStr[512];
         sprintf_s( tmpStr, "%s looked frightened.\n", m_owner->getName().c_str() );
         story += tmpStr;
      }

      void onAnswerDenied( Investigator& investigator ) override
      {
         EvidenceWorld& world = investigator.getWorld();
         std::string& story = world.getContext<std::string>();

         char tmpStr[512];
         sprintf_s( tmpStr, "- Back off! - said %s - I'm not afraid of you.\n", m_owner->getName().c_str() );
         story += tmpStr;
      }

      void onRevealed( Investigator& investigator, const EvidenceHolder& interrogated ) override
      {
         EvidenceWorld& world = investigator.getWorld();
         std::string& story = world.getContext<std::string>();

         char tmpStr[512];
         sprintf_s( tmpStr, "%s told me that he's seen %s being worked up about something.\n", interrogated.getName().c_str(), m_owner->getName().c_str() );
         story += tmpStr;
      }
   };

   // -------------------------------------------------------------------------

   class DataTerminal : public EvidenceHolder
   {
      DECLARE_ALLOCATOR( DataTerminal, AM_DEFAULT );
      DECLARE_EVIDENCE_HOLDER();

   public:
      DataTerminal()
      {
         addType< Article >();
         addType< Picture >();
         setName( "Data terminal" );
      }
   };
   DEFINE_EVIDENCE_HOLDER( DataTerminal, 3 );

   class Informant : public EvidenceHolder
   {
      DECLARE_ALLOCATOR( Informant, AM_DEFAULT );
      DECLARE_EVIDENCE_HOLDER();

   public:
      Informant()
      {
         addType< Memory >();
         addType< FacialExpression >();

         const char* names[3] = {
            "Dirty Mike",
            "Rhonda",
            "Jake Myers",
         };

         int randNameIdx = UniqueIdGenerator<Informant>::getNextId() % 3;

         setName( names[randNameIdx] );
      }
   };
   DEFINE_EVIDENCE_HOLDER( Informant, 2 );

   class Bystander : public EvidenceHolder
   {
      DECLARE_ALLOCATOR( Bystander, AM_DEFAULT );
      DECLARE_EVIDENCE_HOLDER();

   public:
      Bystander()
      {
         addType< Memory >();

         const char* names[3] = {
            "A woman",
            "A man",
            "A child",
         };

         int randNameIdx = UniqueIdGenerator<Bystander>::getNextId() % 3;
         setName( names[randNameIdx] );
      }
   };
   DEFINE_EVIDENCE_HOLDER( Bystander, 1 );

}

///////////////////////////////////////////////////////////////////////////////

TEST( StoryPlanning, evidenceGraph )
{
   Evidence goal;
   Evidence witness1;
   Evidence witness2;
   Evidence witness3;
   Evidence friendOfW1AndW2;
   Evidence friendOfW3;
   Evidence anotherFriendOfW3;

   Graph< Evidence* > evidenceGraph;
   {
      goal.addToGraph( evidenceGraph );
      witness1.addToGraph( evidenceGraph );
      witness2.addToGraph( evidenceGraph );
      witness3.addToGraph( evidenceGraph );
      friendOfW1AndW2.addToGraph( evidenceGraph );
      friendOfW3.addToGraph( evidenceGraph );
      anotherFriendOfW3.addToGraph( evidenceGraph );

      evidenceGraph.connect( goal.getIdx(), witness1.getIdx() );
      evidenceGraph.connect( goal.getIdx(), witness2.getIdx() );
      evidenceGraph.connect( goal.getIdx(), witness3.getIdx() );
      evidenceGraph.connect( witness1.getIdx(), friendOfW1AndW2.getIdx() );
      evidenceGraph.connect( witness2.getIdx(), friendOfW1AndW2.getIdx() );
      evidenceGraph.connect( witness2.getIdx(), witness1.getIdx() );
      evidenceGraph.connect( witness1.getIdx(), witness2.getIdx() );
      evidenceGraph.connect( witness3.getIdx(), friendOfW3.getIdx() );
      evidenceGraph.connect( witness3.getIdx(), anotherFriendOfW3.getIdx() );
   }

   EvidenceWorld world( goal, evidenceGraph );
   world.compile();

   Investigator investigator( world );

   CPPUNIT_ASSERT( !investigator.investigate( goal ) );
   CPPUNIT_ASSERT( !investigator.investigate( witness1 ) );
   CPPUNIT_ASSERT( !investigator.investigate( witness2 ) );
   CPPUNIT_ASSERT( !investigator.investigate( witness3 ) );
   CPPUNIT_ASSERT( !investigator.isGoalDiscovered() );

   CPPUNIT_ASSERT( investigator.investigate( friendOfW1AndW2 ) );
   CPPUNIT_ASSERT( !investigator.isGoalDiscovered() );
   CPPUNIT_ASSERT( investigator.investigate( witness1 ) );
   CPPUNIT_ASSERT( !investigator.isGoalDiscovered() );
   CPPUNIT_ASSERT( investigator.investigate( witness2 ) );
   CPPUNIT_ASSERT( !investigator.isGoalDiscovered() );

   CPPUNIT_ASSERT( !investigator.investigate( witness3 ) );
   CPPUNIT_ASSERT( investigator.investigate( friendOfW3 ) );
   CPPUNIT_ASSERT( !investigator.investigate( witness3 ) );
   CPPUNIT_ASSERT( investigator.investigate( anotherFriendOfW3 ) );

   CPPUNIT_ASSERT( !investigator.isGoalDiscovered() );
   CPPUNIT_ASSERT( investigator.investigate( witness3 ) );
   CPPUNIT_ASSERT( investigator.isGoalDiscovered() );
}

///////////////////////////////////////////////////////////////////////////////

TEST( StoryPlanning, evidenceGraphParser )
{
   // set the randomizer seed to the same value to make sure that the same 'random' numbers will always be drawn
   srand( 0 );

   Evidence goal;
   Evidence witness1;
   Evidence witness2;
   Evidence witness3;
   Evidence friendOfW1AndW2;
   Evidence friendOfW3;
   Evidence anotherFriendOfW3;

   Graph< Evidence* > evidenceGraph;
   {
      goal.addToGraph( evidenceGraph );
      witness1.addToGraph( evidenceGraph );
      witness2.addToGraph( evidenceGraph );
      witness3.addToGraph( evidenceGraph );
      friendOfW1AndW2.addToGraph( evidenceGraph );
      friendOfW3.addToGraph( evidenceGraph );
      anotherFriendOfW3.addToGraph( evidenceGraph );

      evidenceGraph.connect( goal.getIdx(), witness1.getIdx() );
      evidenceGraph.connect( goal.getIdx(), witness2.getIdx() );
      evidenceGraph.connect( goal.getIdx(), witness3.getIdx() );
      evidenceGraph.connect( witness1.getIdx(), friendOfW1AndW2.getIdx() );
      evidenceGraph.connect( witness2.getIdx(), friendOfW1AndW2.getIdx() );
      evidenceGraph.connect( witness2.getIdx(), witness1.getIdx() );
      evidenceGraph.connect( witness1.getIdx(), witness2.getIdx() );
      evidenceGraph.connect( witness3.getIdx(), friendOfW3.getIdx() );
      evidenceGraph.connect( witness3.getIdx(), anotherFriendOfW3.getIdx() );
   }

   EvidenceWorld world( goal, evidenceGraph );
   {
      world.addType< DataTerminal >();
      world.addType< Informant >();
      world.addType< Bystander >();
   }
   world.compile();
   world.createEvidenceHoldersAndDistributeEvidence();

   // calculate the number of evidence pieces distributed among the EvidenceHolders
   uint numDistributedPieces = 0;
   const uint evidenceHoldersCount = world.getEvidenceHoldersCount();
   CPPUNIT_ASSERT( evidenceHoldersCount > 0 );

   for ( uint i = 0; i < evidenceHoldersCount; ++i )
   {
      EvidenceHolder& evidenceHolder = world.getEvidenceHolder( i );
      numDistributedPieces += evidenceHolder.getNumEvidencePieces();
   }
   CPPUNIT_ASSERT_EQUAL( ( uint )7, numDistributedPieces );

   // associate a context with the world
   std::string story;
   world.setContext( &story );

   // we should be able to talk to each EvidenceHolder using an Investigator instance - then, they should write a text or something
   Investigator investigator( world );
   CPPUNIT_ASSERT( story.length() == 0 );

   Evidence* interrogationSequence[7] = {
      &friendOfW1AndW2,
      &friendOfW3,
      &anotherFriendOfW3,
      &witness3,
      &witness2,
      &witness1,
      &goal
   };

   for ( uint i = 0; i < 7; ++i )
   {
      story += " --------------------------------------------------\n";
      RenderedEvidence* evidence = world.findRenderedEvidence( *interrogationSequence[i] );
      CPPUNIT_ASSERT( evidence != NULL );
      CPPUNIT_ASSERT( evidence->investigate( investigator ) );
   }
   LOG( story.c_str() );
}

///////////////////////////////////////////////////////////////////////////////


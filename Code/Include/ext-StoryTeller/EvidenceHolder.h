/// @file   ext-StoryTeller\EvidenceHolder.h
/// @brief  ye who holds the evidence
#pragma once

#include "core\MemoryRouter.h"
#include "core\Array.h"
#include "ext-StoryTeller\RandomFactory.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

class RenderedEvidence;
class Evidence;

///////////////////////////////////////////////////////////////////////////////

class EvidenceHolder
{
   DECLARE_ALLOCATOR( EvidenceHolder, AM_DEFAULT );

private:
   class Creator
   {
      DECLARE_ALLOCATOR( Creator, AM_DEFAULT );

   public:
      virtual ~Creator() {}

      virtual RenderedEvidence* create() const = 0;
   };

   template< class T >
   class TCreator : public Creator
   {
      DECLARE_ALLOCATOR( TCreator, AM_DEFAULT );

   public:
      RenderedEvidence* create() const override
      {
         return new T();
      }
   };

private:
   std::string                               m_name;
   int                                       m_numLeadsToGiveUp;
   Array< RenderedEvidence* >                m_renderedEvidence;

   Array< Creator* >                         m_creators;

public:
   /**
    * Constructor.
    */
   EvidenceHolder();
   virtual ~EvidenceHolder();

   /**
    * Gives the holder another piece of evidence to keep.
    *
    * @param pieceOfEvidence
    */
   void addPieceOfEvidence( Evidence& pieceOfEvidence );

   /**
    * Sets a number of leads to give up.
    */
   void setNumLeadsToGiveUp( uint count )
   {
      m_numLeadsToGiveUp = count;
   }

   /**
    * Tells if the holder has any spare evidence slots left.
    */
   inline uint getNumLeadsToGiveUp() const
   {
      return m_numLeadsToGiveUp;
   }

   /**
    * Returns the number of evidence pieces already stored.
    */
   inline uint getNumEvidencePieces() const
   {
      return m_renderedEvidence.size();
   }

   /**
    * Returns the specified rendered piece of evidence.
    *
    * @param idx
    */
   inline RenderedEvidence& getPieceOfEvidence( uint idx ) const
   {
      return *m_renderedEvidence[idx];
   }

   /**
    * Sets the name of the holder.
    *
    * @param name
    */
   inline void setName( const char* name )
   {
      m_name = name;
   }

   /**
    * Returns the name of the holder.
    */
   inline const std::string& getName() const
   {
      return m_name;
   }

protected:
   /**
    * Registers a new type of evidence holder with the factory.
    */
   template< class T >
   void addType()
   {
      m_creators.push_back( new TCreator< T >() );
   }

};

///////////////////////////////////////////////////////////////////////////////

#define DECLARE_EVIDENCE_HOLDER() public: static uint __maxRevealedLeads; private:

#define DEFINE_EVIDENCE_HOLDER( Type, maxRevealedLeads ) uint Type::__maxRevealedLeads = maxRevealedLeads;

///////////////////////////////////////////////////////////////////////////////

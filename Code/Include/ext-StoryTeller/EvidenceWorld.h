/// @file   ext-StoryTeller\EvidenceWorld.h
/// @brief  domain in which the evidence exists
#pragma once

#include "core\MemoryRouter.h"
#include "core\Graph.h"
#include "core\Array.h"
#include "core\List.h"
#include "ext-StoryTeller\RandomFactory.h"


///////////////////////////////////////////////////////////////////////////////

class Evidence;
class EvidenceHolder;
class RenderedEvidence;

///////////////////////////////////////////////////////////////////////////////

class EvidenceWorld
{
   DECLARE_ALLOCATOR( EvidenceWorld, AM_DEFAULT );

private:
   class Creator
   {
      DECLARE_ALLOCATOR( Creator, AM_DEFAULT );

   public:
      uint        m_maxRevealedLeads;

   public:
      virtual ~Creator() {}

      virtual EvidenceHolder* create() const = 0;
   };

   template< class T >
   class TCreator : public Creator
   {
      DECLARE_ALLOCATOR( TCreator, AM_DEFAULT );

   public:
      TCreator()
      {
         m_maxRevealedLeads = T::__maxRevealedLeads;
      }

      EvidenceHolder* create() const override
      {
         EvidenceHolder* newHolder = new T();
         newHolder->setNumLeadsToGiveUp( T::__maxRevealedLeads );

         return newHolder;
      }
   };

private:
   Evidence&                           m_goal;
   Graph< Evidence* >&                 m_evidenceGraph;
   void*                               m_context;

   List< Creator* >                    m_creators;
   Array< EvidenceHolder* >            m_evidenceHolders;

public:
   /**
    * Constructor.
    *
    * @param goal
    * @param evidenceGraph
    */
   EvidenceWorld( Evidence& goal, Graph< Evidence* >& evidenceGraph );
   ~EvidenceWorld();

   /**
    * Gives access to the evidence graph ( non-const ).
    */
   inline Graph< Evidence* >& graph() {
      return m_evidenceGraph;
   }

   /**
    * Returns the evidence graph.
    */
   inline const Graph< Evidence* >& graph() const {
      return m_evidenceGraph;
   }

   /**
    * Associates a runtime context with the world.
    */
   inline void setContext( void* context )
   {
      m_context = context;
   }

   /**
    * Returns a runtime context associated with this world.
    */
   template< typename T >
   inline T& getContext() const {
      return *( static_cast< T* >( m_context ) );
   }

   /**
    * Registers a new type of evidence holder with the factory.
    */
   template< class T >
   void addType()
   {
      m_creators.pushBack( new TCreator< T >() );
   }

   /**
    * Returns the number of active evidence holders.
    */
   inline uint getEvidenceHoldersCount() const {
      return m_evidenceHolders.size();
   }

   /**
    * Returns the specified evidence holder.
    *
    * @param idx
    */
   inline EvidenceHolder& getEvidenceHolder( uint idx ) const {
      return *m_evidenceHolders[idx];
   }

   /**
    * Parses the evidence graph, creating EvidenceHolders and tying the evidence to them.
    */
   void createEvidenceHoldersAndDistributeEvidence();

   /**
    * Compiles the graph in preparation to the investigation.
    */
   void compile();

   /**
    * Searches for a rendered piece of evidence that corresponds to the specified evidence node.
    *
    * @param evidenceNode
    */
   RenderedEvidence* findRenderedEvidence( Evidence& evidenceNode ) const;

   /**
    * Searches for an EvidenceHolder who owns this evidence instance.
    *
    * @param evidenceNode
    */
   EvidenceHolder* findEvidenceHolder( Evidence& evidenceNode ) const;

   /**
    * Returns the assigned goal node.
    */
   inline Evidence& getGoal() const
   {
      return m_goal;
   }

   /**
    * Returns the evidence graph instance.
    */
   inline const Graph< Evidence* >& getEvidenceGraph() const
   {
      return m_evidenceGraph;
   }

   /**
   * Collects the rendered pieces of evidence the specified evidence holds the leads on.
   *
   * @param evidenceNode
   * @param outLeads
   */
   void collectLeads( const Evidence& evidenceNode, Array< RenderedEvidence* >& outLeads ) const;

private:
   static int edgeCostFunc( const Graph< Evidence* >& graph, int startVtx, int endVtx );
   bool canHolderStoreEvidence( EvidenceHolder* evidenceHolder, Evidence* pieceOfEvidence ) const;

private:
   /**
    * A helper structure used by the compiler to assign clearance values to particular graph nodes.
    */
   struct ClearanceAssigner
   {
      const Graph< Evidence* >&     m_evidenceGraph;

      /**
       * Constructor.
       *
       * @param evidenceGraph
       */
      ClearanceAssigner( const Graph< Evidence* >& evidenceGraph );

      /**
       * Operator()
       *
       * @param pieceOfEvidence
       */
      void operator()( Evidence* pieceOfEvidence );
   };
};

///////////////////////////////////////////////////////////////////////////////

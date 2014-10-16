/// @file   core/Subject.h
/// @brief Subject-Observer pattern
#pragma once

#include <vector>
#include <algorithm>
#include "core\Observer.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * This is the observation subject that allows to attach a
 * a single type of observer. @see Observer class for more details
 * about realized observation model
 */
template<typename SubjectType, typename MessageEnum>
class Subject
{
private:
   std::vector< Observer< SubjectType, MessageEnum>* >   m_observers;

   std::vector< Observer< SubjectType, MessageEnum>* >   m_observersToAttach;
   std::vector< int >                                    m_observersToDetach;

public:
   virtual ~Subject() {}

   /**
    * Attaches a new observer.
    *
    * @param observer
    * @param sendInitialUpdate   should the initial update be sent?
    */
   void attachObserver( Observer< SubjectType, MessageEnum >& observer, bool sendInitialUpdate = true )
   {
      // check for duplicates
      uint count = m_observers.size();
      for ( uint i = 0; i < count; ++i )
      {
         if ( m_observers[i] == &observer )
         {
            // that's a duplicate of a registered observer
            return;
         }
      }

      count = m_observersToAttach.size();
      for ( uint i = 0; i < count; ++i )
      {
         if ( m_observersToAttach[i] == &observer )
         {
            // that's a duplicate of an observer scheduled for attachment
            return;
         }
      }

      m_observersToAttach.push_back( &observer );

      if ( sendInitialUpdate )
      {
         SubjectType& castSubject = static_cast< SubjectType& >( *this );
         observer.update( castSubject );
      }
   }

   /**
    * Detaches the observer.
    *
    * @param observer 
    */
   void detachObserver( Observer< SubjectType, MessageEnum >& observer )
   {
      // check if the node wasn't scheduled for attachment, and remove it if it was
      int count = m_observersToAttach.size();
      for ( int i = count - 1; i >= 0; --i )
      {
         if ( m_observersToAttach[i] == &observer )
         {
            m_observersToAttach.erase( m_observersToAttach.begin() + i );
            return;
         }
      }

      // locate the detached observer
      int observerIdx = -1;
      count = m_observers.size();
      for ( int i = 0; i < count; ++i )
      {
         if ( m_observers[i] == &observer )
         {
            observerIdx = i;
            break;
         }
      }

      if ( observerIdx < 0 )
      {
         return;
      }

      count = m_observersToDetach.size();
      for ( int i = 0; i < count; ++i )
      {
         if ( m_observersToDetach[i] == observerIdx )
         {
            // that's a duplicate of an observer scheduled for detachment
            return;
         }
      }

      m_observersToDetach.push_back( observerIdx );
   }

protected:
   void notify( const MessageEnum& msg )
   {
      // perform lazy detachment and attachment of observers
      {
         uint count = m_observersToDetach.size();
         if ( count > 0 )
         {
            std::sort( m_observersToDetach.begin(), m_observersToDetach.end() );
         }
         for ( int i = count - 1; i >= 0; --i )
         {
            m_observers.erase( m_observers.begin() + m_observersToDetach[i] );
         }
         m_observersToDetach.clear();

         count = m_observersToAttach.size();
         for ( uint i = 0 ; i < count; ++i )
         {
            m_observers.push_back( m_observersToAttach[i] );
         }
         m_observersToAttach.clear();
      }


      // broadcast updates
      unsigned int count = m_observers.size();
      for (unsigned int i = 0; i < count; ++i)
      {
         m_observers[i]->update( static_cast< SubjectType& >( *this ), msg );
      }
   }

};

///////////////////////////////////////////////////////////////////////////////


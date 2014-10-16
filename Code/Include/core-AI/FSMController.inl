#ifndef _FSM_CONTROLLER_H
#error "This file can only be included from FSMController.h"
#else

#include <string>
#include "core/Assert.h"


///////////////////////////////////////////////////////////////////////////////

template< typename Impl >
FSMController< Impl >::FSMController() 
   : m_currentState( NULL )
   , m_nextState( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl >
FSMController< Impl >::~FSMController()
{
   m_currentState = NULL;
   m_nextState = NULL;

   unsigned int count = m_states.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      delete m_states[i];
   }
   m_states.clear();
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl >
void FSMController< Impl >::update( float timeElapsed )
{
   // check if there's a new state waiting to be activated
   if ( m_nextState )
   {
      // deactivate the old state and activate the new one
      if ( m_currentState )
      {
         m_currentState->deactivate();
      }
      m_currentState = m_nextState;
      m_currentState->activate();

      m_nextState = NULL;
   }

   // update the current state
   if ( m_currentState ) 
   {
      m_currentState->execute( timeElapsed );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl > template< typename StateInterface >
StateInterface& FSMController< Impl >::getCurrentState() const
{
   return dynamic_cast< StateInterface& >( *m_currentState );
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl > template< typename State >
State* FSMController< Impl >::registerState()
{
   // check if the state isn't already registered
   unsigned int count = m_states.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      if ( m_states[i]->isExactlyA< typename State >() )
      {
         // yup - we already have it registered
         return NULL;
      }
   }

   State* state = new State();
   m_states.push_back( static_cast< FSMState< Impl >* >( state ) );
   m_states.back()->setHostController( static_cast< Impl& >( *this ) );

   return state;
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl > template< typename State >
State& FSMController< Impl >::getState()
{
   // find the state we want to begin working with
   unsigned int count = m_states.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      FSMState< Impl >* checkedState = m_states[i];
      if ( checkedState->isExactlyA< typename State >() )
      {
         State* castState = DynamicCast< State >( checkedState );
         return *castState;
      }
   }

   ASSERT_MSG( false, "FSM state not found" );
   return *static_cast< State* >( NULL );
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl >template< typename State >
State& FSMController< Impl >::transitionTo()
{
   // find the state we want to transition to
   unsigned int count = m_states.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      FSMState< Impl >* checkedState = m_states[i];
      if ( checkedState->isExactlyA< typename State >() )
      {
         m_nextState = checkedState;
         State* castState = DynamicCast< State >( m_nextState );
         return *castState;
      }
   }

   ASSERT_MSG( false, "FSM state not found" );
   return *static_cast< State* >( NULL );
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl > template< typename State >
State& FSMController< Impl >::begin()
{
   return transitionTo< State >();
}

///////////////////////////////////////////////////////////////////////////////

#endif // _FSM_CONTROLLER_H

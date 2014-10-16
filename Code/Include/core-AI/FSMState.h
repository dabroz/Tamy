/// @file   core-AI/FSMState.h
/// @brief  basic FSM state
#ifndef _FSM_STATE_H
#define _FSM_STATE_H

#include "core\MemoryRouter.h"
#include "core\ReflectionObject.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * This FSM state contains logic behind a single state of mind of an agent.
 * It's designed to work with a specific type of controller only - thus
 * the controller(1)-(*)states group create a family of classes working
 * together.
 */
template< typename Controller >
class FSMState : public ReflectionObject
{
   DECLARE_ALLOCATOR( FSMState, AM_DEFAULT );

protected:
   Controller*             m_fsm;

public:
   virtual ~FSMState() {}

   /**
    * This method will be called once the state is activated.
    */
   virtual void activate() {}

   /**
    * This method will be called once the state is deactivated.
    */
   virtual void deactivate() {}

   /**
    * This method will be called on an active state each time
    * the controller gets updated.
    *
    * @param timeElapsed   time (in seconds) that has elapsed
    *                      since the last time this method was called
    */
   virtual void execute( float timeElapsed ) {}


   /**
    * Sets the host controller instance.
    */
   inline void setHostController( Controller& controller )
   {
      m_fsm = &controller;
   }

protected:
   /**
    * Constructor.
    */
   FSMState()
      : m_fsm( NULL )
   {
   }

   /**
    * Returns the host controller instance.
    */
   inline Controller& fsm() const { return *m_fsm; }

   /**
    * Helper method that allows to transition between the controller's states.
    */
   template< typename State >
   State& transitionTo();
};

///////////////////////////////////////////////////////////////////////////////

#include "core-AI/FSMState.inl"

///////////////////////////////////////////////////////////////////////////////

#endif _FSM_STATE_H

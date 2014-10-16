#ifndef _FSM_STATE_H
#error "This file can be included only from FSMState.h"
#else

///////////////////////////////////////////////////////////////////////////////

template< typename Controller > template< typename State >
State& FSMState< Controller >::transitionTo()
{
   return m_fsm->transitionTo< State >();
}

///////////////////////////////////////////////////////////////////////////////

#endif _FSM_STATE_H

#ifndef _STEPPED_LOCKLESS_COMMAND_FENCE_H
#error "This file can only be included in SteppedLocklessCommandFence.h"
#else

#include "core\CriticalSection.h"


///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
SteppedLocklessCommandFence< Owner >::SteppedLocklessCommandFence( CriticalSection* fenceLock, SteppedLocklessCommandFence< Owner >* prevFence, bool& fenceReachedFlag )
   : m_proceed( false )
   , m_fenceReachedFlag( fenceReachedFlag )
{
   if ( prevFence )
   {
      prevFence->proceed();
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
SteppedLocklessCommandFence< Owner >::~SteppedLocklessCommandFence()
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
void SteppedLocklessCommandFence< Owner >::execute( Owner& owner )
{
   m_fenceReachedFlag = !m_proceed;
}

///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
void SteppedLocklessCommandFence< Owner >::proceed()
{
   m_proceed = true;
}

///////////////////////////////////////////////////////////////////////////////

#endif // _STEPPED_LOCKLESS_COMMAND_FENCE_H

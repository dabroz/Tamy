#ifndef _LOCKLESS_COMMAND_FENCE_H
#error "This file can only be included in LocklessCommandFence.h"
#else

#include "core\CriticalSection.h"


///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
LocklessCommandFence< Owner >::LocklessCommandFence( CriticalSection* fenceLock, LocklessCommandFence< Owner >* prevFence )
   : m_fenceLock( fenceLock )
   , m_proceed( false )
{
   if ( prevFence )
   {
      prevFence->proceed();
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
LocklessCommandFence< Owner >::~LocklessCommandFence()
{
   m_fenceLock = NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
void LocklessCommandFence< Owner >::execute( Owner& owner )
{
   // wait for the next fence to release this lock
   bool flag = false;
   while( !flag )
   {
      m_fenceLock->enter();
      flag = m_proceed;
      m_fenceLock->leave();
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename Owner >
void LocklessCommandFence< Owner >::proceed()
{
   m_fenceLock->enter();
   m_proceed = true;
   m_fenceLock->leave();
}

///////////////////////////////////////////////////////////////////////////////

#endif // _LOCKLESS_COMMAND_FENCE_H

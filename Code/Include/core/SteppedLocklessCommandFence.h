/// @file   core/SteppedLocklessCommandFence.h
/// @brief  a fence command
#ifndef _STEPPED_LOCKLESS_COMMAND_FENCE_H
#define _STEPPED_LOCKLESS_COMMAND_FENCE_H

#include "core\LocklessCommand.h"
#include "core\MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * A lock-less command interface.
 */
template< typename Owner >
class SteppedLocklessCommandFence : public LocklessCommand< Owner >
{
   DECLARE_ALLOCATOR( SteppedLocklessCommandFence, AM_DEFAULT );

private:
   bool                    m_proceed;
   bool&                   m_fenceReachedFlag;

public:
   /**
    * Constructor.
    */
   SteppedLocklessCommandFence( CriticalSection* fenceLock, SteppedLocklessCommandFence< Owner >* prevFence, bool& fenceReachedFlag );
   ~SteppedLocklessCommandFence();

   /**
    * Unlocks this fence, making the queue proceed to the next one.
    */
   void proceed();

   // -------------------------------------------------------------------------
   // LocklessCommand implementation
   // -------------------------------------------------------------------------
   void execute( Owner& owner );
};

///////////////////////////////////////////////////////////////////////////////

#include "core\SteppedLocklessCommandFence.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _STEPPED_LOCKLESS_COMMAND_FENCE_H

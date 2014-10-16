/// @file   core/LocklessCommandFence.h
/// @brief  a fence command
#ifndef _LOCKLESS_COMMAND_FENCE_H
#define _LOCKLESS_COMMAND_FENCE_H

#include "core\LocklessCommand.h"
#include "core\MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

class CriticalSection;

///////////////////////////////////////////////////////////////////////////////

/**
 * A lock-less command interface.
 */
template< typename Owner >
class LocklessCommandFence : public LocklessCommand< Owner >
{
   DECLARE_ALLOCATOR( LocklessCommandFence, AM_DEFAULT );

private:
   CriticalSection*        m_fenceLock;
   bool                    m_proceed;

public:
   /**
    * Constructor.
    */
   LocklessCommandFence( CriticalSection* fenceLock, LocklessCommandFence< Owner >* prevFence );
   ~LocklessCommandFence();

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

#include "core\LocklessCommandFence.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _LOCKLESS_COMMAND_FENCE_H

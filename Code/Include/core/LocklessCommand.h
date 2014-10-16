/// @file   core\LocklessCommand.h
/// @brief  a lock-less command interface
#ifndef _LOCKLESS_COMMAND_H
#define _LOCKLESS_COMMAND_H


///////////////////////////////////////////////////////////////////////////////

/**
 * A lock-less command interface.
 */
template< typename Owner >
class LocklessCommand
{
public:
   virtual ~LocklessCommand() {}

   /**
    * Command execution code.
    *
    * @param owner
    */
   virtual void execute( Owner& owner ) = 0;
};

///////////////////////////////////////////////////////////////////////////////

#endif // _LOCKLESS_COMMAND_H

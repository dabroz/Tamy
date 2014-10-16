#ifndef _LOCAL_STACK_H
#error "This file can only be included in LocalStack.h"
#else

#include "core/ThreadSystem.h"
#include "core/Thread.h"
#include "core/TSFragmentedMemoryPool.h"


///////////////////////////////////////////////////////////////////////////////

template< typename T >
LocalStack< T >::LocalStack()
   : Stack( TSingleton< ThreadSystem >::getInstance().getCurrentThread().m_sharedMemoryPool )
{
}

///////////////////////////////////////////////////////////////////////////////

#endif // _LOCAL_STACK_H

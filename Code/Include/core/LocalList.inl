#ifndef _LOCAL_LIST_H
#error "This file can only be included in LocalList.h"
#else

#include "core/ThreadSystem.h"
#include "core/Thread.h"
#include "core/TSFragmentedMemoryPool.h"


///////////////////////////////////////////////////////////////////////////////

template< typename T >
LocalList< T >::LocalList()
   : List( TSingleton< ThreadSystem >::getInstance().getCurrentThread().m_sharedMemoryPool )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
LocalList< T >::LocalList( const LocalList& rhs )
   : List( rhs )
{
}

///////////////////////////////////////////////////////////////////////////////

#endif // _LOCAL_LIST_H

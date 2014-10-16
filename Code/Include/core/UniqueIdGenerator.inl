#ifndef _UNIQUE_ID_GENERATOR_H
#error "This file can only be included from UniqueIdGenerator.h"
#else

///////////////////////////////////////////////////////////////////////////////

template< typename T >
uint UniqueIdGenerator< T >::s_nextId = 0;

template< typename T >
typename UniqueIdGenerator< T >::Node* UniqueIdGenerator< T >::s_freeIds = NULL;

///////////////////////////////////////////////////////////////////////////////

template< typename T >
UniqueIdGenerator< T >::~UniqueIdGenerator()
{
   while( s_freeIds )
   {
      Node* nextNode = s_freeIds->m_nextNode;
      delete s_freeIds;
      s_freeIds = nextNode;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
uint UniqueIdGenerator< T >::getNextId()
{
   uint id = 0;
   if ( s_freeIds )
   {
      id = s_freeIds->m_nextFreeId;
      
      Node* nextNode = s_freeIds->m_nextNode;
      delete s_freeIds;
      s_freeIds = nextNode;
   }
   else
   {
      id = s_nextId++;
   }
   return id;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void UniqueIdGenerator< T >::releaseId( uint id )
{
   s_freeIds = new Node( s_freeIds, id );
}

///////////////////////////////////////////////////////////////////////////////

#endif // _UNIQUE_ID_GENERATOR_H

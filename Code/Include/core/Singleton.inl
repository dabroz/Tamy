#ifndef _SINGLETON_H
#error "This file can only be included from Singleton.h"
#else

#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

template< typename T >
T& TSingleton< T >::initialize()
{
   SingletonConstruct construct;
   if ( !s_theInstance )
   {
      s_theInstance = new T( construct );

      SingletonsManager& sm = SingletonsManager::getInstance();
      sm.registerInstance< T >( s_theInstance );
   }

   return *s_theInstance;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TSingleton< T >::deinitialize( bool calledByUser )
{
   if ( calledByUser )
   {
      SingletonsManager& sm = SingletonsManager::getInstance();
      sm.unregisterInstance< T >( s_theInstance );
   }

   delete s_theInstance;
   s_theInstance = 0;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
T& TSingleton< T >::getInstance() 
{ 
   if ( !s_theInstance )
   {
      initialize();
   }

   return *s_theInstance; 
} 

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
void SingletonsManager::registerInstance( T* newInstance )
{
   const uint insertionOrder = T::getSingletonInsertionOrder();

   // check for uniqueness
   Entry** insertionPoint = &m_rootEntry;
   for ( Entry* entry = m_rootEntry; entry != 0; entry = entry->m_next )
   {
      if ( entry->compare( newInstance ) )
      {
         ASSERT_MSG( false, "Trying to register the same singleton instance twice" );
         return;
      }

      if ( insertionOrder > entry->m_order )
      {
         // this is where we need to insert the new entry
         break;
      }

      insertionPoint = &entry->m_next;
   }

   Entry* newEntry = new TEntry< T >( newInstance );
   newEntry->m_order = insertionOrder;
   newEntry->m_next = *insertionPoint;
   *insertionPoint = newEntry;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void SingletonsManager::unregisterInstance( T* removedInstance )
{
   // find the instance
   Entry** prevEntry = &m_rootEntry;
   for ( Entry* entry = m_rootEntry; entry != 0; entry = entry->m_next )
   {
      if ( entry->compare( removedInstance ) )
      {
         // found it
         *prevEntry = entry->m_next;
         
         // don't call deinitialize - this method IS called from T::deinitialize
         delete entry;

         return;
      }

      prevEntry = &entry->m_next;
   }

   // we were trying to remove an unregistered singleton instance - best notify the coder about it
   ASSERT_MSG( false, "Trying to remove a previously unregistered singleton instance" );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
SingletonsManager::TEntry< T >::TEntry( T* instance )
   : m_instance( instance )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
SingletonsManager::TEntry< T >::~TEntry()
{
   m_instance = 0;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool SingletonsManager::TEntry< T >::compare( void* rhs ) const
{
   return m_instance == rhs;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void SingletonsManager::TEntry< T >::deinitialize()
{
   TSingleton< T >::deinitialize( false );
}

///////////////////////////////////////////////////////////////////////////////

#endif // _SINGLETON_H

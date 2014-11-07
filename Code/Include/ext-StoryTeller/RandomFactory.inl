#ifndef _RANDOM_FACTORY_H
#error "This file can only be included in RandomFactory.h"
#else

#include "ext-StoryTeller\Evidence.h"


///////////////////////////////////////////////////////////////////////////////

template< typename CreatedBaseType >
RandomFactory< CreatedBaseType >::~RandomFactory()
{
   const uint count = m_creators.size();
   for ( uint i = 0; i < count; ++i )
   {
      Creator* creator = m_creators[i];
      delete creator;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename CreatedBaseType > template< class T >
void RandomFactory< CreatedBaseType >::addType()
{
   m_creators.push_back( new TCreator< T >() );
}

///////////////////////////////////////////////////////////////////////////////


#endif _RANDOM_FACTORY_H

#ifndef _REFLECTION_LOADER_H
#error "This file can only be included in ReflectionLoader.h"
#else

#include "core/ReflectionType.h"
#include "core/InStream.h"


///////////////////////////////////////////////////////////////////////////////

template< typename T >
T* ReflectionLoader::getNextObject()
{
   // get the first object from the list, or return NULL if it's empty
   if ( m_loadedObjects.empty() )
   {
      return NULL;
   }
   else
   {
      // notify all loaded objects that it's been successfully loaded
      ReflectionObject* obj = m_loadedObjects.front();
      m_loadedObjects.pop_front();

      return static_cast< T* >( obj );
   }
}

///////////////////////////////////////////////////////////////////////////////


#endif // _REFLECTION_LOADER_H

#include "ObjectNameGetter.h"
#include "core\Assert.h"
#include "core\ReflectionType.h"
#include "core\ReflectionObject.h"

// supported types
#include "core-AI\BlendTreeEvent.h"
#include "core-AI\BlendTreeVariable.h"


///////////////////////////////////////////////////////////////////////////////

QString ObjectNameGetter::toString( ReflectionObject* object)
{
   ASSERT( object );

   if ( object->isA< BlendTreeEvent >() )
   {
      BlendTreeEvent* btEvent = static_cast< BlendTreeEvent* >( object );
      QString outName = btEvent->getName().c_str();
      return outName;
   }
   else if ( object->isA< BlendTreeVariable >() )
   {
      BlendTreeVariable* btVariable = static_cast< BlendTreeVariable* >( object );
      QString outName = btVariable->getName().c_str();
      return outName;
   }
   else
   {
      // default case
      const SerializableReflectionType& reflectionType = object->getVirtualRTTI();
      QString outName;
      outName.sprintf( "%s( %p )", reflectionType.m_name.c_str(), (void*)object );

      return outName;
   }
}

///////////////////////////////////////////////////////////////////////////////

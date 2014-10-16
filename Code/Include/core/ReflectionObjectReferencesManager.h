/// @file   core\ReflectionObjectReferencesManager.h
/// @brief  manages references of one ReflectionObject to other ReflectionObjects
#pragma once

#include "core\MemoryRouter.h"
#include "core\ReflectionObjectChangeListener.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class ReflectionTypeComponent;

///////////////////////////////////////////////////////////////////////////////

/**
 * This is an internal class that handles references of one ReflectionObject
 * to other ReflectionObjects ( defined with PROPERTY_EDIT_REFLECTION macro ).
 *
 * The class instance stores a list of member fields that are references,
 * and tracks the objects those fields reference, attaching self as their listener
 * for that purpose.
 */
class ReflectionObjectReferencesManager : public ReflectionObjectChangeListener
{
   DECLARE_ALLOCATOR( ReflectionObjectReferencesManager, AM_DEFAULT );

private:
   ReflectionObject*                                  m_host;
   List< const ReflectionTypeComponent* >             m_memberFields;

public:
   /**
    * Constructor.
    *
    * @param hostObject
    * @param referenceMemberFields
    */
   ReflectionObjectReferencesManager( ReflectionObject* hostObject, const List< const ReflectionTypeComponent* >& referenceMemberFields );
   ~ReflectionObjectReferencesManager();

   /**
    * Called when a property value's about to change.
    *
    * @param property
    */
   void onPrePropertyChanged( ReflectionProperty& property );

   /**
    * Called after a property value changes.
    *
    * @param property
    */
   void onPropertyChanged( ReflectionProperty& property );

   // ----------------------------------------------------------------------
   // ReflectionObjectChangeListener implementation
   // ----------------------------------------------------------------------
   void onObservedObjectDeleted( ReflectionObject* deletedObject );
};

///////////////////////////////////////////////////////////////////////////////

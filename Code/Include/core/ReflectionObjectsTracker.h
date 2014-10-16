/// @file   core/ReflectionObjectsTracker.h
/// @brief  an interface that allows to automatically gather info about loaded objects during deserialization
#pragma once


///////////////////////////////////////////////////////////////////////////////

/**
 * An interface that allows to automatically gather info about loaded objects during deserialization.
 */
class ReflectionObjectsTracker
{
public:
   virtual ~ReflectionObjectsTracker() {}

   /**
    * Registers a new ReflectionObject instance that should be tracked.
    *
    * @param object
    */
   virtual void trackInstance( ReflectionObject* object ) = 0;

   /**
    * The method that allows to retrieve an instance of tracked object using its unique ID.
    *
    * @param id
    */
   virtual ReflectionObject* findInstance( const ReflectionObject::UniqueId& id ) = 0;
};

///////////////////////////////////////////////////////////////////////////////

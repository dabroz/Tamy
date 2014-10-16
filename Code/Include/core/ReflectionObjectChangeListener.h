/// @file   core/ReflectionObjectChangeListener.h
/// @brief  listener interface that will get notified about the changes the observed object undergoes
#pragma once


///////////////////////////////////////////////////////////////////////////////

class ReflectionProperty;
class ReflectionObject;

///////////////////////////////////////////////////////////////////////////////

/**
 * Listener interface that will get notified about the changes the observed object undergoes.
 */
class ReflectionObjectChangeListener
{
public:
   virtual ~ReflectionObjectChangeListener() {}

   /**
    * This method will be called if the edited property is about to be changed externally and the editor
    * should update the value.
    *
    * @param property
    */
   virtual void onObservedPrePropertyChanged( ReflectionProperty& property ) {}

   /**
    * This method will be called if the edited property gets changed externally and the editor
    * should update the value.
    *
    * @param property
    */
   virtual void onObservedPropertyChanged( ReflectionProperty& property ) {}

   /**
    * Called by the object the listener is attached to to inform that it's being deleted and should 
    * no longer be listened to.
    *
    * @param deletedObject
    */
   virtual void onObservedObjectDeleted( ReflectionObject* deletedObject ) {}
};

///////////////////////////////////////////////////////////////////////////////

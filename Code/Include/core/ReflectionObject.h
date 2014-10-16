/// @file   core/ReflectionObject.h
/// @brief  object with RTTI and serialization support
#ifndef _REFLECTION_OBJECT_H
#define _REFLECTION_OBJECT_H

#include "core\MemoryRouter.h"
#include "core\types.h"
#include <string>
#include "core\ReflectionMacros.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class SerializableReflectionType;
class ReflectionPropertiesView;
class ReflectionProperty;
class ReflectionObjectChangeListener;
class CriticalSection;
class ReflectionObjectReferencesManager;

///////////////////////////////////////////////////////////////////////////////

/**
 * Derive your class from this class if you want to be able to use RTTI
 * and serialization features on it.
 *
 * ReflectionObjects implement a thread-safe reference counting mechanism.
 * Because we may want to store constant references to ReflectionObjects,
 * or even simply use them in a constant context and still change their reference
 * count, this mechanism was made constant and is implemented using mutable values.
 * Thus - a reference counter is not considered a part of the object's state - it's
 * rather its trait.
 */
class ReflectionObject
{
   DECLARE_ALLOCATOR( ReflectionObject, AM_DEFAULT );
   DECLARE_CLASS();

public:
   // a unique id assigned to each instance
   typedef std::string                                UniqueId;
   UniqueId                                           m_uniqueId;

protected:
   // The list is mutable, because we also need to be able to attach external listeners to 'const' objects.
   // After all - in this case we're only interested in listening to changes the object undergoes - so 
   // it may as well be constant from that context. 
   // And this is the only way we can go around teh fact that we keep a list of attached listeners on such a constant object.
   mutable List< ReflectionObjectChangeListener* >    m_listener;

private:
   ReflectionObjectReferencesManager*                 m_referencesListener;

   // a counter that counts the number of references to this object
   CriticalSection*                                   m_referencesCounterLock;
   mutable int                                        m_referencesCounter;

public:
   /**
    * Copy constructor.
    *
    * @param rhs
    */
   ReflectionObject( const ReflectionObject& rhs );
   virtual ~ReflectionObject();

   /**
    * Checks if the type of this instance matches exactly the specified reference type.
    *
    * @param referenceType
    */
   bool isExactlyA( const SerializableReflectionType& referenceType ) const;

   /**
    * Checks if the type of this instance matches exactly the specified reference type.
    *
    * @param ReferenceType
    */
   template< typename ReferenceType >
   bool isExactlyA() const;

   /**
    * Checks if this instance can be safely downcast to the specified reference type.
    *
    * @param referenceType
    */
   bool isA( const SerializableReflectionType& referenceType ) const;

   /**
    * Checks if this instance can be safely downcast to the specified reference type.
    *
    * @param ReferenceType
    */
   template< typename ReferenceType >
   bool isA() const;

   /**
    * Gives the specified view access to the object's properties,
    * so that their values can be edited.
    *
    * @param view
    */
   void viewProperties( ReflectionPropertiesView& view );

   // -------------------------------------------------------------------------
   // Notifications
   // -------------------------------------------------------------------------

   /**
    * Attaches a new object listener.
    *
    * @param listener
    */
   void attachListener( ReflectionObjectChangeListener& listener ) const;

   /**
    * Attaches a new object listener.
    *
    * @param listener
    */
   void detachListener( ReflectionObjectChangeListener& listener ) const;

   // -------------------------------------------------------------------------
   // Property value change notifications
   // -------------------------------------------------------------------------

   /**
    * Notifies that the value of one of this object's properties has changed.
    *
    * CAUTION: The caller is taking responsibility for destroying the property instance 
    *          once he's done with it
    *
    * @param propertyName     name of the property that's changed
    */
   ReflectionProperty* getProperty( const std::string& propertyName );

   /**
    * Notifies that the value of one of this object's properties is about to change.
    *
    * @param propertyName     name of the property that's about to change
    */
   void notifyPrePropertyChange( ReflectionProperty& property );

   /**
    * Notifies that the value of one of this object's properties has changed.
    *
    * @param propertyName     name of the property that's changed
    */
   void notifyPropertyChange( ReflectionProperty& property );

   /**
    * Notifies the object that it's just been loaded.
    * Called once an object and all of its dependencies is fully loaded.
    */
   void notifyObjectLoaded();

   /**
    * Called before an object is serialized.
    */
   virtual void onObjectPreSave() {}

   // -------------------------------------------------------------------------
   // References counting
   // -------------------------------------------------------------------------
   /**
    * Checks how many references to the object are there.
    */
   int getReferencesCount() const;

   /**
    * Adds a new reference to the object.
    */
   void addReference() const;

   /**
    * Removes a reference to the object.
    * 
    * When the references counter reaches 0, the object will be automatically deleted.
    */
   void removeReference() const;

protected:
   /**
    * Constructor.
    *
    * @param uniqueId   ( optional )
    */
   ReflectionObject( const char* uniqueId = NULL );

   /**
    * Called once an object and all of its dependencies is fully loaded.
    *
    * CAUTION: the order in which those methods will be called on dependent objects
    * is UNDEFINED. Therefore it's not a good place to be accessing any methods on the
    * referenced objects, because those objects onObjectLoaded method may have not yet been called !
    *
    * If you need to do that, Resurce::onResourceLoaded is the place to do it as that method
    * will be called once ALL dependencies have been restored and all objects have their onObjectLoaded method called.
    */
   virtual void onObjectLoaded() {}

   /**
    * Called when a property value's about to change.
    *
    * @param property
    */
   virtual void onPrePropertyChanged( ReflectionProperty& property ) {}

   /**
    * Called after a property value changes.
    *
    * @param property
    */
   virtual void onPropertyChanged( ReflectionProperty& property ) {}

   /**
    * Call this method in your constructor IF you have any references registered
    * ( properties registered with PROPERTY_EDIT_REFERENCE macro ).
    */
   void registerReferences();
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Casts an object from one type to another. The types need to share a common hierarchy.
 */
template< typename T >
T* DynamicCast( ReflectionObject* obj );

/**
* Casts an object from one type to another. The types need to share a common hierarchy ( const version )
*/
template< typename T >
const T* DynamicCast( const ReflectionObject* obj );

///////////////////////////////////////////////////////////////////////////////

/**
 * A macro that safely deletes a ReflectionObject instance
 */
#define SAFE_DELETE( object ) \
   if ( object ) \
   { \
      object->removeReference(); \
      object = NULL; \
   }

///////////////////////////////////////////////////////////////////////////////

#include "core/ReflectionObject.inl"

///////////////////////////////////////////////////////////////////////////////

// an include that facilitates the use of ReflectionObject, so that we don't
// have to add similar includes to each file that contains a class that
// uses the ReflectionObject
#include "core/ReflectionType.h"

///////////////////////////////////////////////////////////////////////////////

#endif // _REFLECTION_OBJECT_H

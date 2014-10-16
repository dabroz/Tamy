/// @file   core\ReflectionProperty.h
/// @brief  generic property interface
#ifndef REFLECTION_PROPERTY_H
#define REFLECTION_PROPERTY_H

#include <string>
#include <map>
#include <vector>
#include "core\MemoryRouter.h"
#include "core\types.h"
#include "core\RefPtr.h"
#include "core\Array.h"


///////////////////////////////////////////////////////////////////////////////

class ReflectionObject;
class ReflectionProperty;
class ReflectionPropertiesView;
class ReflectionType;

///////////////////////////////////////////////////////////////////////////////

/**
 * This is a generic property interface. It will allow to store properties
 * of various data types in a uniform manner in all sorts of collections.
 *
 * A property can be observed - its observers will get notified about changes 
 * the property undergoes.
 */
class ReflectionProperty
{
   DECLARE_ALLOCATOR( ReflectionProperty, AM_DEFAULT );

private:
   ReflectionObject*                m_observer;
   uint                             m_id;
   std::string                      m_name;
   std::string                      m_label;
   byte                             m_traits;

public:
   virtual ~ReflectionProperty() {}

   /**
    * Sets the property name.
    *
    * @param id
    * @param name
    * @param label
    * @param traits
    */
   inline void setParams( uint id, const std::string& name, const std::string& label, byte traits );

   /**
    * This method returns the id assigned to the member field this property represents.
    *
    * @return  property id
    */
   inline uint getId() const;

   /**
    * This method returns the name assigned to this property.
    *
    * @return  property name
    */
   inline const std::string& getName() const;

   /**
    * This method returns a label under which the property should
    * be advertised (in editor i.e.)
    *
    * @return  property label
    */
   inline const std::string& getLabel() const;

   /**
    * Does this property represent a reference to a ReflectionObject.
    */
   inline bool isReference() const;

   /**
    * Tells whether we want to edit this property or not.
    */
   inline bool canBeEdited() const;

   /**
    * Sets a new value of the property.
    *
    * @param val
    */
   virtual void set( void* val ) = 0;

   /**
    * Allows to edit the property contents by a property editor.
    */
   virtual void* edit() = 0;

   /**
    * Tells if the property can aggregate other properties, or is it a leaf.
    */
   virtual bool isComposite() const = 0;

   /**
    * Returns an instance of ReflectionObject this property belongs to.
    */
   inline ReflectionObject* getHostObject() const { return m_observer; }

   // -------------------------------------------------------------------------
   // Type identification mechanism.
   // -------------------------------------------------------------------------
   /**
    * Retrieves the virtual (topmost in terms of inheritance hierarchy) class
    * of the element stored in the property.
    *
    * i.e. lets take the following classes hierarchy:
    *       class A {};  class B : public A {};
    *    and property setup:
    *       B* val;
    *       TProperty< A* > property( val );
    *    then:
    *       property.getVirtualClass() == B;
    */
   virtual const ReflectionType* getVirtualClass() const = 0;

   /**
    * Returns the type the property is parametrized with ( static type, but accessed through a specific instance ).
    */
   virtual const ReflectionType& getPropertyClass() const = 0;

   /**
    * Another RTTI method is required:
    *
    * static const ReflectionType& getRTTIClass();
    *
    * It should return the static type of the represented property.
    */

   // -------------------------------------------------------------------------
   // Observation mechanism
   // -------------------------------------------------------------------------
   /**
    * Call this method when the value of the property is about to change ( a great moment
    * to clean up after the old value ).
    */
   void notifyBeforeChange();

   /**
    * Call this method when the value of the property changes.
    */
   void notifyAfterChange();

protected:
   /**
    * Constructor.
    *
    * @param hostObject
    */
   ReflectionProperty( ReflectionObject* hostObject = NULL );

};

///////////////////////////////////////////////////////////////////////////////

/**
 * This specialized property allows to create properties for all types of data.
 */
template< typename T >
class TReflectionProperty : public ReflectionProperty
{
   DECLARE_ALLOCATOR( TReflectionProperty, AM_DEFAULT );

private:
   T*             m_val;

public:
   /**
    * Constructor.
    *
    * @param hostObject
    * @param val
    */
   TReflectionProperty( ReflectionObject* hostObject, T* val );

   // -------------------------------------------------------------------------
   // ReflectionProperty implementation
   // -------------------------------------------------------------------------
   void set( void* val );
   void* edit();
   bool isComposite() const { return false; }

   // -------------------------------------------------------------------------
   // Type identification mechanism implementation.
   // -------------------------------------------------------------------------
   const ReflectionType* getVirtualClass() const;
   const ReflectionType& getPropertyClass() const;
   static const ReflectionType& getRTTIClass();
};

///////////////////////////////////////////////////////////////////////////////

/**
 * This specialized property allows to create properties for all types of pointers.
 */
template< typename T >
class TReflectionProperty< T* > : public ReflectionProperty
{
   DECLARE_ALLOCATOR( TReflectionProperty, AM_DEFAULT );

private:
   T** m_val;

public:
   /**
    * Constructor.
    *
    * @param hostObject
    * @param val
    */
   TReflectionProperty( ReflectionObject* hostObject, T** val );

   // -------------------------------------------------------------------------
   // ReflectionProperty implementation
   // -------------------------------------------------------------------------
   void set( void* val );
   void* edit();
   bool isComposite() const { return false; }

   // -------------------------------------------------------------------------
   // Type identification mechanism implementation.
   // -------------------------------------------------------------------------
   const ReflectionType* getVirtualClass() const;
   const ReflectionType& getPropertyClass() const;
   static const ReflectionType& getRTTIClass();
};

///////////////////////////////////////////////////////////////////////////////

/**
 * This specialized property allows to create properties for all types of reference counted pointers.
 */
template< typename T >
class TReflectionProperty< TRefPtr< T > > : public ReflectionProperty
{
   DECLARE_ALLOCATOR( TReflectionProperty, AM_DEFAULT );

private:
   TRefPtr< T >*  m_val;
   T*             m_tmpVal;

public:
   /**
    * Constructor.
    *
    * @param hostObject
    * @param val
    */
   TReflectionProperty( ReflectionObject* hostObject, TRefPtr< T >* val );

   // -------------------------------------------------------------------------
   // ReflectionProperty implementation
   // -------------------------------------------------------------------------
   void set( void* val );
   void* edit();
   bool isComposite() const { return false; }

   // -------------------------------------------------------------------------
   // Type identification mechanism implementation.
   // -------------------------------------------------------------------------
   const ReflectionType* getVirtualClass() const;
   const ReflectionType& getPropertyClass() const;
   static const ReflectionType& getRTTIClass();
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A marker interface for the properties that holds arrays of data.
 * It's needed so that we can write generic editors for those properties.
 */
class ReflectionPropertyArray : public ReflectionProperty
{
   DECLARE_ALLOCATOR( ReflectionPropertyArray, AM_DEFAULT );

private:
   std::string             m_emptyStr;

public:
   /**
    * Constructor.
    *
    * @param hostObject
    */
   ReflectionPropertyArray( ReflectionObject* hostObject );
   virtual ~ReflectionPropertyArray() {}

   /**
    * Returns the size of the array.
    */
   virtual uint size() const = 0;

   /**
    * Returns an object the specified array element contains.
    *
    * @param idx     element's index
    */
   virtual ReflectionObject* getElement( uint idx ) = 0;

   // -------------------------------------------------------------------------
   // ReflectionProperty implementation
   // -------------------------------------------------------------------------
   bool isComposite() const { return true; }
};

///////////////////////////////////////////////////////////////////////////////

/**
 * This specialized property can hold a vector of pointers
 */
template< typename T >
class TReflectionProperty< std::vector< T* > > : public ReflectionPropertyArray
{
   DECLARE_ALLOCATOR( TReflectionProperty, AM_DEFAULT );

private:
   std::vector<T*>*     m_val;

public:
   /**
    * Constructor.
    *
    * @param hostObject
    * @param val
    */
   TReflectionProperty( ReflectionObject* hostObject, std::vector<T*>* val );

   // -------------------------------------------------------------------------
   // VectorProperty implementation
   // -------------------------------------------------------------------------
   uint size() const;
   ReflectionObject* getElement( uint idx );

   // -------------------------------------------------------------------------
   // ReflectionProperty implementation
   // -------------------------------------------------------------------------
   void set( void* val );
   void* edit();

   // -------------------------------------------------------------------------
   // Type identification mechanism implementation.
   // -------------------------------------------------------------------------
   const ReflectionType* getVirtualClass() const;
   const ReflectionType& getPropertyClass() const;
   static const ReflectionType& getRTTIClass();
};

///////////////////////////////////////////////////////////////////////////////

/**
 * This specialized property can hold a vector of reference counted pointers
 */
template< typename T >
class TReflectionProperty< std::vector< TRefPtr< T > > > : public ReflectionPropertyArray
{
   DECLARE_ALLOCATOR( TReflectionProperty, AM_DEFAULT );

private:
   std::vector< TRefPtr< T > >*     m_val;
   std::vector< T* >                m_tmpVal;

public:
   /**
    * Constructor.
    *
    * @param hostObject
    * @param val
    */
   TReflectionProperty( ReflectionObject* hostObject, std::vector< TRefPtr< T > >* val );

   // -------------------------------------------------------------------------
   // VectorProperty implementation
   // -------------------------------------------------------------------------
   uint size() const;
   ReflectionObject* getElement( uint idx );

   // -------------------------------------------------------------------------
   // ReflectionProperty implementation
   // -------------------------------------------------------------------------
   void set( void* val );
   void* edit();

   // -------------------------------------------------------------------------
   // Type identification mechanism implementation.
   // -------------------------------------------------------------------------
   const ReflectionType* getVirtualClass() const;
   const ReflectionType& getPropertyClass() const;
   static const ReflectionType& getRTTIClass();
};

///////////////////////////////////////////////////////////////////////////////

/**
 * This specialized property can hold an array of pointers
 */
template< typename T >
class TReflectionProperty< Array< T* > > : public ReflectionPropertyArray
{
   DECLARE_ALLOCATOR( TReflectionProperty, AM_DEFAULT );

private:
   Array<T*>*     m_val;

public:
   /**
    * Constructor.
    *
    * @param hostObject
    * @param val
    */
   TReflectionProperty( ReflectionObject* hostObject, Array<T*>* val );

   // -------------------------------------------------------------------------
   // VectorProperty implementation
   // -------------------------------------------------------------------------
   uint size() const;
   ReflectionObject* getElement( uint idx );

   // -------------------------------------------------------------------------
   // ReflectionProperty implementation
   // -------------------------------------------------------------------------
   void set( void* val );
   void* edit();

   // -------------------------------------------------------------------------
   // Type identification mechanism implementation.
   // -------------------------------------------------------------------------
   const ReflectionType* getVirtualClass() const;
   const ReflectionType& getPropertyClass() const;
   static const ReflectionType& getRTTIClass();
};

///////////////////////////////////////////////////////////////////////////////

/**
 * This specialized property can hold an array of reference counted pointers
 */
template< typename T >
class TReflectionProperty< Array< TRefPtr< T > > > : public ReflectionPropertyArray
{
   DECLARE_ALLOCATOR( TReflectionProperty, AM_DEFAULT );

private:
   Array< TRefPtr< T > >*     m_val;
   Array< T* >                m_tmpVal;

public:
   /**
    * Constructor.
    *
    * @param hostObject
    * @param val
    */
   TReflectionProperty( ReflectionObject* hostObject, Array< TRefPtr< T > >* val );

   // -------------------------------------------------------------------------
   // VectorProperty implementation
   // -------------------------------------------------------------------------
   uint size() const;
   ReflectionObject* getElement( uint idx );

   // -------------------------------------------------------------------------
   // ReflectionProperty implementation
   // -------------------------------------------------------------------------
   void set( void* val );
   void* edit();

   // -------------------------------------------------------------------------
   // Type identification mechanism implementation.
   // -------------------------------------------------------------------------
   const ReflectionType* getVirtualClass() const;
   const ReflectionType& getPropertyClass() const;
   static const ReflectionType& getRTTIClass();
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Editable version of a property.
 */
template< typename T >
class TEditableReflectionProperty
{
   DECLARE_ALLOCATOR( TEditableReflectionProperty, AM_DEFAULT );

private:
   ReflectionProperty*   m_property;

public:
   TEditableReflectionProperty( ReflectionProperty* property );
   ~TEditableReflectionProperty();

   /**
    * This method returns a label under which the property should
    * be advertised (in editor i.e.)
    *
    * @return  property label
    */
   const std::string& getLabel() const;

   /**
    * Does this property represent a reference to a ReflectionObject.
    */
   bool isReference() const;

   /**
    * Sets new property value.
    *
    * @param val     new property value
    */
   void set( const T& val );

   /**
    * Returns current property value (const version).
    *
    * @return        property value.
    */
   const T& get() const;

   /**
    * Returns current property value.
    *
    * @return        property value.
    */
   T& get();

   /**
    * Returns the type the property is parametrized with.
    */
   const ReflectionType& getType() const;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A handy macro that facilitates the process of notifying about a property value
 * change
 */
#define NOTIFY_PROPERTY_CHANGE( variable ) ReflectionPropertValueChangeNotifier __variable_changeNotifier( this, #variable );

/**
 * A utility class used by the NOTIFY_PROPERTY_CHANGE.
 */
class ReflectionPropertValueChangeNotifier
{
private:
   ReflectionObject*          m_owner;
   ReflectionProperty*        m_property;

public:
   /**
    * Constructor.
    *
    * @param owner
    * @param variable
    */
   ReflectionPropertValueChangeNotifier( ReflectionObject* owner, const char* variable );
   ~ReflectionPropertValueChangeNotifier();
};

///////////////////////////////////////////////////////////////////////////////

#include "core\ReflectionProperty.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // REFLECTION_PROPERTY_H

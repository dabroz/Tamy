/// @file   core/SerializedReflectionObject.h
/// @brief  structures used to serialize a ReflectionObject instance
#ifndef _SERIALIZED_REFLECTION_OBJECT_H
#define _SERIALIZED_REFLECTION_OBJECT_H

#include "core\types.h"
#include "core\Array.h"
#include "core\MemoryRouter.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

class InStream;
class OutStream;

///////////////////////////////////////////////////////////////////////////////

enum SerializedFieldType
{
   SFT_Unknown,

   SFT_Value,
   SFT_Pointer,
   SFT_ArrayOfValues,
   SFT_ArrayOfPointers,
};

///////////////////////////////////////////////////////////////////////////////

class SerializedValue
{
   DECLARE_ALLOCATOR( SerializedValue, AM_DEFAULT );

public:
   uint              m_id;

private:
   Array< byte >     m_buffer;

public:
   /**
    * Constructor.
    *
    * @param id      fieldId
    */
   SerializedValue( uint id );

   /**
    * Serialized the specified value.
    *
    * @param ptr
    */
   template< typename T >
   void set( const T& value );

   /**
    * Initializes the specified variable with the serialized value.
    *
    * @param value
    */
   template< typename T >
   void initialize( T& value ) const;

   // -------------------------------------------------------------------------
   // Serialization support
   // -------------------------------------------------------------------------
   friend OutStream& operator<<( OutStream& stream, const SerializedValue& object );
   friend InStream& operator>>( InStream& stream, SerializedValue& object );
};

///////////////////////////////////////////////////////////////////////////////

class SerializedPointer
{
   DECLARE_ALLOCATOR( SerializedPointer, AM_DEFAULT );

public:
   uint              m_id;

private:
   uint              m_ptrVal;

public:
   /**
    * Constructor.
    *
    * @param id      fieldId
    */
   SerializedPointer( uint id );

   /**
    * Sets the pointer value.
    *
    * @param ptr
    */
   void set( const void* ptr );

   /**
    * Initializes the specified variable with the serialized pointer.
    *
    * @param ptr
    */
   void initialize( void*& ptr ) const;

   // -------------------------------------------------------------------------
   // Serialization support
   // -------------------------------------------------------------------------
   friend OutStream& operator<<( OutStream& stream, const SerializedPointer& object );
   friend InStream& operator>>( InStream& stream, SerializedPointer& object );
};

///////////////////////////////////////////////////////////////////////////////

class SerializedArrayOfValues
{
   DECLARE_ALLOCATOR( SerializedArrayOfValues, AM_DEFAULT );

public:
   uint              m_id;

private:
   Array< uint >     m_startOffsets;
   Array< byte >     m_buffer;

public:
   /**
    * Constructor.
    *
    * @param id         fieldId
    */
   SerializedArrayOfValues( uint id );

   /**
    * Sets a new size of the array.
    *
    * @param size
    */
   void setSize( uint size );

   /**
    * Serializes the specified value, adding it to the array at the specified element index.
    *
    * @param elemIdx
    * @param value
    */
   template< typename T >
   void set( uint elemIdx, const T& value );

   /**
    * Returns a number of elements stored in the array.
    */
   uint size() const;

   /**
    * Initializes the specified variable with a value of the specified element.
    *
    * @param elemIdx
    * @param value
    */
   template< typename T >
   void initialize( uint elemIdx, T& value ) const;

   // -------------------------------------------------------------------------
   // Serialization support
   // -------------------------------------------------------------------------
   friend OutStream& operator<<( OutStream& stream, const SerializedArrayOfValues& object );
   friend InStream& operator>>( InStream& stream, SerializedArrayOfValues& object );
};

///////////////////////////////////////////////////////////////////////////////

class SerializedArrayOfPointers
{
   DECLARE_ALLOCATOR( SerializedArrayOfPointers, AM_DEFAULT );

public:
   uint              m_id;

private:
   Array< uint >     m_pointers;

public:
   /**
    * Constructor.
    *
    * @param id         fieldId
    */
   SerializedArrayOfPointers( uint id );

   /**
    * Sets a new size of the array.
    *
    * @param size
    */
   void setSize( uint size );

   /**
    * Serializes the specified pointer, adding it to the array at the specified element index.
    *
    * @param elemIdx
    * @param value
    */
   void set( uint elemIdx, const void* ptr );

   /**
    * Returns a number of elements stored in the array.
    */
   uint size() const;

   /**
    * Initializes the specified variable with a value of the serialized pointer.
    *
    * @param ptr
    */
   void initialize(uint elemIdx, void*& ptr ) const;

   // -------------------------------------------------------------------------
   // Serialization support
   // -------------------------------------------------------------------------
   friend OutStream& operator<<( OutStream& stream, const SerializedArrayOfPointers& object );
   friend InStream& operator>>( InStream& stream, SerializedArrayOfPointers& object );
};

///////////////////////////////////////////////////////////////////////////////

class SerializedReflectionObject
{
   DECLARE_ALLOCATOR( SerializedReflectionObject, AM_DEFAULT );

private:
   Array< SerializedValue* >             m_values;
   Array< SerializedPointer* >           m_pointers;
   Array< SerializedArrayOfValues* >     m_arraysOfValues;
   Array< SerializedArrayOfPointers* >   m_arraysOfPointers;

public:
   ~SerializedReflectionObject();

   /**
    * Adds a field with the specified type.
    *
    * @param type
    */
   void addField( uint fieldId, SerializedFieldType type );

   /**
    * Adds a new primitive value.
    *
    * @param id
    */
   SerializedValue& addValue( uint fieldId );

   /**
    * Adds a new pointer.
    *
    * @param id
    */
   SerializedPointer& addPointer( uint fieldId );

   /**
    * Adds an array of primitive values.
    *
    * @param id
    */
   SerializedArrayOfValues& addArrayOfValues( uint fieldId );

   /**
    * Adds an array of pointers values.
    *
    * @param id
    */
   SerializedArrayOfPointers& addArrayOfPointers( uint fieldId );

   /**
    * Removes the specified field from the object.
    *
    * @param fieldId
    */
   void removeField( uint fieldId );

   /**
    * Change an id of one of the fields.
    *
    * @param oldFieldId
    * @param newFieldId
    */
   void changeFieldId( uint oldFieldId, uint newFieldId );

   // -------------------------------------------------------------------------
   // Data query methods
   // -------------------------------------------------------------------------

   /**
    * Queries for a primitive value ( by id )
    *
    * @param fieldId
    */
   const SerializedValue* getValue( uint fieldId ) const;
   SerializedValue* getValue( uint fieldId );

   /**
    * Queries for a primitive value ( by name )
    *
    * @param fieldNa
    */
   const SerializedValue* getValue( const std::string& fieldName ) const;
   SerializedValue* getValue( const std::string& fieldName );

   /**
    * Queries for a pointer value ( by id )
    *
    * @param fieldId
    */
   const SerializedPointer* getPointer( uint fieldId ) const;
   SerializedPointer* getPointer( uint fieldId );

   /**
    * Queries for a pointer value ( by name )
    *
     * @param fieldName
    */
   const SerializedPointer* getPointer( const std::string& fieldName ) const;
   SerializedPointer* getPointer( const std::string& fieldName );

   /**
    * Queries for an array of values ( by id )
    *
    * @param fieldId
    */
   const SerializedArrayOfValues* getArrayOfValues( uint fieldId ) const;
   SerializedArrayOfValues* getArrayOfValues( uint fieldId );

   /**
    * Queries for an array of values ( by name )
    *
     * @param fieldName
    */
   const SerializedArrayOfValues* getArrayOfValues( const std::string& fieldName ) const;
   SerializedArrayOfValues* getArrayOfValues( const std::string& fieldName );

   /**
    * Queries for an array of pointers values ( by id )
    *
    * @param fieldId
    */
   const SerializedArrayOfPointers* getArrayOfPointers( uint fieldId ) const;
   SerializedArrayOfPointers* getArrayOfPointers( uint fieldId );

   /**
    * Queries for an array of pointers values ( by name )
    *
     * @param fieldName
    */
   const SerializedArrayOfPointers* getArrayOfPointers( const std::string& fieldName ) const;
   SerializedArrayOfPointers* getArrayOfPointers( const std::string& fieldName );

   // -------------------------------------------------------------------------
   // Serialization methods
   // -------------------------------------------------------------------------
   /**
    * Serializes the object to the specified output stream.
    *
    * @param stream
    * @param object
    */
   friend OutStream& operator<<( OutStream& stream, const SerializedReflectionObject& object );

   /**
    * Deserializes the object from the specified input stream.
    *
    * @param stream
    * @param object
    */
   friend InStream& operator>>( InStream& stream, SerializedReflectionObject& object );
};

///////////////////////////////////////////////////////////////////////////////

#include "core\SerializedReflectionObject.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _SERIALIZED_REFLECTION_OBJECT_H

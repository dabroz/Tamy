#include "core\SerializedReflectionObject.h"
#include "core\ReflectionTypeComponent.h"
#include "core\OutArrayStream.h"
#include "core\InArrayStream.h"
#include "core\OutRawArrayStream.h"
#include "core\InRawArrayStream.h"


///////////////////////////////////////////////////////////////////////////////

SerializedValue::SerializedValue( uint id )
   : m_id( id )
{
}

///////////////////////////////////////////////////////////////////////////////

OutStream& operator<<( OutStream& stream, const SerializedValue& object )
{
   stream << object.m_id;
   stream << object.m_buffer;

   return stream;
}

///////////////////////////////////////////////////////////////////////////////

InStream& operator>>( InStream& stream, SerializedValue& object )
{
   stream >> object.m_id;
   stream >> object.m_buffer;

   return stream;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

SerializedPointer::SerializedPointer( uint id )
   : m_id( id )
   , m_ptrVal( 0 )
{
}

///////////////////////////////////////////////////////////////////////////////

void SerializedPointer::set( const void* ptr )
{
   m_ptrVal = (uint)ptr;
}

///////////////////////////////////////////////////////////////////////////////

void SerializedPointer::initialize( void*& ptr ) const
{
   ptr = (void*)m_ptrVal;
}

///////////////////////////////////////////////////////////////////////////////

OutStream& operator<<( OutStream& stream, const SerializedPointer& object )
{
   stream << object.m_id;
   stream << object.m_ptrVal;

   return stream;
}

///////////////////////////////////////////////////////////////////////////////

InStream& operator>>( InStream& stream, SerializedPointer& object )
{
   stream >> object.m_id;
   stream >> object.m_ptrVal;

   return stream;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

SerializedArrayOfValues::SerializedArrayOfValues( uint id )
   : m_id( id )
{
}

///////////////////////////////////////////////////////////////////////////////

void SerializedArrayOfValues::setSize( uint size )
{
   m_buffer.clear();
   m_startOffsets.clear();
}

///////////////////////////////////////////////////////////////////////////////

uint SerializedArrayOfValues::size() const
{
   return m_startOffsets.size();
}

///////////////////////////////////////////////////////////////////////////////

OutStream& operator<<( OutStream& stream, const SerializedArrayOfValues& object )
{
   stream << object.m_id;
   stream << object.m_buffer;
   stream << object.m_startOffsets;

   return stream;
}

///////////////////////////////////////////////////////////////////////////////

InStream& operator>>( InStream& stream, SerializedArrayOfValues& object )
{
   stream >> object.m_id;
   stream >> object.m_buffer;
   stream >> object.m_startOffsets;

   return stream;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

SerializedArrayOfPointers::SerializedArrayOfPointers( uint id )
   : m_id( id )
{
}

///////////////////////////////////////////////////////////////////////////////

void SerializedArrayOfPointers::setSize( uint size )
{
   m_pointers.resize( size, 0 );
}

///////////////////////////////////////////////////////////////////////////////

void SerializedArrayOfPointers::set( uint elemIdx, const void* ptr )
{
   m_pointers[elemIdx] = (uint)ptr;
}

///////////////////////////////////////////////////////////////////////////////

uint SerializedArrayOfPointers::size() const
{
   return m_pointers.size();
}

///////////////////////////////////////////////////////////////////////////////

void SerializedArrayOfPointers::initialize(uint elemIdx, void*& ptr ) const
{
   ptr = (void*)m_pointers[elemIdx];
}

///////////////////////////////////////////////////////////////////////////////

OutStream& operator<<( OutStream& stream, const SerializedArrayOfPointers& object )
{
   stream << object.m_id;
   stream << object.m_pointers;

   return stream;
}

///////////////////////////////////////////////////////////////////////////////

InStream& operator>>( InStream& stream, SerializedArrayOfPointers& object )
{
   stream >> object.m_id;
   stream >> object.m_pointers;

   return stream;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

SerializedReflectionObject::~SerializedReflectionObject()
{
   uint count = m_values.size();
   for ( uint i = 0; i < count; ++i )
   {
      delete m_values[i];
   }

   count = m_pointers.size();
   for ( uint i = 0; i < count; ++i )
   {
      delete m_pointers[i];
   }

   count = m_arraysOfValues.size();
   for ( uint i = 0; i < count; ++i )
   {
      delete m_arraysOfValues[i];
   }

   count = m_arraysOfPointers.size();
   for ( uint i = 0; i < count; ++i )
   {
      delete m_arraysOfPointers[i];
   }
}

///////////////////////////////////////////////////////////////////////////////

void SerializedReflectionObject::addField( uint fieldId, SerializedFieldType type )
{
   switch( type )
   {
   case SFT_Value:
      {
         addValue( fieldId );
         break;
      }

   case SFT_Pointer:
      {
         addPointer( fieldId );
         break;
      }

   case SFT_ArrayOfValues:
      {
         addArrayOfValues( fieldId );
         break;
      }

   case SFT_ArrayOfPointers:
      {
         addArrayOfPointers( fieldId );
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

SerializedValue& SerializedReflectionObject::addValue( uint fieldId )
{
   // check for duplicates
   uint count = m_values.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_values[i]->m_id == fieldId )
      {
         // such field already exists
         return *m_values[i];
      }
   }

   SerializedValue* element = new SerializedValue( fieldId );
   m_values.push_back( element );
   return *element;
}

///////////////////////////////////////////////////////////////////////////////

SerializedPointer& SerializedReflectionObject::addPointer( uint fieldId )
{
   // check for duplicates
   uint count = m_pointers.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_pointers[i]->m_id == fieldId )
      {
         // such field already exists
         return *m_pointers[i];
      }
   }

   SerializedPointer* element = new SerializedPointer( fieldId );
   m_pointers.push_back( element );
   return *element;
}

///////////////////////////////////////////////////////////////////////////////

SerializedArrayOfValues& SerializedReflectionObject::addArrayOfValues( uint fieldId )
{
   // check for duplicates
   uint count = m_arraysOfValues.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_arraysOfValues[i]->m_id == fieldId )
      {
         // such field already exists
         return *m_arraysOfValues[i];
      }
   }

   SerializedArrayOfValues* element = new SerializedArrayOfValues( fieldId );
   m_arraysOfValues.push_back( element );
   return *element;
}

///////////////////////////////////////////////////////////////////////////////

SerializedArrayOfPointers& SerializedReflectionObject::addArrayOfPointers( uint fieldId )
{
   // check for duplicates
   uint count = m_arraysOfPointers.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_arraysOfPointers[i]->m_id == fieldId )
      {
         // such field already exists
         return *m_arraysOfPointers[i];
      }
   }

   SerializedArrayOfPointers* element = new SerializedArrayOfPointers( fieldId );
   m_arraysOfPointers.push_back( element );
   return *element;
}

///////////////////////////////////////////////////////////////////////////////

void SerializedReflectionObject::removeField( uint fieldId )
{
   // check all data storages, and as soon as you find the field - remove it

   uint count = m_values.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_values[i]->m_id == fieldId )
      {
         delete m_values[i];
         m_values.remove( i );
         return;
      }
   }

   count = m_pointers.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_pointers[i]->m_id == fieldId )
      {
         delete m_pointers[i];
         m_pointers.remove( i );
         return;
      }
   }

   count = m_arraysOfValues.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_arraysOfValues[i]->m_id == fieldId )
      {
         delete m_arraysOfValues[i];
         m_arraysOfValues.remove( i );
         return;
      }
   }

   count = m_arraysOfPointers.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_arraysOfPointers[i]->m_id == fieldId )
      {
         delete m_arraysOfPointers[i];
         m_arraysOfPointers.remove( i );
         return;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void SerializedReflectionObject::changeFieldId( uint oldFieldId, uint newFieldId )
{
   // check all data storages, and as soon as you find the field - update it

   uint count = m_values.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_values[i]->m_id == oldFieldId )
      {
         m_values[i]->m_id = newFieldId;
         return;
      }
   }

   count = m_pointers.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_pointers[i]->m_id == oldFieldId )
      {
         m_pointers[i]->m_id = newFieldId;
         return;
      }
   }

   count = m_arraysOfValues.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_arraysOfValues[i]->m_id == oldFieldId )
      {
         m_arraysOfValues[i]->m_id = newFieldId;
         return;
      }
   }

   count = m_arraysOfPointers.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_arraysOfPointers[i]->m_id == oldFieldId )
      {
         m_arraysOfPointers[i]->m_id = newFieldId;
         return;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

const SerializedValue* SerializedReflectionObject::getValue( uint fieldId ) const
{
   uint count = m_values.size();
   for ( uint i = 0; i < count; ++i )
   {
      const SerializedValue* element = m_values[i];
      if ( fieldId == element->m_id )
      {
         return element;
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

SerializedValue* SerializedReflectionObject::getValue( uint fieldId )
{
   uint count = m_values.size();
   for ( uint i = 0; i < count; ++i )
   {
      SerializedValue* element = m_values[i];
      if ( fieldId == element->m_id )
      {
         return element;
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

const SerializedValue* SerializedReflectionObject::getValue( const std::string& fieldName ) const
{
   uint fieldId = ReflectionTypeComponent::generateId( fieldName );
   return getValue( fieldId );
}

///////////////////////////////////////////////////////////////////////////////

SerializedValue* SerializedReflectionObject::getValue( const std::string& fieldName )
{
   uint fieldId = ReflectionTypeComponent::generateId( fieldName );
   return getValue( fieldId );
}

///////////////////////////////////////////////////////////////////////////////

const SerializedPointer* SerializedReflectionObject::getPointer( uint fieldId ) const
{
   uint count = m_pointers.size();
   for ( uint i = 0; i < count; ++i )
   {
      const SerializedPointer* element = m_pointers[i];
      if ( fieldId == element->m_id )
      {
         return element;
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

SerializedPointer* SerializedReflectionObject::getPointer( uint fieldId )
{
   uint count = m_pointers.size();
   for ( uint i = 0; i < count; ++i )
   {
      SerializedPointer* element = m_pointers[i];
      if ( fieldId == element->m_id )
      {
         return element;
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

const SerializedPointer* SerializedReflectionObject::getPointer( const std::string& fieldName ) const
{
   uint fieldId = ReflectionTypeComponent::generateId( fieldName );
   return getPointer( fieldId );
}

///////////////////////////////////////////////////////////////////////////////

SerializedPointer* SerializedReflectionObject::getPointer( const std::string& fieldName )
{
   uint fieldId = ReflectionTypeComponent::generateId( fieldName );
   return getPointer( fieldId );
}

///////////////////////////////////////////////////////////////////////////////

const SerializedArrayOfValues* SerializedReflectionObject::getArrayOfValues( uint fieldId ) const
{
   uint count = m_arraysOfValues.size();
   for ( uint i = 0; i < count; ++i )
   {
      const SerializedArrayOfValues* element = m_arraysOfValues[i];
      if ( fieldId == element->m_id )
      {
         return element;
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

SerializedArrayOfValues* SerializedReflectionObject::getArrayOfValues( uint fieldId )
{
   uint count = m_arraysOfValues.size();
   for ( uint i = 0; i < count; ++i )
   {
      SerializedArrayOfValues* element = m_arraysOfValues[i];
      if ( fieldId == element->m_id )
      {
         return element;
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

const SerializedArrayOfValues* SerializedReflectionObject::getArrayOfValues( const std::string& fieldName ) const
{
   uint fieldId = ReflectionTypeComponent::generateId( fieldName );
   return getArrayOfValues( fieldId );
}

///////////////////////////////////////////////////////////////////////////////

SerializedArrayOfValues* SerializedReflectionObject::getArrayOfValues( const std::string& fieldName )
{
   uint fieldId = ReflectionTypeComponent::generateId( fieldName );
   return getArrayOfValues( fieldId );
}

///////////////////////////////////////////////////////////////////////////////

const SerializedArrayOfPointers* SerializedReflectionObject::getArrayOfPointers( uint fieldId ) const
{
   uint count = m_arraysOfPointers.size();
   for ( uint i = 0; i < count; ++i )
   {
      const SerializedArrayOfPointers* element = m_arraysOfPointers[i];
      if ( fieldId == element->m_id )
      {
         return element;
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

SerializedArrayOfPointers* SerializedReflectionObject::getArrayOfPointers( uint fieldId )
{
   uint count = m_arraysOfPointers.size();
   for ( uint i = 0; i < count; ++i )
   {
      SerializedArrayOfPointers* element = m_arraysOfPointers[i];
      if ( fieldId == element->m_id )
      {
         return element;
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

const SerializedArrayOfPointers* SerializedReflectionObject::getArrayOfPointers( const std::string& fieldName ) const
{
   uint fieldId = ReflectionTypeComponent::generateId( fieldName );
   return getArrayOfPointers( fieldId );
}

///////////////////////////////////////////////////////////////////////////////

SerializedArrayOfPointers* SerializedReflectionObject::getArrayOfPointers( const std::string& fieldName )
{
   uint fieldId = ReflectionTypeComponent::generateId( fieldName );
   return getArrayOfPointers( fieldId );
}

///////////////////////////////////////////////////////////////////////////////

OutStream& operator<<( OutStream& stream, const SerializedReflectionObject& object )
{
   // values
   uint count = object.m_values.size();
   stream << count;
   for ( uint i = 0; i < count; ++i )
   {
      stream << *object.m_values[i];
   }

   // pointers
   count = object.m_pointers.size();
   stream << count;
   for ( uint i = 0; i < count; ++i )
   {
      stream << *object.m_pointers[i];
   }

   // arrays of values
   count = object.m_arraysOfValues.size();
   stream << count;
   for ( uint i = 0; i < count; ++i )
   {
      stream << *object.m_arraysOfValues[i];
   }

   // arrays of pointers
   count = object.m_arraysOfPointers.size();
   stream << count;
   for ( uint i = 0; i < count; ++i )
   {
      stream << *object.m_arraysOfPointers[i];
   }

   return stream;
}

///////////////////////////////////////////////////////////////////////////////

InStream& operator>>( InStream& stream, SerializedReflectionObject& object )
{
   // values
   uint count;
   stream >> count;
   object.m_values.resize( count );
   for ( uint i = 0; i < count; ++i )
   {
      SerializedValue* element = new SerializedValue(0);
      stream >> *element;
      object.m_values[i] = element;
   }

   // pointers
   stream >> count;
   object.m_pointers.resize( count );
   for ( uint i = 0; i < count; ++i )
   {
      SerializedPointer* element = new SerializedPointer(0);
      stream >> *element;
      object.m_pointers[i] = element;
   }

   // arrays of values
   stream >> count;
   object.m_arraysOfValues.resize( count );
   for ( uint i = 0; i < count; ++i )
   {
      SerializedArrayOfValues* element = new SerializedArrayOfValues(0);
      stream >> *element;
      object.m_arraysOfValues[i] = element;
   }

   // arrays of pointers
   stream >> count;
   object.m_arraysOfPointers.resize( count );
   for ( uint i = 0; i < count; ++i )
   {
      SerializedArrayOfPointers* element = new SerializedArrayOfPointers(0);
      stream >> *element;
      object.m_arraysOfPointers[i] = element;
   }


   return stream;
}

///////////////////////////////////////////////////////////////////////////////


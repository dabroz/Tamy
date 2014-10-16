#ifndef _SERIALIZED_REFLECTION_OBJECT_H
#error "This file can only be included from SerializedReflectionObject.h"
#else

#include "core\OutArrayStream.h"
#include "core\InArrayStream.h"
#include "core\InRawArrayStream.h"


///////////////////////////////////////////////////////////////////////////////

template< typename T >
void SerializedValue::set( const T& value )
{
   m_buffer.clear();
   OutArrayStream stream( m_buffer );
   stream << value;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void SerializedValue::initialize( T& value ) const
{
   T* valPtr = new ( &value ) T();
   
   InArrayStream stream( m_buffer );
   stream >> *valPtr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
void SerializedArrayOfValues::set( uint elemIdx, const T& value )
{
   m_startOffsets.push_back( m_buffer.size() );
   OutArrayStream stream( m_buffer );
   stream << value;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void SerializedArrayOfValues::initialize( uint elemIdx, T& value ) const
{
   uint startOffset = m_startOffsets[elemIdx];
   const byte* startPtr = (const byte*)m_buffer + startOffset;
   uint remainingBufferSize = m_buffer.size() - startOffset;

   T* valPtr = new ( &value ) T();

   InRawArrayStream stream( startPtr, remainingBufferSize );
   stream >> *valPtr;
}

///////////////////////////////////////////////////////////////////////////////

#endif // _SERIALIZED_REFLECTION_OBJECT_H

#ifndef _RUNTIME_DATA_H
#error "This file can only be included from RuntimeData.h"
#else

#include "core\Assert.h"
#include "core\MemoryUtils.h"


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
TRuntimeVar< T >::TRuntimeVar()
{
   m_id = s_nextId++;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
unsigned int TRuntimeVar< T >::getId() const 
{ 
   return m_id; 
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
unsigned int TRuntimeVar< T >::getTypeSize() const 
{ 
   return sizeof( T ); 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
void RuntimeDataBuffer::registerVar( const typename TRuntimeVar< T >& var, const T& defaultVal )
{
   if ( var.isRegistered( this ) )
   {
      ASSERT_MSG( false, "This runtime variable has already been registered." );
      return;
   }

   ulong newEndAddress = MemoryUtils::calcAlignedSize( m_endAddress + var.getTypeSize() );

   ASSERT_MSG( newEndAddress < BUFFER_SIZE, "Allocate a larger buffer" );
   if ( newEndAddress >= BUFFER_SIZE )
   {
      return;
   }


   void* addr = (void*)( m_buffer + m_endAddress );
   void* alignedAddress = MemoryUtils::alignAddress( addr );
   var.registerWithBuffer( this, alignedAddress );

   // initialize the memory 
   new ( alignedAddress ) T( defaultVal );

   // store the new size of the data held in the buffer
   m_endAddress = newEndAddress;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void RuntimeDataBuffer::unregisterVar( const typename TRuntimeVar< T >& var )
{
   // we're not destroying the layout - the memory previously occupied by the variable will unfortunately
   // still remain allocated - but a reference to the variable will be gone

   if ( var.isRegistered( this ) )
   {
      // call variable's destructor
      void* variableAlignedAddress = var.m_variableAddresses[m_bufferIdx];
      static_cast< T* >(variableAlignedAddress)->~T();

      var.removeFromBuffer( this );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
T& RuntimeDataBuffer::operator[]( const typename TRuntimeVar< T >& var )
{
   ASSERT_MSG( var.isRegistered( this ), "This runtime variable wasn't registered.");
   void* data = var.m_variableAddresses[m_bufferIdx];
   return *reinterpret_cast< T* >( data );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const T& RuntimeDataBuffer::operator[]( const typename TRuntimeVar< T >& var ) const
{

   ASSERT_MSG( var.isRegistered( this ), "This runtime variable wasn't registered.");
   void* data = var.m_variableAddresses[m_bufferIdx];
   return *reinterpret_cast< const T* >( data );
}

///////////////////////////////////////////////////////////////////////////////

#endif // _RUNTIME_DATA_H

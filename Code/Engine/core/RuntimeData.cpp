#include "core.h"
#include "core\RuntimeData.h"
#include <stdio.h>


///////////////////////////////////////////////////////////////////////////////

unsigned int IRuntimeVar::s_nextId = 0;

uint RuntimeDataBuffer::s_nextGlobalIndex = 0;
RuntimeDataBuffer::IndexListNode* RuntimeDataBuffer::s_freeBufferIndicesHead = NULL;
RuntimeDataBuffer::IndexListNode* RuntimeDataBuffer::s_freeBufferIndicesTail = NULL;

///////////////////////////////////////////////////////////////////////////////

RuntimeDataBuffer::RuntimeDataBuffer()
   : BUFFER_SIZE( 65536 )
   , m_buffer( NULL )
   , m_endAddress( 0 )
{
   m_bufferIdx = acquireBufferIdx();
   m_buffer = (char*)malloc( BUFFER_SIZE );
}

///////////////////////////////////////////////////////////////////////////////

RuntimeDataBuffer::~RuntimeDataBuffer()
{
   freeBufferIdx( m_bufferIdx );

   free( m_buffer );
   m_buffer = NULL;
}

///////////////////////////////////////////////////////////////////////////////

uint RuntimeDataBuffer::acquireBufferIdx()
{
   if ( s_freeBufferIndicesHead )
   {
      // take an index from the pool of free indices
      uint idx = s_freeBufferIndicesHead->m_freeIdx;
      IndexListNode* nodeToDelete = s_freeBufferIndicesHead;
      s_freeBufferIndicesHead = s_freeBufferIndicesHead->m_next;
      delete nodeToDelete;

      if ( !s_freeBufferIndicesHead )
      {
         s_freeBufferIndicesTail = NULL; 
      }

      return idx;
   }
   else
   {
      uint idx = s_nextGlobalIndex;
      ++s_nextGlobalIndex;
      return idx;
   }
}

///////////////////////////////////////////////////////////////////////////////

void RuntimeDataBuffer::freeBufferIdx( uint idx )
{
   IndexListNode* newNode = new IndexListNode( idx );
   if ( !s_freeBufferIndicesHead )
   {
      s_freeBufferIndicesHead = newNode;
      s_freeBufferIndicesTail = newNode;
   }
   else
   {
      s_freeBufferIndicesTail->m_next = newNode;
      s_freeBufferIndicesTail = newNode;
   }
}

///////////////////////////////////////////////////////////////////////////////

void RuntimeDataBuffer::resetIndicesPool()
{
   while( s_freeBufferIndicesHead )
   {
      IndexListNode* next = s_freeBufferIndicesHead->m_next;
      delete s_freeBufferIndicesHead;
      s_freeBufferIndicesHead = next;
   }

   s_freeBufferIndicesTail = 0;
   s_nextGlobalIndex = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool IRuntimeVar::isRegistered( RuntimeDataBuffer* buffer ) const
{
   if ( !buffer || buffer->m_bufferIdx >= m_variableAddresses.size() )
   {
      return false;
   }

   uint addr = (uint)m_variableAddresses[buffer->m_bufferIdx];
   const uint bufStartAddr = (uint)buffer->m_buffer;
   const uint bufEndAddr = bufStartAddr + buffer->m_endAddress;
   return addr != 0 && addr >= bufStartAddr && addr < bufEndAddr;
}

///////////////////////////////////////////////////////////////////////////////

void IRuntimeVar::registerWithBuffer( RuntimeDataBuffer* buffer, void* address ) const
{
   if ( buffer->m_bufferIdx >= m_variableAddresses.size() )
   {
      m_variableAddresses.resize( buffer->m_bufferIdx + 1, NULL );
   }

   m_variableAddresses[buffer->m_bufferIdx] = address;
}

///////////////////////////////////////////////////////////////////////////////

void IRuntimeVar::removeFromBuffer( RuntimeDataBuffer* buffer ) const
{
   if ( buffer->m_bufferIdx < m_variableAddresses.size() )
   {
      m_variableAddresses[buffer->m_bufferIdx] = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

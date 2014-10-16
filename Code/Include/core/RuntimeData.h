/// @file   core\RuntimeData.h
/// @brief  runtime data buffer
#ifndef _RUNTIME_DATA_H
#define _RUNTIME_DATA_H

#include "core\types.h"
#include "core\Array.h"


///////////////////////////////////////////////////////////////////////////////

class RuntimeDataBuffer;

///////////////////////////////////////////////////////////////////////////////

class IRuntimeVar 
{
public:
   mutable Array< void* >        m_variableAddresses;

protected:
   static unsigned int           s_nextId;

public: 
   virtual ~IRuntimeVar() {}

   /**
    * Checks if the variable is defined in the specified data buffer.
    *
    * @param buffer
    */
   bool isRegistered( RuntimeDataBuffer* buffer ) const;

   /**
    * Registers this variable with the specified data buffer, assigning it an area of memory
    * at the specified address
    *
    * @param buffer
    * @param address
    */
   void registerWithBuffer( RuntimeDataBuffer* buffer, void* address ) const;

   /**
    * Clears the record of the variable being registered with the specified data buffer.
    *
    * @param buffer
    */
   void removeFromBuffer( RuntimeDataBuffer* buffer ) const;
};

///////////////////////////////////////////////////////////////////////////////

template< typename T >
class TRuntimeVar : public IRuntimeVar
{
private:
   unsigned int         m_id;

public:
   TRuntimeVar();

private:

   /**
      * Returns the variable id.
      */
   inline unsigned int getId() const;

   /**
      * Returns the size of the described type.
      */
   inline unsigned int getTypeSize() const;

   friend class RuntimeDataBuffer;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A buffer that stores the values of runtime variables.
 *
 * It's advised to first register all variables, and then initialize and use them.
 */
class RuntimeDataBuffer
{
private:
   struct IndexListNode
   {
      uint                    m_freeIdx;
      IndexListNode*          m_next;

      IndexListNode( uint idx ) : m_freeIdx( idx ), m_next( NULL ) {}
   };

public:
   uint                       m_bufferIdx;

   const ulong                BUFFER_SIZE;

   char*                      m_buffer;
   ulong                      m_endAddress;

private:
   // buffers indexing
   static uint                s_nextGlobalIndex;
   static IndexListNode*      s_freeBufferIndicesHead;
   static IndexListNode*      s_freeBufferIndicesTail;

public:
   /**
    * Constructor.
    */
   RuntimeDataBuffer();
   ~RuntimeDataBuffer();

   /**
    * Registers a new runtime variable.
    *
    * @param var           variable to register
    * @param defaultVal    variable's default value
    */
   template< typename T >
   void registerVar( const typename TRuntimeVar< T >& var, const T& defaultVal = T() );

   /**
    * Unregisters a runtime variable.
    *
    * @param var           variable to register
    */
   template< typename T >
   void unregisterVar( const typename TRuntimeVar< T >& var );

   /**
    * An access operator.
    *
    * @param var     variable data of which we want to access
    */
   template< typename T >
   T& operator[]( const typename TRuntimeVar< T >& var );

   /**
    * An access operator ( const version ).
    *
    * @param var     variable data of which we want to access
    */
   template< typename T >
   const T& operator[]( const typename TRuntimeVar< T >& var ) const;

   /**
    * Resets the pool of buffer indices.
    *
    * CAUTION: Exposed in the public interface for testing purposes only!
    */
   static void resetIndicesPool();

private:
   /**
    * Copy constructor - copying of RuntimeDataBuffers is not allowed.
    */
   RuntimeDataBuffer( const RuntimeDataBuffer& rhs ) : BUFFER_SIZE( 0 ) {}
   void operator=( const RuntimeDataBuffer& rhs ) {}

   // -------------------------------------------------------------------------
   // Buffers indexing
   // -------------------------------------------------------------------------
   /**
    * Allocates and returns the first free buffer index.
    *
    * Released buffer indices will return to the indices pool, which allows
    * to have all active buffers indexed with a continuous range of indices.
    */
   static uint acquireBufferIdx();

   /**
    * Frees a buffer index, returning it to the indices pool.
    *
    * @param idx
    */
   static void freeBufferIdx( uint idx );
};

///////////////////////////////////////////////////////////////////////////////

#include "core\RuntimeData.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _RUNTIME_DATA_H

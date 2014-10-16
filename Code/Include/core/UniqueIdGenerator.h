/// @file   core/UniqueIdGenerator.h
/// @brief utility class for creating unique id's for the specified type
#ifndef _UNIQUE_ID_GENERATOR_H
#define _UNIQUE_ID_GENERATOR_H

#include "core\Stack.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * A small utility class for creating unique id's for the specified type.
 */
template< typename T >
class UniqueIdGenerator
{
   struct Node 
   {
      uint     m_nextFreeId;
      Node*    m_nextNode;

      Node( Node* prevTop, uint val )
      {
         m_nextFreeId = val;
         m_nextNode = prevTop;
      }
   };

private:
   static uint             s_nextId;
   static Node*            s_freeIds;

public:
   ~UniqueIdGenerator();

   /**
    * Returns the next unique index for the class.
    */
   static uint getNextId();

   /**
    * Releases an id back to the pool of unique ids.
    */
   static void releaseId( uint id );
};

///////////////////////////////////////////////////////////////////////////////

#include "core\UniqueIdGenerator.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _UNIQUE_ID_GENERATOR_H

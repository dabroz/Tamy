/// @file   core/CallstackTree.h
/// @brief  a tree structure for storing runtime callstacks
#pragma once

#include "core\DefaultAllocator.h"
#include "core\types.h"


///////////////////////////////////////////////////////////////////////////////

class CriticalSection;

///////////////////////////////////////////////////////////////////////////////

/**
 * A tree structure for storing runtime callstacks.
 */
class CallstackTree
{
public:
   struct Node
   {
      MemoryAllocator*                       m_allocator;
      ulong                                  m_traceId;

      typedef Node*                          PNode;
      PNode*                                 m_children;
      uint                                   m_childrenCount;

      uint*                                  m_callstackIds;
      uint                                   m_callstackIdsCount;

      /**
       * Constructor.
       *
       * @param allocator
       * @param traceId
       */
      Node( MemoryAllocator* allocator, ulong traceId );
      ~Node();

      /**
       * Inserts the specified item into the node and propagates
       * the rest of the items further down the tree,
       *
       * @param callstackId
       * @param callstack
       * @param itemIdx
       * @param size
       */
      void insert( uint callstackId, ulong* callstack, uint itemIdx, uint size );

      /**
       * Checks if node is a part of the specified callstack
       *
       * @param callstackId
       */
      bool isPartOfCallstack( uint callstackId ) const;

      /**
       * Removes the specified callstack from the list, returning true if that was the very last
       * callstack there.
       *
       * @param callstackId
       * @return              'true' if it was the last callstack, 'false' if there are still callstacks left
       */
      bool remove( uint callstackId );
   };

private:
   DefaultAllocator*                      m_allocator;

   CriticalSection*                       m_treeLock;
   Node*                                  m_root;

   uint*                                  m_callstackIds;
   uint                                   m_callstackIdsCount;

public:
   /**
    * Constructor.
    */
   CallstackTree();
   ~CallstackTree();

   /**
    * Inserts a callstack into the tree
    *
    * @param callstackId
    * @param callstack
    * @param size
    */
   void insert( uint callstackId, ulong* callstack, uint size );

   /**
    * Removes a callstack.
    */
   void remove( uint callstackId );

   /**
    * Returns depth of the tree.
    */
   uint getDepth() const;

   /**
    * Returns the number of inserted callstacks.
    */
   uint getCallstacksCount() const;

   /**
    * Returns an id of the specified callstack.
    *
    * @param idx     requested callstack idx
    */
   uint getCallstackId( uint idx ) const;

   /**
    * Returns the specified callstack.
    *
    * @param callstackId
    * @param outCallstack
    * @param maxCallstackSize
    * @return                    callstack size
    */
   uint getCallstack( uint callstackId, ulong* outCallstack, uint maxCallstackSize ) const;

   /**
    * Checks how much memory does the tree consume.
    */
   ulong getMemoryConsumption() const;
};

///////////////////////////////////////////////////////////////////////////////

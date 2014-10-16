#include "core.h" 
#include "core\CallstackTree.h"
#include "core\RawArrayUtil.h"
#include "core\CriticalSection.h"


///////////////////////////////////////////////////////////////////////////////

CallstackTree::CallstackTree()
{
   m_allocator = new DefaultAllocator();
   m_treeLock = new CriticalSection();

   void* nodeMem = m_allocator->alloc( sizeof( Node ) );
   m_root = new ( nodeMem ) Node( m_allocator, -1 );

   m_callstackIds = NULL;
   m_callstackIdsCount = 0;
}

///////////////////////////////////////////////////////////////////////////////

CallstackTree::~CallstackTree()
{
   if ( m_callstackIds )
   {
      m_allocator->dealloc( m_callstackIds );
   }
   m_callstackIds = NULL;

   delete m_allocator;
   m_allocator = NULL;

   delete m_treeLock;
   m_treeLock = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void CallstackTree::insert( uint callstackId, ulong* callstack, uint size )
{
   m_treeLock->enter();

   // we allow to insert many callstacks with the same id
   m_root->insert( callstackId, callstack, 0, size );

   RawArrayUtil::push_back( m_allocator, m_callstackIds, m_callstackIdsCount, callstackId );

   m_treeLock->leave();
}

///////////////////////////////////////////////////////////////////////////////

void CallstackTree::remove( uint callstackId )
{
   m_treeLock->enter();

   // check if the callstack was recorded
   bool wasRecorded = false;
   for ( long i = m_callstackIdsCount - 1; i >= 0; --i )
   {
      if ( m_callstackIds[i] == callstackId )
      {
         wasRecorded = true;
         RawArrayUtil::remove( m_allocator, m_callstackIds, m_callstackIdsCount, i );
         break;
      }
   }

   if ( !wasRecorded )
   {
      m_treeLock->leave();
      return;
   }

   // remove callstack from the tree
   Node* nextNode = m_root;
   while( nextNode != NULL )
   {
      Node* analyzedNode = nextNode;
      nextNode = NULL;

      // find a child that corresponds to this callstack id
      uint count = analyzedNode->m_childrenCount;
      for ( uint i = 0; i < count; ++i )
      {
         Node* child = analyzedNode->m_children[i];
         if ( child->isPartOfCallstack( callstackId ) )
         {
            // remove the callstack 
            bool wasLastCallstack = child->remove( callstackId );

            if ( wasLastCallstack )
            {
               // and if it was the last one - remove the node altogether, thus removing the whole branch
               child->~Node();
               m_allocator->dealloc( child );
               RawArrayUtil::remove( m_allocator, analyzedNode->m_children, analyzedNode->m_childrenCount, i );
               nextNode = NULL;
            }
            else
            {
               // go to the next node
               nextNode = child;
            }
            break;
         }
      }
   }

   m_treeLock->leave();
}

///////////////////////////////////////////////////////////////////////////////

struct DFSItem
{
   int  m_currDepth;
   CallstackTree::Node* m_node;

   void set( int currDepth, CallstackTree::Node* node )
   {
      m_currDepth = currDepth;
      m_node = node;
   }
};

uint CallstackTree::getDepth() const
{
   int maxDepth = 0;

   uint stackPtr = 1;
   DFSItem nodesStack[128];
   nodesStack[0].set( 0, m_root );

   while( stackPtr > 0 )
   {
      --stackPtr;
      const DFSItem& item = nodesStack[stackPtr];

      if ( maxDepth < item.m_currDepth )
      {
         maxDepth = item.m_currDepth;
      }

      // scan the children
      uint newDepth = item.m_currDepth + 1;

      const Node::PNode* children = item.m_node->m_children;
      uint childrenCount = item.m_node->m_childrenCount;
      for ( uint i = 0; i < childrenCount; ++i )
      {
         nodesStack[stackPtr].set( newDepth, children[i] );
         ++stackPtr;
      }
   }

   return maxDepth;
}

///////////////////////////////////////////////////////////////////////////////

// @MainThread
uint CallstackTree::getCallstacksCount() const
{
   m_treeLock->enter();
   uint val = m_callstackIdsCount;
   m_treeLock->leave();

   return val;
}

///////////////////////////////////////////////////////////////////////////////

// @MainThread
uint CallstackTree::getCallstackId( uint idx ) const
{
   return m_callstackIds[idx];
}

///////////////////////////////////////////////////////////////////////////////

// @MainThread
uint CallstackTree::getCallstack( uint callstackId, ulong* outCallstack, uint maxCallstackSize ) const
{
   const Node* nextNode = m_root;
   uint traceIdx = 0;

   while( nextNode != NULL && traceIdx < maxCallstackSize )
   {
      const Node* analyzedNode = nextNode;
      nextNode = NULL;

      uint count = analyzedNode->m_childrenCount;
      for ( uint i = 0; i < count; ++i )
      {
         Node* child = analyzedNode->m_children[i];

         if ( child->isPartOfCallstack( callstackId ) )
         {
            outCallstack[traceIdx] = child->m_traceId;
            ++traceIdx;

            nextNode = child;
            break;
         }
      }
   }

   return traceIdx;
}

///////////////////////////////////////////////////////////////////////////////

// @MainThread
ulong CallstackTree::getMemoryConsumption() const
{
   return m_allocator->getMemoryUsed();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

CallstackTree::Node::Node( MemoryAllocator* allocator, ulong traceId )
   : m_allocator( allocator )
   , m_children( NULL )
   , m_childrenCount( 0 )
   , m_callstackIds( NULL )
   , m_callstackIdsCount( 0 )
   , m_traceId( traceId )
{
}

///////////////////////////////////////////////////////////////////////////////

CallstackTree::Node::~Node()
{
   uint count = m_childrenCount;
   for ( uint i = 0; i < count; ++i )
   {
      m_children[i]->~Node();
      m_allocator->dealloc( m_children[i] );
   }

   if ( m_children )
   {
      m_allocator->dealloc( m_children );
   }
   m_children = NULL;

   m_allocator = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void CallstackTree::Node::insert( uint callstackId, ulong* callstack, uint itemIdx, uint size )
{
   ulong traceId = callstack[itemIdx];

   Node* childNode = NULL;
   uint count = m_childrenCount;
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_children[i]->m_traceId == traceId )
      {
         childNode = m_children[i];
         break;
      }
   }

   if ( !childNode )
   {
      void* nodeMem = m_allocator->alloc( sizeof( Node ) );
      childNode = new ( nodeMem ) Node( m_allocator, traceId );

      // add the new trace id
      RawArrayUtil::push_back( m_allocator, m_children, m_childrenCount, childNode );
   }

   // mark that this node is a part of the inserted callstack
   RawArrayUtil::push_back( m_allocator, childNode->m_callstackIds, childNode->m_callstackIdsCount, callstackId );

   // propagate the remaining items further down the tree
   ++itemIdx;
   if ( itemIdx < size )
   {
      childNode->insert( callstackId, callstack, itemIdx, size );
   }
}

///////////////////////////////////////////////////////////////////////////////

bool CallstackTree::Node::isPartOfCallstack( uint callstackId ) const
{
   for ( uint i = 0; i < m_callstackIdsCount; ++i )
   {
      if ( m_callstackIds[i] == callstackId )
      {
         return true;
      }
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////

bool CallstackTree::Node::remove( uint callstackId )
{
   for ( uint i = 0; i < m_callstackIdsCount; ++i )
   {
      if ( m_callstackIds[i] == callstackId )
      {
         RawArrayUtil::remove( m_allocator, m_callstackIds, m_callstackIdsCount, i );
         break;
      }
   }

   return m_callstackIdsCount == 0;
}

///////////////////////////////////////////////////////////////////////////////

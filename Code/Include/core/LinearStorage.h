/// @file   core\LinearStorage.h
/// @brief  spatial storage that keeps its elements in an array
#ifndef _LINEAR_STORAGE_H
#define _LINEAR_STORAGE_H

#include "core\Array.h"
#include "core\AxisAlignedBox.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * This is the simplest spatial storage - one that does not have
 * any spatial hierarchy - it simply uses a linear array as an underlying
 * storage.
 * Use when you're dealing with a small set of objects, in which case
 * using any other spatial container would be an overkill.
 */
template<typename T>
class LinearStorage
{
   DECLARE_ALLOCATOR( LinearStorage, AM_DEFAULT );

private:
   Array< T* >*      m_container;
   AxisAlignedBox     m_sceneBounds;

public:
   LinearStorage();
   ~LinearStorage();

   /**
    * Checks if an element is stored in the storage.
    *
    * @param elem
    */
   bool isAdded( const T& elem ) const;

   /**
    * Inserts an element into the storage.
    *
    * @param elem
    */
   void insert( T& elem );

   /**
    * Removes an element into the storage.
    *
    * @param elem
    */
   void remove( T& elem );

   /**
    * Clears the storage of all elements.
    */
   void clear();

   /**
    * Queries the storage for elements located in the specified area.
    *
    * @param sceneArea
    * @param output
    */
   template< typename AreaTestCallback >
   void query( const AreaTestCallback& sceneArea, Array<T*>& output ) const;

   /**
    * Returns a bounding box around all elements.
    *
    * @param outBounds
    */
   void getSceneBounds( AxisAlignedBox& outBounds ) const;

   /**
    * Returns the number of elements in the storage;
    */
   unsigned int size() const;

   /**
    * Recalculates scene boundaries.
    */
   void recalcSceneBounds();
};

///////////////////////////////////////////////////////////////////////////////

#include "core\LinearStorage.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _LINEAR_STORAGE_H

#ifndef _LINEAR_STORAGE_H
#error "This file can only be included from LinearStorage.h"
#else

#include "core/ThreadSystem.h"


///////////////////////////////////////////////////////////////////////////////

template<typename T>
LinearStorage<T>::LinearStorage() 
{
   m_container = new Array<T*>();
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
LinearStorage<T>::~LinearStorage()
{
   delete m_container;
   m_container = NULL;
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
bool LinearStorage<T>::isAdded( const T& elem ) const
{
   uint idx = m_container->find( &elem );
   return idx != EOA;
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
void LinearStorage<T>::insert( T& elem )
{
   m_container->push_back( &elem );

   const AxisAlignedBox& bb = elem.getBoundingVolume();
   m_sceneBounds.add( bb, m_sceneBounds );
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
void LinearStorage<T>::remove( T& elem )
{
   unsigned int index = m_container->find( &elem );
   if ( index != EOA )
   {
      m_container->remove(index);
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T > template< typename AreaTestCallback >
void LinearStorage<T>::query(const AreaTestCallback& sceneArea, Array<T*>& output) const
{
   unsigned int elemsCount = m_container->size();
   T* elem = NULL;
   for (unsigned int i = 0; i < elemsCount; ++i)
   {
      elem = (*m_container)[i];
      const AxisAlignedBox& elemBounds = elem->getBoundingVolume();
      if ( sceneArea.testIntersection( elemBounds ) == true )
      {
         output.push_back(elem);
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
void LinearStorage<T>::getSceneBounds( AxisAlignedBox& outBounds ) const
{
   outBounds = m_sceneBounds;
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
void LinearStorage<T>::clear()
{
   m_container->clear();
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
unsigned int LinearStorage<T>::size() const
{
   return m_container->size();
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
void LinearStorage<T>::recalcSceneBounds()
{
   m_sceneBounds.reset();
   
   uint elemsCount = m_container->size();
   for (unsigned int i = 0; i < elemsCount; ++i)
   {
      T* elem = (*m_container)[i];

      const AxisAlignedBox& bb = elem->getBoundingVolume();
      m_sceneBounds.add( bb, m_sceneBounds );
   }
}

///////////////////////////////////////////////////////////////////////////////

#endif // _LINEAR_STORAGE_H

#ifndef _FAST_RELEASE_ARRAY
#error "This file can only be included from ConstSizeArray.h"
#else

#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

template<typename T>
ConstSizeArray<T>::ConstSizeArray(unsigned int size = 1)
: m_elements(new Array<T>(size))
, m_isFree(new Array<bool>(size))
, m_freePos(new Stack<unsigned int>())
{
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
ConstSizeArray<T>::ConstSizeArray(const ConstSizeArray& rhs)
: m_elements(new Array<T>(*(rhs.m_elements)))
, m_isFree(new Array<bool>(*(rhs.m_isFree)))
, m_freePos(new Stack<unsigned int>(*(rhs.m_freePos)))
{
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
ConstSizeArray<T>::~ConstSizeArray()
{
   delete m_elements; m_elements = NULL;
   delete m_isFree; m_isFree = NULL;
   delete m_freePos; m_freePos = NULL;
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
void ConstSizeArray<T>::clear()
{
   unsigned int count = m_isFree->size();
   for (unsigned int i = 0; i < count; ++i)
   {
      if ((*m_isFree)[i])
      {
         m_freePos->push(i);
         (*m_isFree)[i] = true;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
unsigned int ConstSizeArray<T>::size() const 
{
   return m_elements->size();
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
void ConstSizeArray<T>::allocate(unsigned int newSize)
{
   m_elements->allocate(newSize);
   m_isFree->allocate(newSize);
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
void ConstSizeArray<T>::resize(unsigned int newSize, const T& defaultValue)
{
   m_elements->resize(newSize, defaultValue);
   m_isFree->resize(newSize, false);
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
unsigned int ConstSizeArray<T>::insert(const T& elem)
{
   unsigned int freePosIdx = 0;
   if (m_freePos->empty() == false)
   {
      freePosIdx = m_freePos->pop();
      (*m_isFree)[freePosIdx] = false;

      (*m_elements)[freePosIdx] = elem;
   }
   else
   {
      freePosIdx = m_elements->size();
      m_elements->push_back(elem);
      m_isFree->push_back(false);
   }

   return freePosIdx;
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
void ConstSizeArray<T>::remove(unsigned int idx)
{
   ASSERT_MSG(idx < m_elements->size(), "index out of array boundaries"); 

   if ((*m_isFree)[idx] == true) {return;}

   m_freePos->push(idx);
   (*m_isFree)[idx] = true;
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
T& ConstSizeArray<T>::at(unsigned int idx)
{
   ASSERT_MSG(idx < m_elements->size(),  "index out of array boundaries");
   if ((*m_isFree)[idx] == true)
   {
      ASSERT_MSG( false, "This element has been removed" );
   }
   return (*m_elements)[idx];
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
const T& ConstSizeArray<T>::at(unsigned int idx) const
{
   ASSERT_MSG(idx < m_elements->size(), "index out of array boundaries"); 
   if ((*m_isFree)[idx] == true)
   {
      ASSERT_MSG( false, "This element has been removed");
   }
   return (*m_elements)[idx];
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
T& ConstSizeArray<T>::operator[](unsigned int idx)
{
   ASSERT_MSG(idx < m_elements->size(),  "index out of array boundaries"); 
   if ((*m_isFree)[idx] == true)
   {
      ASSERT_MSG( false, "This element has been removed");
   }
   return (*m_elements)[idx];
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
const T& ConstSizeArray<T>::operator[](unsigned int idx) const
{
   ASSERT_MSG(idx < m_elements->size(), "index out of array boundaries");
   if ((*m_isFree)[idx] == true)
   {
      ASSERT_MSG( false, "This element has been removed");
   }
   return (*m_elements)[idx];
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
unsigned int ConstSizeArray<T>::find(const T& elem, unsigned int startPos) const
{
   unsigned int count = m_elements->size();
   if (startPos >= count)
   {
      return EOA;
   }

   for (unsigned int i = startPos; i < count; ++i)
   {
      if (((*m_isFree)[i] == false) && ((*m_elements)[i] == elem))
      {
         return i;
      }
   }

   return EOA;
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
typename ConstSizeArray<T>::iterator ConstSizeArray<T>::begin() const
{
   unsigned int startIdx = 0;
   while ((startIdx < m_elements->size()) && ((*m_isFree)[startIdx] == true))
   {
      ++startIdx;
   }
   return iterator(*m_elements, *m_isFree, startIdx);
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
typename ConstSizeArray<T>::iterator ConstSizeArray<T>::end() const
{
   return iterator(*m_elements, *m_isFree, m_elements->size());
}

///////////////////////////////////////////////////////////////////////////////
// iterator implementation
///////////////////////////////////////////////////////////////////////////////

template<typename T>
ConstSizeArray<T>::iterator::iterator(const Array<T>& elements,
                                      const Array<bool>& isFree,
                                      unsigned int idx)
: m_elements(&elements)
, m_isFree(&isFree)
, m_idx(idx)
{
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
ConstSizeArray<T>::iterator::iterator(const iterator& rhs)
: m_elements(rhs.m_elements)
, m_isFree(rhs.m_isFree)
, m_idx(rhs.m_idx)
{
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
bool ConstSizeArray<T>::iterator::operator==(const iterator& rhs) const
{
   return ((m_elements == rhs.m_elements) && (m_idx == rhs.m_idx));
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
bool ConstSizeArray<T>::iterator::operator!=(const iterator& rhs) const
{
   return !(*this == rhs);
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
typename ConstSizeArray<T>::iterator& 
ConstSizeArray<T>::iterator::operator=(const iterator& rhs)
{
   m_elements = rhs.m_elements;
   m_isFree = rhs.m_isFree;
   m_idx = rhs.m_idx;
   return *this;
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
typename ConstSizeArray<T>::iterator& 
ConstSizeArray<T>::iterator::operator++()
{
   m_idx = getNext();
   return *this;
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
typename ConstSizeArray<T>::iterator
ConstSizeArray<T>::iterator::operator++(int)
{
   iterator newIter(*this);
   m_idx = getNext();
   return newIter;
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
typename ConstSizeArray<T>::iterator& 
ConstSizeArray<T>::iterator::operator--()
{
   m_idx = getPrev();
   return *this;
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
typename ConstSizeArray<T>::iterator
ConstSizeArray<T>::iterator::operator--(int)
{
   iterator newIter(*this);
   m_idx = getPrev();
   return newIter;
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
const T& ConstSizeArray<T>::iterator::operator*() const
{
   if (m_idx >= m_elements->size())
   {
      ASSERT_MSG( false, "The end of array reached");
   }
   return (*m_elements)[m_idx];
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
unsigned int ConstSizeArray<T>::iterator::getNext() const
{
   unsigned int newIdx = m_idx + 1;
   while ((newIdx < m_elements->size()) && ((*m_isFree)[newIdx] == true))
   {
      ++newIdx;
   }

   if (newIdx >= m_elements->size())
   {
      newIdx = m_elements->size();
   }

   return newIdx;
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
unsigned int ConstSizeArray<T>::iterator::getPrev() const
{
   long newIdx = m_idx - 1;
   while ((newIdx >= 0) && ((*m_isFree)[newIdx] == true))
   {
      --newIdx;
   }

   if (newIdx < 0)
   {
      newIdx = m_idx;
   }

   return newIdx;
}

///////////////////////////////////////////////////////////////////////////////

#endif // _FAST_RELEASE_ARRAY

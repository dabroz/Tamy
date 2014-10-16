/// @file   core\UniqueObject.h
/// @brief  class each instance of which has a unique id assigned
#ifndef _UNIQUE_OBJECT_H
#define _UNIQUE_OBJECT_H


///////////////////////////////////////////////////////////////////////////////

/**
 * Each instance of this class will have a unique ID assigned - and the genericness
 * assures that objects that don't share the same hierarchy will not
 * share unique numbers ( pretty useful when you want
 * to use that as the indices into an array )
 */
template< typename T >
class UniqueObject
{
private:
   static unsigned int m_nextIndex;

   unsigned int m_index;

public:
   virtual ~UniqueObject() {}

   /** 
    * Returns the unique id assigned to each instance.
    *
    * @return     unique object id
    */
   unsigned int getIndex() const { return m_index; }
   bool operator<( const UniqueObject& rhs ) const { return m_index < rhs.m_index; }

   /** 
    * Operator returns true if both objects have the same id.
    */
   bool operator==( const UniqueObject& rhs ) const { return m_index == rhs.m_index; }
   bool operator!=( const UniqueObject& rhs ) const { return m_index != rhs.m_index; }

protected:
   UniqueObject();
   UniqueObject(const UniqueObject& rhs);
};

///////////////////////////////////////////////////////////////////////////////

#include "core\UniqueObject.inl"

///////////////////////////////////////////////////////////////////////////////

#endif _UNIQUE_OBJECT_H

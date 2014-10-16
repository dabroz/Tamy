/// @file   core/LocalList.h
/// @brief  a list that allocates memory in the memory pool dedicated to the running thread
#ifndef _LOCAL_LIST_H
#define _LOCAL_LIST_H

#include "core/List.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * A list that allocates memory in the memory pool dedicated to the running thread.
 *
 * That's why it's perfect for all sorts of algorithms that operate in a single frame
 * of execution only.
 * Don't use it to store data between subsequent frames of execution, as it will 
 * cause the thread memory pool to overflow ( so using it as a class member would be a bad idea ).
 */
template< typename T >
class LocalList : public List< T >
{
   DECLARE_ALLOCATOR( LocalList, AM_DEFAULT );

public:
   /**
    * Constructor.
    */
   LocalList();

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   LocalList( const LocalList& rhs );
};

///////////////////////////////////////////////////////////////////////////////

#include "core/LocalList.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _LOCAL_LIST_H

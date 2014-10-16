/// @file   core/LocalStack.h
/// @brief  a stack that allocates memory in the memory pool dedicated to the running thread
#ifndef _LOCAL_STACK_H
#define _LOCAL_STACK_H

#include "core/Stack.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * A stack that allocates memory in the memory pool dedicated to the running thread.
 *
 * That's why it's perfect for all sorts of algorithms that operate in a single frame
 * of execution only.
 * Don't use it to store data between subsequent frames of execution, as it will 
 * cause the thread memory pool to overflow ( so using it as a class member would be a bad idea ).
 */
template< typename T >
class LocalStack : public Stack< T >
{
   DECLARE_ALLOCATOR( LocalStack, AM_DEFAULT );

public:
   /**
    * Constructor.
    */
   LocalStack();
};

///////////////////////////////////////////////////////////////////////////////

#include "core/LocalStack.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _LOCAL_STACK_H

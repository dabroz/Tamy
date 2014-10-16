#pragma once

#include "core\MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * An interface all objects that can be updated can implement.
 */
class TimeDependent
{
public:
   virtual ~TimeDependent() {}

   /**
    * Updates the object's state.
    *
    * @param timeElapsed      time elapsed since the last time this method was called.
    */
   virtual void update( float timeElapsed ) = 0;
};

///////////////////////////////////////////////////////////////////////////////


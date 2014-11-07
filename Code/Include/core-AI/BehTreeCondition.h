/// @file   core-AI\BehTreeCondition.h
/// @brief  a behavior tree condition
#pragma once

#include "core\ReflectionObject.h"


///////////////////////////////////////////////////////////////////////////////

class BehaviorTreeRunner;
class BehTreeSelector;

///////////////////////////////////////////////////////////////////////////////

class BehTreeCondition : public ReflectionObject
{
   DECLARE_ALLOCATOR( BehTreeCondition, AM_DEFAULT );
   DECLARE_CLASS();

public:
   virtual ~BehTreeCondition() {}

   /**
    * Evaluates the condition.
    *
    * The convention is to be able to use selectors as 'if' and 'if-else' clauses, so make sure
    * to evaluate the conditions accordingly: 
    *   - 'true' value should be returned with idx 0
    *   - 'false value should be returned with idx 1
    *   - other values ( multiple selection node ) should be assigned remaining positive indices
    *   - no value ( no selection applicable ) should be returned with idx - 1
    *
    * @param   runner
    * @param   hostSelector
    * @return  index the selector should use ( -1 means that no node should be selected ).
    */
   virtual int evaluate( BehaviorTreeRunner& runner, const BehTreeSelector& hostSelector ) const {
      return -1;
   }
};

///////////////////////////////////////////////////////////////////////////////

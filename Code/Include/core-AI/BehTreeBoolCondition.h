/// @file   core-AI\BehTreeBoolCondition.h
/// @brief  a condition that depends on the value of the specified boolean variable
#pragma once

#include "core-AI\BehTreeCondition.h"


///////////////////////////////////////////////////////////////////////////////

class BehTreeVarBool;

///////////////////////////////////////////////////////////////////////////////

class BehTreeBoolCondition : public BehTreeCondition
{
   DECLARE_ALLOCATOR( BehTreeBoolCondition, AM_DEFAULT );
   DECLARE_CLASS();

private:
   BehTreeVarBool*         m_variable;

public:
   /**
    * Constructor.
    */
   BehTreeBoolCondition();
   ~BehTreeBoolCondition();

   // -------------------------------------------------------------------------
   // BehTreeAction implementation
   // -------------------------------------------------------------------------
   int evaluate( BehaviorTreeRunner& runner, const BehTreeSelector& hostSelector ) const override;
};

///////////////////////////////////////////////////////////////////////////////

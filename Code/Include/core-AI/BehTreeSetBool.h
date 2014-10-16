/// @file   core-AI\BehTreeSetBool.h
/// @brief  sets the value of a boolean variable
#pragma once

#include "core-AI\BehTreeAction.h"


///////////////////////////////////////////////////////////////////////////////

class BehTreeVarBool;

///////////////////////////////////////////////////////////////////////////////

class BehTreeSetBool : public BehTreeAction
{
   DECLARE_ALLOCATOR( BehTreeSetBool, AM_DEFAULT );
   DECLARE_CLASS();

private:
   BehTreeVarBool*               m_variable;
   bool                          m_value;

public:
   /**
    * Constructor.
    */
   BehTreeSetBool();
   ~BehTreeSetBool();

   // -------------------------------------------------------------------------
   // BehTreeAction implementation
   // -------------------------------------------------------------------------
   Result execute( BehaviorTreeRunner& runner ) const;
};

///////////////////////////////////////////////////////////////////////////////

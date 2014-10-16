/// @file   core-AI/BTTTVariable.h
/// @brief  a variable-dependent transition trigger
#pragma once

#include "core-AI\BlendTreeTransitionTrigger.h"
#include "core\ReflectionObjectChangeListener.h"


///////////////////////////////////////////////////////////////////////////////

class BlendTreeVariable;

///////////////////////////////////////////////////////////////////////////////

enum ComparisonOp
{
   CO_Equal,
   CO_NotEqual,
   CO_Less,
   CO_LessEqual,
   CO_Greater,
   CO_GreaterEqual,
};

///////////////////////////////////////////////////////////////////////////////

class BTTTVariable : public BlendTreeTransitionTrigger, public ReflectionObjectChangeListener
{
   DECLARE_ALLOCATOR( BTTTVariable, AM_DEFAULT );
   DECLARE_CLASS();

private:
   BlendTreeVariable*            m_testedValue;
   BlendTreeVariable*            m_referenceValue;
   ComparisonOp                  m_comparisonOp;

public:
   /**
    * Constructor.
    *
    * @param testedValue
    * @param desiredValue
    * @param comparisonOp
    */
   BTTTVariable( BlendTreeVariable* testedValue = NULL, BlendTreeVariable* desiredValue = NULL, ComparisonOp comparisonOp = CO_Equal );
   ~BTTTVariable();

   // -------------------------------------------------------------------------
   // BlendTreeTransitionTrigger implementation
   // -------------------------------------------------------------------------
   bool testActivation( BlendTreePlayer* player ) const;
};

///////////////////////////////////////////////////////////////////////////////

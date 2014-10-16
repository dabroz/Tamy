#include "core-AI\BehTreeBoolCondition.h"
#include "core-AI\BehaviorTreeRunner.h"
#include "core-AI\BehTreeVariable.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BehTreeBoolCondition );
   PARENT( BehTreeCondition );
   PROPERTY_EDIT_REFERENCE( "Variable", BehTreeVarBool*, m_variable );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BehTreeBoolCondition::BehTreeBoolCondition()
   : m_variable( NULL )
{
   registerReferences();
}

///////////////////////////////////////////////////////////////////////////////

BehTreeBoolCondition::~BehTreeBoolCondition()
{
}

///////////////////////////////////////////////////////////////////////////////

int BehTreeBoolCondition::evaluate( BehaviorTreeRunner& runner ) const
{
   if ( m_variable )
   {
      const bool val = m_variable->getRuntime( &runner );
      return val ? 0 : 1;
   }
   else
   {
      return -1;
   }
}

///////////////////////////////////////////////////////////////////////////////
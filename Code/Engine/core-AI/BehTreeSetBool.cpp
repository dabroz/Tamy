#include "core-AI\BehTreeSetBool.h"
#include "core-AI\BehaviorTreeRunner.h"
#include "core-AI\BehTreeVariable.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BehTreeSetBool );
   PARENT( BehTreeAction );
   PROPERTY_EDIT_REFERENCE( "Variable", BehTreeVarBool*, m_variable );
   PROPERTY_EDIT( "Value", bool, m_value );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BehTreeSetBool::BehTreeSetBool()
   : m_variable( NULL )
   , m_value( false )
{
   registerReferences();
}

///////////////////////////////////////////////////////////////////////////////

BehTreeSetBool::~BehTreeSetBool()
{
}

///////////////////////////////////////////////////////////////////////////////

BehTreeNode::Result BehTreeSetBool::execute( BehaviorTreeRunner& runner ) const
{
   if ( m_variable )
   {
      m_variable->setRuntime( &runner, m_value );
   }

   return BehTreeNode::FINISHED;
}

///////////////////////////////////////////////////////////////////////////////

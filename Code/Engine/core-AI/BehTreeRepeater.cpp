#include "core-AI\BehTreeRepeater.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BehTreeRepeater );
   PARENT( BehTreeDecorator );
   PROPERTY_EDIT( "Break on failure", bool, m_breakOnFailure );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BehTreeRepeater::BehTreeRepeater( bool breakOnFailure )
   : m_breakOnFailure( breakOnFailure )
{
}

///////////////////////////////////////////////////////////////////////////////

BehTreeNode::Result BehTreeRepeater::execute( BehaviorTreeRunner& runner ) const
{
   if ( !m_childNode )
   {
      return FINISHED;
   }

   BehTreeNode::Result result = m_childNode->execute( runner );
   if ( result != IN_PROGRESS )
   {
      // reinitialize the node
      m_childNode->deinitialize( runner );
      m_childNode->initialize( runner );
   }

   if ( result == FAILED && m_breakOnFailure )
   {
      return FAILED;
   }

   return IN_PROGRESS;
}

///////////////////////////////////////////////////////////////////////////////

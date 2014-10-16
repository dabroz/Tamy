#include "core-AI\BTSSVariable.h"
#include "core-AI\BlendTreeVariable.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BTSSVariable );
   PARENT( BlendTreeSelectorSwitch );
   PROPERTY_EDIT_REFERENCE( "Node idx", BTVarInt*, m_desiredNodeIdx );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BTSSVariable::BTSSVariable( BTVarInt* desiredNodeIdx )
   : m_desiredNodeIdx( desiredNodeIdx )
{
   registerReferences();
}

///////////////////////////////////////////////////////////////////////////////

BTSSVariable::~BTSSVariable()
{
   // don't erase the pointer to m_desiredNodeIdx, or it won't be unregistered properly by the reference tracking mechanism
}

///////////////////////////////////////////////////////////////////////////////

int BTSSVariable::getNodeIdx( BlendTreePlayer* player ) const
{
   if ( m_desiredNodeIdx )
   {
      int nodeIdx = m_desiredNodeIdx->getRuntime( player );
      return nodeIdx;
   }
   else
   {
      return -1;
   }
}

///////////////////////////////////////////////////////////////////////////////

/// @file   core-AI/BTSSVariable.h
/// @brief  a BlendTreeSelectorSwitch customized by a specified variable value
#pragma once

#include "core-AI\BlendTreeSelectorSwitch.h"


///////////////////////////////////////////////////////////////////////////////

class BTVarInt;

///////////////////////////////////////////////////////////////////////////////

/**
 * A BlendTreeSelectorSwitch customized by a specified variable value.
 */
class BTSSVariable : public BlendTreeSelectorSwitch
{
   DECLARE_ALLOCATOR( BTSSVariable, AM_DEFAULT );
   DECLARE_CLASS();

private:
   BTVarInt*         m_desiredNodeIdx;

public:
   /**
    * Constructor.
    *
    * @param desiredNodeIdx
    */
   BTSSVariable( BTVarInt* desiredNodeIdx = NULL );
   ~BTSSVariable();

   // -------------------------------------------------------------------------
   // BlendTreeSelectorSwitch implementation
   // -------------------------------------------------------------------------
   int getNodeIdx( BlendTreePlayer* player ) const;
};

///////////////////////////////////////////////////////////////////////////////

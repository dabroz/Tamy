/// @file   ext-StoryTeller\BTAPursue.h
/// @brief  Pursue steering behavior velocity calculator
#pragma once

#include "core-AI\BehTreeAction.h"
#include "core\RuntimeData.h"


///////////////////////////////////////////////////////////////////////////////

class BehTreeVarVector;
class StoryActor;

///////////////////////////////////////////////////////////////////////////////

class BTAPursue : public BehTreeAction
{
   DECLARE_ALLOCATOR( BTAPursue, AM_DEFAULT );
   DECLARE_CLASS();

private:
   float                                     m_arrivalRadius;
   float                                     m_topSpeed;
   BehTreeVarVector*                         m_targetPos;
   BehTreeVarVector*                         m_targetVelocity;
   BehTreeVarVector*                         m_outVelocity;

public:
   /**
    * Constructor.
    */
   BTAPursue();
   ~BTAPursue();

   // -------------------------------------------------------------------------
   // BehTreeAction implementation
   // -------------------------------------------------------------------------
   Result execute( BehaviorTreeRunner& runner ) const;
};

///////////////////////////////////////////////////////////////////////////////

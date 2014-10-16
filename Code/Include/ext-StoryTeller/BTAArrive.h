/// @file   ext-StoryTeller\BTAArrive.h
/// @brief  Arrival steering behavior velocity calculator
#pragma once

#include "core-AI\BehTreeAction.h"
#include "core\RuntimeData.h"


///////////////////////////////////////////////////////////////////////////////

class BehTreeVarVector;
class StoryActor;

///////////////////////////////////////////////////////////////////////////////

class BTAArrive : public BehTreeAction
{
   DECLARE_ALLOCATOR( BTAArrive, AM_DEFAULT );
   DECLARE_CLASS();

private:
   float                                     m_arrivalRadius;
   float                                     m_topSpeed;
   BehTreeVarVector*                         m_targetPos;
   BehTreeVarVector*                         m_outVelocity;

public:
   /**
   * Constructor.
   */
   BTAArrive();
   ~BTAArrive();

   // -------------------------------------------------------------------------
   // BehTreeAction implementation
   // -------------------------------------------------------------------------
   Result execute( BehaviorTreeRunner& runner ) const;
};

///////////////////////////////////////////////////////////////////////////////

/// @file   ext-StoryTeller\BTASlideTo.h
/// @brief  makes the actor slide to the specified point
#pragma once

#include "core-AI\BehTreeAction.h"
#include "core\RuntimeData.h"
#include "core\Vector.h"


///////////////////////////////////////////////////////////////////////////////

class StoryNodeInstance;
class BehTreeVarVector;

///////////////////////////////////////////////////////////////////////////////

/**
* This action will tell the actor to move to the specified location.
*/
class BTASlideTo : public BehTreeAction
{
   DECLARE_ALLOCATOR( BTASlideTo, AM_DEFAULT );
   DECLARE_CLASS();

private:
   BehTreeVarVector*                         m_destination;
   float                                     m_slideDuration;

   // runtime data
   TRuntimeVar< Vector >                     m_slideVec;
   TRuntimeVar< float >                      m_remainingDuration;

public:
   /**
   * Constructor.
   */
   BTASlideTo();
   ~BTASlideTo();

   // -------------------------------------------------------------------------
   // BehTreeAction implementation
   // -------------------------------------------------------------------------
   void createLayout( BehaviorTreeRunner& runner ) const;
   void initialize( BehaviorTreeRunner& runner ) const;
   void deinitialize( BehaviorTreeRunner& runner ) const;
   Result execute( BehaviorTreeRunner& runner ) const;
};

///////////////////////////////////////////////////////////////////////////////

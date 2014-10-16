/// @file   ext-StoryTeller/BTAInputToVelocity.h
/// @brief  this action transforms user input to velocity that can be used to propel things
#pragma once

#include "core-AI\BehTreeAction.h"
#include "core\RuntimeData.h"


///////////////////////////////////////////////////////////////////////////////

class StoryNodeInstance;
class PlayerSceneQuery;
class BehTreeVarVector;
class BehTreeVarFloat;

///////////////////////////////////////////////////////////////////////////////

/**
 * This action handles user's input.
 */
class BTAInputToVelocity : public BehTreeAction
{
   DECLARE_ALLOCATOR( BTAInputToVelocity, AM_DEFAULT );
   DECLARE_CLASS();

private:
   BehTreeVarVector*                m_velocity;
   BehTreeVarFloat*                 m_rotation;
   BehTreeVarFloat*                 m_velocityScale;
   BehTreeVarFloat*                 m_angularSpeed;

   TRuntimeVar< float >             m_yaw;

public:
   /**
    * Constructor.
    */
   BTAInputToVelocity();
   ~BTAInputToVelocity();

   // -------------------------------------------------------------------------
   // BehTreeAction implementation
   // -------------------------------------------------------------------------
   void createLayout( BehaviorTreeRunner& runner ) const;
   void initialize( BehaviorTreeRunner& runner ) const;
   Result execute( BehaviorTreeRunner& runner ) const;
};

///////////////////////////////////////////////////////////////////////////////

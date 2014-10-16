/// @file   ext-StoryTeller/BTAMoveWithVelocity.h
/// @brief  Moves an actor with the specified velocity
#pragma once

#include "core-AI\BehTreeAction.h"
#include "core\RuntimeData.h"


///////////////////////////////////////////////////////////////////////////////

class StoryNodeInstance;
class BehTreeVarVector;
class BehTreeVarFloat;
class PhysicsCharacterController;

///////////////////////////////////////////////////////////////////////////////

/**
 * This action will tell the actor to move to the specified location.
 */
class BTAMoveWithVelocity : public BehTreeAction
{
   DECLARE_ALLOCATOR( BTAMoveWithVelocity, AM_DEFAULT );
   DECLARE_CLASS();

private:
   BehTreeVarVector*                                        m_velocity;
   BehTreeVarFloat*                                         m_characterRotation;

   // runtime data
   TRuntimeVar< PhysicsCharacterController* >               m_characterController;

public:
   /**
    * Constructor.
    */
   BTAMoveWithVelocity();
   ~BTAMoveWithVelocity();

   // -------------------------------------------------------------------------
   // BehTreeAction implementation
   // -------------------------------------------------------------------------
   void createLayout( BehaviorTreeRunner& runner ) const;
   void initialize( BehaviorTreeRunner& runner ) const;
   void deinitialize( BehaviorTreeRunner& runner ) const;
   Result execute( BehaviorTreeRunner& runner ) const;
};

///////////////////////////////////////////////////////////////////////////////

/// @file   ext-StoryTeller\BTAAnimationToPhysics.h
/// @brief  action that relays animation induced movement to the physical character controller
#pragma once

#include "core-AI\BehTreeAction.h"
#include "core\RuntimeData.h"


///////////////////////////////////////////////////////////////////////////////

class StoryNodeInstance;
class PhysicsCharacterController;
class BlendTreePlayer;
class BehTreeVarFloat;

///////////////////////////////////////////////////////////////////////////////

/**
* This action will tell the actor to move to the specified location.
*/
class BTAAnimationToPhysics : public BehTreeAction
{
   DECLARE_ALLOCATOR( BTAAnimationToPhysics, AM_DEFAULT );
   DECLARE_CLASS();

private:
   // static data
   BehTreeVarFloat*                                         m_characterRotation;

   // runtime data
   TRuntimeVar< PhysicsCharacterController* >               m_characterController;
   TRuntimeVar< BlendTreePlayer* >                          m_blendTreePlayer;

public:
   /**
   * Constructor.
   */
   BTAAnimationToPhysics();
   ~BTAAnimationToPhysics();

   // -------------------------------------------------------------------------
   // BehTreeAction implementation
   // -------------------------------------------------------------------------
   void createLayout( BehaviorTreeRunner& runner ) const;
   void initialize( BehaviorTreeRunner& runner ) const;
   void deinitialize( BehaviorTreeRunner& runner ) const;
   Result execute( BehaviorTreeRunner& runner ) const;
};

///////////////////////////////////////////////////////////////////////////////

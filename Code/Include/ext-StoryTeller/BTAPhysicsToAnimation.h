/// @file   ext-StoryTeller\BTAPhysicsToAnimation.h
/// @brief  action that relays physical movement to the blend tree, translating it to animation
#pragma once

#include "core-AI\BehTreeAction.h"
#include "core\RuntimeData.h"


///////////////////////////////////////////////////////////////////////////////

class StoryNodeInstance;
class PhysicsCharacterController;
class BlendTreePlayer;
class BTVarFloat;

///////////////////////////////////////////////////////////////////////////////

/**
* This action will tell the actor to move to the specified location.
*/
class BTAPhysicsToAnimation : public BehTreeAction
{
   DECLARE_ALLOCATOR( BTAPhysicsToAnimation, AM_DEFAULT );
   DECLARE_CLASS();

private:
   // static data
   std::string                                              m_linearSpeedVarName;
   std::string                                              m_directionVarName;

   // runtime data
   TRuntimeVar< PhysicsCharacterController* >               m_characterController;
   TRuntimeVar< BlendTreePlayer* >                          m_blendTreePlayer;
   TRuntimeVar< const BTVarFloat* >                         m_linearSpeedVar;
   TRuntimeVar< const BTVarFloat* >                         m_directionVar;

public:
   /**
    * Constructor.
    */
   BTAPhysicsToAnimation();
   ~BTAPhysicsToAnimation();

   // -------------------------------------------------------------------------
   // BehTreeAction implementation
   // -------------------------------------------------------------------------
   void createLayout( BehaviorTreeRunner& runner ) const;
   void initialize( BehaviorTreeRunner& runner ) const;
   void deinitialize( BehaviorTreeRunner& runner ) const;
   Result execute( BehaviorTreeRunner& runner ) const;
};

///////////////////////////////////////////////////////////////////////////////

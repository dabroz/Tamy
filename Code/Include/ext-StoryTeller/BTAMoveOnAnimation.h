/// @file   ext-StoryTeller/BTAMoveOnAnimation.h
/// @brief  Moves an actor with the specified velocity using animation synthesized by the blendtree
#pragma once

#include "core-AI\BehTreeAction.h"
#include "core\RuntimeData.h"
#include "core\Vector.h"


///////////////////////////////////////////////////////////////////////////////

class StoryNodeInstance;
class BehTreeVarVector;
class BTVarFloat;
class BlendTreePlayer;

///////////////////////////////////////////////////////////////////////////////

/**
* This action will tell the actor to move to the specified location.
*/
class BTAMoveOnAnimation : public BehTreeAction
{
   DECLARE_ALLOCATOR( BTAMoveOnAnimation, AM_DEFAULT );
   DECLARE_CLASS();

private:
   // static data
   std::string                                              m_linearSpeedVarName;
   std::string                                              m_directionVarName;
   Vector                                                   m_characterFwd;
   BehTreeVarVector*                                        m_velocity;

   // runtime data
   TRuntimeVar< BlendTreePlayer* >                          m_blendTreePlayer;
   TRuntimeVar< const BTVarFloat* >                         m_linearSpeedVar;
   TRuntimeVar< const BTVarFloat* >                         m_directionVar;

public:
   /**
   * Constructor.
   */
   BTAMoveOnAnimation();
   ~BTAMoveOnAnimation();

   // -------------------------------------------------------------------------
   // BehTreeAction implementation
   // -------------------------------------------------------------------------
   void createLayout( BehaviorTreeRunner& runner ) const;
   void initialize( BehaviorTreeRunner& runner ) const;
   void deinitialize( BehaviorTreeRunner& runner ) const;
   Result execute( BehaviorTreeRunner& runner ) const;
};

///////////////////////////////////////////////////////////////////////////////

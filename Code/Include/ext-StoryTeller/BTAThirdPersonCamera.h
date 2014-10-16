/// @file   ext-StoryTeller/BTAThirdPersonCamera.h
/// @brief  this action controls the behavior of in-game camera
#pragma once

#include "core-AI\BehTreeAction.h"
#include "core\RuntimeData.h"
#include "core\Vector.h"


///////////////////////////////////////////////////////////////////////////////

class StoryNodeInstance;

///////////////////////////////////////////////////////////////////////////////

/**
 * This action controls the behavior of in-game camera - implements a 3rd person camera controller
 * that follows an actor.
 */
class BTAThirdPersonCamera : public BehTreeAction
{
   DECLARE_ALLOCATOR( BTAThirdPersonCamera, AM_DEFAULT );
   DECLARE_CLASS();

private:
   Vector                                 m_cameraPosOffset;
   Vector                                 m_cameraLookAtOffset;
   float                                  m_slowdownRadius;

   // runtime data
   TRuntimeVar< StoryNodeInstance* >      m_actorInstance;
   TRuntimeVar< Vector >                  m_cameraVelocity;

public:
   /**
    * Constructor.
    */
   BTAThirdPersonCamera();
   ~BTAThirdPersonCamera();

   // -------------------------------------------------------------------------
   // BehTreeAction implementation
   // -------------------------------------------------------------------------
   void createLayout( BehaviorTreeRunner& runner ) const;
   void initialize( BehaviorTreeRunner& runner ) const;
   void deinitialize( BehaviorTreeRunner& runner ) const;
   Result execute( BehaviorTreeRunner& runner ) const;
};

///////////////////////////////////////////////////////////////////////////////

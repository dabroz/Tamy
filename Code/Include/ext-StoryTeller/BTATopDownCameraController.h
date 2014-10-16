/// @file   ext-StoryTeller/BTATopDownCameraController.h
/// @brief  this action controls the behavior of in-game camera
#pragma once

#include "core-AI\BehTreeAction.h"
#include "core\RuntimeData.h"


///////////////////////////////////////////////////////////////////////////////

class StoryNodeInstance;

///////////////////////////////////////////////////////////////////////////////

/**
 * This action controls the behavior of in-game camera - shows the scene from the top, hovering above 
 * an actor.
 */
class BTATopDownCameraController : public BehTreeAction
{
   DECLARE_ALLOCATOR( BTATopDownCameraController, AM_DEFAULT );
   DECLARE_CLASS();

private:
   float                                  m_height;
      
   // runtime data
   TRuntimeVar< StoryNodeInstance* >      m_actorInstance;

public:
   /**
    * Constructor.
    */
   BTATopDownCameraController();
   ~BTATopDownCameraController();

   // -------------------------------------------------------------------------
   // BehTreeAction implementation
   // -------------------------------------------------------------------------
   void createLayout( BehaviorTreeRunner& runner ) const;
   void initialize( BehaviorTreeRunner& runner ) const;
   void deinitialize( BehaviorTreeRunner& runner ) const;
   Result execute( BehaviorTreeRunner& runner ) const;
};

///////////////////////////////////////////////////////////////////////////////

/// @file   ext-StoryTeller/BTAMoveTo.h
/// @brief  this action will tell the actor to move to the specified location
#pragma once

#include "core-AI\BehTreeAction.h"
#include "core\RuntimeData.h"


///////////////////////////////////////////////////////////////////////////////

class StoryNodeInstance;
class BehTreeVarVector;

///////////////////////////////////////////////////////////////////////////////

/**
 * This action will tell the actor to move to the specified location.
 */
class BTAMoveTo : public BehTreeAction
{
   DECLARE_ALLOCATOR( BTAMoveTo, AM_DEFAULT );
   DECLARE_CLASS();

private:
   BehTreeVarVector*                         m_worldPos;

public:
   /**
    * Constructor.
    */
   BTAMoveTo();
   ~BTAMoveTo();

   // -------------------------------------------------------------------------
   // BehTreeAction implementation
   // -------------------------------------------------------------------------
   void createLayout( BehaviorTreeRunner& runner ) const;
   void initialize( BehaviorTreeRunner& runner ) const;
   void deinitialize( BehaviorTreeRunner& runner ) const;
   Result execute( BehaviorTreeRunner& runner ) const;
};

///////////////////////////////////////////////////////////////////////////////

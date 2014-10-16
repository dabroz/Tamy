/// @file   core-AI/BlendTreeStateMachine.h
/// @brief  a state machine used as a composite blend tree node
#pragma once

#include "core-AI\BlendTreeComposite.h"
#include "core\RuntimeData.h"


///////////////////////////////////////////////////////////////////////////////

class BlendTreeStateTransition;

///////////////////////////////////////////////////////////////////////////////

/**
 * A state machine used as a composite blend tree node.
 */
class BlendTreeStateMachine : public BlendTreeComposite
{
   DECLARE_ALLOCATOR( BlendTreeStateMachine, AM_DEFAULT );
   DECLARE_CLASS();

private:
   // static data
   Array< BlendTreeStateTransition* >                 m_transitions;

   // runtime data
   TRuntimeVar< const BlendTreeNode* >                m_activeState;
   TRuntimeVar< const BlendTreeStateTransition* >     m_activeTransition;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   BlendTreeStateMachine( const char* name = "StaterMachine" );
   ~BlendTreeStateMachine();

   /**
    * Adds a transition that connects two states.
    *
    * @param transition
    */
   void addTransition( BlendTreeStateTransition* transition );

   /**
    * Removes a transition connecting two states.
    *
    * @param transition
    */
   void removeTransition( BlendTreeStateTransition* transition );

   // -------------------------------------------------------------------------
   // BlendTreeNode implementation
   // -------------------------------------------------------------------------
   void pullStructure( BlendTreeListener* listener ) override;
   void initializeLayout( BlendTreePlayer* player ) const override;
   void deinitializeLayout( BlendTreePlayer* player ) const override;
   void onActivateNode( BlendTreePlayer* player ) const override;
   void onDeactivateNode( BlendTreePlayer* player ) const override;
   void onUpdateLogic( BlendTreePlayer* player ) const override;
   void onSynchronizeNodeToTree( BlendTreePlayer* player, const BlendTreeSyncProfile& syncData, BlendTreeNodeSyncProfile& outNodeSyncProfile ) const override;
   void onSamplePose( BlendTreePlayer* player, float timeDelta, Transform* outGeneratedPose, Transform& outAccMotion, uint bonesCount ) const override;

   // -------------------------------------------------------------------------
   // BlendTreeComposite implementation
   // -------------------------------------------------------------------------
   void onNodeRemoved( BlendTreeNode* node, uint idx ) override;

private:
   BlendTreeStateTransition* sampleNoTransition( BlendTreePlayer* player, float& timeDelta ) const;
};

///////////////////////////////////////////////////////////////////////////////

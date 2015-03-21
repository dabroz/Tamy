/// @file   core-AI/BlendTreeSelector.h
/// @brief  a composite node that selects between attached nodes and plays one of them
#pragma once

#include "core-AI\BlendTreeComposite.h"
#include "core\RuntimeData.h"


///////////////////////////////////////////////////////////////////////////////

class BlendTreeSelectorSwitch;
class BlendTreeTransitionEffect;

///////////////////////////////////////////////////////////////////////////////

/**
 * A composite node that selects between attached nodes and plays one of them.
 */
class BlendTreeSelector : public BlendTreeComposite
{
   DECLARE_ALLOCATOR( BlendTreeSelector, AM_ALIGNED_16 );
   DECLARE_CLASS();

private:
   // static data
   BlendTreeTransitionEffect*                         m_effect;
   BlendTreeSelectorSwitch*                           m_btSwitch;

   // runtime data
   TRuntimeVar< const BlendTreeNode* >                m_activeNode;
   TRuntimeVar< const BlendTreeNode* >                m_transitionTargetNode;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   BlendTreeSelector( const char* name = "Selector" );
   ~BlendTreeSelector();

   /**
    * Sets an effect the selector should use during transitions.
    *
    * @param effect.
    */
   void setEffect( BlendTreeTransitionEffect* effect );

   /**
    * Sets a switch that will decide which child should be activated.
    *
    * @param switch
    */
   void setSwitch( BlendTreeSelectorSwitch* btSwitch );

   // -------------------------------------------------------------------------
   // BlendTreeNode implementation
   // -------------------------------------------------------------------------
   void initializeLayout( BlendTreePlayer* player ) const override;
   void deinitializeLayout( BlendTreePlayer* player ) const override;
   void onActivateNode( BlendTreePlayer* player ) const override;
   void onDeactivateNode( BlendTreePlayer* player ) const override;
   void onUpdateLogic( BlendTreePlayer* player ) const override;
   void onSynchronizeNodeToTree( BlendTreePlayer* player, const BlendTreeSyncProfile& syncData, BlendTreeNodeSyncProfile& outNodeSyncProfile ) const override;
   void onSamplePose( BlendTreePlayer* player, float timeDelta, Transform* outGeneratedPoseDiffLS, Transform& outAccMotion, uint bonesCount ) const override;
};

///////////////////////////////////////////////////////////////////////////////

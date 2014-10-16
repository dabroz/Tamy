/// @file   core-AI/BlendTreeTransitionEffect.h
/// @brief  a blend tree transition effect interface
#pragma once

#include "core\ReflectionObject.h"
#include "core\RuntimeData.h"
#include "core\Transform.h"


/////////////////////////////////////////////////////////////////////////////////////////

class BlendTreeNode;
class BlendTreePlayer;

/////////////////////////////////////////////////////////////////////////////////////////

/**
 * A transition effect is responsible for combining poses coming from two distinct
 * nodes the tree transitions between.
 */
class BlendTreeTransitionEffect : public ReflectionObject
{
   DECLARE_ALLOCATOR( BlendTreeTransitionEffect, AM_DEFAULT );
   DECLARE_CLASS();

protected:
   TRuntimeVar< const BlendTreeNode* >       m_sourceNode;
   TRuntimeVar< const BlendTreeNode* >       m_targetNode;

private:
   TRuntimeVar< float >                      m_duration;
   TRuntimeVar< float >                      m_progress;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   BlendTreeTransitionEffect();
   virtual ~BlendTreeTransitionEffect();

   /**
    * Initializes the effect's runtime data layout.
    *
    * @param player
    */
   virtual void initializeLayout( BlendTreePlayer* player ) const;

   /**
    * Deinitializes the effect's runtime data layout.
    *
    * @param player
    */
   virtual void deinitializeLayout( BlendTreePlayer* player ) const;

   /**
    * Called when the effect is activated.
    *
    * @param player
    * @param sourceNode
    * @param targetNode
    */
   virtual void activateEffect( BlendTreePlayer* player, const BlendTreeNode* sourceNode, const BlendTreeNode* targetNode ) const;

   /**
    * Called when the effect is deactivated.
    *
    * @param player
    */
   virtual void deactivateEffect( BlendTreePlayer* player ) const;

   /**
    * Updates the transition effect.
    *
    * @param player
    * @param timeDelta     upon return this value will be reduced by the amount the transition required this frame
    * @param outGeneratedPose
    * @param outAccMotion
    * @param bonesCount
    *
    * @return              'true' if the transition has ended, 'false' if it's still in progress
    */
   bool update( BlendTreePlayer* player, float timeDelta, Transform* outGeneratedPose, Transform& outAccMotion, uint bonesCount ) const;

   /**
    * Returns the progress value as a percent.
    *
    * @param player
    */
   float getProgressPercentage( BlendTreePlayer* player ) const;

protected:
   /**
    * Implementation specific update function.
    *
    * @param player
    * @param timeDelta 
    * @param outGeneratedPose
    * @param outAccMotion
    * @param bonesCount
    *
    * @return              'true' if the transition has ended, 'false' if it's still in progress
    */
   virtual void combinePoses( BlendTreePlayer* player, float timeDelta, Transform* outGeneratedPose, Transform& outAccMotion, uint bonesCount ) const {}

   /**
    * Sets the new duration of the effect ( at runtime ).
    *
    * @param player
    * @param duration
    */
   void setDuration( BlendTreePlayer* player, float duration ) const;
};

/////////////////////////////////////////////////////////////////////////////////////////

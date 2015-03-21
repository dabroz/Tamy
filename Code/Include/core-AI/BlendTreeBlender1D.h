/// @file   core-AI/BlendTreeBlender1D.h
/// @brief  a 1-dimensional continuous blend node
#pragma once

#include "core-AI\BlendTreeComposite.h"
#include "core\Array.h"
#include "core\RuntimeData.h"


///////////////////////////////////////////////////////////////////////////////

class BTVarFloat;

///////////////////////////////////////////////////////////////////////////////

/**
 * A 1-dimensional continuous blend node.
 *
 * This blender allows to specify one blend weight only and
 * will choose accordingly which of its children to blend together
 * based on the blend weights they were assigned off-line.
 */
class BlendTreeBlender1D : public BlendTreeComposite
{
   DECLARE_ALLOCATOR( BlendTreeBlender1D, AM_DEFAULT );
   DECLARE_CLASS();

private:
   struct SortedPoint
   {
      DECLARE_ALLOCATOR( SortedPoint, AM_DEFAULT );

      const BlendTreeBlender1D*           m_blender;
      uint                                m_nodeIdx;

      SortedPoint( const BlendTreeBlender1D* blender );

      // ----------------------------------------------------------------------
      // Comparator API
      // ----------------------------------------------------------------------
      static bool isLesser( const SortedPoint& lhs, const SortedPoint& rhs );
      static bool isGreater( const SortedPoint& lhs, const SortedPoint& rhs );
   };
   friend struct SortedPoint;

   struct BlendSegment
   {
      DECLARE_ALLOCATOR( BlendSegment, AM_DEFAULT );

      uint                                m_firstNodeIdx;
      uint                                m_secondNodeIdx;
   };

private:
   // static data
   Array< float >                         m_paramValues;
   BTVarFloat*                            m_controlParam;

   // runtime data
   // We're making it mutable because it will be initialized the first time the node's runtime
   // layout is created, and then the data is gonna be reused by subsequent instances.
   mutable Array< BlendSegment >          m_blendSegments; 
   mutable bool                           m_rebuildBlendSectors; // does the m_blendPoints array require rebuilding

   TRuntimeVar< uint >                    m_activeSegmentIdx;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   BlendTreeBlender1D( const char* name = "1D Blender" );
   ~BlendTreeBlender1D();
   
   /**
    * Assigns the specified child a specific parameter value.
    * If the control parameter value reaches this value, the child
    * it corresponds to will be blended with blend weight equal 1.0f.
    *
    * @param childIdx
    * @param blendWeight
    */
   void assignParameterValue( uint childIdx, float value );

   /**
    * Returns a parameter value assigned to a node with the specified index.
    *
    * @param childIdx
    */
   float getParameterValue( uint childIdx ) const;

   /**
    * Sets a new BlendTree variable that will be used as a control parameter.
    *
    * @param controlParam
    */
   void setControlParameter( BTVarFloat* controlParam );

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

protected:
   // -------------------------------------------------------------------------
   // BlendTreeComposite implementation
   // -------------------------------------------------------------------------
   void onNodeRemoved( BlendTreeNode* node, uint idx ) override;
   void onNodeInserted( BlendTreeNode* node, uint idx ) override;

private:
   void buildSegmentsList() const;
   float getControlParamValue( BlendTreePlayer* player ) const;
};

///////////////////////////////////////////////////////////////////////////////

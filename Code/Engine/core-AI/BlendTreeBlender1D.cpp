#include "core-AI\BlendTreeBlender1D.h"
#include "core-AI\BlendTreePlayer.h"
#include "core-AI\BlendTreeVariable.h"
#include "core-AI\BlendTreeSyncProfile.h"
#include "core-AI\PoseBlendingUtils.h"
#include "core\ArrayUtils.h"
#include "core\Array.h"
#include "core\Log.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BlendTreeBlender1D );
   PARENT( BlendTreeComposite );
   PROPERTY( Array< float >, m_paramValues );
   PROPERTY_EDIT_REFERENCE( "Blend control", BTVarFloat*, m_controlParam );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BlendTreeBlender1D::BlendTreeBlender1D( const char* name )
   : BlendTreeComposite( name )
   , m_controlParam( NULL )
   , m_rebuildBlendSectors( true )
{
   registerReferences();
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeBlender1D::~BlendTreeBlender1D()
{
   // don't reset the m_controlParam pointer to NULL - the reference mechanism needs to do it itself
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeBlender1D::onNodeInserted( BlendTreeNode* node, uint idx )
{
   // grow the array of control params
   m_paramValues.insert( idx, 0.0f );
   m_rebuildBlendSectors = true;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeBlender1D::onNodeRemoved( BlendTreeNode* node, uint idx )
{
   m_paramValues.remove( idx );
   m_rebuildBlendSectors = true;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeBlender1D::assignParameterValue( uint childIdx, float value )
{
   ASSERT_MSG( childIdx < m_paramValues.size(), "Invalid child index" );

   m_paramValues[childIdx] = value;
   m_rebuildBlendSectors = true;
}

///////////////////////////////////////////////////////////////////////////////

float BlendTreeBlender1D::getParameterValue( uint childIdx ) const
{
   return m_paramValues[childIdx];
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeBlender1D::setControlParameter( BTVarFloat* controlParam )
{
   NOTIFY_PROPERTY_CHANGE( m_controlParam );
   m_controlParam = controlParam;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeBlender1D::buildSegmentsList() const 
{
   m_blendSegments.clear();

   const uint nodesCount = m_nodes.size();
   if ( nodesCount == 0 )
   {
      return;
   }

   // sort the blend points
   Array< SortedPoint > sortedPoints( nodesCount );
   sortedPoints.resize( nodesCount, SortedPoint( this ) );
   for ( uint i = 0; i < nodesCount; ++i )
   {
      SortedPoint& point = sortedPoints[i];
      point.m_nodeIdx = i;
   }

   ArrayUtils::quickSort< SortedPoint, SortedPoint >( sortedPoints );

   // build an array of sectors from it
   uint segmentsCount = nodesCount - 1;
   uint entiresCount = segmentsCount * 2 + 1;
   m_blendSegments.resize( entiresCount, BlendSegment() );

   uint entryIdx = 0;
   for ( uint segmentIdx = 0; segmentIdx < segmentsCount; ++segmentIdx )
   {
      BlendSegment* segment = &m_blendSegments[entryIdx]; ++entryIdx;
      segment->m_firstNodeIdx = sortedPoints[segmentIdx].m_nodeIdx;
      segment->m_secondNodeIdx = sortedPoints[segmentIdx].m_nodeIdx;

      segment = &m_blendSegments[entryIdx]; ++entryIdx;
      segment->m_firstNodeIdx = sortedPoints[segmentIdx].m_nodeIdx;
      segment->m_secondNodeIdx = sortedPoints[segmentIdx + 1].m_nodeIdx;
   }

   BlendSegment* segment = &m_blendSegments[entryIdx];
   segment->m_firstNodeIdx = sortedPoints[segmentsCount].m_nodeIdx;
   segment->m_secondNodeIdx = sortedPoints[segmentsCount].m_nodeIdx;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeBlender1D::initializeLayout( BlendTreePlayer* player ) const
{
   BlendTreeComposite::initializeLayout( player );

   RuntimeDataBuffer& data = player->data();
   data.registerVar( m_activeSegmentIdx );

   // the first segment on the list is initially active
   data[m_activeSegmentIdx] = -1;

   // lazily build a list of blend points
   if ( m_rebuildBlendSectors )
   {
      buildSegmentsList();
      m_rebuildBlendSectors = false;
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeBlender1D::deinitializeLayout( BlendTreePlayer* player ) const
{
   RuntimeDataBuffer& data = player->data();
   data.unregisterVar( m_activeSegmentIdx );

   BlendTreeComposite::deinitializeLayout( player );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeBlender1D::onActivateNode( BlendTreePlayer* player ) const
{
   BlendTreeComposite::onActivateNode( player );

   RuntimeDataBuffer& data = player->data();
   data[m_activeSegmentIdx] = -1;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeBlender1D::onDeactivateNode( BlendTreePlayer* player ) const
{
   BlendTreeComposite::onDeactivateNode( player );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeBlender1D::onUpdateLogic( BlendTreePlayer* player ) const
{
   float controlParamValue = getControlParamValue( player );

   // The algorithm here is as follows - we have clips ordered by the rising parameter value.
   // We treat these nodes as points on a line - points we want to interpolate between.
   //
   // So having a control parameter value, we find a blend line segment it falls to
   // and we blend the nodes the ends of the segment correspond to.
   RuntimeDataBuffer& data = player->data();

   int newActiveSegmentIdx = -1;
   const uint segmentsCount = m_blendSegments.size();
   for ( uint i = 0; i < segmentsCount; ++i )
   {
      const BlendSegment& segment = m_blendSegments[i];
      const float startValue = m_paramValues[segment.m_firstNodeIdx];
      const float endValue = m_paramValues[segment.m_secondNodeIdx];

      if ( startValue == endValue && startValue == controlParamValue )
      {
         newActiveSegmentIdx = i;
         break;
      }
      else if ( startValue <= controlParamValue && controlParamValue < endValue )
      {
         newActiveSegmentIdx = i;
         break;
      }
   }
   ASSERT( newActiveSegmentIdx >= 0 );


   const int activeSegmentIdx = data[m_activeSegmentIdx];
   if ( activeSegmentIdx != newActiveSegmentIdx )
   {
      // segment has changed - perform the necessary activations/deactivations
      data[m_activeSegmentIdx] = newActiveSegmentIdx;

      const uint nodesCount = m_nodes.size();
      Array< char > activationFlag( nodesCount );
      activationFlag.resize( nodesCount, 0 );

      if ( activeSegmentIdx >= 0 )
      {
         const BlendSegment& deactivatedSeg = m_blendSegments[activeSegmentIdx];
         activationFlag[deactivatedSeg.m_firstNodeIdx] = -1;
         activationFlag[deactivatedSeg.m_secondNodeIdx] = -1;
      }

      // activate only the start node, if the blend weight is equal to its value
      {
         const BlendSegment& activatedSeg = m_blendSegments[newActiveSegmentIdx];
         activationFlag[activatedSeg.m_firstNodeIdx]++;
         if ( activatedSeg.m_firstNodeIdx != activatedSeg.m_secondNodeIdx )
         {
            activationFlag[activatedSeg.m_secondNodeIdx]++;
         }
      }

      for ( uint i = 0; i < nodesCount; ++i )
      {
         const char flag = activationFlag[i];
         if ( flag < 0 )
         {
            deactivateChildNode( player, m_nodes[i] );
         }
         else if ( flag > 0 )
         {
            activateChildNode( player, m_nodes[i] );
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeBlender1D::onSynchronizeNodeToTree( BlendTreePlayer* player, const BlendTreeSyncProfile& syncData, BlendTreeNodeSyncProfile& outNodeSyncProfile ) const
{
   RuntimeDataBuffer& data = player->data();
   const int activeSegmentIdx = data[m_activeSegmentIdx];
   if ( activeSegmentIdx < 0 )
   {
      return;
   }
   const BlendSegment& activateSeg = m_blendSegments[activeSegmentIdx];
   const uint startNodeIdx = activateSeg.m_firstNodeIdx;
   const uint endNodeIdx = activateSeg.m_secondNodeIdx;

   const BlendTreeNode* startNode = m_nodes[startNodeIdx];
   const BlendTreeNodeSyncProfile* startNodeSyncData = startNode->getSyncData( player );
   if ( startNodeIdx != endNodeIdx )
   {  
      const BlendTreeNode* endNode = m_nodes[endNodeIdx];
      const BlendTreeNodeSyncProfile* endNodeSyncData = endNode->getSyncData( player );

      float controlParamValue = getControlParamValue( player );
      const float blendWeight = ( controlParamValue - m_paramValues[startNodeIdx] ) / ( m_paramValues[endNodeIdx] - m_paramValues[startNodeIdx] );

      outNodeSyncProfile.mergeWith( startNodeSyncData, 1.0f - blendWeight );
      outNodeSyncProfile.mergeWith( endNodeSyncData, blendWeight );
   }
   else
   {
      outNodeSyncProfile.mergeWith( startNodeSyncData, 1.0f );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeBlender1D::onSamplePose( BlendTreePlayer* player, float timeDelta, Transform* outGeneratedPoseDiffLS, Transform& outAccMotion, uint bonesCount ) const
{
   RuntimeDataBuffer& data = player->data();
   const int activeSegmentIdx = data[m_activeSegmentIdx];


   // We need to calculate blend weights for segment ends, such that would satisfy the linear 
   // interpolation below:
   //
   // controlParamValue = pointA_Value * ( 1 - blendWeight ) + pointBValue * blendWeight
   // 
   // where: blendWeight is the variable we're looking for.

   float controlParamValue = getControlParamValue( player );

   const BlendSegment& activateSeg = m_blendSegments[activeSegmentIdx];
   const uint startNodeIdx = activateSeg.m_firstNodeIdx;
   const uint endNodeIdx = activateSeg.m_secondNodeIdx;

   if ( startNodeIdx != endNodeIdx )
   {
      const float blendWeight = ( controlParamValue - m_paramValues[startNodeIdx] ) / ( m_paramValues[endNodeIdx] - m_paramValues[startNodeIdx] );

      // combine the poses using these blend weights
      const BlendTreeNode* sourceNode = m_nodes[startNodeIdx];
      const BlendTreeNode* targetNode = m_nodes[endNodeIdx];

      Transform* sourcePose = sourceNode->getGeneratedPose( player );
      Transform* targetPose = targetNode->getGeneratedPose( player );
      PoseBlendingUtils::blend( FastFloat::fromFloat( blendWeight ), sourcePose, targetPose, bonesCount, outGeneratedPoseDiffLS );

      // blend the accumulated motion
      const Transform& accSourceMotion = sourceNode->getAccumulatedMotion( player );
      const Transform& accTargetMotion = targetNode->getAccumulatedMotion( player );
      PoseBlendingUtils::blend( FastFloat::fromFloat( blendWeight ), &accSourceMotion, &accTargetMotion, 1, &outAccMotion );
   }
   else
   {
      // output the data from the only active node
      const BlendTreeNode* sourceNode = m_nodes[startNodeIdx];
      Transform* sourcePose = sourceNode->getGeneratedPose( player );
      memcpy( outGeneratedPoseDiffLS, sourcePose, sizeof( Transform ) * bonesCount );

      outAccMotion = sourceNode->getAccumulatedMotion( player );
   }
}

///////////////////////////////////////////////////////////////////////////////

float BlendTreeBlender1D::getControlParamValue( BlendTreePlayer* player ) const
{
   float controlParamValue;
   if ( m_controlParam )
   {
      controlParamValue = m_controlParam->getRuntime( player );

      // limit the value top a valid range
      const float minValue = m_paramValues[ m_blendSegments.front().m_firstNodeIdx ];
      const float maxValue = m_paramValues[ m_blendSegments.back().m_secondNodeIdx ];
      controlParamValue = clamp< float >( controlParamValue, minValue, maxValue );
   }
   else
   {
      controlParamValue = m_paramValues[ m_blendSegments.front().m_firstNodeIdx ];
   }

   // put the value in the correct range
   return controlParamValue;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BlendTreeBlender1D::SortedPoint::SortedPoint( const BlendTreeBlender1D* blender )
   : m_blender( blender )
   , m_nodeIdx( 0 )
{
}

///////////////////////////////////////////////////////////////////////////////

bool BlendTreeBlender1D::SortedPoint::isLesser( const SortedPoint& lhs, const SortedPoint& rhs )
{
   return lhs.m_blender->m_paramValues[lhs.m_nodeIdx] < rhs.m_blender->m_paramValues[rhs.m_nodeIdx];
}

///////////////////////////////////////////////////////////////////////////////

bool BlendTreeBlender1D::SortedPoint::isGreater( const SortedPoint& lhs, const SortedPoint& rhs )
{
   return lhs.m_blender->m_paramValues[lhs.m_nodeIdx] > rhs.m_blender->m_paramValues[rhs.m_nodeIdx];
}

///////////////////////////////////////////////////////////////////////////////

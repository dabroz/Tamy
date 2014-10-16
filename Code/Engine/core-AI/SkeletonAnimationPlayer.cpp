#include "core-AI\SkeletonAnimationPlayer.h"
#include "core-AI\SnapshotAnimation.h"
#include "core-AI\Skeleton.h"
#include "core-AI\SkeletonComponent.h"
#include "core-MVC\Entity.h"
#include "core-MVC\EntityUtils.h"
#include "core\ReflectionProperty.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( SkeletonAnimationPlayer );
   PARENT( AnimationPlayer );
   PROPERTY_EDIT( "Animation", SnapshotAnimation*, m_source );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

SkeletonAnimationPlayer::SkeletonAnimationPlayer( const char* name )
   : AnimationPlayer( name )
   , m_source( NULL )
   , m_posesSink( NULL )
   , m_motionExtractionSink( NULL )
   , m_tmpBoneTransforms( NULL )
   , m_trackTime( 0.f )
{
}

///////////////////////////////////////////////////////////////////////////////

SkeletonAnimationPlayer::SkeletonAnimationPlayer( const SkeletonAnimationPlayer& rhs )
   : AnimationPlayer( rhs )
   , m_source( rhs.m_source )
   , m_posesSink( NULL )
   , m_motionExtractionSink( NULL )
   , m_tmpBoneTransforms( NULL )
   , m_trackTime( 0.f )
{

}

///////////////////////////////////////////////////////////////////////////////

SkeletonAnimationPlayer::~SkeletonAnimationPlayer()
{
   delete [] m_tmpBoneTransforms;
   m_tmpBoneTransforms = NULL;

   m_source = NULL;
   m_posesSink = NULL;
   m_motionExtractionSink = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonAnimationPlayer::setAnimationSource( SnapshotAnimation* source )
{
   if ( source != m_source )
   {
      NOTIFY_PROPERTY_CHANGE( m_source );

      // set the new animation source
      m_source = source;
   }
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonAnimationPlayer::onSiblingAttached( SceneNode* node )
{
   AnimationPlayer::onSiblingAttached( node );

   if ( node->isA< SkeletonComponent >() )
   {
      m_posesSink = static_cast< SkeletonComponent* >( node );

      ASSERT( !m_tmpBoneTransforms );
      if ( m_posesSink->m_skeleton )
      {
         // TODO: !!!! if the skeleton instance changes while this component is attached, we won't learn about it and
         // we could possibly start setting transforms on non-existing bones
         // Copy the solution from the BlendTreePlayer

         uint bonesCount = m_posesSink->m_skeleton->getBoneCount();
         m_tmpBoneTransforms = new Transform[bonesCount];
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonAnimationPlayer::onSiblingDetached( SceneNode* node )
{
   AnimationPlayer::onSiblingDetached( node );

   if ( node == m_posesSink )
   {
      delete [] m_tmpBoneTransforms;
      m_tmpBoneTransforms = NULL;

      m_posesSink = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonAnimationPlayer::onAttached( Entity* parent )
{
   AnimationPlayer::onAttached( parent );
   m_motionExtractionSink = parent;
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonAnimationPlayer::onDetached( Entity* parent ) 
{
   m_motionExtractionSink = NULL;

   AnimationPlayer::onDetached( parent );
}

///////////////////////////////////////////////////////////////////////////////

bool SkeletonAnimationPlayer::isEnabled() const
{
   return m_source && m_posesSink && m_motionExtractionSink;
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonAnimationPlayer::samplePoses( float timeElapsed ) 
{
   // update the track time
   float prevTrackTime = m_trackTime;
   m_trackTime += timeElapsed;
   m_trackTime = fmod( m_trackTime, m_source->m_duration );

   // apply the pose
   {
      Skeleton* skeleton = m_posesSink->m_skeleton;

      uint boneCount = skeleton->getBoneCount();
      m_source->samplePose( m_trackTime, m_tmpBoneTransforms, boneCount );

      Matrix tmpMtx;
      for ( uint i = 0; i < boneCount; ++i )
      {
         m_tmpBoneTransforms[i].toMatrix( tmpMtx );

         m_posesSink->m_boneLocalMtx[i].setMul( tmpMtx, skeleton->m_boneLocalMatrices[i] );
      }
   }

   // apply motion extraction
   {
      Transform deltaTransform;
      m_source->sampleMotion( prevTrackTime, m_trackTime, deltaTransform );

      Matrix deltaMotionMtx;
      deltaTransform.toMatrix( deltaMotionMtx );

      Matrix currLocalTransform = m_motionExtractionSink->getLocalMtx();
      Matrix newLocalTransform;
      newLocalTransform.setMul( currLocalTransform, deltaMotionMtx );
      m_motionExtractionSink->setLocalMtx( newLocalTransform );
   }
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonAnimationPlayer::setTrackTime( float time )
{
   m_trackTime = clamp( time, 0.0f, m_source->m_duration );
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonAnimationPlayer::onStarted()
{
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonAnimationPlayer::onFinished()
{
   if ( m_posesSink )
   {
      m_posesSink->resetToTPose();
   }
}

///////////////////////////////////////////////////////////////////////////////

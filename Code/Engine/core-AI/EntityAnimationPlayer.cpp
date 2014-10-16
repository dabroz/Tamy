#include "core-AI\EntityAnimationPlayer.h"
#include "core-AI\SnapshotAnimation.h"
#include "core-MVC\Entity.h"
#include "core-MVC\EntityUtils.h"
#include "core\ReflectionProperty.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( EntityAnimationPlayer );
   PARENT( AnimationPlayer );
   PROPERTY_EDIT( "Animation", SnapshotAnimation*, m_source );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

EntityAnimationPlayer::EntityAnimationPlayer( const char* name )
   : AnimationPlayer( name )
   , m_source( NULL )
   , m_animatedEntity( NULL )
   , m_trackTime( 0.f )
{
}

///////////////////////////////////////////////////////////////////////////////

EntityAnimationPlayer::EntityAnimationPlayer( const EntityAnimationPlayer& rhs )
   : AnimationPlayer( rhs )
   , m_source( rhs.m_source )
   , m_animatedEntity( NULL )
   , m_trackTime( 0.f )
{

}

///////////////////////////////////////////////////////////////////////////////

EntityAnimationPlayer::~EntityAnimationPlayer()
{
   m_source = NULL;
   m_animatedEntity = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void EntityAnimationPlayer::setAnimationSource( SnapshotAnimation& source )
{
   ReflectionProperty* property = getProperty( "m_source" );
   notifyPrePropertyChange( *property );

   // set the new animation source
   m_source = &source;

   // notify listeners about it
   notifyPropertyChange( *property );
   delete property;
}

///////////////////////////////////////////////////////////////////////////////

void EntityAnimationPlayer::onAttached( Entity* parent )
{
   AnimationPlayer::onAttached( parent );

   m_animatedEntity = parent;
}

///////////////////////////////////////////////////////////////////////////////

void EntityAnimationPlayer::onDetached( Entity* parent ) 
{
   m_animatedEntity = NULL;

   AnimationPlayer::onDetached( parent );
}

///////////////////////////////////////////////////////////////////////////////

bool EntityAnimationPlayer::isEnabled() const
{
   return m_source && m_animatedEntity;
}

///////////////////////////////////////////////////////////////////////////////

void EntityAnimationPlayer::samplePoses( float timeElapsed ) 
{
   // update the track time
   float prevTrackTime = m_trackTime;
   m_trackTime += timeElapsed;
   m_trackTime = fmod( m_trackTime, m_source->m_duration );

   // apply animation results
   Transform startTransform, endTransform, deltaTransform;
   m_source->samplePose( prevTrackTime, &startTransform, 1 );
   m_source->samplePose( m_trackTime, &endTransform, 1 );

   deltaTransform.setMulInverse( endTransform, startTransform );

   Matrix deltaMotionMtx;
   deltaTransform.toMatrix( deltaMotionMtx );

   Matrix currLocalTransform = m_animatedEntity->getLocalMtx();
   Matrix newLocalTransform;
   newLocalTransform.setMul( currLocalTransform, deltaMotionMtx );
   m_animatedEntity->setLocalMtx( newLocalTransform );
}

///////////////////////////////////////////////////////////////////////////////

void EntityAnimationPlayer::setTrackTime( float time )
{
   m_trackTime = clamp( time, 0.0f, m_source->m_duration );
}

///////////////////////////////////////////////////////////////////////////////

void EntityAnimationPlayer::onStarted()
{
}

///////////////////////////////////////////////////////////////////////////////

void EntityAnimationPlayer::onFinished()
{
}

///////////////////////////////////////////////////////////////////////////////

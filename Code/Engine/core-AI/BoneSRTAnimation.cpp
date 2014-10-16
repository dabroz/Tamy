#include "core-AI\BoneSRTAnimation.h"


///////////////////////////////////////////////////////////////////////////////

BoneSRTAnimation::BoneSRTAnimation()
   : m_duration( 0.0f )
   , m_orientationKeyIdx( 0 )
   , m_translationKeyIdx( 0 )
{
}

///////////////////////////////////////////////////////////////////////////////

void BoneSRTAnimation::addOrientationKey( float time, const Quaternion& orientation )
{
   m_orientation.addKey( time, orientation );

   // update the animation duration
   updateDuration();
}

///////////////////////////////////////////////////////////////////////////////

void BoneSRTAnimation::addTranslationKey( float time, const Vector& translation )
{
   m_translation.addKey( time, translation );

   // update the animation duration
   updateDuration();
}

///////////////////////////////////////////////////////////////////////////////

void BoneSRTAnimation::updateDuration()
{
   m_duration = 0.0f;

   if ( m_orientation.m_time.empty() == false && m_duration < m_orientation.m_time.back() )
   {
      m_duration = m_orientation.m_time.back();
   }

   if ( m_translation.m_time.empty() == false && m_duration < m_translation.m_time.back() )
   {
      m_duration = m_translation.m_time.back();
   }
}

///////////////////////////////////////////////////////////////////////////////

void BoneSRTAnimation::sample( float trackTime, Transform& outTransform )
{
   m_translation.getKey( m_translationKeyIdx, trackTime, outTransform.m_translation );
   m_orientation.getKey( m_orientationKeyIdx, trackTime, outTransform.m_rotation );
}

///////////////////////////////////////////////////////////////////////////////

#ifndef _ANIMATION_TIMELINE_H
#error "This file can only be included from AnimationTimeline.h"
#else

#include "core/Assert.h"
#include "core/FastFloat.h"


///////////////////////////////////////////////////////////////////////////////

template< typename T, typename LERP >
void AnimationTimeline< T, LERP >::addKey( float time, const T& key )
{
   // time needs to be specified in range <0, inf )
   if ( time < 0.0f )
   {
      time = 0.0f;
   }

   // find the first appropriate time to insert the orientation key at
   unsigned int count = m_time.size();
   unsigned int idx = 0;
   bool replace = false;
   for ( idx = 0; idx < count; ++idx )
   {
      // find the first entry larger than the specified time, and insert directly before it
      if ( m_time[idx] == time )
      {
         replace = true;
         break;
      }
      else if ( m_time[idx] > time )
      {
         replace = false;
         break;
      }
   }

   if ( replace )
   {
      ASSERT_MSG( idx < count, "Trying to replace a non-existent key" );
      m_keys[idx] = key;
   }
   else
   {
      if ( idx < count )
      {
         // this is the very first key
         m_time.push_front( time );
         m_keys.push_front( key );
      }
      else
      {
         // this is the very last key
         m_time.push_back( time );
         m_keys.push_back( key );
      }
   }

   // make sure there's the same number of entries in both list that describe the orientation timeline
   ASSERT_MSG( m_time.size() == m_keys.size(), "Animation timeline discrepancy" );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T, typename LERP >
bool AnimationTimeline< T, LERP >::getKey( unsigned int& lastCheckedKeyIdx, float time, T& outKey ) const
{
   if ( m_keys.empty() )
   {
      return false;
   }

   if ( time <= 0.0f )
   {
      // fetch the first key
      outKey = m_keys[ 0 ];
   }
   else if ( time >= m_time.back() )
   {
      outKey = m_keys.back();
   }

   // verify if the passed index is within the animation bounds
   unsigned int count = m_time.size();
   if ( lastCheckedKeyIdx >= count )
   {
      // no - it's not - reset
      lastCheckedKeyIdx = 0;
   }

   // find the time
   FastFloat ffTime;
   ffTime.setFromFloat( time );

   unsigned lastIdx, nextIdx;
   FastFloat lastTime, nextTime, lerpFactor, duration;
   lastIdx = lastCheckedKeyIdx;
   for ( unsigned int i = 0; i < count; ++i )
   {
      nextIdx = ( lastIdx + 1 ) % count;
      lastTime.setFromFloat( m_time[ lastIdx ] );
      nextTime.setFromFloat( m_time[ nextIdx ] );

      if ( lastTime <= ffTime && ffTime < nextTime )
      {
         duration.setSub( nextTime, lastTime );
         ASSERT_MSG( duration > Float_0, "Two keys can't occupy the same time frame" );
         lerpFactor.setSub( ffTime, lastTime );
         lerpFactor.div( duration );

         // we found our key space
         m_lerp( outKey, m_keys[ lastIdx ], m_keys[ nextIdx ], lerpFactor );
         lastCheckedKeyIdx = lastIdx;
         break;
      }


      lastIdx = nextIdx;
   }

   return true;
}

///////////////////////////////////////////////////////////////////////////////

#endif // _ANIMATION_TIMELINE_H

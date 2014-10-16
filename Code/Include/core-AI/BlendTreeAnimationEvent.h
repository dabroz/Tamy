/// @file   core-AI/BlendTreeAnimationEvent.h
/// @brief  an event triggered directly from an animation time line
#pragma once

#include "core\ReflectionObject.h"


///////////////////////////////////////////////////////////////////////////////

class BlendTreeEvent;

///////////////////////////////////////////////////////////////////////////////

/**
 * An event triggered directly from an animation time line.
 */
class BlendTreeAnimationEvent : public ReflectionObject
{
   DECLARE_ALLOCATOR( BlendTreeAnimationEvent, AM_DEFAULT );
   DECLARE_CLASS();

private:
   BlendTreeEvent*         m_btEvent;

   // The time is not measured in percents ( 0..1 ) - the value corresponds to a position
   // on a time track that's as long as the animation.
   float                   m_time;

public:
   /**
    * Constructor.
    *
    * @param btEvent       a blend tree event we want to trigger using this animation event
    * @param time
    */
   BlendTreeAnimationEvent( BlendTreeEvent* btEvent = NULL, float time = 0.0f );
   ~BlendTreeAnimationEvent();

   /**
    * Makes this animation event trigger the specified BlendTreeEvent
    */
   void setBlendTreeEvent( BlendTreeEvent* btEvent );

   /**
    * Returns a BlendTreeEvent instance this animation event triggers.
    */
   inline BlendTreeEvent* getBlendTreeEvent() const { return m_btEvent; }

   /**
    * Returns the time the event was set for. 
    */
   inline float getTime() const { return m_time; }

   /**
    * Sets a new time when the event will be triggered.
    *
    * @param time
    */
   void setTime( float time );
};

///////////////////////////////////////////////////////////////////////////////

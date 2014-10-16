/// @file   core-AI/BTTTEvent.h
/// @brief  a BlendTree transition trigger activated when an event is triggered
#pragma once

#include "core-AI\BlendTreeTransitionTrigger.h"


///////////////////////////////////////////////////////////////////////////////

class BlendTreeEvent;

///////////////////////////////////////////////////////////////////////////////

/**
 * A BlendTree transition trigger activated when an event is triggered.
 */
class BTTTEvent : public BlendTreeTransitionTrigger
{
   DECLARE_ALLOCATOR( BTTTEvent, AM_DEFAULT );
   DECLARE_CLASS();

private:
   BlendTreeEvent*               m_btEvent;

public:
   /**
    * Constructor.
    */
   BTTTEvent();
   ~BTTTEvent();

   /**
    * Sets am event instance, that, when fired, will trigger the host transition.
    *
    * @param btEvent
    */
   void setEvent( BlendTreeEvent* btEvent );

   // -------------------------------------------------------------------------
   // BlendTreeTransitionTrigger implementation
   // -------------------------------------------------------------------------
   bool testActivation( BlendTreePlayer* player ) const;
};

///////////////////////////////////////////////////////////////////////////////

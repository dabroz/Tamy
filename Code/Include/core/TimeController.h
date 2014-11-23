/// @file   core\TimeController.h
/// @brief  tool for scheduling tasks in time
#pragma once

#include <string>
#include "core\MemoryRouter.h"
#include "core\TimeControllerTrack.h"
#include "core\Singleton.h"
#include "core\Array.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * TimeController provides the means to update multiple 'updatable' classes
 * simultaneously in a timely fashion.
 *
 * It can have many time tracks defined, each running with a different speed.
 * Each 'updatable' can be added to such track, and thus effectively
 * be running with a different speed.
 *
 * It also provides the facility to limit the execution of a particular track
 * to a specified frame rate.
 */
class TimeController
{
   PRIORITY_SINGLETON( 11 );
   DECLARE_ALLOCATOR( TimeController, AM_DEFAULT );

private:
   TimeController*                           m_parentController;
   Array< TimeControllerTrack* >             m_tracks;
   Array< TimeController* >                  m_childControllers;

   List< TimeControllerTrack* >              m_tracksToAdd;
   List< uint >                              m_tracksToRemove;

   float                                     m_timeElapsed;

public:
   /**
    * Constructor.
    */
   TimeController();

   /**
    * Singleton constructor.
    */
   TimeController( const SingletonConstruct& );

   /**
    * Constructor setting a controllers hierarchy.
    */
   TimeController( TimeController& parentController );
   ~TimeController();

   /**
    * Adds a new track.
    *
    * @param object
    */
   TimeControllerTrack& add( TimeDependent& object );

   /**
    * Returns a track the specified object runs on
    *
    * @param object
    */
   TimeControllerTrack* get( const TimeDependent& object );

   /**
    * Removes a track with the specified ID
    *
    * @param object
    */
   void remove( const TimeDependent& object );

   /**
    * Updates all registered tracks.
    *
    * @param timeElapsed
    */
   void update( float timeElapsed );

   /**
    * Resets all tracks by resetting their speeds and removing all
    * attached objects and events.
    */
   void resetAll();

   /**
    * Returns the time step method 'update' was called with.
    */
   inline float getTimeElapsed() const { return m_timeElapsed; }

protected:
   /**
    * Attaches a new child controller.
    *
    * @param child
    */
   void attach( TimeController& child );

   /**
    * Detaches a child controller.
    *
    * @param child
    */
   void detach( TimeController& child );

   /**
    * Removes the parent information.
    */
   void clearParent();

private:
   void processTrackOperations();
};

///////////////////////////////////////////////////////////////////////////////

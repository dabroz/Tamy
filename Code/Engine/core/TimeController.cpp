#include "core\TimeController.h"
#include "core\Assert.h"
#include "core\ListUtils.h"
#include <algorithm>


///////////////////////////////////////////////////////////////////////////////

INIT_SINGLETON( TimeController );

///////////////////////////////////////////////////////////////////////////////

TimeController::TimeController()
   : m_parentController( NULL )
   , m_timeElapsed( 0.0f )
{
}

///////////////////////////////////////////////////////////////////////////////

TimeController::TimeController( const SingletonConstruct& )
   : m_parentController( NULL )
   , m_timeElapsed( 0.0f )
{
}

///////////////////////////////////////////////////////////////////////////////

TimeController::TimeController( TimeController& parentController )
   : m_parentController( &parentController )
   , m_timeElapsed( 0.0f )
{
   m_parentController->attach( *this );
}

///////////////////////////////////////////////////////////////////////////////

TimeController::~TimeController()
{
   if ( m_parentController )
   {
      m_parentController->detach( *this );
      m_parentController = NULL;
   }

   while( !m_tracksToAdd.empty() )
   {
      TimeControllerTrack* trackToAdd = m_tracksToAdd.front();
      m_tracksToAdd.popFront();

      delete trackToAdd;
   }

   uint count = m_tracks.size();
   for ( uint i = 0; i < count; ++i )
   {
      delete m_tracks[i];
   }

   // auto detach child controllers
   count = m_childControllers.size();
   for ( uint i = 0; i < count; ++i )
   {
      m_childControllers[i]->clearParent();
   }
   m_childControllers.clear();
}

///////////////////////////////////////////////////////////////////////////////

void TimeController::attach( TimeController& child )
{
   // make sure the controller's not already attached
   uint idx = m_childControllers.find( &child );
   if ( idx == EOA )
   {
      m_childControllers.push_back( &child );
   }
}

///////////////////////////////////////////////////////////////////////////////

void TimeController::detach( TimeController& child )
{
   uint idx = m_childControllers.find( &child );
   if ( idx != EOA )
   {
      m_childControllers.remove( idx );
   }
}

///////////////////////////////////////////////////////////////////////////////

void TimeController::clearParent()
{
   m_parentController = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void TimeController::update(float timeElapsed)
{
   m_timeElapsed = timeElapsed;

   processTrackOperations();

   unsigned int count = m_tracks.size();
   for( unsigned int i = 0; i < count; ++i )
   {
      TimeControllerTrack* track = m_tracks[i];
      track->update( timeElapsed );
   }

   count = m_childControllers.size();
   for( unsigned int i = 0; i < count; ++i )
   {
      TimeController* childController = m_childControllers[i];
      childController->update( timeElapsed );
   }
}

///////////////////////////////////////////////////////////////////////////////

TimeControllerTrack* TimeController::get( const TimeDependent& object )
{
   unsigned int count = m_tracks.size();
   for( unsigned int i = 0; i < count; ++i )
   {
      TimeControllerTrack* track = m_tracks[i];

      if ( &track->m_object == &object )
      {
         return track;
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

TimeControllerTrack& TimeController::add( TimeDependent& object )
{
   // check if we're not trying to remove the track
   for ( List< uint >::iterator it = m_tracksToRemove.begin(); !it.isEnd(); ++it )
   {
      uint trackIdx = *it;
      TimeControllerTrack* track = m_tracks[trackIdx];
      if ( &track->m_object == &object )
      {
         // yes - we're actually trying to add this track
         track->play( true );
         it.markForRemoval();
         break;
      }
   }

   // check if we haven't already scheduled this object for addition
   for ( List< TimeControllerTrack* >::iterator it = m_tracksToAdd.begin(); !it.isEnd(); ++it )
   {
      TimeControllerTrack* track = *it;
      if ( &track->m_object == &object )
      {
         // yes, it's already supposed to be added
         return *track;
      }
   }

   // check if it's not running already
   uint count = m_tracks.size();
   for ( uint i = 0; i < count; ++i )
   {
      TimeControllerTrack* track = m_tracks[i];
      if ( &track->m_object == &object )
      {
         // yes, it's already supposed to be added
         return *track;
      }
   }

   // add it to the queue
   TimeControllerTrack* newTrack = new TimeControllerTrack( object );
   m_tracksToAdd.pushBack( newTrack );

   return *newTrack;
}

///////////////////////////////////////////////////////////////////////////////

void TimeController::remove( const TimeDependent& object )
{
   // check if we're not trying to add the track
   for ( List< TimeControllerTrack* >::iterator it = m_tracksToAdd.begin(); !it.isEnd(); ++it )
   {
      TimeControllerTrack* track = *it;
      if ( &track->m_object == &object )
      {
         // yes - we're actually trying to add this track
         delete track;
         it.markForRemoval();
         return;
      }
   }

   // do we even have a track dedicated to the specified object
   uint count = m_tracks.size();
   for( unsigned int i = 0; i < count; ++i )
   {
      TimeControllerTrack* track = m_tracks[i];
      if ( &track->m_object == &object )
      {
         // that's the track we want to remove
         track->play( false );
         ListUtils::insertSorted( m_tracksToRemove, i );
         
         break;
      }
   } 
}

///////////////////////////////////////////////////////////////////////////////

void TimeController::resetAll()
{
   unsigned int count = m_tracks.size();
   for (unsigned int i = 0; i < count; ++i)
   {
      m_tracks[i]->reset();
   }
}

///////////////////////////////////////////////////////////////////////////////

void TimeController::processTrackOperations()
{
   // remove tracks
   while( !m_tracksToRemove.empty() )
   {
      uint trackForRemovalIdx = m_tracksToRemove.back();
      m_tracksToRemove.popBack();

      // find the track and remove it
      TimeControllerTrack* track = m_tracks[ trackForRemovalIdx ];
      m_tracks.remove( trackForRemovalIdx );
      delete track;
   }

   // add tracks
   // remove tracks
   while( !m_tracksToAdd.empty() )
   {
      TimeControllerTrack* trackToAdd = m_tracksToAdd.front();
      m_tracksToAdd.popFront();

      m_tracks.push_back( trackToAdd );
   }
}

///////////////////////////////////////////////////////////////////////////////

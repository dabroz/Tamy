/// @file   core-AI/BlendTreeEvent.h
/// @brief  an internal blend tree event
#pragma once

#include "core\ReflectionObject.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

/**
 * An internal blend tree event.
 */
class BlendTreeEvent : public ReflectionObject
{
   DECLARE_ALLOCATOR( BlendTreeEvent, AM_DEFAULT );
   DECLARE_CLASS();

private:
   // static data
   std::string       m_name;

   // runtime data
   uint              m_eventIdx; // an index assigned internally to each event, identifying it's position in its host BlendTree's events array

public:
   /**
    * Constructor.
    *
    * @param name
    */
   BlendTreeEvent( const char* name = "Event" );
   ~BlendTreeEvent();

   /**
    * Returns the name of the event.
    */
   inline const std::string& getName() const { return m_name; }

   /**
    * Gives the event a new name.
    *
    * @param name
    */
   void setName( const char* name );

   /**
    * Returns the event's internal index.
    */
   inline uint getIndex() const { return m_eventIdx; }

   /**
    * Assigns the event an internal index in the host BlendTree's internal array.
    *
    * @param idx
    */
   inline void setIndex( uint idx ) { m_eventIdx = idx; }
};

///////////////////////////////////////////////////////////////////////////////

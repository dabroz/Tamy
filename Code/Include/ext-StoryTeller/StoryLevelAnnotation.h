/// @file   ext-StoryTeller\SpawnPoint.h
/// @brief  a component that marks an entity as a special place on the level
#pragma once

#include "core-MVC\Component.h"


///////////////////////////////////////////////////////////////////////////////

class StoryLevelAnnotation : public Component
{
   DECLARE_ALLOCATOR( StoryLevelAnnotation, AM_DEFAULT );
   DECLARE_CLASS();

public:
   bool           m_spawnPoint;

public:
   /**
    * Constructor.
    */
   StoryLevelAnnotation( const char* name = "StoryLevelAnnotation" );

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   StoryLevelAnnotation( const StoryLevelAnnotation& rhs );

};

///////////////////////////////////////////////////////////////////////////////

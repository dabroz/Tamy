/// @file   ext-StoryTeller\SAGetRandomAnnotation.h
/// @brief  returns a random, unoccupied entity annotated with the specified annotation
#pragma once

#include "ext-StoryTeller\StoryAction.h"
#include "ext-StoryTeller\StoryActionSockets.h"


///////////////////////////////////////////////////////////////////////////////

class SAGetRandomAnnotation : public StoryAction
{
   DECLARE_ALLOCATOR( SAGetRandomAnnotation, AM_DEFAULT );
   DECLARE_CLASS();

private:
   // static data
   bool                          m_spawnPoint;
   std::string                   m_annotationName;

   // runtime data
   SAAnnotationOutput*           m_out;

public:
   /**
    * Constructor.
    */
   SAGetRandomAnnotation();

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   SAGetRandomAnnotation( const SAGetRandomAnnotation& rhs );
   ~SAGetRandomAnnotation();

   // -------------------------------------------------------------------------
   // StoryAction implementation
   // -------------------------------------------------------------------------
   bool execute( StoryPlayer& runner, List< StoryAction* >& outActionsToActivate ) const;

   // -------------------------------------------------------------------------
   // ReflectionObject implementation
   // -------------------------------------------------------------------------
   void onObjectLoaded();

};

///////////////////////////////////////////////////////////////////////////////

/// @file   ext-StoryTeller/StoryActionSockets.h
/// @brief  story action sockets definitions.
#pragma once

#include "core\GraphBuilderSockets.h"


///////////////////////////////////////////////////////////////////////////////

class StoryAction;
class StoryLevelAnnotation;
typedef GBNodeOutput< StoryAction > SAOutput;

///////////////////////////////////////////////////////////////////////////////

class SAVoidInput : public GBNodeInput< StoryAction >
{
   DECLARE_ALLOCATOR( SAVoidInput, AM_DEFAULT );
   DECLARE_CLASS()

public:
   /**
    * Constructor.
    */
   SAVoidInput( const std::string& name = "" ) : GBNodeInput< StoryAction >( name ) {}
};

class SAVoidOutput : public GBNodeOutput< StoryAction >
{
   DECLARE_ALLOCATOR( SAVoidOutput, AM_DEFAULT );
   DECLARE_CLASS()

public:
   /**
    * Constructor.
    */
   SAVoidOutput( const std::string& name = "" ) : GBNodeOutput< StoryAction >( name ) {}
};

///////////////////////////////////////////////////////////////////////////////

class SAAnnotationInput : public TGBNodePtrInput< StoryAction, StoryLevelAnnotation >
{
   DECLARE_ALLOCATOR( SAAnnotationInput, AM_DEFAULT );
   DECLARE_CLASS()

public:
   /**
   * Constructor.
   */
   SAAnnotationInput( const std::string& name = "" ) : TGBNodePtrInput< StoryAction, StoryLevelAnnotation >( name ) {}
};

class SAAnnotationOutput : public TGBNodePtrOutput< StoryAction, StoryLevelAnnotation >
{
   DECLARE_ALLOCATOR( SAAnnotationOutput, AM_DEFAULT );
   DECLARE_CLASS()

public:
   /**
   * Constructor.
   */
   SAAnnotationOutput( const std::string& name = "" ) : TGBNodePtrOutput< StoryAction, StoryLevelAnnotation >( name ) {}
};

///////////////////////////////////////////////////////////////////////////////

#include "StoryChapterLayout.h"
#include "core.h"
#include "ext-StoryTeller.h"

// blocks & sockets
#include "StoryActionSocketRepresentation.h"


///////////////////////////////////////////////////////////////////////////////

StoryChapterLayout::StoryChapterLayout( StoryChapterGraphBuilder& graphBuilder )
   : TGraphLayout< StoryAction >( graphBuilder )
{
   initialize();
}

///////////////////////////////////////////////////////////////////////////////

void StoryChapterLayout::initSocketsFactory( SocketsFactory& factory )
{
   factory.associate< SAVoidInput, SSSVoid >();
   factory.associate< SAVoidOutput, SSSVoid >();
   factory.associate< SAAnnotationInput, SSSAnnotation >();
   factory.associate< SAAnnotationOutput, SSSAnnotation >();
}

///////////////////////////////////////////////////////////////////////////////

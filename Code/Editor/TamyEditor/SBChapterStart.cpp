#include "SBChapterStart.h"
#include "core\ResourceHandle.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( SBChapterStart );
   PARENT( GraphBlock );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

SBChapterStart::SBChapterStart( StoryChapterStart& node )
   : TGraphWidgetBlock< StoryChapterStart, StoryAction >( node )
{
   setCaption( "Start" );
}

///////////////////////////////////////////////////////////////////////////////

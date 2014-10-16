#include "STIRoot.h"
#include "ext-StoryTeller\Story.h"
#include "ext-StoryTeller\StoryChapter.h"


///////////////////////////////////////////////////////////////////////////////

STIRoot::STIRoot( QTreeWidget* parentTreeWidget, Story& story )
   : STIChapter( story.getRoot() )
   , m_story( story )
{
   parentTreeWidget->addTopLevelItem( this );

   setIcon( 0, QIcon( ":/TamyEditor/Resources/Icons/Engine/StoryRootIcon.png" ) );
   updateDescription();
}

///////////////////////////////////////////////////////////////////////////////

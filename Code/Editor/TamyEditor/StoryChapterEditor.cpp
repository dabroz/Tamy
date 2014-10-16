#include "StoryChapterEditor.h"
#include "ext-StoryTeller\StoryChapter.h"
#include <QtWidgets\QVBoxLayout>
#include <QtWidgets\QGraphicsView>
#include "GraphView.h"
#include "StoryChapterLayout.h"
#include <QtWidgets\QMenu>


///////////////////////////////////////////////////////////////////////////////

StoryChapterEditor::StoryChapterEditor( QWidget* parentWidget, StoryChapter& chapter )
   : QFrame( parentWidget )
{
   QVBoxLayout* layout = new QVBoxLayout( this );
   layout->setSpacing(0);
   layout->setMargin(0);
   setLayout( layout );

   m_graphWidget = new GraphView( this );
   layout->addWidget( m_graphWidget );

   m_chapterLayout = new StoryChapterLayout( chapter );
   m_graphWidget->setScene( m_chapterLayout );
}

///////////////////////////////////////////////////////////////////////////////

StoryChapterEditor::~StoryChapterEditor()
{
   delete m_chapterLayout;
   m_chapterLayout = NULL;
}

///////////////////////////////////////////////////////////////////////////////

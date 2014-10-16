/// @file   TamyEditor/StoryChapterEditor.h
/// @brief  a StoryChapter graph editor
#pragma once

#include <QtWidgets\QFrame>


///////////////////////////////////////////////////////////////////////////////

class StoryChapter;
class QGraphicsView;
class StoryChapterLayout;

///////////////////////////////////////////////////////////////////////////////

/**
 * A StoryChapter property editor.
 */
class StoryChapterEditor : public QFrame
{
   Q_OBJECT

private:
   QGraphicsView*                            m_graphWidget;
   StoryChapterLayout*                       m_chapterLayout;

public:
   /**
    * Constructor.
    *
    * @param parentWidget
    * @param chapter
    */
   StoryChapterEditor( QWidget* parentWidget, StoryChapter& chapter );
   ~StoryChapterEditor();
};

///////////////////////////////////////////////////////////////////////////////

/// @file   TamyEditor/BlendTreeStateMachineEditor.h
/// @brief  an editor that edits blend tree state machine's internal structure
#pragma once

#include <QtWidgets\QFrame>


///////////////////////////////////////////////////////////////////////////////

class BlendTreeStateMachine;
class BlendTreeStateMachineLayout;
class QGraphicsView;
class QPropertiesView;
class ReflectionObject;

///////////////////////////////////////////////////////////////////////////////

/**
 * A StoryChapter property editor.
 */
class BlendTreeStateMachineEditor : public QFrame
{
   Q_OBJECT

private:
   QGraphicsView*                            m_graphWidget;
   BlendTreeStateMachineLayout*              m_stateMachineLayout;
   QPropertiesView*                          m_propertiesView;

public:
   /**
    * Constructor.
    *
    * @param parentWidget
    * @param stateMachine
    */
   BlendTreeStateMachineEditor( QWidget* parentWidget, BlendTreeStateMachine* stateMachine );
   ~BlendTreeStateMachineEditor();

public slots:
   void onShowObjectProperties( ReflectionObject* object );
};

///////////////////////////////////////////////////////////////////////////////

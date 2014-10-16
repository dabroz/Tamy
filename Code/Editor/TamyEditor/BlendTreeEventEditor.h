/// @file   TamyEditor/BlendTreeEventEditor.h
/// @brief  a dedicated BlendTreeEvent editor
#pragma once

#include <QtWidgets\QFrame>


///////////////////////////////////////////////////////////////////////////////

class BlendTree;
class BlendTreeEvent;
class QLineEdit;

///////////////////////////////////////////////////////////////////////////////

class BlendTreeEventEditor : public QFrame
{
   Q_OBJECT

private:
   BlendTree&        m_blendTree;
   BlendTreeEvent*   m_event;

   QLineEdit*        m_eventName;

public:
   /**
    * Constructor.
    *
    * @param parent
    * @param blendTree
    */
   BlendTreeEventEditor( QWidget* parent, BlendTree& blendTree );
   ~BlendTreeEventEditor();

   /**
    * Initializes the editor.
    *
    * @param event
    */
   void initialize( BlendTreeEvent* event );

   /**
    * Returns the name of the edited event
    */
   QString getEventName() const;

public slots:
   void removeEvent( bool );
};

///////////////////////////////////////////////////////////////////////////////

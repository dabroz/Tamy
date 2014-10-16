#include "BlendTreeStateMachineEditor.h"
#include "core-AI\BlendTreeStateMachine.h"
#include "GraphView.h"
#include <QtWidgets\QVBoxLayout>
#include <QtWidgets\QGraphicsView>
#include <QtWidgets\QSplitter>
#include "QPropertiesView.h"
#include "BlendTreeStateMachineLayout.h"
#include <QtWidgets\QMenu>


///////////////////////////////////////////////////////////////////////////////

BlendTreeStateMachineEditor::BlendTreeStateMachineEditor( QWidget* parentWidget, BlendTreeStateMachine* stateMachine )
   : QFrame( parentWidget )
   , m_propertiesView( NULL )
{
   QVBoxLayout* layout = new QVBoxLayout( this );
   layout->setSpacing(0);
   layout->setMargin(0);
   setLayout( layout );

   QSplitter* mainAreaSplitter = new QSplitter( Qt::Vertical, this );
   layout->addWidget( mainAreaSplitter );
   {
      // state graph
      m_graphWidget = new QGraphicsView( this );
      {
         mainAreaSplitter->addWidget( m_graphWidget );

         m_stateMachineLayout = new BlendTreeStateMachineLayout( stateMachine );
         m_graphWidget->setScene( m_stateMachineLayout );
         m_graphWidget->setContextMenuPolicy( Qt::CustomContextMenu );
         m_graphWidget->setViewportUpdateMode( QGraphicsView::FullViewportUpdate );
         connect( m_stateMachineLayout, SIGNAL( onObjectSelected( ReflectionObject* ) ), this, SLOT( onShowObjectProperties( ReflectionObject* ) ) );
      }

      // properties view
      m_propertiesView = new QPropertiesView();
      {
         mainAreaSplitter->addWidget( m_propertiesView );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeStateMachineEditor::~BlendTreeStateMachineEditor()
{
   delete m_stateMachineLayout;
   m_stateMachineLayout = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateMachineEditor::onShowObjectProperties( ReflectionObject* object )
{
   if ( object )
   {
      m_propertiesView->set( *object );
   }
   else
   {
      m_propertiesView->reset();
   }
}

///////////////////////////////////////////////////////////////////////////////

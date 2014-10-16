#include "RenderStatesBrowser.h"
#include <QtWidgets\QTabWidget>
#include <QtWidgets\QToolBar>
#include <QtWidgets\QScrollArea>
#include <QtWidgets\QVBoxLayout>
#include <QtWidgets\QAction>
#include "QPropertiesView.h"

// misc
#include "core\Filesystem.h"

// geometry and materials
#include "core-Renderer\GeometryComponent.h"
#include "core-Renderer\RenderState.h"


///////////////////////////////////////////////////////////////////////////////

RenderStatesBrowser::RenderStatesBrowser( QWidget* parent, GeometryComponent& geometryComponent )
   : QFrame( parent )
   , m_geometryComponent( geometryComponent )
{
   // initialize UI
   QVBoxLayout* layout = new QVBoxLayout( this );
   setLayout( layout );

   m_renderStateView = new QPropertiesView();
   layout->addWidget( m_renderStateView );

   RenderState* renderState = geometryComponent.accessRenderState();
   m_renderStateView->set( *renderState );

}

///////////////////////////////////////////////////////////////////////////////

RenderStatesBrowser::~RenderStatesBrowser()
{
   RenderState* renderState = static_cast< RenderState* >( m_renderStateView->getEditedObject() );

   // remove its editor
   m_renderStateView->deleteLater();
}

///////////////////////////////////////////////////////////////////////////////

#include "SelectedEntityPropertiesViewer.h"
#include "QPropertiesView.h"
#include "SelectionManager.h"
#include "core-MVC\SceneNode.h"
#include "core-MVC\Entity.h"
#include <QtWidgets\QScrollArea>

// custom editors
#include "core-Renderer\GeometryComponent.h"
#include "RenderStatesBrowser.h"
#include "ext-2DGameLevel\GL2DProceduralLevel.h"
#include "ProceduralLevelGenerator.h"


///////////////////////////////////////////////////////////////////////////////

SelectedEntityPropertiesViewer::SelectedEntityPropertiesViewer( QWidget* parent )
   : QTabWidget( parent )
   , m_displayedNode( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

void SelectedEntityPropertiesViewer::onSelectionChanged( SelectionManager* selectionMgr )
{
   SceneNode* node = selectionMgr->getLastPickedNode();

   // We can have only one node selected and displayed at a time.
   // If there's more, simply ignore the selection and hide the properties view
   if ( node == m_displayedNode )
   {
      return;
   }
   
   deinitializeViewer();
   m_displayedNode = node;
   
   if ( m_displayedNode )
   {
      // memorize the displayed node
      initializeViewer();
   }

}

///////////////////////////////////////////////////////////////////////////////

void SelectedEntityPropertiesViewer::initializeViewer()
{
   // display this node's properties
   {
      QScrollArea* propertiesArea = new QScrollArea( this );
      propertiesArea->setWidgetResizable( true );
      addTab( propertiesArea, "Properties" );

      QPropertiesView* selectedNodeProperties = new QPropertiesView();
      propertiesArea->setWidget( selectedNodeProperties );
      m_displayedNode->viewProperties( *selectedNodeProperties );
   }

   if ( m_displayedNode->isA< GeometryComponent >() )
   {
      // if the selected node is a GeometryComponent, display the list of render states it holds
      GeometryComponent* geomComp = static_cast< GeometryComponent* >( m_displayedNode );
      RenderStatesBrowser* renderStatesBrowser = new RenderStatesBrowser( this, *geomComp );
      addTab( renderStatesBrowser, "Materials" );
   }
   else if ( m_displayedNode->isExactlyA< GL2DProceduralLevel >() )
   {
      // if it's the scene root that's selected, show some additional editors
      ProceduralLevelGenerator* generator = new ProceduralLevelGenerator( this, static_cast< GL2DProceduralLevel* >( m_displayedNode ) );
      addTab( generator, "Procedural generation" );
   }
}

///////////////////////////////////////////////////////////////////////////////

void SelectedEntityPropertiesViewer::deinitializeViewer()
{
   int tabsCount = count();
   for ( int i = tabsCount - 1; i >= 0; --i )
   {
      QWidget* tab = widget(i);
      delete tab;
   }
   clear();
}

///////////////////////////////////////////////////////////////////////////////


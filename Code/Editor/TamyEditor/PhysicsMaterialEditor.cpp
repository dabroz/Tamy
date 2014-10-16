#include "PhysicsMaterialEditor.h"
#include "core-Physics\PhysicsMaterial.h"

// qt
#include <QtWidgets\QVBoxLayout>
#include <QtWidgets\QToolBar>
#include <QtWidgets\QAction>

// tamy widgets
#include "QPropertiesView.h"


///////////////////////////////////////////////////////////////////////////////

PhysicsMaterialEditor::PhysicsMaterialEditor( PhysicsMaterial& material )
   : m_material( material )
{
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsMaterialEditor::onInitialize()
{
   QVBoxLayout* layout = new QVBoxLayout();
   this->setLayout( layout );

   // toolbar
   QToolBar* toolbar = new QToolBar( this );
   {
      layout->addWidget( toolbar );

      QAction* actionSave = new QAction( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/saveFile.png" ) ), tr( "Save" ), toolbar );
      actionSave->setShortcut( QKeySequence( tr( "Ctrl+S" ) ) );
      toolbar->addAction( actionSave );
      connect( actionSave, SIGNAL( triggered() ), this, SLOT( save() ) );
   }

   // main editor
   QPropertiesView* propertiesView = new QPropertiesView();
   {
      layout->addWidget( propertiesView );
      propertiesView->set( m_material );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsMaterialEditor::onDeinitialize( bool saveProgress )
{
   if ( saveProgress )
   {
      m_material.saveResource();
   }
}

///////////////////////////////////////////////////////////////////////////////

void PhysicsMaterialEditor::save()
{
   m_material.saveResource();
}

///////////////////////////////////////////////////////////////////////////////

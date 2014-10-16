#include "BehaviorTreeEditor.h"
#include "core-AI.h"
#include "core\ResourcesManager.h"
#include "core\Filesystem.h"
#include <QtWidgets\QVBoxLayout>
#include <QtWidgets\QSplitter>
#include <QtWidgets\QToolBar>
#include "QPropertiesView.h"

// behavior tree related widgets
#include "BehaviorTreeWidget.h"
#include "BehaviorTreeVariablesViewer.h"


///////////////////////////////////////////////////////////////////////////////

BehaviorTreeEditor::BehaviorTreeEditor( BehaviorTree& behTree )
   : m_behTree( behTree )
   , m_treeWidget( NULL )
   , m_nodeProperties( NULL )
   , m_variablesViewer( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

BehaviorTreeEditor::~BehaviorTreeEditor()
{
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeEditor::onInitialize()
{
   QVBoxLayout* mainLayout = new QVBoxLayout( this );
   {
      mainLayout->setSpacing( 0 );
      mainLayout->setMargin( 0 );
      setLayout( mainLayout );
   }

   QToolBar* toolbar = new QToolBar( this );
   {
      mainLayout->addWidget( toolbar );

      // add basic actions
      QAction* saveAction = toolbar->addAction( QIcon( ":/TamyEditor/Resources/Icons/Editor/saveFile.png" ), "Save story" );
      connect( saveAction, SIGNAL( triggered() ), this, SLOT( onSave() ) );
   }

   QSplitter* mainSplitter = new QSplitter( Qt::Horizontal, this );
   {
      mainLayout->addWidget( mainSplitter );

      // tree viewer
      m_treeWidget = new BehaviorTreeWidget( mainSplitter, m_behTree );
      {
         mainSplitter->addWidget( m_treeWidget );

         connect( m_treeWidget, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( editItem( QTreeWidgetItem*, int ) ) );
      }

      createRightHalfWindowSplitter( mainSplitter );
   }

   // adjust splitter's position
   QList< int > splitterWindowSizes;
   splitterWindowSizes.push_back( width() * 0.25 );
   splitterWindowSizes.push_back( width() * 0.75 );
   mainSplitter->setSizes( splitterWindowSizes );
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeEditor::createRightHalfWindowSplitter( QSplitter* mainSplitter )
{
   QSplitter* splitter = new QSplitter( Qt::Vertical, mainSplitter );
   {
      mainSplitter->addWidget( splitter );

      // properties viewer frame
      m_nodeProperties = new QPropertiesView();
      {
         splitter->addWidget( m_nodeProperties );
      }

      // variables viewer frame
      m_variablesViewer = new BehaviorTreeVariablesViewer( splitter, m_behTree, NULL );
      {
         splitter->addWidget( m_variablesViewer );
      }

   }

   // adjust splitter's position
   QList< int > splitterWindowSizes;
   splitterWindowSizes.push_back( width() * 0.5 );
   splitterWindowSizes.push_back( width() * 0.5 );
   splitter->setSizes( splitterWindowSizes );
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeEditor::onDeinitialize( bool saveProgress )
{
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeEditor::onSave()
{
   m_behTree.saveResource();
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeEditor::editItem( QTreeWidgetItem* item, int column )
{
   BehTreeWidgetItem* btItem = static_cast< BehTreeWidgetItem* >( item );
   BehTreeNode* btNode = btItem->getNode();

   btNode->viewProperties( *m_nodeProperties );
}

///////////////////////////////////////////////////////////////////////////////

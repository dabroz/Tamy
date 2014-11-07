#include "StoryEditor.h"
#include "core\ResourcesManager.h"
#include "core\Filesystem.h"
#include "core\TimeController.h"
#include "core-MVC\EntityUtils.h"
#include "ext-StoryTeller\Story.h"
#include "ext-StoryTeller\StoryPlayer.h"
#include "ext-StoryTeller\StoryChapter.h"
#include "ext-StoryTeller\StoryNodeInstance.h"
#include <QtWidgets\QVBoxLayout>
#include <QtWidgets\QSplitter>
#include <QtWidgets\QToolBar>
#include <QtWidgets\QTabWidget>
#include "StoryBrowserTreeWidget.h"
#include "QPropertiesView.h"
#include "StoryChapterEditor.h"
#include "TamySceneWidget.h"
#include "SelectionManager.h"
#include <QtWidgets\QMenu>


///////////////////////////////////////////////////////////////////////////////

StoryEditor::StoryEditor( Story& story )
   : m_story( story )
   , m_storyPlayer( NULL )
   , m_playing( false )
   , m_treeWidget( NULL )
   , m_nodeProperties( NULL )
   , m_selectedItem( NULL )
   , m_selectedChapter( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

StoryEditor::~StoryEditor()
{
}

///////////////////////////////////////////////////////////////////////////////

void StoryEditor::onInitialize()
{
   // create the story player
   m_storyPlayer = new StoryPlayer( m_story );
   m_playing = false;

   // create UI layout
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

   m_mainSplitter = new QSplitter( Qt::Horizontal, this );
   m_mainSplitter->setMaximumWidth( 200 );
   {
      mainLayout->addWidget( m_mainSplitter );

      // side toolbox
      QSplitter* sideToolboxSplitter = new QSplitter( Qt::Vertical, m_mainSplitter );
      {
         m_mainSplitter->addWidget( sideToolboxSplitter );

         // tree viewer
         m_treeWidget = new StoryBrowserTreeWidget( sideToolboxSplitter, m_story, m_storyPlayer->gameWorld() );
         {
            sideToolboxSplitter->addWidget( m_treeWidget );

            connect( m_treeWidget, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( onItemClicked( QTreeWidgetItem*, int ) ) );
            connect( m_treeWidget, SIGNAL( popupMenuShown( QTreeWidgetItem*, QMenu& ) ), this, SLOT( onStoryTreePopupMenuShown( QTreeWidgetItem*, QMenu& ) ) );
         }

         // properties viewer frame
         m_nodeProperties = new QPropertiesView();
         {
            sideToolboxSplitter->addWidget( m_nodeProperties );
         }

         // adjust splitter's position
         QList< int > splitterWindowSizes;
         splitterWindowSizes.push_back( width() * 0.5 );
         splitterWindowSizes.push_back( width() * 0.5 );
         sideToolboxSplitter->setSizes( splitterWindowSizes );
      }

      // work area frame
      m_workAreaTabsManager = new QTabWidget( m_mainSplitter );
      {
         m_mainSplitter->addWidget( m_workAreaTabsManager );

         // chapter graph editor
         QFrame* chapterGraphEditorFrame = new QFrame( m_workAreaTabsManager );
         {
            m_chapterGraphEditorLayout = new QVBoxLayout( chapterGraphEditorFrame );
            m_chapterGraphEditorLayout->setSpacing( 0 );
            m_chapterGraphEditorLayout->setMargin( 0 );
            chapterGraphEditorFrame->setLayout( m_chapterGraphEditorLayout );

            m_workAreaTabsManager->addTab( chapterGraphEditorFrame, "Chapter graph" );
         }

         // scene rendering widget
         {
            QFrame* sceneRendererFrame = new QFrame( m_workAreaTabsManager );
            QLayout* sceneRendererLayout = new QHBoxLayout();
            sceneRendererFrame->setLayout( sceneRendererLayout );
            sceneRendererLayout->setMargin( 0 );
            sceneRendererLayout->setSpacing( 0 );
            sceneRendererFrame->setFrameStyle( QFrame::Panel | QFrame::Sunken );
            sceneRendererFrame->setLineWidth( 2 );
            m_workAreaTabsManager->addTab( sceneRendererFrame, "Scene" );

            m_sceneWidget = new TamySceneWidget( sceneRendererFrame );
            sceneRendererLayout->addWidget( m_sceneWidget );

            connect( m_sceneWidget, SIGNAL( scenePlayed( bool ) ), this, SLOT( onPlay( bool ) ) );
         }

      }

      // adjust splitter's position
      QList< int > splitterWindowSizes;
      splitterWindowSizes.push_back( width() * 0.25 );
      splitterWindowSizes.push_back( width() * 0.75 );
      m_mainSplitter->setSizes( splitterWindowSizes );

      m_mainSplitter->setStretchFactor( 0, 0 );
   }

   // establish connections between widgets
   {
      m_storyPlayer->setExternalSystems( m_sceneWidget->getRenderer(), m_sceneWidget->getUserInputController() );

      Model& gameScene = m_storyPlayer->gameWorld();
      m_sceneWidget->setScene( &gameScene );
      m_sceneWidget->getSelectionMgr().attach( *this );
   }

   // hook self into an update loop
   TimeController& timeController = TSingleton< TimeController >::getInstance();
   timeController.add( *this );

   // show the editor
   show();
}

///////////////////////////////////////////////////////////////////////////////

void StoryEditor::onDeinitialize( bool saveProgress )
{
   // unhook self into an update loop
   TimeController& timeController = TSingleton< TimeController >::getInstance();
   timeController.remove( *this );

   m_sceneWidget->getSelectionMgr().detach( *this );
   m_sceneWidget->setScene( NULL );

   m_treeWidget->clearSnapshot();

   delete m_storyPlayer;
   m_storyPlayer = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void StoryEditor::onSave()
{
   m_story.saveResource();
}

///////////////////////////////////////////////////////////////////////////////

void StoryEditor::onItemClicked( QTreeWidgetItem* item, int column )
{
   if ( m_selectedItem == item )
   {
      // the selection hasn't changed
      return;
   }
   m_selectedItem = static_cast< StoryBrowserTreeItem* >( item );

   editItem( item );
   updatePreviewItemSelection( item );
}

///////////////////////////////////////////////////////////////////////////////

void StoryEditor::editItem( QTreeWidgetItem* item )
{
   if ( item )
   {
      StoryBrowserTreeItem* stItem = static_cast< StoryBrowserTreeItem* >( item );
      StoryNode* stNode = stItem->getNode();

      m_nodeProperties->set( *stNode );


      // get the nearest chapter node and draw up its graph
      StoryChapter* chapter = NULL;
      {
         StoryNode* storyChapterNode = NULL;
         if ( stNode->isA< StoryChapter >() )
         {
            storyChapterNode = stNode;
         }
         else
         {
            StoryBrowserTreeItem* parentItem = static_cast< StoryBrowserTreeItem* >( stItem->parent() );
            storyChapterNode = parentItem->getNode();
         }

         chapter = static_cast< StoryChapter* >( storyChapterNode );
      }

      if ( chapter != m_selectedChapter )
      {
         m_selectedChapter = chapter;

         // clear the work area
         int workAreaWidgetsCount = m_chapterGraphEditorLayout->count();
         if ( workAreaWidgetsCount >= 1 )
         {
            QLayoutItem* removedItem = m_chapterGraphEditorLayout->takeAt( 0 );
            delete removedItem->widget();
            delete removedItem;
         }

         StoryChapterEditor* editor = new StoryChapterEditor( m_chapterGraphEditorLayout->parentWidget(), *m_selectedChapter );
         m_chapterGraphEditorLayout->addWidget( editor );
      }
   }
   else
   {
      m_nodeProperties->reset();
   }
}

///////////////////////////////////////////////////////////////////////////////

void StoryEditor::updatePreviewItemSelection( QTreeWidgetItem* item )
{
   StoryBrowserTreeItem* stItem = static_cast< StoryBrowserTreeItem* >( item );
   StoryNode* stNode = stItem->getNode();

   // find a corresponding node in the scene
   Model& previewScene = m_storyPlayer->gameWorld();
   SceneNode* sceneNode = EntityUtils::findNodeByName( previewScene.getRoot(), stNode->getName() );
   
   // update the selection manager's selection
   SelectionManager& selectionMgr = m_sceneWidget->getSelectionMgr();
   if ( sceneNode )
   {
      selectionMgr.selectNode( *sceneNode );
   }
   else
   {
      selectionMgr.resetSelection();
   }
}

///////////////////////////////////////////////////////////////////////////////

void StoryEditor::onSelectionChanged( SelectionManager* selectionMgr )
{   
   SceneNode* selectedSceneNode = selectionMgr->getLastPickedNode();
   if ( !selectedSceneNode )
   {
      // don't change the selection if nothing has changed
      return;
   }

   StoryNodeInstance* nodeInstance = static_cast< StoryNodeInstance* >( selectedSceneNode );
   StoryNode* selectedStoryNode = nodeInstance->getStoryNode();

   // check if the selection has changed
   StoryNode* currentlySelectedNode = NULL;
   if ( m_selectedItem != NULL )
   {
      currentlySelectedNode = m_selectedItem->getNode();
   }

   if ( selectedStoryNode == currentlySelectedNode )
   {
      return;
   }

   if ( selectedStoryNode )
   {
      m_selectedItem = m_treeWidget->findItem( selectedStoryNode );
      m_treeWidget->setCurrentItem( m_selectedItem );
      editItem( m_selectedItem );
   }
   else
   {
      m_treeWidget->setCurrentItem( NULL );
      editItem( NULL );
   }
}

///////////////////////////////////////////////////////////////////////////////

void StoryEditor::onStoryTreePopupMenuShown( QTreeWidgetItem* clickedItem, QMenu& menu )
{
   StoryBrowserTreeItem* stItem = static_cast< StoryBrowserTreeItem* >( clickedItem );

   if ( stItem->isContainerNode() )
   {

      QAction* generateSceneSnapshotAction = new GenerateSceneSnapshotAction( &menu, &m_storyPlayer->gameWorld(), stItem );
      menu.addAction( generateSceneSnapshotAction );
   }
}

///////////////////////////////////////////////////////////////////////////////

void StoryEditor::onPlay( bool scenePlayed )
{
   m_playing = scenePlayed;

   if ( m_playing )
   {
      // set focus on the scene tab
      m_workAreaTabsManager->setCurrentIndex( StoryEditorTab_Scene );
   }
   
   updateStoryPlayerStatus();
}

///////////////////////////////////////////////////////////////////////////////

void StoryEditor::update( float timeElapsed )
{
   if ( !m_playing )
   {
      return;
   }

   // stop playing the scene as soon as the user presses ESC
   bool isStillPlaying = true;
   if ( m_sceneWidget->getUserInputController().isKeyPressed( VK_ESCAPE ) )
   {
      isStillPlaying = false;
   }
   else
   {
      // play the scene
      isStillPlaying = m_storyPlayer->execute();
   }

   if ( !isStillPlaying )
   {
      m_sceneWidget->playScene( isStillPlaying );
   }
}

///////////////////////////////////////////////////////////////////////////////

void StoryEditor::updateStoryPlayerStatus()
{
   // reset the player, if we stopped playing the story
   if ( !m_playing )
   {
      m_storyPlayer->reset();
   }

   // toggle back the preview
   freezePreview( m_playing );

   // toggle selection manager on/off ( we don't want the manager to work when we simulate the scene )
   m_sceneWidget->enableObjectsManipulator( !m_playing );
   m_sceneWidget->showToolbar( !m_playing );
   
   m_sceneWidget->setInputPolicy( m_playing ? IP_ALWAYS_RELATIVE : IP_RELATIVE_ON_DEMAND );
}

///////////////////////////////////////////////////////////////////////////////

void StoryEditor::freezePreview( bool freeze )
{
   LocalStack< StoryBrowserTreeItem* > items;
   items.push( static_cast< StoryBrowserTreeItem* >( m_treeWidget->topLevelItem( 0 ) ) );

   while( !items.empty() )
   {
      StoryBrowserTreeItem* item = items.pop();
      item->freezePreview( freeze );

      int count = item->childCount();
      for ( int i = 0; i < count; ++i )
      {
         StoryBrowserTreeItem* childItem = static_cast< StoryBrowserTreeItem* >( item->child( i ) );
         items.push( childItem );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

GenerateSceneSnapshotAction::GenerateSceneSnapshotAction( QObject* parent, Model* scene, StoryBrowserTreeItem* item )
   : QAction( "View", parent )
   , m_scene( scene )
   , m_item( item )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void GenerateSceneSnapshotAction::onTriggered()
{
   m_item->toggleSnapshot( m_scene );
}

///////////////////////////////////////////////////////////////////////////////

#include "BaseSceneEditor.h"
#include "core.h"
#include "tamyeditor.h"
#include "ToolsWindow.h"
#include "core-AppFlow.h"
#include "core-MVC.h"
#include "core-Renderer.h"
#include <QtWidgets\QMessageBox>
#include <QtWidgets\QSplitter>
#include <QtGui\QKeyEvent>

// tools
#include "EntitiesClipboard.h"
#include "core-MVC\EntityUtils.h"

// widgets
#include "TamySceneWidget.h"
#include "SelectionManager.h"
#include "SceneTreeWidget.h"
#include "Gizmo.h"

// mime
#include "ResourceMimeData.h"

// properties browser
#include "SelectedEntityPropertiesViewer.h"


///////////////////////////////////////////////////////////////////////////////

BaseSceneEditor::BaseSceneEditor( Resource& editedResource )
   : m_editedResource( editedResource )
   , m_root( NULL )
   , m_scene( NULL )
   , m_sceneWidget( NULL )
   , m_sceneTreeViewer( NULL )
   , m_actionSave( NULL )
   , m_clipboard( NULL )
   , m_toolBar( NULL )
{
   setFocusPolicy( Qt::StrongFocus );
}

///////////////////////////////////////////////////////////////////////////////

BaseSceneEditor::~BaseSceneEditor()
{
}

///////////////////////////////////////////////////////////////////////////////

void BaseSceneEditor::initializeBase( Entity* root, Model* scene )
{
   m_root = root;
   m_scene = scene;
}

///////////////////////////////////////////////////////////////////////////////

void BaseSceneEditor::onInitialize()
{
   TamyEditor& mainEditor = TamyEditor::getInstance();

   // setup the main layout
   m_mainLayout = new QVBoxLayout( this );
   m_mainLayout->setSpacing( 0 );
   m_mainLayout->setMargin( 0 );
   setLayout( m_mainLayout );

   // create and add the toolbar ( we need to do this before we add any other parts to the layout as we want it to stay on top )
   m_toolBar = new QToolBar( this );
   m_mainLayout->addWidget( m_toolBar );


   // add the splitter that will host the render window and the browsers frames
   {
      QSplitter* sceneViewSplitter = new QSplitter( Qt::Horizontal, this );
      m_mainLayout->addWidget( sceneViewSplitter, 1 );

      // extract the camera from the scene widget and set up the scene tree viewer
      // so that each time we select a node, the camera focuses on it
      Camera* camera = NULL;

      // add the scene preview
      {
         m_sceneWidget = new TamySceneWidget( sceneViewSplitter );
         camera = &m_sceneWidget->getCamera();
         m_sceneWidget->setScene( m_scene );
         m_sceneWidget->setDataEncoder( ResourceMimeData::create< Prefab >( m_droppedPrefab ) );
         sceneViewSplitter->addWidget( m_sceneWidget );
         connect( m_sceneWidget, SIGNAL( changed( const QMimeData* ) ), this, SLOT( onPrefabDropped( const QMimeData* ) ) );
         connect( m_sceneWidget, SIGNAL( scenePlayed( bool ) ), this, SLOT( onScenePlayed( bool ) ) );
      }

      // add the browsers
      {
         QSplitter* browsersSplitter = new QSplitter( Qt::Vertical, sceneViewSplitter );
         sceneViewSplitter->addWidget( browsersSplitter );
         sceneViewSplitter->setStretchFactor( 1, 0 );

         {
            // add the properties browser
            m_customPropertiesEditor = new SelectedEntityPropertiesViewer( browsersSplitter );
            m_customPropertiesEditor->setObjectName( ( m_editedResource.getFilePath().getRelativePath() + "/PropertiesEditor" ).c_str() );
            browsersSplitter->addWidget( m_customPropertiesEditor );

            // add the scene tree browser
            m_sceneTreeViewer = new SceneTreeWidget( browsersSplitter, *m_scene, camera, m_root );
            m_sceneTreeViewer->setObjectName( ( m_editedResource.getFilePath().getRelativePath() + "/SceneTreeWidget" ).c_str() );
            browsersSplitter->addWidget( m_sceneTreeViewer );
            connect( m_sceneTreeViewer, SIGNAL( onSceneTreeObjectSelected( SceneNode* ) ), this, SLOT( nodeSelected( SceneNode* ) ) );
            connect( m_sceneTreeViewer, SIGNAL( onSceneTreeSelectionCleaned() ), this, SLOT( selectionCleaned() ) );

            // connect the systems
            SelectionManager& selectionManager = m_sceneWidget->getSelectionMgr();
            selectionManager.attach( *m_customPropertiesEditor );
            selectionManager.attach( *m_sceneTreeViewer );
         }
      }

      // adjust splitter's position
      QList< int > splitterWindowSizes;
      splitterWindowSizes.push_back( height() * 0.75 );
      splitterWindowSizes.push_back( height() * 0.25 );
      sceneViewSplitter->setSizes( splitterWindowSizes );
   }


   // fill the toolbar
   {
      // resource management actions
      {
         m_actionSave = new QAction( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/saveFile.png" ) ), tr( "Save" ), m_toolBar );
         m_toolBar->addAction( m_actionSave );
         connect( m_actionSave, SIGNAL( triggered() ), this, SLOT( saveResource() ) );

         m_toolBar->addSeparator();
      }

   }

   // create the clipboard
   m_clipboard = new EntitiesClipboard();

   // listen to selection changes - if we select a GL2DProceduralLevel, and we're in 'procedurally generated edition' mode,
   // we'll snap it to the generation grid etc.
   m_sceneWidget->getSelectionMgr().attach( *this );

   // start listening to the scene in search for GL2DProceduralLevel components
   m_scene->attachListener( this );
   m_scene->pullStructure( this );
}

///////////////////////////////////////////////////////////////////////////////

void BaseSceneEditor::onDeinitialize( bool saveProgress )
{
   // stop listening to selection changes
   m_sceneWidget->getSelectionMgr().detach( *this );

   // stop listening to the scene
   m_scene->detachListener( this );

   m_customPropertiesEditor->deinitializeViewer();

   m_actionSave = NULL;
   m_toolBar = NULL;
   // delete the clipboard contents
   delete m_clipboard;
}

///////////////////////////////////////////////////////////////////////////////

void BaseSceneEditor::saveResource()
{
   onPreSaveResource();

   try
   {
      m_editedResource.saveResource();
   }
   catch ( std::exception& ex )
   {
      QMessageBox::warning( this, QString( "Error occurred while saving scene" ), ex.what(), QMessageBox::Ok );

      return;
   }
}

///////////////////////////////////////////////////////////////////////////////

void BaseSceneEditor::nodeSelected( SceneNode* node )
{
   SelectionManager& selectionMgr = m_sceneWidget->getSelectionMgr();

   selectionMgr.selectNode( *node );
}

///////////////////////////////////////////////////////////////////////////////

void BaseSceneEditor::selectionCleaned()
{
   m_sceneWidget->getSelectionMgr().resetSelection();
}

///////////////////////////////////////////////////////////////////////////////

void BaseSceneEditor::mousePressEvent( QMouseEvent* event )
{
   UserInputController& uic = m_sceneWidget->getUserInputController();

   if ( uic.isKeyPressed( VK_CONTROL ) && event->button() == Qt::RightButton )
   {
      // any Ctrl key + Right mouse button clicked in the scene rendering widget's area
      // opens a menu that allows to add a new element to the root of the scene
      Point mousePos = uic.getMousePos();
      QPoint pos( mousePos.x, mousePos.y );

      m_sceneTreeViewer->addToRoot( m_sceneWidget->mapToGlobal( pos ) );
   }

   ResourceEditor::mousePressEvent( event );
}

///////////////////////////////////////////////////////////////////////////////

void BaseSceneEditor::keyPressEvent( QKeyEvent* event )
{
   // input coming from other widgets embedded in the scene editor frame

   // check if any other keys were pressed and execute actions assigned to them
   if ( event->key() == Qt::Key_Delete )
   {
      deleteSelectedEntities();
   }
   else if ( event->matches( QKeySequence::Copy ) )
   {
      copySelectedEntities();
   }
   else if ( event->matches( QKeySequence::Paste ) )
   {
      pasteSelectedEntities();
   }
}

///////////////////////////////////////////////////////////////////////////////

void BaseSceneEditor::deleteSelectedEntities()
{
   SelectionManager& selectionManager = m_sceneWidget->getSelectionMgr();

   List< SceneNode* > selectedNodes = selectionManager.getSelectedNodes();
   selectionManager.resetSelection();

   // Analyze all selected entities and select only the ones that are the topmost parents
   // among the selected entities.
   // In other words - delete only the entities that don't have their parents ( immediate or not ) listed
   // as a selected entity.

   std::set< SceneNode* > allUniqueChildren;
   {
      List< SceneNode* > allChildren;
      for ( List< SceneNode* >::iterator it = selectedNodes.begin(); !it.isEnd(); ++it )
      {
         SceneNode* node = *it;
         if ( node->isA< Entity >() )
         {
            Entity* entity = static_cast< Entity* >( node );
            EntityUtils::collectChildren( entity, allChildren );
         }
      }

      // remove duplicates
      for ( List< SceneNode* >::iterator it = allChildren.begin(); !it.isEnd(); ++it )
      {
         SceneNode* child = *it;
         allUniqueChildren.insert( child );
      }
   }

   // remove all children from the list of the selected entities
   for ( List< SceneNode* >::iterator it = selectedNodes.begin(); !it.isEnd(); ++it )
   {
      SceneNode* node = *it;
      if ( allUniqueChildren.find( node ) != allUniqueChildren.end() )
      {
         // this is one of the children - it's gonna get deleted either way, so skip it
         it.markForRemoval();
      }
   }

   // now that we only have the topmost entities, it's time to remove them
   for ( List< SceneNode* >::iterator it = selectedNodes.begin(); !it.isEnd(); ++it )
   {
      SceneNode* node = *it;
      node->remove();
   }
}

///////////////////////////////////////////////////////////////////////////////

void BaseSceneEditor::copySelectedEntities()
{
   SelectionManager& selectionManager = m_sceneWidget->getSelectionMgr();

   const List< SceneNode* >& selectedNodes = selectionManager.getSelectedNodes();
   m_clipboard->copySelectedEntities( selectedNodes, m_root );
}

///////////////////////////////////////////////////////////////////////////////

void BaseSceneEditor::pasteSelectedEntities()
{
   // find the insertion spot ( the spot where the camera looks )
   Vector instantiationPos;
   if ( getEntityPlacementPoint( instantiationPos ) )
   {
      m_clipboard->pasteSelectedEntities( instantiationPos, m_root );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BaseSceneEditor::onScenePlayed( bool isPlayed )
{
   // Don't allow the scene to be saved when we're playing it.
   m_actionSave->setEnabled( !isPlayed );
}

///////////////////////////////////////////////////////////////////////////////

void BaseSceneEditor::onPrefabDropped( const QMimeData* mimeData )
{
   if ( m_droppedPrefab == NULL )
   {
      return;
   }

   // create the entity
   Vector instantiationPos;
   if ( getEntityPlacementPoint( instantiationPos ) == false )
   {
      return;
   }

   // create a new entity
   Entity* newEntity = new Entity();
   newEntity->setPrefab( m_droppedPrefab );

   Matrix posMtx;
   posMtx.setTranslation( instantiationPos );
   newEntity->setLocalMtx( posMtx );

   m_root->addChild( newEntity );
   m_droppedPrefab = NULL;
}

///////////////////////////////////////////////////////////////////////////////

bool BaseSceneEditor::getEntityPlacementPoint( Vector& outPos ) const
{
   Camera& camera = m_sceneWidget->getRenderer().getActiveCamera();

   Vector cameraPos, cameraLookVec;
   camera.getPosition( cameraPos );
   camera.getLookVec( cameraLookVec );

   Ray ray( cameraPos, cameraLookVec );

   Plane groundPlane;
   groundPlane.setFromPointNormal( Vector_ZERO, Vector_OZ );

   RaycastResult result;
   if ( !ray.rayCast( groundPlane, result ) )
   {
      // there's no intersection
      return false;
   }

   outPos.setMulAdd( ray.direction, result.m_dist, cameraPos );

   if ( m_sceneWidget->isSnapToGridEnabled() )
   {
      outPos[0] = snap( outPos[0], m_sceneWidget->getTranslationSnapStep() );
      outPos[1] = snap( outPos[1], m_sceneWidget->getTranslationSnapStep() );
   }
   outPos[2] = 0.0f;

   return true;
}

///////////////////////////////////////////////////////////////////////////////

void BaseSceneEditor::onNodeAdded( SceneNode* node )
{
}

///////////////////////////////////////////////////////////////////////////////

void BaseSceneEditor::onNodeRemoved( SceneNode* node )
{
}

///////////////////////////////////////////////////////////////////////////////

void BaseSceneEditor::resetContents( Model& model )
{
}

///////////////////////////////////////////////////////////////////////////////

void BaseSceneEditor::onSelectionChanged( SelectionManager* selectionMgr )
{
}

///////////////////////////////////////////////////////////////////////////////

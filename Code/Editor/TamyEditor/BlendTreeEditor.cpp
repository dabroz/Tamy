#include "BlendTreeEditor.h"
#include "core-AI.h"
#include "core\Assert.h"
#include <QtWidgets\QVBoxLayout>
#include <QtWidgets\QSplitter>
#include <QtWidgets\QToolBar>
#include "QPropertiesView.h"
#include "TamySceneWidget.h"
#include "DebugEntitiesManager.h"

// properties
#include "core\ReflectionProperty.h"

// animated entity
#include "core-AI\SkeletonComponent.h"

// lighting
#include "core\MatrixUtils.h"
#include "core-Renderer\Camera.h"
#include "core-Renderer\DirectionalLight.h"
#include "ext-RenderingPipeline\LocationRenderSettings.h"

// animations
#include "core-AI\BlendTreePlayer.h"
#include "core-AI\BlendTreeAnimation.h"

// blend tree related widgets
#include "BlendTreeWidget.h"
#include "BlendTreeStateMachineEditor.h"
#include "BlendTreeVariablesViewer.h"
#include "BlendTreeEventsViewer.h"
#include "BlendTreeEditorBottomFrame.h"
#include "OrbitingCameraController.h"
#include "TimetrackDebugger.h"


///////////////////////////////////////////////////////////////////////////////

BlendTreeEditor::BlendTreeEditor( BlendTree& blendTree )
   : m_blendTree( blendTree )
   , m_treeWidget( NULL )
   , m_sceneWidget( NULL )
   , m_bottomFrame( NULL )
   , m_nodeProperties( NULL )
   , m_variablesViewer( NULL )
   , m_eventsViewer( NULL )
   , m_timetrackDebugger( NULL )
   , m_selectedItem( NULL )
   , m_selectedStateMachine( NULL )
   , m_scene( NULL )
   , m_animatedEntity( NULL )
   , m_skeletonComponent( NULL )
   , m_blendTreePlayer( NULL )
   , m_attachedAnimPlayer( NULL )
   , m_camera( NULL )
   , m_cameraController( new OrbitingCameraController( 10.0f ) )
   , m_frontLight( NULL )
{
   ( static_cast< ReflectionObject& >( m_blendTree ) ).attachListener( *this );
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeEditor::~BlendTreeEditor()
{
   ( static_cast< ReflectionObject& >( m_blendTree ) ).detachListener( *this );

   delete m_cameraController;
   m_cameraController = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeEditor::onInitialize()
{
   // prepare the scene
   initScene();

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

   // the splitter that shares screen between the editor widgets and additional editors and debuggers
   // ( such as animation events editor, or a timeline view )
   QSplitter* topBottomMainSplitter = new QSplitter( Qt::Vertical, this );
   {
      mainLayout->addWidget( topBottomMainSplitter );

      // the splitter that stores the tree widget and the preview frame
      QSplitter* leftRightMainSplitter = new QSplitter( Qt::Horizontal, topBottomMainSplitter );
      {
         topBottomMainSplitter->addWidget( leftRightMainSplitter );
         buildLeftRightMainSplitter( leftRightMainSplitter );
      }

      // add the bottom frame
      m_bottomFrame = new BlendTreeEditorBottomFrame( topBottomMainSplitter, m_blendTree, *this );
      {
         topBottomMainSplitter->addWidget( m_bottomFrame );

         // create a time-track debugger
         m_timetrackDebugger = new TimetrackDebugger( m_bottomFrame );
         m_bottomFrame->addTab( m_timetrackDebugger, "Timetrack debugger" );
         m_timetrackDebugger->initialize( m_blendTreePlayer );
      }
         
      // adjust splitter's position
      {
         QList< int > splitterWindowSizes;
         splitterWindowSizes.push_back( height() * 0.75 );
         splitterWindowSizes.push_back( height() * 0.25 );
         topBottomMainSplitter->setSizes( splitterWindowSizes );
      }
   }

   // add self to an update loop - we're gonna be rotating the lights
   TimeController& timeController = TSingleton< TimeController >::getInstance();
   timeController.add( *this );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeEditor::onDeinitialize( bool saveProgress )
{
   // remove self from the update loop
   TimeController& timeController = TSingleton< TimeController >::getInstance();
   timeController.remove( *this );

   if ( m_timetrackDebugger )
   {
      m_timetrackDebugger->deinitialize();
      m_timetrackDebugger = NULL;
   }

   if ( m_blendTreePlayer )
   {
      m_blendTreePlayer->detachListener( m_treeWidget );
   }

   if ( m_sceneWidget )
   {
      m_sceneWidget->setScene( NULL );
      m_sceneWidget = NULL;

      m_scene->removeReference();
      m_scene = NULL;

      m_frontLight = NULL;
      m_camera = NULL;
      m_skeletonComponent = NULL;
      m_animatedEntity = NULL;
      m_blendTreePlayer = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeEditor::buildLeftRightMainSplitter( QSplitter* leftRightMainSplitter )
{
   // tree viewer
   QSplitter* treeAreaSplitter = new QSplitter( Qt::Vertical, leftRightMainSplitter );
   {
      leftRightMainSplitter->addWidget( treeAreaSplitter );

      m_treeWidget = new BlendTreeWidget( treeAreaSplitter, m_blendTree );
      {
         treeAreaSplitter->addWidget( m_treeWidget );
         connect( m_treeWidget, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( editItem( QTreeWidgetItem*, int ) ) );

         // listen to runtime state changes of the tree
         m_blendTreePlayer->attachListener( m_treeWidget );
      }


      // properties views
      QTabWidget* sideAreaTabs = new QTabWidget( treeAreaSplitter );
      {
         treeAreaSplitter->addWidget( sideAreaTabs );

         // tree properties
         QPropertiesView* treeProperties = new QPropertiesView();
         {
            treeProperties->set( m_blendTree );
            sideAreaTabs->addTab( treeProperties, tr( "Tree properties" ) );
         }

         // node properties
         m_nodeProperties = new QPropertiesView();
         {
            m_nodeProperties->set( m_blendTree.getRoot() );
            sideAreaTabs->addTab( m_nodeProperties, tr( "Node properties" ) );
         }

         // variables
         m_variablesViewer = new BlendTreeVariablesViewer( sideAreaTabs, m_blendTree, m_blendTreePlayer );
         sideAreaTabs->addTab( m_variablesViewer, "Variables" );

         // events
         m_eventsViewer = new BlendTreeEventsViewer( sideAreaTabs, m_blendTree, m_blendTreePlayer );
         sideAreaTabs->addTab( m_eventsViewer, "Events" );


         // activate the node properties tab by default
         sideAreaTabs->setCurrentWidget( m_nodeProperties );
      }

      // adjust splitter's sizes
      {
         QList< int > splitterWindowSizes;
         splitterWindowSizes.push_back( height() * 0.75 );
         splitterWindowSizes.push_back( height() * 0.25 );
         treeAreaSplitter->setSizes( splitterWindowSizes );
      }
   }

   // central area
   QTabWidget* centralAreaTabs = new QTabWidget( treeAreaSplitter );
   {
      leftRightMainSplitter->addWidget( centralAreaTabs );

      // state graph
      QFrame* stateGraphEditorFrame = new QFrame( centralAreaTabs );
      {
         m_stateGraphEditorLayout = new QVBoxLayout( stateGraphEditorFrame );
         m_stateGraphEditorLayout->setSpacing( 0 );
         m_stateGraphEditorLayout->setMargin( 0 );
         stateGraphEditorFrame->setLayout( m_stateGraphEditorLayout );

         centralAreaTabs->addTab( stateGraphEditorFrame, "State graph" );
      }

      // preview window
      m_sceneWidget = new TamySceneWidget( centralAreaTabs );
      {
         centralAreaTabs->addTab( m_sceneWidget, tr( "Preview" ) );

         // disable all debug rendering features
         m_sceneWidget->enableObjectsSelection( false );
         m_sceneWidget->setScene( m_scene );
         m_camera = &m_sceneWidget->getCamera();

         connect( m_sceneWidget, SIGNAL( scenePlayed( bool ) ), this, SLOT( onScenePlayed( bool ) ) );
      }

      // activate the preview tab by default
      centralAreaTabs->setCurrentWidget( m_sceneWidget );
   }

   // adjust splitter's position
   {
      QList< int > splitterWindowSizes;
      splitterWindowSizes.push_back( width() * 0.25 );
      splitterWindowSizes.push_back( width() * 0.75 );
      leftRightMainSplitter->setSizes( splitterWindowSizes );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeEditor::initScene()
{
   ASSERT( m_scene == NULL );

   m_scene = new Model();

   // setup lighting
   {
      m_frontLight = new Entity( "frontLight" );
      {
         m_frontLight->addChild( new DirectionalLight( "frontLight" ) );
         m_scene->addChild( m_frontLight );
         MatrixUtils::generateLookAtLH( Vector( 10000, 10000, 10000 ), Vector( 0, 0, 0 ), Vector_OZ, m_frontLight->accessLocalMtx() );
      }


      LocationRenderSettings* renderSettings = new LocationRenderSettings();
      {
         m_scene->addChild( renderSettings );
         renderSettings->m_ambientLight = Color( 0.5f, 0.5f, 0.5f, 1.0f );
      }
   }

   // create the animated entity
   {
      m_animatedEntity = new Entity( "animatedEntity" );

      // create a skeleton component 
      m_skeletonComponent = new SkeletonComponent();
      {
         m_animatedEntity->addChild( m_skeletonComponent );

         if ( m_blendTree.m_skeleton )
         {
            m_skeletonComponent->setSkeleton( m_blendTree.m_skeleton );
         }
      }

      // blend tree player ( not attached yet )
      m_blendTreePlayer = new BlendTreePlayer();
      {
         m_blendTreePlayer->setBlendTree( m_blendTree );
         m_animatedEntity->addChild( m_blendTreePlayer );
         m_attachedAnimPlayer = m_blendTreePlayer;
      }

      m_scene->addChild( m_animatedEntity );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeEditor::setAnimationPlayer( AnimationPlayer* animPlayer )
{
   if ( animPlayer == m_attachedAnimPlayer || !m_animatedEntity )
   {
      // nothing's changed
      return;
   }

   // remove the old player
   if ( m_attachedAnimPlayer )
   {
      if ( m_attachedAnimPlayer == m_blendTreePlayer )
      {
         // make sure we don't loose its instance - this one should stick through till the end
         m_blendTreePlayer->addReference();
      }
      m_animatedEntity->removeChild( m_attachedAnimPlayer );
   }

   if ( animPlayer )
   {
      m_attachedAnimPlayer = animPlayer;
   }
   else
   {
      m_attachedAnimPlayer = m_blendTreePlayer;
   }

   // attach the new player
   m_animatedEntity->addChild( m_attachedAnimPlayer );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeEditor::onObservedPropertyChanged( ReflectionProperty& property )
{
   // switch the skeleton representation
   if ( property.getName() == "m_skeleton" )
   {
      m_skeletonComponent->setSkeleton( m_blendTree.m_skeleton );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeEditor::onObservedObjectDeleted( ReflectionObject* deletedObject )
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeEditor::onSave()
{
   m_blendTree.saveResource();
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeEditor::onScenePlayed( bool isPlaying )
{
   m_eventsViewer->setPlayingMode( isPlaying );
   m_variablesViewer->setPlayingMode( isPlaying );
   m_timetrackDebugger->setPlayingMode( isPlaying );
   m_bottomFrame->setPlayingMode( isPlaying );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeEditor::editItem( QTreeWidgetItem* item, int column )
{
   if ( m_selectedItem == item )
   {
      // selection hasn't changed
      return;
   }
   m_selectedItem = item;

   BlendTreeWidgetItem* btItem = static_cast< BlendTreeWidgetItem* >( item );
   BlendTreeNode* btNode = btItem->getNode();

   // show node's properties
   btNode->viewProperties( *m_nodeProperties );

   // get the nearest state machine ( doesn't have to be the immediate parent, it can also be the node we've jsut clicked
   // or two parent nodes above ) and show its graph
   BlendTreeStateMachine* btHostStateMachine = getNearestStateMachine( btItem );
   if ( m_selectedStateMachine != btHostStateMachine )
   {
      m_selectedStateMachine = btHostStateMachine;

      // clear the state machine graph layout
      int workAreaWidgetsCount = m_stateGraphEditorLayout->count();
      if ( workAreaWidgetsCount >= 1 )
      {
         QLayoutItem* removedItem = m_stateGraphEditorLayout->takeAt( 0 );
         delete removedItem->widget();
         delete removedItem;
      }

      BlendTreeStateMachineEditor* editor = new BlendTreeStateMachineEditor( m_stateGraphEditorLayout->parentWidget(), m_selectedStateMachine );
      m_stateGraphEditorLayout->addWidget( editor );
   }

   // if it's an animation, create a dedicated animation editor
   m_bottomFrame->createEditor( btNode );
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeStateMachine* BlendTreeEditor::getNearestStateMachine( BlendTreeWidgetItem* btItem ) const
{
   BlendTreeStateMachine* stateMachine = NULL;
   while ( btItem != NULL || stateMachine == NULL )
   {
      BlendTreeNode* node = btItem->getNode();
      if ( node->isA< BlendTreeStateMachine >() )
      {
         stateMachine = static_cast< BlendTreeStateMachine* >( node );
         btItem = NULL;
      }
      else
      {
         btItem = static_cast< BlendTreeWidgetItem* >( btItem->parent() );
      }
   }

   return stateMachine;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeEditor::update( float timeElapsed )
{
   // update camera's position
   if ( timeElapsed > 0.0f )
   {
      // follow the character
      Transform entityTransform;
      entityTransform.set( m_animatedEntity->getGlobalMtx() );

      // calculate the desired camera transformation
      Transform desiredCameraTransform;
      {
         FastFloat yaw;
         Quaternion remainingRot;
         entityTransform.m_rotation.decompose( Vector_OZ, remainingRot, yaw );
      
         desiredCameraTransform.m_rotation.setAxisAngle( Vector_OZ, yaw );

         desiredCameraTransform.m_rotation.transform( Vector_OY, desiredCameraTransform.m_translation );
         desiredCameraTransform.m_translation.mul( FastFloat::fromFloat( 6.0f ) );
         desiredCameraTransform.m_translation.add( entityTransform.m_translation );

         static Vector cameraArmVertDisplacement( 0.0f, 0.0f, 3.0f );
         desiredCameraTransform.m_translation.add( cameraArmVertDisplacement );
      }
   }

   // update camera
   {
      Vector entityPos;
      m_animatedEntity->getPosition( entityPos );

      m_cameraController->update( timeElapsed, entityPos, *m_sceneWidget );
   }
}

///////////////////////////////////////////////////////////////////////////////

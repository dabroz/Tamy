#include "TamySceneWidget.h"
#include "core.h"
#include "core-AppFlow.h"
#include "core-MVC.h"
#include "core-Renderer.h"
#include "ext-RenderingPipeline.h"
#include "core-AI\AnimationWorld.h"
#include "core-Physics\PhysicsWorld.h"
#include <stdexcept>

// renderer implementation
#include "gl-Renderer.h"

// qt classes
#include <QEvent.h>
#include <QtWidgets\QApplication>
#include <QtWidgets\QDesktopWidget>
#include <QtWidgets\QVBoxLayout>
#include <QtWidgets\QToolButton>
#include <QtWidgets\QComboBox>
#include <QtCore\QSettings>

// utility classes
#include "MimeDataEncoder.h"
#include "tamyeditor.h"
#include "DebugEntitiesManager.h"
#include "SelectionManager.h"
#include "SceneObjectsManipulator.h"
#include "NavigationState.h"
#include "Gizmo.h"
#include "GizmoAxis.h"
#include "GizmoOperation.h"
#include "DebugMenuActions.h"
#include "SceneNodesTransformsCache.h"


///////////////////////////////////////////////////////////////////////////////

TamySceneWidget::TamySceneWidget( QWidget* parent, const char* settingsGroupName )
   : QFrame( parent )
{
   // initialize rendering pipeline paths
   {
      TamyEditor& mainEditor = TamyEditor::getInstance();
      QSettings& settings = mainEditor.getSettings();

      settings.beginGroup( settingsGroupName );
      settings.endGroup();
   }

   initialize();
}

///////////////////////////////////////////////////////////////////////////////

TamySceneWidget::~TamySceneWidget()
{
   deinitializeRenderingPipeline();

   // deinitialize animations world
   {
      delete m_animationWorld;
      m_animationWorld = NULL;
   }

   // deinitialize physics world
   {
      delete m_physicsWorld;
      m_physicsWorld = NULL;
   }

   // deinitialize transforms cache
   {
      delete m_transformsCache;
      m_transformsCache = NULL;
   }

   // delete debug mechanism components
   {
      delete m_debugEntitiesManager;
      m_debugEntitiesManager = NULL;

      m_debugScene->removeReference();
      m_debugScene = NULL;
   }

   // delete gizmo and its operations
   {
      m_gizmoScene->removeReference();
      m_gizmoScene = NULL;

      m_gizmo = NULL;

      delete m_gizmoOperations[Gizmo::GM_TRANSLATION][GizmoAxis::OX];
      delete m_gizmoOperations[Gizmo::GM_TRANSLATION][GizmoAxis::OY];
      delete m_gizmoOperations[Gizmo::GM_TRANSLATION][GizmoAxis::OZ];
      delete m_gizmoOperations[Gizmo::GM_TRANSLATION][GizmoAxis::UNKNOWN];

      delete m_gizmoOperations[Gizmo::GM_ROTATION][GizmoAxis::OX];
      delete m_gizmoOperations[Gizmo::GM_ROTATION][GizmoAxis::OY];
      delete m_gizmoOperations[Gizmo::GM_ROTATION][GizmoAxis::OZ];
      delete m_gizmoOperations[Gizmo::GM_ROTATION][GizmoAxis::UNKNOWN];
   }

   if ( m_selectionManager )
   {
      delete m_selectionManager;
      m_selectionManager = NULL;
   }

   if ( m_selectionGroup )
   {
      delete m_selectionGroup;
      m_selectionGroup = NULL;
   }

   // remove self from the time track
   delete m_sceneObjectsManipulator;
   m_sceneObjectsManipulator = NULL;

   delete m_localTimeController;
   m_localTimeController = NULL;

   delete m_userInputController;
   m_userInputController = NULL;

   delete m_dataEncoder;
   m_dataEncoder = NULL;

   m_camera = NULL;

   m_resMgr = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::initialize()
{
   setObjectName( "TamySceneWidget" );
   setAcceptDrops( true );

   m_toolbar = NULL;
   m_debugViewMenuButton = NULL;
   m_localTimeController = NULL;
   m_renderer = NULL;
   m_camera = NULL;
   m_scene = NULL;
   m_sceneRenderer = NULL;
   m_debugScene = NULL;
   m_debugEntitiesManager = NULL;
   m_animationWorld = NULL;
   m_physicsWorld = NULL;
   m_transformsCache = NULL;
   m_selectionManager = NULL;
   m_selectionGroup = NULL;
   m_sceneObjectsManipulator = NULL;
   m_resMgr = NULL;
   m_gizmoMode = Gizmo::GM_TRANSLATION;
   m_gizmo = NULL;
   m_gizmoScene = NULL;
   m_snapToGridEnabled = false;
   m_snapToGridAction = NULL;
   m_userInputController = NULL;
   m_inputPolicy = IP_RELATIVE_ON_DEMAND;
   m_relativeInputEnabled = false;
   m_dataEncoder = NULL;
   
   // focus policy
   setFocusPolicy( Qt::ClickFocus );

   // create a frame layout
   QVBoxLayout* frameLayout = new QVBoxLayout( this );
   frameLayout->setSpacing( 0 );
   frameLayout->setMargin( 0 );
   setLayout( frameLayout );

   // create a toolbar
   m_toolbar = new QToolBar( this );
   frameLayout->addWidget( m_toolbar );

   m_renderWindow = new QWidget( this );
   frameLayout->addWidget( m_renderWindow );

   // initialize the user input manager
   {
      m_userInputController = new TamySceneWidgetInputManager( m_renderWindow );
   }

   // initialize debug display mechanisms
   {
      m_debugScene = new Model();
      m_debugEntitiesManager = new DebugEntitiesManager( *m_debugScene );
   }

   // initialize animations world
   {
      m_animationWorld = new AnimationWorld();
      m_animationWorld->play( false );
   }

   // initialize physics world
   {
      m_physicsWorld = new PhysicsWorld( Vector( 0.0f, 0.0f, -9.81f ) );
      m_physicsWorld->enableSimulation( false );
   }

   // initialize transforms cache
   {
      m_transformsCache = new SceneNodesTransformsCache();
   }

   // initialize the widget
   initializeSubsystems( m_renderWindow );
   initializeToolbar( m_toolbar );
   initializeRenderingPipeline();

   // forward the events to the input controller
   installEventFilter( m_userInputController );
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::initializeSubsystems( QWidget* renderWindow )
{
   m_hWnd = (HWND)renderWindow->winId();

   setFocusPolicy( Qt::ClickFocus );

   // acquire the application's resources manager instance
   m_resMgr = &TSingleton< ResourcesManager >::getInstance();

   // create a renderer
   {
      // get the desktop resolution
      QDesktopWidget* desktop = QApplication::desktop();
      QRect screenGeometry = desktop->screenGeometry( 1 );
      uint desktopWidth = screenGeometry.width();
      uint desktopHeight = screenGeometry.height();

      GLRenderer* rendererImplementation = new GLRenderer( m_hWnd );
      m_renderer = new Renderer( rendererImplementation, desktopWidth, desktopHeight );
   }

   // setup the camera
   {
      m_camera = &m_renderer->getActiveCamera();    
      m_camera->accessLocalMtx().setRows( Quad_1000, Quad_0010, Quad_Neg_0100, Vector( 0, 0, 5 ) );
   }

   // create a selection manager
   m_selectionGroup = new EntitiesGroup();
   m_selectionManager = new SelectionManager( *m_selectionGroup );

   // initialize gizmo and its operations
   {
      m_gizmoScene = new Model();

      m_gizmo = new Gizmo( m_selectionManager, m_gizmoMode, Gizmo::GRF_Local );
      m_gizmo->show( false );
      m_gizmoScene->addChild( m_gizmo );

      m_gizmoOperations[Gizmo::GM_TRANSLATION][GizmoAxis::OX] = new GOTranslationAxis( GizmoAxis::OX );
      m_gizmoOperations[Gizmo::GM_TRANSLATION][GizmoAxis::OY] = new GOTranslationAxis( GizmoAxis::OY );
      m_gizmoOperations[Gizmo::GM_TRANSLATION][GizmoAxis::OZ] = new GOTranslationAxis( GizmoAxis::OZ );
      m_gizmoOperations[Gizmo::GM_TRANSLATION][GizmoAxis::UNKNOWN] = new GOTranslationPlane();

      m_gizmoOperations[Gizmo::GM_ROTATION][GizmoAxis::OX] = new GORotationAxis( GizmoAxis::OX );
      m_gizmoOperations[Gizmo::GM_ROTATION][GizmoAxis::OY] = new GORotationAxis( GizmoAxis::OY );
      m_gizmoOperations[Gizmo::GM_ROTATION][GizmoAxis::OZ] = new GORotationAxis( GizmoAxis::OZ );
      m_gizmoOperations[Gizmo::GM_ROTATION][GizmoAxis::UNKNOWN] = new GORotationPlane();

      setDefaultSnapSteps();
   }

   // setup the manipulator
   m_sceneObjectsManipulator = new SceneObjectsManipulator();
   m_sceneObjectsManipulator->initialize( *this );

   // create and setup the time controller
   TimeController& timeController = TSingleton< TimeController >::getInstance();
   m_localTimeController = new TimeController( timeController );
   {
      m_localTimeController->add( *m_debugEntitiesManager );
      m_localTimeController->add( *m_sceneObjectsManipulator );
      m_localTimeController->add( *this );
   }
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::initializeToolbar( QToolBar* toolBar )
{
   // object manipulation mode change commands
   {
      QActionGroup* actionGroup = new QActionGroup( toolBar );

      QAction* actionTranslate = new QAction( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/translate.png" ) ), tr( "Translate" ), toolBar );
      actionTranslate->setCheckable( true );
      toolBar->addAction( actionTranslate );
      actionGroup->addAction( actionTranslate );
      connect( actionTranslate, SIGNAL( triggered() ), this, SLOT( setNodeTranslateMode() ) );

      QAction* actionRotate = new QAction( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/rotate.png" ) ), tr( "Rotate" ), toolBar );
      actionRotate->setCheckable( true );
      toolBar->addAction( actionRotate );
      actionGroup->addAction( actionRotate );
      connect( actionRotate, SIGNAL( triggered() ), this, SLOT( setNodeRotateMode() ) );
      actionTranslate->setChecked( true );

      QComboBox* referenceSystemSelection = new QComboBox( toolBar );
      referenceSystemSelection->addItem( "Local" );
      referenceSystemSelection->addItem( "World" );
      referenceSystemSelection->setMinimumWidth( 50 );
      toolBar->addWidget( referenceSystemSelection );
      connect( referenceSystemSelection, SIGNAL( currentIndexChanged( const QString& ) ), this, SLOT( onReferenceSystemChanged( const QString& ) ) );

      QComboBox* cameraRotationModeSelection = new QComboBox( toolBar );
      cameraRotationModeSelection->addItem( "Orbit" );
      cameraRotationModeSelection->addItem( "Free" );
      cameraRotationModeSelection->setMinimumWidth( 50 );
      toolBar->addWidget( cameraRotationModeSelection );
      connect( cameraRotationModeSelection, SIGNAL( currentIndexChanged( const QString& ) ), this, SLOT( onCameraRotationModeChanged( const QString& ) ) );

      toolBar->addSeparator();

      m_snapToGridAction = new QAction( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/snapToGridOn.png" ) ), tr( "Snap to grid" ), toolBar );
      toolBar->addAction( m_snapToGridAction );
      actionGroup->addAction( m_snapToGridAction );
      connect( m_snapToGridAction, SIGNAL( triggered() ), this, SLOT( toggleSnapToGrid() ) );
      actionTranslate->setChecked( false );

      // snap to grid should be on by default
      toggleSnapToGrid();


      toolBar->addSeparator();
   }

   // debug stuff
   {
      // debuggable entities
      {
         m_debugViewMenuButton = new QToolButton( toolBar );
         m_debugViewMenuButton->setIcon( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/debugMenu.png" ) ) );
         m_debugViewMenuButton->setPopupMode( QToolButton::MenuButtonPopup );
         toolBar->addWidget( m_debugViewMenuButton );

         QMenu *debugMenu = new QMenu( m_debugViewMenuButton );
         m_debugViewMenuButton->setMenu( debugMenu );

         {
            ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
            TReflectionEnum< DebugFeature >* debugFeatureEnum = static_cast< TReflectionEnum< DebugFeature >* >( typesRegistry.find< DebugFeature >() );

            std::vector< std::string > debugFeatureEnumIds;
            debugFeatureEnum->getEnumerators( debugFeatureEnumIds );

            unsigned int count = debugFeatureEnumIds.size();
            for ( unsigned int i = 0; i < count; ++i )
            {
               DebugEntitiesAction* action = new DebugEntitiesAction( debugFeatureEnumIds[i].c_str(), ( DebugFeature ) i, *m_debugEntitiesManager, debugMenu );
               debugMenu->addAction( action );
            }
         }
      }

      // render passes selection
      {
         QComboBox* renderPassSelection = new QComboBox( toolBar );
         
         renderPassSelection->setMinimumWidth( 50 );
         toolBar->addWidget( renderPassSelection );
         connect( renderPassSelection, SIGNAL( currentIndexChanged( const QString& ) ), this, SLOT( onRenderPassSelected( const QString& ) ) );
         {
            ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
            TReflectionEnum< RenderPassSelection >* renderPassEnum = static_cast< TReflectionEnum< RenderPassSelection >* >( typesRegistry.find< RenderPassSelection >() );

            std::vector< std::string > renderPassEnumIds;
            renderPassEnum->getEnumerators( renderPassEnumIds );

            unsigned int count = renderPassEnumIds.size();
            for ( unsigned int i = 0; i < count; ++i )
            {
               renderPassSelection->addItem( renderPassEnumIds[i].c_str() );
            }
         }
      }

      toolBar->addSeparator();
   }

   // animations
   {
      m_playAnimationsAction = new QAction( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/play.png" ) ), tr( "Play" ), toolBar );
      toolBar->addAction( m_playAnimationsAction );
      connect( m_playAnimationsAction, SIGNAL( triggered() ), this, SLOT( onPlayAnimations() ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::initializeRenderingPipeline()
{
   m_sceneRenderer = new DeferredRenderingMechanism();
   m_renderer->setMechanism( m_sceneRenderer );

   // start updating the scenes
   TransformsManagementSystem& transformsMgr = TSingleton< TransformsManagementSystem >::getInstance();
   transformsMgr.addScene( m_debugScene );
   transformsMgr.addScene( m_gizmoScene );

   // setup the debug scene
   m_sceneRenderer->assignScene( FRS_Debug, m_debugScene );
   m_sceneRenderer->assignScene( FRS_Gizmo, m_gizmoScene );
   m_sceneRenderer->assignSelectionGroup( m_selectionGroup );
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::deinitializeRenderingPipeline()
{
   // stop updating the scenes
   TransformsManagementSystem& transformsMgr = TSingleton< TransformsManagementSystem >::getInstance();
   transformsMgr.removeScene( m_debugScene );
   transformsMgr.removeScene( m_gizmoScene );


   // detach the views
   if ( m_scene )
   {
      m_scene->detachListener( m_transformsCache );
      m_scene->detachListener( m_physicsWorld );
      m_scene->detachListener( m_animationWorld );
      m_scene->detachListener( m_debugEntitiesManager );
      m_scene->detachListener( m_selectionManager );

      m_scene->removeReference();
      m_scene = NULL;
   }

   m_renderer->setMechanism( NULL );
   m_sceneRenderer = NULL;
   delete m_renderer; m_renderer = NULL;

   // don't release the selection renderer - we're managing that one on our own
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::setDataEncoder( MimeDataEncoder* dataEncoder )
{
   delete m_dataEncoder;
   m_dataEncoder = dataEncoder;
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::saveUI( QSettings& settings ) const
{
   settings.beginGroup( "TamySceneWidget" );

   // setup the camera
   {
      const Matrix& mtx = m_camera->getLocalMtx();
     
      QByteArray mtxByteArr;
      for ( int rowIdx = 0; rowIdx < 4; ++rowIdx )
      {
         const Vector& row = mtx.getRow( rowIdx );

         for ( int colIdx = 0; colIdx < 4; ++colIdx )
         {
            const float val = row.getComponent( colIdx ).getFloat();
            mtxByteArr.append( (const char*)&val, sizeof( float ) );
         }
      }
      settings.setValue( "cameraPos", mtxByteArr );
   }

   settings.endGroup();
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::loadUI( QSettings& settings ) 
{
   settings.beginGroup( "TamySceneWidget" );

   // setup the camera
   {
      QByteArray mtxByteArr = settings.value( "cameraPos" ).toByteArray();
      if ( mtxByteArr.size() == sizeof( float ) * 16 )
      {
         ALIGN_16 float camMtxVals[16];
         int dataOffset = 0;
         int cellIdx = 0;
         for ( int rowIdx = 0; rowIdx < 4; ++rowIdx )
         {
            for ( int colIdx = 0; colIdx < 4; ++colIdx, ++cellIdx )
            {
               char* valPtr = (char*)&(camMtxVals[cellIdx]);

               for ( int i = 0; i < sizeof( float ); ++i, ++valPtr, ++dataOffset )
               {
                  *valPtr = (const char)(mtxByteArr[dataOffset]);
               }

            }
         }

         Matrix camMtx;
         camMtx.set( camMtxVals );
         m_camera->setLocalMtx( camMtx );
      }
   }

   settings.endGroup();
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::setScene( Model* scene )
{
   TransformsManagementSystem& transformsMgr = TSingleton< TransformsManagementSystem >::getInstance();

   // detach the views
   if ( m_scene )
   {
      // stop updating the previous scene
      transformsMgr.removeScene( m_scene );

      m_sceneRenderer->assignScene( FRS_Main, NULL );
      m_scene->detachListener( m_debugEntitiesManager );
      m_scene->detachListener( m_selectionManager );
      m_scene->detachListener( m_animationWorld );
      m_scene->detachListener( m_physicsWorld );
      m_scene->detachListener( m_transformsCache );

      m_scene->removeReference();
   }
   
   m_scene = scene;

   if ( m_scene )
   {
      m_scene->addReference();

      m_sceneRenderer->assignScene( FRS_Main, m_scene );
      m_scene->attachListener( m_debugEntitiesManager );
      m_scene->attachListener( m_selectionManager );
      m_scene->attachListener( m_animationWorld );
      m_scene->attachListener( m_physicsWorld );
      m_scene->attachListener( m_transformsCache );

      // start updating the new scene
      transformsMgr.addScene( m_scene );
   }
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::onPlayAnimations()
{
   bool isPlaying = m_animationWorld->isPlaying();
   isPlaying = !isPlaying;
   playScene( isPlaying );
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::playScene( bool play )
{
   const bool isPlaying = m_animationWorld->isPlaying();
   if ( isPlaying == play )
   {
      /// nothing changes
   }

   m_transformsCache->cacheTransforms( play );
   m_animationWorld->play( play );
   m_physicsWorld->enableSimulation( play );

   // adjust an icon on the action button
   {
      if ( play )
      {
         m_playAnimationsAction->setIcon( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/stop.png" ) ) );
      }
      else
      {
         m_playAnimationsAction->setIcon( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/play.png" ) ) );
      }
   }

   // notify listeners
   emit scenePlayed( play );
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::moveEvent( QMoveEvent* event )
{
   uint w = m_renderWindow->width();
   uint h = m_renderWindow->height();

   m_renderer->resizeViewport( w, h );
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::resizeEvent( QResizeEvent* event )
{
   uint w = m_renderWindow->width();
   uint h = m_renderWindow->height();

   m_renderer->resizeViewport( w, h );
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::showEvent( QShowEvent* event )
{
   // reset the keys buffer
   m_userInputController->resetInput();

   // restart the transform updates
   TransformsManagementSystem& transformsMgr = TSingleton< TransformsManagementSystem >::getInstance();
   transformsMgr.addScene( m_debugScene );
   transformsMgr.addScene( m_gizmoScene );
   transformsMgr.addScene( m_scene );

   // pass the event higher
   QFrame::showEvent( event );
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::hideEvent( QHideEvent* event )
{
   // reset the keys buffer
   m_userInputController->resetInput();

   // stop the transform updates
   TransformsManagementSystem& transformsMgr = TSingleton< TransformsManagementSystem >::getInstance();
   transformsMgr.removeScene( m_debugScene );
   transformsMgr.removeScene( m_gizmoScene );
   transformsMgr.removeScene( m_scene );

   // pass the event higher
   QFrame::hideEvent( event );
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::focusInEvent( QFocusEvent* event )
{
   QFrame::focusInEvent( event );

   m_userInputController->setEnabled( true );
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::focusOutEvent( QFocusEvent* event )
{
   m_userInputController->setEnabled( false );

   // pass the event higher
   QFrame::focusOutEvent( event );
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::localToViewport( const Point& screenPt, Vector& viewportPt ) const
{
   float w = (float)m_renderWindow->width();
   float h = (float)m_renderWindow->height();
   float x = (float)( ( screenPt.x < (int)w ) ? screenPt.x : w );
   float y = (float)( ( screenPt.y < (int)h ) ? screenPt.y : h );

   if ( x < 0 ) x = 0;
   if ( y < 0 ) y = 0;

   viewportPt[0] = ( x / ( w * 0.5f ) ) - 1;
   viewportPt[1] = 1 - ( y / ( h * 0.5f ) );
   viewportPt[2] = 0.0f;
   viewportPt[3] = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::setNodeTranslateMode()
{
   m_gizmoMode = Gizmo::GM_TRANSLATION;

   if ( m_gizmo )
   {
      m_gizmo->setMode( m_gizmoMode );
   }
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::setNodeRotateMode()
{
   m_gizmoMode = Gizmo::GM_ROTATION;
   if ( m_gizmo )
   {
      m_gizmo->setMode( m_gizmoMode );
   }
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::toggleSnapToGrid()
{
   m_snapToGridEnabled = !m_snapToGridEnabled;

   updateSnapToGrid();
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::updateSnapToGrid()
{
   bool isEnabled = m_snapToGridEnabled;

   if ( isEnabled )
   {
      m_snapToGridAction->setIcon( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/snapToGridOn.png" ) ) );
   }
   else
   {
      m_snapToGridAction->setIcon( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/snapToGridOff.png" ) ) );
   }

   // translation
   for ( uint axisIdx = 0; axisIdx < 4; ++ axisIdx )
   {
      GizmoOperation* operation = m_gizmoOperations[Gizmo::GM_TRANSLATION][axisIdx];
      operation->setSnapToGrid( isEnabled ? m_snapTranslationStep : 0.0f );
   }

   // rotation
   for ( uint axisIdx = 0; axisIdx < 4; ++axisIdx )
   {
      GizmoOperation* operation = m_gizmoOperations[Gizmo::GM_ROTATION][axisIdx];
      operation->setSnapToGrid( isEnabled ? m_snapRotationStep : 0.0f );
   }
}

///////////////////////////////////////////////////////////////////////////////

GizmoOperation* TamySceneWidget::getGizmoOperation( GizmoAxis* axis ) const
{
   byte modeIdx = (byte)m_gizmoMode;
   byte axisIdx = (byte)( axis != NULL ? axis->getAxisId() : GizmoAxis::UNKNOWN );
   
   GizmoOperation* operation = m_gizmoOperations[modeIdx][axisIdx];
   return operation;
}

///////////////////////////////////////////////////////////////////////////////

Gizmo* TamySceneWidget::getGizmo()
{
   return m_gizmo;
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::setSnapSteps( float translation, float rotation )
{
   m_snapTranslationStep = translation;
   m_snapRotationStep = rotation;
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::setDefaultSnapSteps()
{
   m_snapTranslationStep = 1.0f;
   m_snapRotationStep = 15.0f;
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::enableObjectsManipulator( bool flag )
{
   m_sceneObjectsManipulator->enable( flag );
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::enableObjectsSelection( bool enable )
{
   NavigationState& navigationState = m_sceneObjectsManipulator->getState< NavigationState >();
   navigationState.enableObjectsSelection( enable );
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::enableDebugView( bool enable )
{
   m_debugEntitiesManager->setEnabled( enable );
   m_debugViewMenuButton->setEnabled( enable );
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::showToolbar( bool show )
{
   if ( show )
   {
      m_toolbar->show();
   }
   else
   {
      m_toolbar->hide();
   }
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::setRelativeMouseMovement( bool enable )
{
   // We're storing the input mode here, 'cause in case we switch the policy to IP_ALWAYS_RELATIVE,
   // we're loosing info about what mode we were previously working in, and if we then want to switch
   // back to IP_RELATIVE_ON_DEMAND, we're gonna have to store that additional info somewhere outside the
   // class. 
   // This way we can change the two options ( the policy and the mode ) independent of one another and be sure
   // that if we apply a proper policy, the desired input mode will be activated.
   m_relativeInputEnabled = enable;

   switch( m_inputPolicy )
   {
   case IP_RELATIVE_ON_DEMAND:
      {
         m_userInputController->setRelativeMouseMovement( enable );
         break;
      }

   case IP_ALWAYS_RELATIVE:
      {
         m_userInputController->setRelativeMouseMovement( true );
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::setInputPolicy( TamySceneWidgetInputPolicy policy )
{
   m_inputPolicy = policy;

   // update controller's input mode
   setRelativeMouseMovement( m_relativeInputEnabled );
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::onReferenceSystemChanged( const QString& referenceSystemId )
{
   if ( referenceSystemId == "Local" )
   {
      m_gizmo->setReferenceFrame( Gizmo::GRF_Local );
   }
   else if ( referenceSystemId == "World" )
   {
      m_gizmo->setReferenceFrame( Gizmo::GRF_World );
   }
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::onCameraRotationModeChanged( const QString& rotationModeId )
{
   NavigationState& navigationState = m_sceneObjectsManipulator->getState< NavigationState >();
   if ( rotationModeId == "Orbit" )
   {
      navigationState.setCameraControlMode( NavigationState::CCM_ORBIT );
   }
   else if ( rotationModeId == "Free" )
   {
      navigationState.setCameraControlMode( NavigationState::CCM_FREE );
   }
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::onRenderPassSelected( const QString& renderPassId )
{
   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   TReflectionEnum< RenderPassSelection >* renderPassEnum = static_cast< TReflectionEnum< RenderPassSelection >* >( typesRegistry.find< RenderPassSelection >() );

   RenderPassSelection passId = ( RenderPassSelection ) renderPassEnum->getValue( renderPassId.toStdString() );
   if ( m_sceneRenderer )
   {
      m_sceneRenderer->selectRenderPass( passId );
   }

}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::update( float timeElapsed )
{
   m_userInputController->update( timeElapsed );
   m_gizmo->update( timeElapsed );
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::dragEnterEvent( QDragEnterEvent *event )
{
   const QMimeData* mimeData = event->mimeData();
   if ( m_dataEncoder && m_dataEncoder->canAccept( *mimeData ) )
   {
      setBackgroundRole( QPalette::Highlight );
      event->acceptProposedAction();
   }
   else
   {
      QFrame::dragEnterEvent( event );
   }
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::dragMoveEvent( QDragMoveEvent *event )
{
   const QMimeData* mimeData = event->mimeData();
   if ( m_dataEncoder && m_dataEncoder->canAccept( *mimeData ) )
   {
      event->acceptProposedAction();
   }
   else
   {
      // maybe the scene will use it
      QFrame::dragMoveEvent( event );
   }
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::dropEvent( QDropEvent *event )
{
   const QMimeData* mimeData = event->mimeData();
   if ( m_dataEncoder && m_dataEncoder->canAccept( *mimeData ) )
   {
      m_dataEncoder->load( *mimeData );

      setBackgroundRole( QPalette::Light );
      event->acceptProposedAction();

      emit changed();
   }
   else
   {
      // maybe the scene will use it
      QFrame::dropEvent( event );
   }
}

///////////////////////////////////////////////////////////////////////////////

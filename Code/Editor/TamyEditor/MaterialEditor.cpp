#include "MaterialEditor.h"
#include "core.h"
#include "core-Renderer.h"
#include "ext-RenderingPipeline.h"
#include "core-MVC\Prefab.h"
#include "MaterialLayout.h"
#include "QPropertiesView.h"
#include "tamyeditor.h"
#include "TamySceneWidget.h"
#include "DebugEntitiesManager.h"
#include "TextEditWidget.h"
#include "ShaderSyntaxHighlighter.h"
#include "GraphView.h"
#include "FilesystemTree.h"
#include <QtWidgets\QSplitter>
#include <QtWidgets\QScrollArea>
#include <QtWidgets\QVBoxLayout>
#include <QtWidgets\QToolButton>
#include <QtWidgets\QGraphicsView>
#include <QtWidgets\QTabWidget>
#include <QtWidgets\QListWidget>
#include "Defines.h"
#include <windows.h>
#include "QPropertiesView.h"

// additional resources
#include "MaterialRoutine.h"

// mime
#include "ResourceMimeData.h"


///////////////////////////////////////////////////////////////////////////////

const char* g_routineLibraryDirs[LAST_SHADER_ROUTINE] =
{
   "/Editor/Materials/Library/Vertex/",
   "/Editor/Materials/Library/Tess_Ctrl/",
   "/Editor/Materials/Library/Tess_Eval/",
   "/Editor/Materials/Library/Geometry/",
   "/Editor/Materials/Library/Fragment/",
};

const char* g_routineLibraryDefaultNames[LAST_SHADER_ROUTINE] =
{
   "vertexLib",
   "tessCtrlLib",
   "tessEvalLib",
   "geometryLib",
   "fragmentLib",
};

///////////////////////////////////////////////////////////////////////////////

MaterialEditor::MaterialEditor( Material& material )
   : m_material( material )
   , m_renderingPipeline( NULL )
   , m_activeRoutineIdx( -1 )
   , m_droppedRoutineLib( NULL )
   , m_synchronizationRootNode( NULL )
   , m_codePreview( NULL )
{
   memset( m_graphView, NULL, sizeof( QGraphicsView* ) * LAST_SHADER_ROUTINE );
   memset( m_materialLayout, NULL, sizeof( MaterialLayout* ) * LAST_SHADER_ROUTINE );
}

///////////////////////////////////////////////////////////////////////////////

MaterialEditor::~MaterialEditor()
{
}

///////////////////////////////////////////////////////////////////////////////

void MaterialEditor::onInitialize()
{
   // setup the UI
   QVBoxLayout* layout = new QVBoxLayout( this );
   layout->setMargin( 0 );
   layout->setSpacing( 0 );
   setLayout( layout );

   QSplitter* mainHorizontalSplitter = new QSplitter( Qt::Horizontal, this );
   {
      layout->addWidget( mainHorizontalSplitter );

      setupRightSide( mainHorizontalSplitter );
      setupLeftSide( mainHorizontalSplitter );

      // adjust splitter's position
      QList< int > splitterWindowSizes;
      splitterWindowSizes.push_back( height() * 0.75 );
      splitterWindowSizes.push_back( height() * 0.25 );
      mainHorizontalSplitter->setSizes( splitterWindowSizes );
   }

   // start listening to the graphs
   m_activeRoutineIdx = 0;
   {
      MaterialProfile* newProfile = m_material.accessProfile();
      activateProfile( newProfile );

      // update the properties tab
      m_materialProperties->set( *newProfile );
   }

   // build the material
   buildMaterial();

   // reset some of the members
   m_droppedRoutineLib = NULL;
   m_synchronizationRootNode = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void MaterialEditor::setupLeftSide( QSplitter* graphSplitter )
{
   QSplitter* previewsAreaSplitter = new QSplitter( Qt::Vertical, graphSplitter );
   {
      graphSplitter->addWidget( previewsAreaSplitter );

      setupMainPreviewWindow( previewsAreaSplitter );
      setupBrowsers( previewsAreaSplitter );

      // adjust splitter's position
      QList< int > splitterWindowSizes;
      splitterWindowSizes.push_back( height() * 0.25 );
      splitterWindowSizes.push_back( height() * 0.75 );
      previewsAreaSplitter->setSizes( splitterWindowSizes );
   }
}

///////////////////////////////////////////////////////////////////////////////

void MaterialEditor::setupRightSide( QSplitter* graphSplitter )
{
   QSplitter* splitter = new QSplitter( Qt::Vertical, graphSplitter );
   graphSplitter->addWidget( splitter );

   QFrame* graphEditionFrame = new QFrame( splitter );
   {
      splitter->addWidget( graphEditionFrame );

      QVBoxLayout* layout = new QVBoxLayout( graphEditionFrame );
      graphEditionFrame->setLayout( layout );
      layout->setSpacing( 0 );
      layout->setMargin( 0 );

      // the toolbar
      QToolBar* mainSceneToolbar = new QToolBar( graphEditionFrame );
      {
         layout->addWidget( mainSceneToolbar );

         QAction* actionSave = new QAction( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/saveFile.png" ) ), tr( "Save" ), mainSceneToolbar );
         actionSave->setShortcut( QKeySequence( tr( "Ctrl+S" ) ) );
         mainSceneToolbar->addAction( actionSave );
         connect( actionSave, SIGNAL( triggered() ), this, SLOT( save() ) );

         QAction* actionExport = new QAction( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/export.png" ) ), tr( "Export to library" ), mainSceneToolbar );
         actionExport->setShortcut( QKeySequence( tr( "Ctrl+E" ) ) );
         mainSceneToolbar->addAction( actionExport );
         connect( actionExport, SIGNAL( triggered() ), this, SLOT( exportToLibrary() ) );

         mainSceneToolbar->addSeparator();

         QAction* actionBuild = new QAction( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/play.png" ) ), tr( "Build material" ), mainSceneToolbar );
         mainSceneToolbar->addAction( actionBuild );
         connect( actionBuild, SIGNAL( triggered() ), this, SLOT( buildMaterial() ) );

         QAction* actionClear = new QAction( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/clear.png" ) ), tr( "Clear routine" ), mainSceneToolbar );
         actionClear->setShortcut( QKeySequence( tr( "Ctrl+X" ) ) );
         mainSceneToolbar->addAction( actionClear );
         connect( actionClear, SIGNAL( triggered() ), this, SLOT( clearCurrentRoutine() ) );
      }

      // graph editors window
      QTabWidget* shaderBlockEditors = new QTabWidget( graphEditionFrame );
      {
         shaderBlockEditors->setObjectName( "MaterialEditor/shaderBlockEditors" );
         layout->addWidget( shaderBlockEditors );
         connect( shaderBlockEditors, SIGNAL( currentChanged( int ) ), this, SLOT( onRoutineTabSelected( int ) ) );

         for ( uint i = 0; i < LAST_SHADER_ROUTINE; ++i )
         {
            m_graphView[i] = new GraphView( shaderBlockEditors );
            m_graphView[i]->setDataEncoder( ResourceMimeData::create< MaterialRoutine >( m_droppedRoutineLib ) );
            connect( m_graphView[i], SIGNAL( changed( const QMimeData* ) ), this, SLOT( onLibraryDropped( const QMimeData* ) ) );

            shaderBlockEditors->addTab( m_graphView[i], g_shaderRoutineNames[i] );
         }
      }

   }

   // shader code preview frame
   m_codePreview = new TextEditWidget( splitter );
   {
      splitter->addWidget( m_codePreview );
      QFont font( "Courier", 12 );
      m_codePreview->setFont( font );
      m_codePreview->setSyntaxHighlighter( new ShaderSyntaxHighlighter() );
   }

}

///////////////////////////////////////////////////////////////////////////////

void MaterialEditor::setupMainPreviewWindow( QSplitter* previewsAreaSplitter )
{
   m_sceneWidget = new TamySceneWidget( previewsAreaSplitter );
   m_sceneWidget->setObjectName( "MaterialEditor/TamySceneWidget" );
   m_sceneWidget->showToolbar( false );
   m_sceneWidget->enableDebugView( false );
   m_sceneWidget->enableObjectsManipulator( false );
   previewsAreaSplitter->addWidget( m_sceneWidget );

   setupTestScene();
}

///////////////////////////////////////////////////////////////////////////////

void MaterialEditor::setupBrowsers( QSplitter* previewsAreaSplitter )
{
   QTabWidget* browsersTabMgr = new QTabWidget( previewsAreaSplitter );
   browsersTabMgr->setObjectName( "MaterialEditor/browsersTabMgr" );

   previewsAreaSplitter->addWidget( browsersTabMgr );

   // libraries browser
   m_librariesBrowser = new FilesystemTree( browsersTabMgr, "MaterialEditor/browsersTabMgr", "/Editor/Materials/Library/" );
   {
      browsersTabMgr->addTab( m_librariesBrowser, "Libraries" );

      connect( m_librariesBrowser, SIGNAL( popupMenuRequest( QTreeWidgetItem*, QMenu& ) ), this, SLOT( showLibrariesBrowserPopupMenu( QTreeWidgetItem*, QMenu& ) ) );
   }

   // material profile properties
   m_materialProperties = new QPropertiesView();
   {
      browsersTabMgr->addTab( m_materialProperties, "Properties" );
   }
}

///////////////////////////////////////////////////////////////////////////////

void MaterialEditor::onDeinitialize( bool saveProgress )
{
   if ( saveProgress )
   {
      save();
   }
   
   MaterialProfile* profile = m_material.accessProfile();
   deactivateProfile( profile );

   m_sceneWidget->setScene( NULL );
   m_previewScene->removeReference();
   m_previewScene = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void MaterialEditor::activateProfile( MaterialProfile* profile )
{
   for ( uint routineIdx = 0; routineIdx < LAST_SHADER_ROUTINE; ++routineIdx )
   {
      ShaderRoutine routine = ( ShaderRoutine ) routineIdx;

      // create layouts for our material's sub-routines
      m_materialLayout[routineIdx] = new MaterialLayout( *profile->getGraph( routine ), m_activeRoutineIdx );
      switch ( routine )
      {
         case SHADER_VERTEX:
         {
            m_materialLayout[routineIdx]->registerBlockCallback< MNInputStruct, GNAAddVertexShaderInputSockets >();
            m_materialLayout[routineIdx]->registerBlockCallback< MNOutputStruct, GNAAddVertexShaderOutputSockets >();
            break;
         }

         case SHADER_FRAGMENT:
         {
            m_materialLayout[routineIdx]->registerBlockCallback< MNInputStruct, GNAAddFragmentShaderInputSockets >();
            m_materialLayout[routineIdx]->registerBlockCallback< MNOutputStruct, GNAAddFragmentShaderOutputSockets >();
            break;
         }

      }


      m_graphView[routineIdx]->setScene( m_materialLayout[routineIdx] );

      // pull its initial structure and start listening to its changes
      profile->attachGraphListener( routine, this );
      profile->pullGraphStructure( routine, this );
   }

   // create the missing entry point structures
   createProfileEntryPoints();

   // update the code preview
   onRoutineTabSelected( m_activeRoutineIdx );
}

///////////////////////////////////////////////////////////////////////////////

void MaterialEditor::deactivateProfile( MaterialProfile* profile )
{
   for ( uint routineIdx = 0; routineIdx < LAST_SHADER_ROUTINE; ++routineIdx )
   {
      ShaderRoutine routine = ( ShaderRoutine ) routineIdx;

      // so.. stop listening to the graph...
      profile->detachGraphListener( routine, this );

      // .. to its nodes
      const std::vector< MaterialNode* >& nodes = profile->getNodes( routine );
      uint count = nodes.size();
      for ( uint i = 0; i < count; ++i )
      {
         MaterialNode* node = nodes[i];
         node->detachNodeListener( this );
      }

      // and delete the layout that showed the graph
      m_graphView[routineIdx]->setScene( NULL );

      delete m_materialLayout[routineIdx];
      m_materialLayout[routineIdx] = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void MaterialEditor::setupTestScene()
{
   // add the preview scene template setting
   m_previewScene = new Model( FilePath() );

   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();

   FilePath previewBackgroundPath( "/Editor/MaterialInstanceEditor/Prefabs/background.tpf" );
   Prefab* previewBackgroundPrefab = resMgr.create< Prefab >( previewBackgroundPath, true );
   if ( previewBackgroundPrefab )
   {
      Entity* backgroundEntity = new Entity( "background" );
      backgroundEntity->setPrefab( previewBackgroundPrefab );
      m_previewScene->addChild( backgroundEntity );
   }

   // create an object we're gonna display the material on
   FilePath sphereMeshPath( "/Editor/MaterialInstanceEditor/Meshes/sphere_0.ttm" );
   TriangleMesh* sphereMesh = resMgr.create< TriangleMesh >( sphereMeshPath, true );
   if ( sphereMesh )
   {
      Entity* materialSphere = new Entity( "MaterialSphere" );
      GeometryComponent* materialSphereGeometry = new GeometryComponent( *sphereMesh, "SphereMesh" );
      materialSphereGeometry->setMaterial( &m_material );
      materialSphere->addChild( materialSphereGeometry );

      m_previewScene->addChild( materialSphere );
   }

   // load the debug scene
   m_sceneWidget->setScene( m_previewScene );
   m_sceneWidget->enableObjectsManipulator( false );

   // position the camera correctly
   Camera& camera = m_sceneWidget->getCamera();
   Matrix& localMtx = camera.accessLocalMtx();
   localMtx.setRows( 
      Vector( -0.613674f, -0.789559f, 0.0f, 0.0f ),
      Vector( -0.250264f, 0.194514f, 0.948437f, 0.0f ), 
      Vector( -0.748848f, 0.58203f, -0.316966f, 0.0f ), 
      Vector( 2.59875f, -2.14833f, 1.19996f, 1.0f ) );
}

///////////////////////////////////////////////////////////////////////////////

void MaterialEditor::save()
{
   m_material.saveResource();
}

///////////////////////////////////////////////////////////////////////////////

void MaterialEditor::buildMaterial()
{
   m_material.buildMaterial();

   // update the code preview
   onRoutineTabSelected( m_activeRoutineIdx );
}

///////////////////////////////////////////////////////////////////////////////

void MaterialEditor::onNodeCreated( MaterialNode* node )
{
   node->attachNodeListener( this );
}

///////////////////////////////////////////////////////////////////////////////

void MaterialEditor::onNodeRemoved( MaterialNode* node )
{
   node->detachNodeListener( this );
}

///////////////////////////////////////////////////////////////////////////////

void MaterialEditor::afterNodeRemoved( MaterialNode* node )
{
}

///////////////////////////////////////////////////////////////////////////////

void MaterialEditor::onConnectionCreated( const MaterialNode* startNode, const char* outputName, const MaterialNode* endNode, const char* inputName )
{
}

///////////////////////////////////////////////////////////////////////////////

void MaterialEditor::afterConnectionRemoved( const MaterialNode* endNode, const char* inputName )
{
}

///////////////////////////////////////////////////////////////////////////////

void MaterialEditor::onInputAdded( MaterialNode* node, GBNodeInput< MaterialNode >* input )
{ 
   // if the input was added to a routine output structure, propagate the change
   // to the bound input structures in the other shader stages
   if ( !node->isA< MNOutputStruct >() || m_synchronizationRootNode == node )
   {
      return;
   }

   m_synchronizationRootNode = node;

   MNOutputStruct* outputStruct = static_cast< MNOutputStruct* >( node );
   
   // we know that the only changes the user can make pertain to the active routine tab, 
   // so the node must be located there

   // synchronize it with the counterparts
   switch ( m_activeRoutineIdx )
   {
   case SHADER_VERTEX:
   {
      // vertex shader's output is bound with the fragment shader's input,
      // except for the first, mandatory output, that doesn't transfer its data to the vertex shader
      if ( outputStruct->getInputs().size() > 1 )
      {
         MNInputStruct* fsInput = m_routinesInputs[SHADER_FRAGMENT];
         ASSERT( fsInput );

         GBNodeOutput< MaterialNode >* output = input->createOutput( input->getName().c_str() );
         fsInput->defineOutput( output );
      }

      break;
   }

   // append other conditions here
   }

   m_synchronizationRootNode = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void MaterialEditor::onInputRemoved( MaterialNode* node, GBNodeInput< MaterialNode >* input )
{
   // if the input was added to a routine output structure, propagate the change
   // to the bound input structures in the other shader stages
   if ( !node->isA< MNOutputStruct >() || m_synchronizationRootNode == node )
   {
      return;
   }

   m_synchronizationRootNode = node;

   MNOutputStruct* outputStruct = static_cast< MNOutputStruct* >( node );

   // we know that the only changes the user can make pertain to the active routine tab, 
   // so the node must be located there
   std::vector< std::string > outputsToRemove;
   outputsToRemove.push_back( input->getName() );


   // synchronize it with the counterparts
   switch ( m_activeRoutineIdx )
   {
   case SHADER_VERTEX:
   {
      MNInputStruct* fsInput = m_routinesInputs[SHADER_FRAGMENT];
      ASSERT( fsInput );

      fsInput->removeSelectedOutputs( outputsToRemove );
      break;
   }

   // append other conditions here
   }

   m_synchronizationRootNode = NULL;
}


///////////////////////////////////////////////////////////////////////////////

void MaterialEditor::onOutputAdded( MaterialNode* node, GBNodeOutput< MaterialNode >* output )
{
   // if the output was added to a routine input structure, propagate the change
   // to the bound output structures in the other shader stages
   if ( !node->isA< MNInputStruct >() || m_synchronizationRootNode == node )
   {
      return;
   }

   m_synchronizationRootNode = node;

   MNInputStruct* inputStruct = static_cast< MNInputStruct* >( node );

   // we know that the only changes the user can make pertain to the active routine tab, 
   // so the node must be located there

   // synchronize it with the counterparts
   switch ( m_activeRoutineIdx )
   {
   case SHADER_FRAGMENT:
   {
      // fragment shader's input is bound with the vertex shader's output
      MNOutputStruct* vsOutput = m_routinesOutputs[SHADER_VERTEX];
      ASSERT( vsOutput );

      GBNodeInput< MaterialNode >* input = output->createInput( output->getName().c_str() );
      vsOutput->defineInput( input );
      
      break;
   }

   // append other conditions here
   }

   m_synchronizationRootNode = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void MaterialEditor::onOutputRemoved( MaterialNode* node, GBNodeOutput< MaterialNode >* output )
{
   // if the output was added to a routine input structure, propagate the change
   // to the bound output structures in the other shader stages
   if ( !node->isA< MNInputStruct >() || m_synchronizationRootNode == node )
   {
      return;
   }

   m_synchronizationRootNode = node;

   MNInputStruct* inputStruct = static_cast< MNInputStruct* >( node );

   // we know that the only changes the user can make pertain to the active routine tab, 
   // so the node must be located there
   std::vector< std::string > inputsToRemove;
   inputsToRemove.push_back( output->getName() );


   // synchronize it with the counterparts
   switch ( m_activeRoutineIdx )
   {
   case SHADER_FRAGMENT:
   {
      // vertex shader's output is bound with the fragment shader's input,
      // except for the first, mandatory output, that doesn't transfer its data to the vertex shader
      if ( inputStruct->getOutputs().size() > 1 )
      {
         MNOutputStruct* vsOutput = m_routinesOutputs[SHADER_VERTEX];
         ASSERT( vsOutput );

         vsOutput->removeSelectedInputs( inputsToRemove );
      }
      break;
   }

   // append other conditions here
   }

   m_synchronizationRootNode = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void MaterialEditor::onRoutineTabSelected( int tabIdx )
{
   m_activeRoutineIdx = tabIdx;

   if ( !m_codePreview )
   {
      return;
   }

   // update the code visible in the editor
   const MaterialProfile* profile = m_material.getProfile();
   const Shader* shader = profile->getShader();

   std::string code;
   shader->getScript( ( ShaderRoutine ) tabIdx, code );
   m_codePreview->setPlainText( code.c_str() );
}

///////////////////////////////////////////////////////////////////////////////

void MaterialEditor::exportToLibrary()
{   
   char tmpPath[512];
   sprintf_s( tmpPath, "%s%s.%s", g_routineLibraryDirs[m_activeRoutineIdx], g_routineLibraryDefaultNames[m_activeRoutineIdx], MaterialRoutine::getExtension() );

   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
   MaterialRoutine* routine = resMgr.create< MaterialRoutine >( FilePath( tmpPath ) );

   // set the graph in the library
   const MaterialProfile* activeProfile = m_material.getProfile();
   routine->grabSnapshot( activeProfile, m_activeRoutineIdx );

   routine->saveResource();
}

///////////////////////////////////////////////////////////////////////////////

void MaterialEditor::onLibraryDropped( const QMimeData* data )
{
   if ( m_droppedRoutineLib == NULL )
   {
      return;
   }

   const QPointF& currMouseScenePos = m_materialLayout[1]->getCurrentMousePos();
   const Vector localToWorld( currMouseScenePos.x(), currMouseScenePos.y(), 0 );

    // clear currently displayed routine and update it with the library's contents
   MaterialProfile* activeProfile = m_material.accessProfile();
   activeProfile->clearRoutine( (ShaderRoutine)m_activeRoutineIdx );
   m_droppedRoutineLib->pasteSnapshot( activeProfile, m_activeRoutineIdx, localToWorld );
}

///////////////////////////////////////////////////////////////////////////////

void MaterialEditor::showLibrariesBrowserPopupMenu( QTreeWidgetItem* selectedItem, QMenu& menu )
{
}

///////////////////////////////////////////////////////////////////////////////

void MaterialEditor::clearCurrentRoutine()
{
   MaterialProfile* activeProfile = m_material.accessProfile();
   activeProfile->clear();

   // create the missing entry points
   createProfileEntryPoints();
}

///////////////////////////////////////////////////////////////////////////////

void MaterialEditor::createProfileEntryPoints()
{
   // Go through the profile, identify all entry points.

   // do it for the 2 profiles used by the engine ATM - the VertexShader and the FragmentShader.
   // We're gonna add other shaders afterwards
   MaterialProfile* activeProfile = m_material.accessProfile();

   for ( uint i = 0; i < LAST_SHADER_ROUTINE; ++i )
   {
      ShaderRoutine routine = ( ShaderRoutine ) i;
      m_routinesInputs[i] = activeProfile->findNodeByType< MNInputStruct >( routine );
      m_routinesOutputs[i] = activeProfile->findNodeByType< MNOutputStruct >( routine );

      GraphBuilderTransaction< MaterialNode > transaction( *activeProfile->getGraph( routine ) );

      if ( !m_routinesInputs[i] )
      {
         // the input structure is missing - add it
         m_routinesInputs[i] = new MNInputStruct();
         MaterialLayout::createMandatorySockets( m_routinesInputs[i], i );

         transaction.addNode( m_routinesInputs[i] );
      }

      if ( !m_routinesOutputs[i] )
      {
         // the input structure is missing - add it
         m_routinesOutputs[i] = new MNOutputStruct();
         MaterialLayout::createMandatorySockets( m_routinesOutputs[i], i );

         transaction.addNode( m_routinesOutputs[i] );
      }

      transaction.commit();
   }
}

///////////////////////////////////////////////////////////////////////////////

#include "ProceduralLevelGenerator.h"
#include "core-MVC\Prefab.h"
#include "core-MVC\Entity.h"
#include "core-MVC\EntityUtils.h"
#include <QtWidgets\QVBoxLayout>
#include <QtWidgets\QHBoxLayout>
#include <QtWidgets\QToolBar>
#include <QtWidgets\QLabel>
#include <QtWidgets\QAction>
#include <QtGui\QIcon>
#include "QPropertiesView.h"

#include "core-Renderer\GeometryComponent.h"
#include "core-Renderer\RenderSystem.h"

// level voxelization
#include "ext-2DGameLevel\GL2DLevelGenerator.h"
#include "ext-2DGameLevel\GL2DVoxelPrefabsMap.h"
#include "ext-2DGameLevel\GL2DProceduralLevel.h"
#include "ext-2DGameLevel\GL2DLevelGenUtils.h"
#include "ext-2DGameLevel\GL2DRoom.h"
#include "ext-2DGameLevel\GL2DCorridor.h"
#include "ext-2DGameLevel\GL2DRoomLayout.h"


///////////////////////////////////////////////////////////////////////////////

ProceduralLevelGenerator::ProceduralLevelGenerator( QWidget* parent, GL2DProceduralLevel* params )
   : QFrame( parent )
   , m_params( params )
   , m_root( params->getParent() )
{
   QVBoxLayout* layout = new QVBoxLayout( this );
   setLayout( layout );

   QToolBar* toolbar = new QToolBar( this );
   {
      layout->addWidget( toolbar );

      QAction* generateLevelAction = toolbar->addAction( QIcon( ":/TamyEditor/Resources/Icons/Editor/play.png" ), "Generate level" );
      connect( generateLevelAction, SIGNAL( triggered() ), this, SLOT( generateScene() ) );
   }
   
   QFrame* statusBar = new QFrame( this );
   {
      layout->addWidget( statusBar );

      QHBoxLayout* statusBarLayout = new QHBoxLayout( statusBar );
      statusBarLayout->setSpacing( 0 );
      statusBarLayout->setMargin( 0 );
      statusBar->setLayout( statusBarLayout );

      QLabel* statusLabel = new QLabel( "Status: ", this );
      statusLabel->setMaximumWidth( 100 );
      statusBarLayout->addWidget( statusLabel );

      m_status = new QLabel( this );
      statusBarLayout->addWidget( m_status );
   }
}

///////////////////////////////////////////////////////////////////////////////

ProceduralLevelGenerator::~ProceduralLevelGenerator()
{
}

///////////////////////////////////////////////////////////////////////////////

void ProceduralLevelGenerator::generateScene()
{
   GL2DVoxelPrefabsMap prefabsMap( m_params->m_geometryVoxelsDir );
   GL2DLevelGenerator generator( prefabsMap, *m_params );

   // create custom rooms
   {
      GL2DRoomTheme* labTheme = prefabsMap.findTheme( "Lab" );
      GL2DRoomTheme* warehouseTheme = prefabsMap.findTheme( "Warehouse" );

      FilePath tubesPath( "/Projects/SaveHer/LevelGeometry/BlenderAssets/SciFiLevelGeometry/Prefabs/Lab/Furniture/PlasmaTubes.tpf" );
      FilePath operatingTablePath( "/Projects/SaveHer/LevelGeometry/BlenderAssets/SciFiLevelGeometry/Prefabs/Lab/Furniture/OperatingTable.tpf" );
      FilePath cupboardPath( "/Projects/SaveHer/LevelGeometry/BlenderAssets/SciFiLevelGeometry/Prefabs/Lab/Furniture/Shelves_1.tpf" );

      Prefab* tubesPrefab = TSingleton< ResourcesManager >::getInstance().create< Prefab >( tubesPath );
      Prefab* operatingTablePrefab = TSingleton< ResourcesManager >::getInstance().create< Prefab >( operatingTablePath );
      Prefab* cupboardPrefab = TSingleton< ResourcesManager >::getInstance().create< Prefab >( cupboardPath );

      GL2DRoomLayoutLeaf* livingQuaters = new GL2DRoomLayoutLeaf( warehouseTheme, GL2D_Room_Small );


      GL2DRoomLayoutComposite* layout = new GL2DRoomLayoutComposite( warehouseTheme );
      layout->addLayout( livingQuaters );

      for ( byte i = 0; i < 3; ++i )
      {
         GL2DRoomLayout* childLayout = new GL2DRoomLayoutLeaf( warehouseTheme, GL2D_Room_Small );
         layout->addLayout( childLayout );
      }
      generator.addLayout( layout );

      GL2DRoom* startRoom = livingQuaters->getRoom( 0 );
      startRoom->addFurniture( tubesPrefab ); // some required furniture
      startRoom->addFurniture( operatingTablePrefab ); // some required furniture

      GL2DRoom* endRoom = livingQuaters->getRoom( 4 );
      endRoom->addFurniture( tubesPrefab ); // some required furniture
      endRoom->addFurniture( cupboardPrefab ); // some required furniture
   }

   Entity* proceduralGeometry = generator.renderLevel();

   m_root->addChild( proceduralGeometry );
}

///////////////////////////////////////////////////////////////////////////////

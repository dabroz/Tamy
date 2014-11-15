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
#include "ext-2DGameLevel\GL2DProceduralLevel.h"



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
   GL2DLevelGenerator generator( m_params->m_backgroundPrefabsDir, m_params->m_geometryDir );

   Entity* proceduralGeometry = generator.generateLevel( m_params->m_levelLenght );
   if ( proceduralGeometry )
   {
      m_root->addChild( proceduralGeometry );
   }
}

///////////////////////////////////////////////////////////////////////////////

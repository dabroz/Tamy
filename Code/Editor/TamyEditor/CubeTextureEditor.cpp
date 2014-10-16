#include "CubeTextureEditor.h"
#include "TamyEditor.h"
#include "core.h"
#include "FSNodeMimeData.h"
#include "DropArea.h"
#include <QtWidgets\QVBoxLayout>
#include <QtWidgets\QHBoxLayout>
#include <QtWidgets\QGridLayout>
#include <QtWidgets\QToolBar>
#include <QtWidgets\QAction>
#include <QtGui\QPixmap>
#include <QtGui\QImage>
#include "QPropertiesView.h"
#include <QtWidgets\QFormLayout>
#include <QtWidgets\QGroupBox>


///////////////////////////////////////////////////////////////////////////////

CubeTextureEditor::CubeTextureEditor( CubeTexture& texture )
   : m_texture( texture )
{
   memset( m_image, NULL, sizeof( DropArea* ) * 6 );
}

///////////////////////////////////////////////////////////////////////////////

CubeTextureEditor::~CubeTextureEditor()
{
}

///////////////////////////////////////////////////////////////////////////////

void CubeTextureEditor::onInitialize()
{
   TamyEditor& mainEditor = TamyEditor::getInstance();


   // setup the main layout
   QVBoxLayout* mainLayout = new QVBoxLayout( this );
   mainLayout->setContentsMargins( 0, 0, 0, 0 );
   setLayout( mainLayout );

   // add the toolbar
   {
      QToolBar* toolBar = new QToolBar( this );
      mainLayout->addWidget( toolBar );

      QAction* actionSaveScene = new QAction( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/saveFile.png" ) ), tr( "Save Scene" ), toolBar );
      toolBar->addAction( actionSaveScene );
      connect( actionSaveScene, SIGNAL( triggered() ), this, SLOT( saveTexture() ) );
   }

   // edit frame
   {
      QFrame* editFrame = new QFrame( this );
      QHBoxLayout* editFrameLayout = new QHBoxLayout( editFrame );
      editFrame->setLayout( editFrameLayout );
      mainLayout->addWidget( editFrame );

      // side panel for properties
      {
         QFrame* propertiesFrame = new QFrame( editFrame );
         QVBoxLayout* propertiesLayout = new QVBoxLayout( propertiesFrame );
         propertiesFrame->setLayout( propertiesLayout );
         editFrameLayout->addWidget( propertiesFrame, 0 );

         // properties viewer
         {
            QPropertiesView* propertiesView = new QPropertiesView();
            propertiesLayout->addWidget( propertiesView, 1 );
            m_texture.viewProperties( *propertiesView );
         }

         // image info box
         {
            QGroupBox* infoFrame = new QGroupBox( "Info", propertiesFrame );
            QFormLayout* infoLayout = new QFormLayout( infoFrame );
            infoFrame->setLayout( infoLayout );
            propertiesLayout->addWidget( infoFrame, 1 );

            infoLayout->addRow( new QLabel( "Edge length:", infoFrame ), m_edgeLengthInfo = new QLabel( "0", infoFrame ) );
            infoLayout->addRow( new QLabel( "Depth:", infoFrame ), m_imageDepthInfo = new QLabel( "0 bits", infoFrame ) );
         }

      }

      // add the image viewer widget
      {
         QFrame* previewsFrame = new QFrame( editFrame );
         QGridLayout* previewsFrameLayout = new QGridLayout( previewsFrame );
         previewsFrameLayout->setSpacing( 2 );
         previewsFrame->setLayout( previewsFrameLayout );
         editFrameLayout->addWidget( previewsFrame, 1 );

         // create 6 preview areas
         for ( int i = 0; i < 6; ++i )
         {
            m_image[i] = new DropArea( editFrame, new FSNodeMimeData( m_imagePaths ) );
            m_image[i]->setBackgroundRole( QPalette::Base );
            m_image[i]->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
            m_image[i]->setScaledContents( true );
         }

         // form the cube map cross with the preview areas
         previewsFrameLayout->addWidget( m_image[CubeTexture::Cube_Top], 0, 1 );
         previewsFrameLayout->addWidget( m_image[CubeTexture::Cube_Left], 1, 0 );
         previewsFrameLayout->addWidget( m_image[CubeTexture::Cube_Front], 1, 1 );
         previewsFrameLayout->addWidget( m_image[CubeTexture::Cube_Right], 1, 2 );
         previewsFrameLayout->addWidget( m_image[CubeTexture::Cube_Rear], 1, 3 );
         previewsFrameLayout->addWidget( m_image[CubeTexture::Cube_Bottom], 2, 1 );

         // connect signals
         connect( m_image[CubeTexture::Cube_Top], SIGNAL( changed() ), this, SLOT( onTopTextureSet() ) );
         connect( m_image[CubeTexture::Cube_Left], SIGNAL( changed() ), this, SLOT( onLeftTextureSet() ) );
         connect( m_image[CubeTexture::Cube_Front], SIGNAL( changed() ), this, SLOT( onFrontTextureSet() ) );
         connect( m_image[CubeTexture::Cube_Right], SIGNAL( changed() ), this, SLOT( onRightTextureSet() ) );
         connect( m_image[CubeTexture::Cube_Rear], SIGNAL( changed() ), this, SLOT( onBackTextureSet() ) );
         connect( m_image[CubeTexture::Cube_Bottom], SIGNAL( changed() ), this, SLOT( onBottomTextureSet() ) );

      }
   }

   // initialize the contents
   refreshImage();
}

///////////////////////////////////////////////////////////////////////////////

void CubeTextureEditor::onDeinitialize( bool saveProgress )
{
   if ( saveProgress )
   {
      m_texture.saveResource();
   }

   memset( m_image, NULL, sizeof( DropArea* ) * 6 );
}

///////////////////////////////////////////////////////////////////////////////

void CubeTextureEditor::refreshImage()
{
   const Filesystem& fs = TSingleton< Filesystem >::getInstance();
   const char* areaName [] = { "Left", "Right", "Top", "Bottom", "Front", "Back" };

   for ( uint i = 0; i < 6; ++i )
   {
      std::string absolutePath = fs.toAbsolutePath( m_texture.getImagePath( (CubeTexture::Side)i ) );
      QImage image( absolutePath.c_str() );
      if ( image.isNull() )
      {
         m_image[i]->setText( areaName[i] );
      }
      else
      {
         m_image[i]->setPixmap( QPixmap::fromImage( image ) );

         // set info about the image
         char tmpStr[32];
         sprintf( tmpStr, "%d", image.width() );
         m_edgeLengthInfo->setText( tmpStr );
         sprintf( tmpStr, "%d bits", image.depth() );
         m_imageDepthInfo->setText( tmpStr );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void CubeTextureEditor::onTopTextureSet()
{
   onTextureSet( CubeTexture::Cube_Top );
}

///////////////////////////////////////////////////////////////////////////////

void CubeTextureEditor::onLeftTextureSet()
{
   onTextureSet( CubeTexture::Cube_Left );
}

///////////////////////////////////////////////////////////////////////////////

void CubeTextureEditor::onFrontTextureSet()
{
   onTextureSet( CubeTexture::Cube_Front );
}

///////////////////////////////////////////////////////////////////////////////

void CubeTextureEditor::onRightTextureSet()
{
   onTextureSet( CubeTexture::Cube_Right );
}

///////////////////////////////////////////////////////////////////////////////

void CubeTextureEditor::onBackTextureSet()
{
   onTextureSet( CubeTexture::Cube_Rear );
}

///////////////////////////////////////////////////////////////////////////////

void CubeTextureEditor::onBottomTextureSet()
{
   onTextureSet( CubeTexture::Cube_Bottom );
}

///////////////////////////////////////////////////////////////////////////////

void CubeTextureEditor::onTextureSet( CubeTexture::Side side )
{
   const Filesystem& fs = TSingleton< ResourcesManager >::getInstance().getFilesystem();

   unsigned int count = m_imagePaths.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      std::string absolutePath = fs.toAbsolutePath( m_imagePaths[i] );
      QImage image( absolutePath.c_str() );
      if ( image.isNull() == false )
      {
         m_texture.setImagePath( side, m_imagePaths[i] );
         break;
      }
   }
   m_imagePaths.clear();

   refreshImage();
}

///////////////////////////////////////////////////////////////////////////////

void CubeTextureEditor::saveTexture()
{
   m_texture.saveResource();
}

///////////////////////////////////////////////////////////////////////////////

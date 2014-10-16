#include "TextureEditor.h"
#include "TamyEditor.h"
#include "core.h"
#include "core-Renderer/Texture.h"
#include "FSNodeMimeData.h"
#include "DropArea.h"
#include <QtWidgets\QVBoxLayout>
#include <QtWidgets\QHBoxLayout>
#include <QtWidgets\QToolBar>
#include <QtWidgets\QAction>
#include <QtGui\QPixmap>
#include <QtGui\QImage>
#include "QPropertiesView.h"
#include <QtWidgets\QFormLayout>
#include <QtWidgets\QGroupBox>


///////////////////////////////////////////////////////////////////////////////

TextureEditor::TextureEditor( Texture& texture )
   : m_texture( texture )
   , m_image( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

TextureEditor::~TextureEditor()
{
}

///////////////////////////////////////////////////////////////////////////////

void TextureEditor::onInitialize()
{
   TamyEditor& mainEditor = TamyEditor::getInstance();


   // setup the main layout
   QVBoxLayout* mainLayout = new QVBoxLayout( this );
   mainLayout->setContentsMargins(0, 0, 0, 0);
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

            infoLayout->addRow( new QLabel( "Width:", infoFrame ), m_imageWidthInfo = new QLabel( "0", infoFrame ) );
            infoLayout->addRow( new QLabel( "Height:", infoFrame ), m_imageHeightInfo = new QLabel( "0", infoFrame ) );
            infoLayout->addRow( new QLabel( "Depth:", infoFrame ), m_imageDepthInfo = new QLabel( "0 bits", infoFrame ) );
         }

      }

      // add the image viewer widget
      {
         m_image = new DropArea( editFrame, new FSNodeMimeData( m_imagePaths ) );
         editFrameLayout->addWidget( m_image, 1 );
         m_image->setBackgroundRole( QPalette::Base );
         m_image->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
         m_image->setScaledContents( true );

         connect( m_image, SIGNAL( changed() ), this, SLOT( onTextureSet() ) );
      }
   }

   // initialize the contents
   refreshImage();
}

///////////////////////////////////////////////////////////////////////////////

void TextureEditor::onDeinitialize( bool saveProgress )
{
   if ( saveProgress )
   {
      m_texture.saveResource();
   }

   m_image = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void TextureEditor::refreshImage()
{
   const Filesystem& fs = TSingleton< ResourcesManager >::getInstance().getFilesystem();
   std::string absolutePath = fs.toAbsolutePath( m_texture.getImagePath() );

   QImage image( absolutePath.c_str() );
   m_image->setPixmap( QPixmap::fromImage( image ) );

   // set info about the image
   char tmpStr[32];
   sprintf( tmpStr, "%d", image.width() );
   m_imageWidthInfo->setText( tmpStr );
   sprintf( tmpStr, "%d", image.height() );
   m_imageHeightInfo->setText( tmpStr );
   sprintf( tmpStr, "%d bits", image.depth() );
   m_imageDepthInfo->setText( tmpStr );
}

///////////////////////////////////////////////////////////////////////////////

void TextureEditor::onTextureSet()
{
   const Filesystem& fs = TSingleton< ResourcesManager >::getInstance().getFilesystem();

   unsigned int count = m_imagePaths.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      std::string absolutePath = fs.toAbsolutePath( m_imagePaths[i] );
      QImage image( absolutePath.c_str() );
      if ( image.isNull() == false )
      {
         m_texture.setImagePath( m_imagePaths[i] );
         break;
      }
   }
   m_imagePaths.clear();

   refreshImage();
}

///////////////////////////////////////////////////////////////////////////////

void TextureEditor::saveTexture()
{
   m_texture.saveResource();
}

///////////////////////////////////////////////////////////////////////////////

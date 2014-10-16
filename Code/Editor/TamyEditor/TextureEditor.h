/// @file   TamyEditor/TextureEditor.h
/// @brief  texture resource editor
#pragma once

#include "ResourceEditor.h"
#include "core\Array.h"
#include "core\FilePath.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

class Texture;
class DropArea;
class QLabel;

///////////////////////////////////////////////////////////////////////////////

class TextureEditor : public ResourceEditor
{
   Q_OBJECT 

private:
   Texture&                      m_texture;

   DropArea*                     m_image;
   Array< FilePath >             m_imagePaths;

   QLabel*                       m_imageWidthInfo;
   QLabel*                       m_imageHeightInfo;
   QLabel*                       m_imageDepthInfo;

public:
   /**
    * Constructor.
    *
    * @param texture      texture to be edited
    */
   TextureEditor( Texture& texture );
   ~TextureEditor();

public slots:
   void onTextureSet();
   void saveTexture();

protected:
   // -------------------------------------------------------------------------
   // ResourceEditor implementation
   // -------------------------------------------------------------------------
   void onInitialize();
   void onDeinitialize( bool saveProgress );

private:
   void refreshImage();
};

///////////////////////////////////////////////////////////////////////////////

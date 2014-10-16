/// @file   TamyEditor/CubeTextureEditor.h
/// @brief  cube texture resource editor
#pragma once

#include "ResourceEditor.h"
#include "core\Array.h"
#include "core\FilePath.h"
#include <string>
#include "core-Renderer\CubeTexture.h"

///////////////////////////////////////////////////////////////////////////////

class CubeTexture;
class DropArea;
class QLabel;

///////////////////////////////////////////////////////////////////////////////

class CubeTextureEditor : public ResourceEditor
{
   Q_OBJECT

private:
   CubeTexture&                  m_texture;

   DropArea*                     m_image[6];
   Array< FilePath >             m_imagePaths;

   QLabel*                       m_edgeLengthInfo;
   QLabel*                       m_imageDepthInfo;

public:
   /**
   * Constructor.
   *
   * @param texture      cube texture to be edited
   */
   CubeTextureEditor( CubeTexture& texture );
   ~CubeTextureEditor();

public slots:
   void saveTexture();
   void onTopTextureSet();
   void onLeftTextureSet();
   void onFrontTextureSet();
   void onRightTextureSet();
   void onBackTextureSet();
   void onBottomTextureSet();

protected:
   // -------------------------------------------------------------------------
   // ResourceEditor implementation
   // -------------------------------------------------------------------------
   void onInitialize();
   void onDeinitialize( bool saveProgress );

private:
   void refreshImage();
   void onTextureSet( CubeTexture::Side side );
};

///////////////////////////////////////////////////////////////////////////////
